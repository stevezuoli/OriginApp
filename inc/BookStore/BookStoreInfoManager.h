#ifndef __BOOKSTORE_BOOKSTOREINFOMANAGER_H__
#define __BOOKSTORE_BOOKSTOREINFOMANAGER_H__

#include <vector>
#include "BookStore/BasicObject.h"
#include "BookStore/BookInfo.h"
#include "BookStore/TopicInfo.h"
#include "BookStore/CategoryInfo.h"
#include "BookStore/CreateOrderResult.h"
#include "GUI/EventSet.h"
#include "GUI/EventArgs.h"
#include "BookStore/BookStoreTypes.h"
#include "BookStore/DataUpdateArgs.h"
#include "BookStore/ReadingBookInfo.h"
#include "BookStore/BookComment.h"
#include "BookStore/BookScore.h"
#include "BookStore/BookNoteSummary.h"
#include "Utility/JsonObject.h"

using namespace dk::bookstore::sync;

namespace dk
{
namespace bookstore
{

class TopicBookListUpdateArgs: public DataUpdateArgs
{
public:
    TopicBookListUpdateArgs(const DataUpdateArgs& dataUpdateArgs)
        :DataUpdateArgs(dataUpdateArgs)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new TopicBookListUpdateArgs(*this);
    }
    int topicId;
    std::string m_label;
    std::string m_description;
    std::string m_bannerUrl;
};

class CategoryBookListUpdateArgs: public DataUpdateArgs
{
public:
    CategoryBookListUpdateArgs(const DataUpdateArgs& dataUpdateArgs)
        : DataUpdateArgs(dataUpdateArgs)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new CategoryBookListUpdateArgs(*this);
    }
    std::string categoryId;
};

class BookInfoUpdateArgs: public EventArgs
{
public:
    BookInfoUpdateArgs()
        :succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new BookInfoUpdateArgs(*this);
    }

    bool succeeded;
    model::BookInfoSPtr bookInfo;
    std::string bookId;
};

class SendBookKeyUpdateArgs: public EventArgs
{
public:
    SendBookKeyUpdateArgs()
        : succeeded(false)
        , errorCode(0)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new SendBookKeyUpdateArgs(*this);
    }

    bool succeeded;
    int errorCode;
    model::BookInfoSPtr bookInfo;
    std::string msg;
};

class ExchangeDuokanKeyUpdateArgs: public EventArgs
{
public:
    ExchangeDuokanKeyUpdateArgs()
        : result(0)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new ExchangeDuokanKeyUpdateArgs(*this);
    }

    int result;
    std::string order_uuid;
    std::string message;
};

class CoverImageUpdateArgs: public EventArgs
{
public:
    CoverImageUpdateArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new CoverImageUpdateArgs(*this);
    }
    bool succeeded;
    std::string imageUrl;
};

class CreateOrderUpdateArgs: public EventArgs
{
public:
    CreateOrderUpdateArgs()
        : succeeded(false)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new CreateOrderUpdateArgs(*this);
    }
    bool succeeded;
    model::CreateOrderResultSPtr result;
};

class DrmCertUpdateArgs: public EventArgs
{
public:
    DrmCertUpdateArgs()
        : succeeded(false) {}
    virtual EventArgs* Clone() const { return new DrmCertUpdateArgs(*this); }
    bool        succeeded;
    std::string cert;
    std::string userId;
    std::string orderId;
    std::string bookId;
    std::string bookRevision;
    std::string magicId;
    std::string bookTitle;
};

class MailOrderUpdateArgs: public EventArgs
{
public:
    MailOrderUpdateArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new MailOrderUpdateArgs(*this);
    }

    bool succeeded;
    std::string orderId;
};

class SearchBookUpdateArgs: public DataUpdateArgs
{
public:
    SearchBookUpdateArgs(const DataUpdateArgs& dataUpdateArgs)
        : DataUpdateArgs(dataUpdateArgs)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new SearchBookUpdateArgs(*this);
    }

    std::string query;
};

class CommentListUpdateArgs: public DataUpdateArgs
{
public:
    CommentListUpdateArgs(const DataUpdateArgs& dataUpdateArgs)
        : DataUpdateArgs(dataUpdateArgs)
        , myScore(-1)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new CommentListUpdateArgs(*this);
    }
    std::string bookId;
    int myScore;
};

