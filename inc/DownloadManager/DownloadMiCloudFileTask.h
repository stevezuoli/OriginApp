#ifndef __DOWNLOAD_MICLOUD_FILE_TASK_H__
#define __DOWNLOAD_MICLOUD_FILE_TASK_H__

#include "DownloadManager/IDownloadTask.h"
#include "DownloadManager/MiCloudFileDownloadEngine.h"
#include <string>
#include "interface.h"
class DownloadMiCloudFileTask : public IDownloadTask{
public:
    const static std::string TASKFILENAME;
    const static std::string TASKURL;
    const static std::string TASKORIGURL;
    const static std::string TASKPERCENTAGE;
    const static std::string TASKFILESIZE;
    const static std::string TASKLASTOFFSET;
    const static std::string TASKCANRESUME;
    const static std::string TASKSTATE;
    const static std::string TASKORIGURLID;
    const static std::string TASKTYPE;
    const static std::string TASKBOOKSOUCE;
    const static std::string TASKBOOKAUTHOR;
    const static std::string TASKPRIORITY;
    const static std::string TASKDESCRIPTION;
    const static std::string TASKPASSWORD;
    const static std::string TASKUSERAGENT;
    const static std::string TASKMOVETO;
    const static std::string TASKDISPLAYNAME;
    const static std::string TASKUPLOADID;
    const static std::string TASKKSSINFO;
public:
    DownloadMiCloudFileTask()
    {
        m_uFileSize = 0;
        m_eState =  WAITING;
        m_bResume = false;
        m_uLastOffset =0;
        m_eType = IDownloadTask::MICLOUDFILE;
        m_uPriority = 3;
        m_pEngine = new MiCloudFileDownloadEngine(this);
    };

    ~DownloadMiCloudFileTask()
    {
        if(m_pEngine)
        {
            delete m_pEngine;
            m_pEngine = NULL;
        }
    };

    void OnKSSProgressCallback(double cur, double total, double speed);
    std::string GetFileName() const;
    void SetFileName(std::string _filename);

    std::string GetUrl() const;
    void SetUrl(std::string _url);

    std::string GetOrigUrl() const {return m_strOrigurl;};
    void SetOrigUrl(std::string _url) {m_strOrigurl = _url;};

    std::string GetOrigUrlID() const {return m_strOrigUrlId;};
    void SetOrigUrlID(std::string _urlid){m_strOrigUrlId = _urlid;};

    int GetPercentage() const;

    IDownloadTask::DLState GetState() const;
    void SetState(IDownloadTask::DLState _state);

    unsigned int GetFileSize() const { return m_uFileSize; };
    void SetFileSize(unsigned int _size) { m_uFileSize = _size; };

    bool CanResume() const { return m_bResume; };
    void SetCanResume(bool _value) { m_bResume = _value; };

    unsigned int GetLastOffSet() const { return m_uLastOffset;};
    void SetLastOffSet(unsigned int n) { m_uLastOffset = n;};

    IDownloadTask::DLType GetType() const;
    void SetType(IDownloadTask::DLType _type);

    unsigned int GetPriority() const
    {
        return m_uPriority;
    }
    void SetPriority(unsigned int _priority)
    {
        m_uPriority = _priority;
    }

    std::string GetUserAgent() const
    {
        return m_UserAgent;
    }
    void SetUserAgent(std::string _userAgent)
    {
        m_UserAgent = _userAgent;
    }
    void SetFilePassword(std::string _pwd)
    {
        m_strPassword = _pwd;
    }
    std::string GetFilePassword() const
    {
        return m_strPassword;
    }

    void SetDisplayName(const std::string& displayName)
    {
    }

    std::string GetDisplayName() const
    {
        return "";
    }

    void SaveTask(DlSerializationParentsNode& _parentsnode);
    void LoadTask(xmlNodePtr pNode);

    int FinishTask();
    void StartEngine()
    {
        return m_pEngine->Start();
    }
    void StopEngine()
    {
        return m_pEngine->Stop();
    }

    bool TaskIsWorking()
    {
        return m_pEngine->TaskIsWorking();
    }
    int GetFileInfoFromServer(std::string _url,void* _pinfo)
    {
        return m_pEngine->GetFileInfoFromServer(_url,  _pinfo);
    }

    void SetKSSInfo(const std::string& _kssInfo)
    {
        m_kssInfo = _kssInfo;
    }
    std::string GetKSSInfo() const
    {
        return m_kssInfo;
    }

    void SetMoveTo(const std::string& _moveTo)
    {
        m_moveTo = _moveTo;
    }
    std::string GetMoveTo() const
    {
        return m_moveTo;
    }

    void SetUploadId(const std::string& uploadId)
    {
        m_uploadId = uploadId;
    }

    std::string GetUploadId() const
    {
        return m_uploadId;
    }

    bool IsUploadTask() const
    {
        return !m_uploadId.empty();
    }
private:
    std::string                         m_strFileName;
    std::string                         m_strUrl;
    std::string                         m_strOrigurl;
    unsigned int                        m_uFileSize;
    unsigned int                        m_uLastOffset;
    bool                                m_bResume;
    IDownloadTask::DLState              m_eState;
    std::string                         m_strOrigUrlId;
    IDownloadTask::DLType               m_eType;
    unsigned int                        m_uPriority;
    MiCloudFileDownloadEngine*     m_pEngine;
    std::string                         m_strPassword;


    std::string                    m_UserAgent;

    std::string                    m_kssInfo;
    std::string                    m_moveTo;

    //used in upload
    std::string                    m_uploadId;
};
#endif//__DOWNLOADMICLOUDFILETASK_H_
