#include "interface.h"
#include "Utility/XmlUtil.h"
#include "Utility/PathManager.h"
#include "Utility/ReaderUtil.h"
#include "Utility/FileSystem.h"
#include "CommonUI/UIUtility.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/OfflineBookStoreManager.h"
#include "BookStore/BookStoreTypes.h"
#include "Common/CAccountManager.h"
#include "Common/File_DK.h"
#include "Common/FileManager_DK.h"
#include "KernelVersion.h"
#include "Wifi/WifiManager.h"
#include <algorithm>
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"

using namespace std;
using namespace dk::utility;
using namespace dk::bookstore;
using namespace dk::bookstore::model;
using dk::account::CAccountManager;
using dk::account::AccountEventArgs;

namespace dk
{

namespace bookstore
{

const char* DK_OFFLINE_MANAGER_CORRECTION_VERSIONS = "DK-OFFLINE-MANAGER-CORRECTION-1.0";

const char* OfflineBookStoreManager::CORRECTION_ITEM = "CorrectionItem";
const char* OfflineBookStoreManager::BOOK_ITEM = "BookItem";
const char* OfflineBookStoreManager::LOCAL_CART_ITEM = "LocalCartItem";
const char* OfflineBookStoreManager::LOCAL_FAVOURITES_ITEM = "LocalFavouritesItem";

template<class Queue>
static bool SaveToXML(const Queue& queue,
                      const DK_CHAR* rootName,
                      const char* itemName,
                      const DK_CHAR* path,
                      pthread_mutex_t& lock)
{
    XMLDomDocument *pDocument = XMLDomDocument::CreateDocument();
    if(NULL == pDocument)
    {
        return false;
    }

    XMLDomNode TmpNode;
    XMLDomNode *pDomNode = pDocument->ToDomNode();
    XMLDomNode RootNode;
    pDomNode->AddRootElement(rootName);
    pDomNode->GetLastChild(&RootNode);

    // Add ReadingData
    pthread_mutex_lock(&lock);
    DK_ReturnCode rc;
    XMLDomNode child;
    for (DK_AUTO(iter, queue.begin()); iter != queue.end(); ++iter)
    {
        RootNode.AddElement(itemName);
        rc = RootNode.GetLastChild(&child);
        if(rc == DKR_OK)
        {
            (*iter)->WriteToXML(child);
        }
    }
    pDocument->SaveXmlToFile(path);
    pthread_mutex_unlock(&lock);
    XMLDomDocument::ReleaseDocument(pDocument);
    return true;
}

template<class Queue>
static bool LoadFromXML(Queue& queue,
                        const char* itemName,
                        const DK_CHAR* path,
                        pthread_mutex_t& lock)
{
    XMLDomDocument* pDocument = XMLDomDocument::CreateDocument();
    if (NULL == pDocument)
    {
        return false;
    }

    pthread_mutex_lock(&lock);
    if (!pDocument->LoadXmlFromFile(path, false))
    {
        XMLDomDocument::ReleaseDocument(pDocument);
        pthread_mutex_unlock(&lock);
        return false;
    }

    XMLDomNode* pRootNode = pDocument->RootElement();
    if (!pRootNode)
    {
        XMLDomDocument::ReleaseDocument(pDocument);
        pthread_mutex_unlock(&lock);
        return false;
    }

    DK_INT count = 0;
    pRootNode->GetChildrenCount(&count);
    for (DK_INT i = 0; i < count; ++i)
    {
        XMLDomNode TmpNode;
        pRootNode->GetChildByIndex(i, &TmpNode);
        if (strcmp(TmpNode.GetNodeValue(), itemName) == 0)
        {
            OfflineBookStoreTaskPtr task(new OfflineBookStoreTask);
            task->ReadFromXml(TmpNode);
            queue.push_back(task);
        }
    }

    pthread_mutex_unlock(&lock);
    XMLDomDocument::ReleaseDocument(pDocument);
    return true;
}

OfflineBookStoreTask::OfflineBookStoreTask()
{
    m_mInfoMap.clear();
}
OfflineBookStoreTask::~OfflineBookStoreTask()
{
    m_mInfoMap.clear();
}

bool OfflineBookStoreTask::SetInfo(const std::string& _strName,const std::string& _strValue)
{
    if(_strName.empty() || _strValue.empty())
    {
        return false;
    }
    m_mInfoMap[_strName] = _strValue;
    return true;
}

string OfflineBookStoreTask::GetInfo(const std::string& _strName) const
{
    if(_strName.empty())
    {
        return "";
    }
    map<string,string>::const_iterator  itr = m_mInfoMap.find(_strName);
    if(itr == m_mInfoMap.end())
    {
        return "";
    }
    return itr->second;
}


bool OfflineBookStoreTask::WriteToXML(XMLDomNode& xmlNode) const
{
    for(map<string,string>::const_iterator itr = m_mInfoMap.begin();
        itr != m_mInfoMap.end(); itr++)
    {
        AddAtomSubNode(xmlNode, itr->first, itr->second);
    }
    return true;
}
bool OfflineBookStoreTask::ReadFromXml(const XMLDomNode& xmlNode)
{
    XMLDomNode TmpNode;
    if(DKR_OK == xmlNode.GetFirstElementChild(&TmpNode))
    {
        do
        {
            string strTempValue = "";
            if(DK_NULL == TmpNode.GetNodeValue())
            {
                continue;
            }

            if(DKR_OK == ReadAtomNodeValue(TmpNode,strTempValue) && !strTempValue.empty())
            {
                m_mInfoMap[TmpNode.GetNodeValue()] = strTempValue;
            }
        }while(TmpNode.GetNextSibling(&TmpNode) == DKR_OK);
    }

    return true;
}


// OfflineBookStoreManager
OfflineBookStoreManager::OfflineBookStoreManager()
    : m_strCorrectionPath(PathManager::GetBookStoreCorrectionPath())
    , m_strBooksWaitUploadPath(PathManager::GetBookWaitUploadNotePath())
    , m_strCartPath(PathManager::GetBookStoreCartPath())
    , m_strFavouritesPath(PathManager::GetBookStoreFavouritesPath())
    , m_offlineFavouriteBooks(new DataUpdateArgs)
    , m_offlineCartBooks(new DataUpdateArgs)
{
    pthread_mutex_init(&m_cacheLock, 0);
    pthread_mutex_init(&m_bookLock, 0);
    pthread_mutex_init(&m_cartBooksLock, 0);
    pthread_mutex_init(&m_favouriteBooksLock, 0);

    Load();
    SubscribeMessageEvent(CAccountManager::EventAccount, *CAccountManager::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&OfflineBookStoreManager::StartSendBookNotes),
            this,
            std::tr1::placeholders::_1));
}

