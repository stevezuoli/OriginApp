/*
 * =====================================================================================
 *       Filename:  SinaWeiboMethodImpl.h
 *    Description:  新浪微博接口
 *
 *        Version:  1.0
 *        Created:  12/03/2012 03:19:08 PM
 * =====================================================================================
 */

#ifndef SinaWeiboMethodImpl_H
#define SinaWeiboMethodImpl_H

#include <string>
#include "Weibo/IWeiboMethod.h"
#include "DownloadManager/DownloadTask.h"

namespace dk
{
namespace weibo
{
class SinaWeiboMethodImpl : public IWeiboMethod
{
public:
    static const char* EventOauthAchieved;
    static const char* EventPostUpdateAchieved;
    static const char* EventGetUsersShowAchieved;

public:
    eWeiboResultCode Authorize();
    eWeiboResultCode Oauth2(const char* userName, const char* password);
    eWeiboResultCode Oauth2Code(const char* authCode, const char* url);
    virtual eWeiboResultCode EndSession();

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 微博读取
    virtual eWeiboResultCode GetStatusesPublicTimeline() ;
    virtual eWeiboResultCode GetStatusesFriendTimeline() ;
    virtual eWeiboResultCode GetStatusesHomeTimeline() ;
    virtual eWeiboResultCode GetStatusesUserTimeline(ID &usrId) ;
    virtual eWeiboResultCode GetStatusesTimelineBatch(ID &usrId) ;
    virtual eWeiboResultCode GetStatusesRePostTimeline(const char* id) ;
    virtual eWeiboResultCode GetStatusesRePostByMe() ;
    virtual eWeiboResultCode GetStatusesMentions() ;
    virtual eWeiboResultCode GetStatusesShow(const char* id) ;
    virtual eWeiboResultCode GetStatusesShowBatch(const char* inputIDs) ;
    virtual eWeiboResultCode GetStatusesQueryMID(const char *weiboId, const int type, const int is_batch = 0) ;
    virtual eWeiboResultCode GetStatusesQueryID(const char *mId, const int type, const int is_batch = 0, const int inbox = 0, const int isBase62 = 0) ;
    virtual eWeiboResultCode GetStatusesRePostDaily() ;
    virtual eWeiboResultCode GetStatusesRePostWeekly() ;
    virtual eWeiboResultCode GetStatusesHotCommmentsDaily() ;
    virtual eWeiboResultCode GetStatusesHotCommmentsWeekly() ;

    // 微博写入
    virtual eWeiboResultCode PostStatusesRePost(const char *weiboId, const char *statusText, const int isComment) ;
    virtual eWeiboResultCode PostStatusesDestroy(const char *weiboId) ;
    virtual eWeiboResultCode PostStatusesUpdate(const char* statusText) ;
    virtual eWeiboResultCode PostStatusesUploadUrlText(const char* statusText, const char *picInfo, bool isPicUrl) ;
    virtual eWeiboResultCode PostStatusesUpload(const char* statusText, const char *filePath) ;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 评论读取
    virtual eWeiboResultCode GetCommentsShow(const char *weiboId) ;
    virtual eWeiboResultCode GetCommentsByMe() ;
    virtual eWeiboResultCode GetCommentsToMe() ;
    virtual eWeiboResultCode GetCommentsTimeline() ;
    virtual eWeiboResultCode GetCommentsMentions() ;

    // 评论写入
    virtual eWeiboResultCode GetCommentsShowBatch(const char* commentIds) ;
    virtual eWeiboResultCode PostCommentsCreate(const char* id, const char* comment, const int commentOri) ;
    virtual eWeiboResultCode PostCommentsDestroy(const char* id) ;
    virtual eWeiboResultCode PostCommentsDestroyBatch(const char* id) ;
    virtual eWeiboResultCode PostCommentsReply(const char* commentId, const char* comment, const char* weiboId
            , const int withoutMention, const int commentOri) ;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 私信读取
    virtual eWeiboResultCode GetDirectMessages() ;
    virtual eWeiboResultCode GetDirectMessagesSent() ;
    virtual eWeiboResultCode GetDirectMessagesUserList() ;
    virtual eWeiboResultCode GetDirectMessagesCoversation(ID &usrId) ;
    virtual eWeiboResultCode GetDirectMessagesShowBatch(const char* id) ;
    virtual eWeiboResultCode GetDirectMessagesIsCapable(const char* uid) ;

