#ifndef IWEIBOMETHOD_H
#define IWEIBOMETHOD_H
#include "Weibo/IWeiboDef.h"
#include "GUI/EventListener.h"
#include "GUI/EventSet.h"

namespace dk
{
/** Weibo interface for weibo v4
* Please reference http://open.weibo.com/wiki/API%E6%96%87%E6%A1%A3_V2 
*/
namespace weibo
{
enum eMethodAction
{
    WBOPT_NONE,

    WBOPT_OAUTH2_ACCESS_TOKEN,
    WBOPT_OAUTH2_AUTHORIZE,
    WBOPT_END_SESSION,

    WBOPT_GET_STATUSES_PUBLIC_TIMELINE,//获取最新更新的公共微博消息 
    WBOPT_GET_STATUSES_FRIENDS_TIMELINE,//获取当前用户所关注用户的最新微博信息 (别名: statuses/home_timeline) 
    WBOPT_GET_STATUSES_HOME_TIMELINE,//获取当前登录用户及其所关注用户的最新微博消息
    WBOPT_GET_STATUSES_USER_TIMELINE,//获取用户发布的微博信息列表 
    WBOPT_GET_STATUSES_TIMELINE_BATCH,//批量获得指定的某一批用户的timeline
    WBOPT_GET_STATUSES_REPOST_TIMELINE,//返回一条微博的最新n条转发微博信息
    WBOPT_GET_STATUSES_REPOST_BYME,//返回用户转发的最新n条微博信息
    WBOPT_GET_STATUSES_MENTIONS,//获取@当前用户的微博列表 
    WBOPT_GET_STATUSES_SHOW,//根据ID获取单条微博信息
    WBOPT_GET_STATUSES_SHOW_BATCH,//批量获取微博信息
    WBOPT_GET_STATUSES_QUERYMID,//通过id获取mid
    WBOPT_GET_STATUSES_QUERYID,//通过mid获取id
    WBOPT_GET_STATUSES_REPOST_DAILY,//按天返回热门转发榜
    WBOPT_GET_STATUSES_REPOST_WEEKLY,//按周返回热门转发榜
    WBOPT_GET_STATUSES_HOT_COMMENTS_DAILY,//按天返回热门评论榜
    WBOPT_GET_STATUSES_HOT_COMMENTS_WEEKLY,//按周返回热门评论榜
    WBOPT_POST_STATUSES_REPOST,//转发一条微博信息
    WBOPT_POST_STATUSES_DESTROY,//删除微博评论信息
    WBOPT_POST_STATUSES_UPDATE,//发布一条微博信息
    WBOPT_POST_STATUSES_UPLOAD_URL_TEXT,//发布一条微博，同时指定上传的图片或图片url
    WBOPT_POST_STATUSES_UPLOAD,//发布一条微博，上传图片并发布一条微博信息

    WBOPT_GET_COMMENTS_SHOW,//获取某条微博的评论列表
    WBOPT_GET_COMMENTS_BYME,//我发出的评论列表
    WBOPT_GET_COMMENTS_TOME,//我收到的评论列表
    WBOPT_GET_COMMENTS_TIMELINE,//获取当前用户发送及收到的评论列表
    WBOPT_GET_COMMENTS_MENTIONS,//@到我的评论
    WBOPT_GET_COMMENTS_SHOWBATCH,//@批量获取评论内容

    WBOPT_POST_COMMENTS_CREATE,//评论一条微博
    WBOPT_POST_COMMENTS_DESTROY,//删除一条评论
    WBOPT_POST_COMMENTS_DESTROY_BATCH ,//批量删除评论
    WBOPT_POST_COMMENTS_REPLY,//回复一条评论