OfflineBookStoreManager::~OfflineBookStoreManager()
{
    pthread_mutex_destroy(&m_cacheLock);
    pthread_mutex_destroy(&m_bookLock);
    pthread_mutex_destroy(&m_cartBooksLock);
    pthread_mutex_destroy(&m_favouriteBooksLock);
}

OfflineBookStoreManager* OfflineBookStoreManager::GetInstance()
{
    static OfflineBookStoreManager offlineManager;
    return &offlineManager;
}

bool OfflineBookStoreManager::ReportedBookBug(const char* bookId,
                                              const char* bookName,
                                              const char* bookRevision,
                                              const char* position,
                                              const char* ref_chapter,
                                              const char* reference,
                                              const char* suggestion,
                                              bool bConvertCht)
{
    if (0 == bookId || 0 == bookName || 0 == bookRevision || 0 == reference || 0 == suggestion)
    {
        return false;
    }

    OfflineBookStoreTaskPtr task(new OfflineBookStoreTask);
    task->SetInfo("bookid", bookId);
    task->SetInfo("bookname", bookName);
    task->SetInfo("revision", bookRevision);
    task->SetInfo("position", position);
    task->SetInfo("ref_chapter", ref_chapter);
    task->SetInfo("reference", reference);
    task->SetInfo("suggestion", suggestion);
    task->SetInfo("convertcht", bConvertCht ? "1" : "0");
    //task->SetInfo("userid", GetCurrentUserName().c_str()); //Do we need to save user id?

    {
        AutoLock lock(&m_cacheLock);
        m_offlineCorrectionTasks.push_back(task);
    }

    //DebugPrintf(DLC_DIAGNOSTIC, "\n Report new correction:");
    //DebugPrintf(DLC_DIAGNOSTIC, "\n bookId:%s", bookId);
    //DebugPrintf(DLC_DIAGNOSTIC, "\n bookName:%s", bookName);
    //DebugPrintf(DLC_DIAGNOSTIC, "\n bookRevision:%s", bookRevision);
    //DebugPrintf(DLC_DIAGNOSTIC, "\n position:%s", position);
    //DebugPrintf(DLC_DIAGNOSTIC, "\n ref_chapter:%s", ref_chapter);
    //DebugPrintf(DLC_DIAGNOSTIC, "\n reference:%s", reference);
    //DebugPrintf(DLC_DIAGNOSTIC, "\n suggestion:%s \n", suggestion);

    // Save all of the corrections per request?
    return SaveCorrections();
}

bool OfflineBookStoreManager::RetrieveBookBug(std::string& bookId,
                                            std::string& bookName,
                                            std::string& bookRevision,
                                            std::string& position,
                                            std::string& ref_chapter,
                                            std::string& reference,
                                            std::string& suggestion,
                                            bool& bConvertCht)
{
    AutoLock lock(&m_cacheLock);
    if (m_offlineCorrectionTasks.empty())
    {
        return false;
    }

    OfflineBookStoreTaskPtr task = m_offlineCorrectionTasks.front();
    bookId = task->GetInfo("bookid");
    bookName = task->GetInfo("bookname");
    bookRevision = task->GetInfo("revision");
    position = task->GetInfo("position");
    ref_chapter = task->GetInfo("ref_chapter");
    reference = task->GetInfo("reference");
    suggestion = task->GetInfo("suggestion");
    std::string isConvert = task->GetInfo("convertcht");
    bConvertCht = isConvert.compare("1") == 0 ? true : false;

    m_offlineCorrectionTasks.pop_front();
    return true;
}

bool OfflineBookStoreManager::Send()
{
    return SendCorrections();
}

bool OfflineBookStoreManager::Save()
{
    SaveCorrections();
    SaveLocalCart();
    SaveLocalFavourites();
    return true;
}

bool OfflineBookStoreManager::Load()
{
    LoadCorrections();
    LoadBooksWaitUploadNewNote();
    LoadLocalCart();
    LoadLocalFavourites();
    return true; 
}

