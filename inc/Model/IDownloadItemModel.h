////////////////////////////////////////////////////////////////////////
// I91SearchItemModel.h
// Contact: xukai
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __IDOWNLOADITEMMODEL_H__
#define __IDOWNLOADITEMMODEL_H__

#include "dkBaseType.h"
#include "DownloadManager/DownloaderImpl.h"
#include <string>

class IDownloadItemModel
{
public:
    IDownloadItemModel() {}
    virtual ~IDownloadItemModel() {}

    virtual const std::string GetBookName()=0;

    virtual IDownloadTask::DLState GetDownloadStatus()=0;

    virtual int GetDownloadPercentage()=0;

    virtual int GetDownloadFileID() = 0;
    virtual void PauseDownloadUrl() = 0;
    virtual void ResumeFromPauseUrl() = 0;
    virtual void DeleteDownloadUrl() = 0;
    virtual bool CheckTaskIsExist() = 0;
    virtual bool CanResume() = 0;
    virtual bool IsUploadTask() = 0;
};


#endif  // __IDOWNLOADITEMMODEL_H__


