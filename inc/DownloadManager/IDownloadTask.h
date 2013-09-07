#ifndef __IDOWNLOAD_TASK_H__
#define __IDOWNLOAD_TASK_H__
#include <fstream>
#include <string>
#include <time.h>
#include "../../DownloadManager/DlManagerSerialization.h"
#define USEXML 1

class IDownloadTask
{
public:
    enum DLState {
        NONE      = 0x0000,
        WAITING   = 0x0001,
        WORKING   = 0x0002,
        PAUSED    = 0x0004,
        DONE      = 0x0008,
        CANCELED  = 0x0010,
        UPDATED   = 0x0020,
        FAILED    = 0x0040,
        CURL_DONE = 0x0080,
        WAITING_QUEUE = 0x0100,
        FETCHING_DRM  = 0x0200,
        TOTAL     = 0x0400,
    };

    enum DLType {
        UNKNOWN_TYPE = 0x0000,
        BOOK  = 0x0001,
        VIDEO = 0x0002,
        AUDIO = 0x0004,
        UPGRADEPACKAGE = 0x0008,
		SCREENSAVER = 0x0010,
        MICLOUDFILE = 0x0020
    };

public:
    IDownloadTask()
        : m_lastUpdateTime(0)
    {}
    virtual ~IDownloadTask(){};

    virtual std::string GetFileName() const = 0;
    virtual void SetFileName(std::string _filename) = 0;

    virtual std::string GetUrl() const = 0;
    virtual void SetUrl(std::string _url) = 0;

    virtual std::string GetOrigUrl() const = 0;
    virtual void SetOrigUrl(std::string _url) = 0;

    virtual std::string GetOrigUrlID() const = 0;
    virtual void SetOrigUrlID(std::string _urlid) = 0;

    // from 0 to 100
    int GetPercentage() const
    {
        if (0 == GetFileSize())
        {
            return 0;
        }
        return (int)((double)GetLastOffSet() / (double)GetFileSize() * 100);
    }

    virtual DLState GetState() const = 0;
    virtual void SetState(DLState _state) = 0;

    virtual unsigned int GetFileSize() const = 0;
    virtual void SetFileSize(unsigned int _size) = 0;

    //TODO check if this is necessary
    virtual bool CanResume() const = 0;
    virtual void SetCanResume(bool _value) = 0;

    virtual unsigned int GetLastOffSet() const = 0;
    virtual void SetLastOffSet(unsigned int n) = 0;

    virtual DLType GetType() const = 0;
    virtual void SetType(DLType _type) = 0;

    virtual unsigned int GetPriority() const = 0;
    virtual void SetPriority(unsigned int _priority) = 0;

    virtual std::string GetUserAgent() const = 0;
    virtual void SetUserAgent(std::string _userAgent) = 0;

	virtual std::string GetFilePassword() const = 0;
	virtual void SetFilePassword(std::string _pwd) = 0;

    virtual std::string GetDisplayName() const = 0;
    virtual void SetDisplayName(const std::string& displayName) = 0;

    virtual void SaveTask(DlSerializationParentsNode& _parentsnode) = 0;
    virtual void LoadTask(xmlNodePtr pNode) = 0;
    
    //TODO check if this is necessary
    virtual int FinishTask() = 0;

    virtual void StartEngine() = 0;
    virtual void StopEngine() = 0;

    virtual bool TaskIsWorking() = 0;

    virtual bool IsUploadTask() const
    {
        return false;
    }
    // curl里回调, downloadTotal是本次需要下的字节数，downloadNow是这次已经下的字节数
    virtual void OnProgressUpdate(unsigned int downloadTotal, unsigned downloadNow);

    // Fire download event
    virtual void FireDownloadProgressUpdateEvent();

protected:
    time_t m_lastUpdateTime;
};
#endif//__IDOWNLOAD_TASK_H__

