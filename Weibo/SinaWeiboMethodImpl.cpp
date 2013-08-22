#include <tr1/functional>
#include <string>
#include "Weibo/SinaWeiboManager.h"
#include "Weibo/SinaWeiboMethodImpl.h"
#include "Weibo/SinaWeiboTaskBuilder.h"
#include "Weibo/SinaWeiboResultParser.h"
#include "Weibo/WeiboFactory.h"
#include "DownloadManager/DownloadManager.h"
#include "Utility/HttpUtil.h"
#include "interface.h"
using namespace dk::utility;

namespace dk
{
namespace weibo
{

const char* SinaWeiboMethodImpl::EventOauthAchieved = "EventOauthAchieved";
const char* SinaWeiboMethodImpl::EventPostUpdateAchieved = "EventPostUpdateAchieved";
const char* SinaWeiboMethodImpl::EventGetUsersShowAchieved = "EventGetUsersShowAchieved";

void SinaWeiboMethodImpl::SubscribeDownloadEvent(DownloadTaskSPtr& downloadTask, bool (SinaWeiboMethodImpl::*memfun)(const EventArgs&))
{
    if (!downloadTask)
        return;

    SubscribeEvent(DownloadTask::EventDownloadTaskFinished, 
            *downloadTask.get(), 
            std::tr1::bind(
                std::tr1::mem_fn(memfun), 
                this,
                std::tr1::placeholders::_1));
}

eWeiboResultCode SinaWeiboMethodImpl::Authorize() { return WRC_NOT_IMPL; }

eWeiboResultCode SinaWeiboMethodImpl::Oauth2(const char* userName, const char* password)
{ 
    DownloadTaskSPtr task = SinaWeiboTaskBuilder::BuildOauth2UsernamePwdTask(userName, password);
    if (!task)
    {
        return WRC_INTERNAL_ERROR;
    }
    SubscribeDownloadEvent(task, &SinaWeiboMethodImpl::OnOauthAchieved);
    DownloadManager::GetInstance()->AddTask(task);

    return WRC_OK;
}

eWeiboResultCode SinaWeiboMethodImpl::Oauth2Code(const char* authCode, const char* url)
{
    DownloadTaskSPtr task = SinaWeiboTaskBuilder::BuildOauth2CodeTask(authCode, url);
    if (!task)
    {
        return WRC_INTERNAL_ERROR;
    }
    SubscribeDownloadEvent(task, &SinaWeiboMethodImpl::OnOauthAchieved);
    DownloadManager::GetInstance()->AddTask(task);

    return WRC_OK;
}

bool SinaWeiboMethodImpl::OnOauthAchieved(const EventArgs& args)
{
    std::cout << "" << __FUNCTION__ << std::endl;
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;

    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    if (task)
    {
        WeiboResultArgs args;
        std::string resultString = task->GetString();
        if (downloadTaskFinishedArgs.succeeded)
        {
            args.mResultType = WeiboResultArgs::ERT_CORRECT;
            args.mParsingInfo = SinaWeiboResultParser::DoParseOauthRet(resultString.c_str());
            ParsingOauthRetSPtr oauthRet = std::tr1::dynamic_pointer_cast<ParsingOauthRet>(args.mParsingInfo);
            if (oauthRet)
            {
                WeiboFactory::GetSinaWeibo()->SetUserInfo(oauthRet->GetAccessToken().c_str(), oauthRet->GetExpiresIn(), oauthRet->GetUid().c_str());
                ID id(ID::IDT_ID, oauthRet->GetUid().c_str());
                GetUsersShow(id);
            }
        }
        else if (task->GetString().empty())
        {
            DebugPrintf(DLC_DIAGNOSTIC, "SinaWeiboMethodImpl::OnOauthAchieved ERT_NETWORKISSUE");
            args.mResultType = WeiboResultArgs::ERT_NETWORKISSUE;
        }
        else
        {
            DebugPrintf(DLC_DIAGNOSTIC, "SinaWeiboMethodImpl::OnOauthAchieved \n %s", resultString.c_str());
            args.mResultType = WeiboResultArgs::ERT_ERROR;
            args.mParsingInfo = SinaWeiboResultParser::DoParseApiError(resultString.c_str());
        }

        FireEvent(EventOauthAchieved, args);
    }

    return false;
}

eWeiboResultCode SinaWeiboMethodImpl::EndSession() { return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 微博读取
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesPublicTimeline() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesFriendTimeline() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesHomeTimeline() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesUserTimeline(ID &usrId) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesTimelineBatch(ID &usrId) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesRePostTimeline(const char* id) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesRePostByMe() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesMentions() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesShow(const char* id) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesShowBatch(const char* inputIDs) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesQueryMID(const char *weiboId, const int type, const int is_batch) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesQueryID(const char *mId, const int type, const int is_batch, const int inbox, const int isBase62) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesRePostDaily() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesRePostWeekly() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesHotCommmentsDaily() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetStatusesHotCommmentsWeekly() { return WRC_NOT_IMPL; }

// 微博写入
eWeiboResultCode SinaWeiboMethodImpl::PostStatusesRePost(const char *weiboId, const char *statusText, const int isComment) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostStatusesDestroy(const char *weiboId) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostStatusesUpdate(const char* statusText)
{
    DebugPrintf(DLC_DIAGNOSTIC, "SinaWeiboMethodImpl::PostStatusesUpdate");
    DownloadTaskSPtr task = SinaWeiboTaskBuilder::BuildStatusUpdate(statusText);
    if (!task)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "SinaWeiboMethodImpl::PostStatusesUpdate return ERROR");
        return WRC_INTERNAL_ERROR;
    }
    SubscribeDownloadEvent(task, &SinaWeiboMethodImpl::OnStatusUpdateAchieved);
    DownloadManager::GetInstance()->AddTask(task);

