#ifndef __DOWNLOADRELEASETASKIMPL_H_
#define __DOWNLOADRELEASETASKIMPL_H_

#include "DownloadManager/IDownloadReleaseTask.h"
#include "DownloadManager/DownloadEngineImpl.h"
#include <string>
#include "interface.h"
class DownloadReleaseTaskImpl : public IDownloadReleaseTask{
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
    const static std::string TASKFILEID;
    const static std::string TASKPRIORITY;
    const static std::string TASKDESCRIPTION;
    const static std::string TASKPASSWORD;
    const static std::string TASKUSERAGENT;
public:
    DownloadReleaseTaskImpl()
    {
        m_state =  WAITING;
        m_resumeDownload = false;
        m_lastoffset =0;
        m_type = IDownloadTask::UPGRADEPACKAGE;
        m_nPriority = 5;
        m_pengine = new SingleThreadDownloadEngineImpl(this);
        m_useragent = "";
    };
    ~DownloadReleaseTaskImpl()
    {
        if(m_pengine)
        {
            delete m_pengine;
            m_pengine = NULL;
        }
    };
    std::string GetFileName() const;
    void SetFileName(std::string _filename);

    std::string GetUrl() const;
    void SetUrl(std::string _url);

    std::string GetOrigUrl() const {return m_origurl;};
    void SetOrigUrl(std::string _url) {m_origurl = _url;};

    std::string GetOrigUrlID() const {return m_strOrigUrlId;};
    void SetOrigUrlID(std::string _urlid){m_strOrigUrlId = _urlid;};

    IDownloadTask::DLState GetState() const;
    void SetState(IDownloadTask::DLState _state);

    unsigned int GetFileSize() const { return m_filesize; };
    void SetFileSize(unsigned int _size) { m_filesize = _size; };

    bool CanResume() const { return m_resumeDownload; };
    void SetCanResume(bool _value) { m_resumeDownload = _value; };

    unsigned int GetLastOffSet() const { return m_lastoffset;};
    void SetLastOffSet(unsigned int n) { m_lastoffset = n;};

    IDownloadTask::DLType GetType() const;
    void SetType(IDownloadTask::DLType _type);

    unsigned int GetPriority() const
    {
        return m_nPriority;
    }
    void SetPriority(unsigned int _priority)
    {
        m_nPriority = _priority;
    }
    std::string GetUserAgent() const
    {
        return m_useragent;
    }
    void SetUserAgent(std::string _userAgent)
    {
        m_useragent = _userAgent;
    }
    void SaveTask(DlSerializationParentsNode& _parentsnode);
    void LoadTask(xmlNodePtr pNode);
    int FinishTask();
    void StartEngine()
    {
        return m_pengine->Start();
    }
    void StopEngine()
    {
        return m_pengine->Stop();
    }

    bool TaskIsWorking()
    {
        return m_pengine->TaskIsWorking();
    }
    int GetFileInfoFromServer(std::string _url,void* _pinfo)
    {
        return m_pengine->GetFileInfoFromServer(_url,  _pinfo);
    }

    void SetDisplayName(const std::string& displayName)
    {

    }

    std::string GetDisplayName() const
    {
        return "";
    }

public:
    void SetDescription(std::string _des);
    std::string GetDescription() const;


    void SetFilePassword(std::string _pwd)
    {
        m_password = _pwd;
    }
    std::string GetFilePassword() const
    {
        return m_password;
    }
    
private:
    
    std::string                        m_filename;
    std::string                        m_url;
    std::string                        m_origurl;
    unsigned int                    m_filesize;
    unsigned int                    m_lastoffset;
    bool                            m_resumeDownload;
    IDownloadTask::DLState            m_state;
    std::string                        m_strOrigUrlId;
    IDownloadTask::DLType            m_type;
    int                                m_ifileID;
    unsigned int                    m_nPriority;
    SingleThreadDownloadEngineImpl*    m_pengine;
    std::string                     m_description;
    std::string                     m_password;
    std::string                        m_useragent;

};
#endif//__DOWNLOADUPDATETASKIMPL_H_