    WBOPT_GET_DIRECTMESSAGES,//获取当前用户最新私信列表
    WBOPT_GET_DIRECTMESSAGES_SENT,//获取当前用户发送的最新私信列表
    WBOPT_GET_DIRECTMESSAGES_USER_LIST,//获取私信往来用户列表
    WBOPT_GET_DIRECTMESSAGES_CONVERSATION,//获取与指定用户的往来私信列表
    WBOPT_GET_DIRECTMESSAGES_SHOW_BATCH,//批量获取私信内容
    WBOPT_GET_DIRECTMESSAGES_IS_CAPABLE,//判断是否可以给对方发私信
    WBOPT_POST_DIRECTMESSAGES_NEW,//发送一条私信
    WBOPT_POST_DIRECTMESSAGES_DESTORY,//删除一条私信
    WBOPT_POST_DIRECTMESSAGES_DESTORY_BATCH,//批量删除私信

    WBOPT_GET_USERS_SHOW,//获取用户资料
    WBOPT_GET_USERS_DOMAIN_SHOW,//通过个性域名获取用户信息
    WBOPT_GET_USERS_SHOW_BATCH,//批量获取用户信息

    WBOPT_GET_ACCOUNT_PROFILE_BASIC, //获取用户基本信息
    WBOPT_GET_ACCOUNT_PROFILE_EDUCATION, //获取教育信息
    WBOPT_GET_ACCOUNT_PROFILE_EDUCATION_BATCH, //批量获取教育信息
    WBOPT_GET_ACCOUNT_PROFILE_CAREER, //获取职业信息
    WBOPT_GET_ACCOUNT_PROFILE_CAREER_BATCH, //批量获取职业信息
    WBOPT_GET_ACCOUNT_GET_PRIVACY, //获取隐私设置信息
    WBOPT_GET_ACCOUNT_PROFILE_SCHOOL_LIST, //获取所有学校列表
    WBOPT_GET_ACCOUNT_RATE_LIMIT_STATUS, //获取当前用户API访问频率限制
    WBOPT_GET_ACCOUNT_GET_UID, //OAuth授权之后获取用户UID（作用相当于旧版接口的account/verify_credentials）
    WBOPT_POST_ACCOUNT_PROFILE_BASIC_UPDATE, // 更新用户基本信息
    WBOPT_POST_ACCOUNT_PROFILE_EDU_UPDATE, // 更新用户教育信息
    WBOPT_POST_ACCOUNT_PROFILE_EDU_DESTROY, // 删除用户教育信息
    WBOPT_POST_ACCOUNT_PROFILE_CAR_UPDATE, // 更新用户职业信息
    WBOPT_POST_ACCOUNT_PROFILE_CAR_DESTROY, // 删除用户职业信息
    WBOPT_POST_ACCOUNT_AVATAR_UPLOAD, // 上传头像
    WBOPT_POST_ACCOUNT_UPDATE_PRIVACY, // 更新隐私设置

    WBOPT_GET_FRIENDSHIPS_FRIENDS,//获取用户的关注列表
    WBOPT_GET_FRIENDSHIPS_IN_COMMON,//获取共同关注人列表接口
    WBOPT_GET_FRIENDSHIPS_BILATERAL,//获取双向关注列表
    WBOPT_GET_FRIENDSHIPS_BILATERAL_IDS,//获取双向关注ID列表
    WBOPT_GET_FRIENDSHIPS_FRIENDS_IDS,//获取用户关注对象uid列表
    WBOPT_GET_FRIENDSHIPS_FRIENDS_REMARK_BATCH,//批量获取备注
    WBOPT_GET_FRIENDSHIPS_FRIENDS_FOLLOWERS,//获取用户粉丝列表及每个粉丝的最新一条微博
    WBOPT_GET_FRIENDSHIPS_FRIENDS_FOLLOWERS_IDS,//获取用户粉丝对象uid列表
    WBOPT_GET_FRIENDSHIPS_FRIENDS_FOLLOWERS_ACTIVE,//获取用户优质粉丝列表关系链
    WBOPT_GET_FRIENDSHIPS_CHAIN_FOLLOWERS,//获取我的关注人中关注了指定用户的分组
    WBOPT_GET_FRIENDSHIPS_SHOW,//获取两个用户关系的详细情况
    WBOPT_POST_FRIENDSHIPS_CREATE,//关注某用户
    WBOPT_POST_FRIENDSHIPS_CREATE_BATCH,//批量关注用户
    WBOPT_POST_FRIENDSHIPS_DESTROY,//取消关注某用户 
    WBOPT_POST_FRIENDSHIPS_FOLLOWERS_DESTORY,//移除粉丝
    WBOPT_POST_FRIENDSHIPS_REMARK_UPDATE,//更新关注人备注