class CommentReplyArgs: public EventArgs
{
public:
    CommentReplyArgs()
        : succeeded(false)
        , errorCode(0)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new CommentReplyArgs(*this);
    }

    bool succeeded;
    int errorCode;
    model::BookCommentSPtr commentInfo;
};

class FetchBookScoreUpdateArgs: public EventArgs
{
public:
    FetchBookScoreUpdateArgs()
        : succeeded(false)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new FetchBookScoreUpdateArgs(*this);
    }
    bool succeeded;
    model::BookScoreSPtr bookScore;
    std::string bookId;
};


class FetchReadingProgressUpdateArgs: public EventArgs
{
public:
    FetchReadingProgressUpdateArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new FetchReadingProgressUpdateArgs(*this);
    }
    bool succeeded;
    ReadingProgressGetResult result;
};

class ReadingDataSyncUpdateArgs: public EventArgs
{
public:
    ReadingDataSyncUpdateArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new ReadingDataSyncUpdateArgs(*this);
    }
    bool succeeded;
    ReadingDataSyncResult result;
};

class AddBookScoreUpdateArgs: public EventArgs
{
public:
    AddBookScoreUpdateArgs()
        : succeeded(false)
        , score(0)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new AddBookScoreUpdateArgs(*this);
    }
    bool succeeded;
    std::string bookId;
    int score;
};

class AddBookCommentUpdateArgs: public EventArgs
{
public:
    AddBookCommentUpdateArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new AddBookCommentUpdateArgs(*this);
    }
    bool succeeded;
    std::string bookId;
    std::string errorInfo;
};

class ReportBookBugUpdateArgs: public EventArgs
{
public:
    ReportBookBugUpdateArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new ReportBookBugUpdateArgs(*this);
    }
    bool succeeded;
    std::string message;
};

class AddBookCommentReplyUpdateArgs: public EventArgs
{
public:
    AddBookCommentReplyUpdateArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new AddBookCommentReplyUpdateArgs(*this);
    }
    bool succeeded;
    std::string bookId;
    std::string commentId;
    std::string errorInfo;
};

class VoteBookCommentUpdateArgs: public EventArgs
{
public:
    VoteBookCommentUpdateArgs()
        : succeeded(false)
        , errorCode(0)
        , useful(-1)
        , useless(-1)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new VoteBookCommentUpdateArgs(*this);
    }
    bool succeeded;
    int errorCode;
    std::string bookId;
    std::string commentId;
    std::string errorInfo;
    int useful;
    int useless;
};

class BookScoreByCurrentUserArgs: public EventArgs
{
public:
    BookScoreByCurrentUserArgs()
        : succeeded(false)
        , errorCode(0)
        , myScore(-1)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new BookScoreByCurrentUserArgs(*this);
    }

    bool succeeded;
    int errorCode;
    int myScore;
    std::string errorInfo;
    std::string bookId;
    std::string userId;
};

class BookChangeLogListArgs: public DataUpdateArgs
{
public:
    BookChangeLogListArgs(const DataUpdateArgs& dataUpdateArgs)
        : DataUpdateArgs(dataUpdateArgs)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new BookChangeLogListArgs(*this);
    }
    std::string bookId;
};


class FetchPersonalExperiencesArgs: public EventArgs
{
public:
	FetchPersonalExperiencesArgs()
		: succeeded(false)
	{
	}
	
	virtual EventArgs* Clone() const
	{
		return new FetchPersonalExperiencesArgs(*this);
	}
	
	bool succeeded;
	int errorCode;
	std::string jsonReadingData;
};

class CategoryTreeArgs : public DataUpdateArgs
{
public:
    CategoryTreeArgs()
        : lastUpdateTime(-1)
    {
    };

    virtual EventArgs* Clone() const
    {
        return new CategoryTreeArgs(*this);
    }

    bool GetChildrenCategoryOfId(const char* categoryId, std::vector<model::BasicObjectSPtr>* childrenList) const;
    int lastUpdateTime;
};