    // 私信写入
    virtual eWeiboResultCode PostDirectMessagesNew(const char* text, const ID& uid, const char* fields = NULL,
            const char* weiboId = NULL);
    virtual eWeiboResultCode PostDirectMessagesDestory(const char* id) ;
    virtual eWeiboResultCode PostDirectMessagesDestoryBatch(const char* ids, const char* uid);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 用户读取
    virtual eWeiboResultCode GetUsersShow(ID& usrId, const char* extend = NULL) ;
    virtual eWeiboResultCode GetUsersDomainShow(const char* domain = NULL, const char* extend = NULL) ;
    virtual eWeiboResultCode GetUsersShowBatch(ID& usrId, const char* extend = NULL) ;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 关注读取
    virtual eWeiboResultCode GetFriendshipsFriends(ID& usrId, const int order/*= 0*/) ;
    virtual eWeiboResultCode GetFriendshipsInCommon(const char* id, const char* suid) ;
    virtual eWeiboResultCode GetFriendshipsBilateral(const char* uid) ;
    virtual eWeiboResultCode GetFriendshipsBilateralIDs(const char* uid) ;
    virtual eWeiboResultCode GetFriendshipsFriendIDs(ID& usrId) ;
    virtual eWeiboResultCode GetFriendshipsFriendRemarkBatch(const char* uids) ;

    // 粉丝读取
    virtual eWeiboResultCode GetFriendshipsFriendsFollowers(ID &usrId) ;
    virtual eWeiboResultCode GetFriendshipsFriendsFollowersIDs(ID &usrId) ;
    virtual eWeiboResultCode GetFriendshipsFriendsFollowersActive(ID &usrIDs) ;

    // 关系链读取
    virtual eWeiboResultCode GetFriendshipsFriendsChainFollowers(const char* uid) ;

    // 关系状态
    virtual eWeiboResultCode GetFriendshipShow(ID& sourceId, ID& tarGetId) ;

    // 关系写入
    virtual eWeiboResultCode PostFriendshipsCreate(ID &usrId, const int skipCheck) ;
    virtual eWeiboResultCode PostFriendshipsCreateBatch(const char* uids) ;
    virtual eWeiboResultCode PostFriendshipsDestroy(ID &usrId) ;
    virtual eWeiboResultCode PostFriendshipsFollowersDestroy(const char* uid) ;
    virtual eWeiboResultCode PostFriendshipsRemarkUpdate(const char* uid, const char* remark) ;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 帐号读取
    virtual eWeiboResultCode GetAccountProfileBasic(const char *uid);
    virtual eWeiboResultCode GetAccountProfileEducation(const char* uid);
    virtual eWeiboResultCode GetAccountProfileEducationBatch(const char* uids);
    virtual eWeiboResultCode GetAccountProfileCareer(const char* uid);
    virtual eWeiboResultCode GetAccountProfileCareerBatch(const char* uids);
    virtual eWeiboResultCode GetAccountGetPrivacy();
    virtual eWeiboResultCode GetAccountProfileSchoolList(const int province = 0, const int city = 0, const int area = 0, const int type = 0, 
            const char* capital = 0, const char* keyword = 0, const int counts = 0);
    virtual eWeiboResultCode GetAccountRateLimitStatus();
    virtual eWeiboResultCode GetAccountGetUID();

    // 帐号写入
    virtual eWeiboResultCode PostAccountProfileBasicUpdate(const BasicInfo& basic);
    virtual eWeiboResultCode PostAccountProfileEduUpdate(const EducationInfo& edu);
    virtual eWeiboResultCode PostAccountProfileEduDestroy(const char* id);
    virtual eWeiboResultCode PostAccountProfileCarUpdate(const CareerInfo& career);
    virtual eWeiboResultCode PostAccountProfileCarDestroy(const char* id);
    virtual eWeiboResultCode PostAccountAvatarUpload(const char* filePath);
    virtual eWeiboResultCode PostAccountUpdatePrivacy(const int comment = 0, const int geo = 0, const int message = 0,
            const int realname = 0, const int badge = 0, const int mobile = 0);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 收藏读取
    virtual eWeiboResultCode GetFavorites() ;
    virtual eWeiboResultCode GetFavoritesShow(const char* id) ;
    virtual eWeiboResultCode GetFavoritesByTags(const char* tid) ;
    virtual eWeiboResultCode GetFavoritesTags() ;

    // 收藏写入
    virtual eWeiboResultCode PostFavoritesCreate(const char* id) ;
    virtual eWeiboResultCode PostFavoritesDestroy(const char* id) ;
    virtual eWeiboResultCode PostFavoritesDestroyBatch(const char* ids) ;
    virtual eWeiboResultCode PostFavoritesTagUpdate(const char* tagId, const char* tagName) ;
    virtual eWeiboResultCode PostFavoritesTagsUpdateBatch(const char* tagId, const char* tagName) ;
    virtual eWeiboResultCode PostFavoritesTagsDestroyBatch(const char* tagId) ;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 话题读取
    virtual eWeiboResultCode GetTrends(const char* uid) ;
    virtual eWeiboResultCode GetTrendsStatuses(const char* trendWords, const char* provinceId) ;
    virtual eWeiboResultCode GetTrendsHourly() ;
    virtual eWeiboResultCode GetTrendsDaily() ;
    virtual eWeiboResultCode GetTrendsWeekly() ;

    // 话题写入
    virtual eWeiboResultCode PostTrendsFollow(const char* trendName) ;
    virtual eWeiboResultCode PostTrendsDestroy(const char* trendId) ;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 标签读取
    virtual eWeiboResultCode GetTags(const char* uid) ;
    virtual eWeiboResultCode GetTagsTagsBatch(const char* uids) ;
    virtual eWeiboResultCode GetTagsSuggestions() ;