        DebugPrintf(DLC_DIAGNOSTIC, "SinaWeiboMethodImpl::PostStatusesUpdate return OK");
    return WRC_OK;
}
bool SinaWeiboMethodImpl::OnStatusUpdateAchieved(const EventArgs& args)
{
    std::cout << "" << __FUNCTION__ << std::endl;
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;

    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    if (task)
    {
        WeiboResultArgs args;
        std::string resultString = task->GetString();
        if (downloadTaskFinishedArgs.succeeded)
        {
            args.mResultType = WeiboResultArgs::ERT_CORRECT;
        }
        else if (task->GetString().empty())
        {
            DebugPrintf(DLC_DIAGNOSTIC, "SinaWeiboMethodImpl::OnStatusUpdateAchieved ERT_NETWORKISSUE");
            args.mResultType = WeiboResultArgs::ERT_NETWORKISSUE;
        }
        else
        {
            DebugPrintf(DLC_DIAGNOSTIC, "SinaWeiboMethodImpl::OnStatusUpdateAchieved \n%s", resultString.c_str());
            args.mResultType = WeiboResultArgs::ERT_ERROR;
            args.mParsingInfo = SinaWeiboResultParser::DoParseApiError(resultString.c_str());
        }

        FireEvent(EventPostUpdateAchieved, args);
    }

    return true;
}

eWeiboResultCode SinaWeiboMethodImpl::PostStatusesUploadUrlText(const char* statusText, const char *picInfo, bool isPicUrl) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostStatusesUpload(const char* statusText, const char *filePath)
{
    DownloadTaskSPtr task = SinaWeiboTaskBuilder::BuildStatusUpload(statusText, filePath);
    if (!task)
    {
        return WRC_INTERNAL_ERROR;
    }
    SubscribeDownloadEvent(task, &SinaWeiboMethodImpl::OnStatusUploadAchieved);
    DownloadManager::GetInstance()->AddTask(task);

    return WRC_OK;
}

bool SinaWeiboMethodImpl::OnStatusUploadAchieved(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    if (task)
    {
        WeiboResultArgs args;
        std::string resultString = task->GetString();
        if (downloadTaskFinishedArgs.succeeded)
        {
            args.mResultType = WeiboResultArgs::ERT_CORRECT;
        }
        else if (task->GetString().empty())
        {
            DebugPrintf(DLC_DIAGNOSTIC, "SinaWeiboMethodImpl::OnStatusUploadAchieved ERT_NETWORKISSUE");
            args.mResultType = WeiboResultArgs::ERT_NETWORKISSUE;
        }
        else
        {
            DebugPrintf(DLC_DIAGNOSTIC, "SinaWeiboMethodImpl::OnStatusUploadAchieved \n%s", resultString.c_str());
            args.mResultType = WeiboResultArgs::ERT_ERROR;
            args.mParsingInfo = SinaWeiboResultParser::DoParseApiError(resultString.c_str());
        }

        FireEvent(EventPostUpdateAchieved, args);
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 评论读取
eWeiboResultCode SinaWeiboMethodImpl::GetCommentsShow(const char *weiboId) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetCommentsByMe() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetCommentsToMe() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetCommentsTimeline() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetCommentsMentions() { return WRC_NOT_IMPL; }

// 评论写入
eWeiboResultCode SinaWeiboMethodImpl::GetCommentsShowBatch(const char* commentIds) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostCommentsCreate(const char* id, const char* comment, const int commentOri) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostCommentsDestroy(const char* id) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostCommentsDestroyBatch(const char* id) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostCommentsReply(const char* commentId, const char* comment, const char* weiboId
        , const int withoutMention, const int commentOri) { return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 私信读取
eWeiboResultCode SinaWeiboMethodImpl::GetDirectMessages() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetDirectMessagesSent() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetDirectMessagesUserList() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetDirectMessagesCoversation(ID &usrId) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetDirectMessagesShowBatch(const char* id) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetDirectMessagesIsCapable(const char* uid) { return WRC_NOT_IMPL; }

// 私信写入
eWeiboResultCode SinaWeiboMethodImpl::PostDirectMessagesNew(const char* text, const ID& uid, const char* fields,
        const char* weiboId){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostDirectMessagesDestory(const char* id) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostDirectMessagesDestoryBatch(const char* ids, const char* uid){ return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 用户读取
eWeiboResultCode SinaWeiboMethodImpl::GetUsersShow(ID& userId, const char* extend)
{
    string uid;
    if (userId.idType == ID::IDT_ID)
    {
        uid = userId.id;
    }
    DownloadTaskSPtr task = SinaWeiboTaskBuilder::BuildGetUsersShow(uid.c_str());
    if (!task)
    {
        return WRC_INTERNAL_ERROR;
    }
    SubscribeDownloadEvent(task, &SinaWeiboMethodImpl::OnUsersShowAchieved);
    DownloadManager::GetInstance()->AddTask(task);

    return WRC_OK;
}

bool SinaWeiboMethodImpl::OnUsersShowAchieved(const EventArgs& args)
{
    std::cout << "" << __FUNCTION__ << std::endl;
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;

    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    if (task)
    {
        WeiboResultArgs args;
        std::string resultString = task->GetString();
        if (downloadTaskFinishedArgs.succeeded)
        {
            args.mResultType = WeiboResultArgs::ERT_CORRECT;
            args.mParsingInfo = SinaWeiboResultParser::DoParseUser(resultString.c_str());
            ParsingUserSPtr user = std::tr1::dynamic_pointer_cast<ParsingUser>(args.mParsingInfo);
            if (user)
            {
                WeiboFactory::GetSinaWeibo()->SetUserName(user->GetScreenName().c_str());
            }
        }
        else
        {
            DebugPrintf(DLC_DIAGNOSTIC, "SinaWeiboMethodImpl::OnUsersShowAchieved \n%s", resultString.c_str());
            args.mResultType = WeiboResultArgs::ERT_ERROR;
            args.mParsingInfo = SinaWeiboResultParser::DoParseApiError(resultString.c_str());
        }

        FireEvent(EventGetUsersShowAchieved, args);
    }

    return true;
}

eWeiboResultCode SinaWeiboMethodImpl::GetUsersDomainShow(const char* domain, const char* extend) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetUsersShowBatch(ID& usrId, const char* extend) { return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 关注读取
eWeiboResultCode SinaWeiboMethodImpl::GetFriendshipsFriends(ID& usrId, const int order) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetFriendshipsInCommon(const char* id, const char* suid) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetFriendshipsBilateral(const char* uid) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetFriendshipsBilateralIDs(const char* uid) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetFriendshipsFriendIDs(ID& usrId) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetFriendshipsFriendRemarkBatch(const char* uids) { return WRC_NOT_IMPL; }

// 粉丝读取
eWeiboResultCode SinaWeiboMethodImpl::GetFriendshipsFriendsFollowers(ID &usrId) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetFriendshipsFriendsFollowersIDs(ID &usrId) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetFriendshipsFriendsFollowersActive(ID &usrIDs) { return WRC_NOT_IMPL; }

// 关系链读取
eWeiboResultCode SinaWeiboMethodImpl::GetFriendshipsFriendsChainFollowers(const char* uid) { return WRC_NOT_IMPL; }

// 关系状态
eWeiboResultCode SinaWeiboMethodImpl::GetFriendshipShow(ID& sourceId, ID& tarGetId) { return WRC_NOT_IMPL; }

// 关系写入
eWeiboResultCode SinaWeiboMethodImpl::PostFriendshipsCreate(ID &usrId, const int skipCheck) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostFriendshipsCreateBatch(const char* uids) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostFriendshipsDestroy(ID &usrId) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostFriendshipsFollowersDestroy(const char* uid) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostFriendshipsRemarkUpdate(const char* uid, const char* remark) { return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 帐号读取
eWeiboResultCode SinaWeiboMethodImpl::GetAccountProfileBasic(const char *uid){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetAccountProfileEducation(const char* uid){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetAccountProfileEducationBatch(const char* uids){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetAccountProfileCareer(const char* uid){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetAccountProfileCareerBatch(const char* uids){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetAccountGetPrivacy(){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetAccountProfileSchoolList(const int province, const int city, const int area, const int type, 
        const char* capital, const char* keyword, const int counts){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetAccountRateLimitStatus(){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetAccountGetUID(){ return WRC_NOT_IMPL; }

// 帐号写入
eWeiboResultCode SinaWeiboMethodImpl::PostAccountProfileBasicUpdate(const BasicInfo& basic){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostAccountProfileEduUpdate(const EducationInfo& edu){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostAccountProfileEduDestroy(const char* id){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostAccountProfileCarUpdate(const CareerInfo& career){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostAccountProfileCarDestroy(const char* id){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostAccountAvatarUpload(const char* filePath){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostAccountUpdatePrivacy(const int comment, const int geo, const int message,
        const int realname, const int badge, const int mobile){ return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 收藏读取
eWeiboResultCode SinaWeiboMethodImpl::GetFavorites() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetFavoritesShow(const char* id) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetFavoritesByTags(const char* tid) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetFavoritesTags() { return WRC_NOT_IMPL; }

// 收藏写入
eWeiboResultCode SinaWeiboMethodImpl::PostFavoritesCreate(const char* id) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostFavoritesDestroy(const char* id) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostFavoritesDestroyBatch(const char* ids) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostFavoritesTagUpdate(const char* tagId, const char* tagName) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostFavoritesTagsUpdateBatch(const char* tagId, const char* tagName) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostFavoritesTagsDestroyBatch(const char* tagId) { return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 话题读取
eWeiboResultCode SinaWeiboMethodImpl::GetTrends(const char* uid) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetTrendsStatuses(const char* trendWords, const char* provinceId) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetTrendsHourly() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetTrendsDaily() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetTrendsWeekly() { return WRC_NOT_IMPL; }

// 话题写入
eWeiboResultCode SinaWeiboMethodImpl::PostTrendsFollow(const char* trendName) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostTrendsDestroy(const char* trendId) { return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 标签读取
eWeiboResultCode SinaWeiboMethodImpl::GetTags(const char* uid) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetTagsTagsBatch(const char* uids) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetTagsSuggestions() { return WRC_NOT_IMPL; }

// 标签写入
eWeiboResultCode SinaWeiboMethodImpl::PostTagsCreate(const char* tags){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostTagsDestroy(const char* tagId) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostTagsDestroyBatch(const char* tagIds) { return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 搜索
eWeiboResultCode SinaWeiboMethodImpl::GetSearchSuggestionsUsers(const char* q, const int count){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetSearchSuggestionsStatues(const char* q, const int count){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetSearchSuggestionsSchool(const char* q, const int count){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetSearchSuggestionsCompanies(const char* q, const int count){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetSearchSuggestionsApps(const char* q, const int count){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetSearchSuggestionsAtUsers(const char* q, const int type, const int count, const int range){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetSearchSuggestionsIntegrate(const char* query, const int sort_user, const int sort_app,
        const int sort_grp, const int user_count, const int app_count, const int grp_count){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetSearchStatuses(const char* q, const int filter_ori, const int filter_pic, const char* fuid, 
        const int province, const int city, const char* starttime, const char* endtime, const int count, 
        const int page, const bool needcount, const int base_app){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetSearchGeoStatuses(const float longitude, const float latitudeconst, const int range, const int time,
        const int sort_type, const int page, const int count, const int base_app){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetSearchUsers(const char* q, const int snick, const int sdomain, const int sintro, const int stag, 
        const int province, const int city, const char* gender, const char* comorsch, const int sort,
        const int count, const int page, const int base_app){ return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 推荐读取
eWeiboResultCode SinaWeiboMethodImpl::GetSuggestionsFavoritesHot() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetSuggestionsUsersHot(const char* category) { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetSuggestionsUsersMayInterested() { return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetSuggestionsUsersByStatus(const char* content, const int num, const char* url) { return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 推荐写入
eWeiboResultCode SinaWeiboMethodImpl::PostSuggestionsUsersNotInterested(const char* uid) { return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Short URL
eWeiboResultCode SinaWeiboMethodImpl::GetShortURLShorten(const char* url_long){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetShortURLExpand(const char* url_short){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetShortURLShareCounts(const char* url_short){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetShortURLShareStatuses(const char* url_short){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetShortURLCommentCounts(const char* url_short){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetShortURLCommentComments(const char* url_short){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetShortUrlInfo(const char* shortUrlIDs){ return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Unread
eWeiboResultCode SinaWeiboMethodImpl::GetRemindUnreadCount(const char* uid){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostStatusesResetCount(const int type){ return WRC_NOT_IMPL; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Groups
eWeiboResultCode SinaWeiboMethodImpl::GetGroupsJoined(const char* uid, const int page, const int count, 
        const int type, const int filter, const int sort, const int member_limit, const int simplify){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetGroupsShowBatch(const char* gids){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetGroupsGetAnnouncement(const char* gid){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetGroupsAdmin(const char* gid){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetGroupsUsers(const char* gid){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetGroupsSuggestionMayInterested(const int count){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetGroupsChatIsBlocked(const char* gids){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostGroupsChatBlock(const char* gid){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::PostGroupsChatUnblock(const char* gid){ return WRC_NOT_IMPL; }
eWeiboResultCode SinaWeiboMethodImpl::GetGroupsUsersJoinedStatuses(const char* uid,int simplify){ return WRC_NOT_IMPL; }
}//weibo
}//dk