    WBOPT_GET_FAVORITES,//获取当前登录用户的收藏列表
    WBOPT_GET_FAVORITES_SHOW,//获取单条收藏信息
    WBOPT_GET_FAVORITES_BY_TAGS,//获取当前用户某个标签下的收藏列表
    WBOPT_GET_FAVORITES_TAGS,//当前登录用户的收藏标签列表
    WBOPT_POST_FAVORITES_CREATE,//添加收藏
    WBOPT_POST_FAVORITES_DESTROY,//删除收藏
    WBOPT_POST_FAVORITES_DESTROY_BATCH,//批量删除收藏
    WBOPT_POST_FAVORITES_TAGS_UPDATE,//更新收藏标签
    WBOPT_POST_FAVORITES_TAGS_UPDATE_BATCH,//更新当前用户所有收藏下的指定标签
    WBOPT_POST_FAVORITES_TAGS_DESTROY_BATCH,//删除当前用户所有收藏下的指定标签

    WBOPT_GET_TRENDS,//获取某人话题
    WBOPT_GET_TRENDS_STATUSES,//获取某一话题下的微博
    WBOPT_GET_TRENDS_HOURLY,//返回最近一小时内的热门话题
    WBOPT_GET_TRENDS_DAILY,//返回最近一天内的热门话题
    WBOPT_GET_TRENDS_WEEKLY,//返回最近一周内的热门话题
    WBOPT_POST_TRENDS_FOLLOW,//关注某话题
    WBOPT_POST_TRENDS_DESTROY,//取消关注的某一个话题

    WBOPT_GET_TAGS,//返回指定用户的标签列表
    WBOPT_GET_TAGS_TAGS_BATCH,//批量获取用户标签
    WBOPT_GET_TAGS_SUGGESTIONS,//返回系统推荐的标签列表
    WBOPT_POST_TAGS_CREATE,//添加用户标签
    WBOPT_POST_TAGS_DESTROY,//删除用户标签
    WBOPT_POST_TAGS_DESTROY_BATCH,//批量删除用户标签

    WBOPT_GET_SEARCH_SUGGESTIONS_USERS,//搜用户搜索建议
    WBOPT_GET_SEARCH_SUGGESTIONS_STATUSES,//搜微博搜索建议
    WBOPT_GET_SEARCH_SUGGESTIONS_SCHOOLS,//搜学校搜索建议
    WBOPT_GET_SEARCH_SUGGESTIONS_COMPANIES, //搜公司搜索建议
    WBOPT_GET_SEARCH_SUGGESTIONS_APPS,//搜应用搜索建议
    WBOPT_GET_SEARCH_SUGGESTIONS_AT_USERS, //@联想搜索
    WBOPT_GET_SEARCH_SUGGESTIONS_INTEGRATE,  //综合联想搜索
    WBOPT_GET_SEARCH_STATUSES,//微博搜索 
    WBOPT_GET_SEARCH_GEO_STATUSES, //搜索指定范围内的微博
    WBOPT_GET_SEARCH_USERS,//搜索用户

    WBOPT_GET_SUGGESTIONS_FAVORITES_HOT,//热门收藏
    WBOPT_GET_SUGGESTIONS_USERS_HOT,//获取系统推荐用户
    WBOPT_GET_SUGGESTIONS_USERS_MAY_INTERESTED,//获取用户可能感兴趣的人
    WBOPT_POST_SUGGESTIONS_USERS_NOT_INTERESTED,//不感兴趣的人
    WBOPT_GET_SUGGESTIONS_USERS_BY_STATUS,//根据微博内容推荐用户

