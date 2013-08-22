#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include "curl/curl.h"
#include "Wifi/WifiManager.h"
#include "dkWinError.h"
#include "Utility/ThreadHelper.h"
#include "Utility/JsonObject.h"
#include "drivers/DeviceInfo.h"
#include "Common/SystemSetting_DK.h"
#include "Common/SystemOperation.h"
#include "BookStore/DownloadTaskBuilder.h"
#include "DownloadManager/DownloadManager.h"
#include "SQMUploader.h"
#include "SQM.h"

#define SLEEP_AFTER_CONNECT  300

#define SQM_COUNTER_FILE            "/mnt/us/DK_System/xKindle/SQMData/counter.log"
#define SQM_POST_DATA_HEADER_1      "Expect:"
#define SQM_POST_DATA_HEADER_2      "Content-Type: multipart/form-data; boundary=----------------------------b752"

extern sem_t SQMCounterSem;

void SQMUploader::StartUploadSQMDataThread()
{

    DebugPrintf(DLC_CHENM, "StartUploadSQMDataThread!!!!!!");

    //if (CheckTimeShift()) 不用检查时间，每次连接都上传sqm 数据。
    {
        DebugPrintf(DLC_CHENM, "CheckTimeShift ### TRUE !!!!!!");    
        pthread_t pthid;
        ThreadHelper::CreateThread(&pthid, SQMUploadThread,NULL, "SQMUploader @ SQMUploadThread" , true, 51200, SQM_THREAD_PRIORITY);
    }
}

bool  SQMUploader::CheckTimeShift()
{
    static bool first_get_clock = true;
    static struct timeval _upload_time_stamp;

    if (first_get_clock)
    {
        if (0 != gettimeofday(&_upload_time_stamp, NULL))
            return false;
        first_get_clock = false;
    }

    struct timeval _now;
    if (0 != gettimeofday(&_now, NULL))
            return false;

    int _duration_seconds = (_now.tv_sec - _upload_time_stamp.tv_sec) + (_now.tv_usec - _upload_time_stamp.tv_usec) / 1000000;

    DebugPrintf(DLC_CHENM, "CheckTimeShift _duration_seconds = %d !!!!!!", _duration_seconds);    

    if (_duration_seconds > 86400) // 24 * 60 * 60
    {   
        _upload_time_stamp = _now;
        return true;
    }

    return false;
}

string SQMUploader::GetDataNeedToPost()
{
	string strResult = "";
    int fExists = access(SQM_COUNTER_FILE, 0) == 0;
    if(fExists)
    {
        FILE *fp = fopen(SQM_COUNTER_FILE, "r+b");
        if(fp)
    	{
    		char buff[COUNTER_SIZE * SIZE_OF_INT] = {0};
	        unsigned int* arr;
	        int size = fread(buff,1,COUNTER_SIZE * SIZE_OF_INT,fp);
	        if(size !=0)
	        {
	        	JsonObjectSPtr jsonData = JsonObject::CreateJsonObject();
	        	int countData[COUNTER_SIZE] = {0};
	            arr = (unsigned int *)buff;
	            for(int i=0;i<COUNTER_SIZE;i++)
	            {
	                countData[i] += arr[i];
					if(countData[i] != 0)
					{
						char content[10] = {0};
						snprintf(content, DK_DIM(content), "%d", i);
						jsonData->AddIntValue(content, countData[i]);
					}
	            }
				strResult = jsonData->GetJsonString();
	        }
			fclose(fp);
    	}
    }
	return strResult;
}


void* SQMUploader::SQMUploadThread(void* pV)
{
    DebugPrintf(DLC_CHENM, "SQMUploadThread @@ !!!!!!");     
    
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype (PTHREAD_CANCEL_DEFERRED, NULL);

    //sleep(SLEEP_AFTER_CONNECT); //等待5分钟 应该是每天的第一连接WIFI 5分钟后上传SQM

    if (WifiManager::GetInstance()->IsConnected())
    {
        DebugPrintf(DLC_CHENM, "SQMUploadThread@@ IsConnected() = TRUE !!!!!!");     
        PostDataToServer();
    }
    
    return NULL;
}

void SQMUploader::PostDataToServer()
{
	string postData = GetDataNeedToPost();
	if(!postData.empty())
	{
		dk::DownloadTaskSPtr task = dk::bookstore::DownloadTaskBuilder::BuildPostSQMTask(postData.c_str());
	    dk::DownloadManager::GetInstance()->RunTask(task);
		if(task)
		{
			string strResult = task->GetString();
			JsonObjectSPtr jsonObj = JsonObject::CreateFromString(strResult.c_str());
	        if (jsonObj)
	        {
	        	int code = -1;
	            if (jsonObj->GetIntValue("code", &code) && code == 0)
	            {
	            	unlink(SQM_COUNTER_FILE);
					return;
	            }
	        }
		}
	}
}

