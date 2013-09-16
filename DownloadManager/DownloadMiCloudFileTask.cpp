#include "DownloadManager/DownloadMiCloudFileTask.h"
#include "DownloadManager/DownloaderImpl.h"
#include "BookStore/BookStoreInfoManager.h"
#include "Framework/CNativeThread.h"
#include "Utility/PathManager.h"
#include "Common/FileManager_DK.h"
#include "Common/File_DK.h"
#include "dkBaseType.h"
#include "interface.h"
#include "Utility.h"
#include <string>
#include "I18n/StringManager.h"
using namespace std;

const std::string DownloadMiCloudFileTask::TASKFILENAME = "DkDlTaskFileName";
const std::string DownloadMiCloudFileTask::TASKURL = "DkDlTaskUrl";
const std::string DownloadMiCloudFileTask::TASKORIGURL = "DkDlTaskOrigUrl";
const std::string DownloadMiCloudFileTask::TASKPERCENTAGE = "DkDlTaskPercentage";
const std::string DownloadMiCloudFileTask::TASKFILESIZE = "DkDlTaskFileSize";
const std::string DownloadMiCloudFileTask::TASKLASTOFFSET = "DkDlTaskLastOffset";
const std::string DownloadMiCloudFileTask::TASKCANRESUME = "DkDlTaskCanResume";
const std::string DownloadMiCloudFileTask::TASKSTATE = "DkDlTaskState";
const std::string DownloadMiCloudFileTask::TASKORIGURLID = "DkDlTaskOrigUrlId";
const std::string DownloadMiCloudFileTask::TASKTYPE = "DkDlTaskType";
const std::string DownloadMiCloudFileTask::TASKBOOKSOUCE = "DkDlTaskSource";
const std::string DownloadMiCloudFileTask::TASKBOOKAUTHOR = "DkDlTaskAuthor";
const std::string DownloadMiCloudFileTask::TASKPRIORITY = "DkDlTaskPriority";
const std::string DownloadMiCloudFileTask::TASKDESCRIPTION = "DkDlTaskDescription";
const std::string DownloadMiCloudFileTask::TASKPASSWORD = "DkDlTaskPassword";
const std::string DownloadMiCloudFileTask::TASKUSERAGENT = "DkDlTaskUserAgent";
const std::string DownloadMiCloudFileTask::TASKMOVETO = "DkDlTaskMoveTo";
const std::string DownloadMiCloudFileTask::TASKDISPLAYNAME = "DkDlTaskDisplayName";
const std::string DownloadMiCloudFileTask::TASKUPLOADID = "DkDlTaskUploadID";
const std::string DownloadMiCloudFileTask::TASKKSSINFO = "DkDlTaskKSSInfo";

std::string DownloadMiCloudFileTask::GetFileName() const
{
    return m_strFileName;
}

void DownloadMiCloudFileTask::SetFileName(std::string _filename)
{
    m_strFileName = _filename;
}

std::string DownloadMiCloudFileTask::GetUrl() const
{
    return m_strUrl;
}

void DownloadMiCloudFileTask::SetUrl(std::string _url)
{
    m_strUrl = _url;
}

int DownloadMiCloudFileTask::GetPercentage() const
{
    if (GetFileSize() == 0)
    {
        return 0;
    }
    return GetLastOffSet() * 100 / GetFileSize();
}


IDownloadTask::DLState DownloadMiCloudFileTask::GetState() const
{
    return m_eState;
}

void DownloadMiCloudFileTask::SetState(IDownloadTask::DLState _state)
{
    m_eState = _state;
}

IDownloadTask::DLType DownloadMiCloudFileTask::GetType() const
{
    return m_eType;
}
void DownloadMiCloudFileTask::SetType(IDownloadTask::DLType _type)
{
    m_eType = _type;
}

void DownloadMiCloudFileTask::SaveTask(DlSerializationParentsNode& _parentsnode)
{
    _parentsnode.AddChildNode(TASKFILENAME,GetFileName());
    _parentsnode.AddChildNode(TASKFILESIZE,GetFileSize());
    _parentsnode.AddChildNode(TASKLASTOFFSET,GetLastOffSet());
    _parentsnode.AddChildNode(TASKCANRESUME,CanResume());
    _parentsnode.AddChildNode(TASKORIGURL,GetOrigUrl());
    _parentsnode.AddChildNode(TASKORIGURLID,GetOrigUrlID());
    _parentsnode.AddChildNode(TASKPERCENTAGE,GetPercentage());
    _parentsnode.AddChildNode(TASKSTATE,(int)GetState());
    _parentsnode.AddChildNode(TASKTYPE,(int)GetType());
    _parentsnode.AddChildNode(TASKURL,GetUrl());
    _parentsnode.AddChildNode(TASKPASSWORD,m_strPassword);
    _parentsnode.AddChildNode(TASKUSERAGENT,m_UserAgent);
    _parentsnode.AddChildNode(TASKPRIORITY,m_uPriority);
    _parentsnode.AddChildNode(TASKMOVETO,m_moveTo);
    _parentsnode.AddChildNode(TASKUPLOADID,m_uploadId);
    _parentsnode.AddChildNode(TASKKSSINFO,m_kssInfo);
}

