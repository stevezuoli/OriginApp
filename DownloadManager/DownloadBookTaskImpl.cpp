#include "DownloadManager/DownloadBookTaskImpl.h"
#include "DownloadManager/DownloaderImpl.h"
#include "BookStore/BookStoreInfoManager.h"
#include "Framework/CNativeThread.h"
#include "Utility/PathManager.h"
#include "../Common/FileManager_DK.h"
#include "Common/File_DK.h"
#include "dkBaseType.h"
#include "interface.h"
#include "Utility.h"
#include <string>
#include "I18n/StringManager.h"
using namespace std;

const std::string DownloadBookTaskImpl::TASKFILENAME = "DkDlTaskFileName";
const std::string DownloadBookTaskImpl::TASKURL = "DkDlTaskUrl";
const std::string DownloadBookTaskImpl::TASKORIGURL = "DkDlTaskOrigUrl";
const std::string DownloadBookTaskImpl::TASKPERCENTAGE = "DkDlTaskPercentage";
const std::string DownloadBookTaskImpl::TASKFILESIZE = "DkDlTaskFileSize";
const std::string DownloadBookTaskImpl::TASKLASTOFFSET = "DkDlTaskLastOffset";
const std::string DownloadBookTaskImpl::TASKCANRESUME = "DkDlTaskCanResume";
const std::string DownloadBookTaskImpl::TASKSTATE = "DkDlTaskState";
const std::string DownloadBookTaskImpl::TASKORIGURLID = "DkDlTaskOrigUrlId";
const std::string DownloadBookTaskImpl::TASKTYPE = "DkDlTaskType";
const std::string DownloadBookTaskImpl::TASKBOOKSOUCE = "DkDlTaskSource";
const std::string DownloadBookTaskImpl::TASKBOOKAUTHOR = "DkDlTaskAuthor";
const std::string DownloadBookTaskImpl::TASKPRIORITY = "DkDlTaskPriority";
const std::string DownloadBookTaskImpl::TASKDESCRIPTION = "DkDlTaskDescription";
const std::string DownloadBookTaskImpl::TASKPASSWORD = "DkDlTaskPassword";
const std::string DownloadBookTaskImpl::TASKUSERAGENT = "DkDlTaskUserAgent";
const std::string DownloadBookTaskImpl::TASKMOVETO = "DkDlTaskMoveTo";
const std::string DownloadBookTaskImpl::TASKDISPLAYNAME = "DkDlTaskDisplayName";

std::string DownloadBookTaskImpl::GetFileName() const
{
    return m_fileName;
}

void DownloadBookTaskImpl::SetFileName(std::string _filename)
{
    m_fileName = _filename;
}

std::string DownloadBookTaskImpl::GetUrl() const
{
    return url;
}

void DownloadBookTaskImpl::SetUrl(std::string _url)
{
    url = _url;
}

int DownloadBookTaskImpl::GetPercentage() const
{
    if (GetFileSize() == 0)
    {
        return 0;
    }
    return GetLastOffSet() * 100 / GetFileSize();
}


IDownloadTask::DLState DownloadBookTaskImpl::GetState() const
{
    return state;
}

void DownloadBookTaskImpl::SetState(IDownloadTask::DLState _state)
{
    //if (state != _state)
    //    FireDownloadStateUpdateEvent();
    state = _state;
}

IDownloadTask::DLType DownloadBookTaskImpl::GetType() const
{
    return m_type;
}
void DownloadBookTaskImpl::SetType(IDownloadTask::DLType _type)
{
    m_type = _type;
}

void DownloadBookTaskImpl::SaveTask(DlSerializationParentsNode& _parentsnode)
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
    _parentsnode.AddChildNode(TASKBOOKSOUCE,GetSource());
    _parentsnode.AddChildNode(TASKBOOKAUTHOR,m_author);
    _parentsnode.AddChildNode(TASKDESCRIPTION,m_description);
    _parentsnode.AddChildNode(TASKPASSWORD,m_password);
    _parentsnode.AddChildNode(TASKUSERAGENT,m_UserAgent);
    _parentsnode.AddChildNode(TASKPRIORITY,m_nPriority);
    _parentsnode.AddChildNode(TASKMOVETO,m_moveTo);
    _parentsnode.AddChildNode(TASKDISPLAYNAME,m_displayName);
}

