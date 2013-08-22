#include "DownloadManager/DownloadScreenSaverTaskImpl.h"
#include "DownloadManager/DownloaderImpl.h"
#include "Framework/CNativeThread.h"
#include "../Common/FileManager_DK.h"
#include "Common/File_DK.h"
#include "dkBaseType.h"
#include "interface.h"
#include "Utility.h"
#include <string>
#include "I18n/StringManager.h"
using namespace std;

const std::string DownloadScreenSaverTaskImpl::TASKFILENAME = "DkDlTaskFileName";
const std::string DownloadScreenSaverTaskImpl::TASKURL = "DkDlTaskUrl";
const std::string DownloadScreenSaverTaskImpl::TASKORIGURL = "DkDlTaskOrigUrl";
const std::string DownloadScreenSaverTaskImpl::TASKFILESIZE = "DkDlTaskFileSize";
const std::string DownloadScreenSaverTaskImpl::TASKLASTOFFSET = "DkDlTaskLastOffset";
const std::string DownloadScreenSaverTaskImpl::TASKCANRESUME = "DkDlTaskCanResume";
const std::string DownloadScreenSaverTaskImpl::TASKSTATE = "DkDlTaskState";
const std::string DownloadScreenSaverTaskImpl::TASKORIGURLID = "DkDlTaskOrigUrlId";
const std::string DownloadScreenSaverTaskImpl::TASKTYPE = "DkDlTaskType";
const std::string DownloadScreenSaverTaskImpl::TASKBOOKSOUCE = "DkDlTaskSource";
const std::string DownloadScreenSaverTaskImpl::TASKBOOKAUTHOR = "DkDlTaskAuthor";
const std::string DownloadScreenSaverTaskImpl::TASKPRIORITY = "DkDlTaskPriority";
const std::string DownloadScreenSaverTaskImpl::TASKDESCRIPTION = "DkDlTaskDescription";
const std::string DownloadScreenSaverTaskImpl::TASKPASSWORD = "DkDlTaskPassword";
const std::string DownloadScreenSaverTaskImpl::TASKUSERAGENT = "DkDlTaskUserAgent";

#define XUKAIDEBUG 1
std::string DownloadScreenSaverTaskImpl::GetFileName() const
{
	DebugPrintf(DLC_XU_KAI,"DownloadScreenSaverTaskImpl::GetFileName this->targetFileName IS %s",this->m_strFileName.c_str());
	return this->m_strFileName;
}

void DownloadScreenSaverTaskImpl::SetFileName(std::string _strFilename)
{
	this->m_strFileName = _strFilename;
}

std::string DownloadScreenSaverTaskImpl::GetUrl() const
{
	return this->m_strUrl;
}

void DownloadScreenSaverTaskImpl::SetUrl(std::string _url)
{
	this->m_strUrl = _url;
}


IDownloadTask::DLState DownloadScreenSaverTaskImpl::GetState() const
{
	return this->m_eState;
}

void DownloadScreenSaverTaskImpl::SetState(IDownloadTask::DLState _state)
{
	this->m_eState = _state;
}

IDownloadTask::DLType DownloadScreenSaverTaskImpl::GetType() const
{
	return this->m_eType;
}
void DownloadScreenSaverTaskImpl::SetType(IDownloadTask::DLType _type)
{
	this->m_eType = _type;
}

void DownloadScreenSaverTaskImpl::SaveTask(DlSerializationParentsNode& _parentsnode)
{
	_parentsnode.AddChildNode(TASKFILENAME,this->GetFileName());
	_parentsnode.AddChildNode(TASKFILESIZE,this->GetFileSize());
	_parentsnode.AddChildNode(TASKLASTOFFSET,this->GetLastOffSet());
	_parentsnode.AddChildNode(TASKCANRESUME,this->CanResume());
	_parentsnode.AddChildNode(TASKORIGURL,this->GetOrigUrl());
	_parentsnode.AddChildNode(TASKORIGURLID,this->GetOrigUrlID());
	_parentsnode.AddChildNode(TASKSTATE,(int)this->GetState());
	_parentsnode.AddChildNode(TASKTYPE,(int)this->GetType());
	_parentsnode.AddChildNode(TASKURL,this->GetUrl());
	_parentsnode.AddChildNode(TASKUSERAGENT,this->GetUserAgent());
	_parentsnode.AddChildNode(TASKPRIORITY,this->GetPriority());
}