class BookNoteSummaryArgs : public DataUpdateArgs
{
public:
    BookNoteSummaryArgs()
        : totalBookMarkCount(-1)
        , totalCommentCount(-1)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new BookNoteSummaryArgs(*this);
    }

    bool Init(const JsonObject* jsonObject, bool isDKBook);
private:
    int totalBookMarkCount;
    int totalCommentCount;
    std::string message;
};//BookNoteSummaryArgs

//BoxMessages start
class BoxMessagesUnreadCountUpdateArgs: public EventArgs
{
public:
    BoxMessagesUnreadCountUpdateArgs()
        : succeeded(false)
        , count(0)
        , interval(15)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new BoxMessagesUnreadCountUpdateArgs(*this);
    }
    std::string deletionThreshold;
    bool succeeded;
    int count;
    int interval;
};

class PostBoxMessagesUnreadCountUpdateArgs: public EventArgs
{
public:
    PostBoxMessagesUnreadCountUpdateArgs()
        : succeeded(false)
        , result(0)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new PostBoxMessagesUnreadCountUpdateArgs(*this);
    }
    bool succeeded;
    int result;
};
//BoxMessages end

class BookStoreInfoManager: public EventSet
{
public:
    // with DataUpdateArgs
    static const char* EventRecommendListUpdate;
    static const char* EventDiscountListUpdate;
    static const char* EventFreeListUpdate;
    static const char* EventTopicListUpdate;
    static const char* EventHotFreeListUpdate;
    static const char* EventHotPayListUpdate;
    static const char* EventFreshListUpdate;
    static const char* EventMonthlyPayListUpdate;
    static const char* EventFavorPayListUpdate;
    static const char* EventDouBanRankListUpdate;
    static const char* EventAmazonRankListUpdate;
    static const char* EventJingDongRankListUpdate;
    static const char* EventDangDangRankListUpdate;

    // publishing
    static const char* EventPublishingListUpdate;
    static const char* EventPublishingBookListUpdate;

    // with TopicBookListUpdateArgs
    static const char* EventTopicBookListUpdate;

    static const char* EventCategoryListUpdate;
    static const char* EventCategoryTreeUpdate;

    // with CategoryBookListUpdateArgs
    static const char* EventCategoryBookListUpdate;

    // BoolInfoUpdateArgs
    static const char* EventBookInfoUpdate;

    // SendBookKeyUpdateArgs
    static const char* EventSendBookKey;

    // ExchangeDuokanKeyUpdateArgs
    static const char* EventExchangeDuokanKey;

    // CoverImageUpdateArgs
    static const char* EventCoverImageUpdate;

    // CreateOrderUpdateArgs
    static const char* EventCreateOrderUpdate;
    
    // CoverPurchasedListUpdate
    static const char* EventPurchasedListUpdate;
    // DrmCertUpdateArgs
    static const char* EventDrmCertUpdate;

    // MailOrderUpdateArgs
    static const char* EventMailOrderUpdate;

    // FetchReadingProgressUpdateArgs
    static const char* EventGetReadingProgressUpdate;

    // ReadingDataSyncUpdateArgs
    static const char* EventGetReadingDataUpdate;

    // ReadingDataSyncUpdateArgs
    static const char* EventPutReadingDataUpdate;

    static const char* EventPutReadingDataUpdateNoNeedSyncToDKX;

    static const char* EventSearchBookUpdate;

    static const char* EventAdsListUpdate;

    static const char* EventRankAdsListUpdate;

    static const char* EventCommentListUpdate;

    static const char* EventOwnCommentListUpdate;

    static const char* EventHotCommentListUpdate;
    static const char* EventMessagesListUpdate;

    static const char* EventBookScoreUpdate;

    static const char* EventAddBookScoreUpdate;

    static const char* EventAddBookCommentUpdate;

    static const char* EventVoteBookCommentUpdate;

    static const char* EventAddBookCommentReplyUpdate;

    static const char* EventBookScoreByCurrentUserUpdate;
    static const char* EventCommentReplyUpdate;

    static const char* EventBookChangeLogListUpdate;

    static const char* EventReportBookBugUpdate;

	static const char* EventPostPersonalExperiencesUpdate;

	static const char* EventGetPersonalExperiencesUpdate;

    static const char* EventLocalNotesSummaryUpdate;
    static const char* EventDuoKanNotesSummaryUpdate;
    //