    // 标签写入
    virtual eWeiboResultCode PostTagsCreate(const char* tags);
    virtual eWeiboResultCode PostTagsDestroy(const char* tagId) ;
    virtual eWeiboResultCode PostTagsDestroyBatch(const char* tagIds) ;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 搜索
    virtual eWeiboResultCode GetSearchSuggestionsUsers(const char* q, const int count = 0);
    virtual eWeiboResultCode GetSearchSuggestionsStatues(const char* q, const int count = 0);
    virtual eWeiboResultCode GetSearchSuggestionsSchool(const char* q, const int count = 0);
    virtual eWeiboResultCode GetSearchSuggestionsCompanies(const char* q, const int count = 0);
    virtual eWeiboResultCode GetSearchSuggestionsApps(const char* q, const int count = 0);
    virtual eWeiboResultCode GetSearchSuggestionsAtUsers(const char* q, const int type, const int count = 0, const int range = 0);
    virtual eWeiboResultCode GetSearchSuggestionsIntegrate(const char* query, const int sort_user = 0, const int sort_app = 0,
            const int sort_grp = 0, const int user_count = 0, const int app_count = 0, const int grp_count = 0);
    virtual eWeiboResultCode GetSearchStatuses(const char* q, const int filter_ori = 0, const int filter_pic = 0, const char* fuid = 0, 
            const int province = 0, const int city = 0, const char* starttime = NULL, const char* endtime = NULL, const int count = 0, 
            const int page = 0, const bool needcount = 0, const int base_app = 0);
    virtual eWeiboResultCode GetSearchGeoStatuses(const float longitude, const float latitudeconst, const int range = 0, const int time = 0,
            const int sort_type = 0, const int page = 0, const int count = 0, const int base_app = 0);
    virtual eWeiboResultCode GetSearchUsers(const char* q, const int snick = 0, const int sdomain = 0, const int sintro = 0, const int stag = 0, 
            const int province = 0, const int city = 0, const char* gender = 0, const char* comorsch = 0, const int sort = 0,
            const int count = 0, const int page = 0, const int base_app = 0);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 推荐读取
    virtual eWeiboResultCode GetSuggestionsFavoritesHot() ;
    virtual eWeiboResultCode GetSuggestionsUsersHot(const char* category) ;
    virtual eWeiboResultCode GetSuggestionsUsersMayInterested() ;
    virtual eWeiboResultCode GetSuggestionsUsersByStatus(const char* content, const int num = 0, const char* url = NULL) ;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 推荐写入
    virtual eWeiboResultCode PostSuggestionsUsersNotInterested(const char* uid) ;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Short URL
    virtual eWeiboResultCode GetShortURLShorten(const char* url_long);
    virtual eWeiboResultCode GetShortURLExpand(const char* url_short);
    virtual eWeiboResultCode GetShortURLShareCounts(const char* url_short);
    virtual eWeiboResultCode GetShortURLShareStatuses(const char* url_short);
    virtual eWeiboResultCode GetShortURLCommentCounts(const char* url_short);
    virtual eWeiboResultCode GetShortURLCommentComments(const char* url_short);
    virtual eWeiboResultCode GetShortUrlInfo(const char* shortUrlIDs);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Unread
    virtual eWeiboResultCode GetRemindUnreadCount(const char* uid);
    virtual eWeiboResultCode PostStatusesResetCount(const int type);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Groups
    virtual eWeiboResultCode GetGroupsJoined(const char* uid, const int page, const int count, 
            const int type, const int filter, const int sort, const int member_limit, const int simplify);
    virtual eWeiboResultCode GetGroupsShowBatch(const char* gids);
    virtual eWeiboResultCode GetGroupsGetAnnouncement(const char* gid);
    virtual eWeiboResultCode GetGroupsAdmin(const char* gid);
    virtual eWeiboResultCode GetGroupsUsers(const char* gid);
    virtual eWeiboResultCode GetGroupsSuggestionMayInterested(const int count);
    virtual eWeiboResultCode GetGroupsChatIsBlocked(const char* gids);
    virtual eWeiboResultCode PostGroupsChatBlock(const char* gid);
    virtual eWeiboResultCode PostGroupsChatUnblock(const char* gid);
    virtual eWeiboResultCode GetGroupsUsersJoinedStatuses(const char* uid,int simplify);

private:
    void SubscribeDownloadEvent(DownloadTaskSPtr& downloadTask,
            bool (SinaWeiboMethodImpl::*memfun)(const EventArgs&));
    bool OnOauthAchieved(const EventArgs& args);
    bool OnStatusUpdateAchieved(const EventArgs& args);
    bool OnUsersShowAchieved(const EventArgs& args);
    bool OnStatusUploadAchieved(const EventArgs& args);
};
}//weibo
}//dk

#endif//SinaWeiboMethodImpl_H

