#include "DownloadManager/DownloaderImpl.h"
#include "DownloadManager/DownloadBookTaskImpl.h"
#include "DownloadManager/DownloadTaskFactory.h"
#include "DownloadManager/curlDownload.h"
#include "Common/FileManager_DK.h"
#include "DlManagerSerialization.h"
#include "interface.h"
#include "Framework/CNativeThread.h"
#include "Utility/FileSystem.h"
#include "Utility/ThreadHelper.h"
#include <fstream>
#include<sstream>
#include<string.h>
#include <algorithm>
#include <unistd.h>
#include <list>
#include "PowerManager/PowerManager.h"

using namespace std;
using namespace PowerManagement;
using namespace dk::common;

const char* IDownloader::EventDownloadProgressUpdate = "EventDownloadProgressUpdate";
//################## Implement the DownloadManager #############################

// Called by downloading thread
void IDownloader::FireDownloadProgressUpdateEvent(IDownloadTask::DLType type,
                                                  IDownloadTask::DLState state,
                                                  int percentage,
                                                  std::string urlID,
                                                  bool isUploadTask)
{
    DownloadUpdateEventArgs args;
    args.taskId = urlID;
    args.percentage = percentage;
    args.state = state;
    args.type  = type;
    args.isUpload = isUploadTask;
    FireEvent(IDownloader::EventDownloadProgressUpdate, args);
}

int GetDownloadTaskNums(DownloadTaskNums& tasks, int state)
{
    if (tasks.find(state) != tasks.end())
    {
        return tasks[state];
    }
    return 0;
}

DownloaderImpl::DownloaderImpl(void)
{
    m_threadWorking = false;
    m_maxtasknum = 1;
    m_taskThread = 0;
    LoadHistroy();
    StartDownload();
}

DownloaderImpl::~DownloaderImpl(void)
{
    StopDownload();

    //STOP the downding thread, cancel the curl thread, stop the date to local file
    DlManagerSerialization *pSerialization = new DlManagerSerialization();
    if(pSerialization != 0)
    {
        DlSerializationParentsNode &RootNode = pSerialization->CreateRootNode("DKDownloadManager");
        for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end(); itr++ )
        {
            if((*itr))
            {
                if((*itr)->GetState() == IDownloadTask::WORKING)
                {
                    (*itr)->StopEngine();
                    (*itr)->SetState(IDownloadTask::WAITING);
                }
                DlSerializationParentsNode &ParentsNode = RootNode.AddChildNode("DkDlTaskItem");
                ParentsNode.AddAttribute("DkDlTaskType",(int)(*itr)->GetType());
                (*itr)->SaveTask(ParentsNode);
                delete (*itr);
                *itr = 0;
            }
        }
        pSerialization->SaveXmlFile(DOWNLOADHISTORY);
        delete pSerialization;
    }
}


void DownloaderImpl::SaveHistroy(void)
{
    LockScope scope(m_lock);
    DlManagerSerialization *pSerialization = new DlManagerSerialization();
    if(pSerialization == 0)
    {
        return;
    }
    DlSerializationParentsNode &RootNode = pSerialization->CreateRootNode("DKDownloadManager");
    for(std::list<IDownloadTask*>::iterator itr = m_tasks.begin();
        itr != m_tasks.end();
        itr++ )
    {
        if((*itr) && !((*itr)->GetState() & (IDownloadTask::CANCELED |
                                             IDownloadTask::DONE |
                                             IDownloadTask::FAILED)))
        {
            DlSerializationParentsNode &ParentsNode = RootNode.AddChildNode("DkDlTaskItem");
            ParentsNode.AddAttribute("DkDlTaskType",(int)(*itr)->GetType());
            (*itr)->SaveTask(ParentsNode);
        }
    }
    pSerialization->SaveXmlFile(DOWNLOADHISTORY);
    delete pSerialization; 
    pSerialization = 0;
}