    WBOPT_GET_SHORT_URL_SHORTEN, //将一个或多个长链接转换成短链接
    WBOPT_GET_SHORT_URL_EXPAND, //将一个或多个短链接还原成原始的长链接
    WBOPT_GET_SHORT_URL_SHARE_COUNTS, //取得一个短链接在微博上的微博分享数（包含原创和转发的微博）
    WBOPT_GET_SHORT_URL_SHARE_STATUSES, //取得包含指定单个短链接的最新微博内容
    WBOPT_GET_SHORT_URL_COMMENT_COUNTS, //取得一个短链接在微博上的微博评论数
    WBOPT_GET_SHORT_URL_COMMENT_COMMENTS, //取得包含指定单个短链接的最新微博评论内容
    WBOPT_GET_SHORT_URL_INFO,

    WBOPT_GET_REMIND_UNREAD_COUNT,//获取某个用户的各种消息未读数
    WBOPT_POST_STATUSES_RESET_COUNT,//设置消息未读数

    // Groups
    WBOPT_GET_GROUPS_JOINED,
    WBOPT_GET_GROUPS_SHOW_BATCH,
    WBOPT_GET_GROUPS_GET_ANNOUNCEMENT,
    WBOPT_GET_GROUPS_ADMIN,
    WBOPT_GET_GROUPS_USERS,
    WBOPT_GET_GROUPS_SUGGESTION_MAY_INTERESTED,
    WBOPT_GET_GROUPS_CHAT_BLOCK,
    WBOPT_GET_GROUPS_CHAT_UNBLOCK,
    WBOPT_GET_GROUPS_CHAT_IS_BLOCKED,
    WBOPT_GET_GROUP_USERS_JOINED_STATUSES,

    WBOPT_END,
};

class IWeiboMethod : public EventSet,  public EventListener
{
public:
    virtual eWeiboResultCode Authorize() = 0;
    virtual eWeiboResultCode Oauth2(const char* userName, const char* password) = 0;
    virtual eWeiboResultCode Oauth2Code(const char* authCode, const char* url) = 0;
    virtual eWeiboResultCode EndSession() = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 微博读取
    virtual eWeiboResultCode GetStatusesPublicTimeline() = 0;
    virtual eWeiboResultCode GetStatusesFriendTimeline() = 0;
    virtual eWeiboResultCode GetStatusesHomeTimeline() = 0;
    virtual eWeiboResultCode GetStatusesUserTimeline(ID &usrId) = 0;
    virtual eWeiboResultCode GetStatusesTimelineBatch(ID &usrId) = 0;
    virtual eWeiboResultCode GetStatusesRePostTimeline(const char* id) = 0;
    virtual eWeiboResultCode GetStatusesRePostByMe() = 0;
    virtual eWeiboResultCode GetStatusesMentions() = 0;
    virtual eWeiboResultCode GetStatusesShow(const char* id) = 0;
    virtual eWeiboResultCode GetStatusesShowBatch(const char* inputIDs) = 0;
    virtual eWeiboResultCode GetStatusesQueryMID(const char *weiboId, const int type, const int is_batch = 0) = 0;
    virtual eWeiboResultCode GetStatusesQueryID(const char *mId, const int type, const int is_batch = 0, const int inbox = 0, const int isBase62 = 0) = 0;
    virtual eWeiboResultCode GetStatusesRePostDaily() = 0;
    virtual eWeiboResultCode GetStatusesRePostWeekly() = 0;
    virtual eWeiboResultCode GetStatusesHotCommmentsDaily() = 0;
    virtual eWeiboResultCode GetStatusesHotCommmentsWeekly() = 0;

    // 微博写入
    virtual eWeiboResultCode PostStatusesRePost(const char *weiboId, const char *statusText, const int isComment) = 0;
    virtual eWeiboResultCode PostStatusesDestroy(const char *weiboId) = 0;
    virtual eWeiboResultCode PostStatusesUpdate(const char* statusText) = 0;
    virtual eWeiboResultCode PostStatusesUploadUrlText(const char* statusText, const char *picInfo, bool isPicUrl) = 0;
    virtual eWeiboResultCode PostStatusesUpload(const char* statusText, const char *filePath) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 评论读取
    virtual eWeiboResultCode GetCommentsShow(const char *weiboId) = 0;
    virtual eWeiboResultCode GetCommentsByMe() = 0;
    virtual eWeiboResultCode GetCommentsToMe() = 0;
    virtual eWeiboResultCode GetCommentsTimeline() = 0;
    virtual eWeiboResultCode GetCommentsMentions() = 0;