bool OfflineBookStoreManager::SendCorrections()
{
    if (!LoadCorrections())
    {
        return false;
    }

    if (m_offlineCorrectionTasks.empty())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "\n Empty Correction Queue! \n");
        return true;
    }

    if (!UIUtility::CheckAndReConnectWifi())
    {
        return false;
    }

    std::string bookId;
    std::string bookName;
    std::string bookRevision;
    std::string position;
    std::string ref_chapter;
    std::string reference;
    std::string suggestion;
    bool        bConvertCht;
    while (RetrieveBookBug(bookId, bookName, bookRevision, position, ref_chapter, reference, suggestion, bConvertCht))
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "\n Sending cached correction:");
        //DebugPrintf(DLC_DIAGNOSTIC, "bookId:%s", bookId.c_str());
        //DebugPrintf(DLC_DIAGNOSTIC, "bookName:%s", bookName.c_str());
        //DebugPrintf(DLC_DIAGNOSTIC, "bookRevision:%s", bookRevision.c_str());
        //DebugPrintf(DLC_DIAGNOSTIC, "position:%s", position.c_str());
        //DebugPrintf(DLC_DIAGNOSTIC, "ref_chapter:%s", ref_chapter.c_str());
        //DebugPrintf(DLC_DIAGNOSTIC, "reference:%s", reference.c_str());
        //DebugPrintf(DLC_DIAGNOSTIC, "suggestion:%s \n", suggestion.c_str());
        BookStoreInfoManager* pBSInfoManager = BookStoreInfoManager::GetInstance();
        pBSInfoManager->ReportBookBug(bookId.c_str(),
            bookName.c_str(),
            bookRevision.c_str(),
            position.c_str(),
            ref_chapter.c_str(),
            reference.c_str(),
            suggestion.c_str());
    }

    // Clear file
    SaveCorrections();
    return true;
}

bool OfflineBookStoreManager::SaveCorrections()
{
    return SaveToXML<std::deque<OfflineBookStoreTaskPtr> >(m_offlineCorrectionTasks,
                                                           "DK_CORRECTION_ROOT",
                                                           CORRECTION_ITEM,
                                                           m_strCorrectionPath.c_str(),
                                                           m_cacheLock);
}

bool OfflineBookStoreManager::LoadCorrections()
{
    return LoadFromXML<std::deque<OfflineBookStoreTaskPtr> >(m_offlineCorrectionTasks,
                                                             CORRECTION_ITEM,
                                                             m_strCorrectionPath.c_str(),
                                                             m_cacheLock);
}

bool OfflineBookStoreManager::AddBookWaitUpload(const char* bookId,
                                                const char* bookName,
                                                const char* bookRevision)
{
    if (bookId && bookName && bookRevision)
    {
        if (!IsBookInWaitUploadQueue(bookId, bookName))
        {
            OfflineBookStoreTaskPtr bookInfo(new OfflineBookStoreTask);
            if (bookInfo)
            {
                bookInfo->SetInfo("BookId", bookId);
                bookInfo->SetInfo("BookName", bookName);
                bookInfo->SetInfo("BookRevision", bookRevision);
                pthread_mutex_lock(&m_bookLock);
                m_booksWaitUpload.push_back(bookInfo);
                pthread_mutex_unlock(&m_bookLock);
                return SaveBooksWaitUploadNewNote();
            }
        }

    }

    return false;
}

bool OfflineBookStoreManager::DelBookWaitUpload(const char* bookId)
{
    if (bookId)
    {
        DK_AUTO(iter, m_booksWaitUpload.begin());
        for (; iter != m_booksWaitUpload.end(); ++iter)
        {
            if ((*iter)->GetInfo("BookId").compare(bookId) == 0)
                break;
        }

        if (iter != m_booksWaitUpload.end())
        {
            pthread_mutex_lock(&m_bookLock);
            //DebugPrintf(DLC_DIAGNOSTIC, "OfflineBookStoreManager::DelBookWaitUpload, %s, %s",
            //        (*iter)->GetInfo("BookId").c_str(), (*iter)->GetInfo("BookName").c_str());
            m_booksWaitUpload.erase(iter);
            pthread_mutex_unlock(&m_bookLock);
            return SaveBooksWaitUploadNewNote();
        }
    }

    return false;
}

bool OfflineBookStoreManager::LoadBooksWaitUploadNewNote()
{
    bool loadedSuccess = LoadFromXML<std::vector<OfflineBookStoreTaskPtr> >(m_booksWaitUpload,
                                                              BOOK_ITEM,
                                                              m_strBooksWaitUploadPath.c_str(),
                                                              m_bookLock);
    std::reverse(m_booksWaitUpload.begin(), m_booksWaitUpload.end());
    return loadedSuccess;
}

bool OfflineBookStoreManager::SaveBooksWaitUploadNewNote()
{
    return SaveToXML<std::vector<OfflineBookStoreTaskPtr> >(m_booksWaitUpload,
                                                            "DK_BOOK_ROOT",
                                                            BOOK_ITEM,
                                                            m_strBooksWaitUploadPath.c_str(),
                                                            m_bookLock);
}

bool OfflineBookStoreManager::SendBooksWaitUploadNewNote()
{
    if (m_booksWaitUpload.empty())
    {
        return true;
    }

    //DebugPrintf(DLC_DIAGNOSTIC, "OfflineBookStoreManager::SendBooksWaitUploadNewNote");
    pthread_mutex_lock(&m_bookLock);
    OfflineBookStoreTaskPtr bookInfo = m_booksWaitUpload.back();
    pthread_mutex_unlock(&m_bookLock);
    while (!GenerateAndPostReadingBookInfo(bookInfo->GetInfo("BookId").c_str(), bookInfo->GetInfo("BookRevision").c_str()))
    {
        DelBookWaitUpload(bookInfo->GetInfo("BookId").c_str());
        if (m_booksWaitUpload.empty())
        {
            return true;
        }
        bookInfo = m_booksWaitUpload.back();
    }

    SaveBooksWaitUploadNewNote();
    return true;
}