void DownloaderImpl::LoadHistroy(void)
{
    LockScope scope(m_lock);
    DlManagerSerialization *pSerialization = new DlManagerSerialization();
    if(pSerialization == 0)
    {
        return;
    }
    bool bre = pSerialization->ReadXmlFile(DOWNLOADHISTORY);
    if(!bre)
    {
        delete pSerialization;
        pSerialization = 0;
        return;
    }
    xmlNodePtr pNode = pSerialization->GetRootNode();
    if(pNode == 0)
    {
        delete pSerialization;
        pSerialization = 0;
        return;
    }

    xmlNodePtr pChild = pNode->children;
    while (pChild != 0)
    {
        if(!xmlStrcmp(pChild->name, BAD_CAST"DkDlTaskItem"))
        {
            xmlChar *pType = xmlGetProp(pChild, BAD_CAST"DkDlTaskType");
            if(pType)
            {
                int _itype = atoi((const char *)pType);
                IDownloadTask *dli = 0;
                dli = DownloadTaskFactory::CreateDownloadTask((IDownloadTask::DLType)_itype);
                if(0 == dli)
                {
                    pChild = pChild->next;
                    continue;
                }
                dli->LoadTask(pChild);
                m_tasks.push_back(dli);
                xmlFree(pType);
                pType = 0;
            }

        }
        pChild = pChild->next;
    }
    delete pSerialization;
    pSerialization = 0;
}

std::string DownloaderImpl::ReplaceHiddenCharactersWithSpace(std::string _filename)
{
    char szFileName[512] = {0};
    if(_filename.length() >= 512)
    {
        return _filename;
    }
    const char *pStrSrc = _filename.data();
    char *pStrDes = szFileName;
    while (*pStrSrc)
    {
        if (*pStrSrc < 32 || *pStrSrc == ':' || *pStrSrc == '*' || *pStrSrc == '?' ||
            *pStrSrc == '/' || *pStrSrc == '\\' || *pStrSrc == '\"' || *pStrSrc == '>' ||
            *pStrSrc == '<' || *pStrSrc == '|')
        {
            *pStrDes = '_';
        }
        else
        {
            *pStrDes = *pStrSrc; 
        }
        pStrSrc++;
        pStrDes++;
    }
    *pStrDes = 0;
    return szFileName;
}

void DownloaderImpl::DoAddTask(IDownloadTask* downloadTask)
{
    {
        LockScope scope(m_lock);
        bool isInList = false;
        string _url = downloadTask->GetOrigUrl();
        string _urlid = downloadTask->GetOrigUrlID();
        string _filename = downloadTask->GetFileName();
        //unsigned int _filesize = downloadTask->GetFileSize();
        for(std::list<IDownloadTask*>::iterator itr = m_tasks.begin();
            itr != m_tasks.end();
            itr++ )
        {
            if ((*itr))
            {
                if ((*itr)->GetOrigUrlID() == _urlid)
                {
                    isInList = true;
                    if ((*itr)->GetState() & (IDownloadTask::CANCELED |
                        IDownloadTask::PAUSED |
                        IDownloadTask::UPDATED |
                        IDownloadTask::FAILED))
                    {
                        (*itr)->SetState(IDownloadTask::WAITING);
                    }
                    break;
                }
            }
        }

        if (!isInList)
        {
            if (downloadTask->IsUploadTask())
            {
            }
            else
            {
                string _nametmp(DOWNLOADPATH);
                Download::UrlFileInfo info = {0,{0},{0},true,{0},{0},{0}};
                string useragent = downloadTask->GetUserAgent();
                useragent.copy(info.szUesrAgent,RANGE_BUF_LENGTH);
                // Download::DLResult res = (Download::DLResult)downloadTask->GetFileInfoFromServer(_url,&info);

                if(strlen(info.szFileName))
                {
                    //szFileName is the Extension name.
                    string tmp1 = info.szFileName;
                    int pos = _filename.find_last_of('.');
                    //TODO check if tmp1.length()  == _filename.length() - pos
                    _filename.replace(pos,_filename.length() - pos,tmp1);
                }

                _nametmp+=ReplaceHiddenCharactersWithSpace(_filename);
                _nametmp = CheckForSameFilename(_nametmp);
                int pos = strlen(DOWNLOADPATH);
                //Update the filename if there are some duplicated files.
                _filename = _nametmp.substr(pos);

                downloadTask->SetFileName(_filename);
                downloadTask->SetUrl(info.szUrl);
            }
            downloadTask->SetCanResume(true);

            std::list<IDownloadTask*>::iterator itr = m_tasks.begin();
            for(itr = m_tasks.begin(); itr != m_tasks.end(); itr++ )
            {
                if ((*itr))
                {
                    if ((*itr)->GetState() == IDownloadTask::PAUSED)
                    {
                        m_tasks.insert(itr, downloadTask);
                        break;
                    }
                }
            }

            if(itr == m_tasks.end())
            {
                m_tasks.push_back(downloadTask);
            }
            SNativeMessage msg;
            msg.iType = KMessageDownloadAddTask;
            CNativeThread::Send(msg);
        }
    }
}