void DownloadMiCloudFileTask::LoadTask(xmlNodePtr pNode)
{
    pNode = pNode->children;
    while(pNode)
    {
        if(!xmlStrcmp(pNode->name,BAD_CAST(TASKFILENAME.c_str())))
        {
            xmlChar *pName = xmlNodeGetContent(pNode);
            if(pName)
            {
                SetFileName((const char *)pName);
                xmlFree(pName);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKFILESIZE.c_str())))
        {
            xmlChar *pSize = xmlNodeGetContent(pNode);
            if(pSize)
            {
                SetFileSize(atoi((const char *)pSize));
                xmlFree(pSize);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKLASTOFFSET.c_str())))
        {
            xmlChar *plastOff = xmlNodeGetContent(pNode);
            if(plastOff)
            {
                SetLastOffSet(atoi((const char *)plastOff));
                xmlFree(plastOff);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKCANRESUME.c_str())))
        {
            xmlChar *pCanResume = xmlNodeGetContent(pNode);
            if(pCanResume)
            {
                SetCanResume(atoi((const char *)pCanResume));
                xmlFree(pCanResume);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKORIGURL.c_str())))
        {
            xmlChar *pOrigUrl = xmlNodeGetContent(pNode);
            if(pOrigUrl)
            {
                SetOrigUrl((const char *)pOrigUrl);
                xmlFree(pOrigUrl);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKORIGURLID.c_str())))
        {
            xmlChar *pOrigUrlId = xmlNodeGetContent(pNode);
            if(pOrigUrlId)
            {
                SetOrigUrlID((const char *)pOrigUrlId);
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
                SetState(iState);
                xmlFree(pState);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKTYPE.c_str())))
        {
            xmlChar *pType = xmlNodeGetContent(pNode);
            if(pType)
            {
                SetType((DLType)atoi((const char *)pType));
                xmlFree(pType);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKURL.c_str())))
        {
            xmlChar *pUrl = xmlNodeGetContent(pNode);
            if(pUrl)
            {
                SetUrl((const char *)pUrl);
                xmlFree(pUrl);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKMOVETO.c_str())))
        {
            xmlChar *pMoveTo = xmlNodeGetContent(pNode);
            if(pMoveTo)
            {
                SetMoveTo((const char *)pMoveTo);
                xmlFree(pMoveTo);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKDISPLAYNAME.c_str())))
        {
            xmlChar *pDisplayName = xmlNodeGetContent(pNode);
            if(pDisplayName)
            {
                SetDisplayName((const char *)pDisplayName);
                xmlFree(pDisplayName);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKUPLOADID.c_str())))
        {
            xmlChar *pUpload = xmlNodeGetContent(pNode);
            if(pUpload)
            {
                SetUploadId((const char *)pUpload);
                xmlFree(pUpload);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKKSSINFO.c_str())))
        {
            xmlChar *pKSSInfo = xmlNodeGetContent(pNode);
            if(pKSSInfo)
            {
                SetKSSInfo((const char *)pKSSInfo);
                xmlFree(pKSSInfo);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKPASSWORD.c_str())))
        {
            xmlChar *pPassword = xmlNodeGetContent(pNode);
            if(pPassword)
            {
                SetFilePassword((const char *)pPassword);
                xmlFree(pPassword);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKUSERAGENT.c_str())))
        {
            xmlChar *pUserAgent = xmlNodeGetContent(pNode);
            if(pUserAgent)
            {
                SetUserAgent((const char *)pUserAgent);
                xmlFree(pUserAgent);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKPRIORITY.c_str())))
        {
            xmlChar *pPriority = xmlNodeGetContent(pNode);
            if(pPriority)
            {
                SetPriority(atoi((const char *)pPriority));
                xmlFree(pPriority);
            }
        }
        pNode = pNode->next;
    }
}

int DownloadMiCloudFileTask::FinishTask()
{
    SNativeMessage msg;
    msg.iType = KMessageDownloadStatusUpdate;
    CNativeThread::Send(msg);

    FireDownloadProgressUpdateEvent();
    return 1;
}

void DownloadMiCloudFileTask::OnKSSProgressCallback(double cur, double total, double speed)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "%s ... %f %f %f", IsUploadTask() ? "Uploading" : "Downloading", cur, total, speed);
    SetFileSize(total);
    OnProgressUpdate(total, cur);
}