bool OfflineBookStoreManager::IsBookInWaitUploadQueue(const char* bookId, const char* bookName)
{
    for (DK_AUTO(iter, m_booksWaitUpload.begin()); iter != m_booksWaitUpload.end(); ++iter)
    {
        if ((*iter)->GetInfo("BookId").compare(bookId) == 0)
         //&& (*iter)->GetInfo("BookName").compare(bookName) == 0)
        {
            return true;
        }
    }

    return false;
}
    
bool OfflineBookStoreManager::StartSendBookNotes(const EventArgs& args)
{
    const AccountEventArgs& accountEventArgs = (const AccountEventArgs&)args;
    if (accountEventArgs.IsLoggedIn())
    {
        SendBooksWaitUploadNewNote();
    }

    return true;
}

bool OfflineBookStoreManager::GenerateAndPostReadingBookInfo(const char* bookId, const char* revision)
{
    if (!bookId || !revision)
    {
        return false;
    }
    //DebugPrintf(DLC_DIAGNOSTIC, "OfflineBookStoreManager::GenerateAndPostReadingBookInfo %s, %s", bookId, revision);
    PCDKFile fileInfo = CDKFileManager::GetFileManager()->GetFileByBookId(bookId);
    if (!fileInfo)
    {
        //file removed?
        DebugPrintf(DLC_DIAGNOSTIC, "No FileInfo, %s", bookId);
        DelBookWaitUpload(bookId);
        return false;
    }
    else
    {
        ReadingBookInfo readingDataInfo;
        readingDataInfo.SetKernelVersion(DK_GetKernelVersion());
        readingDataInfo.SetVersion(ReadingBookInfo::CURRENT_VERSION);
        readingDataInfo.SetBookId(fileInfo->GetBookID());
        readingDataInfo.SetBookRevision(revision);
        //DebugPrintf(DLC_DIAGNOSTIC, "FilePath: %s, FileName: %s, FileAbstract: %s"
        //        ,fileInfo->GetFilePath(), fileInfo->GetFileName(), fileInfo->GetFileAbstract());

#ifdef KINDLE_FOR_TOUCH
        DKXBlock* dkxBlock= DKXManager::CreateDKXBlock(fileInfo->GetFilePath());
        if (NULL == dkxBlock || !dkxBlock->IsReadingDataModified())
        {
            DebugPrintf(DLC_DIAGNOSTIC, "No dkxBlock or NoModified, dkxBlock: %x", dkxBlock);
            return false;
        }

        vector<ICT_ReadingDataItem *> vBookmarks;
        if(!dk::utility::ReaderUtil::GetAllBookmarksForUpgrade(dkxBlock, &vBookmarks, fileInfo->GetFileFormat()))
        {
            DebugPrintf(DLC_DIAGNOSTIC, "ReaderUtil::GetAllBookmarksForUpgrade Error!");
            return false;
        }

        int dataVersion = dkxBlock->GetDataVersion();
#else
        DKXManager* pDKXManager = DKXManager::GetInstance();
        const string filePath = fileInfo->GetFilePath();
        if (NULL == pDKXManager || !pDKXManager->IsReadingDataModified(filePath))
        {
            return false;
        }

        vector<ICT_ReadingDataItem *> vBookmarks;
        if(!dk::utility::ReaderUtil::GetAllBookmarksForUpgrade(filePath, &vBookmarks, fileInfo->GetFileFormat()))
        {
            DebugPrintf(DLC_DIAGNOSTIC, "ReaderUtil::GetAllBookmarksForUpgrade Error!");
            return false;
        }
        int dataVersion = pDKXManager->GetDataVersion(filePath);
#endif

        for (size_t i = 0; i < vBookmarks.size(); ++i)
        {
            ICT_ReadingDataItem* localReadingDataItem = vBookmarks[i];
            if (NULL == localReadingDataItem)
            {
                continue;
            }
            ReadingDataItem* readingDataItem = CreateReadingDataItemFromICTReadingDataItem(*localReadingDataItem);
            if (NULL != readingDataItem)
            {
                switch (readingDataItem->GetType())
                {
                    case ReadingDataItem::PROGRESS:
                        //readingProgressInfo.AddReadingDataItem(*readingDataItem);
                        break;
                    default:
                        readingDataInfo.AddReadingDataItem(*readingDataItem);
                        break;
                }
                delete readingDataItem;
            }
            DKXReadingDataItemFactory::DestoryReadingDataItem(localReadingDataItem);
        }
#ifdef KINDLE_FOR_TOUCH
        if (dkxBlock)
        {
            DKXManager::DestoryDKXBlock(dkxBlock);
        }
#endif

        BookStoreInfoManager* bookStoreInfoManager = BookStoreInfoManager::GetInstance();
        //DebugPrintf(DLC_DIAGNOSTIC, "WifiConnected: %d, LoggedIn: %d", WifiManager::GetInstance()->IsConnected(), CAccountManager::GetInstance()->IsLoggedIn());
        if (WifiManager::GetInstance()->IsConnected() && CAccountManager::GetInstance()->IsLoggedIn())
        {
            //DebugPrintf(DLC_DIAGNOSTIC, "BookID: %s, FileName: %s, dataVersion: %d", fileInfo->GetBookID(), fileInfo->GetFileName(), dataVersion);
            bookStoreInfoManager->PutReadingData(fileInfo->GetBookID()
                    , fileInfo->GetFileName(), dataVersion, readingDataInfo, IsDuoKanBookStoreBook(fileInfo->GetFilePath()));
        }
    }

    return true;
}