void DownloaderImpl::AddTask(IDownloadTask *pDownloadTask)
{
    if(0 == pDownloadTask)
    {
        return;
    }
    DeleteTask(pDownloadTask->GetOrigUrlID().c_str());
    DoAddTask(pDownloadTask);
    SaveHistroy();

    if(!IsPthreadWorking())
    {
        StartDownload();
    }
    else
    {
        sem_post(&m_sem);
    }
}


bool DownloaderImpl::IsDownloading(std::string _urlid)
{
    LockScope scope(m_lock);
    for(std::list<IDownloadTask *>::iterator itr=m_tasks.begin(); itr != m_tasks.end(); itr++ )
    if ((*itr))
    {
        if ((*itr)->GetOrigUrlID()== _urlid)
        {
            if ((*itr)->GetState() & (IDownloadTask::WAITING | IDownloadTask::WORKING))
            {
                return true;
            }
        }
    }
    return false;
}

void DownloaderImpl::ResumeTask(std::string _urlID)
{
    {
        LockScope scope(m_lock);
        IDownloadTask *ptask = 0;
        list<IDownloadTask*>::iterator tmpitr = m_tasks.begin();
        for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end(); itr++ )
        {
            if ((*itr) && (*itr)->GetOrigUrlID() == _urlID)
            {
                if((*itr)->GetState() & (IDownloadTask::CANCELED |
                                         IDownloadTask::PAUSED | 
                                         IDownloadTask::FAILED))
                {
                    (*itr)->SetState(IDownloadTask::WAITING);
                }
                if((*itr)->GetState() == IDownloadTask::WAITING)
                {
                    ptask = (*itr);
                    itr = m_tasks.erase(itr);

                    //move tmpitr to target position
                    for (tmpitr = m_tasks.begin(); tmpitr != m_tasks.end(); tmpitr++ )
                    {
                        if (*tmpitr)
                        {
                            IDownloadTask::DLState iState = (*tmpitr)->GetState();
                            if(iState & (IDownloadTask::WAITING |
                                         IDownloadTask::PAUSED |
                                         IDownloadTask::CANCELED))
                            {
                                break;
                            }
                        }
                    }
                    m_tasks.insert(tmpitr,ptask);
                }
                break;
            }
        }
    }

    if(!IsPthreadWorking())
    {
        StartDownload();
    }
    else
    {
        sem_post(&m_sem);
    }
}

void DownloaderImpl::PauseTask(std::string _urlID)
{
    {
        LockScope scope(m_lock);
        IDownloadTask *ptask = 0;
        for (std::list<IDownloadTask*>::iterator itr = m_tasks.begin();
             itr != m_tasks.end();
             itr++)
        {
            if ((*itr))
            {
                if ((*itr)->GetOrigUrlID() == _urlID &&
                    ((*itr)->GetState() & IDownloadTask::WORKING))
                {
                    (*itr)->StopEngine();
                    (*itr)->SetState(IDownloadTask::PAUSED);
                    ptask = *itr;
                    m_tasks.push_back(ptask);
                    m_tasks.erase(itr);
                    break;
                }
            }
        }
    }
}
void DownloaderImpl::ResumeAllPauses()
{
    {
        LockScope scope(m_lock);
        for (std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end();itr++)
        if ((*itr))
        {
            IDownloadTask::DLState iState = (*itr)->GetState();
            if (iState == IDownloadTask::PAUSED)
            {
                (*itr)->SetState(IDownloadTask::WAITING);
                DebugPrintf(DLC_XU_KAI,"DownloaderImpl::ResumeAllPauses itr->SetState(DLTaskInfo::WAITING);");
            }
        }
    }
    if(!IsPthreadWorking())
    {
        StartDownload();
    }
    else
    {
        sem_post(&m_sem);
    }
}

