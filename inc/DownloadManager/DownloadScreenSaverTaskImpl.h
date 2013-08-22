#ifndef __DOWNLOADSCREENSAVERTASKIMPL_H_
#define __DOWNLOADSCREENSAVERTASKIMPL_H_

#include "DownloadManager/IDownloadTask.h"
#include "DownloadManager/DownloadEngineImpl.h"
#include <string>
#include "interface.h"
class DownloadScreenSaverTaskImpl : public IDownloadTask{
public:
	const static std::string TASKFILENAME;
	const static std::string TASKURL;
	const static std::string TASKORIGURL;
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

public:
	DownloadScreenSaverTaskImpl()
	{
		m_strFileName = "";
		m_strUrl = "";
		m_strOrigurl = "";
		m_uFileSize = 0;
		m_strOrigUrlId = "";
		m_eState =  WAITING;
		m_bResume = false;
		m_uLastOffset =0;
		m_eType = IDownloadTask::SCREENSAVER;
		m_uPriority = 3;
		m_pEngine = new SingleThreadDownloadEngineImpl(this);
		m_UserAgent = "";
		m_strPassword = "";
	};
	~DownloadScreenSaverTaskImpl()
	{
		if(m_pEngine)
		{
			delete m_pEngine;
			m_pEngine = NULL;
		}
	};
	std::string GetFileName() const;
	void SetFileName(std::string _filename);

	std::string GetUrl() const;
	void SetUrl(std::string _url);

	std::string GetOrigUrl() const {return m_strOrigurl;};
	void SetOrigUrl(std::string _url) {m_strOrigurl = _url;};

	std::string GetOrigUrlID() const {return m_strOrigUrlId;};
	void SetOrigUrlID(std::string _urlid){m_strOrigUrlId = _urlid;};

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


private:

	std::string							m_strFileName;
	std::string							m_strUrl;
	std::string							m_strOrigurl;
	unsigned int						m_uFileSize;
	unsigned int						m_uLastOffset;
	bool								m_bResume;
	IDownloadTask::DLState				m_eState;
	std::string							m_strOrigUrlId;
	IDownloadTask::DLType				m_eType;
	unsigned int                        m_uPriority;
	SingleThreadDownloadEngineImpl*		m_pEngine;
	std::string							m_strPassword;


	std::string                    m_UserAgent;

};
#endif//__DOWNLOADSCREENSAVERTASKIMPL_H_