    // LoginUpdateArgs
    //static const char* EventLoginUpdate;

    virtual ~BookStoreInfoManager(){};

    // start from 0
    // if data in local, return succeeded
    // orig data in recommendList will be cleared,
    // recommendList will contain data from start to xx
    virtual FetchDataResult FetchRecommendList(
            int start,
            int length) = 0;
    virtual FetchDataResult FetchDiscountBookList(
            int start,
            int length,
            FetchDataOrder order = FDO_DEFAULT) = 0;
    virtual FetchDataResult FetchFreeBookList(
            int start,
            int length) = 0;
    virtual FetchDataResult FetchTopicList(
            int start,
            int length) = 0;
    virtual FetchDataResult FetchTopicBookList(
            int topicId,
            int start,
            int length) = 0;
    virtual FetchDataResult FetchCategoryList() = 0;
    virtual FetchDataResult FetchCategoryTree() = 0;
    virtual FetchDataResult FetchPublishList(int fetchCount) = 0;
    virtual FetchDataResult FetchPublishBookList(const char* publishId, int start, int length, FetchDataOrder order = FDO_DEFAULT) = 0;
    virtual FetchDataResult FetchCategoryBookList(
            const char* categoryId,
            int start,
            int length,
            FetchDataOrder order = FDO_DEFAULT) = 0;
    virtual FetchDataResult FetchBookInfo(const char* bookId) = 0;
    virtual FetchDataResult FetchBookKeyInfo(const char* key) = 0;
    virtual FetchDataResult ExchangeDuokanKey(const char* key) = 0;
    // if return FDR_SUCCESSED
    // caller should use PathManager::CoverImageUrlToFile 
    // to get local file
    virtual FetchDataResult FetchCoverImage(
            const char* imgUrl) = 0;
    virtual FetchDataResult FetchDrmCert(
            const char* bookId,
            const char* bookRevision,
            const char* orderId) = 0;
    virtual FetchDataResult FetchPurchasedList() = 0;
    virtual FetchDataResult CreateOrder(const char* bookId) = 0;
    virtual FetchDataResult MailOrder(const char* orderId) = 0;
    virtual FetchDataResult FetchReadingProgress(const char* bookId, bool IsDKBookStoreBook) = 0;
    virtual FetchDataResult FetchReadingData(const char* bookId, int dataVersion, bool IsDKBookStoreBook) = 0;
    virtual FetchDataResult FetchSearchBookList(
            const char* query,
            int start,
            int length) = 0;

    virtual FetchDataResult FetchHotFreeBookList(
            int start,
            int length) = 0;

    virtual FetchDataResult FetchHotPayBookList(
            int start,
            int length) = 0;

    virtual FetchDataResult FetchHotMonthlyBookList(
        int start,
        int length) = 0;

    virtual FetchDataResult FetchHotFavorBookList(
        int start,
        int length) = 0;

    virtual FetchDataResult FetchDouBanRankBookList(
        int start,
        int length) = 0;

    virtual FetchDataResult FetchAmazonRankBookList(
        int start,
        int length) = 0;

    virtual FetchDataResult FetchJingDongRankBookList(
        int start,
        int length) = 0;

    virtual FetchDataResult FetchDangDangRankBookList(
        int start,
        int length) = 0;

    virtual FetchDataResult FetchFreshBookList(
            int start,
            int length) = 0;

    virtual FetchDataResult FetchBookChangeLogList(
            const char* bookId,
            int start,
            int length) = 0;
    virtual std::string GetCurrentUserName() = 0;
    virtual std::string GetCurrentToken() = 0;

    virtual int GetBookStatus(const char* bookId) = 0;
    virtual int GetBookStatus(model::BookInfoSPtr bookInfo) = 0;
    virtual TrialBookStatus GetTrialBookStatus(const char* bookId) = 0;