void DownloaderImpl::PauseAllTasks(int stopState)
{
    {
        LockScope scope(m_lock);
        for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end();itr++)
        if ((*itr))
        {
            IDownloadTask::DLState iState = (*itr)->GetState();
            if (iState == IDownloadTask::WORKING)
            {
                (*itr)->StopEngine();
                if (stopState & IDownloadTask::PAUSED)
                    (*itr)->SetState(IDownloadTask::PAUSED);
                else
                    (*itr)->SetState(IDownloadTask::CANCELED);
            }
        }
    }
}

void DownloaderImpl::CancelTask(const char* urlID)
{
    LockScope scope(m_lock);
    for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end();itr++)
    if ((*itr))
    {
        if ((*itr)->GetOrigUrlID() == urlID)
        {
            if ((*itr)->GetState() == IDownloadTask::WORKING)
            {
                (*itr)->StopEngine();
                (*itr)->SetState(IDownloadTask::CANCELED);
            }
        }
    }
}

void DownloaderImpl::ClearDone()
{
    LockScope scope(m_lock);
    for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end();)
    {
        if ((*itr))
        {
            if ((*itr)->GetState() == IDownloadTask::DONE)
            {
                delete *itr;
                *itr = 0;
                itr = m_tasks.erase(itr);
            }
            else
            {
                itr++;
            }
        }
        else
        {
            itr++;
        }
    }
}

void DownloaderImpl::ClearCanceled()
{
    LockScope scope(m_lock);

    for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end();)
    {
        if ((*itr))
        {
            if ((*itr)->GetState() == IDownloadTask::CANCELED)
            {
                delete *itr;
                *itr = 0;
                itr = m_tasks.erase(itr);
            }
            else
            {
                itr++;
            }
        }
        else
        {
            itr++;
        }
    }
}

void DownloaderImpl::SetMaxDownloadTaskNum(int _num)
{
    m_maxtasknum = _num;
}

int  DownloaderImpl::GetMaxDownloadTaskNum()
{
    return m_maxtasknum;
}

int  DownloaderImpl::GetDownloadTaskNum()
{
    LockScope scope(m_lock);
    return m_tasks.size();
}

IDownloadTask * DownloaderImpl::GetTaskInfoByUrlId(const char* urlId)
{
    LockScope scope(m_lock);
    for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end(); itr++ )
    {
        if ((*itr) && (*itr)->GetOrigUrlID() == urlId)
        {
            return (*itr);
        }
    }
    return 0;
}

IDownloadTask *DownloaderImpl::GetTaskInfoByIndex(int _index)
{
    LockScope scope(m_lock);
    int i = 0;
    for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end(); itr++ ,i++)
    if ((*itr))
    {
        if(i == _index)
        {
            return (*itr);

        }
    }
    return 0;
}


int DownloaderImpl::GetTaskPercentageByUrlId(std::string _urlID)
{
    LockScope scope(m_lock);
    for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end(); itr++ )
    if ((*itr))
    {
        if((*itr)->GetOrigUrlID() == _urlID)
        {
            return (*itr)->GetPercentage();
        }
    }
    return 0;
}

IDownloadTask::DLState DownloaderImpl::GetTaskStateByUrlId(std::string _urlID)
{
    LockScope scope(m_lock);
    for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end(); itr++ )
    if ((*itr))
    {
        if((*itr)->GetOrigUrlID() == _urlID)
        {
            return (*itr)->GetState();
        }
    }
    return IDownloadTask::NONE;
}

const std::string DownloaderImpl::GetTaskFileNameByUrlId(std::string _urlID)
{
    LockScope scope(m_lock);
    for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end(); itr++ )
    if ((*itr))
    {
        if((*itr)->GetOrigUrlID() == _urlID)
        {
            string filename = (*itr)->GetDisplayName();
            if (filename.empty())
            {
                filename = (*itr)->GetFileName();
            }
            return filename;
        }
    }
    return "";
}


bool DownloaderImpl::CheckSomeTaskIsExistByUrlId(std::string _urlID)
{
    LockScope scope(m_lock);
    for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end(); itr++ )
    if ((*itr))
    {
        if((*itr)->GetOrigUrlID() == _urlID)
        {
            return true;
        }
    }
    return false;
}

