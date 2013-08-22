/*
 * =====================================================================================
 *       Filename:  ParsingDataStruct.h
 *    Description:  微博返回数据结构
 *
 *        Version:  1.0
 *        Created:  12/28/2012 09:37:32 AM
 * =====================================================================================
 */

#ifndef PARSINGDATASTRUCT_H
#define PARSINGDATASTRUCT_H

#include <string>
#include <tr1/memory>
using std::string;

class JsonObject;

namespace dk
{
namespace weibo
{
class ParsingBase;
class ParsingOauthRet;
class ParsingApiError;
class ParsingGeo;
class ParsingUser;
class ParsingStatus;
typedef std::tr1::shared_ptr<ParsingBase> ParsingBaseSPtr;
typedef std::tr1::shared_ptr<ParsingOauthRet> ParsingOauthRetSPtr;
typedef std::tr1::shared_ptr<ParsingApiError> ParsingApiErrorSPtr;
typedef std::tr1::shared_ptr<ParsingGeo> ParsingGeoSPtr;
typedef std::tr1::shared_ptr<ParsingUser> ParsingUserSPtr;
typedef std::tr1::shared_ptr<ParsingStatus> ParsingStatusSPtr;

class ParsingBase
{
public:
    virtual ~ParsingBase() {}
};//ParsingBase

class ParsingOauthRet : public ParsingBase
{
public:
    static ParsingOauthRet* CreateOautRet(const JsonObject* jsonObj);
    string GetAccessToken() const { return m_accessToken; }
    int GetExpiresIn() const { return m_expiresIn; }
    string GetUid() const { return m_uid; }

private:
    bool Init(const JsonObject* jsonObj);
private:
    string m_accessToken;
    int m_expiresIn;
    string m_remindIn;//obsolete, use expiresIn instead
    string m_uid;
};//ParsingOauthRet

class ParsingApiError : public ParsingBase
{
public:
    ParsingApiError()
        : m_errorCode(0)
    {}

    static ParsingApiError* CreateApiError(const JsonObject* jsonObj);
    int GetErrorCode() const { return m_errorCode; }
    string GetRequest() const { return m_request; }
    string GetError() const { return m_error; }
    string GetErrorUri() const { return m_errorUri; }
    string GetErrorDescription() const { return m_errorDescription; }
    string GetErrorMessage() const { return m_errorMessage; }

private:
    bool Init(const JsonObject* jsonObj);
private:
    int m_errorCode;
    string m_request;
    string m_error;
    string m_errorUri;
    string m_errorDescription;
    string m_errorMessage;
};//ParsingApiError

class ParsingUser :  public ParsingBase
{
public:
    ParsingUser()
        : m_followersCount(0)
        , m_friendsCount(0)
        , m_statusesCount(0)
        , m_favouritesCount(0)
        , m_following(false)
        , m_geoEnabled(false)
        , m_verified(false)
        , m_verifiedType(-1)
        , m_allowAllActMsg(false)
        , m_allowAllComment(false)
        , m_followMe(false)
        , m_onlineStatus(-1)
        , m_biFollowersCount(0)
    {}

    static ParsingUser* CreateUser(const JsonObject* jsonObj);

    string GetScreenName() const { return m_screenName; }

private:
    bool Init(const JsonObject* jsonObj);
    string m_id;///<用户UID
    string m_screenName;///< 微博昵称
    string m_name;///< 友好显示名称，如Bill Gates,名称中间的空格正常显示(此特性暂不支持)
    string m_province;///< 省份编码（参考省份编码表）
    string m_city;///< 城市编码（参考城市编码表）
    string m_location;///<地址
    string m_description;///< 个人描述
    string m_url;///< 用户博客地址
    string m_profileImageUrl;///< 自定义图像
    string m_profileUrl;//用户微博地址
    string m_domain;///< 用户个性化URL
    string m_gender;///< 性别,m--男，f--女,n--未知
    int m_followersCount;///< 粉丝数
    int m_friendsCount;///< 关注数
    int m_statusesCount;///< 微博数
    int m_favouritesCount;///< 收藏数
    string m_createdAt;///< 创建时间
    bool m_following;///< 是否已关注
    bool m_geoEnabled;
    bool m_verified;///< 加V标示，是否微博认证用户
    int m_verifiedType;///<认证类型
    bool m_allowAllActMsg;///<是否允许所有人给我发私信
    bool m_allowAllComment;///<是否允许所有人对我的微博进行评论
    bool m_followMe;///<此用户是否关注我
    string m_avatarLarge;///<大头像地址
    string m_verifiedReason;///<验证原因
    int m_onlineStatus;///<用户在线状态
    int m_biFollowersCount;///<互粉数
    string m_remark;///<备注信息，在查询用户关系时提供此字段。
    string m_lang;///<用户语言版本
};//ParsingUser

class ParsingGeo : public ParsingBase
{

};

class ParsingStatus : public ParsingBase
{
public:
    ParsingStatus()
        : m_id(-1)
        , m_repostsCount(-1)
        , m_commentsCount(-1)
    {}

    static ParsingStatus* CreateStatus(const JsonObject* jsonObj);

private:
    bool Init(const JsonObject* jsonObj);
    string m_createdAt; ///< 创建时间
    int m_id; ///< 微博ID
    string m_text; ///<微博信息内容
    string m_source; ///< 微博来源
    bool m_favorited; ///< 是否已收藏
    bool m_truncated;//是否截断
    string m_inReplyToStatusId; ///< 回复ID
    string m_inReplyToUserId; ///< 回复人UID
    string m_inReplyToScreenName; ///< 回复人昵称
    string m_thumbnailPic; ///< 缩略图，没有时不返回此字段
    string m_bmiddlePic; ///<中型图片，没有时不返回此字段
    string m_originalPic; ///<原始图片，没有时不返回此字段
    int m_repostsCount; ///<转发数
    int m_commentsCount; ///<评论数
    string m_mid; ///<微博MID
    string m_annotations; ///<元数据，没有时不返回此字段

    ParsingGeoSPtr m_geo; ///<地理信息，保存经纬度，没有时不返回此字段
    ParsingUserSPtr m_user; ///< 作者信息
    ParsingStatusSPtr m_retweetedStatus; ///<转发的博文，内容为status，如果不是转发，则没有此字段
};
}//weibo
}//dk
#endif//PARSINGDATASTRUCT_H





















