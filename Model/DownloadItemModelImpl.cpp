////////////////////////////////////////////////////////////////////////
// CBookItemModelImpl.cpp
// Contact: zhangxb
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "Model/DownloadItemModelImpl.h"
#include "Common/FileManager_DK.h"

const std::string  CDownloadItemModelImpl::GetBookName()
{
    IDownloader *_pDownloadManager = DownloaderImpl::GetInstance();
    if(_pDownloadManager)
    {
        return _pDownloadManager->GetTaskFileNameByUrlId(m_TaskUrl);
    }
    return "";
}


IDownloadTask::DLState CDownloadItemModelImpl::GetDownloadStatus()
{
    IDownloader *_pDownloadManager = DownloaderImpl::GetInstance();
    if(_pDownloadManager)
    {
        return _pDownloadManager->GetTaskStateByUrlId(m_TaskUrl);
    }
    return IDownloadTask::NONE;
}

int CDownloadItemModelImpl::GetDownloadFileID()
{
    return -1;
}


int CDownloadItemModelImpl::GetDownloadPercentage()
{
    IDownloader *_pDownloadManager = DownloaderImpl::GetInstance();
    if(_pDownloadManager)
    {
        return _pDownloadManager->GetTaskPercentageByUrlId(m_TaskUrl);
    }
    return 0;
}

void CDownloadItemModelImpl::PauseDownloadUrl()
{
    IDownloader *_pDownloadManager = DownloaderImpl::GetInstance();
    if(_pDownloadManager)
        _pDownloadManager->PauseTask(m_TaskUrl);
}

void CDownloadItemModelImpl::ResumeFromPauseUrl()
{
    IDownloader *_pDownloadManager = DownloaderImpl::GetInstance();
    if(_pDownloadManager)
        _pDownloadManager->ResumeTask(m_TaskUrl);
}

void CDownloadItemModelImpl::DeleteDownloadUrl()
{
    IDownloader *_pDownloadManager = DownloaderImpl::GetInstance();
    if(_pDownloadManager)
        _pDownloadManager->DeleteTask(m_TaskUrl);
}

bool CDownloadItemModelImpl::CheckTaskIsExist()
{
    IDownloader *_pDownloadManager = DownloaderImpl::GetInstance();
    if(_pDownloadManager)
    {
        return _pDownloadManager->CheckSomeTaskIsExistByUrlId(m_TaskUrl);
    }
    return false;

}