void DownloaderImpl::DeleteTasks(const int state, const int type)
{
    BOOL bExist = FALSE;
    {
        LockScope scope(m_lock);
        for(std::list<IDownloadTask*>::iterator itr = m_tasks.begin(); itr != m_tasks.end();)
        {
            if ((*itr) != 0)
            {
                IDownloadTask::DLState dl_state = (*itr)->GetState();
                IDownloadTask::DLType  dl_type  = (*itr)->GetType();
                if ((dl_state & state) && (dl_type & type))
                {
                    if(dl_state & IDownloadTask::WORKING)
                    {
                        (*itr)->StopEngine();
                    }
                    (*itr)->SetState(IDownloadTask::CANCELED);

                    if ((*itr)->GetType() != IDownloadTask::MICLOUDFILE)
                    {
                        // remove local file
                        string _szfilepath(DOWNLOADPATH);
                        _szfilepath+=(*itr)->GetFileName();
                        _szfilepath+=".dat";
                        unlink(_szfilepath.c_str());
                    }
                    delete *itr;
                    *itr = 0;
                    itr = m_tasks.erase(itr);

                    // notify listeners
                    DebugPrintf(DLC_XU_KAI,"DownloaderImpl::DeleteTask delete task");
                    bExist = TRUE;
                    continue;
                }
            }
            itr++;
        }
    }

    if(bExist)
    {
        SNativeMessage msg;
        msg.iType = KMessageDownloadDeleteTask;
        CNativeThread::Send(msg);
        SaveHistroy();
    }
}

void DownloaderImpl::DeleteTask(const char* urlId)
{
    BOOL bFindIt = FALSE;
    {
        LockScope scope(m_lock);
        for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end();)
        {
            if ((*itr))
            {
                if((*itr)->GetOrigUrlID() == urlId)
                {
                    if((*itr)->GetState() == IDownloadTask::WORKING)
                    {
                        DebugPrintf(DLC_XU_KAI,"DownloaderImpl::DeleteTask prepare to pause Download \n");
                        (*itr)->StopEngine();
                        (*itr)->SetState(IDownloadTask::CANCELED);                        
                        DebugPrintf(DLC_XU_KAI,"DownloaderImpl::DeleteTask pause Download OK\n");
                    }

                    if ((*itr)->GetType() != IDownloadTask::MICLOUDFILE)
                    {
                        string _szfilepath(DOWNLOADPATH);
                        _szfilepath+=(*itr)->GetFileName();
                        _szfilepath+=".dat";
                        DebugPrintf(DLC_XU_KAI,"DownloaderImpl::DeleteTask prepare to remove book _szfilepath.c_str() is %s",_szfilepath.c_str());
                        unlink(_szfilepath.c_str());
                        DebugPrintf(DLC_XU_KAI,"DownloaderImpl::DeleteTask remove book ok");
                    }
                    delete *itr;
                    *itr = 0;
                    m_tasks.erase(itr);
                    DebugPrintf(DLC_XU_KAI,"DownloaderImpl::DeleteTask delete task");
                    bFindIt = TRUE;
                    SNativeMessage msg;
                    msg.iType = KMessageDownloadDeleteTask;
                    CNativeThread::Send(msg);
                    break;
                }
            }
            itr++;
        }
    }

    if(bFindIt)
    {
        SaveHistroy();
    }
}

DownloadTaskNums DownloaderImpl::UpdateDownloadTaskNums(int type)
{
    int num_waiting  = 0;
    int num_working  = 0;
    int num_paused   = 0;
    int num_done     = 0;
    int num_canceled = 0;
    int num_updated  = 0;
    int num_failed   = 0;
    int num_curldone = 0;
    //int num_total    = m_tasks.size();
    {
        for(std::list<IDownloadTask*>::iterator itr = m_tasks.begin(); itr != m_tasks.end(); itr++)
        {
            if ((*itr) != 0)
            {
                IDownloadTask::DLState task_state = (*itr)->GetState();
                IDownloadTask::DLType  task_type  = (*itr)->GetType();
                if (task_type == type)
                {
                    if (task_state & IDownloadTask::WAITING)
                    {
                        num_waiting++;
                    }
                    if (task_state & IDownloadTask::WORKING)
                    {
                        num_working++;
                    }
                    if (task_state & IDownloadTask::PAUSED)
                    {
                        num_paused++;
                    }
                    if (task_state & IDownloadTask::DONE)
                    {
                        num_done++;
                    }
                    if (task_state & IDownloadTask::CANCELED)
                    {
                        num_canceled++;
                    }
                    if (task_state & IDownloadTask::UPDATED)
                    {
                        num_updated++;
                    }
                    if (task_state & IDownloadTask::FAILED)
                    {
                        num_failed++;
                    }
                    if (task_state & IDownloadTask::CURL_DONE)
                    {
                        num_curldone++;
                    }
                }
            }
        }
    }

    DownloadTaskNums download_tasks;
    download_tasks[IDownloadTask::WAITING]  = num_waiting;
    download_tasks[IDownloadTask::WORKING]  = num_working;
    download_tasks[IDownloadTask::PAUSED]   = num_paused;
    download_tasks[IDownloadTask::DONE]     = num_done;
    download_tasks[IDownloadTask::CANCELED] = num_canceled;
    download_tasks[IDownloadTask::UPDATED]  = num_updated;
    download_tasks[IDownloadTask::FAILED]   = num_failed;
    download_tasks[IDownloadTask::CURL_DONE]= num_curldone;
    return download_tasks;
}

