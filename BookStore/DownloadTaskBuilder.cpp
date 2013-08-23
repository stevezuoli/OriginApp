#include "BookStore/DownloadTaskBuilder.h"
#include "KernelDef.h"
#include <cstdio>
#include "Utility/PathManager.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/ConstStrings.h"
#include "Common/DuoKanServer.h"
#include "Common/SystemSetting_DK.h"
#include "drivers/DeviceInfo.h"
#include "Utility/HttpUtil.h"
#include "Utility/EncodeUtil.h"
#include "I18n/StringManager.h"
#include <strings.h>
#include "interface.h"

using dk::utility::PathManager;
using dk::utility::HttpUtil;

namespace dk
{
namespace bookstore
{

const int MAX_URL = 1024;


static std::string s_bookHost = DuoKanServer::GetBookHost();
static std::string s_syncHost = DuoKanServer::GetSyncHost();
static std::string s_commentHost = DuoKanServer::GetCommentHost();
static std::string s_orderHost = DuoKanServer::GetOrderHost();
static std::string s_loginHost = DuoKanServer::GetLoginHost();
static std::string s_drmHost = DuoKanServer::GetDrmHost();
static std::string s_xiaomiHost = XiaomiServer::GetAccountHost();


#define DUOKAN_DEVICE "kindle"

static std::string s_recommendUrl = "/store/v0/"DUOKAN_DEVICE"/recommend?start=%d&count=%d";
static std::string s_hotFreeUrl = "/store/v0/"DUOKAN_DEVICE"/hot_free?start=%d&count=%d";
static std::string s_hotPayUrl = "/store/v0/"DUOKAN_DEVICE"/hot_pay?start=%d&count=%d";
static std::string s_hotMonthlyUrl = "/store/v0/"DUOKAN_DEVICE"/rank/monthly_pay?start=%d&count=%d";
static std::string s_hotFavorUrl = "/store/v0/"DUOKAN_DEVICE"/rank/favour_pay?start=%d&count=%d";
static std::string s_chartsUrl = "/store/v0/"DUOKAN_DEVICE"/rank?rt=%d&start=%d&count=%d";
static std::string s_freshUrl = "/store/v0/"DUOKAN_DEVICE"/fresh?start=%d&count=%d";
static std::string s_discountUrl = "/store/v0/"DUOKAN_DEVICE"/discount?start=%d&count=%d";
static std::string s_freeUrl = "/store/v0/"DUOKAN_DEVICE"/free?start=%d&count=%d";
static std::string s_topicListUrl = "/store/v0/"DUOKAN_DEVICE"/list/all?start=%d&count=%d";
static std::string s_topicBookListUrl = "/store/v0/"DUOKAN_DEVICE"/list/%d?start=%d&count=%d";
static std::string s_categoryListUrl = "/store/v0/"DUOKAN_DEVICE"/category/all";
static std::string s_categoryTreeUrl = "/store/v0/"DUOKAN_DEVICE"/category/tree";
static std::string s_categoryBookListUrl = "/store/v0/"DUOKAN_DEVICE"/category/%s?start=%d&page_length=%d";
static std::string s_bookInfoUrl = "/store/v0/"DUOKAN_DEVICE"/book/%s";

static std::string s_purchasedListUrl = "/store/v0/payment/book/list";
static std::string s_createOrderUrl = "/store/v0/order/create";
static std::string s_drmCertUrl = "/drm/v0/certificate";
static std::string s_mailOrderUrl = "/store/v0/order/mail_order";
static std::string s_getReadingProgressUrl = "/sync/duokan/progress?userid=%s&token=%s&deviceid=%s&appid=%s&bookids=%s";
static std::string s_getLocalBookReadingProgressUrl = "/sync/local/progress?userid=%s&token=%s&deviceid=%s&appid=%s&bookids=%s";
static std::string s_getReadingDataUrl = "/sync/duokan/note?userid=%s&token=%s&deviceid=%s&appid=%s&bookids=%s&currentversions=%d";
static std::string s_getLocalBookReadingDataUrl = "/sync/local/note?userid=%s&token=%s&deviceid=%s&appid=%s&bookids=%s&currentversions=%d";
static std::string s_postReadingProgressUrl = "/sync/duokan/progress";
static std::string s_postLocalBookReadingProgressUrl = "/sync/local/progress";
static std::string s_postReadingDataUrl = "/sync/duokan/note";
static std::string s_postLocalBookReadingDataUrl = "/sync/local/note";
static std::string s_localNoteSummaryUrl="/sync/local/note_summary?userid=%s&token=%s&deviceid=%s&appid=%s";
static std::string s_duokanNoteSummaryUrl="/sync/duokan/note_summary?userid=%s&token=%s&deviceid=%s&appid=%s";
static std::string s_adsListUrl = "/store/v0/ad?deviceid=%s&build=%s&key=%s&count=%d";
static std::string s_commentListUrl = "/comment/v0/get_book_comments?device_id=%s&user_id=%s&start_index=%d&count=%d&book_id=%s&order_type=%d";
static std::string s_myCommentsListUrl = "/comment/v0/get_own_comments";
static std::string s_personalOwnCommentListUrl = "/comment/v0/get_own_comments?token=%s&device_id=%s&app_id=%s&user_id=%s&user_ip=%s&start_index=%d&count=%d";
static std::string s_personalHotCommentListUrl = "/comment/v0/get_useful_comments?token=%s&device_id=%s&app_id=%s&user_id=%s&user_ip=%s&start_index=%d&count=%d";
static std::string s_commentReplyUrl = "/comment/v0/get_reply?token=%s&device_id=%s&app_id=%s&book_id=%s&comment_id=%s&start_index=%d&count=%d&user_id=%s&user_ip=%s";
//BoxMessages start
static std::string s_boxMessagesPostUnreadCountUrl = "/message/v0/box/remind/unread_count";
static std::string s_boxMessagesGetUnreadCountUrl = "/message/v0/box/remind/unread_count?user_id=%s&token=%s&device_id=%s&appid=%s&message_type=%s&book_serial=%d";
static std::string s_boxMessagesLstUrl = "/message/v0/box/remind?user_id=%s&token=%s&device_id=%s&appid=%s&message_type=%s&book_serial=%d&start=%d&count=%d";
//BoxMessages end

//tag start
static std::string s_fetchBookListWithTagUrl = "/store/v0/"DUOKAN_DEVICE"/book/tag_list?tag_name=%s&start=%d&count=%d";
//tag end

//Publish
static std::string s_publishBookListUrl = "/store/v0/"DUOKAN_DEVICE"/rights/%s?start=%d&count=%d";

// Book Details
static std::string s_bookChangeLogListUrl = "/store/v0/"DUOKAN_DEVICE"/book/%s/change_log?start=%d&count=%d";
static std::string s_getBookPreviewsUrl = "/store/v0/"DUOKAN_DEVICE"/book/%s/preview"; // TODO
static std::string s_searchBookUrl = "/store/v0/"DUOKAN_DEVICE"/search?start=%d&count=%d&s=%s";
static std::string s_getBooksByIds = "/store/v0/"DUOKAN_DEVICE"/book/id_list?ids=%s"; // TODO
static std::string s_getSearchHots = "/store/v0/"DUOKAN_DEVICE"/search/hot"; //TODO
static std::string s_getBooksByAuthor = "/store/v0/"DUOKAN_DEVICE"/author/%s?start=%d&count=%d"; //TODO

static std::string s_pushedMessagesUrl = "/message/v0/get_own_message?device_id=%s&user_id=%s&received_message_id=%s&build_number=%s&app_id=%s";
static std::string s_bookScoreUrl = "/comment/v0/get_book_summary?device_id=%s&user_id=%s&book_id=%s";
static std::string s_addBookScoreUrl = "/comment/v0/add_book_score";
static std::string s_addBookCommentUrl = "/comment/v0/add_book_comment";
static std::string s_addBookCommentReplyUrl = "/comment/v0/add_comment_reply";
static std::string s_voteBookCommentUrl = "/comment/v0/vote_comment";

static std::string s_bookKeySendUrl = "/store/v0/redeem/single/status/%s";
static std::string s_exchangeDuokanKeyUrl = "/store/v0/redeem/single/consume/%s";
static std::string s_postReportBookBugUrl = "/sync/report_bug/book";
static std::string s_registerUrl = "/dk_id/api/account/secure";
static std::string s_loginUrl = "/dk_id/api/account/secure/logged";
static std::string s_logoutUrl = "/dk_id/api/account/logged/%s";
static std::string s_forgetPasswordUrl = "/dk_id/api/account/forgetpwd/%s";
static std::string s_loginWithTokenUrl = "/dk_id/api/account/logged/%s?token=%s&deviceid=%s&appid=%s&format=xml";
static std::string s_changePasswordUrl = "/dk_id/api/account/email/%s?token=%s&deviceid=%s&appid=%s&format=xml&oldpwd=%s&newpwd=%s";
static std::string s_changeAliasUrl = "/dk_id/api/account/email/%s?token=%s&deviceid=%s&appid=%s&format=xml&newalias=%s";
static std::string s_migrateStageUrl = "/dk_id/migrate/stage";
static std::string s_getUserInfoUrl = "/dk_id/api/account/email/%s?token=%s&deviceid=%s&appid=%s&format=xml";

static std::string s_postPersonalExperiencesUrl = "/sync/readstat";
static std::string s_getPersonalExperiencesUrl = "/sync/readstat?userid=%s&token=%s&deviceid=%s&appid=%s";

static std::string s_GetHideBookListUrl = "/store/v0/payment/book/hide_list";
static std::string s_hideBookUlr = "/store/v0/payment/book/hide";
static std::string s_revealHideBookUrl = "/store/v0/payment/book/reveal";

static std::string s_postGetFavouriteUrl = "/store/v0/user/favourite";
static std::string s_postAddFavouriteUrl = "/store/v0/user/favourite/add";
static std::string s_postRemoveFavouriteUrl = "/store/v0/user/favourite/remove";
static std::string s_postMoveFavouriteToCartUrl = "/store/v0/user/favourite/cart";

static std::string s_postGetBooksInCartUrl = "/store/v0/user/cart";
static std::string s_postAddBooksToCartUrl = "/store/v0/user/cart/add";
static std::string s_postRemoveBooksFromCartUrl = "/store/v0/user/cart/remove";
static std::string s_getPaymentSpecialEventsUrl = "/store/v0/payment/special_events";
static std::string s_postMoveCartToFavouriteUrl = "/store/v0/user/cart/fav";
static std::string s_postSQMUrl = "http://update.duokan.com/DuoKanServer/servlet/sqm";

static std::string s_accountMappingsUrl = "/dk_id/migrate/dk_user";

// Xiaomi Account APIs
static std::string s_getXiaomiUserCards = "/pass/usersCard";

static std::string Md5Password(const char* password)
{
    std::vector<unsigned char> result = 
        EncodeUtil::Md5(password, strlen(password));
    return EncodeUtil::BinToHex(&result[0], result.size());
}

static std::string HashPassword(const char* password)
{
    std::string midString = 
        Md5Password(password) + "ILOVEDUOKANILIKEREADING";
    return Md5Password(midString.c_str());
}

void DownloadTaskBuilder::PostProcessingDownloadTask(DownloadTaskSPtr task)
{
    if (task)
    {
        if (task->GetMethod() == DownloadTask::POST)
        {
            task->AddPostData(ConstStrings::CLIENT_BUILD, SystemSettingInfo::GetInstance()->GetVersion().GetBuffer());
        }
        else if (task->GetMethod() == DownloadTask::GET)
        {
            std::string url = task->GetUrl();
            if (url.find('?') == std::string::npos)
            {
                url.append(1, '?').append(ConstStrings::CLIENT_BUILD).append(1, '=').append(SystemSettingInfo::GetInstance()->GetVersion().GetBuffer());
            }
            else
            {
                url.append(1, '&').append(ConstStrings::CLIENT_BUILD).append(1, '=').append(SystemSettingInfo::GetInstance()->GetVersion().GetBuffer());
            }
            task->SetUrl(url.c_str());
        }
    }
}

std::string DownloadTaskBuilder::BuildUrlWithStartAndLength(const char* pattern, int start, int length)
{
    char buf[MAX_URL];
    snprintf(buf, DK_DIM(buf), pattern, start, length);
    return buf;
}

std::string DownloadTaskBuilder::BuildRecommendUrl(int start, int length)
{
    return BuildUrlWithStartAndLength((s_bookHost + s_recommendUrl).c_str(), start, length);
}

std::string DownloadTaskBuilder::BuildDiscountUrl(int start, int length, FetchDataOrder order)
{
    std::string url = BuildUrlWithStartAndLength((s_bookHost + s_discountUrl).c_str(), start, length);
    if (order == FDO_LATEST)
    {
        url += "&latest=1";
    }
    return url;
}

std::string DownloadTaskBuilder::BuildFreeUrl(int start, int length)
{
    return BuildUrlWithStartAndLength((s_bookHost + s_freeUrl).c_str(), start, length);
}

std::string DownloadTaskBuilder::BuildTopicListUrl(int start, int length)
{
    return BuildUrlWithStartAndLength((s_bookHost + s_topicListUrl).c_str(), start, length);
}

std::string DownloadTaskBuilder::BuildCategoryListUrl()
{
    return s_bookHost + s_categoryListUrl;
}

std::string DownloadTaskBuilder::BuildCategoryTreeUrl()
{
    return s_bookHost + s_categoryTreeUrl;
}

std::string DownloadTaskBuilder::BuildTopicBookListUrl(int topicId, int start, int length)
{
    char buf[MAX_URL];
    snprintf(buf, DK_DIM(buf), (s_bookHost + s_topicBookListUrl).c_str(), topicId, start, length);
    return buf;
}

std::string DownloadTaskBuilder::BuildCategoryBookListUrl(
        const char* categoryId,
        int start,
        int length,
        FetchDataOrder order)
{
    char buf[MAX_URL];
    snprintf(buf, DK_DIM(buf), (s_bookHost + s_categoryBookListUrl).c_str(), categoryId, start, length);
    if (FDO_LATEST == order)
    {
        strncat(buf, "&latest=1", DK_DIM(buf));
    }
    return buf;
}

std::string DownloadTaskBuilder::BuildBookInfoUrl(const char* bookId)
{
    char buf[MAX_URL];
    snprintf(buf, DK_DIM(buf), (s_bookHost + s_bookInfoUrl).c_str(), bookId);
    return buf;
}

std::string DownloadTaskBuilder::BuildBookKeySendUrl(const char* key)
{
    char buf[MAX_URL];
    snprintf(buf, DK_DIM(buf), (s_bookHost + s_bookKeySendUrl).c_str(), key);
    return buf;
}

std::string DownloadTaskBuilder::BuildExchangeDuokanKeyUrl(const char* key)
{
    char buf[MAX_URL];
    snprintf(buf, DK_DIM(buf), (s_bookHost + s_exchangeDuokanKeyUrl).c_str(), key);
    return buf;
}

std::string DownloadTaskBuilder::BuildDrmCertUrl()
{
    return s_drmHost + s_drmCertUrl;
}

std::string DownloadTaskBuilder::BuildPurchasedListUrl()
{
    return s_orderHost + s_purchasedListUrl;
}

std::string DownloadTaskBuilder::BuildCreateOrderUrl()
{
    return s_orderHost + s_createOrderUrl;
}

std::string DownloadTaskBuilder::BuildMailOrderUrl()
{
    return s_orderHost + s_mailOrderUrl;;
}

DownloadTaskSPtr DownloadTaskBuilder::CreateDownloadTask()
{
    DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    task->AddCookie("device", GetDeviceId().c_str());
    task->AddCookie("build", GetBuildVersion().c_str());
    task->AddCookie("user", GetCurrentUserName().c_str());
    task->AddCookie(ConstStrings::TOKEN, GetToken().c_str());
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildRecommendTask(int start, int length, FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildRecommendUrl(start, length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildDiscountTask(int start, int length, FetchDataOrder order)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildDiscountUrl(start, length, order).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildFreeTask(
        int start,
        int length,
        FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildFreeUrl(start, length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildTopicListTask(
        int start, 
        int length,
        FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildTopicListUrl(start, length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildCategoryListTask(int /* start */, int /* length */, FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildCategoryListUrl().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildCategoryTreeTask(int /* start */, int /* length */, FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildCategoryTreeUrl().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildTopicBookListTask(
        int topicId,
        int start,
        int length,
        FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildTopicBookListUrl(topicId, start, length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildBookInfoTask(const char* bookId)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildBookInfoUrl(bookId).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildBookKeySendTask(const char* key)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildBookKeySendUrl(key).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildExchangeDuokanKeyTask(const char* key)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildExchangeDuokanKeyUrl(key).c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData(ConstStrings::USER_ID, GetCurrentUserName().c_str());
    task->AddPostData(ConstStrings::DEVICE_ID, GetDeviceId().c_str());
    task->AddPostData(ConstStrings::TOKEN, GetToken().c_str());
    task->AddPostData(ConstStrings::APP_ID, GetAppId().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildCategoryBookListTask(
    const char* categoryId,
    int start,
    int length,
    FetchDataOrder order)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildCategoryBookListUrl(categoryId, start, length, order).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildCoverImageTask(
        const char* coverImageUrl)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(coverImageUrl);
    task->SetDestFile(PathManager::CoverImageUrlToFile(coverImageUrl).c_str());
    PostProcessingDownloadTask(task);
    return task;
}


DownloadTaskSPtr DownloadTaskBuilder::BuildPurchasedListTask(int /* start */, int /* length */, FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildPurchasedListUrl().c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData(ConstStrings::USER_ID, GetCurrentUserName().c_str());
    task->AddPostData(ConstStrings::DEVICE_ID, GetDeviceId().c_str());
    task->AddPostData(ConstStrings::TOKEN, GetToken().c_str());
    task->AddPostData(ConstStrings::APP_ID, GetAppId().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildCreateOrderTask(const char* bookId)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildCreateOrderUrl().c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData(ConstStrings::USER_ID, GetCurrentUserName().c_str());
    task->AddPostData(ConstStrings::DEVICE_ID, GetDeviceId().c_str());
    task->AddPostData(ConstStrings::TOKEN, GetToken().c_str());
    task->AddPostData(ConstStrings::APP_ID, GetAppId().c_str());
    task->AddPostData(ConstStrings::BOOK_UUID, bookId);
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildDrmCertTask(const char* bookId, const char* bookRevision, const char* orderId)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetMethod(DownloadTask::POST);
    task->SetUrl(BuildDrmCertUrl().c_str());
    task->AddPostData(ConstStrings::USER_ID, GetCurrentUserName().c_str());
    task->AddPostData(ConstStrings::DEVICE_ID, GetDeviceId().c_str());
    task->AddPostData(ConstStrings::TOKEN, GetToken().c_str());
    task->AddPostData(ConstStrings::APP_ID, GetAppId().c_str());
    task->AddPostData(ConstStrings::BOOK_UUID, bookId);
    task->AddPostData(ConstStrings::BOOK_REVISION, bookRevision);
    task->AddPostData(ConstStrings::ORDER_UUID, orderId);
    task->AddPostData(ConstStrings::MAGIC_ID, GetMagicId().c_str());
    PostProcessingDownloadTask(task);
    return task;
}


DownloadTaskSPtr DownloadTaskBuilder::BuildMailOrderTask(const char* orderId)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildMailOrderUrl().c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData(ConstStrings::USER_ID, GetCurrentUserName().c_str());
    task->AddPostData(ConstStrings::DEVICE_ID, GetDeviceId().c_str());
    task->AddPostData(ConstStrings::TOKEN, GetToken().c_str());
    task->AddPostData(ConstStrings::APP_ID, GetAppId().c_str());
    task->AddPostData(ConstStrings::ORDER_UUID, orderId);
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::GetBuildVersion()
{
    return "2012051401";
}

std::string DownloadTaskBuilder::GetDeviceId()
{
    return DeviceInfo::GetDeviceID();
}

std::string DownloadTaskBuilder::GetCurrentUserName()
{
    return BookStoreInfoManager::GetInstance()->GetCurrentUserName();
}

std::string DownloadTaskBuilder::GetToken()
{
    return BookStoreInfoManager::GetInstance()->GetCurrentToken();
}

std::string DownloadTaskBuilder::GetAppId()
{
    return "KindleUser";
}

std::string DownloadTaskBuilder::GetMagicId()
{
    return "ED6CB6DF-67D9-4A43-865C-0498480BDC26";
}

std::string DownloadTaskBuilder::BuildLocalNoteSummaryUrl()
{
    string url = s_syncHost + s_localNoteSummaryUrl;
    char buf[MAX_URL];
    snprintf(buf, 
            DK_DIM(buf),
            url.c_str(),
            GetCurrentUserName().c_str(),
            GetToken().c_str(),
            GetDeviceId().c_str(),
            GetAppId().c_str());
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildGetLocalNoteSummaryTask()
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildLocalNoteSummaryUrl().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildDuoKanNoteSummaryUrl()
{
    string url = s_syncHost + s_duokanNoteSummaryUrl;
    char buf[MAX_URL];
    snprintf(buf, 
            DK_DIM(buf),
            url.c_str(),
            GetCurrentUserName().c_str(),
            GetToken().c_str(),
            GetDeviceId().c_str(),
            GetAppId().c_str());
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildGetDuoKanNoteSummaryTask()
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildDuoKanNoteSummaryUrl().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildGetReadingProgressUrl(const char* bookId, bool IsDKBookStoreBook)
{
    string url = s_syncHost;
    url += (IsDKBookStoreBook ? s_getReadingProgressUrl : s_getLocalBookReadingProgressUrl);
    char buf[MAX_URL];
    snprintf(buf, 
            DK_DIM(buf),
            url.c_str(),
            GetCurrentUserName().c_str(),
            GetToken().c_str(),
            GetDeviceId().c_str(),
            GetAppId().c_str(),
            bookId);
    return buf;
}

std::string DownloadTaskBuilder::BuildPutReadingProgressUrl(const char* bookId, bool IsDKBookStoreBook)
{
    string url = s_syncHost;
    url += IsDKBookStoreBook ? s_postReadingProgressUrl : s_postLocalBookReadingProgressUrl;
    return url;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildGetReadingProgressTask(const char* bookId, bool IsDKBookStoreBook)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildGetReadingProgressUrl(bookId, IsDKBookStoreBook).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildPutReadingProgressTask(
    const char* bookId,
    const char* bookName,
    const char* data,
    int dataVersion,
    bool IsDKBookStoreBook)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }

    char versionStr[10] = {0};
    snprintf(versionStr, DK_DIM(versionStr), "%d", dataVersion);
    
    
    task->SetMethod(DownloadTask::POST);
    task->SetUrl(BuildPutReadingProgressUrl(bookId, IsDKBookStoreBook).c_str());
    task->AddPostData("userid", GetCurrentUserName().c_str());
    task->AddPostData("token", GetToken().c_str());
    task->AddPostData("deviceid", GetDeviceId().c_str());
    task->AddPostData("appid", GetAppId().c_str());
    task->AddPostData("bookid", bookId);
    task->AddPostData("bookname", bookName);
    task->AddPostData("data", data);
    task->AddPostData("version", versionStr);
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildSearchBookUrl(const char* query, int start, int length)
{
    char buf[MAX_URL];
    snprintf(buf, DK_DIM(buf), (s_bookHost + s_searchBookUrl).c_str(), start, length, HttpUtil::UrlEncode(query).c_str());
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildSearchBookTask(const char* query, int start, int length, FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildSearchBookUrl(query, start, length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildHotFavorBookListUrl(int start, int length)
{
    return BuildUrlWithStartAndLength((s_bookHost + s_hotFavorUrl).c_str(), start, length);
}

std::string DownloadTaskBuilder::BuildHotMonthLyBookListUrl(int start, int length)
{
    return BuildUrlWithStartAndLength((s_bookHost + s_hotMonthlyUrl).c_str(), start, length);
}

DownloadTaskSPtr DownloadTaskBuilder::BuildHotMonthLyBookListTask(int start, int length, FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildHotMonthLyBookListUrl(start, length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildHotFavorBookListTask(int start, int length, FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildHotFavorBookListUrl(start, length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildFreshBookListUrl(int start, int length)
{
    return BuildUrlWithStartAndLength((s_bookHost + s_freshUrl).c_str(), start, length);
}

DownloadTaskSPtr DownloadTaskBuilder::BuildFreshBookListTask(int start, int length, FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildFreshBookListUrl(start, length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildHotFreeBookListUrl(int start, int length)
{
    return BuildUrlWithStartAndLength((s_bookHost + s_hotFreeUrl).c_str(), start, length);
}

DownloadTaskSPtr DownloadTaskBuilder::BuildHotFreeBookListTask(int start, int length, FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildHotFreeBookListUrl(start, length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildHotPayBookListUrl(int start, int length)
{
    return BuildUrlWithStartAndLength((s_bookHost + s_hotPayUrl).c_str(), start, length);
}

DownloadTaskSPtr DownloadTaskBuilder::BuildHotPayBookListTask(int start, int length, FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildHotPayBookListUrl(start, length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildDouBanRankListTask(int start, int length, FetchDataOrder order)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildRankBookListUrl(start, length, CRT_DOUBAN).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildAmazonRankListTask(int start, int length, FetchDataOrder order)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildRankBookListUrl(start, length, CRT_AMAZON).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildJingDongRankListTask(int start, int length, FetchDataOrder order)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildRankBookListUrl(start, length, CRT_JINGDONG).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildDangDangRankListTask(int start, int length, FetchDataOrder order)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildRankBookListUrl(start, length, CRT_DANGDANG).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildRankBookListUrl(int start, int length, ChartsRankType type)
{
    int rankTypes[CRT_COUNTS] = {0, 1, 10, 100, 200, 210, 220, 230};//http://dkmars/wiki/index.php?title=%E4%B9%A6%E5%9F%8EWeb_API#.E6.A6.9C.E5.8D.95
    if (type >=0 && type < CRT_COUNTS)
    {
        char buf[MAX_URL];
        snprintf(buf, DK_DIM(buf), (s_bookHost + s_chartsUrl).c_str(), rankTypes[type], start, length);
        return buf;
    }

    return string();
}

std::string DownloadTaskBuilder::BuildAdsListUrl()
{
    char buf[MAX_URL];
    snprintf(buf, DK_DIM(buf), 
        (s_bookHost + s_adsListUrl).c_str(), 
        GetDeviceId().c_str(), 
        GetBuildVersion().c_str(), 
        "kindle_main", 
        4);
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildAdsListTask(int, int, FetchDataOrder)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildAdsListUrl().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildGetReadingDataUrl(const char* bookId, int dataVersion, bool IsDKBookStoreBook)
{
    string url = s_syncHost;
    url += (IsDKBookStoreBook ? s_getReadingDataUrl : s_getLocalBookReadingDataUrl);
    char buf[MAX_URL];
    snprintf(buf, 
            DK_DIM(buf),
            url.c_str(),
            GetCurrentUserName().c_str(),
            GetToken().c_str(),
            GetDeviceId().c_str(),
            GetAppId().c_str(),
            bookId,
            dataVersion);
    return buf;
}

std::string DownloadTaskBuilder::BuildPutReadingDataUrl(const char* , int , bool IsDKBookStoreBook)
{
    string url = s_syncHost;
    url += (IsDKBookStoreBook ? s_postReadingDataUrl : s_postLocalBookReadingDataUrl);
    return url;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildPutReadingDataTask(
        const char* bookId,
        const char* bookName,
        int dataVersion,
        const char* data,
        bool IsDKBookStoreBook)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildPutReadingDataUrl(bookId, dataVersion, IsDKBookStoreBook).c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData("userid", GetCurrentUserName().c_str());
    task->AddPostData("token", GetToken().c_str());
    task->AddPostData("deviceid", GetDeviceId().c_str());
    task->AddPostData("appid", GetAppId().c_str());
    char versionStr[10] = {0};
    snprintf(versionStr, DK_DIM(versionStr), "%d", dataVersion);
    task->AddPostData("version", versionStr);
    task->AddPostData("bookid", bookId);
    task->AddPostData("bookname", bookName);
    task->AddPostData("data", data);
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildGetReadingDataTask(const char* bookId, int dataVersion, bool IsDKBookStoreBook)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildGetReadingDataUrl(bookId, dataVersion, IsDKBookStoreBook).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildCommentListUrl(const char* bookId, int start, int length, FetchDataOrder order)
{
    char buf[MAX_URL];
    snprintf(buf,
            DK_DIM(buf),
            (s_commentHost + s_commentListUrl).c_str(),
            GetDeviceId().c_str(),
            GetCurrentUserName().c_str(),
            start,
            length,
            bookId,
            order);
    return buf;
}

std::string DownloadTaskBuilder::BuildPushedMessagesUrl(const char* messageID)
{
    char buf[MAX_URL];
    snprintf(buf,
        DK_DIM(buf),
        (s_bookHost + s_pushedMessagesUrl).c_str(),
        GetDeviceId().c_str(),
        GetCurrentUserName().c_str(),
        messageID,
        GetBuildVersion().c_str(),
        GetAppId().c_str());
    return buf;
}
//BoxMessages start
DownloadTaskSPtr DownloadTaskBuilder::BuildPostBoxMessagesUnreadCountTask(const char* messageType, const char* deletionThreshold)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (task)
    {
        task->SetUrl((s_bookHost + s_boxMessagesPostUnreadCountUrl).c_str());
        task->SetMethod(DownloadTask::POST);
        task->AddPostData(ConstStrings::USER_ID, GetCurrentUserName().c_str());
        task->AddPostData(ConstStrings::TOKEN, GetToken().c_str());
        task->AddPostData(ConstStrings::DEVICE_ID, GetDeviceId().c_str());
        task->AddPostData(ConstStrings::APPID, GetAppId().c_str());
        task->AddPostData("message_type", messageType);
        task->AddPostData("deletion_threshold", deletionThreshold);
        task->AddPostData("currentversions", GetBuildVersion().c_str());
        PostProcessingDownloadTask(task);
    }
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildGetBoxMessagesUnreadCountTask(const char* messageType, int bookSerial)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (task)
    {
        char buf[MAX_URL];
        snprintf(buf,
            DK_DIM(buf),
            (s_bookHost + s_boxMessagesGetUnreadCountUrl).c_str(),
            GetCurrentUserName().c_str(),
            GetToken().c_str(),
            GetDeviceId().c_str(),
            GetAppId().c_str(),
            messageType,
            bookSerial);
        task->SetUrl(buf);
        PostProcessingDownloadTask(task);
    }
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildGetBoxMessagesTask(const char* messageType, int bookSerial, int start, int length)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (task)
    {
        char buf[MAX_URL];
        snprintf(buf,
            DK_DIM(buf),
            (s_bookHost + s_boxMessagesLstUrl).c_str(),
            GetCurrentUserName().c_str(),
            GetToken().c_str(),
            GetDeviceId().c_str(),
            GetAppId().c_str(),
            messageType,
            bookSerial,
            start,
            length);
        task->SetUrl(buf);
        PostProcessingDownloadTask(task);
    }
    return task;
}
//BoxMessages end

//tag start
DownloadTaskSPtr DownloadTaskBuilder::BuildGetBookListWithTagTask(const char* tagName, int start, int length)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (task)
    {
        char buf[MAX_URL];
        snprintf(buf,
            DK_DIM(buf),
            (s_bookHost + s_fetchBookListWithTagUrl).c_str(),
            HttpUtil::UrlEncode(tagName).c_str(),
            start,
            length);
        task->SetUrl(buf);
        PostProcessingDownloadTask(task);
    }
    return task;
}
//tag end

std::string DownloadTaskBuilder::BuildPublishListUrl(int count)
{
    char buf[MAX_URL];
    snprintf(buf,
        DK_DIM(buf),
        (s_bookHost + s_adsListUrl).c_str(),
        GetDeviceId().c_str(),
        GetBuildVersion().c_str(),
        "rights",
        count);
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildPublishListTask(int start, int length, FetchDataOrder order)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildPublishListUrl(length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildPublishBookListUrl(const char* publishId, int start, int length)
{
    char buf[MAX_URL];
    snprintf(buf,
        DK_DIM(buf),
        (s_bookHost + s_publishBookListUrl).c_str(),
        publishId,
        start,
        length);
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildPublishBookListTask(const char* publishId, int start, int length, FetchDataOrder order)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildPublishBookListUrl(publishId, start, length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildRankingAdsTask(int start, int length, FetchDataOrder order)
{
    (void)start;
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildRankingAdsUrl(length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildRankingAdsUrl(int count)
{
    char buf[MAX_URL];
    snprintf(buf,
        DK_DIM(buf),
        (s_bookHost + s_adsListUrl).c_str(),
        GetDeviceId().c_str(),
        GetBuildVersion().c_str(),
        "kindle_ranking",
        count);
    return buf;
}

std::string DownloadTaskBuilder::BuildOwnCommentsListUrl()
{
    //POST
    //return s_commentHost + s_myCommentsListUrl;
    //GET
    return s_commentHost + s_personalOwnCommentListUrl;
}

std::string DownloadTaskBuilder::BuildHotCommentsListUrl()
{
    //POST
    //return s_commentHost + s_myCommentsListUrl;
    //GET
    return s_commentHost + s_personalHotCommentListUrl;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildCommentListTask(const char* bookId, int start, int length, FetchDataOrder order)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildCommentListUrl(bookId, start, length, order).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildOwnCommentsListTask(int start, int length, FetchDataOrder order)
{
    char buf[MAX_URL] = {0};
    snprintf(buf,
        DK_DIM(buf),
        BuildOwnCommentsListUrl().c_str(),
        GetToken().c_str(),
        GetDeviceId().c_str(),
        GetAppId().c_str(),
        GetCurrentUserName().c_str(),
        NULL,
        start,
        length);
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(buf);
    PostProcessingDownloadTask(task);
    /*task->SetUrl(BuildOwnCommentsListUrl().c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData(ConstStrings::TOKEN, GetToken().c_str());
    task->AddPostData(ConstStrings::DEVICE_ID, GetDeviceId().c_str());
    task->AddPostData(ConstStrings::APP_ID, GetAppId().c_str());
    task->AddPostData(ConstStrings::USER_ID, GetCurrentUserName().c_str());
    //task->AddPostData("user_ip", NULL);
    char value[8] = {0};
    snprintf(value, DK_DIM(value), "%d", start);
    task->AddPostData("start_index", value);
    snprintf(value, DK_DIM(value), "%d", length);
    task->AddPostData("count ", value);*/
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildHotCommentsListTask(int start, int length, FetchDataOrder order)
{
    char buf[MAX_URL] = {0};
    snprintf(buf,
        DK_DIM(buf),
        BuildHotCommentsListUrl().c_str(),
        GetToken().c_str(),
        GetDeviceId().c_str(),
        GetAppId().c_str(),
        GetCurrentUserName().c_str(),
        NULL,
        start,
        length);
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(buf);
    PostProcessingDownloadTask(task);

    return task;
}

std::string DownloadTaskBuilder::BuildBookScoreUrl(const char* bookId)
{
    char buf[MAX_URL];
    snprintf(buf,
            DK_DIM(buf),
            (s_commentHost + s_bookScoreUrl).c_str(),
            GetDeviceId().c_str(),
            GetCurrentUserName().c_str(),
            bookId);
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildBookScoreTask(const char* bookId)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildBookScoreUrl(bookId).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildAddBookScoreUrl()
{
    return s_commentHost + s_addBookScoreUrl;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildAddBookScoreTask(const char* bookId, int score)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildAddBookScoreUrl().c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData("user_id", GetCurrentUserName().c_str());
    task->AddPostData("token", GetToken().c_str());
    task->AddPostData("device_id", GetDeviceId().c_str());
    task->AddPostData("book_id", bookId);
    char buf[10];
    snprintf(buf, DK_DIM(buf), "%d", score);
    task->AddPostData("score", buf);
    task->AddPostData(ConstStrings::APP_ID, GetAppId().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildAddBookCommentUrl()
{
    return s_commentHost + s_addBookCommentUrl;
}

std::string DownloadTaskBuilder::BuildReportBookBugUrl()
{
    return s_syncHost + s_postReportBookBugUrl;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildAddBookCommentTask(
    const char* bookId,
    int score,
    const char* title,
    const char* content)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    char buf[10];
    snprintf(buf, DK_DIM(buf), "%d", score);
    task->SetUrl(BuildAddBookCommentUrl().c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData("user_id", GetCurrentUserName().c_str());
    task->AddPostData("token", GetToken().c_str());
    task->AddPostData("device_id", GetDeviceId().c_str());
    task->AddPostData("book_id", bookId);
    task->AddPostData("title", title);
    task->AddPostData("content", content);
    task->AddPostData("score", buf);
    task->AddPostData(ConstStrings::APP_ID, GetAppId().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildAddBookCommentReplyUrl()
{
    return s_commentHost + s_addBookCommentReplyUrl;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildAddBookCommentReplyTask(
        const char* bookId,
        const char* commentId,
        const char* content)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildAddBookCommentReplyUrl().c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData("user_id", GetCurrentUserName().c_str());
    task->AddPostData("token", GetToken().c_str());
    task->AddPostData("device_id", GetDeviceId().c_str());
    task->AddPostData("book_id", bookId);
    task->AddPostData("comment_id", commentId);
    task->AddPostData("content", content);
    task->AddPostData(ConstStrings::APP_ID, GetAppId().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildVoteBookCommentUrl()
{
    return s_commentHost + s_voteBookCommentUrl;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildCommentReplyTask(
    const char* bookId, 
    const char* commentId, 
    int start, 
    int length)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (task)
    {
        char buf[MAX_URL];
        snprintf(buf,
            DK_DIM(buf),
            (s_commentHost + s_commentReplyUrl).c_str(),
            GetToken().c_str(),
            GetDeviceId().c_str(),
            GetAppId().c_str(),
            bookId,
            commentId,
            start + 1,
            length,
            NULL,
            NULL);
        task->SetUrl(buf);
        PostProcessingDownloadTask(task);
    }
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildVoteBookCommentTask(
        const char* bookId,
        const char* commentId,
        bool useful)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildVoteBookCommentUrl().c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData(ConstStrings::USER_ID, GetCurrentUserName().c_str());
    task->AddPostData(ConstStrings::TOKEN, GetToken().c_str());
    task->AddPostData(ConstStrings::DEVICE_ID, GetDeviceId().c_str());
    task->AddPostData(ConstStrings::BOOK_ID, bookId);
    task->AddPostData(ConstStrings::COMMENT_ID, commentId);
    task->AddPostData(ConstStrings::APP_ID, GetAppId().c_str());
    char buf[10];
    snprintf(buf, DK_DIM(buf), "%d", useful);
    task->AddPostData("useful", buf);
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildBookChangeLogListTask(
    const char* bookId,
    int start,
    int length)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildBookChangeLogListUrl(bookId, start, length).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildBookChangeLogListUrl(
    const char* bookId,
    int start,
    int length)
{
    char buf[MAX_URL];
    snprintf(buf, DK_DIM(buf), (s_bookHost + s_bookChangeLogListUrl).c_str(), bookId, start, length);
    return buf;
}

std::string DownloadTaskBuilder::BuildLoginWithPasswordUrl(const char* email, const char* password)
{
    return s_loginHost + s_loginUrl;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildLoginWithPasswordTask(
        const char* email,
        const char* password)
{
    DownloadTaskSPtr task = CreateAccountDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetMethod(DownloadTask::POST);
    task->SetUrl(BuildLoginWithPasswordUrl(email, password).c_str());
    task->AddPostData(ConstStrings::EMAIL, email);
    task->AddPostData(ConstStrings::PASSWORD, HashPassword(password).c_str());
    //PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildLoginWithTokenUrl(
        const char* email,
        const char* token)
{
    char buf[MAX_URL];
    snprintf(buf,
            DK_DIM(buf),
            (s_loginHost + s_loginWithTokenUrl).c_str(),
            email,
            token,
            GetDeviceId().c_str(),
            GetAppId().c_str());
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildLoginWithTokenTask(const char* email, const char* token)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildLoginWithTokenUrl(email, token).c_str());
    //PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildLogoutUrl(const char* email)
{
    char buf[MAX_URL];
    snprintf(buf,
            DK_DIM(buf),
            (s_loginHost + s_logoutUrl).c_str(),
            email);
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildLogoutTask(const char* email)
{
    DownloadTaskSPtr task = CreateAccountDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildLogoutUrl(email).c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData(ConstStrings::TOKEN, GetToken().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildRegisterUrl()
{
    return s_loginHost + s_registerUrl;
}


DownloadTaskSPtr DownloadTaskBuilder::CreateAccountDownloadTask()
{
    DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    task->AddPostData("deviceid", GetDeviceId().c_str());
    //task->AddPostData("format", "xml");
    task->AddPostData("appid", GetAppId().c_str());
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::CreateBookOrderDownloadTask()
{
    DownloadTaskSPtr task = DownloadTaskSPtr(new BookOrderDownloadTask());
    task->AddCookie("device", GetDeviceId().c_str());
    task->AddCookie("build", GetBuildVersion().c_str());
    task->AddCookie("user", GetCurrentUserName().c_str());
    task->AddCookie(ConstStrings::TOKEN, GetToken().c_str());
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildRegisterTask(
        const char* user,
        const char* alias,
        const char* password)
{
    DownloadTaskSPtr task = CreateAccountDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildRegisterUrl().c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData(ConstStrings::EMAIL, user);
    task->AddPostData(ConstStrings::ALIASNAME, alias);
    task->AddPostData(ConstStrings::PASSWORD, Md5Password(password).c_str());
    task->AddPostData("duration", "311040000");
    PostProcessingDownloadTask(task);
    return task;
}


std::string DownloadTaskBuilder::BuildForgetPasswordUrl(
        const char* user)
{
    char buf[MAX_URL];
    snprintf(buf,
            DK_DIM(buf),
            (s_loginHost + s_forgetPasswordUrl).c_str(),
            user);
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildForgetPasswordTask(const char* user)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildForgetPasswordUrl(user).c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildChangePasswordUrl(
        const char* email,
        const char* oldPassword,
        const char* newPassword)
{
    char buf[MAX_URL];
    snprintf(buf,
            DK_DIM(buf),
            (s_loginHost + s_changePasswordUrl).c_str(),
            email,
            GetToken().c_str(),
            GetDeviceId().c_str(),
            GetAppId().c_str(),
            HttpUtil::UrlEncode(oldPassword).c_str(),
            HttpUtil::UrlEncode(newPassword).c_str());
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildChangePasswordTask(
            const char* email,
            const char* oldPassword,
            const char* newPassword)
{
    DownloadTaskSPtr task = CreateAccountDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(
            BuildChangePasswordUrl(email, oldPassword, newPassword).c_str());
    task->SetMethod(DownloadTask::PUT);
    task->AddPostData(ConstStrings::TOKEN, GetToken().c_str());
    PostProcessingDownloadTask(task);
    return task;
}
        

std::string DownloadTaskBuilder::BuildChangeAliasUrl(
                const char* email,
                const char* newAlias)
{
    char buf[MAX_URL];
    snprintf(buf,
            DK_DIM(buf),
            (s_loginHost + s_changeAliasUrl).c_str(),
            email,
            GetToken().c_str(),
            GetDeviceId().c_str(),
            GetAppId().c_str(),
            HttpUtil::UrlEncode(newAlias).c_str());
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildChangeAliasTask(
        const char* email,
        const char* newAlias)
{
    DownloadTaskSPtr task = CreateAccountDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildChangeAliasUrl(email, newAlias).c_str());
    task->SetMethod(DownloadTask::PUT);
    return task;
}

std::string DownloadTaskBuilder::BuildGetMigrateStageUrl(const std::string& duokanEmail)
{
	char buf[MAX_URL];
    if(!duokanEmail.empty())
	{
        snprintf(buf, DK_DIM(buf), (s_loginHost + s_migrateStageUrl + "?user_id=%s").c_str(), duokanEmail.c_str());
	}
	else
	{
		snprintf(buf, DK_DIM(buf), (s_loginHost + s_migrateStageUrl).c_str());
	}
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildGetMigrateStageTask(const std::string& duokanEmail, const std::string& userData)
{
	DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildGetMigrateStageUrl(duokanEmail).c_str());
    task->SetUserData(userData);
    return task;
}

std::string DownloadTaskBuilder::BuildGetUserInfoUrl(const char* email)
{
	char buf[MAX_URL];
    snprintf(buf,
            DK_DIM(buf),
            (s_loginHost + s_getUserInfoUrl).c_str(),
            email,
            GetToken().c_str(),
            GetDeviceId().c_str(),
            GetAppId().c_str());
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildGetUserInfoTask(const char* email)
{
	DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildGetUserInfoUrl(email).c_str());
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildReportBookBugTask(
    const char* bookId,
    const char* bookName,
    const char* bookRevision,
    const char* position,
    const char* ref_chapter,
    const char* reference,
    const char* suggestion,
    bool bConvertCht)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    char strApp[256] = {0};
    snprintf(strApp, DK_DIM(strApp), "%s for Kindle %s", 
        StringManager::GetPCSTRById(APPLICATION_NAME),
        GetBuildVersion().c_str());

    task->SetUrl(BuildReportBookBugUrl().c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData("app", strApp);
    task->AddPostData("deviceid", GetDeviceId().c_str());
    task->AddPostData("bookid", bookId);
    task->AddPostData("bookname", bookName);
    task->AddPostData("revision", bookRevision);
    task->AddPostData("position", position);
    task->AddPostData("ref_chapter", ref_chapter);
    task->AddPostData("reference", reference);
    task->AddPostData("suggestion", suggestion);
    task->AddPostData("convertcht", bConvertCht ? "1" : "0");
    task->AddPostData("userid", GetCurrentUserName().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildUploadPersonalExperiencesUrl()
{
    return s_syncHost + s_postPersonalExperiencesUrl;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildUploadPersonalExperiencesTask(const char* newData)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }

    task->SetUrl(BuildUploadPersonalExperiencesUrl().c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData("userid", GetCurrentUserName().c_str());
    task->AddPostData("token", GetToken().c_str());
    task->AddPostData("deviceid", GetDeviceId().c_str());
    task->AddPostData("appid", GetAppId().c_str());
    task->AddPostData("newdata", newData);
    PostProcessingDownloadTask(task);
    return task;
}

std::string DownloadTaskBuilder::BuildPersonalExperiencesUrl()
{
    char buf[MAX_URL];
    snprintf(buf, DK_DIM(buf), (s_syncHost + s_getPersonalExperiencesUrl).c_str(),
        GetCurrentUserName().c_str(),
        GetToken().c_str(),
        GetDeviceId().c_str(),
        GetAppId().c_str());
    return buf;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildPersonalExperiencesTask()
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildPersonalExperiencesUrl().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildPushedMessagesTask(const char* messageID)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }
    task->SetUrl(BuildPushedMessagesUrl(messageID).c_str());
    PostProcessingDownloadTask(task);
    return task;

}

std::string DownloadTaskBuilder::BuildHideBookListUrl()
{
	return s_bookHost + s_GetHideBookListUrl;
}

std::string DownloadTaskBuilder::BuildHideBookUrl()
{
	return s_bookHost + s_hideBookUlr;
}

std::string DownloadTaskBuilder::BuildRevealHideBookUrl()
{
	return s_bookHost + s_revealHideBookUrl;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildHideBookListTask(int /* start */, int /* length */, FetchDataOrder)
{
	DownloadTaskSPtr task = CreateDownloadTask();
    if (!task)
    {
        return task;
    }

	task->SetUrl(BuildHideBookListUrl().c_str());
    task->SetMethod(DownloadTask::POST);
	task->AddPostData("user_id", GetCurrentUserName().c_str());
	task->AddPostData("device_id", GetDeviceId().c_str());
	task->AddPostData("token", GetToken().c_str());
	task->AddPostData("app_id", GetAppId().c_str());
    PostProcessingDownloadTask(task);
	return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildHideBookTask(const model::BookInfoList& books)
{
	DownloadTaskSPtr task = CreateBookOrderDownloadTask();
    if (!task && books.empty())
    {
        return task;
    }

	task->SetUrl(BuildHideBookUrl().c_str());
    task->SetMethod(DownloadTask::POST);
	task->AddPostData("user_id", GetCurrentUserName().c_str());
	task->AddPostData("device_id", GetDeviceId().c_str());
	task->AddPostData("token", GetToken().c_str());
	task->AddPostData("app_id", GetAppId().c_str());
	task->AddPostData("book_id", books[0]->GetBookId().c_str());
	dynamic_cast<BookOrderDownloadTask*>(task.get())->SetBooks(books);
	PostProcessingDownloadTask(task);
	return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildRevealHideBookTask(const model::BookInfoList& books)
{
	DownloadTaskSPtr task = CreateBookOrderDownloadTask();
    if (!task && books.empty())
    {
        return task;
    }

	task->SetUrl(BuildRevealHideBookUrl().c_str());
    task->SetMethod(DownloadTask::POST);
	task->AddPostData("user_id", GetCurrentUserName().c_str());
	task->AddPostData("device_id", GetDeviceId().c_str());
	task->AddPostData("token", GetToken().c_str());
	task->AddPostData("app_id", GetAppId().c_str());
	task->AddPostData("book_id", books[0]->GetBookId().c_str());
	dynamic_cast<BookOrderDownloadTask*>(task.get())->SetBooks(books);
    PostProcessingDownloadTask(task);
	return task;
}

std::string DownloadTaskBuilder::BuildGetFavouriteUrl()
{
    return s_bookHost + s_postGetFavouriteUrl;
}

std::string DownloadTaskBuilder::BuildAddFavouriteUrl()
{
    return s_bookHost + s_postAddFavouriteUrl;
}

std::string DownloadTaskBuilder::BuildRemoveFavouriteUrl()
{
    return s_bookHost + s_postRemoveFavouriteUrl;
}

std::string DownloadTaskBuilder::BuildMoveFavouriteToCartUrl()
{
    return s_bookHost + s_postMoveFavouriteToCartUrl;
}

std::string DownloadTaskBuilder::BuildGetBooksInCartUrl()
{
    return s_bookHost + s_postGetBooksInCartUrl;
}

std::string DownloadTaskBuilder::BuildAddBooksInCartUrl()
{
    return s_bookHost + s_postAddBooksToCartUrl;
}

std::string DownloadTaskBuilder::BuildRemoveBooksFromCartUrl()
{
    return s_bookHost + s_postRemoveBooksFromCartUrl;
}

std::string DownloadTaskBuilder::BuildMoveCartToFavouriteUrl()
{
    return s_bookHost + s_postMoveCartToFavouriteUrl;
}

std::string DownloadTaskBuilder::BuildSpecialEventsUrl()
{
    return s_bookHost + s_getPaymentSpecialEventsUrl;
}
std::string DownloadTaskBuilder::BuildXiaomiGetUserCardsUrl()
{
    return s_xiaomiHost + s_getXiaomiUserCards;
}

std::string DownloadTaskBuilder::BuildGetAccountMappingUrl()
{
	return s_loginHost + s_accountMappingsUrl;
}


DownloadTaskSPtr DownloadTaskBuilder::BuildGetFavouriteTask(int start,
                                                            int length,
                                                            FetchDataOrder order)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (task == 0)
    {
        return task;
    }
    task->SetUrl(BuildGetFavouriteUrl().c_str());
    task->SetMethod(DownloadTask::POST);
	task->AddPostData("user_id", GetCurrentUserName().c_str());
	task->AddPostData("token", GetToken().c_str());
	task->AddPostData("device_id", GetDeviceId().c_str());
	task->AddPostData("app_id", GetAppId().c_str());

    PostProcessingDownloadTask(task);
    //DebugPrintf(DLC_DIAGNOSTIC, "\n BuildGetFavouriteTask:%s, %s\n", task->GetAllPostDataAsString().c_str(), BuildGetFavouriteUrl().c_str());
    return task;
}
DownloadTaskSPtr DownloadTaskBuilder::BuildGetBooksInCartTask(int start,
                                                              int length,
                                                              FetchDataOrder order)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    if (task == 0)
    {
        return task;
    }
    task->SetUrl(BuildGetBooksInCartUrl().c_str());
    task->SetMethod(DownloadTask::POST);
	task->AddPostData("user_id", GetCurrentUserName().c_str());
	task->AddPostData("token", GetToken().c_str());
	task->AddPostData("device_id", GetDeviceId().c_str());
	task->AddPostData("app_id", GetAppId().c_str());

    PostProcessingDownloadTask(task);
    //DebugPrintf(DLC_DIAGNOSTIC, "\n BuildGetBooksInCartTask:%s, %s\n", task->GetAllPostDataAsString().c_str(), BuildGetBooksInCartUrl().c_str());
    return task;
}

static bool GetIDStringsFromBookInfoList(const model::BookInfoList& books, std::string& result)
{
    if (books.empty())
    {
        return false;
    }
    result = books.at(0)->GetBookId();
    model::BookInfoList::const_iterator iter = books.begin();
    iter++;
    while (iter != books.end())
    {
        result.append("&")
              .append("item")
              .append("=")
              .append(HttpUtil::UrlEncode((*iter)->GetBookId().c_str()));
        iter++;
    }
    return true;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildAddFavouriteTask(const model::BookInfoList& books)
{
    DownloadTaskSPtr task = CreateBookOrderDownloadTask();
    if (task == 0)
    {
        return task;
    }

    std::string items_id;
    task->SetUrl(BuildAddFavouriteUrl().c_str());
    task->SetMethod(DownloadTask::POST);
	task->AddPostData("user_id", GetCurrentUserName().c_str());
	task->AddPostData("token", GetToken().c_str());
	task->AddPostData("device_id", GetDeviceId().c_str());
	task->AddPostData("app_id", GetAppId().c_str());
    if (GetIDStringsFromBookInfoList(books, items_id))
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "\n Add Books:%s \n", items_id.c_str());
        task->AddPostData("item", items_id.c_str());
        dynamic_cast<BookOrderDownloadTask*>(task.get())->SetBooks(books);
    }
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildRemoveFavouriteTask(const model::BookInfoList& books)
{
    DownloadTaskSPtr task = CreateBookOrderDownloadTask();
    if (task == 0)
    {
        return task;
    }

    std::string items_id;
    task->SetUrl(BuildRemoveFavouriteUrl().c_str());
    task->SetMethod(DownloadTask::POST);
	task->AddPostData("user_id", GetCurrentUserName().c_str());
	task->AddPostData("token", GetToken().c_str());
	task->AddPostData("device_id", GetDeviceId().c_str());
	task->AddPostData("app_id", GetAppId().c_str());
    if (GetIDStringsFromBookInfoList(books, items_id))
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "\n Remove Books:%s \n", items_id.c_str());
        task->AddPostData("item", items_id.c_str());
        dynamic_cast<BookOrderDownloadTask*>(task.get())->SetBooks(books);
    }
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildMoveFavouriteToCart(const model::BookInfoList& books)
{
    DownloadTaskSPtr task = CreateBookOrderDownloadTask();
    if (task == 0)
    {
        return task;
    }

    std::string items_id;
    task->SetUrl(BuildMoveFavouriteToCartUrl().c_str());
    task->SetMethod(DownloadTask::POST);
	task->AddPostData("user_id", GetCurrentUserName().c_str());
	task->AddPostData("token", GetToken().c_str());
	task->AddPostData("device_id", GetDeviceId().c_str());
	task->AddPostData("app_id", GetAppId().c_str());
    if (GetIDStringsFromBookInfoList(books, items_id))
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "\n Move Favourite Books:%s To Cart\n", items_id.c_str());
        task->AddPostData("item", items_id.c_str());
        dynamic_cast<BookOrderDownloadTask*>(task.get())->SetBooks(books);
    }
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildAddBooksInCartTask(const model::BookInfoList& books)
{
    DownloadTaskSPtr task = CreateBookOrderDownloadTask();
    if (task == 0)
    {
        return task;
    }

    std::string items_id;
    task->SetUrl(BuildAddBooksInCartUrl().c_str());
    task->SetMethod(DownloadTask::POST);
	task->AddPostData("user_id", GetCurrentUserName().c_str());
	task->AddPostData("token", GetToken().c_str());
	task->AddPostData("device_id", GetDeviceId().c_str());
	task->AddPostData("app_id", GetAppId().c_str());
    if (GetIDStringsFromBookInfoList(books, items_id))
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "\n Add Books:%s To Cart\n", items_id.c_str());
        task->AddPostData("item", items_id.c_str());
        dynamic_cast<BookOrderDownloadTask*>(task.get())->SetBooks(books);
    }
    PostProcessingDownloadTask(task);
    return task;
}


DownloadTaskSPtr DownloadTaskBuilder::BuildRemoveBooksInCartTask(const model::BookInfoList& books)
{
    DownloadTaskSPtr task = CreateBookOrderDownloadTask();
    if (task == 0)
    {
        return task;
    }

    std::string items_id;
    task->SetUrl(BuildRemoveBooksFromCartUrl().c_str());
    task->SetMethod(DownloadTask::POST);
	task->AddPostData("user_id", GetCurrentUserName().c_str());
	task->AddPostData("token", GetToken().c_str());
	task->AddPostData("device_id", GetDeviceId().c_str());
	task->AddPostData("app_id", GetAppId().c_str());
    if (GetIDStringsFromBookInfoList(books, items_id))
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "\n Remove Books:%s From Cart\n", items_id.c_str());
        task->AddPostData("item", items_id.c_str());
        dynamic_cast<BookOrderDownloadTask*>(task.get())->SetBooks(books);
    }
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildMoveCartToFavourite(const model::BookInfoList& books)
{
    DownloadTaskSPtr task = CreateBookOrderDownloadTask();
    if (task == 0)
    {
        return task;
    }

    std::string items_id;
    task->SetUrl(BuildMoveCartToFavouriteUrl().c_str());
    task->SetMethod(DownloadTask::POST);
	task->AddPostData("user_id", GetCurrentUserName().c_str());
	task->AddPostData("token", GetToken().c_str());
	task->AddPostData("device_id", GetDeviceId().c_str());
	task->AddPostData("app_id", GetAppId().c_str());
    if (GetIDStringsFromBookInfoList(books, items_id))
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "\n Move Cart Books:%s To Favourite\n", items_id.c_str());
        task->AddPostData("item", items_id.c_str());
        dynamic_cast<BookOrderDownloadTask*>(task.get())->SetBooks(books);
    }
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildGetSpecialEventsTask(int start, int length, FetchDataOrder order)
{
    DownloadTaskSPtr task = CreateDownloadTask();
    task->SetUrl(BuildSpecialEventsUrl().c_str());
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildXiaomiGetUserCardsTask(const std::vector<std::string>& ids)
{
    if (ids.empty())
    {
        return DownloadTaskSPtr();
    }

    DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    std::string path = BuildXiaomiGetUserCardsUrl();
    path.append("?ids=");
    for (int i = 0; i < (ids.size() - 1); ++i)
    {
        path.append(ids[i]).append(",");
    }
    path.append(ids[ids.size() - 1]);
    task->SetUrl(path.c_str());
    return task;

}

DownloadTaskSPtr DownloadTaskBuilder::BuildPostSQMTask(const char* data)
{
	DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    task->SetUrl(s_postSQMUrl.c_str());
	task->SetMethod(DownloadTask::POST);
	task->AddPostData("deviceid", GetDeviceId().c_str());
	task->AddPostData("version", SystemSettingInfo::GetInstance()->GetVersion().GetBuffer());
	task->AddPostData("logtype", "0");
	task->AddPostData("sqm_fields", data);
    PostProcessingDownloadTask(task);
    return task;
}

DownloadTaskSPtr DownloadTaskBuilder::BuildGetAccountMappingTask()
{
	DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    task->SetUrl(BuildGetAccountMappingUrl().c_str());
	task->SetMethod(DownloadTask::POST);
	task->AddPostData("user_id", GetCurrentUserName().c_str());
	task->AddPostData("device_id", GetDeviceId().c_str());
	task->AddPostData("app_id", GetAppId().c_str());
	task->AddPostData("token", GetToken().c_str());
	task->AddPostData("format", "json");
    PostProcessingDownloadTask(task);
    return task;
}

} // namespace dkstore
} // namespace dk
