#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/BookStoreOrderManager.h"
#include "Mutex.h"
#include "ListDataFetcher.h"
#include "BookStore/BasicObject.h"
#include "BookStore/PushedMessagesInfo.h"
#include "BookStore/DownloadTaskBuilder.h"
#include "BookStore/OfflineBookStoreManager.h"
#include "BookStore/BookDownloadController.h"
#include "GUI/EventListener.h"
#include "GUI/MessageEventArgs.h"
#include "I18n/StringManager.h"
#include "BookStore/DataUpdateArgs.h"
#include "BookStore/ConstStrings.h"
#include "Utility/JsonObject.h"
#include "Utility/PathManager.h" 
#include "Utility/ConfigFile.h" 
#include "Utility/StringUtil.h" 
#include "Common/CAccountManager.h"
#include "DownloadManager/DownloaderImpl.h"
#include "DownloadManager/DownloadTaskFactory.h"
#include "PersonalUI/PushedMessagesManager.h"
#include "Common/LockObject.h"
#include "Common/FileManager_DK.h"
#include "XMLDomDocument.h"
#include "Utility/HttpUtil.h"
#include "Utility/SystemUtil.h"
#include "Utility/FileSystem.h"
#include <set>
#include <tr1/memory>


using dk::utility::ConfigFile;
using dk::utility::PathManager;
using dk::utility::StringUtil;
using dk::utility::HttpUtil;
using namespace dk::common;
using namespace dk::account;

namespace dk
{
namespace bookstore
{

bool CategoryTreeArgs::GetChildrenCategoryOfId(const char* categoryId, std::vector<model::BasicObjectSPtr>* childrenList) const
{
    for (size_t i = 0; i < dataList.size(); ++i)
    {
        model::CategoryInfo* categoryInfo = (model::CategoryInfo*)(dataList[i].get());
        if (categoryInfo && categoryInfo->GetChildrenNodeList(categoryId, *childrenList))
        {
            return true;
        }
    }

    return false;
}

bool BookNoteSummaryArgs::Init(const JsonObject* jsonObject, bool isDKBook)
{
    if (!jsonObject)
    {
        return false;
    }

    jsonObject->GetIntValue("code", &errorCode);
    jsonObject->GetIntValue("book_count", &total);
    jsonObject->GetIntValue("total_bookmark_count", &totalBookMarkCount);
    jsonObject->GetIntValue("total_comment_count", &totalCommentCount);
    jsonObject->GetStringValue("message", &message);
    succeeded = (errorCode == 0);
    JsonObjectSPtr bookNotesObj = jsonObject->GetSubObject("book_list");
    if (bookNotesObj)
    {
        std::vector<std::string> bookIds = bookNotesObj->GetAllKeys();
        for ( unsigned int i = 0; i < bookIds.size(); ++i)
        {
            JsonObjectSPtr bookNoteObj = bookNotesObj->GetSubObject(bookIds[i].c_str());
            model::BookNoteSummary* pBookNoteSummary = model::BookNoteSummary::CreateBookNoteSummary(bookNoteObj.get());
            if (pBookNoteSummary)
            {
                pBookNoteSummary->SetBookId(bookIds[i]);
                pBookNoteSummary->SetDuoKanBook(isDKBook);
                dataList.push_back(model::BookNoteSummarySPtr(pBookNoteSummary));
            }
        }

        return true;
    }

    return false;
}

class TopicBookListFetcher: public ListDataFetcher
{
public:
    TopicBookListFetcher()
        : m_topicId(-1)
    {
        SetObjectType(model::OT_BOOK);
    }

    DownloadTaskSPtr BuildDownloadTask(int start, int length) {
        return DownloadTaskBuilder::BuildTopicBookListTask(m_topicId, start, length);
    }

    int GetTopicId() const { return m_topicId;}
    const char* GetLabel() const { return m_label.c_str(); }
    const char* GetDescription() const { return m_description.c_str(); }
    const char* GetBannerUrl() const { return m_bannerUrl.c_str(); }
    std::string GetCoverUrl() const { return m_coverUrl; }
    std::string GetUpdatedTime() const { return m_updatedTime; }
    int GetWeight() const { return m_weight; }

    void SetTopicId(int topicId){
        AutoLock lock(&m_lock);
        if (topicId != m_topicId)
        {
            if (IsBusy())
            {
                // Abort current task
                m_currentTask = DownloadTaskSPtr();
            }
            m_topicId = topicId;
            Reset();
        }
    }
protected:
    virtual bool PostParseData(JsonObject* jsonObject)
    {
        jsonObject->GetStringValue("description", &m_description);
        jsonObject->GetStringValue("banner", &m_bannerUrl);
        jsonObject->GetStringValue("label", &m_label);
        jsonObject->GetStringValue("cover", &m_coverUrl);
        jsonObject->GetIntValue("weight", &m_weight);
        jsonObject->GetStringValue("updated", &m_updatedTime);
        return true;
    }
private:
    int m_topicId;
    std::string m_label;
    std::string m_description;
    std::string m_bannerUrl;
    std::string m_coverUrl;
    std::string m_updatedTime;
    int m_weight;
};

class BookCommentListFetcher: public ListDataFetcher
{
public:
    BookCommentListFetcher()
        : ListDataFetcher(
                std::tr1::function<DownloadTaskSPtr(int, int, FetchDataOrder)>(),
                model::OT_COMMENT,
                "comments")
        , m_userLastScore(-1)

    {
    }

    DownloadTaskSPtr BuildDownloadTask(int start, int length)
    {
        return DownloadTaskBuilder::BuildCommentListTask(m_bookId.c_str(), start, length, m_order);
    }

    std::string GetBookId() const
    {
        return m_bookId;
    }

    void SetBookId(const char* bookId)
    {
        AutoLock lock(&m_lock);
        if (m_bookId != bookId)
        {
            if (IsBusy())
            {
                // Abort current task
                m_currentTask = DownloadTaskSPtr();
            }
            m_bookId = bookId;
            m_userLastScore = -1;
            Reset();
        }
    }

    int GetUserLastScore() const
    {
        return m_userLastScore;
    }
protected:
    virtual bool PostParseData(JsonObject* jsonObject)
    {
        jsonObject->GetIntValue("user_last_score", &m_userLastScore);
        return true;
    }
private:
    std::string m_bookId;
    int m_userLastScore;;
};

class PushedMessagesFetcher: public ListDataFetcher
{
public:
    PushedMessagesFetcher()
        : ListDataFetcher(
        std::tr1::function<DownloadTaskSPtr(int, int, FetchDataOrder)>(),
        model::OT_PUSHEDMESSAGES,
        "message")
        , m_interval(0)

    {
    }

    DownloadTaskSPtr BuildDownloadTask(int start, int length)
    {
        return DownloadTaskBuilder::BuildPushedMessagesTask(m_messageID.c_str());
    }

    int GetInterval() const 
    {
        return m_interval;
    }

    std::string GetMessageID() const
    {
        return m_messageID;
    }

    void SetMessageID(const std::string& messageID)
    {
        if (!messageID.empty())
        {
            if (!m_messageID.empty())
            {
                m_messageID += ',';
            }
            m_messageID += messageID;
        }
    }

    void ClearMessageID()
    {
        m_messageID.clear();
        m_interval = 0;
    }
protected:
    virtual bool PostParseData(JsonObject* jsonObject)
    {
        jsonObject->GetIntValue("interval", &m_interval);
        return true;
    }
private:
    int m_interval;
    std::string m_messageID;
};

class BoxMessagesFetcher: public ListDataFetcher
{
public:
    BoxMessagesFetcher()
        : ListDataFetcher(
        std::tr1::function<DownloadTaskSPtr(int, int, FetchDataOrder)>(),
        model::OT_BOXMESSAGES,
        "messages")
        , m_bookSerial(1)
    {
    }

    DownloadTaskSPtr BuildDownloadTask(int start, int length)
    {
        return DownloadTaskBuilder::BuildGetBoxMessagesTask(m_messageType.c_str(), m_bookSerial, start, length);
    }

    std::string GetMessageType() const
    {
        return m_messageType;
    }

    void SetMessageType(const std::string& messageType)
    {
        m_messageType = messageType;
    }

    void ClearMessageType()
    {
        m_messageType.clear();
    }
private:
    int m_bookSerial;
    std::string m_messageType;
};

class TagBookListFetcher: public ListDataFetcher
{
public:
    TagBookListFetcher()
    {
        SetObjectType(model::OT_BOOK);
    }

    DownloadTaskSPtr BuildDownloadTask(int start, int length)
    {
        return DownloadTaskBuilder::BuildGetBookListWithTagTask(m_tagName.c_str(), start, length);
    }

    std::string GetTagName() const
    {
        return m_tagName;
    }

    void SetTagName(const char* tagName)
    {
        AutoLock lock(&m_lock);
        if (m_tagName != tagName)
        {
            if (IsBusy())
            {
                // Abort current task
                m_currentTask = DownloadTaskSPtr();
            }
            m_tagName = tagName;
            Reset();
        }
    }
private:
    std::string m_tagName;
};

class PublishBookListFetcher: public ListDataFetcher
{
public:
    PublishBookListFetcher()
    {
        SetObjectType(model::OT_BOOK);
    }

    DownloadTaskSPtr BuildDownloadTask(int start, int length)
    {
        return DownloadTaskBuilder::BuildPublishBookListTask(m_publishId.c_str(), start, length, m_order);
    }

    std::string GetPublishId() const
    {
        return m_publishId;
    }

    void SetPublishId(const char* publishId)
    {
        AutoLock lock(&m_lock);
        if (m_publishId != publishId)
        {
            if (IsBusy())
            {
                // Abort current task
                m_currentTask = DownloadTaskSPtr();
            }
            m_publishId = publishId;
            Reset();
        }
    }
private:
    std::string m_publishId;
};

class CategoryBookListFetcher: public ListDataFetcher
{
public:
    CategoryBookListFetcher()
    {
        SetObjectType(model::OT_BOOK);
    }

    DownloadTaskSPtr BuildDownloadTask(int start, int length)
    {
        return DownloadTaskBuilder::BuildCategoryBookListTask(m_categoryId.c_str(), start, length, m_order);
    }

    std::string GetCategoryId() const
    {
        return m_categoryId;
    }

    void SetCategoryId(const char* categoryId)
    {
        AutoLock lock(&m_lock);
        if (m_categoryId != categoryId)
        {
            if (IsBusy())
            {
                // Abort current task
                m_currentTask = DownloadTaskSPtr();
            }
            m_categoryId = categoryId;
            Reset();
        }
    }
private:
    std::string m_categoryId;
};

class SearchBookListFetcher: public ListDataFetcher
{
public:
    SearchBookListFetcher()
    {
        SetObjectType(model::OT_BOOK);
    }

    DownloadTaskSPtr BuildDownloadTask(int start, int length)
    {
        return DownloadTaskBuilder::BuildSearchBookTask(m_query.c_str(), start, length);
    }

    std::string GetQuery() const
    {
        return m_query;
    }

    void SetQuery(const char* query)
    {
        AutoLock lock(&m_lock);
        if (m_query != query)
        {
            if (IsBusy())
            {
                // Abort current task
                m_currentTask = DownloadTaskSPtr();
            }
            m_query = query;
            Reset();
        }
    }
private:
    std::string m_query;
};

class BookChangeLogListFetcher: public ListDataFetcher
{
public:
    BookChangeLogListFetcher()
        : ListDataFetcher()
    {
        SetObjectType(model::OT_CHANGELOG);
    }

    DownloadTaskSPtr BuildDownloadTask(int start, int length)
    {
        return DownloadTaskBuilder::BuildBookChangeLogListTask(m_bookId.c_str(), start, length);
    }

    std::string GetBookId() const
    {
        return m_bookId;
    }

