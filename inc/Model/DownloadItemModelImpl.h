////////////////////////////////////////////////////////////////////////
// 91SearchItemModelImpl.h
// Contact: xukai
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __DOWNLOADITEMMODELIMPL_H__
#define __DOWNLOADITEMMODELIMPL_H__

#include "Model/IDownloadItemModel.h"
#include "DownloadManager/IDownloader.h"
#include "DownloadManager/DownloaderImpl.h"
#include "DownloadManager/IDownloadTask.h"
#include <string>

class CDownloadItemModelImpl : public IDownloadItemModel
{
public:
    CDownloadItemModelImpl(std::string url) : m_TaskUrl(url) {}

    virtual ~CDownloadItemModelImpl() {}

    virtual const std::string GetBookName();

    virtual IDownloadTask::DLState GetDownloadStatus();

    virtual int GetDownloadPercentage();
    virtual int GetDownloadFileID();
    virtual void PauseDownloadUrl();
    virtual void ResumeFromPauseUrl();
    virtual void DeleteDownloadUrl();
    virtual bool CheckTaskIsExist();
    void SetTaskUrl(std::string url)
    {
        m_TaskUrl = url;
    }

    std::string GetTaskUrl()
    {
        return m_TaskUrl;
    }

private:
    std::string m_TaskUrl;

};
#endif  // __DOWNLOADITEMMODELIMPL_H__



