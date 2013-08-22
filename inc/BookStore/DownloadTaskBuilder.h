#ifndef __BOOKSTORE_DOWNLOADTASKBUILDER_H__
#define __BOOKSTORE_DOWNLOADTASKBUILDER_H__

#include <string>
#include "DownloadManager/DownloadTask.h"
#include "BookStore/BookInfo.h"
#include "BookStore/BookStoreTypes.h"

namespace dk
{
namespace bookstore
{

class DownloadTaskBuilder
{
public:
    static DownloadTaskSPtr BuildRecommendTask(int start, int length, FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildDiscountTask(int start, int length, FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildFreeTask(int start, int length, FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildTopicListTask(int start, int length, FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildTopicBookListTask(
            int topicId,
            int start,
            int length,
            FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildCategoryListTask(
            int start,
            int length,
            FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildCategoryTreeTask(
            int start,
            int length,
            FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildCategoryBookListTask(
            const char* categoryId,
            int start,
            int length,
            FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildBookInfoTask(const char* bookId);
    static DownloadTaskSPtr BuildBookKeySendTask(const char* key);
    static DownloadTaskSPtr BuildExchangeDuokanKeyTask(const char* key);
    static DownloadTaskSPtr BuildCoverImageTask(const char* coverImageUrl);

    static DownloadTaskSPtr BuildPurchasedListTask(
            int start,
            int length,
            FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildBookTask(const char* bookUrl);
    static DownloadTaskSPtr BuildCreateOrderTask(const char* bookId);
    static DownloadTaskSPtr BuildDrmCertTask(
            const char* bookId,
            const char* bookRevision,
            const char* orderId);
    static DownloadTaskSPtr BuildMailOrderTask(const char* orderId);
    static DownloadTaskSPtr BuildGetReadingProgressTask(const char* bookId, bool IsDKBookStoreBook);
    static DownloadTaskSPtr BuildPutReadingProgressTask(
            const char* bookId,
            const char* bookName,
            const char* data,
            int dataVersion,
            bool IsDKBookStoreBook);
    static DownloadTaskSPtr BuildGetReadingDataTask(
            const char* bookId,
            int dataVersion,
            bool IsDKBookStoreBook);
    static DownloadTaskSPtr BuildPutReadingDataTask(
            const char* bookId,
            const char* bookName,
            int dataVersion,
            const char* data,
            bool IsDKBookStoreBook);
    static DownloadTaskSPtr BuildSearchBookTask(
            const char* query,
            int start,
            int length,
            FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildHotFreeBookListTask(
            int start,
            int length,
            FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildHotPayBookListTask(
            int start,
            int length,
            FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildHotMonthLyBookListTask(
        int start,
        int length,
        FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildHotFavorBookListTask(
        int start,
        int length,
        FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildDouBanRankListTask(
        int start,
        int length,
        FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildAmazonRankListTask(
        int start,
        int length,
        FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildJingDongRankListTask(
        int start,
        int length,
        FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildDangDangRankListTask(
        int start,
        int length,
        FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildFreshBookListTask(
            int start,
            int length,
            FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildAdsListTask(
            int,
            int,
            FetchDataOrder);
    static DownloadTaskSPtr BuildCommentListTask(
            const char* bookId,
            int start,
            int length,
            FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildOwnCommentsListTask(
        int start,
        int length,
        FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildHotCommentsListTask(
        int start,
        int length,
        FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildPublishListTask(
        int start,
        int length,
        FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildPublishBookListTask(
        const char* publishId,
        int start,
        int length,
        FetchDataOrder order = FDO_DEFAULT);

    static DownloadTaskSPtr BuildRankingAdsTask(
            int start,
            int length,
            FetchDataOrder order = FDO_DEFAULT);
    static DownloadTaskSPtr BuildLoginWithTokenTask(
            const char* email,
            const char* token);

    static DownloadTaskSPtr BuildCommentReplyTask(const char* bookId, const char* commentId, int start, int length);
    static DownloadTaskSPtr BuildBookScoreTask(const char* bookId);
    static DownloadTaskSPtr BuildAddBookScoreTask(
            const char* bookId,
            int score);
    static DownloadTaskSPtr BuildAddBookCommentTask(
            const char* bookId,
            int score,
            const char* title,
            const char* content);
    static DownloadTaskSPtr BuildAddBookCommentReplyTask(
            const char* bookId,
            const char* commentId,
            const char* content);
    static DownloadTaskSPtr BuildVoteBookCommentTask(
            const char* bookId,
            const char* commentId,
            bool useful);
    static DownloadTaskSPtr BuildBookChangeLogListTask(
            const char* bookId,
            int start,
            int length);

    static DownloadTaskSPtr BuildRegisterTask(
            const char* user,
            const char* alias,
            const char* password);
    static DownloadTaskSPtr BuildLoginWithPasswordTask(
            const char* user,
            const char* password);
    static DownloadTaskSPtr BuildLogoutTask(const char* user);
    static DownloadTaskSPtr BuildForgetPasswordTask(const char* user);
    static DownloadTaskSPtr BuildChangePasswordTask(
            const char* email,
            const char* oldPassword,
            const char* newPassword);
    static DownloadTaskSPtr BuildChangeAliasTask(
            const char* email,
            const char* newalias);
	static DownloadTaskSPtr BuildGetMigrateStageTask(const std::string& duokanEmail, const std::string& userData);
	static DownloadTaskSPtr BuildGetUserInfoTask(const char* email);
	

    static DownloadTaskSPtr BuildReportBookBugTask(
            const char* bookId,
            const char* bookName,
            const char* bookRevision,
            const char* position,
            const char* ref_chapter,
            const char* reference,
            const char* suggestion,
            bool bConvertCht);
    static DownloadTaskSPtr BuildUploadPersonalExperiencesTask(const char* newData);
    static DownloadTaskSPtr BuildPersonalExperiencesTask();
    static DownloadTaskSPtr BuildPushedMessagesTask(const char* message_id);
    static DownloadTaskSPtr BuildGetLocalNoteSummaryTask();
    static DownloadTaskSPtr BuildGetDuoKanNoteSummaryTask();

	static DownloadTaskSPtr BuildHideBookListTask(int start,
													int length, 
													FetchDataOrder order = FDO_DEFAULT);
	static DownloadTaskSPtr BuildHideBookTask(const model::BookInfoList& books);
	static DownloadTaskSPtr BuildRevealHideBookTask(const model::BookInfoList& books);
    // Favourite
    static DownloadTaskSPtr BuildGetFavouriteTask(int start,
                                                  int length,
                                                  FetchDataOrder order = FDO_DEFAULT);
    // The number of items could be more than 1
    static DownloadTaskSPtr BuildAddFavouriteTask(const model::BookInfoList& books);
    static DownloadTaskSPtr BuildRemoveFavouriteTask(const model::BookInfoList& books);
    static DownloadTaskSPtr BuildMoveFavouriteToCart(const model::BookInfoList& books);

    // Cart
    static DownloadTaskSPtr BuildGetBooksInCartTask(int start,
                                                    int length,
                                                    FetchDataOrder order = FDO_DEFAULT);
    // The number of items could be more than 1
    static DownloadTaskSPtr BuildAddBooksInCartTask(const model::BookInfoList& books);
    static DownloadTaskSPtr BuildRemoveBooksInCartTask(const model::BookInfoList& books);
    static DownloadTaskSPtr BuildMoveCartToFavourite(const model::BookInfoList& books);

    //BoxMessages start
    static DownloadTaskSPtr BuildPostBoxMessagesUnreadCountTask(const char* messageType, const char* deletionThreshold);
    static DownloadTaskSPtr BuildGetBoxMessagesUnreadCountTask(const char* messageType, int bookSerial);
    static DownloadTaskSPtr BuildGetBoxMessagesTask(const char* messageType, int bookSerial, int start, int length);
    //BoxMessages end

    //tag start
    static DownloadTaskSPtr BuildGetBookListWithTagTask(const char* tagName, int start, int length);
    //tag end

    // Special Events
    static DownloadTaskSPtr BuildGetSpecialEventsTask(int start = 0, // Not Used
                                                      int length = 100, //Not Used
                                                      FetchDataOrder order = FDO_DEFAULT);

    // Get user cards from Xiaomi account server
    static DownloadTaskSPtr BuildXiaomiGetUserCardsTask(const std::vector<std::string>& ids);

	static DownloadTaskSPtr BuildPostSQMTask(const char* data);
	static DownloadTaskSPtr BuildGetAccountMappingTask();

private:
    static DownloadTaskSPtr CreateDownloadTask();
    static DownloadTaskSPtr CreateAccountDownloadTask();
    static DownloadTaskSPtr CreateBookOrderDownloadTask();
    static void PostProcessingDownloadTask(DownloadTaskSPtr);
    static std::string BuildUrlWithStartAndLength(
            const char* pattern,
            int start,
            int length);
    // start from 0
    static std::string BuildRecommendUrl(int start, int length);
    static std::string BuildDiscountUrl(
            int start,
            int length,
            FetchDataOrder order);
    static std::string BuildFreeUrl(int start, int length);
    static std::string BuildTopicListUrl(int start, int length);
    static std::string BuildHotFreeBookListUrl(int start, int length);
    static std::string BuildHotPayBookListUrl(int start, int length);
    static std::string BuildFreshBookListUrl(int start, int length);
    static std::string BuildHotFavorBookListUrl(int start, int length);
    static std::string BuildHotMonthLyBookListUrl(int start, int length);
    static std::string BuildRankBookListUrl(int start, int length, ChartsRankType type);
    static std::string BuildTopicBookListUrl(
            int topicId,
            int start,
            int length);
    static std::string BuildCategoryBookListUrl(
            const char* categoryId,
            int start,
            int length,
            FetchDataOrder order);
    static std::string BuildAdsListUrl();
    static std::string BuildCategoryListUrl();
    static std::string BuildCategoryTreeUrl();
    static std::string BuildBookInfoUrl(const char* bookId);
    static std::string BuildBookKeySendUrl(const char* key);
    static std::string BuildExchangeDuokanKeyUrl(const char* key);


    static std::string BuildPurchasedListUrl();
    static std::string BuildCreateOrderUrl();
    static std::string BuildDrmCertUrl();
    static std::string BuildMailOrderUrl();
    static std::string BuildGetReadingProgressUrl(const char* bookId, bool IsDKBookStoreBook);
    static std::string BuildPutReadingProgressUrl(const char* bookId, bool IsDKBookStoreBook);
    static std::string BuildGetReadingDataUrl(const char* bookId, int dataVersion, bool IsDKBookStoreBook);
    static std::string BuildLocalNoteSummaryUrl();
    static std::string BuildDuoKanNoteSummaryUrl();
    static std::string BuildPutReadingDataUrl(const char* bookId, int dataVersion, bool IsDKBookStoreBook);
    static std::string BuildSearchBookUrl(const char* query, int start, int length);
    static std::string BuildPushedMessagesUrl(const char* messageID);

    static std::string BuildCommentListUrl(
            const char* bookId,
            int start,
            int length,
            FetchDataOrder order);
    static std::string BuildOwnCommentsListUrl();
    static std::string BuildHotCommentsListUrl();
    static std::string BuildPublishListUrl(int count);
    static std::string BuildPublishBookListUrl(const char* publishId, int start, int length);
    
    static std::string BuildRankingAdsUrl(int count);
    static std::string BuildBookScoreUrl(const char* bookId);
    static std::string BuildAddBookScoreUrl();
    static std::string BuildAddBookCommentUrl();
    static std::string BuildAddBookCommentReplyUrl();
    static std::string BuildVoteBookCommentUrl();
    static std::string BuildReportBookBugUrl();

    static std::string GetBuildVersion();
    static std::string GetDeviceId();
    static std::string GetCurrentUserName();
    static std::string GetToken();
    static std::string GetAppId();
    static std::string GetMagicId();

    static std::string BuildBookChangeLogListUrl(
        const char* bookId,
        int start,
        int length);

    static std::string BuildRegisterUrl();
    static std::string BuildLoginWithPasswordUrl(
            const char* email,
            const char* password);
    static std::string BuildLoginWithTokenUrl(
            const char* email,
            const char* token);
    static std::string BuildLogoutUrl(const char* email);
    static std::string BuildForgetPasswordUrl(
            const char* user);
    static std::string BuildChangePasswordUrl(
            const char* email,
            const char* oldPassword,
            const char* newPassword);
    static std::string BuildChangeAliasUrl(
                const char* email,
                const char* newAlias);
	static std::string BuildGetMigrateStageUrl(const std::string& duokanEmail);
	static std::string BuildGetUserInfoUrl(const char* email);

    static std::string BuildUploadPersonalExperiencesUrl();
	static std::string BuildPersonalExperiencesUrl();

	static std::string BuildHideBookListUrl();
	static std::string BuildHideBookUrl();
	static std::string BuildRevealHideBookUrl();

    // favourite books
    static std::string BuildGetFavouriteUrl();
    static std::string BuildAddFavouriteUrl();
    static std::string BuildRemoveFavouriteUrl();
    static std::string BuildMoveFavouriteToCartUrl();

    // carts books
    static std::string BuildGetBooksInCartUrl();
    static std::string BuildAddBooksInCartUrl();
    static std::string BuildRemoveBooksFromCartUrl();
    static std::string BuildMoveCartToFavouriteUrl();

    // special events
    static std::string BuildSpecialEventsUrl();

    // Xiaomi Account Related urls
    static std::string BuildXiaomiGetUserCardsUrl();
	static std::string BuildGetAccountMappingUrl();
};

} // namespace bookstore
} // namespace dk
#endif
