#include "DownloadManager/DownloadReleaseTaskImpl.h"
#include "DownloadManager/DownloaderImpl.h"
#include "Framework/CNativeThread.h"
#include "Common/FileManager_DK.h"
#include "interface.h"
#include <string>

using namespace std;

const std::string DownloadReleaseTaskImpl::TASKFILENAME = "DkDlTaskFileName";
const std::string DownloadReleaseTaskImpl::TASKURL = "DkDlTaskUrl";
const std::string DownloadReleaseTaskImpl::TASKORIGURL = "DkDlTaskOrigUrl";
const std::string DownloadReleaseTaskImpl::TASKPERCENTAGE = "DkDlTaskPercentage";
const std::string DownloadReleaseTaskImpl::TASKFILESIZE = "DkDlTaskFileSize";
const std::string DownloadReleaseTaskImpl::TASKLASTOFFSET = "DkDlTaskLastOffset";
const std::string DownloadReleaseTaskImpl::TASKCANRESUME = "DkDlTaskCanResume";
const std::string DownloadReleaseTaskImpl::TASKSTATE = "DkDlTaskState";
const std::string DownloadReleaseTaskImpl::TASKORIGURLID = "DkDlTaskOrigUrlId";
const std::string DownloadReleaseTaskImpl::TASKTYPE = "DkDlTaskType";
const std::string DownloadReleaseTaskImpl::TASKFILEID = "DkDlTaskFileID";
const std::string DownloadReleaseTaskImpl::TASKPRIORITY = "DkDlTaskPriority";
const std::string DownloadReleaseTaskImpl::TASKDESCRIPTION = "DkDlTaskDescription";
const std::string DownloadReleaseTaskImpl::TASKPASSWORD = "DkDlTaskPassword";
const std::string DownloadReleaseTaskImpl::TASKUSERAGENT = "DkDlTaskUserAgent";

std::string DownloadReleaseTaskImpl::GetFileName() const
{
    //DebugPrintf(DLC_XU_KAI,"DownloadReleaseTaskImpl::GetFileName this->targetFileName IS %s",this->m_filename.c_str());
    return this->m_filename;
}

void DownloadReleaseTaskImpl::SetFileName(std::string _filename)
{
    this->m_filename = _filename;
}

std::string DownloadReleaseTaskImpl::GetUrl() const
{
    return this->m_url;
}

void DownloadReleaseTaskImpl::SetUrl(std::string _url)
{
    this->m_url = _url;
}



IDownloadTask::DLState DownloadReleaseTaskImpl::GetState() const
{
    return this->m_state;
}

void DownloadReleaseTaskImpl::SetState(IDownloadTask::DLState _state)
{
    this->m_state = _state;
}

IDownloadTask::DLType DownloadReleaseTaskImpl::GetType() const
{
    return this->m_type;
}
void DownloadReleaseTaskImpl::SetType(IDownloadTask::DLType _type)
{
    this->m_type = _type;
}

void DownloadReleaseTaskImpl::SaveTask(DlSerializationParentsNode& _parentsnode)
{
    _parentsnode.AddChildNode(TASKFILENAME,this->GetFileName());
    _parentsnode.AddChildNode(TASKFILESIZE,this->GetFileSize());
    _parentsnode.AddChildNode(TASKLASTOFFSET,this->GetLastOffSet());
    _parentsnode.AddChildNode(TASKCANRESUME,this->CanResume());
    _parentsnode.AddChildNode(TASKORIGURL,this->GetOrigUrl());
    _parentsnode.AddChildNode(TASKORIGURLID,this->GetOrigUrlID());
    _parentsnode.AddChildNode(TASKPERCENTAGE,this->GetPercentage());
    _parentsnode.AddChildNode(TASKSTATE,(int)this->GetState());
    _parentsnode.AddChildNode(TASKTYPE,(int)this->GetType());
    _parentsnode.AddChildNode(TASKURL,this->GetUrl());
    _parentsnode.AddChildNode(TASKDESCRIPTION,this->m_description);
    _parentsnode.AddChildNode(TASKPASSWORD,this->m_password);
    _parentsnode.AddChildNode(TASKUSERAGENT,this->m_useragent);
    _parentsnode.AddChildNode(TASKFILEID,this->m_ifileID);
    _parentsnode.AddChildNode(TASKPRIORITY,this->m_nPriority);
}

void DownloadReleaseTaskImpl::LoadTask(xmlNodePtr pNode)
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
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKFILEID.c_str())))
        {
            xmlChar *pFileid = xmlNodeGetContent(pNode);
            if(pFileid)
            {
                this->m_ifileID = atoi((const char *)pFileid);
                xmlFree(pFileid);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKDESCRIPTION.c_str())))
        {
            xmlChar *pDescription = xmlNodeGetContent(pNode);
            if(pDescription)
            {
                this->SetDescription((const char *)pDescription);
                xmlFree(pDescription);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKPASSWORD.c_str())))
        {
            xmlChar *pPassword = xmlNodeGetContent(pNode);
            if(pPassword)
            {
                this->SetFilePassword((const char *)pPassword);
                xmlFree(pPassword);
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

int DownloadReleaseTaskImpl::FinishTask()
{
    SNativeMessage msg;
    msg.iType = KMessageDownloadStatusUpdate;
    CNativeThread::Send(msg);

    msg.iType = KMessageNewReleaseUpgradeNotify;
    CNativeThread::Send(msg);
    return 0;
}

void DownloadReleaseTaskImpl::SetDescription(std::string _des)
{
    this->m_description = _des;
}

std::string DownloadReleaseTaskImpl::GetDescription() const
{
    return this->m_description;
}