bool OfflineBookStoreManager::OnPutReadingDataUpdate(const EventArgs& args)
{
    const ReadingDataSyncUpdateArgs readingDataSyncUpdateArgs =
        (const ReadingDataSyncUpdateArgs&)args;
    if (readingDataSyncUpdateArgs.succeeded 
            && readingDataSyncUpdateArgs.result.HasSyncResult())
    {
        const SyncResult& syncResult = 
            readingDataSyncUpdateArgs.result.GetSyncResult();

        //DebugPrintf(DLC_DIAGNOSTIC, "OfflineBookStoreManager::OnPutReadingDataUpdate syncResult: %d, bookId: %s"
        //        , syncResult.GetResultCode(), syncResult.GetBookId().c_str());
        if (syncResult.GetResultCode() != SRC_SUCCEEDED && syncResult.GetResultCode() != SRC_CONFLICT)
        {
            return false;
        }

        string bookId = syncResult.GetBookId();
        DelBookWaitUpload(bookId.c_str());

        SendBooksWaitUploadNewNote();

        return true;
    }

    return false;
}

bool OfflineBookStoreManager::IsBookAdded(const char* bookId,
                                          std::tr1::shared_ptr<DataUpdateArgs>& container)
{
    for (size_t i = 0; i < container->dataList.size(); ++i)
    {
        if (container->dataList[i] != 0 &&
            dynamic_cast<model::BookInfo*>(container->dataList[i].get())->GetBookId().compare(bookId) == 0)
        {
            return true;
        }
    }
    return false;
}

bool OfflineBookStoreManager::IsBookAdded(const char* bookId,
                                          const DataUpdateArgs& container)
{
    for (size_t i = 0; i < container.dataList.size(); ++i)
    {
        if (container.dataList[i] != 0 &&
            dynamic_cast<model::BookInfo*>(container.dataList[i].get())->GetBookId().compare(bookId) == 0)
        {
            return true;
        }
    }
    return false;
}
                        

bool OfflineBookStoreManager::IsBookAdded(const char* bookId,
                                          pthread_mutex_t& mutex,
                                          std::tr1::shared_ptr<DataUpdateArgs>& container)
{
    AutoLock lock(&mutex);
    if (container == 0 || container->dataList.empty())
    {
        return false;
    }

    return IsBookAdded(bookId, container);
}

OfflineBookStoreManager::ContainerError OfflineBookStoreManager::AddBooksIntoContainer(const model::BookInfoList& books,
                                                              pthread_mutex_t& mutex,
                                                              std::tr1::shared_ptr<DataUpdateArgs>& container,
                                                              dk::bookstore::model::ObjectType subType)
{
    AutoLock lock(&mutex);
    if (container == 0)
    {
        container.reset(new DataUpdateArgs());
    }

    if (container->dataList.size() >= CART_SIZE_LIMIT)
    {
        return CONTAINER_FULL;
    }

    model::BookInfoList::const_iterator iter = books.begin();
    for (; iter != books.end(); iter++)
    {
        bool found = false;
        for (size_t i = 0; i < container->dataList.size(); ++i)
        {
            if (container->dataList[i] != 0 &&
                dynamic_cast<model::BookInfo*>(container->dataList[i].get())->GetBookId().compare((*iter)->GetBookId()) == 0)
            {
                found = true;
                model::BookInfoSPtr addedBook(dynamic_cast<model::BookInfo*>((*iter)->Clone()));
                addedBook->SetSubType(subType);
                container->dataList[i] = addedBook;
                break;
            }
        }
        if (!found)
        {
            model::BookInfoSPtr addedBook(dynamic_cast<model::BookInfo*>((*iter)->Clone()));
            addedBook->SetSubType(subType);
            container->dataList.push_back(addedBook);
        }
    }
    return CONTAINER_OK;
}

OfflineBookStoreManager::ContainerError OfflineBookStoreManager::RemoveBooksFromContainer(const model::BookInfoList& books,
                                                       pthread_mutex_t& mutex,
                                                       std::tr1::shared_ptr<DataUpdateArgs>& container)
{
    AutoLock lock(&mutex);
    if (container == 0 || container->dataList.empty())
    {
        return CONTAINER_EMPTY;
    }

    bool found = false;
    model::BookInfoList::const_iterator iter = books.begin();
    for (; iter != books.end(); iter++)
    {
        model::BasicObjectList::iterator ti = container->dataList.begin();
        for (; ti != container->dataList.end(); ++ti)
        {
            std::string srcBookId = dynamic_cast<model::BookInfo*>((*ti).get())->GetBookId();
            std::string inputBookId = (*iter)->GetBookId();

            std::string srcTitle = dynamic_cast<model::BookInfo*>((*ti).get())->GetTitle();
            if ((*ti) != 0 && (srcBookId == inputBookId))
            {
                found = true;
                container->dataList.erase(ti);
                break;
            }
        }
    }
    return found ? CONTAINER_OK : CONTAINER_NOT_FOUND;
}

