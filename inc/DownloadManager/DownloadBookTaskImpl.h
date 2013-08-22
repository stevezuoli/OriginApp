#ifndef __DOWNLOADBOOKTASKIMPL_H_
#define __DOWNLOADBOOKTASKIMPL_H_

#include "DownloadManager/IDownloadBookTask.h"
#include "DownloadManager/DownloadEngineImpl.h"
#include <string>
#include "interface.h"
class DownloadBookTaskImpl : public IDownloadBookTask
{
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
public:
    DownloadBookTaskImpl()
    {
        m_fileName = "";
        url = "";
        origurl = "";
        filesize = 0;
        m_strOrigUrlId = "";
        state =  WAITING;
        resumeDownload = false;
        lastoffset =0;
        m_type = IDownloadTask::BOOK;
        m_source = "";
        m_author = "";
        m_description = "";
        m_password = "";
        m_nPriority = 3;
        m_pengine = new SingleThreadDownloadEngineImpl(this);
        m_UserAgent = "";
		m_moveTo = "";
        m_displayName = "";
    };
    ~DownloadBookTaskImpl()
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

    std::string GetOrigUrl() const {return origurl;};
    void SetOrigUrl(std::string _url) {origurl = _url;};

    std::string GetOrigUrlID() const{return m_strOrigUrlId;};
    void SetOrigUrlID(std::string _urlid){m_strOrigUrlId = _urlid;};

    int GetPercentage() const;

    IDownloadTask::DLState GetState() const;
    void SetState(IDownloadTask::DLState _state);

    unsigned int GetFileSize() const
    {
        return filesize; 
    };
    void SetFileSize(unsigned int _size) { filesize = _size; };

    bool CanResume() const
    {
        return resumeDownload;
    }
    void SetCanResume(bool _value) { resumeDownload = _value; };

    unsigned int GetLastOffSet() const
    {
        return lastoffset;
    };
    void SetLastOffSet(unsigned int n) 
    {
        lastoffset = n;
    };

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
        return m_UserAgent;
    }
    void SetUserAgent(std::string _userAgent)
    {
        m_UserAgent = _userAgent;
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

public:
    void SetDescription(std::string _des);
    std::string GetDescription();

    void SetSource(std::string _source);
    std::string GetSource();

    void SetArtist(std::string _artist);
    std::string GetArtist();

	void SetMoveTo(const std::string& moveTo)
	{
		m_moveTo = moveTo;
	}

	std::string GetMoveTo()
	{
		return m_moveTo;
	}

    void SetDisplayName(const std::string& displayName)
    {
        m_displayName = displayName;
    }

    std::string GetDisplayName() const
    {
        return m_displayName;
    }


    void SetFilePassword(std::string _pwd)
    {
        m_password = _pwd;
    }
    std::string GetFilePassword() const
    {
        return m_password;
    }

private:
    
    std::string                        m_fileName;
    std::string                        url;
    std::string                        origurl;
    unsigned int                        filesize;
    unsigned int                        lastoffset;
    bool                                resumeDownload;
    IDownloadTask::DLState            state;
    std::string                        m_strOrigUrlId;
    IDownloadTask::DLType            m_type;
    int                                m_ifileID;
    unsigned int                        m_nPriority;
    SingleThreadDownloadEngineImpl*    m_pengine;


    std::string                 m_source;
    std::string                 m_author;
    std::string                 m_description;
    std::string                 m_password;
    std::string                 m_UserAgent;
	std::string					m_moveTo;
    std::string                 m_displayName;
};
#endif//__DOWNLOADBOOKTASKIMPL_H_