// unprotected
AllDownloadTaskNums DownloaderImpl::UpdateAllDownloadTaskNums()
{
    LockScope scope(m_lock);
    m_downloadTaskNums.clear();
    m_downloadTaskNums[IDownloadTask::BOOK] = UpdateDownloadTaskNums(IDownloadTask::BOOK);
    m_downloadTaskNums[IDownloadTask::UPGRADEPACKAGE] = UpdateDownloadTaskNums(IDownloadTask::UPGRADEPACKAGE);
    m_downloadTaskNums[IDownloadTask::SCREENSAVER] = UpdateDownloadTaskNums(IDownloadTask::SCREENSAVER);
    m_downloadTaskNums[IDownloadTask::MICLOUDFILE] = UpdateDownloadTaskNums(IDownloadTask::MICLOUDFILE);
    return m_downloadTaskNums;
}

// unprotected
int DownloaderImpl::GetWorkingTaskNumber()
{
    LockScope scope(m_lock);
    int count = 0;
    for(std::list<IDownloadTask*>::iterator itr = m_tasks.begin();
        itr != m_tasks.end();
        itr++)
    {
        if ((*itr))
        {
            if ((*itr)->GetState() == IDownloadTask::WORKING)
            {
                count++;
            }
        }
    }
    return count;
}


//If task is partially done, but no files there, clean them.
void DownloaderImpl::ClearNotExisted()
{
    LockScope scope(m_lock);
    for(std::list<IDownloadTask*>::iterator itr=m_tasks.begin(); itr != m_tasks.end();itr++)
    if ((*itr))
    {
        if ((*itr)->GetState() & (IDownloadTask::CANCELED | IDownloadTask::WORKING | IDownloadTask::DONE | IDownloadTask::PAUSED))
        {
            ifstream file((*itr)->GetFileName().c_str());
            if (!file.is_open())
            {
                delete *itr;
                *itr = 0;
                itr = m_tasks.erase(itr);
                continue;
            }
        }
    }
}

void DownloaderImpl::ClearTask(const int state, const int type)
{
    LockScope scope(m_lock);
    for(std::list<IDownloadTask*>::iterator itr = m_tasks.begin(); itr != m_tasks.end(); itr++)
    if ((*itr))
    {
        DebugPrintf(DLC_DIAGNOSTIC, "task: %s, state: %d, type: %d", (*itr)->GetFileName().c_str(),
            (*itr)->GetState(), (*itr)->GetType());
        IDownloadTask::DLState task_state = (*itr)->GetState();
        IDownloadTask::DLType  task_type  = (*itr)->GetType();
        if ((task_state & state) && (task_type & type))
        {
            DebugPrintf(DLC_DIAGNOSTIC, "DeleteTask %s", (*itr)->GetFileName().c_str());
            ifstream file((*itr)->GetFileName().c_str());
            if (!file.is_open())
            {
                delete *itr;
                *itr = 0;
                itr = m_tasks.erase(itr);
                continue;
            }
        }
    }
}

void DownloaderImpl::StartDownload()
{
    int err  = ThreadHelper::CreateThread(&m_taskThread,
                                          DownloaderImpl::TaskQueueThread,
                                          this,
                                          "DownloaderImpl @ TaskQueueThread",
                                          true,
                                          THREAD_STACK_SIZE_KINDLE_DEFAULT);
    if (!err)
    {
        this->m_threadWorking = true;
    }
}