    // 评论写入
    virtual eWeiboResultCode GetCommentsShowBatch(const char* commentIds) = 0;
    virtual eWeiboResultCode PostCommentsCreate(const char* id, const char* comment, const int commentOri) = 0;
    virtual eWeiboResultCode PostCommentsDestroy(const char* id) = 0;
    virtual eWeiboResultCode PostCommentsDestroyBatch(const char* id) = 0;
    virtual eWeiboResultCode PostCommentsReply(const char* commentId, const char* comment, const char* weiboId
            , const int withoutMention, const int commentOri) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 私信读取
    virtual eWeiboResultCode GetDirectMessages() = 0;
    virtual eWeiboResultCode GetDirectMessagesSent() = 0;
    virtual eWeiboResultCode GetDirectMessagesUserList() = 0;
    virtual eWeiboResultCode GetDirectMessagesCoversation(ID &usrId) = 0;
    virtual eWeiboResultCode GetDirectMessagesShowBatch(const char* id) = 0;
    virtual eWeiboResultCode GetDirectMessagesIsCapable(const char* uid) = 0;

    // 私信写入
    virtual eWeiboResultCode PostDirectMessagesNew(const char* text, const ID& uid, const char* fields = NULL,
            const char* weiboId = NULL) = 0;
    virtual eWeiboResultCode PostDirectMessagesDestory(const char* id) = 0;
    virtual eWeiboResultCode PostDirectMessagesDestoryBatch(const char* ids, const char* uid) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 用户读取
    virtual eWeiboResultCode GetUsersShow(ID& usrId, const char* extend = NULL) = 0;
    virtual eWeiboResultCode GetUsersDomainShow(const char* domain = NULL, const char* extend = NULL) = 0;
    virtual eWeiboResultCode GetUsersShowBatch(ID& usrId, const char* extend = NULL) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 关注读取
    virtual eWeiboResultCode GetFriendshipsFriends(ID& usrId, const int order/*= 0*/) = 0 ;
    virtual eWeiboResultCode GetFriendshipsInCommon(const char* id, const char* suid) = 0 ;
    virtual eWeiboResultCode GetFriendshipsBilateral(const char* uid) = 0 ;
    virtual eWeiboResultCode GetFriendshipsBilateralIDs(const char* uid) = 0 ;
    virtual eWeiboResultCode GetFriendshipsFriendIDs(ID& usrId) = 0 ;
    virtual eWeiboResultCode GetFriendshipsFriendRemarkBatch(const char* uids) = 0 ;

    // 粉丝读取
    virtual eWeiboResultCode GetFriendshipsFriendsFollowers(ID &usrId) = 0 ;
    virtual eWeiboResultCode GetFriendshipsFriendsFollowersIDs(ID &usrId) = 0 ;
    virtual eWeiboResultCode GetFriendshipsFriendsFollowersActive(ID &usrIDs) = 0 ;

    // 关系链读取
    virtual eWeiboResultCode GetFriendshipsFriendsChainFollowers(const char* uid) = 0 ;

    // 关系状态
    virtual eWeiboResultCode GetFriendshipShow(ID& sourceId, ID& tarGetId) = 0 ;