    void SetBookId(const char* bookId){
        AutoLock lock(&m_lock);
        if (m_bookId != bookId)
        {
            if (IsBusy())
            {
                // Abort current task
                m_currentTask = DownloadTaskSPtr();
            }
            m_bookId = bookId;
            Reset();
        }
    }
private:
    std::string m_bookId;
};

class BookStoreInfoManagerImpl: public BookStoreInfoManager
                              , public EventListener
{
public:
    BookStoreInfoManagerImpl();
    virtual ~BookStoreInfoManagerImpl();
    virtual FetchDataResult FetchRecommendList(
            int start, 
            int length);

    virtual FetchDataResult FetchDiscountBookList(
            int start,
            int length,
            FetchDataOrder order);

    virtual FetchDataResult FetchFreeBookList(
            int start,
            int length);

    virtual FetchDataResult FetchPurchasedList();

    virtual FetchDataResult FetchTopicList(
            int start,
            int length);

    virtual FetchDataResult FetchTopicBookList(
            int topicId,
            int start,
            int length);

    virtual FetchDataResult FetchCategoryList();
    virtual FetchDataResult FetchCategoryTree();

    virtual FetchDataResult FetchPublishList(int fetchCount);
    virtual FetchDataResult FetchPublishBookList(const char* publishId, int start, int length, FetchDataOrder order);

    virtual FetchDataResult FetchCategoryBookList(
            const char* categoryId,
            int start,
            int length,
            FetchDataOrder order);

    virtual FetchDataResult FetchBookInfo(const char* bookId);

    virtual FetchDataResult FetchBookKeyInfo(const char* key);
    virtual FetchDataResult ExchangeDuokanKey(const char* key);

    virtual FetchDataResult FetchCoverImage(
            const char* imgUrl);

    virtual FetchDataResult CreateOrder(const char* bookId);
    virtual FetchDataResult MailOrder(const char* orderId);
    virtual FetchDataResult FetchReadingProgress(const char* bookId, bool IsDKBookStoreBook);
    virtual FetchDataResult FetchReadingData(const char* bookId, int dataVersion, bool IsDKBookStoreBook);
    virtual FetchDataResult FetchSearchBookList(
            const char* bookId,
            int start,
            int length);

    virtual FetchDataResult FetchHotFreeBookList(
            int start,
            int length);

    virtual FetchDataResult FetchHotPayBookList(
            int start,
            int length);

    virtual FetchDataResult FetchHotMonthlyBookList(
        int start,
        int length);

    virtual FetchDataResult FetchHotFavorBookList(
        int start,
        int length);

    virtual FetchDataResult FetchDouBanRankBookList(
        int start,
        int length);

    virtual FetchDataResult FetchAmazonRankBookList(
        int start,
        int length);

    virtual FetchDataResult FetchJingDongRankBookList(
        int start,
        int length);

    virtual FetchDataResult FetchDangDangRankBookList(
        int start,
        int length);

    virtual FetchDataResult FetchFreshBookList(
            int start,
            int length);

    virtual FetchDataResult FetchDrmCert(
            const char* bookId,
            const char* bookRevision,
            const char* orderId);

    virtual FetchDataResult FetchBookChangeLogList(
            const char* bookId,
            int start,
            int length);


    virtual std::string GetCurrentUserName()
    {
        return StringUtil::ToLower(CAccountManager::GetInstance()->GetEmailFromFile().c_str());
    }

    virtual std::string GetCurrentToken()
    {
        return CAccountManager::GetInstance()->GetTokenFromFile();
    }

    virtual const std::string& GetBookNote() const
    {
        return m_bookNote;
    }

    virtual int GetBookStatus(const char* bookId);
    virtual int GetBookStatus(model::BookInfoSPtr bookInfo);
    virtual TrialBookStatus GetTrialBookStatus(const char* bookId);
    virtual bool IsLocalFileTrial(const char* bookId);
    virtual std::string GetLocalFileRevision(const char* bookId);
    virtual std::string GetLocalFileTitle(const char* bookId);
    virtual std::string GetLocalFileAuthor(const char* bookId);
    virtual FetchDataResult FetchBook(model::BookInfoSPtr bookInfo);
    virtual FetchDataResult FetchTrialBook(model::BookInfoSPtr bookInfo);
    virtual bool WriteInfoFile(model::BookInfoSPtr bookInfo, const char* orderId);
    virtual bool WriteTrialInfoFile(model::BookInfoSPtr bookInfo);
    virtual FetchDataResult FetchAds();
    virtual FetchDataResult FetchRankAds();
    virtual FetchDataResult PutReadingProgress(
            const char* bookId,
            const char* bookName,
            const ReadingBookInfo& readingBookInfo,
            int dataVersion,
            bool IsDKBookStoreBook);
    virtual FetchDataResult PutReadingData(
            const char* bookId,
            const char* bookName,
            int dataVersion,
            const ReadingBookInfo& readingBookInfo,
            bool IsDKBookStoreBook,
            bool IsNeedSyncToDKXInCallBack = true);
    virtual int GetPurchasedBookSize();
    virtual model::BookInfoSPtr GetPurchasedBookInfo(const char* bookId);

    //comment start
    virtual FetchDataResult FetchBookScore(const char* bookId);
    virtual FetchDataResult FetchCommentList(const char* bookId, int start, int length, FetchDataOrder order);
    virtual FetchDataResult FetchOwnCommentsList(int start, int length);
    virtual FetchDataResult FetchHotCommentsList(int start, int length);
    virtual FetchDataResult AddBookScore(const char* bookId, int score);
    virtual FetchDataResult AddBookComment(const char* bookId, int score, const char* title, const char* content);
    virtual FetchDataResult AddBookCommentReply(
            const char* bookId,
            const char* commentId,
            const char* content);
    virtual FetchDataResult VoteBookComment(
            const char* bookId,
            const char* commentId,
            bool useful);
    virtual FetchDataResult FetchBookScoreByCurrentUser(const char* bookId);
    virtual FetchDataResult FetchCommentReply(const char* bookId, const char* commentId, int start, int length);
    //comment end

    virtual FetchDataResult ReportBookBug(const char* bookId,
        const char* bookName,
        const char* bookRevision,
        const char* position,
        const char* ref_chapter,
        const char* reference,
        const char* suggestion,
        bool bConvertCht = false);

    virtual void SetReceivedMessageID(const char* messageID);
    virtual FetchDataResult FetchPushedMessageList();
    virtual int GetPushedMessageInterval();

    //BoxMessages start
    virtual FetchDataResult FetchBoxMessageUnreadCount(const char* messageType, int bookSerial);
    virtual FetchDataResult UpdateBoxMessageUnreadCount(const char* messageType, const char* deletionThreshold);
    virtual FetchDataResult FetchBoxMessageList(const char* messageType, int start, int length);
    //BoxMessages end

    //tag start
    virtual FetchDataResult FetchTagList(const char* tagName, int start, int length);
    //tag end

    virtual FetchDataResult FetchPersonalExperiences();
    virtual FetchDataResult PostPersonalExperiences(const char* newData);
    virtual FetchDataResult FetchAllBookNotesSummary();

    // Batch Tasks
    virtual model::BookInfoList GetCachedPurchasedBookList();
    virtual model::BookInfoList GetCachedPurchasedBookListByState(int request);

private:

    bool OnAccountLoginEvent(const EventArgs& args);

    model::BookInfoSPtr FindPurchasedBookInfo(const char* bookId);
	bool RemoveBookFromPurchasedList(const char* bookId);

    bool WriteInfoFile(const DrmCertUpdateArgs& drmArgs);
    bool ParseDrmCert(const char* jsonData, std::string* cert);
    FetchDataResult FetchBookList(
            ListDataFetcher& fetcher,
            int start,
            int length);

    void SubscribeListEvent(ListDataFetcher& listDataFetcher,
            bool (BookStoreInfoManagerImpl::* memfun)(const EventArgs&));
    void SubscribeDownloadEvent(DownloadTaskSPtr& downloadTask,
            bool (BookStoreInfoManagerImpl::* memfun)(const EventArgs&));
    bool OnListDataUpdate(const EventArgs& args, const char* eventName);
    bool OnRecommendListUpdate(const EventArgs& args);
    bool OnDiscountListUpdate(const EventArgs& args);
    bool OnFreeListUpdate(const EventArgs& args);
    bool OnTopicListUpdate(const EventArgs& args);
    bool OnTopicBookListUpdate(const EventArgs& args);
    bool OnCategoryListUpdate(const EventArgs& args);
    bool OnCategoryTreeUpdate(const EventArgs& args);
    bool OnCategoryBookListUpdate(const EventArgs& args);
    bool OnPublishingListUpdate(const EventArgs& args);
    bool OnPublishingBookListUpdate(const EventArgs& args);
    bool OnPurchasedListUpdate(const EventArgs& args);
    bool OnHotFreeUpdate(const EventArgs& args);
    bool OnHotPayUpdate(const EventArgs& args);
    bool OnFreshUpdate(const EventArgs& args);
    bool OnMonthlyPayUpdate(const EventArgs& args);
    bool OnFavorPayUpdate(const EventArgs& args);
    bool OnDouBanRankListUpdate(const EventArgs& args);
    bool OnAmazonRankListUpdate(const EventArgs& args);
    bool OnJingDongRankListUpdate(const EventArgs& args);
    bool OnDangDangRankListUpdate(const EventArgs& args);
    void ClearBookDownloadInfo();
    FetchDataResult BeginDownloadBookInfo(const char* bookId);
    FetchDataResult BeginSendBookKey(const char* key);
    FetchDataResult BeginExchangeDuokanKey(const char* key);
    bool OnBookInfoDownloadFinished(const EventArgs& args);
    bool OnBookKeySendFinished(const EventArgs& args);
    bool OnExchangeDuokanKeyFinished(const EventArgs& args);
    bool OnCoverImageDownloadFinished(const EventArgs& args);
    bool OnCreateOrderFinished(const EventArgs& args);
    bool OnDrmCertFinished(const EventArgs& args);
    bool OnMailOrderFinished(const EventArgs& args);
    bool OnFetchReadingProgressFinished(const EventArgs& args);
    bool OnFetchReadingDataFinished(const EventArgs& args);
    bool OnSearchBookUpdate(const EventArgs& args);
    bool OnAdsListUpdate(const EventArgs& args);
    bool OnRankAdsListUpdate(const EventArgs& args);
    bool OnPutReadingProgressUpdate(const EventArgs& args);
    bool OnPutReadingDataUpdateSyncToDKX(const EventArgs& args);
    bool OnPutReadingDataUpdateNoSyncToDKX(const EventArgs& args);
    bool OnPutReadingDataUpdate(const EventArgs& args, bool needSyncToDKX = true);
    bool OnBookScoreFinished(const EventArgs& args);
    bool OnCommentListUpdate(const EventArgs& args);
    bool OnOwnCommentsListUpdate(const EventArgs& args);
    bool OnHotCommentsListUpdate(const EventArgs& args);
    bool OnAddBookScoreUpdate(const EventArgs& args);
    bool OnAddBookCommentUpdate(const EventArgs& args);
    bool OnAddBookCommentReplyUpdate(const EventArgs& args);
    bool OnVoteBookCommentUpdate(const EventArgs& args);
    bool OnFetchBookScoreByCurrentUserUpdate(const EventArgs& args);
    bool OnCommentReplyUpdate(const EventArgs& args);
    bool OnBookChangeLogListUpdate(const EventArgs& args);
    bool OnReportBookBugUpdate(const EventArgs& args);
	bool OnFetchPersonalExperiencesUpdate(const EventArgs& args);
    bool OnPostPersonalExperiencesUpdate(const EventArgs& args);
	void OnPersonalExperiencesUpdate(const EventArgs& args, bool isGet);
    bool OnPushedMessagesUpdate(const EventArgs& args);
    bool OnFetchBookNoteSummary(const EventArgs& args, bool isDuoKanBookStoreBook);
    bool OnFetchLocalBookNoteSummary(const EventArgs& args);
    bool OnFetchDuoKanBookNoteSummary(const EventArgs& args);
    //BoxMessages
    bool OnBoxMessageUnreadCountInfoUpdate(const EventArgs& args);
    bool OnPostBoxMessageUnreadCountUpdate(const EventArgs& args);
    bool OnBoxMessagesUpdate(const EventArgs& args);

    //tag
    bool OnTagListUpdate(const EventArgs& args);

private:
    ListDataFetcher m_recommendFetcher;
    ListDataFetcher m_discountBookListFetcher;
    ListDataFetcher m_freeBookListFetcher;
    ListDataFetcher m_topicListFetcher;
    ListDataFetcher m_categoryListFetcher;
    ListDataFetcher m_categoryTreeFetcher;
    ListDataFetcher m_purchasedListFetcher;
    ListDataFetcher m_hotPayListFetcher;
    ListDataFetcher m_hotFreeListFetcher;
    ListDataFetcher m_hotMonthLyListFetcher;
    ListDataFetcher m_hotFavorListFetcher;
    ListDataFetcher m_doubanRankListFetcher;
    ListDataFetcher m_amazonRankListFetcher;
    ListDataFetcher m_jingdongRankListFetcher;
    ListDataFetcher m_dangdangRankListFetcher;
    ListDataFetcher m_freshBookListFetcher;
    ListDataFetcher m_adsListFetcher;
    ListDataFetcher m_rankAdsListFetcher;
    ListDataFetcher m_ownCommentsListFetcher;
    ListDataFetcher m_hotCommentsListFetcher;
    ListDataFetcher m_publishingListFetcher;
    PublishBookListFetcher m_publishingBookListFetcher;

    TopicBookListFetcher m_topicBookListFetcher;
    CategoryBookListFetcher m_categoryBookListFetcher;
    SearchBookListFetcher m_searchBookListFetcher;
    BookCommentListFetcher m_commentsListFetcher;
    BookChangeLogListFetcher m_bookChangeLogListFetcher;
    PushedMessagesFetcher m_pushedMessagesFetcher;
    BoxMessagesFetcher m_boxMessagesFetcher;
    TagBookListFetcher m_tagListFetcher;

    pthread_mutex_t m_purchasedBooksLock;
    model::BookInfoList m_purchasedBooks;

    std::string m_currentUser;
    std::string m_currentToken;
    CategoryTreeArgs m_categoryTreeInfo;
    int m_categoryTreeInfoUpdateIntervalInHour;

    BookNoteSummaryArgs m_localBookNoteSummaryInfo;
    BookNoteSummaryArgs m_duokanBookNoteSummaryInfo;

    std::string m_bookNote;
};

BookStoreInfoManagerImpl::BookStoreInfoManagerImpl()
    : m_recommendFetcher(DownloadTaskBuilder::BuildRecommendTask, model::OT_BOOK)
    , m_discountBookListFetcher(DownloadTaskBuilder::BuildDiscountTask, model::OT_BOOK)
    , m_freeBookListFetcher(DownloadTaskBuilder::BuildFreeTask, model::OT_BOOK)
    , m_topicListFetcher(DownloadTaskBuilder::BuildTopicListTask, model::OT_TOPIC)
    , m_categoryListFetcher(DownloadTaskBuilder::BuildCategoryListTask, model::OT_CATEGORY)
    , m_categoryTreeFetcher(DownloadTaskBuilder::BuildCategoryTreeTask, model::OT_CATEGORY)
    , m_purchasedListFetcher(DownloadTaskBuilder::BuildPurchasedListTask, model::OT_BOOK, "items")
    , m_hotPayListFetcher(DownloadTaskBuilder::BuildHotPayBookListTask, model::OT_BOOK)
    , m_hotFreeListFetcher(DownloadTaskBuilder::BuildHotFreeBookListTask, model::OT_BOOK)
    , m_hotMonthLyListFetcher(DownloadTaskBuilder::BuildHotMonthLyBookListTask, model::OT_BOOK)
    , m_hotFavorListFetcher(DownloadTaskBuilder::BuildHotFavorBookListTask, model::OT_BOOK)
    , m_doubanRankListFetcher(DownloadTaskBuilder::BuildDouBanRankListTask, model::OT_BOOK)
    , m_amazonRankListFetcher(DownloadTaskBuilder::BuildAmazonRankListTask, model::OT_BOOK)
    , m_jingdongRankListFetcher(DownloadTaskBuilder::BuildJingDongRankListTask, model::OT_BOOK)
    , m_dangdangRankListFetcher(DownloadTaskBuilder::BuildDangDangRankListTask, model::OT_BOOK)
    , m_freshBookListFetcher(DownloadTaskBuilder::BuildFreshBookListTask, model::OT_BOOK)
    , m_adsListFetcher(DownloadTaskBuilder::BuildAdsListTask, model::OT_AD, "ads", "code")
    , m_rankAdsListFetcher(DownloadTaskBuilder::BuildRankingAdsTask, model::OT_AD, "ads", "code")
    , m_ownCommentsListFetcher(DownloadTaskBuilder::BuildOwnCommentsListTask, model::OT_MYCOMMENTS, "comments")
    , m_hotCommentsListFetcher(DownloadTaskBuilder::BuildHotCommentsListTask, model::OT_MYCOMMENTS, "comments")
    , m_publishingListFetcher(DownloadTaskBuilder::BuildPublishListTask, model::OT_PUBLISH, "ads", "code")
    , m_categoryTreeInfoUpdateIntervalInHour(8)
{
    SubscribeListEvent(m_recommendFetcher,
            &BookStoreInfoManagerImpl::OnRecommendListUpdate);
    SubscribeListEvent(m_discountBookListFetcher,
            &BookStoreInfoManagerImpl::OnDiscountListUpdate);
    SubscribeListEvent(m_freeBookListFetcher,
            &BookStoreInfoManagerImpl::OnFreeListUpdate);
    SubscribeListEvent(m_topicListFetcher,
            &BookStoreInfoManagerImpl::OnTopicListUpdate);
    SubscribeListEvent(m_topicBookListFetcher,
            &BookStoreInfoManagerImpl::OnTopicBookListUpdate);
    SubscribeListEvent(m_categoryListFetcher,
            &BookStoreInfoManagerImpl::OnCategoryListUpdate);
    SubscribeListEvent(m_categoryTreeFetcher,
            &BookStoreInfoManagerImpl::OnCategoryTreeUpdate);
    SubscribeListEvent(m_publishingListFetcher,
            &BookStoreInfoManagerImpl::OnPublishingListUpdate);
    SubscribeListEvent(m_publishingBookListFetcher,
            &BookStoreInfoManagerImpl::OnPublishingBookListUpdate);
    SubscribeListEvent(m_categoryBookListFetcher,
            &BookStoreInfoManagerImpl::OnCategoryBookListUpdate);
    SubscribeListEvent(m_purchasedListFetcher,
            &BookStoreInfoManagerImpl::OnPurchasedListUpdate);
    SubscribeListEvent(m_searchBookListFetcher,
            &BookStoreInfoManagerImpl::OnSearchBookUpdate);
    SubscribeListEvent(m_hotFreeListFetcher,
            &BookStoreInfoManagerImpl::OnHotFreeUpdate);
    SubscribeListEvent(m_hotPayListFetcher,
            &BookStoreInfoManagerImpl::OnHotPayUpdate);
    SubscribeListEvent(m_hotMonthLyListFetcher,
            &BookStoreInfoManagerImpl::OnMonthlyPayUpdate);
    SubscribeListEvent(m_hotFavorListFetcher,
            &BookStoreInfoManagerImpl::OnFavorPayUpdate);
    SubscribeListEvent(m_doubanRankListFetcher,
            &BookStoreInfoManagerImpl::OnDouBanRankListUpdate);
    SubscribeListEvent(m_amazonRankListFetcher,
            &BookStoreInfoManagerImpl::OnAmazonRankListUpdate);
    SubscribeListEvent(m_jingdongRankListFetcher,
            &BookStoreInfoManagerImpl::OnJingDongRankListUpdate);
    SubscribeListEvent(m_dangdangRankListFetcher,
            &BookStoreInfoManagerImpl::OnDangDangRankListUpdate);
    SubscribeListEvent(m_freshBookListFetcher,
            &BookStoreInfoManagerImpl::OnFreshUpdate);
    SubscribeListEvent(m_adsListFetcher,
            &BookStoreInfoManagerImpl::OnAdsListUpdate);
    SubscribeListEvent(m_rankAdsListFetcher,
            &BookStoreInfoManagerImpl::OnRankAdsListUpdate);
    SubscribeListEvent(m_commentsListFetcher,
        &BookStoreInfoManagerImpl::OnCommentListUpdate);
    SubscribeListEvent(m_ownCommentsListFetcher,
        &BookStoreInfoManagerImpl::OnOwnCommentsListUpdate);
    SubscribeListEvent(m_hotCommentsListFetcher,
        &BookStoreInfoManagerImpl::OnHotCommentsListUpdate);
    SubscribeListEvent(m_bookChangeLogListFetcher,
            &BookStoreInfoManagerImpl::OnBookChangeLogListUpdate);
    SubscribeListEvent(m_pushedMessagesFetcher,
        &BookStoreInfoManagerImpl::OnPushedMessagesUpdate);
    SubscribeListEvent(m_boxMessagesFetcher,
        &BookStoreInfoManagerImpl::OnBoxMessagesUpdate);
    SubscribeListEvent(m_tagListFetcher,
        &BookStoreInfoManagerImpl::OnTagListUpdate);

    SubscribeMessageEvent(CAccountManager::EventAccount, 
            *(CAccountManager::GetInstance()),
            std::tr1::bind(
                std::tr1::mem_fn(&BookStoreInfoManagerImpl::OnAccountLoginEvent),
                this,
                std::tr1::placeholders::_1));

    pthread_mutex_init(&m_purchasedBooksLock, 0);
}

void BookStoreInfoManagerImpl::SubscribeListEvent(
        ListDataFetcher& listDataFetcher,
        bool (BookStoreInfoManagerImpl::* memfun)(const EventArgs&))
{
    SubscribeEvent(ListDataFetcher::EventListDataUpdate,
            listDataFetcher,
            std::tr1::bind(
                std::tr1::mem_fn(memfun),
                this,
                std::tr1::placeholders::_1));
}

void BookStoreInfoManagerImpl::SubscribeDownloadEvent(
        DownloadTaskSPtr& downloadTask,
        bool (BookStoreInfoManagerImpl::* memfun)(const EventArgs&))
{
    if (!downloadTask)
    {
        return;
    }
    SubscribeEvent(DownloadTask::EventDownloadTaskFinished,
            *downloadTask.get(),
            std::tr1::bind(
                std::tr1::mem_fn(memfun),
                this,
                std::tr1::placeholders::_1));
}

BookStoreInfoManagerImpl::~BookStoreInfoManagerImpl()
{
    pthread_mutex_destroy(&m_purchasedBooksLock);
}

FetchDataResult BookStoreInfoManagerImpl::FetchRecommendList(
        int start,
        int length)
{
    return m_recommendFetcher.FetchData(start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchBookList(
        ListDataFetcher& fetcher,
        int start,
        int length)
{
    return fetcher.FetchData(start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchDiscountBookList(
        int start,
        int length,
        FetchDataOrder order)
{
    m_discountBookListFetcher.SetFetchOrder(order);
    return FetchBookList(m_discountBookListFetcher, start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchFreeBookList(int start, int length)
{
    return FetchBookList(m_freeBookListFetcher, start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchCategoryList()
{
    return m_categoryListFetcher.FetchData(0, 100);
}

FetchDataResult BookStoreInfoManagerImpl::FetchCategoryTree()
{
    int upTime = dk::utility::SystemUtil::GetUpdateTimeInMs()/1000;
    if (m_categoryTreeInfo.lastUpdateTime == -1 || (upTime - m_categoryTreeInfo.lastUpdateTime) > 3600*m_categoryTreeInfoUpdateIntervalInHour)
    {
        return m_categoryTreeFetcher.FetchData(0, 100);
    }
    else
    {
        OnListDataUpdate(m_categoryTreeInfo, EventCategoryTreeUpdate);
    }
    return FDR_SUCCESSED;
}

FetchDataResult BookStoreInfoManagerImpl::FetchPublishList(int fetchCount)
{
    return m_publishingListFetcher.FetchData(0, fetchCount);
}

FetchDataResult BookStoreInfoManagerImpl::FetchPublishBookList(const char* publishId, int start, int length, FetchDataOrder order)
{
    m_publishingBookListFetcher.SetPublishId(publishId);
    m_publishingBookListFetcher.SetFetchOrder(order);
    return m_publishingBookListFetcher.FetchData(start, length);
}

const char* BookStoreInfoManager::EventRecommendListUpdate = "EventRecommendListUpdate";
const char* BookStoreInfoManager::EventDiscountListUpdate = "EventDiscountListUpdate";
const char* BookStoreInfoManager::EventFreeListUpdate = "EventFreeListUpdate";
const char* BookStoreInfoManager::EventTopicListUpdate = "EventTopicListUpdate";
const char* BookStoreInfoManager::EventTopicBookListUpdate = "EventTopicBookListUpdate";
const char* BookStoreInfoManager::EventCategoryListUpdate = "EventCategoryListUpdate";
const char* BookStoreInfoManager::EventCategoryTreeUpdate = "EventCategoryTreeUpdate";
const char* BookStoreInfoManager::EventCategoryBookListUpdate = "EventCategoryBookListUpdate";
const char* BookStoreInfoManager::EventPublishingListUpdate = "EventPublishingListUpdate";
const char* BookStoreInfoManager::EventPublishingBookListUpdate = "EventPublishingBookListUpdate";
const char* BookStoreInfoManager::EventBookInfoUpdate = "EventBookInfoUpdate";
const char* BookStoreInfoManager::EventSendBookKey = "EventSendBookKey";
const char* BookStoreInfoManager::EventExchangeDuokanKey = "EventExchangeDuokanKey";
const char* BookStoreInfoManager::EventCoverImageUpdate = "EventCoverImageUpdate";
const char* BookStoreInfoManager::EventPurchasedListUpdate = "EventPurchasedListUpdate";
const char* BookStoreInfoManager::EventDrmCertUpdate = "EventDrmCertUpdate";
const char* BookStoreInfoManager::EventCreateOrderUpdate = "EventCreateOrderUpdate";
const char* BookStoreInfoManager::EventMailOrderUpdate = "EventMailOrderUpdate";
const char* BookStoreInfoManager::EventSearchBookUpdate = "EventSearchBookUpdate";
const char* BookStoreInfoManager::EventHotFreeListUpdate = "EventHotFreeListUpdate";
const char* BookStoreInfoManager::EventHotPayListUpdate = "EventHotPayListUpdate";
const char* BookStoreInfoManager::EventFreshListUpdate = "EventFreshListUpdate";
const char* BookStoreInfoManager::EventMonthlyPayListUpdate = "EventMonthlyPayListUpdate";
const char* BookStoreInfoManager::EventFavorPayListUpdate = "EventFavorPayListUpdate";
const char* BookStoreInfoManager::EventDouBanRankListUpdate = "EventDouBanRankListUpdate";
const char* BookStoreInfoManager::EventAmazonRankListUpdate = "EventAmazonRankListUpdate";
const char* BookStoreInfoManager::EventJingDongRankListUpdate = "EventJingDongRankListUpdate";
const char* BookStoreInfoManager::EventDangDangRankListUpdate = "EventDangDangRankListUpdate";
const char* BookStoreInfoManager::EventAdsListUpdate = "EventAdsListUpdate";
const char* BookStoreInfoManager::EventRankAdsListUpdate = "EventRankAdsListUpdate";
const char* BookStoreInfoManager::EventCommentListUpdate = "EventCommentListUpdate";
const char* BookStoreInfoManager::EventHotCommentListUpdate = "EventHotCommentListUpdate";
const char* BookStoreInfoManager::EventOwnCommentListUpdate = "EventOwnCommentListUpdate";
const char* BookStoreInfoManager::EventBookScoreUpdate = "EventBookScoreUpdate";
const char* BookStoreInfoManager::EventAddBookScoreUpdate = "EventAddBookScoreUpdate";
const char* BookStoreInfoManager::EventAddBookCommentUpdate = "EventAddBookCommentUpdate";
const char* BookStoreInfoManager::EventVoteBookCommentUpdate = "EventVoteBookCommentUpdate";
const char* BookStoreInfoManager::EventAddBookCommentReplyUpdate = "EventAddBookCommentReplyUpdate";
const char* BookStoreInfoManager::EventBookScoreByCurrentUserUpdate = "EventBookScoreByCurrentUserUpdate";
const char* BookStoreInfoManager::EventGetReadingProgressUpdate = "EventGetReadProgressUpdate";
const char* BookStoreInfoManager::EventGetReadingDataUpdate = "EventGetReadingDataUpdate";
const char* BookStoreInfoManager::EventPutReadingDataUpdate = "EventPutReadingDataUpdate";
const char* BookStoreInfoManager::EventPutReadingDataUpdateNoNeedSyncToDKX = "EventPutReadingDataUpdateNoNeedSyncToDKX";
const char* BookStoreInfoManager::EventBookChangeLogListUpdate = "EventBookChangeLogListUpdate";
const char* BookStoreInfoManager::EventReportBookBugUpdate = "EventReportBookBugUpdate";
const char* BookStoreInfoManager::EventGetPersonalExperiencesUpdate = "EventGetPersonalExperiencesUpdate";
const char* BookStoreInfoManager::EventLocalNotesSummaryUpdate = "EventLocalNotesSummaryUpdate";
const char* BookStoreInfoManager::EventDuoKanNotesSummaryUpdate = "EventDuoKanNotesSummaryUpdate";
const char* BookStoreInfoManager::EventPostPersonalExperiencesUpdate = "EventPostPersonalExperiencesUpdate";
const char* BookStoreInfoManager::EventBoxMessageUnreadCountUpdate = "EventBoxMessageUnreadCountUpdate";
const char* BookStoreInfoManager::EventPostBoxMessageUnreadCountUpdate = "EventPostBoxMessageUnreadCountUpdate";
const char* BookStoreInfoManager::EventCommentReplyUpdate = "EventCommentReplyUpdate";
const char* BookStoreInfoManager::EventMessagesListUpdate = "EventMessagesListUpdate";
const char* BookStoreInfoManager::EventTagListMessagesUpdate = "EventTagListMessagesUpdate";

BookStoreInfoManager* BookStoreInfoManager::GetInstance()
{
    static BookStoreInfoManagerImpl manager;
    return &manager;
}

bool BookStoreInfoManagerImpl::OnListDataUpdate(
        const EventArgs& args,
        const char* eventName)
{
    const DataUpdateArgs& listDataUpdateArgs = (const DataUpdateArgs&)args;
    FireEvent(eventName, listDataUpdateArgs);
    return true;
}

bool BookStoreInfoManagerImpl::OnRecommendListUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventRecommendListUpdate);
}

bool BookStoreInfoManagerImpl::OnDiscountListUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventDiscountListUpdate);
}

bool BookStoreInfoManagerImpl::OnFreeListUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventFreeListUpdate);
}

bool BookStoreInfoManagerImpl::OnTopicListUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventTopicListUpdate);
}

bool BookStoreInfoManagerImpl::OnCategoryListUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventCategoryListUpdate);
}