    static BookStoreInfoManager* GetInstance();
    virtual bool IsLocalFileTrial(const char* bookId) = 0;
    virtual std::string GetLocalFileRevision(const char* bookId) = 0;
    virtual std::string GetLocalFileTitle(const char* bookId) = 0;
    virtual std::string GetLocalFileAuthor(const char* bookId) = 0;
    virtual FetchDataResult FetchBook(model::BookInfoSPtr bookInfo) = 0;
    virtual FetchDataResult FetchTrialBook(model::BookInfoSPtr bookInfo) = 0;
    virtual bool WriteInfoFile(model::BookInfoSPtr bookInfo, const char* orderId) = 0;
    virtual bool WriteTrialInfoFile(model::BookInfoSPtr bookInfo) = 0;
    virtual FetchDataResult FetchAds() = 0;
    virtual FetchDataResult FetchRankAds() = 0;
    virtual FetchDataResult PutReadingProgress(
            const char* bookId,
            const char* bookName,
            const ReadingBookInfo& readingBookInfo,
            int dataVersion,
            bool IsDKBookStoreBook) = 0;
    virtual FetchDataResult PutReadingData(
            const char* bookId,
            const char* bookName,
            int dataVersion,
            const ReadingBookInfo& readingBookInfo,
            bool IsDKBookStoreBook,
            bool IsNeedSyncToDKXInCallBack = true) = 0;
    virtual int GetPurchasedBookSize() = 0;
    virtual model::BookInfoSPtr GetPurchasedBookInfo(const char* bookId) = 0;
	virtual bool RemoveBookFromPurchasedList(const char* bookId) = 0;
	virtual model::BookInfoList GetCachedPurchasedBookList() = 0;
    virtual model::BookInfoList GetCachedPurchasedBookListByState(int request) = 0;

    //comment start
    virtual FetchDataResult FetchBookScore(const char* bookId) = 0;
    virtual FetchDataResult FetchCommentList(const char* bookId, int start, int length, FetchDataOrder order=FDO_DEFAULT) = 0;
    virtual FetchDataResult FetchOwnCommentsList(int start, int length) = 0;
    virtual FetchDataResult FetchHotCommentsList(int start, int length) = 0;
    // score from 1 to 5
    virtual FetchDataResult AddBookScore(const char* bookId, int score) = 0;
    virtual FetchDataResult AddBookComment(const char* bookId,
            int score,
            const char* title,
            const char* content) = 0;
    virtual FetchDataResult AddBookCommentReply(
            const char* bookId,
            const char* commentId,
            const char* content) = 0;
    virtual FetchDataResult VoteBookComment(
            const char* bookId,
            const char* commentId,
            bool useful) = 0;

    virtual FetchDataResult FetchBookScoreByCurrentUser( const char* bookId) = 0;
    virtual FetchDataResult FetchCommentReply(const char* bookId, const char* commentId, int start, int length) = 0;
    //comment end

    virtual FetchDataResult ReportBookBug(const char* bookId,
        const char* bookName,
        const char* bookRevision,
        const char* position,
        const char* ref_chapter,
        const char* reference,
        const char* suggestion,
        bool bConvertCht = false) = 0;
    virtual void SetReceivedMessageID(const char* messageID) = 0;
    virtual FetchDataResult FetchPushedMessageList() = 0;
    virtual int GetPushedMessageInterval() = 0;

    //BoxMessages start
    static const char* EventBoxMessageUnreadCountUpdate;
    static const char* EventPostBoxMessageUnreadCountUpdate;
    virtual FetchDataResult FetchBoxMessageUnreadCount(const char* messageType, int bookSerial) = 0;
    virtual FetchDataResult UpdateBoxMessageUnreadCount(const char* messageType, const char* deletionThreshold) = 0;
    virtual FetchDataResult FetchBoxMessageList(const char* messageType, int start, int length) = 0;
    //BoxMessages end

    //tag start
    static const char* EventTagListMessagesUpdate;
    virtual FetchDataResult FetchTagList(const char* tagName, int start, int length) = 0;
    //tag end

    virtual FetchDataResult FetchPersonalExperiences() = 0;
    virtual FetchDataResult PostPersonalExperiences(const char* newData) = 0;

    virtual FetchDataResult FetchAllBookNotesSummary() = 0;

    // Batch Tasks
    static std::string BuildUrlIdForBook(const char* bookId)
    {
        return bookId;
    }

    static std::string BuildUrlIdForTrialVersion(const char* bookId)
    {
        return bookId + std::string("_trial");
    }
    virtual const std::string& GetBookNote() const = 0;

};

} // namespace bookstore
} // namespace dk
#endif