    // 关系写入
    virtual eWeiboResultCode PostFriendshipsCreate(ID &usrId, const int skipCheck) = 0 ;
    virtual eWeiboResultCode PostFriendshipsCreateBatch(const char* uids) = 0 ;
    virtual eWeiboResultCode PostFriendshipsDestroy(ID &usrId) = 0 ;
    virtual eWeiboResultCode PostFriendshipsFollowersDestroy(const char* uid) = 0 ;
    virtual eWeiboResultCode PostFriendshipsRemarkUpdate(const char* uid, const char* remark) = 0 ;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 帐号读取
    virtual eWeiboResultCode GetAccountProfileBasic(const char *uid) = 0 ;
    virtual eWeiboResultCode GetAccountProfileEducation(const char* uid) = 0 ;
    virtual eWeiboResultCode GetAccountProfileEducationBatch(const char* uids) = 0 ;
    virtual eWeiboResultCode GetAccountProfileCareer(const char* uid) = 0 ;
    virtual eWeiboResultCode GetAccountProfileCareerBatch(const char* uids) = 0 ;
    virtual eWeiboResultCode GetAccountGetPrivacy() = 0 ;
    virtual eWeiboResultCode GetAccountProfileSchoolList(const int province = 0, const int city = 0, const int area = 0, const int type = 0, 
            const char* capital = 0, const char* keyword = 0, const int counts = 0) = 0 ;
    virtual eWeiboResultCode GetAccountRateLimitStatus() = 0 ;
    virtual eWeiboResultCode GetAccountGetUID() = 0 ;

    // 帐号写入
    virtual eWeiboResultCode PostAccountProfileBasicUpdate(const BasicInfo& basic) = 0 ;
    virtual eWeiboResultCode PostAccountProfileEduUpdate(const EducationInfo& edu) = 0 ;
    virtual eWeiboResultCode PostAccountProfileEduDestroy(const char* id) = 0 ;
    virtual eWeiboResultCode PostAccountProfileCarUpdate(const CareerInfo& career) = 0 ;
    virtual eWeiboResultCode PostAccountProfileCarDestroy(const char* id) = 0 ;
    virtual eWeiboResultCode PostAccountAvatarUpload(const char* filePath) = 0 ;
    virtual eWeiboResultCode PostAccountUpdatePrivacy(const int comment = 0, const int geo = 0, const int message = 0,
            const int realname = 0, const int badge = 0, const int mobile = 0) = 0 ;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 收藏读取
    virtual eWeiboResultCode GetFavorites() = 0;
    virtual eWeiboResultCode GetFavoritesShow(const char* id) = 0;
    virtual eWeiboResultCode GetFavoritesByTags(const char* tid) = 0;
    virtual eWeiboResultCode GetFavoritesTags() = 0;

    // 收藏写入
    virtual eWeiboResultCode PostFavoritesCreate(const char* id) = 0;
    virtual eWeiboResultCode PostFavoritesDestroy(const char* id) = 0;
    virtual eWeiboResultCode PostFavoritesDestroyBatch(const char* ids) = 0;
    virtual eWeiboResultCode PostFavoritesTagUpdate(const char* tagId, const char* tagName) = 0;
    virtual eWeiboResultCode PostFavoritesTagsUpdateBatch(const char* tagId, const char* tagName) = 0;
    virtual eWeiboResultCode PostFavoritesTagsDestroyBatch(const char* tagId) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 话题读取
    virtual eWeiboResultCode GetTrends(const char* uid) = 0;
    virtual eWeiboResultCode GetTrendsStatuses(const char* trendWords, const char* provinceId) = 0;
    virtual eWeiboResultCode GetTrendsHourly() = 0;
    virtual eWeiboResultCode GetTrendsDaily() = 0;
    virtual eWeiboResultCode GetTrendsWeekly() = 0;

    // 话题写入
    virtual eWeiboResultCode PostTrendsFollow(const char* trendName) = 0;
    virtual eWeiboResultCode PostTrendsDestroy(const char* trendId) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 标签读取
    virtual eWeiboResultCode GetTags(const char* uid) = 0;
    virtual eWeiboResultCode GetTagsTagsBatch(const char* uids) = 0;
    virtual eWeiboResultCode GetTagsSuggestions() = 0;