bool BookStoreInfoManagerImpl::OnCategoryTreeUpdate(const EventArgs& args)
{
    if (((const CategoryTreeArgs&)(args)).succeeded)
    {
        m_categoryTreeInfo = (CategoryTreeArgs&)args;
        m_categoryTreeInfo.lastUpdateTime = dk::utility::SystemUtil::GetUpdateTimeInMs()/1000;
    }
    return OnListDataUpdate(args, EventCategoryTreeUpdate);
}

bool BookStoreInfoManagerImpl::OnPublishingListUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventPublishingListUpdate);
}

bool BookStoreInfoManagerImpl::OnPublishingBookListUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventPublishingBookListUpdate);
}

bool BookStoreInfoManagerImpl::OnTopicBookListUpdate(const EventArgs& args)
{
    const DataUpdateArgs& listDataUpdateArgs = (const DataUpdateArgs&)args;
    TopicBookListUpdateArgs topicBookListUpdateArgs(listDataUpdateArgs); 
    if (listDataUpdateArgs.succeeded)
    {
        topicBookListUpdateArgs.topicId = m_topicBookListFetcher.GetTopicId();
        topicBookListUpdateArgs.m_label = m_topicBookListFetcher.GetLabel();
        topicBookListUpdateArgs.m_description = m_topicBookListFetcher.GetDescription();
        topicBookListUpdateArgs.m_bannerUrl = m_topicBookListFetcher.GetBannerUrl();
        topicBookListUpdateArgs.succeeded = true;
    }
    FireEvent(EventTopicBookListUpdate, topicBookListUpdateArgs);
    return true;
}