void DownloadBookTaskImpl::LoadTask(xmlNodePtr pNode)
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
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKBOOKSOUCE.c_str())))
        {
            xmlChar *pSource = xmlNodeGetContent(pNode);
            if(pSource)
            {
                SetSource((const char *)pSource);
                xmlFree(pSource);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKBOOKAUTHOR.c_str())))
        {
            xmlChar *pAuthor = xmlNodeGetContent(pNode);
            if(pAuthor)
            {
				string strAuthor((const char *)pAuthor);
				LPCSTR pcAuthor = StringManager::GetPCSTRById(BOOK_AUTHOR);
				string::size_type tAuthor = strAuthor.find(pcAuthor);
				if(string::npos != tAuthor)
				{
					strAuthor = strAuthor.replace(tAuthor, tAuthor+strlen(pcAuthor), "");
				}
                SetArtist(strAuthor.c_str());
                xmlFree(pAuthor);
            }
        }
        else if(!xmlStrcmp(pNode->name,BAD_CAST(TASKDESCRIPTION.c_str())))
        {
            xmlChar *pDescription = xmlNodeGetContent(pNode);
            if(pDescription)
            {
                SetDescription((const char *)pDescription);
                xmlFree(pDescription);
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

int DownloadBookTaskImpl::FinishTask()
{
    CDKFileManager *fileManager = CDKFileManager::GetFileManager();
	if(!fileManager)
	{
		return 0;
	}

    SNativeMessage msg;
    msg.iType = KMessageDownloadStatusUpdate;
    CNativeThread::Send(msg);

    string _tmpname (DOWNLOADPATH);
    string _sztmp1 = GetFileName();
    _tmpname+=_sztmp1;

    DkFileFormat eFormat = GetFileFormatbyExtName(_sztmp1.c_str());
    if(eFormat == DFF_ZipFile || eFormat == DFF_RoshalArchive)
    {
        size_t iPos = _tmpname.find_last_of('.');
        if(iPos == string::npos)
        {
            return false;
        }

        string strOutputPath = _tmpname.substr(0,iPos);
        strOutputPath += '/';
        DebugPrintf(DLC_XU_KAI,"OutputPath is : %s",strOutputPath.c_str());
        //CUtility::UnpackArchive(_tmpname.c_str(),strOutputPath.c_str(),GetFilePassword().c_str());

        DirInfo* DirList = NULL;
        DirInfo* pTemp   = NULL;
        DirList =ScanFileOnDisk(&DirList, strOutputPath.c_str(), GetFileFormatbyExtName, TRUE);
        if( NULL == DirList)
        {
            return false;
        }

        while(DirList)
        {
            if(DirList->FileCategory == DFC_Book)
            {
                CDKBook* pBookFileInfo = DK_FileFactory::CreateBookFileInfoFromDownload(DirList->FilePath,
                    GetArtist().c_str(),
                    WEBSERVER,
                    GetDescription().c_str(),
                    GetFilePassword().c_str(),
                    DirList->FileSize);

                fileManager->AddFile(pBookFileInfo);

            }
            pTemp = DirList;
            DirList = DirList->next;
            free(pTemp);
            pTemp = NULL;
        }
    }
    else
    {
        DebugPrintf(DLC_XU_KAI,"GetArtist().c_str() utf ------------------ 8: %s\n",this->GetArtist().c_str());
        if(!m_moveTo.empty())
        {
            rename(_tmpname.c_str(),m_moveTo.c_str());
            _tmpname = m_moveTo;
        }

        CDKBook* pBookFileInfo = DK_FileFactory::CreateBookFileInfoFromDownload(_tmpname.c_str(),
            GetArtist().c_str(),
            WEBSERVER,
            GetDescription().c_str(),
            GetFilePassword().c_str(),
            GetFileSize());

        size_t pos = _tmpname.rfind('/');
        if (pos != string::npos)
        {
            _tmpname = _tmpname.substr(0,pos);
            if (_tmpname == dk::utility::PathManager::GetBookStorePath())
            {
                dk::bookstore::BookStoreInfoManager* bookstoreManager = dk::bookstore::BookStoreInfoManager::GetInstance();
                string bookName = GetFileName();
                size_t pos = bookName.find('.');
                string bookId = "";
                if (pos != string::npos)
                {
                    bookId = bookName.substr(0,pos);
                }
                bookName = bookstoreManager->GetLocalFileTitle(bookId.c_str());
                bool isTrial = bookstoreManager->IsLocalFileTrial(bookId.c_str());
                pBookFileInfo->SetIsTrialBook(isTrial);
                pBookFileInfo->SetFileSource(DUOKAN_BOOKSTORE);
                pBookFileInfo->SetFileName(bookName.c_str());
                pBookFileInfo->SetFileArtist(bookstoreManager->GetLocalFileAuthor(bookId.c_str()).c_str());
                pBookFileInfo->SetBookRevision(bookstoreManager->GetLocalFileRevision(bookId.c_str()).c_str());
            }

        }

        fileManager->AddFile(pBookFileInfo);
        fileManager->SortFile(DFC_Book);
        fileManager->FireFileListChangedEvent();
    }
    FireDownloadProgressUpdateEvent();
    return 1;
}

void DownloadBookTaskImpl::SetDescription(std::string _des)
{
    m_description = _des;
}

std::string DownloadBookTaskImpl::GetDescription()
{
    return m_description;
}

void DownloadBookTaskImpl::SetSource(std::string _source)
{
    m_source = _source;
}

std::string DownloadBookTaskImpl::GetSource()
{
    return m_source;
}

void DownloadBookTaskImpl::SetArtist(std::string _artist)
{
    m_author = _artist;
}

std::string DownloadBookTaskImpl::GetArtist()
{
    return m_author;
}