    // 标签写入
    virtual eWeiboResultCode PostTagsCreate(const char* tags) = 0;
    virtual eWeiboResultCode PostTagsDestroy(const char* tagId) = 0;
    virtual eWeiboResultCode PostTagsDestroyBatch(const char* tagIds) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 搜索
    virtual eWeiboResultCode GetSearchSuggestionsUsers(const char* q, const int count = 0) = 0;
    virtual eWeiboResultCode GetSearchSuggestionsStatues(const char* q, const int count = 0) = 0;
    virtual eWeiboResultCode GetSearchSuggestionsSchool(const char* q, const int count = 0) = 0;
    virtual eWeiboResultCode GetSearchSuggestionsCompanies(const char* q, const int count = 0) = 0;
    virtual eWeiboResultCode GetSearchSuggestionsApps(const char* q, const int count = 0) = 0;
    virtual eWeiboResultCode GetSearchSuggestionsAtUsers(const char* q, const int type, const int count = 0, const int range = 0) = 0;
    virtual eWeiboResultCode GetSearchSuggestionsIntegrate(const char* query, const int sort_user = 0, const int sort_app = 0,
            const int sort_grp = 0, const int user_count = 0, const int app_count = 0, const int grp_count = 0) = 0;
    virtual eWeiboResultCode GetSearchStatuses(const char* q, const int filter_ori = 0, const int filter_pic = 0, const char* fuid = 0, 
            const int province = 0, const int city = 0, const char* starttime = NULL, const char* endtime = NULL, const int count = 0, 
            const int page = 0, const bool needcount = 0, const int base_app = 0) = 0;
    virtual eWeiboResultCode GetSearchGeoStatuses(const float longitude, const float latitudeconst, const int range = 0, const int time = 0,
            const int sort_type = 0, const int page = 0, const int count = 0, const int base_app = 0) = 0;
    virtual eWeiboResultCode GetSearchUsers(const char* q, const int snick = 0, const int sdomain = 0, const int sintro = 0, const int stag = 0, 
            const int province = 0, const int city = 0, const char* gender = 0, const char* comorsch = 0, const int sort = 0,
            const int count = 0, const int page = 0, const int base_app = 0) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 推荐读取
    virtual eWeiboResultCode GetSuggestionsFavoritesHot() = 0;
    virtual eWeiboResultCode GetSuggestionsUsersHot(const char* category) = 0;
    virtual eWeiboResultCode GetSuggestionsUsersMayInterested() = 0;
    virtual eWeiboResultCode GetSuggestionsUsersByStatus(const char* content, const int num = 0, const char* url = NULL) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 推荐写入
    virtual eWeiboResultCode PostSuggestionsUsersNotInterested(const char* uid) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 短链接口
    virtual eWeiboResultCode GetShortURLShorten(const char* url_long) = 0;
    virtual eWeiboResultCode GetShortURLExpand(const char* url_short) = 0;
    virtual eWeiboResultCode GetShortURLShareCounts(const char* url_short) = 0;
    virtual eWeiboResultCode GetShortURLShareStatuses(const char* url_short) = 0;
    virtual eWeiboResultCode GetShortURLCommentCounts(const char* url_short) = 0;
    virtual eWeiboResultCode GetShortURLCommentComments(const char* url_short) = 0;
    virtual eWeiboResultCode GetShortUrlInfo(const char* shortUrlIDs) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Unread
    virtual eWeiboResultCode GetRemindUnreadCount(const char* uid) = 0;
    virtual eWeiboResultCode PostStatusesResetCount(const int type) = 0;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Groups
    virtual eWeiboResultCode GetGroupsJoined(const char* uid, const int page, const int count, 
            const int type, const int filter, const int sort, const int member_limit, const int simplify) = 0;
    virtual eWeiboResultCode GetGroupsShowBatch(const char* gids) = 0;
    virtual eWeiboResultCode GetGroupsGetAnnouncement(const char* gid) = 0;
    virtual eWeiboResultCode GetGroupsAdmin(const char* gid) = 0;
    virtual eWeiboResultCode GetGroupsUsers(const char* gid) = 0;
    virtual eWeiboResultCode GetGroupsSuggestionMayInterested(const int count) = 0;
    virtual eWeiboResultCode GetGroupsChatIsBlocked(const char* gids) = 0;
    virtual eWeiboResultCode PostGroupsChatBlock(const char* gid) = 0;
    virtual eWeiboResultCode PostGroupsChatUnblock(const char* gid) = 0;
    virtual eWeiboResultCode GetGroupsUsersJoinedStatuses(const char* uid,int simplify) = 0;
};
}//weibo
}//dk
#endif//IWEIBOMETHOD_H