bool BookStoreInfoManagerImpl::OnCategoryBookListUpdate(const EventArgs& args)
{
    const DataUpdateArgs& listDataUpdateArgs = (const DataUpdateArgs&)args;
    CategoryBookListUpdateArgs categoryBookListUpdateArgs(listDataUpdateArgs); 
    categoryBookListUpdateArgs.categoryId = m_categoryBookListFetcher.GetCategoryId();
    OnListDataUpdate(categoryBookListUpdateArgs, EventCategoryBookListUpdate);
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::FetchTopicList(int start, int length)
{
    return m_topicListFetcher.FetchData(start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchTopicBookList(int topicId, int start, int length)
{
    m_topicBookListFetcher.SetTopicId(topicId);
    return m_topicBookListFetcher.FetchData(start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchCategoryBookList(
        const char* categoryId,
        int start,
        int length,
        FetchDataOrder order)
{
    m_categoryBookListFetcher.SetCategoryId(categoryId);
    m_categoryBookListFetcher.SetFetchOrder(order);
    return m_categoryBookListFetcher.FetchData(start, length);
}

FetchDataResult BookStoreInfoManagerImpl::BeginDownloadBookInfo(const char* bookId)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildBookInfoTask(bookId);
    SubscribeDownloadEvent(
            task,
            &BookStoreInfoManagerImpl::OnBookInfoDownloadFinished);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

FetchDataResult BookStoreInfoManagerImpl::BeginSendBookKey(const char* key)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildBookKeySendTask(key);
    SubscribeDownloadEvent(task, &BookStoreInfoManagerImpl::OnBookKeySendFinished);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

FetchDataResult BookStoreInfoManagerImpl::BeginExchangeDuokanKey(const char* key)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildExchangeDuokanKeyTask(key);
    SubscribeDownloadEvent(task, &BookStoreInfoManagerImpl::OnExchangeDuokanKeyFinished);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

FetchDataResult BookStoreInfoManagerImpl::FetchBookInfo(const char* bookId)
{
    return BeginDownloadBookInfo(bookId);
}

FetchDataResult BookStoreInfoManagerImpl::FetchBookKeyInfo(const char* key)
{
    return BeginSendBookKey(key);
}

FetchDataResult BookStoreInfoManagerImpl::ExchangeDuokanKey(const char* key)
{
    return BeginExchangeDuokanKey(key);
}

bool BookStoreInfoManagerImpl::OnCoverImageDownloadFinished(const EventArgs& args)
{
    CoverImageUpdateArgs coverImageUpdateArgs;
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    if (downloadTaskFinishedArgs.succeeded)
    {
        coverImageUpdateArgs.succeeded = true;
        coverImageUpdateArgs.imageUrl = PathManager::GetBaseUrl(downloadTaskFinishedArgs.downloadTask->GetUrl());
    }
    FireEvent(EventCoverImageUpdate, coverImageUpdateArgs);
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::FetchCoverImage(
            const char* imgUrl)
{
    std::string imgFile = PathManager::CoverImageUrlToFile(imgUrl);
    if (PathManager::IsFileExisting(imgFile.c_str()))
    {
        return FDR_SUCCESSED;
    }
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildCoverImageTask(imgUrl);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(
            task,
            &BookStoreInfoManagerImpl::OnCoverImageDownloadFinished);
    DownloadManager::GetInstance()->AddTask(task);

    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::OnBookInfoDownloadFinished(const EventArgs& args)
{
    BookInfoUpdateArgs bookInfoUpdateArgs;
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    std::string url = PathManager::GetBaseUrl(downloadTaskFinishedArgs.downloadTask->GetUrl());
    bookInfoUpdateArgs.bookId = PathManager::GetFileName(url.c_str());
    if (downloadTaskFinishedArgs.succeeded)
    {
        model::BookInfoSPtr bookInfo = model::BookInfo::ParseBookInfo(
            downloadTaskFinishedArgs.downloadTask->GetString().c_str());
        if (bookInfo)
        {
            bookInfoUpdateArgs.succeeded = true;
            bookInfoUpdateArgs.bookInfo = bookInfo;
        }
    }

    FireEvent(EventBookInfoUpdate, bookInfoUpdateArgs);
    return true;
}

bool BookStoreInfoManagerImpl::OnBookKeySendFinished(const EventArgs& args)
{
    SendBookKeyUpdateArgs bookKeyUpdateArgs;
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    std::string downloadString = downloadTaskFinishedArgs.downloadTask->GetString();
    if (downloadTaskFinishedArgs.succeeded)
    {
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(downloadString.c_str());
        if (jsonObj)
        {
            jsonObj->GetStringValue("msg", &bookKeyUpdateArgs.msg);
            jsonObj->GetIntValue("result", &bookKeyUpdateArgs.errorCode);
        }

        model::BookInfoSPtr bookInfo = model::BookInfo::ParseBookInfo(downloadString.c_str());
        if (bookInfo)
        {
            bookKeyUpdateArgs.succeeded = true;
            bookKeyUpdateArgs.bookInfo = bookInfo;
        }
    }
    FireEvent(EventSendBookKey, bookKeyUpdateArgs);
    return true;
}

bool BookStoreInfoManagerImpl::OnExchangeDuokanKeyFinished(const EventArgs& args)
{
    ExchangeDuokanKeyUpdateArgs bookKeyUpdateArgs;
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    std::string downloadString = downloadTaskFinishedArgs.downloadTask->GetString();
    if (downloadTaskFinishedArgs.succeeded)
    {
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(downloadString.c_str());
        if (jsonObj)
        {
            jsonObj->GetIntValue("result", &bookKeyUpdateArgs.result);
            jsonObj->GetStringValue("message", &bookKeyUpdateArgs.message);
            jsonObj->GetStringValue("order_uuid", &bookKeyUpdateArgs.order_uuid);
        }
    }

    FireEvent(EventExchangeDuokanKey, bookKeyUpdateArgs);
    return true;
}

namespace
{
    struct SortItem
    {
        model::BasicObjectSPtr bookInfoOrg;
        model::BookInfoSPtr bookInfo;
        int status;
    };
    bool operator<(const SortItem& lhs, const SortItem& rhs)
    {
        if ((lhs.status & BS_NEEDSUPDATE) && (rhs.status & BS_NEEDSUPDATE))
        {
            return strcmp(
                    lhs.bookInfo->GetRevision().c_str(),
                    rhs.bookInfo->GetRevision().c_str()) > 0;
        }
        if ((lhs.status & BS_NEEDSUPDATE) && !(rhs.status & BS_NEEDSUPDATE))
        {
            return true;
        } 
        else if (!(lhs.status & BS_NEEDSUPDATE) && (rhs.status & BS_NEEDSUPDATE))
        {
            return false;
        }
        else
        {
            return lhs.bookInfo->GetPurchasedUTCTime() > rhs.bookInfo->GetPurchasedUTCTime();
        }
    }
}

bool BookStoreInfoManagerImpl::OnPurchasedListUpdate(const EventArgs& args)
{
    const DataUpdateArgs& dataUpdateArgs = (const DataUpdateArgs&)args;
    if (dataUpdateArgs.succeeded)
    {
        std::vector<SortItem> sortItems;
        {
            AutoLock lock(&m_purchasedBooksLock);
            m_purchasedBooks.clear();
            m_purchasedBooks = model::BookInfo::FromBasicObjectList(dataUpdateArgs.dataList);
            sortItems.reserve(m_purchasedBooks.size());
            for (size_t i = 0; i < m_purchasedBooks.size(); ++i)
            {
                SortItem item;
                item.bookInfo = m_purchasedBooks[i];
                item.bookInfoOrg = dataUpdateArgs.dataList[i];
                sortItems.push_back(item);
            }
        }

        for (size_t i = 0; i < sortItems.size(); ++i)
        {
            sortItems[i].status = GetBookStatus(sortItems[i].bookInfo);
        }
        std::sort(sortItems.begin(), sortItems.end());
        
        {
            AutoLock lock(&m_purchasedBooksLock);
            for (size_t i = 0; i < sortItems.size(); ++i)
            {
                m_purchasedBooks[i] = sortItems[i].bookInfo;
                dataUpdateArgs.dataList[i] = sortItems[i].bookInfoOrg;
            }
        }
    }

    return OnListDataUpdate(args, EventPurchasedListUpdate);
}

FetchDataResult BookStoreInfoManagerImpl::FetchPurchasedList()
{
	BookStoreOrderManager::GetInstance()->FetchHideBookList();
    return FetchBookList(m_purchasedListFetcher, 0, 10000);
}

bool BookStoreInfoManagerImpl::OnCreateOrderFinished(const EventArgs& args)
{
    CreateOrderUpdateArgs createOrderArgs;
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs 
        = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    std::string bookId = task->GetPostValue(ConstStrings::BOOK_UUID);
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string resultString = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(resultString.c_str());
        if (jsonObj)
        {
            model::CreateOrderResult* createOrderResult = 
                model::CreateOrderResult::CreateCreateOrderResult(jsonObj.get());
            if (NULL != createOrderResult)
            {
                createOrderArgs.result = model::CreateOrderResultSPtr(createOrderResult);
                if (createOrderArgs.result)
                {
                    createOrderArgs.result->SetBookId(bookId.c_str());
                    createOrderArgs.succeeded = true;
                    if (!createOrderResult->IsSuccessful())
                    {
                        DebugPrintf(DLC_DIAGNOSTIC,
                                "Create order result: %s, current user:%s, bookId: %s",
                                resultString.c_str(),
                                GetCurrentUserName().c_str(),
                                bookId.c_str());
                    }
                }
            }
        }
    }
    else
    {
        createOrderArgs.result.reset(new model::CreateOrderResult()); 
        createOrderArgs.result->SetBookId(bookId.c_str());
    }
    FireEvent(EventCreateOrderUpdate, createOrderArgs); 
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::CreateOrder(const char* bookId)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildCreateOrderTask(bookId);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task,
            &BookStoreInfoManagerImpl::OnCreateOrderFinished);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

FetchDataResult BookStoreInfoManagerImpl::FetchDrmCert(
            const char* bookId,
            const char* bookRevision,
            const char* orderId)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildDrmCertTask(
            bookId,
            bookRevision,
            orderId);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task,
            &BookStoreInfoManagerImpl::OnDrmCertFinished);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::ParseDrmCert(const char* jsonData, std::string* cert)
{
    JsonObjectSPtr jsonObject = JsonObject::CreateFromString(jsonData);
    int result = -1;
    return jsonObject
            && jsonObject->GetIntValue("result", &result) 
            && 0 == result
            && jsonObject->GetStringValue("cert", cert);
}


bool BookStoreInfoManagerImpl::OnDrmCertFinished(const EventArgs& args)
{
    DrmCertUpdateArgs drmCertArgs;

    {
        const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
        DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
        drmCertArgs.bookId = task->GetPostValue(ConstStrings::BOOK_UUID);
        if (downloadTaskFinishedArgs.succeeded)
        {
            if (ParseDrmCert(task->GetString().c_str(), &(drmCertArgs.cert)))
            {
                drmCertArgs.succeeded = true;
                drmCertArgs.userId = task->GetPostValue(ConstStrings::USER_ID);
                drmCertArgs.orderId = task->GetPostValue(ConstStrings::ORDER_UUID);
                drmCertArgs.bookRevision = task->GetPostValue(ConstStrings::BOOK_REVISION);
                drmCertArgs.magicId = task->GetPostValue(ConstStrings::MAGIC_ID);
                if (WriteInfoFile(drmCertArgs))
                {
                    drmCertArgs.succeeded = true;
                }
            }
        }
    }

    FireEvent(EventDrmCertUpdate, drmCertArgs);
    return true;
}

bool BookStoreInfoManagerImpl::OnMailOrderFinished(const EventArgs& args)
{
    MailOrderUpdateArgs mailOrderUpdateArgs;
    {
        const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
        mailOrderUpdateArgs.orderId = downloadTaskFinishedArgs.downloadTask->GetPostValue(ConstStrings::ORDER_UUID);
        if (downloadTaskFinishedArgs.succeeded)
        {
            DownloadTask* task = downloadTaskFinishedArgs.downloadTask;

            JsonObjectSPtr jsonObject = JsonObject::CreateFromString(
                    task->GetString().c_str());
            int result = -1;
            if (jsonObject->GetIntValue("result", &result) && 0 == result)
            {
                mailOrderUpdateArgs.succeeded = true;
            }
        }
    }
    FireEvent(EventMailOrderUpdate, mailOrderUpdateArgs);
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::MailOrder(const char* orderId)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildMailOrderTask(orderId);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task,
            &BookStoreInfoManagerImpl::OnMailOrderFinished);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::WriteInfoFile(model::BookInfoSPtr bookInfo, const char* orderId)
{
    ConfigFile config;
    std::string revision = bookInfo->GetRevision();
    if (revision.empty())
    {
        model::BookInfoSPtr purchasedBookInfo = GetPurchasedBookInfo(bookInfo->GetBookId().c_str());
        if (purchasedBookInfo)
        {
            revision = purchasedBookInfo->GetRevision();
        }
    }


    config.SetString(ConstStrings::BOOK_UUID, bookInfo->GetBookId().c_str());
    config.SetString(ConstStrings::USER_ID, GetCurrentUserName().c_str());
    config.SetString(ConstStrings::ORDER_UUID, orderId);
    config.SetString(ConstStrings::BOOK_REVISION, revision.c_str());
    config.SetString(ConstStrings::BOOK_TITLE, bookInfo->GetTitle().c_str());
    config.SetString(ConstStrings::BOOK_AUTHOR, bookInfo->GetAuthorsOrEditors().c_str());
    return config.SaveToFile(PathManager::BookIdToInfoFile(bookInfo->GetBookId().c_str()).c_str());
}

bool BookStoreInfoManagerImpl::WriteTrialInfoFile(model::BookInfoSPtr bookInfo)
{
    ConfigFile config;
    config.SetString(ConstStrings::BOOK_UUID, bookInfo->GetBookId().c_str());
    config.SetString(ConstStrings::BOOK_REVISION, bookInfo->GetRevision().c_str());
    config.SetInt(ConstStrings::IS_TRIAL, 1); 
    config.SetString(ConstStrings::BOOK_TITLE, bookInfo->GetTitle().c_str());
    config.SetString(ConstStrings::BOOK_AUTHOR, bookInfo->GetAuthorsOrEditors().c_str());
    return config.SaveToFile(PathManager::BookIdToInfoFile(bookInfo->GetBookId().c_str()).c_str());
}

bool BookStoreInfoManagerImpl::WriteInfoFile(const DrmCertUpdateArgs& drmArgs)
{
    ConfigFile config;
    config.SetString(ConstStrings::BOOK_UUID, drmArgs.bookId.c_str());
    config.SetString(ConstStrings::USER_ID, drmArgs.userId.c_str());
    config.SetString(ConstStrings::ORDER_UUID, drmArgs.orderId.c_str());
    config.SetString(ConstStrings::BOOK_REVISION, drmArgs.bookRevision.c_str());
    config.SetString(ConstStrings::CERT, drmArgs.cert.c_str());
    config.SetString(ConstStrings::MAGIC_ID, drmArgs.magicId.c_str());
    config.SetString(ConstStrings::BOOK_TITLE, model::BookInfo::BookIdToTitle(drmArgs.bookId.c_str()).c_str());
    return config.SaveToFile(PathManager::BookIdToInfoFile(drmArgs.bookId.c_str()).c_str());
}

FetchDataResult BookStoreInfoManagerImpl::FetchReadingProgress(const char* bookId, bool IsDKBookStoreBook)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildGetReadingProgressTask(bookId, IsDKBookStoreBook);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task,
            &BookStoreInfoManagerImpl::OnFetchReadingProgressFinished);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

FetchDataResult BookStoreInfoManagerImpl::FetchReadingData(const char* bookId, int dataVersion, bool IsDKBookStoreBook)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildGetReadingDataTask(bookId, dataVersion, IsDKBookStoreBook);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task,
            &BookStoreInfoManagerImpl::OnFetchReadingDataFinished);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::OnFetchReadingProgressFinished(const EventArgs& args)
{
    FetchReadingProgressUpdateArgs fetchReadingProgressUpdateArgs;
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs =
        (const DownloadTaskFinishArgs&)args;

    if (downloadTaskFinishedArgs.succeeded)
    {
        fetchReadingProgressUpdateArgs.succeeded = true;
        XMLDomDocument doc;
        std::string result = downloadTaskFinishedArgs.downloadTask->GetString();
        if (doc.LoadXmlFromBuffer(result.c_str(), result.size()))
        {
            const XMLDomNode* rootNode = doc.RootElement();
            fetchReadingProgressUpdateArgs.result.InitFromDomNode(rootNode);
        }
    }
    FireEvent(EventGetReadingProgressUpdate, fetchReadingProgressUpdateArgs);
    return true;
}

bool BookStoreInfoManagerImpl::OnFetchReadingDataFinished(const EventArgs& args)
{
    ReadingDataSyncUpdateArgs readingDataSyncUpdateArgs;
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs =
        (const DownloadTaskFinishArgs&)args;

    if (downloadTaskFinishedArgs.succeeded)
    {
        readingDataSyncUpdateArgs.succeeded = true;
        XMLDomDocument doc;
        m_bookNote = downloadTaskFinishedArgs.downloadTask->GetString();
        if (doc.LoadXmlFromBuffer(m_bookNote.c_str(), m_bookNote.size()))
        {
            const XMLDomNode* rootNode = doc.RootElement();
            readingDataSyncUpdateArgs.result.InitFromDomNode(rootNode);
        }
    }
    FireEvent(EventGetReadingDataUpdate, readingDataSyncUpdateArgs);
    return true;
}


FetchDataResult BookStoreInfoManagerImpl::FetchSearchBookList(const char* query, int start, int length)
{
    m_searchBookListFetcher.SetQuery(query);
    return m_searchBookListFetcher.FetchData(start, length);
}

bool BookStoreInfoManagerImpl::OnSearchBookUpdate(const EventArgs& args)
{
    const DataUpdateArgs& listDataUpdateArgs = (const DataUpdateArgs&)args;
    SearchBookUpdateArgs searchBookUpdateArgs(listDataUpdateArgs);
    if (listDataUpdateArgs.succeeded)
    {
        searchBookUpdateArgs.query = m_searchBookListFetcher.GetQuery();
        searchBookUpdateArgs.succeeded = true;
    }
    FireEvent(EventSearchBookUpdate, searchBookUpdateArgs);
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::FetchHotFreeBookList(
        int start,
        int length)
{
    return FetchBookList(m_hotFreeListFetcher, start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchFreshBookList(
        int start,
        int length)
{
    return FetchBookList(m_freshBookListFetcher, start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchHotPayBookList(
        int start,
        int length)
{
    return FetchBookList(m_hotPayListFetcher, start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchHotMonthlyBookList(
    int start,
    int length)
{
    return FetchBookList(m_hotMonthLyListFetcher, start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchHotFavorBookList(
    int start,
    int length)
{
    return FetchBookList(m_hotFavorListFetcher, start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchDouBanRankBookList(int start, int length)
{
    return FetchBookList(m_doubanRankListFetcher, start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchAmazonRankBookList(int start, int length)
{
    return FetchBookList(m_amazonRankListFetcher, start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchJingDongRankBookList(int start, int length)
{
    return FetchBookList(m_jingdongRankListFetcher, start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchDangDangRankBookList(int start, int length)
{
    return FetchBookList(m_dangdangRankListFetcher, start, length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchBookChangeLogList(
    const char* bookId,
    int start,
    int length)
{
    m_bookChangeLogListFetcher.SetBookId(bookId);
    return FetchBookList(m_bookChangeLogListFetcher, start, length);
}

bool BookStoreInfoManagerImpl::OnHotFreeUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventHotFreeListUpdate);
}

bool BookStoreInfoManagerImpl::OnHotPayUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventHotPayListUpdate);
}

bool BookStoreInfoManagerImpl::OnFreshUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventFreshListUpdate);
}

bool BookStoreInfoManagerImpl::OnMonthlyPayUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventMonthlyPayListUpdate);
}

bool BookStoreInfoManagerImpl::OnFavorPayUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventFavorPayListUpdate);
}

bool BookStoreInfoManagerImpl::OnDouBanRankListUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventDouBanRankListUpdate);
}

bool BookStoreInfoManagerImpl::OnAmazonRankListUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventAmazonRankListUpdate);
}

bool BookStoreInfoManagerImpl::OnJingDongRankListUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventJingDongRankListUpdate);
}

bool BookStoreInfoManagerImpl::OnDangDangRankListUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventDangDangRankListUpdate);
}

model::BookInfoSPtr BookStoreInfoManagerImpl::FindPurchasedBookInfo(const char* bookId)
{
    {
        AutoLock lock(&m_purchasedBooksLock);
        //if (!CAccountManager::GetInstance()->IsLoggedIn())
        //{
        //    m_purchasedBooks.clear();
        //}
        for (size_t i = 0; i < m_purchasedBooks.size(); ++i)
        {
            if (strcasecmp(m_purchasedBooks[i]->GetBookId().c_str(), bookId) == 0)
            {
                return m_purchasedBooks[i];
            }
        }
    }
	return BookStoreOrderManager::GetInstance()->GetBookInfoFromHideList(bookId);
}

bool BookStoreInfoManagerImpl::RemoveBookFromPurchasedList(const char* bookId)
{
	AutoLock lock(&m_purchasedBooksLock);
	for(std::vector<model::BookInfoSPtr>::iterator itr = m_purchasedBooks.begin(); itr != m_purchasedBooks.end(); itr++)
	{
		if((*itr)->GetBookId()== bookId)
		{
			m_purchasedBooks.erase(itr);
			return true;
		}
	}
	return false;
}

model::BookInfoList BookStoreInfoManagerImpl::GetCachedPurchasedBookList()
{
    AutoLock lock(&m_purchasedBooksLock);
	return m_purchasedBooks;
}

model::BookInfoList BookStoreInfoManagerImpl::GetCachedPurchasedBookListByState(int request)
{
    AutoLock lock(&m_purchasedBooksLock);
    model::BookInfoList results;
    if (!m_purchasedBooks.empty())
    {
        for (size_t i = 0; i < m_purchasedBooks.size(); ++i)
        {
            model::BookInfoSPtr bookInfo = m_purchasedBooks[i];
            int status = GetBookStatus(bookInfo);
            if (status & request)
            {
                results.push_back(bookInfo);
            }
        }
    }
    return results;
}

std::string BookStoreInfoManagerImpl::GetLocalFileTitle(const char* bookId)
{
    std::string infoFilePath = PathManager::BookIdToInfoFile(bookId);
    ConfigFile config;
    std::string result;
    if (config.LoadFromFile(infoFilePath.c_str()))
    {
        config.GetString(ConstStrings::BOOK_TITLE, &result);
    }
    return result; 
}

std::string BookStoreInfoManagerImpl::GetLocalFileAuthor(const char* bookId)
{
    std::string infoFilePath = PathManager::BookIdToInfoFile(bookId);
    ConfigFile config;
    std::string result;
    if (config.LoadFromFile(infoFilePath.c_str()))
    {
        config.GetString(ConstStrings::BOOK_AUTHOR, &result);
    }
    return result; 
}

bool BookStoreInfoManagerImpl::IsLocalFileTrial(const char* bookId)
{
    std::string infoFilePath = PathManager::BookIdToInfoFile(bookId);
    ConfigFile config;
    std::string result;
    int isTrial = 0;
    if (config.LoadFromFile(infoFilePath.c_str()))
    {
        config.GetInt(ConstStrings::IS_TRIAL, &isTrial);
    }
    return isTrial != 0;
}

std::string BookStoreInfoManagerImpl::GetLocalFileRevision(const char* bookId)
{
    std::string infoFilePath = PathManager::BookIdToInfoFile(bookId);
    ConfigFile config;
    std::string result;
    if (config.LoadFromFile(infoFilePath.c_str()))
    {
        config.GetString(ConstStrings::BOOK_REVISION, &result);
    }
    return result; 
}

int BookStoreInfoManagerImpl::GetBookStatus(const char* bookId)
{
    model::BookInfoSPtr purchasedBookInfo = FindPurchasedBookInfo(bookId);
    return GetBookStatus(purchasedBookInfo);
}

int BookStoreInfoManagerImpl::GetBookStatus(model::BookInfoSPtr bookInfo)
{
    if (bookInfo == 0)
    {
        return BS_UNPURCHASED;
    }

    std::string bookId = bookInfo->GetBookId();
    model::BookInfoSPtr purchasedBookInfo = FindPurchasedBookInfo(bookId.c_str());
    if (purchasedBookInfo == 0)
    {
        return BS_UNPURCHASED;
    }

    bool infoFileExisting = PathManager::IsFileExisting(PathManager::BookIdToInfoFile(bookInfo->GetBookId()).c_str());
    if (!infoFileExisting)
    {
        int ret = bookInfo->GetFileUrl().empty() ? BS_UNPURCHASED : BS_UNDOWNLOAD;
        int discountPrice = bookInfo->GetDiscountPrice();
        int price = bookInfo->GetPrice();

        // 原价不为零且打折的书
        if(price <= 0 || discountPrice <= 0)
        {
            ret = ret | BS_FREE;
        }
        return ret;
    }

    // Return undownloaded if the trial version exists
    if (IsLocalFileTrial(bookId.c_str()))
    {
        if (bookInfo != 0)
        {
            return BS_UNDOWNLOAD;
        }
    }

    int download_state = BookDownloadController::GetInstance()->GetBookDownloadStatus(bookInfo);
    if (download_state == BS_UNKNOWN)
    {
        IDownloadTask::DLState downloader_status = 
            DownloaderImpl::GetInstance()->GetTaskStateByUrlId(BuildUrlIdForBook(bookId.c_str()));
        switch (downloader_status)
        {
            case IDownloadTask::WAITING:
            case IDownloadTask::WORKING:
            case IDownloadTask::PAUSED:
                return BS_DOWNLOADING;
            default:
                break;

        }
    }
    else
    {
        return download_state;
    }

    bool epubFileExisting = PathManager::IsFileExisting(PathManager::BookIdToEpubFile(bookId).c_str());
    if (!epubFileExisting)
    {
        if (bookInfo)
        {
            return BS_UNDOWNLOAD;
        }
    }

    if (bookInfo)
    {
        std::string revision = GetLocalFileRevision(bookId.c_str());
        std::string latestRevision = purchasedBookInfo->GetRevision();
        if (revision != latestRevision)
        {
            return BS_NEEDSUPDATE;
        }
    }
    return BS_DOWNLOADED;
}

FetchDataResult BookStoreInfoManagerImpl::FetchTrialBook(model::BookInfoSPtr bookInfo)
{

    assert(!bookInfo->GetTrialUrl().empty());
	IDownloadBookTask *booktask = DownloadTaskFactory::CreateBookDownloadTask(
		bookInfo->GetTrialUrl().c_str(),
		BuildUrlIdForTrialVersion(bookInfo->GetBookId().c_str()).c_str(),
		bookInfo->GetAuthorsOrEditors(),
        bookInfo->GetSummary(),
		"DUOKAN_STORE",
		bookInfo->GetBookId() + ".epub",
		0,
		"",
		3,
		"",
		PathManager::BookIdToEpubFile(bookInfo->GetBookId().c_str())
		);
    booktask->SetDisplayName(bookInfo->GetTitle() + "(" + StringManager::GetPCSTRById(BOOKSTORE_TRIALVERSION) + ")");

	IDownloader * downloadManager = DownloaderImpl::GetInstance();
	if (downloadManager)
	{
		downloadManager->AddTask(booktask);
		return FDR_PENDING;
	}
	return FDR_FAILED;
}

FetchDataResult BookStoreInfoManagerImpl::FetchBook(model::BookInfoSPtr bookInfo)
{
    if (!bookInfo || bookInfo->GetFileUrl().empty())
    {
        return FDR_FAILED;
    }

    // Delete trial version
    CDKFileManager *pfilemanager = CDKFileManager::GetFileManager();
    string bookPath = PathManager::BookIdToEpubFile(bookInfo->GetBookId().c_str());
    int fileId = pfilemanager->GetFileIdByPath(bookPath.c_str());
    if (fileId >= 0)
    {
        PCDKFile file = pfilemanager->GetFileById(fileId);
        if (file)
        {
            pfilemanager->RemoveFileNode(file);
        }
        unlink(bookPath.c_str());
        unlink(string(bookPath).append(".dkpt").c_str());
    }

	IDownloadBookTask *booktask = DownloadTaskFactory::CreateBookDownloadTask(
		bookInfo->GetFileUrl(),
        BuildUrlIdForBook(bookInfo->GetBookId().c_str()),
		bookInfo->GetAuthorsOrEditors(),
        bookInfo->GetSummary(),
		"DUOKAN_STORE",
		bookInfo->GetBookId() + ".epub",
		0,
		"",
		3,
		"",
		PathManager::BookIdToEpubFile(bookInfo->GetBookId().c_str())
		);
        booktask->SetDisplayName(bookInfo->GetTitle());

	IDownloader * downloadManager = DownloaderImpl::GetInstance();
    downloadManager->DeleteTask(
        BuildUrlIdForTrialVersion(bookInfo->GetBookId().c_str()).c_str());
    downloadManager->DeleteTask(
            BuildUrlIdForBook(bookInfo->GetBookId().c_str()).c_str());
    downloadManager->AddTask(booktask);

	BookStoreOrderManager* storeManager = BookStoreOrderManager::GetInstance();
	if(storeManager && storeManager->IsBookInHideList(bookInfo->GetBookId().c_str()))
	{
		model::BookInfoList hideBookList;
        hideBookList.push_back(model::BookInfoSPtr(dynamic_cast<model::BookInfo*>(bookInfo->Clone())));
		storeManager->RevealHideBook(hideBookList);
	}
    return FDR_PENDING;
}

TrialBookStatus BookStoreInfoManagerImpl::GetTrialBookStatus(const char* bookId)
{
    model::BookInfoSPtr purchasedBookInfo = FindPurchasedBookInfo(bookId);
    if (purchasedBookInfo)
    {
        return TBS_PURCHASED;
    }
    bool epubFileExisting = PathManager::IsFileExisting(PathManager::BookIdToEpubFile(bookId).c_str());
    if (epubFileExisting)
    {
        if (IsLocalFileTrial(bookId))
        {
            return TBS_DOWNLOADED;
        }
        else
        {
            // Do not display trial button if the user purchased release version.
			return TBS_PURCHASED;
        }
    }

    bool infoFileExisting = PathManager::IsFileExisting(PathManager::BookIdToInfoFile(bookId).c_str());
    if (!infoFileExisting)
    {
        return TBS_UNDOWNLOAD;
    }

    IDownloadTask::DLState dlState =
        DownloaderImpl::GetInstance()->GetTaskStateByUrlId(
                    BuildUrlIdForTrialVersion(bookId).c_str());

    switch (dlState)
    {
        case IDownloadTask::WAITING:
        case IDownloadTask::WORKING:
        case IDownloadTask::PAUSED:
            return TBS_DOWNLOADING;
        default:
            break;

    }
    return TBS_UNDOWNLOAD;
}

FetchDataResult BookStoreInfoManagerImpl::FetchAds()
{
    return m_adsListFetcher.FetchData(0, 4);
}

FetchDataResult BookStoreInfoManagerImpl::FetchRankAds()
{
    return m_rankAdsListFetcher.FetchData(0, 4);
}

bool BookStoreInfoManagerImpl::OnAdsListUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventAdsListUpdate);
}

bool BookStoreInfoManagerImpl::OnRankAdsListUpdate(const EventArgs& args)
{
    return OnListDataUpdate(args, EventRankAdsListUpdate);
}

FetchDataResult BookStoreInfoManagerImpl::PutReadingProgress(
        const char* bookId,
        const char* bookName,
        const ReadingBookInfo& readingBookInfo,
        int dataVersion,
        bool IsDKBookStoreBook)
{
    std::string postData = readingBookInfo.ToPostData();
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildPutReadingProgressTask(
            bookId,
            bookName,
            postData.c_str(),
            dataVersion,
            IsDKBookStoreBook);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(
            task,
            &BookStoreInfoManagerImpl::OnPutReadingProgressUpdate);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

FetchDataResult BookStoreInfoManagerImpl::PutReadingData(
        const char* bookId,
        const char* bookName,
        int dataVersion,
        const ReadingBookInfo& readingBookInfo,
        bool IsDKBookStoreBook,
        bool IsNeedSyncToDKXInCallBack)
{
    std::string postData = readingBookInfo.ToPostData();
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildPutReadingDataTask(
        bookId,
        bookName,
        dataVersion,
        postData.c_str(),
        IsDKBookStoreBook);
    if (!task)
    {
        return FDR_FAILED;
    }
    if (IsNeedSyncToDKXInCallBack)
    {
        SubscribeDownloadEvent(task,
            &BookStoreInfoManagerImpl::OnPutReadingDataUpdateSyncToDKX);
    }
    else
    {
        SubscribeDownloadEvent(task,
            &BookStoreInfoManagerImpl::OnPutReadingDataUpdateNoSyncToDKX);
    }
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}


bool BookStoreInfoManagerImpl::OnPutReadingProgressUpdate(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs =
        (const DownloadTaskFinishArgs&)args;
    if (downloadTaskFinishedArgs.succeeded)
    {
        XMLDomDocument doc;
        std::string result = downloadTaskFinishedArgs.downloadTask->GetString();
        if (doc.LoadXmlFromBuffer(result.c_str(), result.size()))
        {
            //const XMLDomNode* rootNode = doc.RootElement();
        }
    }
    return true;
}

bool BookStoreInfoManagerImpl::OnPutReadingDataUpdateSyncToDKX(const EventArgs& args)
{
    return OnPutReadingDataUpdate(args, true);
}

bool BookStoreInfoManagerImpl::OnPutReadingDataUpdateNoSyncToDKX(const EventArgs& args)
{
    return OnPutReadingDataUpdate(args, false);
}

bool BookStoreInfoManagerImpl::OnPutReadingDataUpdate(const EventArgs& args, bool needSyncToDKX)
{
    ReadingDataSyncUpdateArgs readingDataSyncUpdateArgs;
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs =
        (const DownloadTaskFinishArgs&)args;
    if (downloadTaskFinishedArgs.succeeded)
    {
        readingDataSyncUpdateArgs.succeeded = true;
        XMLDomDocument doc;
        std::string result = downloadTaskFinishedArgs.downloadTask->GetString();
        if (doc.LoadXmlFromBuffer(result.c_str(), result.size()))
        {
            const XMLDomNode* rootNode = doc.RootElement();
            readingDataSyncUpdateArgs.result.InitFromDomNode(rootNode);
        }
    }
    if (needSyncToDKX)
    {
        FireEvent(EventPutReadingDataUpdate, readingDataSyncUpdateArgs);
    }
    else
    {
        FireEvent(EventPutReadingDataUpdateNoNeedSyncToDKX, readingDataSyncUpdateArgs);
    }
    return true;
}

int BookStoreInfoManagerImpl::GetPurchasedBookSize()
{
    return m_purchasedBooks.size();
}

model::BookInfoSPtr BookStoreInfoManagerImpl::GetPurchasedBookInfo(const char* bookId)
{
    return FindPurchasedBookInfo(bookId);
}

bool BookStoreInfoManagerImpl::OnAccountLoginEvent(const EventArgs& args)
{
    const AccountEventArgs& loginEvent = (const AccountEventArgs&)args;
    if (loginEvent.IsLoggedIn())
    {
        FetchPurchasedList();
    }
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::FetchBookScore(const char* bookId)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildBookScoreTask(bookId);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task,
            &BookStoreInfoManagerImpl::OnBookScoreFinished);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::OnBookScoreFinished(const EventArgs& args)
{
    FetchBookScoreUpdateArgs fetchBookScoreUpdateArgs;
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs =
        (const DownloadTaskFinishArgs&)args;
    const DownloadTask* downloadTask = downloadTaskFinishedArgs.downloadTask;
    fetchBookScoreUpdateArgs.bookId = HttpUtil::GetParaFromUrl(downloadTask->GetUrl(), "book_id");
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = downloadTask->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        if (jsonObj.get())
        {
            model::BookScoreSPtr bookScore(model::BookScore::CreateBookScore(jsonObj.get()));
            if (bookScore.get())
            {
                fetchBookScoreUpdateArgs.succeeded = true;
                fetchBookScoreUpdateArgs.bookScore = bookScore;
            }
        }
    }
    FireEvent(EventBookScoreUpdate, fetchBookScoreUpdateArgs);
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::FetchCommentList(const char* bookId, int start, int length, FetchDataOrder order)
{
    m_commentsListFetcher.SetBookId(bookId);
    m_commentsListFetcher.SetFetchOrder(order);
    ++start;
    return m_commentsListFetcher.FetchData(start, length);
}

void BookStoreInfoManagerImpl::SetReceivedMessageID(const char* messageID)
{
    m_pushedMessagesFetcher.SetMessageID(messageID);
}

FetchDataResult BookStoreInfoManagerImpl::FetchOwnCommentsList(int start, int length)
{
    return m_ownCommentsListFetcher.FetchData((start + 1), length);
}

FetchDataResult BookStoreInfoManagerImpl::FetchHotCommentsList(int start, int length)
{
    return m_hotCommentsListFetcher.FetchData((start + 1), length);
}

bool BookStoreInfoManagerImpl::OnCommentListUpdate(const EventArgs& args)
{
    CommentListUpdateArgs commentListUpdateArgs((const DataUpdateArgs&)args);
    --commentListUpdateArgs.startIndex;
    commentListUpdateArgs.bookId = m_commentsListFetcher.GetBookId();
    if (commentListUpdateArgs.succeeded)
    {
        commentListUpdateArgs.myScore = m_commentsListFetcher.GetUserLastScore();
    }
    return OnListDataUpdate(commentListUpdateArgs, EventCommentListUpdate);
}

bool BookStoreInfoManagerImpl::OnOwnCommentsListUpdate(const EventArgs& args)
{
    CommentListUpdateArgs commentListUpdateArgs((const DataUpdateArgs&)args);
    --commentListUpdateArgs.startIndex;
    return OnListDataUpdate(commentListUpdateArgs, EventOwnCommentListUpdate);
}

bool BookStoreInfoManagerImpl::OnHotCommentsListUpdate(const EventArgs& args)
{
    CommentListUpdateArgs commentListUpdateArgs((const DataUpdateArgs&)args);
    --commentListUpdateArgs.startIndex;
    return OnListDataUpdate(commentListUpdateArgs, EventHotCommentListUpdate);
}

bool BookStoreInfoManagerImpl::OnBookChangeLogListUpdate(const EventArgs& args)
{
    BookChangeLogListArgs bookChangeLogListUpdateArgs((const DataUpdateArgs&)args);
    bookChangeLogListUpdateArgs.bookId = m_bookChangeLogListFetcher.GetBookId();
    return OnListDataUpdate(bookChangeLogListUpdateArgs, EventBookChangeLogListUpdate);
}

FetchDataResult BookStoreInfoManagerImpl::FetchPushedMessageList()
{
    //m_pushedMessagesFetcher.FetchData(0, 100);
    //m_pushedMessagesFetcher.ClearMessageID();
    return FDR_SUCCESSED;
}

int BookStoreInfoManagerImpl::GetPushedMessageInterval()
{
    return m_pushedMessagesFetcher.GetInterval();
}

bool BookStoreInfoManagerImpl::OnPushedMessagesUpdate(const EventArgs& args)
{
    PushedMessagesManager* pMessagesMgr = PushedMessagesManager::GetInstance();
    if (pMessagesMgr)
    {
        //return pMessagesMgr->OnPushedMessagesUpdate(args);
    }
    return false;
}

//BoxMessages start
FetchDataResult BookStoreInfoManagerImpl::FetchBoxMessageUnreadCount(const char* messageType, int bookSerial)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildGetBoxMessagesUnreadCountTask(messageType, bookSerial);
    SubscribeDownloadEvent(task, &BookStoreInfoManagerImpl::OnBoxMessageUnreadCountInfoUpdate);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::OnBoxMessageUnreadCountInfoUpdate(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    BoxMessagesUnreadCountUpdateArgs unreadCountArgs;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        int resultCode = -1;
        if (jsonObj.get() 
            && jsonObj->GetIntValue("result", &resultCode) 
            && 0 == resultCode)
        {
            unreadCountArgs.succeeded = true; 
            jsonObj->GetStringValue("deletion_threshold", &unreadCountArgs.deletionThreshold);
            jsonObj->GetIntValue("count", &unreadCountArgs.count);
            jsonObj->GetIntValue("interval", &unreadCountArgs.interval);
        }
    }
    FireEvent(EventBoxMessageUnreadCountUpdate, unreadCountArgs);
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::UpdateBoxMessageUnreadCount(const char* messageType, const char* deletionThreshold)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildPostBoxMessagesUnreadCountTask(messageType, deletionThreshold);
    SubscribeDownloadEvent(task, &BookStoreInfoManagerImpl::OnPostBoxMessageUnreadCountUpdate);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::OnPostBoxMessageUnreadCountUpdate(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    PostBoxMessagesUnreadCountUpdateArgs unreadCountArgs;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        int resultCode = -1;
        if (jsonObj.get() 
            && jsonObj->GetIntValue("result", &resultCode) )
        {
            unreadCountArgs.result = resultCode;
            unreadCountArgs.succeeded = (0 == resultCode); 
        }
    }
    FireEvent(EventPostBoxMessageUnreadCountUpdate, unreadCountArgs);
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::FetchBoxMessageList(const char* messageType, int start, int length)
{
    m_boxMessagesFetcher.SetMessageType(messageType);
    m_boxMessagesFetcher.FetchData((start + 1), length);
    return FDR_SUCCESSED;
}

bool BookStoreInfoManagerImpl::OnBoxMessagesUpdate(const EventArgs& args)
{
    DataUpdateArgs listUpdateArgs((const DataUpdateArgs&)args);
    --listUpdateArgs.startIndex;
    return OnListDataUpdate(listUpdateArgs, EventMessagesListUpdate);
}
//BoxMessages end

//tag start
FetchDataResult BookStoreInfoManagerImpl::FetchTagList(const char* tagName, int start, int length)
{
    m_tagListFetcher.SetTagName(tagName);
    m_tagListFetcher.FetchData(start, length);
    return FDR_SUCCESSED;
}

bool BookStoreInfoManagerImpl::OnTagListUpdate(const EventArgs& args)
{
    DataUpdateArgs listUpdateArgs((const DataUpdateArgs&)args);
    return OnListDataUpdate(listUpdateArgs, EventTagListMessagesUpdate);
}
//tag end

//comment start
FetchDataResult BookStoreInfoManagerImpl::AddBookScore(const char* bookId, int score)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildAddBookScoreTask(bookId, score);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task,
            &BookStoreInfoManagerImpl::OnAddBookScoreUpdate);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::OnAddBookScoreUpdate(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs =
        (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    AddBookScoreUpdateArgs addBookScoreArgs;
    addBookScoreArgs.bookId = task->GetPostValue("book_id");
    addBookScoreArgs.score = atoi(task->GetPostValue("score").c_str()) << 1;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        int resultCode = -1;
        if (jsonObj.get() 
                && jsonObj->GetIntValue("result", &resultCode) 
                && 0 == resultCode)
        {
           addBookScoreArgs.succeeded = true; 
        }
    }
    FireEvent(EventAddBookScoreUpdate, addBookScoreArgs);
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::AddBookComment(
        const char* bookId, 
        int score,
        const char* title,
        const char* content)
{
    if (NULL == bookId || NULL == title || NULL == content)
    {
        return FDR_FAILED;
    }
    DownloadTaskSPtr task = 
        DownloadTaskBuilder::BuildAddBookCommentTask(
                bookId, score, title, content);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task,
            &BookStoreInfoManagerImpl::OnAddBookCommentUpdate);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::OnAddBookCommentUpdate(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs =
        (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    AddBookCommentUpdateArgs addBookCommentArgs;
    addBookCommentArgs.bookId = task->GetPostValue("book_id");
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        int resultCode = -1;
        if (jsonObj)
        {
            if (jsonObj->GetIntValue("result", &resultCode) 
                    && 0 == resultCode)
            {
                addBookCommentArgs.succeeded = true; 
            }
            jsonObj->GetStringValue("info", &addBookCommentArgs.errorInfo);
        }
    }
    FireEvent(EventAddBookCommentUpdate, addBookCommentArgs);
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::AddBookCommentReply(
        const char* bookId, 
        const char* commentId,
        const char* content)
{
    if (NULL == bookId || NULL == commentId || NULL == content)
    {
        return FDR_FAILED;
    }
    DownloadTaskSPtr task = 
        DownloadTaskBuilder::BuildAddBookCommentReplyTask(
                bookId,
                commentId,
                content);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task,
            &BookStoreInfoManagerImpl::OnAddBookCommentReplyUpdate);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::OnAddBookCommentReplyUpdate(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs =
        (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    AddBookCommentReplyUpdateArgs addBookCommentReplyArgs;
    addBookCommentReplyArgs.bookId = task->GetPostValue("book_id");
    addBookCommentReplyArgs.commentId = task->GetPostValue("comment_id");

    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        int resultCode = -1;
        if (jsonObj)
        {
            if (jsonObj->GetIntValue("result", &resultCode) 
                    && 0 == resultCode)
            {
                addBookCommentReplyArgs.succeeded = true; 
            }
            jsonObj->GetStringValue("info", &addBookCommentReplyArgs.errorInfo);
        }
    }
    FireEvent(EventAddBookCommentReplyUpdate, addBookCommentReplyArgs);
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::VoteBookComment(
        const char* bookId, 
        const char* commentId,
        bool useful)
{
    if (NULL == bookId || NULL == commentId)
    {
        return FDR_FAILED;
    }
    DownloadTaskSPtr task = 
        DownloadTaskBuilder::BuildVoteBookCommentTask(
                bookId,
                commentId,
                useful);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task,
            &BookStoreInfoManagerImpl::OnVoteBookCommentUpdate);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::OnVoteBookCommentUpdate(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs =
        (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    VoteBookCommentUpdateArgs voteBookCommentUpdateArgs;
    voteBookCommentUpdateArgs.bookId = task->GetPostValue("book_id");
    voteBookCommentUpdateArgs.commentId = task->GetPostValue("comment_id");

    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        if (jsonObj)
        {
            if (jsonObj->GetIntValue("result", &voteBookCommentUpdateArgs.errorCode) 
                    && 0 == voteBookCommentUpdateArgs.errorCode)
            {
                voteBookCommentUpdateArgs.succeeded = true; 
            }
            jsonObj->GetStringValue("info", &voteBookCommentUpdateArgs.errorInfo);
            jsonObj->GetIntValue("useful", &voteBookCommentUpdateArgs.useful);
            jsonObj->GetIntValue("useless", &voteBookCommentUpdateArgs.useless);
        }
    }
    FireEvent(EventVoteBookCommentUpdate, voteBookCommentUpdateArgs);
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::FetchBookScoreByCurrentUser(const char* bookId)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildCommentListTask(bookId, 1, 1);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task,
            &BookStoreInfoManagerImpl::OnFetchBookScoreByCurrentUserUpdate);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

FetchDataResult BookStoreInfoManagerImpl::FetchCommentReply(const char* bookId, const char* commentId, int start, int length)
{
    DownloadTaskSPtr task = DownloadTaskBuilder::BuildCommentReplyTask(bookId, commentId, start, length);
    if (task)
    {
        SubscribeDownloadEvent(
            task,
            &BookStoreInfoManagerImpl::OnCommentReplyUpdate);
        DownloadManager::GetInstance()->AddTask(task);
        return FDR_PENDING;
    }
    return FDR_FAILED;
}

bool BookStoreInfoManagerImpl::OnCommentReplyUpdate(const EventArgs& args)
{
    CommentReplyArgs commentReplyArgs;
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    if (downloadTaskFinishedArgs.succeeded && task)
    {
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(task->GetString().c_str());
        if (!jsonObj)
        {
            return false;
        }
        model::BookComment* commentReply = model::BookComment::CreateBookComment(jsonObj.get(), model::OT_MYCOMMENTS);
        if (NULL != commentReply)
        {
            if (jsonObj->GetIntValue("result", &commentReplyArgs.errorCode) 
                && 0 == commentReplyArgs.errorCode)
            {
                commentReplyArgs.succeeded = true; 
            }
            commentReplyArgs.commentInfo = model::BookCommentSPtr(commentReply);
        }
    }

    FireEvent(EventCommentReplyUpdate, commentReplyArgs);
    return true;
}

bool BookStoreInfoManagerImpl::OnFetchBookScoreByCurrentUserUpdate(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs =
        (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    BookScoreByCurrentUserArgs bookScoreByCurrentUserArgs;
    std::string url = task->GetUrl();
    bookScoreByCurrentUserArgs.bookId = HttpUtil::GetParaFromUrl(url.c_str(), "book_id");
    bookScoreByCurrentUserArgs.userId = HttpUtil::GetParaFromUrl(url.c_str(), "user_id");

    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        if (jsonObj)
        {
            if (jsonObj->GetIntValue("result", &bookScoreByCurrentUserArgs.errorCode) 
                    && 0 == bookScoreByCurrentUserArgs.errorCode)
            {
                bookScoreByCurrentUserArgs.succeeded = true; 
            }
            jsonObj->GetIntValue("user_last_score", &bookScoreByCurrentUserArgs.myScore);
        }
    }
    FireEvent(EventBookScoreByCurrentUserUpdate, bookScoreByCurrentUserArgs);
    return true;
}
//comment end

FetchDataResult BookStoreInfoManagerImpl::ReportBookBug(
    const char* bookId,
    const char* bookName,
    const char* bookRevision,
    const char* position,
    const char* ref_chapter,
    const char* reference,
    const char* suggestion,
    bool bConvertCht)
{
    if (NULL == bookId || NULL == bookName || NULL == bookRevision || NULL == reference || NULL == suggestion)
    {
        return FDR_FAILED;
    }
    DownloadTaskSPtr task = 
        DownloadTaskBuilder::BuildReportBookBugTask(
        bookId, bookName, bookRevision, position, ref_chapter, reference, suggestion, bConvertCht);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task,
        &BookStoreInfoManagerImpl::OnReportBookBugUpdate);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::OnReportBookBugUpdate(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs =
        (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    ReportBookBugUpdateArgs reportBookBugArgs;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        //DebugPrintf(DLC_DIAGNOSTIC, "Report Book Bug:%s", result.c_str());
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        int code = -1;
        if (jsonObj)
        {
            JsonObjectSPtr statusObj = jsonObj->GetSubObject("status");
            if (statusObj)
            {
                if (statusObj->GetIntValue("code", &code) 
                    && 0 == code)
                {
                    reportBookBugArgs.succeeded = true; 
                }
                statusObj->GetStringValue("message", &reportBookBugArgs.message);
            }
        }
    }
    else
    {
        // Sending failed, the correction data should be restored to offline manager
        OfflineBookStoreManager* pBSOfflineManager = OfflineBookStoreManager::GetInstance();
        std::string bookId = task->GetPostValue("bookid");
        std::string bookName = task->GetPostValue("bookname");
        std::string bookRevision = task->GetPostValue("revision");
        std::string position = task->GetPostValue("position");
        std::string refChapter = task->GetPostValue("ref_chapter");
        std::string reference = task->GetPostValue("reference");
        std::string suggestion = task->GetPostValue("suggestion");
        bool bConvertCht = task->GetPostValue("convertcht").compare("1") == 0 ? true : false;
        pBSOfflineManager->ReportedBookBug(bookId.c_str(),
            bookName.c_str(),
            bookRevision.c_str(),
            position.c_str(),
            refChapter.c_str(),
            reference.c_str(),
            suggestion.c_str(),
            bConvertCht);
    }
    FireEvent(EventReportBookBugUpdate, reportBookBugArgs);
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::FetchPersonalExperiences()
{
	DownloadTaskSPtr task = DownloadTaskBuilder::BuildPersonalExperiencesTask();
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task, &BookStoreInfoManagerImpl::OnFetchPersonalExperiencesUpdate);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

void BookStoreInfoManagerImpl::OnPersonalExperiencesUpdate(const EventArgs& args, bool isGet)
{
	FetchPersonalExperiencesArgs fetchPersonalExperiencesArgs;
	const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        fetchPersonalExperiencesArgs.errorCode = -1;
        if (jsonObj)
        {
            JsonObjectSPtr statusObj = jsonObj->GetSubObject("status");
            if (statusObj)
            {
                if (statusObj->GetIntValue("code", &fetchPersonalExperiencesArgs.errorCode))
                {
                	JsonObjectSPtr jsonData = jsonObj->GetSubObject("data");
					if(jsonData.get())
					{
						string readingData = jsonData->GetJsonString();
						fetchPersonalExperiencesArgs.succeeded = (0 == fetchPersonalExperiencesArgs.errorCode);
    					fetchPersonalExperiencesArgs.jsonReadingData = readingData;
					}
                }
            }
        }
    }
	if(isGet)
	{
		FireEvent(EventGetPersonalExperiencesUpdate, fetchPersonalExperiencesArgs);
	}
	else
	{
		FireEvent(EventPostPersonalExperiencesUpdate, fetchPersonalExperiencesArgs);
	}
}

FetchDataResult BookStoreInfoManagerImpl::PostPersonalExperiences(const char* newData)
{
	DownloadTaskSPtr task = DownloadTaskBuilder::BuildUploadPersonalExperiencesTask(newData);
    if (!task)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(task, &BookStoreInfoManagerImpl::OnPostPersonalExperiencesUpdate);
    DownloadManager::GetInstance()->AddTask(task);
    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::OnFetchPersonalExperiencesUpdate(const EventArgs& args)
{
	OnPersonalExperiencesUpdate(args, true);
    return true;
}

bool BookStoreInfoManagerImpl::OnPostPersonalExperiencesUpdate(const EventArgs& args)
{
    OnPersonalExperiencesUpdate(args, false);
    return true;
}

FetchDataResult BookStoreInfoManagerImpl::FetchAllBookNotesSummary()
{
    DownloadTaskSPtr localBookNotesSummarytask = DownloadTaskBuilder::BuildGetLocalNoteSummaryTask();
    DownloadTaskSPtr duokanBookNotesSummarytask = DownloadTaskBuilder::BuildGetDuoKanNoteSummaryTask();
    if (!localBookNotesSummarytask || !duokanBookNotesSummarytask)
    {
        return FDR_FAILED;
    }
    SubscribeDownloadEvent(localBookNotesSummarytask, &BookStoreInfoManagerImpl::OnFetchLocalBookNoteSummary);
    SubscribeDownloadEvent(duokanBookNotesSummarytask, &BookStoreInfoManagerImpl::OnFetchDuoKanBookNoteSummary);
    DownloadManager::GetInstance()->AddTask(localBookNotesSummarytask);
    DownloadManager::GetInstance()->AddTask(duokanBookNotesSummarytask);
    return FDR_PENDING;
}

bool BookStoreInfoManagerImpl::OnFetchLocalBookNoteSummary(const EventArgs& args)
{
    return OnFetchBookNoteSummary(args, false);
}

bool BookStoreInfoManagerImpl::OnFetchDuoKanBookNoteSummary(const EventArgs& args)
{
    return OnFetchBookNoteSummary(args, true);
}

bool BookStoreInfoManagerImpl::OnFetchBookNoteSummary(const EventArgs& args, bool isDuoKanBookStoreBook)
{
    BookNoteSummaryArgs bookNoteSummaryArgs;
	const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        bookNoteSummaryArgs.Init(jsonObj.get(), isDuoKanBookStoreBook);
    }
    if (isDuoKanBookStoreBook)
    {
        FireEvent(EventLocalNotesSummaryUpdate, bookNoteSummaryArgs);
    }
    else
    {
        FireEvent(EventDuoKanNotesSummaryUpdate, bookNoteSummaryArgs);
    }

    return true;
}

} // namespace bookstore
} // namespace dk
