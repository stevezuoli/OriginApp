#include <pthread.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include "curl/curl.h"
#include "interface.h"
#include "Common/FileManager_DK.h"
#include "Common/File_DK.h"
#include "DownloadManager/IDownloadTask.h"
#include "DownloadManager/DownloadEngineImpl.h"
#include "DownloadManager/curlDownload.h"
#include "DownloadManager/DownloaderImpl.h"
#include "Utility.h"
#include "Utility/FileSystem.h"
#include "Utility/ThreadHelper.h"
#include "I18n/StringManager.h"

using namespace std;
using namespace DkFormat;
#define FILENAME_EXTERSION_ASP ".asp"
#define FILENAME_EXTERSION_JSP ".jsp"
#define FILENAME_EXTERSION_HTML ".html"
#define FILENAME_EXTERSION_HTM ".htm"
#define FILENAME_EXTERSION_PHP ".php"

void SingleThreadDownloadEngineImpl::Start()
{
    if (!m_taskworking)
    {
        this->SetTaskWorking(true);
        ThreadHelper::CreateThread(&m_taskthread,
            SingleThreadDownloadEngineImpl::TaskThread,
            m_pdownloadtask,
            "SingleThreadDownloadEngineImpl @ TaskThread",
            false,
            THREAD_STACK_SIZE_KINDLE_DEFAULT);
    }
}
void SingleThreadDownloadEngineImpl::Stop()
{
    if (m_taskworking)
    {
        this->SetTaskWorking(false);
        ThreadHelper::JoinThread(m_taskthread,NULL);
    }
}

int SingleThreadDownloadEngineImpl::GetFileInfoFromServer(string _url,void* _pin)
{
    if(NULL == _pin)
    {
        return Download::DOWNLOAD_INVALID_ARGUMENT;
    }
    DebugPrintf(DLC_XU_KAI,"URL IS %s",_url.c_str());
    Download::UrlFileInfo*_pinfo = (Download::UrlFileInfo*)_pin;
    Download::DLResult res = Download::getFileInfoFromWeb(_url,_pinfo);
    if(res == Download::DOWNLOAD_OK || res == Download::DOWNLOAD_WRITE_ERROR)// res == Download::DOWNLOAD_WRITE_ERROR代表写文件时被中断
    {
        if(strlen(_pinfo->szFileName))
        {
            const char *sztmp = strrchr(_pinfo->szFileName,'.');
            if(sztmp &&  strcasecmp(sztmp,FILENAME_EXTERSION_ASP)
                && strcasecmp(sztmp,FILENAME_EXTERSION_JSP)
                && strcasecmp(sztmp,FILENAME_EXTERSION_HTML)
                && strcasecmp(sztmp,FILENAME_EXTERSION_HTM)
                && strcasecmp(sztmp,FILENAME_EXTERSION_PHP))
            {
                strcpy(_pinfo->szFileName,sztmp);
            }
            else
            {
                _pinfo->szFileName[0] = 0;
            }

        }

    }
    return res;
}

std::string SingleThreadDownloadEngineImpl::GetExtensionNameAndScreenWebName(std::string _strFileName)
{
	if(_strFileName.empty())
	{
		return "";
	}
	const char *pTmp = strrchr(_strFileName.c_str(),'.');
	if(pTmp &&  strcasecmp(pTmp,FILENAME_EXTERSION_ASP)
		&& strcasecmp(pTmp,FILENAME_EXTERSION_JSP)
		&& strcasecmp(pTmp,FILENAME_EXTERSION_HTML)
		&& strcasecmp(pTmp,FILENAME_EXTERSION_HTM)
		&& strcasecmp(pTmp,FILENAME_EXTERSION_PHP))
	{
		 return pTmp;
	}

	return "";
}

void* SingleThreadDownloadEngineImpl::TaskThread(void* pV)
{
    if(pV == NULL)
    {
        return NULL;
    }
    IDownloadTask *ptask = (IDownloadTask *)pV;
    //DebugPrintf(DLC_DIAGNOSTIC,"SingleThreadDownloadEngineImpl::TaskThread begin : %s",ptask->GetFileName().c_str());
    string _filename = ptask->GetFileName();
    string url = ptask->GetUrl().length() > 0? ptask->GetUrl() : ptask->GetOrigUrl();
    bool resumeFlag = ptask->CanResume();
    string filename = ptask->GetFileName();
	Download::UrlFileInfo info = {0,{0},{0},0,{0},{0}};
	info.bSupportBreakpointContinuingly = resumeFlag;
	snprintf(info.szFileName,MAX_NAME_LENGTH,"%s",filename.c_str());
	snprintf(info.szUesrAgent,RANGE_BUF_LENGTH,"%s",ptask->GetUserAgent().c_str());
    
    DebugPrintf(DLC_DIAGNOSTIC,"SingleThreadDownloadEngineImpl::TaskThread begin To Download::download2Path(url, &info, DOWNLOADPATH,&taskState)");
	Download::DLResult res= Download::download2Path(url, &info, DOWNLOADPATH, ptask);
    DebugPrintf(DLC_DIAGNOSTIC,"SingleThreadDownloadEngineImpl::TaskThread end Download::download2Path(url, &info, DOWNLOADPATH,&taskState)");

    DebugPrintf(DLC_XU_KAI,"Download::download2path return res is %d",res);
    if (res == Download::DOWNLOAD_OK)
    {
        ptask->SetLastOffSet(ptask->GetFileSize());
        string strNewFullPath(DOWNLOADPATH);
        string strOldFileName = ptask->GetFileName();
		string strOldFullPath = strNewFullPath + strOldFileName + ".dat";
		if(strlen(info.szFileName))
		{
			// 通近网络返回文件名更新文件的后缀名
			string strExtensionName = GetExtensionNameAndScreenWebName(info.szFileName);
			size_t iPos = strOldFileName.find_last_of('.');
			if(iPos != string::npos && !strExtensionName.empty())
			{
				strOldFileName.replace(iPos,strOldFileName.length() - iPos,strExtensionName);
			}
		}
        strNewFullPath += strOldFileName;
		strNewFullPath = CheckForSameFilename(strNewFullPath);
		int pos = strlen(DOWNLOADPATH);
		string strNewFileName = strNewFullPath.substr(pos);
		ptask->SetFileName(strNewFileName);

        rename(strOldFullPath.c_str(),strNewFullPath.c_str());
		sync();
		if(ptask->GetType() == IDownloadTask::BOOK)
		{
			DkFileFormat eFormat = GetFileFormatbyExtName(strNewFileName.c_str());
			if(eFormat == DFF_ZipFile || eFormat == DFF_RoshalArchive)
			{
				size_t iPos = strNewFullPath.find_last_of('.');
				if(iPos != string::npos)
				{
					string strOutputPath = strNewFullPath.substr(0,iPos);
					strOutputPath += '/';
					DebugPrintf(DLC_XU_KAI,"OutputPath is : %s",strOutputPath.c_str());
					CUtility::UnpackArchive(strNewFullPath.c_str(),strOutputPath.c_str(),ptask->GetFilePassword().c_str());
				}
			}
		}

		ptask->SetState(IDownloadTask::CURL_DONE);
    }
    else if(res == Download::DOWNLOAD_ABORTED)
    {
        ptask->SetState(IDownloadTask::CANCELED);
        ptask->FireDownloadProgressUpdateEvent();
    }
    else
    {
        DebugPrintf(DLC_DIAGNOSTIC, "download faild");
        ptask->SetState(IDownloadTask::FAILED);
        ptask->FireDownloadProgressUpdateEvent();
    }
    return NULL;
}