bool OfflineBookStoreManager::GetCachedBooksFromContainer(DataUpdateArgs& dst,
                                                          pthread_mutex_t& mutex,
                                                          std::tr1::shared_ptr<DataUpdateArgs>& container)
{
    AutoLock lock(&mutex);
    if (container == 0)
    {
        container.reset(new DataUpdateArgs());
        container->errorCode = BSE_OK;
        container->total = 0;
        container->succeeded = true;
        container->moreData = false;
    }
    dst.dataList = container->dataList;
    dst.errorCode = container->errorCode;
    dst.handled = container->handled;
    dst.moreData = container->moreData;
    dst.startIndex = container->startIndex;
    dst.succeeded = container->succeeded;
    dst.total = container->total;
    return true;
}

OfflineBookStoreManager::ContainerError OfflineBookStoreManager::updateByCloud(const DataUpdateArgs& cloudList,
                                                                               int sizeLimit,
                                                                               pthread_mutex_t& mutex,
                                                                               std::tr1::shared_ptr<DataUpdateArgs>& container,
                                                                               model::BookInfoList& toSendList)
{
    AutoLock lock(&mutex);
    if (container == 0 || container->dataList.empty())
    {
        // Nothing to merge
        return CONTAINER_EMPTY;
    }

    toSendList.clear();
    model::BasicObjectList::iterator iter = container->dataList.begin();
    while (iter != container->dataList.end())
    {
        model::BookInfo* bookInfo = dynamic_cast<model::BookInfo*>((*iter).get());
        if (bookInfo != 0 &&
            IsBookAdded(bookInfo->GetBookId().c_str(), cloudList))
        {
            // remove the local item if could list already has it
            iter = container->dataList.erase(iter);
        }
        else if (toSendList.size() + cloudList.dataList.size() >= sizeLimit)
        {
            // cart is full cannot update anymore
            return CONTAINER_FULL;
        }
        else
        {
            // add this book to sending list and remove it from local cart.
            // it will be sent immediately.
            model::BookInfoSPtr transferBook(dynamic_cast<model::BookInfo*>((*iter)->Clone()));
            transferBook->SetSubType(model::OT_BOOK);
            toSendList.push_back(transferBook);
            iter = container->dataList.erase(iter);
        }
    }
    return CONTAINER_OK;
}

void OfflineBookStoreManager::updateLocalCart()
{
    if (m_offlineCartBooks == 0)
    {
        return;
    }

    m_offlineCartBooks->succeeded = true;
    m_offlineCartBooks->moreData = false;
    if (m_offlineCartBooks->dataList.empty())
    {
        m_offlineCartBooks->errorCode = BSE_OK;
        m_offlineCartBooks->total = 0;
        return;
    }

    m_offlineCartBooks->total = m_offlineCartBooks->dataList.size();
    if (m_offlineCartBooks->dataList.size() >= CART_SIZE_LIMIT)
    {
        m_offlineCartBooks->errorCode = CART_IS_FULL;
    }
    else
    {
        m_offlineCartBooks->errorCode = BSE_OK;
    }
}

void OfflineBookStoreManager::updateLocalFavourites()
{
    if (m_offlineFavouriteBooks == 0)
    {
        return;
    }

    m_offlineFavouriteBooks->succeeded = true;
    m_offlineFavouriteBooks->moreData = false;
    if (m_offlineFavouriteBooks->dataList.empty())
    {
        m_offlineFavouriteBooks->errorCode = BSE_OK;
        m_offlineFavouriteBooks->total = 0;
        return;
    }

    m_offlineFavouriteBooks->total = m_offlineFavouriteBooks->dataList.size();
    if (m_offlineFavouriteBooks->dataList.size() >= CART_SIZE_LIMIT)
    {
        m_offlineFavouriteBooks->errorCode = CART_IS_FULL;
    }
    else
    {
        m_offlineFavouriteBooks->errorCode = BSE_OK;
    }
}

BookStoreErrno OfflineBookStoreManager::AddFavouriteBooks(const model::BookInfoList& books)
{
    OfflineBookStoreManager::ContainerError err = AddBooksIntoContainer(books, m_favouriteBooksLock, m_offlineFavouriteBooks);

    {
        AutoLock lock(&m_favouriteBooksLock);
        updateLocalFavourites();
    }

    SaveLocalFavourites();
    switch (err)
    {
    case CONTAINER_OK:
        return BSE_OK;
    case CONTAINER_FULL:
        return FAVOURITES_IS_FULL;
    default:
        break;
    }
    return BSE_UNKNOWN;
}

BookStoreErrno OfflineBookStoreManager::RemoveFavouriteBooks(const model::BookInfoList& books)
{
    OfflineBookStoreManager::ContainerError err = RemoveBooksFromContainer(books, m_favouriteBooksLock, m_offlineFavouriteBooks);
    
    {
        AutoLock lock(&m_favouriteBooksLock);
        updateLocalFavourites();
    }

    SaveLocalFavourites();
    switch (err)
    {
    case CONTAINER_OK:
        return BSE_OK;
    case CONTAINER_NOT_FOUND:
    case CONTAINER_EMPTY:
    default:
        break;
    }
    return BSE_UNKNOWN;
}

BookStoreErrno OfflineBookStoreManager::MoveFavouriteBooksToCart(const model::BookInfoList& books)
{
    return AddBooksToCart(books);
}

bool OfflineBookStoreManager::IsBookAddedToFavourite(const char* bookId)
{
    return IsBookAdded(bookId, m_favouriteBooksLock, m_offlineFavouriteBooks);
}