void DownloaderImpl::StopDownload(int stopState)
{
    if(m_taskThread)
    {
        ThreadHelper::CancelThread(m_taskThread);
        ThreadHelper::JoinThread(m_taskThread, 0);
        m_taskThread = 0 ;

        m_threadWorking = false;
        PauseAllTasks(stopState);
        PowerManagement::DKPowerManagerImpl::GetInstance()->ReleaseLock(PM_LOCK_SCREEN, MODULE_DOWNLOAD);
    }

}

void* DownloaderImpl::RunTask()
{
    PowerManagement::DKPowerManagerImpl::GetInstance()->AccquireLock(PM_LOCK_SCREEN, MODULE_DOWNLOAD);
    while(true)
    {
        IDownloadTask* work_item = 0;
        unsigned int nPriority = 0;
        bool _working = false;
        bool bHasFinished = false;
        pthread_testcancel();
        if(GetWorkingTaskNumber() >= GetMaxDownloadTaskNum())
        {
            sleep(1);
            continue;
        }

        {
            LockScope scope(m_lock);
            for(list<IDownloadTask *>::iterator itr=m_tasks.begin(); itr != m_tasks.end(); itr++)
            {
                if ((*itr) != 0)
                {
                    if (((*itr)->GetState() & IDownloadTask::WAITING) &&
                        (*itr)->GetPriority() > nPriority)
                    {
                        //get the download task now
                        work_item = (IDownloadTask*)(*itr);
                        nPriority = (*itr)->GetPriority();
                    }
                    else if ((*itr)->GetState() & IDownloadTask::WORKING)
                    {
                        _working = true;
                    }
                    else if ((*itr)->GetState() & IDownloadTask::CURL_DONE)
                    {
                        (*itr)->SetState(IDownloadTask::DONE);
                        (*itr)->StopEngine();
                        (*itr)->FinishTask();
                        bHasFinished = true;
                    }
                    else if ((*itr)->GetState() & (IDownloadTask::CANCELED |
                                                   IDownloadTask::FAILED |
                                                   IDownloadTask::PAUSED))
                    {
                        // do nothing
                    }
                }
            }

            if(work_item != 0)
            {
                work_item->SetState(IDownloadTask::WORKING);
                work_item->StartEngine();
                //continue;
            }
        }

        if (work_item == 0)
        {
            if(bHasFinished)
            {
                SaveHistroy();
            }
            if(!_working)
            {
                sem_init(&(m_sem),0,0);
                PowerManagement::DKPowerManagerImpl::GetInstance()->ReleaseLock(PM_LOCK_SCREEN, MODULE_DOWNLOAD);
                sem_wait(&(m_sem));
                PowerManagement::DKPowerManagerImpl::GetInstance()->AccquireLock(PM_LOCK_SCREEN, MODULE_DOWNLOAD);
            }
        }
        usleep(10000);
    }
    return 0;
}

void* DownloaderImpl::TaskQueueThread(void *pV)
{
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
    DownloaderImpl* pPM = (DownloaderImpl*)pV;
    if(0 == pPM)
    {
        return 0;
    }
    return pPM->RunTask();
}

IDownloader* DownloaderImpl::GetInstance()
{
    static DownloaderImpl s_instance;
    return &s_instance;
}

IDownloader* IDownloader::GetInstance()
{
    return DownloaderImpl::GetInstance();
}

bool DownloaderImpl::CanTaskResume(std::string _urlID)
{
    LockScope scope(m_lock);
    for(std::list<IDownloadTask*>::const_iterator itr=m_tasks.begin(); itr != m_tasks.end(); itr++ )
    {
        if ((*itr))
        {
            if((*itr)->GetOrigUrlID() == _urlID)
            {
                return (*itr)->CanResume();
            }
        }
    }

    DebugPrintf(DLC_DIAGNOSTIC, "ERROR: no task found for url: %s", _urlID.c_str());
    return false;
}

bool DownloaderImpl::IsUploadTask(std::string _urlID)
{
    LockScope scope(m_lock);
    for(std::list<IDownloadTask*>::const_iterator itr=m_tasks.begin(); itr != m_tasks.end(); itr++ )
    {
        if ((*itr))
        {
            if((*itr)->GetOrigUrlID() == _urlID)
            {
                return (*itr)->IsUploadTask();
            }
        }
    }

    DebugPrintf(DLC_DIAGNOSTIC, "ERROR: no task found for url: %s", _urlID.c_str());
    return false;
}