void DownloadScreenSaverTaskImpl::LoadTask(xmlNodePtr pNode)
{
	pNode = pNode->children;
	while(pNode)
	{
		if(!xmlStrcmp(pNode->name,BAD_CAST(TASKFILENAME.c_str())))
		{
			xmlChar *pName = xmlNodeGetContent(pNode);
			if(pName)
			{
				this->SetFileName((const char *)pName);
				xmlFree(pName);
			}
		}
		else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKFILESIZE.c_str())))
		{
			xmlChar *pSize = xmlNodeGetContent(pNode);
			if(pSize)
			{
				this->SetFileSize(atoi((const char *)pSize));
				xmlFree(pSize);
			}
		}
		else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKLASTOFFSET.c_str())))
		{
			xmlChar *plastOff = xmlNodeGetContent(pNode);
			if(plastOff)
			{
				this->SetLastOffSet(atoi((const char *)plastOff));
				xmlFree(plastOff);
			}
		}
		else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKCANRESUME.c_str())))
		{
			xmlChar *pCanResume = xmlNodeGetContent(pNode);
			if(pCanResume)
			{
				this->SetCanResume(atoi((const char *)pCanResume));
				xmlFree(pCanResume);
			}
		}
		else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKORIGURL.c_str())))
		{
			xmlChar *pOrigUrl = xmlNodeGetContent(pNode);
			if(pOrigUrl)
			{
				this->SetOrigUrl((const char *)pOrigUrl);
				xmlFree(pOrigUrl);
			}
		}
		else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKORIGURLID.c_str())))
		{
			xmlChar *pOrigUrlId = xmlNodeGetContent(pNode);
			if(pOrigUrlId)
			{
				this->SetOrigUrlID((const char *)pOrigUrlId);
				xmlFree(pOrigUrlId);
			}
		}
		else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKSTATE.c_str())))
		{
			xmlChar *pState = xmlNodeGetContent(pNode);
			if(pState)
			{
				DLState iState = (DLState)atoi((const char *)pState);
				if(iState == IDownloadTask::WORKING)
				{
					iState = IDownloadTask::PAUSED;
				}
				this->SetState(iState);
				xmlFree(pState);
			}
		}
		else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKTYPE.c_str())))
		{
			xmlChar *pType = xmlNodeGetContent(pNode);
			if(pType)
			{
				this->SetType((DLType)atoi((const char *)pType));
				xmlFree(pType);
			}
		}
		else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKURL.c_str())))
		{
			xmlChar *pUrl = xmlNodeGetContent(pNode);
			if(pUrl)
			{
				this->SetUrl((const char *)pUrl);
				xmlFree(pUrl);
			}
		}
		else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKUSERAGENT.c_str())))
		{
			xmlChar *pUserAgent = xmlNodeGetContent(pNode);
			if(pUserAgent)
			{
				this->SetUserAgent((const char *)pUserAgent);
				xmlFree(pUserAgent);
			}
		}
		else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKPRIORITY.c_str())))
		{
			xmlChar *pPriority = xmlNodeGetContent(pNode);
			if(pPriority)
			{
				this->SetPriority(atoi((const char *)pPriority));
				xmlFree(pPriority);
			}
		}
		pNode = pNode->next;
	}
}

int DownloadScreenSaverTaskImpl::FinishTask()
{

	SNativeMessage msg;
	msg.iType = KMessageScreenSaverDownloaded;
	msg.iParam1 = (UINT32)(GetOrigUrl().c_str());
	msg.iParam2 = (UINT32)(GetFileName().c_str());
    CNativeThread::Send(msg);

	return 1;
}


