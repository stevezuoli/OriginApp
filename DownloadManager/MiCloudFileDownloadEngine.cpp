#include <pthread.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include "curl/curl.h"
#include "interface.h"
#include "Common/FileManager_DK.h"
#include "Common/File_DK.h"
#include "DownloadManager/IDownloadTask.h"
#include "DownloadManager/MiCloudFileDownloadEngine.h"
#include "DownloadManager/DownloadMiCloudFileTask.h"
#include "DownloadManager/DownloaderImpl.h"
#include "Utility.h"
#include "Utility/FileSystem.h"
#include "Utility/ThreadHelper.h"
#include "I18n/StringManager.h"
#include "Thirdparties/KSSManager/DKKSSManager.h"
#include "XiaoMi/XiaoMiServiceFactory.h"
using namespace std;
using namespace DkFormat;

void MiCloudFileDownloadEngine::Start()
{
    if (!m_taskworking)
    {
        this->SetTaskWorking(true);
        ThreadHelper::CreateThread(&m_taskthread,
            MiCloudFileDownloadEngine::TaskThread,
            m_pdownloadtask,
            "MiCloudFileDownloadEngine @ TaskThread",
            false,
            THREAD_STACK_SIZE_KINDLE_DEFAULT);
    }
}
void MiCloudFileDownloadEngine::Stop()
{
    if (m_taskworking)
    {
        this->SetTaskWorking(false);
        ThreadHelper::CancelThread(m_taskthread);
        ThreadHelper::JoinThread(m_taskthread,NULL);
    }
}

int MiCloudFileDownloadEngine::GetFileInfoFromServer(string _url,void* _pin)
{
    return -1;
}

void* MiCloudFileDownloadEngine::TaskThread(void* pV)
{
    DownloadMiCloudFileTask *ptask = static_cast<DownloadMiCloudFileTask*>(pV);
    if (ptask)
    {
        if (ptask->IsUploadTask())
        {
            string fileMeta;
            vector<string> commitMetas;
            ErrorNo res = DKKSSManager::GetInstance()->Upload(ptask->GetKSSInfo(),
                    ptask->GetMoveTo(),
                    fastdelegate::MakeDelegate(ptask, &DownloadMiCloudFileTask::OnKSSProgressCallback),
                    fileMeta,
                    commitMetas);
            //DebugPrintf(DLC_DIAGNOSTIC, "MiCloudFileDownloadEngine kss upload %d %s", res, fileMeta.c_str());
            if (res == KSS_OK)
            {
                xiaomi::XiaoMiServiceFactory::GetMiCloudService()->CommitFile(ptask->GetMoveTo(), 
                    ptask->GetUploadId(), fileMeta, commitMetas);
                return NULL;
            }
            else if (res == KSS_FAIL)
            {
                DebugPrintf(DLC_DIAGNOSTIC, "MiCloudFileDownloadEngine kss upload failed");
                ptask->SetState(IDownloadTask::FAILED);
                ptask->FireDownloadProgressUpdateEvent();
            }
            else
            {
                DebugPrintf(DLC_DIAGNOSTIC, "MiCloudFileDownloadEngine kss upload error!");
                ptask->SetState(IDownloadTask::CANCELED);
                ptask->FireDownloadProgressUpdateEvent();
            }
        }
        else
        {
            ErrorNo res = DKKSSManager::GetInstance()->Download(ptask->GetKSSInfo(),
                    ptask->GetMoveTo(), 
                    fastdelegate::MakeDelegate(ptask, &DownloadMiCloudFileTask::OnKSSProgressCallback));
            if (res == KSS_OK)
            {
                ptask->SetState(IDownloadTask::CURL_DONE);
            }
            else if (res == KSS_FAIL)
            {
                ptask->SetState(IDownloadTask::FAILED);
                ptask->FireDownloadProgressUpdateEvent();
            }
            else
            {
                ptask->SetState(IDownloadTask::CANCELED);
                ptask->FireDownloadProgressUpdateEvent();
            }
        }
    }
    return NULL;
}