bool OfflineBookStoreManager::GetLocalFavouriteList(DataUpdateArgs& dst)
{
    return GetCachedBooksFromContainer(dst, m_favouriteBooksLock, m_offlineFavouriteBooks);
}

BookStoreErrno OfflineBookStoreManager::AddBooksToCart(const model::BookInfoList& books)
{
    OfflineBookStoreManager::ContainerError err = AddBooksIntoContainer(books, m_cartBooksLock, m_offlineCartBooks, model::OT_BOOK_IN_CART);
    {
        AutoLock lock(&m_cartBooksLock);
        updateLocalCart();
    }
    SaveLocalCart();
    switch (err)
    {
    case CONTAINER_OK:
        return BSE_OK;
    case CONTAINER_FULL:
        return CART_IS_FULL;
    default:
        break;
    }
    return BSE_UNKNOWN;
}

BookStoreErrno OfflineBookStoreManager::RemoveBooksFromCart(const model::BookInfoList& books)
{
    OfflineBookStoreManager::ContainerError err = RemoveBooksFromContainer(books, m_cartBooksLock, m_offlineCartBooks);

    {
        AutoLock lock(&m_cartBooksLock);
        updateLocalCart();
    }

    SaveLocalCart();
    switch (err)
    {
    case CONTAINER_OK:
        return BSE_OK;
    case CONTAINER_NOT_FOUND:
    case CONTAINER_EMPTY:
    default:
        break;
    }
    return BSE_UNKNOWN;
}

BookStoreErrno OfflineBookStoreManager::MoveCartBooksToFavourite(const model::BookInfoList& books)
{
    if (RemoveBooksFromCart(books) == BSE_OK)
    {
        return AddFavouriteBooks(books);
    }
    return BSE_UNKNOWN;
}

bool OfflineBookStoreManager::IsBookAddedToCart(const char* bookId)
{
    return IsBookAdded(bookId, m_cartBooksLock, m_offlineCartBooks);
}

bool OfflineBookStoreManager::GetLocalBooksInCart(DataUpdateArgs& dst)
{
    return GetCachedBooksFromContainer(dst, m_cartBooksLock, m_offlineCartBooks);
}

BookStoreErrno OfflineBookStoreManager::UpdateCartByCloud(const DataUpdateArgs& cloudList,
                                                model::BookInfoList& toSendList)
{
    OfflineBookStoreManager::ContainerError err = updateByCloud(cloudList,
                                                                CART_SIZE_LIMIT,
                                                                m_cartBooksLock,
                                                                m_offlineCartBooks,
                                                                toSendList);
    {
        AutoLock lock(&m_cartBooksLock);
        updateLocalCart();
    }
    SaveLocalCart();
    switch (err)
    {
    case CONTAINER_OK:
        return BSE_OK;
    case CONTAINER_FULL:
        return CART_IS_FULL;
    case CONTAINER_NOT_FOUND:
    case CONTAINER_EMPTY:
    default:
        break;
    }
    return BSE_UNKNOWN;
}

BookStoreErrno OfflineBookStoreManager::UpdateFavouritesByCloud(const DataUpdateArgs& cloudList,
                                                      model::BookInfoList& toSendList)
{
    OfflineBookStoreManager::ContainerError err = updateByCloud(cloudList,
                                                                FAVOURITES_SIZE_LIMIT,
                                                                m_favouriteBooksLock,
                                                                m_offlineFavouriteBooks,
                                                                toSendList);
    {
        AutoLock lock(&m_favouriteBooksLock);
        updateLocalFavourites();
    }

    SaveLocalFavourites();
    switch (err)
    {
    case CONTAINER_OK:
        return BSE_OK;
    case CONTAINER_FULL:
        return FAVOURITES_IS_FULL;
    case CONTAINER_NOT_FOUND:
    case CONTAINER_EMPTY:
    default:
        break;
    }
    return BSE_UNKNOWN;
}

OfflineBookStoreTaskPtr OfflineBookStoreManager::GetOfflineTaskByBook(model::BookInfo* bookInfo)
{
    OfflineBookStoreTaskPtr task(new OfflineBookStoreTask);
    task->SetInfo("bookId", bookInfo->GetBookId());
    task->SetInfo("IsDrm", bookInfo->IsDrm() ? "1" : "0");
    task->SetInfo("CoverUrl", bookInfo->GetCoverUrl());
    task->SetInfo("Content", bookInfo->GetContent());
    task->SetInfo("CopyRights", bookInfo->GetCopyRights());
    task->SetInfo("Title", bookInfo->GetTitle());
    task->SetInfo("Authors", bookInfo->GetAuthors());
    task->SetInfo("Editors", bookInfo->GetEditors());

    {
        char priceBuf[256];
        sprintf(priceBuf, "%d", bookInfo->GetPrice());
        task->SetInfo("Price", priceBuf);
    }

    {
        char discountBuf[256];
        sprintf(discountBuf, "%d", bookInfo->GetDiscountPrice());
        task->SetInfo("DiscountPrice", discountBuf);
    }

    {
        char paperBuf[256];
        sprintf(paperBuf, "%d", bookInfo->GetPaperPrice());
        task->SetInfo("PaperPrice", paperBuf);
    }

    {
        char scoreBuf[256];
        sprintf(scoreBuf, "%f", bookInfo->GetScore());
        task->SetInfo("Score", scoreBuf);
    }

    {
        char subTypeBuf[32];
        sprintf(subTypeBuf, "%d", static_cast<int>(bookInfo->m_subType));
        task->SetInfo("SubType", subTypeBuf);
    }
    return task;
}

