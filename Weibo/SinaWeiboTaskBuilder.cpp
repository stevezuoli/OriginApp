#include <map>
#include "Weibo/SinaWeiboTaskBuilder.h"
#include "Weibo/IWeibo.h"
#include "Weibo/IWeiboMethod.h"
#include "Weibo/SinaWeiboManager.h"
#include "Weibo/WeiboFactory.h"

namespace dk
{
namespace weibo
{
static const std::string s_authorize = "https://api.weibo.com/oauth2/authorize";
static const std::string s_accessToken = "https://api.weibo.com/oauth2/access_token";
static const std::string s_getTokenInfo = "http://api.weibo.com/oauth2/get_token_info";
static const std::string s_getOAuth2Token = "http://api.weibo.com/oauth2/get_oauth2_token";
static const std::string s_getUserShow = "https://api.weibo.com/2/users/show.json?access_token=%s&uid=%s";

static const struct tagMethodURLElement gMethodURLElements[] = 
{
    { WBOPT_NONE, "" },
    { WBOPT_GET_STATUSES_PUBLIC_TIMELINE, "/statuses/public_timeline", EHT_V4 }, //获取最新更新的公共微博消息 
    { WBOPT_GET_STATUSES_FRIENDS_TIMELINE, "/statuses/friends_timeline", EHT_V4 }, //获取当前用户所关注用户的最新微博信息 (别名: statuses/home_timeline) 
    { WBOPT_GET_STATUSES_HOME_TIMELINE, "/statuses/home_timeline", EHT_V4 }, //获取当前登录用户及其所关注用户的最新微博消息
    { WBOPT_GET_STATUSES_USER_TIMELINE, "/statuses/user_timeline", EHT_V4 }, //获取用户发布的微博信息列表 
    { WBOPT_GET_STATUSES_TIMELINE_BATCH, "/statuses/timeline_batch", EHT_V4 }, //批量获得指定的某一批用户的timeline
    { WBOPT_GET_STATUSES_REPOST_TIMELINE, "/statuses/repost_timeline", EHT_V4 }, //返回一条微博的最新n条转发微博信息
    { WBOPT_GET_STATUSES_REPOST_BYME, "/statuses/repost_by_me", EHT_V4 }, //返回用户转发的最新n条微博信息
    { WBOPT_GET_STATUSES_MENTIONS, "/statuses/mentions", EHT_V4 },//获取@当前用户的微博列表 
    { WBOPT_GET_STATUSES_SHOW, "/statuses/show", EHT_V4 },//根据ID获取单条微博信息
    { WBOPT_GET_STATUSES_SHOW_BATCH, "/statuses/show_batch", EHT_V4 },//批量获取微博信息
    { WBOPT_GET_STATUSES_QUERYMID, "/statuses/querymid", EHT_V4 },//通过id获取mid
    { WBOPT_GET_STATUSES_QUERYID, "/statuses/queryid", EHT_V4 },//通过mid获取id
    { WBOPT_GET_STATUSES_REPOST_DAILY, "/statuses/hot/repost_daily", EHT_V4 },//按天返回热门转发榜
    { WBOPT_GET_STATUSES_REPOST_WEEKLY, "/statuses/hot/repost_weekly", EHT_V4 },//按周返回热门转发榜
    { WBOPT_GET_STATUSES_HOT_COMMENTS_DAILY, "/statuses/hot/comments_daily", EHT_V4 },//按天返回热门评论榜
    { WBOPT_GET_STATUSES_HOT_COMMENTS_WEEKLY, "/statuses/hot/comments_weekly", EHT_V4 },//按周返回热门评论榜

    { WBOPT_POST_STATUSES_REPOST, "/statuses/repost", EHT_V4 },//转发一条微博信息
    { WBOPT_POST_STATUSES_DESTROY, "/statuses/destroy", EHT_V4 },//删除微博评论信息
    { WBOPT_POST_STATUSES_UPDATE, "/statuses/update", EHT_V4 },//发布一条微博信息
    { WBOPT_POST_STATUSES_UPLOAD_URL_TEXT, "/statuses/upload_url_text", EHT_V4 },//发布一条微博，同时指定上传的图片或图片url
    { WBOPT_POST_STATUSES_UPLOAD, "/statuses/upload", EHT_V4 },//发布一条微博，上传图片并发布一条微博信息

    { WBOPT_GET_COMMENTS_SHOW, "/comments/show", EHT_V4 }, //获取某条微博的评论列表
    { WBOPT_GET_COMMENTS_BYME, "/comments/by_me", EHT_V4 }, //我发出的评论列表
    { WBOPT_GET_COMMENTS_TOME, "/comments/to_me", EHT_V4 }, //我收到的评论列表
    { WBOPT_GET_COMMENTS_TIMELINE, "/comments/timeline", EHT_V4 }, //获取当前用户发送及收到的评论列表
    { WBOPT_GET_COMMENTS_MENTIONS, "/comments/mentions", EHT_V4 }, //@到我的评论
    { WBOPT_GET_COMMENTS_SHOWBATCH, "/comments/show_batch", EHT_V4 }, //@批量获取评论内容

    { WBOPT_POST_COMMENTS_CREATE, "/comments/create", EHT_V4 }, //评论一条微博
    { WBOPT_POST_COMMENTS_DESTROY, "/comments/destroy", EHT_V4 }, //删除一条评论
    { WBOPT_POST_COMMENTS_DESTROY_BATCH , "/comments/destroy_batch", EHT_V4 }, //批量删除评论
    { WBOPT_POST_COMMENTS_REPLY, "/comments/reply", EHT_V4 }, //回复一条评论

    { WBOPT_GET_DIRECTMESSAGES, "/direct_messages", EHT_V4 }, //获取当前用户最新私信列表
    { WBOPT_GET_DIRECTMESSAGES_SENT, "/direct_messages/sent", EHT_V4 }, //获取当前用户发送的最新私信列表
    { WBOPT_GET_DIRECTMESSAGES_USER_LIST, "/direct_messages/user_list", EHT_V4 }, //获取私信往来用户列表
    { WBOPT_GET_DIRECTMESSAGES_CONVERSATION, "/direct_messages/conversation", EHT_V4 }, //获取与指定用户的往来私信列表
    { WBOPT_GET_DIRECTMESSAGES_SHOW_BATCH,  "/direct_messages/show_batch", EHT_V4 }, //批量获取私信内容
    { WBOPT_GET_DIRECTMESSAGES_IS_CAPABLE,  "/direct_messages/is_capable", EHT_V4 }, //判断是否可以给对方发私信
    { WBOPT_POST_DIRECTMESSAGES_NEW, "/direct_messages/new", EHT_V4 }, //发送一条私信
    { WBOPT_POST_DIRECTMESSAGES_DESTORY, "/direct_messages/destroy", EHT_V4 }, //删除一条私信
    { WBOPT_POST_DIRECTMESSAGES_DESTORY_BATCH,"/direct_messages/destroy_batch", EHT_V4 },   //批量删除私信

    { WBOPT_GET_USERS_SHOW, "/users/show", EHT_V4 }, //获取用户资料
    { WBOPT_GET_USERS_DOMAIN_SHOW, "/users/domain_show", EHT_V4 }, //通过个性域名获取用户信息
    { WBOPT_GET_USERS_SHOW_BATCH, "/users/show_batch", EHT_V4 }, //批量获取用户信息

    { WBOPT_GET_ACCOUNT_PROFILE_BASIC, "/account/profile/basic", EHT_V4 }, //获取用户基本信息
    { WBOPT_GET_ACCOUNT_PROFILE_EDUCATION, "/account/profile/education", EHT_V4 }, //获取教育信息
    { WBOPT_GET_ACCOUNT_PROFILE_EDUCATION_BATCH, "/account/profile/education_batch", EHT_V4 }, //批量获取教育信息
    { WBOPT_GET_ACCOUNT_PROFILE_CAREER, "/account/profile/career", EHT_V4 }, //获取职业信息
    { WBOPT_GET_ACCOUNT_PROFILE_CAREER_BATCH, "/account/profile/career_batch", EHT_V4 }, //批量获取职业信息
    { WBOPT_GET_ACCOUNT_GET_PRIVACY, "/account/get_privacy", EHT_V4 }, //获取隐私设置信息
    { WBOPT_GET_ACCOUNT_PROFILE_SCHOOL_LIST, "/account/profile/school_list", EHT_V4 }, //获取所有学校列表
    { WBOPT_GET_ACCOUNT_RATE_LIMIT_STATUS, "/account/rate_limit_status", EHT_V4 }, //获取当前用户API访问频率限制
    { WBOPT_GET_ACCOUNT_GET_UID, "/account/get_uid", EHT_V4 }, //OAuth授权之后获取用户UID（作用相当于旧版接口的account/verify_credentials）

    { WBOPT_POST_ACCOUNT_PROFILE_BASIC_UPDATE , "/account/profile/basic_update", EHT_V4 }, // 更新用户基本信息
    { WBOPT_POST_ACCOUNT_PROFILE_EDU_UPDATE, "/account/profile/edu_update", EHT_V4 }, // 更新用户教育信息
    { WBOPT_POST_ACCOUNT_PROFILE_EDU_DESTROY , "/account/profile/edu_destroy", EHT_V4 }, // 删除用户教育信息
    { WBOPT_POST_ACCOUNT_PROFILE_CAR_UPDATE , "/account/profile/car_update", EHT_V4 }, // 更新用户职业信息
    { WBOPT_POST_ACCOUNT_PROFILE_CAR_DESTROY , "/account/profile/car_destroy", EHT_V4 }, // 删除用户职业信息
    { WBOPT_POST_ACCOUNT_AVATAR_UPLOAD , "/account/avatar/upload", EHT_V4 }, // 上传头像
    { WBOPT_POST_ACCOUNT_UPDATE_PRIVACY , "/account/update_privacy", EHT_V4 }, // 更新隐私设置

    { WBOPT_GET_FRIENDSHIPS_FRIENDS, "/friendships/friends", EHT_V4 }, //获取用户的关注列表
    { WBOPT_GET_FRIENDSHIPS_IN_COMMON, "/friendships/friends/in_common", EHT_V4 }, //获取共同关注人列表接口
    { WBOPT_GET_FRIENDSHIPS_BILATERAL, "/friendships/friends/bilateral", EHT_V4 }, //获取双向关注列表
    { WBOPT_GET_FRIENDSHIPS_BILATERAL_IDS, "/friendships/friends/bilateral/ids", EHT_V4 }, //获取双向关注ID列表
    { WBOPT_GET_FRIENDSHIPS_FRIENDS_IDS, "/friendships/friends/ids", EHT_V4 }, //获取用户关注对象uid列表
    { WBOPT_GET_FRIENDSHIPS_FRIENDS_REMARK_BATCH, "/friendships/friends/remark_batch"}, //批量获取备注
    { WBOPT_GET_FRIENDSHIPS_FRIENDS_FOLLOWERS, "/friendships/followers", EHT_V4 }, //获取用户粉丝列表及每个粉丝的最新一条微博
    { WBOPT_GET_FRIENDSHIPS_FRIENDS_FOLLOWERS_IDS, "/friendships/followers/ids", EHT_V4 }, //获取用户粉丝对象uid列表
    { WBOPT_GET_FRIENDSHIPS_FRIENDS_FOLLOWERS_ACTIVE,"/friendships/followers/active", EHT_V4 },     //获取用户优质粉丝列表关系链
    { WBOPT_GET_FRIENDSHIPS_CHAIN_FOLLOWERS, "/friendships/friends_chain/followers", EHT_V4 }, //获取我的关注人中关注了指定用户的分组
    { WBOPT_GET_FRIENDSHIPS_SHOW, "/friendships/show", EHT_V4 }, //获取两个用户关系的详细情况
    { WBOPT_POST_FRIENDSHIPS_CREATE, "/friendships/create", EHT_V4 }, //关注某用户
    { WBOPT_POST_FRIENDSHIPS_CREATE_BATCH, "/friendships/create_batch", EHT_V4 }, //批量关注用户
    { WBOPT_POST_FRIENDSHIPS_DESTROY, "/friendships/destroy", EHT_V4 }, //取消关注某用户    
    { WBOPT_POST_FRIENDSHIPS_FOLLOWERS_DESTORY,"/friendships/followers/destroy"}, //移除粉丝
    { WBOPT_POST_FRIENDSHIPS_REMARK_UPDATE, "/friendships/remark/update", EHT_V4 }, //更新关注人备注

    { WBOPT_GET_FAVORITES, "/favorites", EHT_V4 }, //获取当前登录用户的收藏列表
    { WBOPT_GET_FAVORITES_SHOW, "/favorites/show", EHT_V4 }, //获取单条收藏信息
    { WBOPT_GET_FAVORITES_BY_TAGS, "/favorites/by_tags", EHT_V4 }, //获取当前用户某个标签下的收藏列表
    { WBOPT_GET_FAVORITES_TAGS, "/favorites/tags", EHT_V4 }, //当前登录用户的收藏标签列表
    { WBOPT_POST_FAVORITES_CREATE, "/favorites/create", EHT_V4 }, //添加收藏
    { WBOPT_POST_FAVORITES_DESTROY, "/favorites/destroy", EHT_V4 }, //删除收藏
    { WBOPT_POST_FAVORITES_DESTROY_BATCH, "/favorites/destroy_batch", EHT_V4 }, //批量删除收藏
    { WBOPT_POST_FAVORITES_TAGS_UPDATE, "/favorites/tags/update", EHT_V4 }, //更新收藏标签
    { WBOPT_POST_FAVORITES_TAGS_UPDATE_BATCH, "/favorites/tags/update_batch", EHT_V4 }, //更新当前用户所有收藏下的指定标签
    { WBOPT_POST_FAVORITES_TAGS_DESTROY_BATCH, "/favorites/tags/destroy_batch", EHT_V4 }, //删除当前用户所有收藏下的指定标签

    { WBOPT_GET_TRENDS, "/trends", EHT_V4 }, //获取某人话题
    { WBOPT_GET_TRENDS_STATUSES, "/trends/statuses", EHT_V4 }, //获取某一话题下的微博
    { WBOPT_GET_TRENDS_HOURLY, "/trends/hourly", EHT_V4 }, //返回最近一小时内的热门话题
    { WBOPT_GET_TRENDS_DAILY, "/trends/daily", EHT_V4 }, //返回最近一天内的热门话题
    { WBOPT_GET_TRENDS_WEEKLY, "/trends/weekly", EHT_V4 }, //返回最近一周内的热门话题
    { WBOPT_POST_TRENDS_FOLLOW, "/trends/follow", EHT_V4 }, //关注某话题
    { WBOPT_POST_TRENDS_DESTROY, "/trends/destroy", EHT_V4 }, //取消关注的某一个话题

    { WBOPT_GET_TAGS, "/tags", EHT_V4 }, //返回指定用户的标签列表
    { WBOPT_GET_TAGS_TAGS_BATCH, "/tags/tags_batch", EHT_V4 }, //批量获取用户标签
    { WBOPT_GET_TAGS_SUGGESTIONS, "/tags/suggestions", EHT_V4 }, //返回系统推荐的标签列表
    { WBOPT_POST_TAGS_CREATE, "/tags/create", EHT_V4 }, //添加用户标签
    { WBOPT_POST_TAGS_DESTROY, "/tags/destroy", EHT_V4 }, //删除用户标签
    { WBOPT_POST_TAGS_DESTROY_BATCH, "/tags/destroy_batch", EHT_V4 }, //批量删除用户标签

    { WBOPT_GET_SEARCH_SUGGESTIONS_USERS, "/search/suggestions/users", EHT_V4 }, //搜用户搜索建议
    { WBOPT_GET_SEARCH_SUGGESTIONS_STATUSES, "/search/suggestions/statuses", EHT_V4 }, //搜微博搜索建议
    { WBOPT_GET_SEARCH_SUGGESTIONS_SCHOOLS, "/search/suggestions/schools", EHT_V4 }, //搜学校搜索建议
    { WBOPT_GET_SEARCH_SUGGESTIONS_COMPANIES, "/search/suggestions/companies", EHT_V4 }, //搜公司搜索建议
    { WBOPT_GET_SEARCH_SUGGESTIONS_APPS, "/search/suggestions/apps" },//搜应用搜索建议
    { WBOPT_GET_SEARCH_SUGGESTIONS_AT_USERS, "/search/suggestions/at_users", EHT_V4 }, //@联想搜索
    { WBOPT_GET_SEARCH_SUGGESTIONS_INTEGRATE, "/search/suggestions/integrate", EHT_V4 }, //综合联想搜索
    { WBOPT_GET_SEARCH_STATUSES, "/search/statuses", EHT_V4 }, //微博搜索 
    { WBOPT_GET_SEARCH_GEO_STATUSES, "/search/geo/statuses", EHT_V4 }, //搜索指定范围内的微博
    { WBOPT_GET_SEARCH_USERS, "/search/users", EHT_V4 }, //搜索用户

    { WBOPT_GET_SUGGESTIONS_FAVORITES_HOT, "/suggestions/favorites/hot", EHT_V4 }, //热门收藏
    { WBOPT_GET_SUGGESTIONS_USERS_HOT, "/suggestions/users/hot", EHT_V4 }, //获取系统推荐用户
    { WBOPT_GET_SUGGESTIONS_USERS_MAY_INTERESTED, "/suggestions/users/may_interested", EHT_V4 }, //获取用户可能感兴趣的人
    { WBOPT_POST_SUGGESTIONS_USERS_NOT_INTERESTED, "/suggestions/users/not_interested", EHT_V4 }, //不感兴趣的人
    { WBOPT_GET_SUGGESTIONS_USERS_BY_STATUS, "/suggestions/users/by_status", EHT_V4 }, //根据微博内容推荐用户

    { WBOPT_GET_SHORT_URL_SHORTEN, "/short_url/shorten", EHT_V3 }, //将一个或多个长链接转换成短链接
    { WBOPT_GET_SHORT_URL_EXPAND, "/short_url/expand", EHT_V3 }, //将一个或多个短链接还原成原始的长链接
    { WBOPT_GET_SHORT_URL_SHARE_COUNTS, "/short_url/share/counts", EHT_V3 }, //取得一个短链接在微博上的微博分享数（包含原创和转发的微博）
    { WBOPT_GET_SHORT_URL_SHARE_STATUSES, "/short_url/share/statuses", EHT_V3 }, //取得包含指定单个短链接的最新微博内容
    { WBOPT_GET_SHORT_URL_COMMENT_COUNTS, "/short_url/comment/counts", EHT_V3 }, //取得一个短链接在微博上的微博评论数
    { WBOPT_GET_SHORT_URL_COMMENT_COMMENTS, "/short_url/comment/comments", EHT_V3 }, //取得包含指定单个短链接的最新微博评论内容
    { WBOPT_GET_SHORT_URL_INFO, "/short_url/batch_info", EHT_V3 },

    // Unread
    { WBOPT_GET_REMIND_UNREAD_COUNT, "/remind/unread_count", EHT_V4}, //获取某个用户的各种消息未读数
    { WBOPT_POST_STATUSES_RESET_COUNT, "/statuses/reset_count", EHT_V3}, //type: 1.comments, 2. @me, 3.Direct Message 4. follower

    // Groups
    { WBOPT_GET_GROUPS_JOINED, "/groups/joined", EHT_V4 },
    { WBOPT_GET_GROUPS_SHOW_BATCH, "/groups/show_batch", EHT_V4 },
    { WBOPT_GET_GROUPS_GET_ANNOUNCEMENT, "/groups/get_announcement", EHT_V4 },
    { WBOPT_GET_GROUPS_ADMIN, "/groups/%s/admin", EHT_V3},
    { WBOPT_GET_GROUPS_USERS, "/groups/%s/users", EHT_V3 },
    { WBOPT_GET_GROUPS_SUGGESTION_MAY_INTERESTED, "/groups/suggestions/may_interested", EHT_V3 },
    { WBOPT_GET_GROUPS_CHAT_BLOCK, "/groups/chat/block", EHT_V4 },
    { WBOPT_GET_GROUPS_CHAT_UNBLOCK, "/groups/chat/unblock", EHT_V4 },
    { WBOPT_GET_GROUPS_CHAT_IS_BLOCKED, "/groups/chat/is_blocked", EHT_V4 },
    { WBOPT_GET_GROUP_USERS_JOINED_STATUSES, "/groups/statuses/joined_timeline", EHT_V4},
};


std::string SinaWeiboTaskBuilder::GetMethodBaseURL(const enHostType type)
{
    switch(type)
    {
    case EHT_V3:
        return "https://api.t.sina.com.cn";

    case EHT_V4: 
        return "https://api.weibo.com/2";

    default:
        {
            // Error: Undefined type!
            assert(false);
        }
        break;
    }
    return "";
}

void SinaWeiboTaskBuilder::GetMethodURLFromOption(unsigned int methodOption, std::string &baseURL, eWeiboRequestFormat format)
{
    static std::map<int, tagMethodURLElement> methodURLElements;
    if (methodURLElements.empty())
    {
        int counts = sizeof(gMethodURLElements)/sizeof(gMethodURLElements[0]);
        for (int i = 0; i < counts; i++)
        {
            const tagMethodURLElement& tempElem = gMethodURLElements[i];
            methodURLElements.insert(std::make_pair(tempElem.option, tempElem));
        }
    }

    std::map<int, tagMethodURLElement>::const_iterator it = methodURLElements.find(methodOption);
    assert(it != methodURLElements.end());
    if (it != methodURLElements.end())
    {
        const tagMethodURLElement& tempElem = it->second;
        baseURL = GetMethodBaseURL(tempElem.type);
        baseURL += tempElem.url;
    }

    if (format == WRF_JSON)
    {
        baseURL += ".json";
    }
    else if (format == WRF_XML)
    {
        baseURL += ".xml";
    }
}

const char* SinaWeiboTaskBuilder::GetConsumerKey()
{
    return WeiboFactory::GetSinaWeibo()->GetConsumerKey().c_str();
}

const char* SinaWeiboTaskBuilder::GetConsumerSecret()
{
    return WeiboFactory::GetSinaWeibo()->GetConsumerSecret().c_str();
}

const char* SinaWeiboTaskBuilder::GetAccessToken()
{
    return WeiboFactory::GetSinaWeibo()->GetAccessToken().c_str();
}

const char* SinaWeiboTaskBuilder::GetRedirectURL()
{
    return WeiboFactory::GetSinaWeibo()->GetRedirectURL().c_str();
}

DownloadTaskSPtr SinaWeiboTaskBuilder::BuildAuthorizeTask()
{
    DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    task->SetUrl(s_authorize.c_str());
    task->AddPostData("client_id", GetConsumerKey());
    task->AddPostData("redirect_uri", GetRedirectURL());
    task->AddPostData("response_type", "code");
    task->AddPostData("forcelogin", "true");

    return task;
}

DownloadTaskSPtr SinaWeiboTaskBuilder::BuildOauth2UsernamePwdTask(const char* userName, const char* password)
{
    DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    task->SetUrl(s_accessToken.c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData("client_id", GetConsumerKey());
    task->AddPostData("client_secret", GetConsumerSecret());
    task->AddPostData("grant_type", "password");
    task->AddPostData("username", userName);
    task->AddPostData("password", password);

    return task;
}

DownloadTaskSPtr SinaWeiboTaskBuilder::BuildOauth2CodeTask(const char* authCode, const char* url)
{
    DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    task->SetUrl(s_accessToken.c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData("client_id", GetConsumerKey());
    task->AddPostData("client_secret", GetConsumerSecret());
    task->AddPostData("grant_type", "authorization_code");
    task->AddPostData("code", authCode);
    task->AddPostData("redirect_uri", url);

    return task;
}

DownloadTaskSPtr SinaWeiboTaskBuilder::BuildGetTokenInfo()
{
    DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    task->SetUrl(s_getTokenInfo.c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData("access_token", GetAccessToken());

    return task;
}

DownloadTaskSPtr SinaWeiboTaskBuilder::BuildStatusUpdate(const char* status)
{
    DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    string url;
    GetMethodURLFromOption(WBOPT_POST_STATUSES_UPDATE, url);
    task->SetUrl(url.c_str());
    task->SetMethod(DownloadTask::POST);
    task->AddPostData("access_token", GetAccessToken());
    task->AddPostData("status", status);

    return task;
}

DownloadTaskSPtr SinaWeiboTaskBuilder::BuildStatusUpload(const char* status, const char* picPath)
{
    DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    string url;
    GetMethodURLFromOption(WBOPT_POST_STATUSES_UPLOAD, url);
    task->SetUrl(url.c_str());
    task->SetMethod(DownloadTask::POSTFORM);
    task->AddFormData("access_token", GetAccessToken());
    task->AddFormData("status", status);
    task->SetPostFile("pic", picPath);

    return task;
}

DownloadTaskSPtr SinaWeiboTaskBuilder::BuildGetUsersShow(const char* uid)
{
    DownloadTaskSPtr task = DownloadTaskSPtr(new DownloadTask());
    char buf[1024] = {0};
    snprintf(buf, DK_DIM(buf), s_getUserShow.c_str(), GetAccessToken(), uid);
    task->SetUrl(buf);
    return task;
}
}//weibo
}//dk