model::BasicObjectSPtr OfflineBookStoreManager::GetBookByOfflineTask(OfflineBookStoreTaskPtr task)
{
    BasicObjectSPtr book(new BookInfo());
    BookInfo* bookInfo = dynamic_cast<BookInfo*>(book.get());
    bookInfo->m_subType = static_cast<ObjectType>(atoi(task->GetInfo("SubType").c_str()));
    bookInfo->m_scoring = atof(task->GetInfo("Score").c_str());
    bookInfo->m_paperPrice = atoi(task->GetInfo("PaperPrice").c_str());
    bookInfo->m_discountPrice = atoi(task->GetInfo("DiscountPrice").c_str());
    bookInfo->m_price = atoi(task->GetInfo("Price").c_str());

    bookInfo->m_bookId = task->GetInfo("bookId");
    bookInfo->m_isDrm = (atoi(task->GetInfo("IsDrm").c_str()) == 1) ? true : false;
    bookInfo->m_coverUrl = task->GetInfo("CoverUrl");
    bookInfo->m_content = task->GetInfo("Content");
    bookInfo->m_rights = task->GetInfo("CopyRights");
    bookInfo->m_title = task->GetInfo("Title");
    bookInfo->m_authors = task->GetInfo("Authors"); 
    bookInfo->m_editors = task->GetInfo("Editors");
    bookInfo->m_translators = task->GetInfo("Translators");
    return book;
}

bool OfflineBookStoreManager::GetTasksFromBookList(const model::BasicObjectList& books,
                          std::vector<OfflineBookStoreTaskPtr>& bookTasks)
{
    bookTasks.clear();
    if (books.empty())
    {
        return true;
    }

    for (int i = 0; i < books.size(); i++)
    {
        model::BookInfo* book = dynamic_cast<model::BookInfo*>(books[i].get());
        OfflineBookStoreTaskPtr task = GetOfflineTaskByBook(book);
        bookTasks.push_back(task);
    }
    return true;
}

bool OfflineBookStoreManager::GetBookListFromTasks(const std::vector<OfflineBookStoreTaskPtr>& bookTasks,
                          model::BasicObjectList& books)
{
    books.clear();
    if (bookTasks.empty())
    {
        return true;
    }

    for (int i = 0; i < bookTasks.size(); i++)
    {
        model::BasicObjectSPtr book = GetBookByOfflineTask(bookTasks[i]);
        books.push_back(book);
    }
    return true;
}

bool OfflineBookStoreManager::SaveLocalCart()
{
    std::vector<OfflineBookStoreTaskPtr> localCartTasks;
    {
        AutoLock lock(&m_cartBooksLock);
        if (m_offlineCartBooks == 0)
        {
            return false;
        }
        if (!GetTasksFromBookList(m_offlineCartBooks->dataList, localCartTasks))
        {
            return false;
        }
    }

    return SaveToXML<std::vector<OfflineBookStoreTaskPtr> >(localCartTasks,
                                                            "DK_CART_ROOT",
                                                            LOCAL_CART_ITEM,
                                                            m_strCartPath.c_str(),
                                                            m_cartBooksLock);
}

bool OfflineBookStoreManager::LoadLocalCart()
{
    std::vector<OfflineBookStoreTaskPtr> localCartTasks;
    if (!LoadFromXML<std::vector<OfflineBookStoreTaskPtr> >(localCartTasks,
                                                            LOCAL_CART_ITEM,
                                                            m_strCartPath.c_str(),
                                                            m_cartBooksLock))
    {
        return false;
    }

    AutoLock lock(&m_cartBooksLock);
    if (m_offlineCartBooks == 0)
    {
        m_offlineCartBooks.reset(new DataUpdateArgs());
    }

    if (!GetBookListFromTasks(localCartTasks, m_offlineCartBooks->dataList))
    {
        return false;
    }
    updateLocalCart();
    return true;
}

bool OfflineBookStoreManager::SaveLocalFavourites()
{
    std::vector<OfflineBookStoreTaskPtr> localFavouritesTasks;
    {
        AutoLock lock(&m_favouriteBooksLock);
        if (m_offlineFavouriteBooks == 0)
        {
            return false;
        }
        if (!GetTasksFromBookList(m_offlineFavouriteBooks->dataList, localFavouritesTasks))
        {
            return false;
        }
    }

    return SaveToXML<std::vector<OfflineBookStoreTaskPtr> >(localFavouritesTasks,
                                                            "DK_FAVOURITES_ROOT",
                                                            LOCAL_FAVOURITES_ITEM,
                                                            m_strFavouritesPath.c_str(),
                                                            m_favouriteBooksLock);
}
    
bool OfflineBookStoreManager::LoadLocalFavourites()
{
    std::vector<OfflineBookStoreTaskPtr> localFavouritesTasks;
    if (!LoadFromXML<std::vector<OfflineBookStoreTaskPtr> >(localFavouritesTasks,
                                                            LOCAL_FAVOURITES_ITEM,
                                                            m_strFavouritesPath.c_str(),
                                                            m_favouriteBooksLock))
    {
        return false;
    }

    AutoLock lock(&m_favouriteBooksLock);
    if (m_offlineFavouriteBooks == 0)
    {
        m_offlineFavouriteBooks.reset(new DataUpdateArgs());
    }

    if (!GetBookListFromTasks(localFavouritesTasks, m_offlineFavouriteBooks->dataList))
    {
        return false;
    }
    updateLocalFavourites();
    return true;
}

} //namespace bookstore
} //namespace dk
