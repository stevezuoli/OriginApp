#include "Weibo/ParsingDataStruct.h"
#include <string>
#include <map>
#include "Utility/JsonObject.h"
#include "I18n/StringManager.h"
#include "Weibo/IWeiboDef.h"

namespace dk
{
namespace weibo
{
std::string GetErrorMessageFromErrorCode(int errorCode)
{
    static std::map<int,std::string> errorMsgMap;
    if (errorMsgMap.size() == 0)
    {
        errorMsgMap.insert(std::make_pair(WREC_USERNAME_PASSWORD_MISMATCH, StringManager::GetPCSTRById(WEIBO_ERROR_USERNAME_PASSWORD_MISMATCH)));
        errorMsgMap.insert(std::make_pair(WREC_USERNAME_PASSWORD_EMPTY, StringManager::GetPCSTRById(WEIBO_ERROR_USERNAME_PASSWORD_MISMATCH)));
        errorMsgMap.insert(std::make_pair(WREC_REPEAT_CONTENT, StringManager::GetPCSTRById(WEIBO_ERROR_REPEAT_CONTENT)));
        errorMsgMap.insert(std::make_pair(WREC_LOGIN_EXPIRED, StringManager::GetPCSTRById(LOGIN_EXPIRED)));
    }

    std::map<int, std::string>::const_iterator it = errorMsgMap.find(errorCode);
    if (it != errorMsgMap.end())
    {
        return it->second;
    }
    else
    {
        return "";
    }
}
ParsingOauthRet* ParsingOauthRet::CreateOautRet(const JsonObject* jsonObj)
{
    ParsingOauthRet* pOauthRet = new ParsingOauthRet;
    if (pOauthRet && pOauthRet->Init(jsonObj))
    {
        return pOauthRet;
    }

    if (pOauthRet)
    {
        delete pOauthRet;
    }
    return NULL;
}

bool ParsingOauthRet::Init(const JsonObject* jsonObj)
{
    if (!jsonObj)
    {
        return false;
    }

    jsonObj->GetStringValue("access_token", &m_accessToken);
    jsonObj->GetIntValue("expires_in", &m_expiresIn);
    jsonObj->GetStringValue("remind_in", &m_remindIn);
    jsonObj->GetStringValue("uid", &m_uid);

    return true;
}

ParsingApiError* ParsingApiError::CreateApiError(const JsonObject* jsonObj)
{
    ParsingApiError* pApiError = new ParsingApiError;
    if (pApiError && pApiError->Init(jsonObj))
    {
        return pApiError;
    }
    if (pApiError)
    {
        delete pApiError;
    }

    return NULL;
}

bool ParsingApiError::Init(const JsonObject* jsonObj)
{
    if (!jsonObj)
    {
        return false;
    }

    jsonObj->GetStringValue("error", &m_error);
    jsonObj->GetIntValue("error_code", &m_errorCode);
    jsonObj->GetStringValue("request", &m_request);
    jsonObj->GetStringValue("error_uri", &m_errorUri);
    jsonObj->GetStringValue("error_description", &m_errorDescription);

    m_errorMessage = GetErrorMessageFromErrorCode(m_errorCode);
    if (m_errorMessage.empty())
    {
        m_errorMessage = m_error;
        if (!m_errorDescription.empty())
        {
            m_errorMessage.append(1, ':').append(m_errorDescription);
        }
    }
    return true;
}

ParsingUser* ParsingUser::CreateUser(const JsonObject* jsonObj)
{
    ParsingUser* user = new ParsingUser;
    if (user && user->Init(jsonObj))
    {
        return user;
    }
    if (user)
    {
        delete user;
    }
    return NULL;
}
bool ParsingUser::Init(const JsonObject* jsonObj)
{
    if (!jsonObj)
    {
        return false;
    }
    jsonObj->GetStringValue("idstr", &m_id);
    jsonObj->GetStringValue("screen_name", &m_screenName);
    jsonObj->GetStringValue("name", &m_name);
    jsonObj->GetStringValue("province", &m_province);
    jsonObj->GetStringValue("city", &m_city);
    jsonObj->GetStringValue("location", &m_location);
    jsonObj->GetStringValue("description", &m_description);
    jsonObj->GetStringValue("url", &m_url);
    jsonObj->GetStringValue("profile_image_url", &m_profileImageUrl);
    jsonObj->GetStringValue("profile_url", &m_profileUrl);
    jsonObj->GetStringValue("domain", &m_domain);
    jsonObj->GetStringValue("gender", &m_gender);
    jsonObj->GetIntValue("followers_count", &m_followersCount);
    jsonObj->GetIntValue("friends_count", &m_friendsCount);
    jsonObj->GetIntValue("statuses_count", &m_statusesCount);
    jsonObj->GetIntValue("favourites_count", &m_favouritesCount);
    jsonObj->GetStringValue("cretaed_at", &m_createdAt);
    jsonObj->GetBooleanValue("following", &m_following);
    jsonObj->GetBooleanValue("allow_all_act_msg", &m_allowAllActMsg);
    jsonObj->GetBooleanValue("geo_enabled", &m_geoEnabled);
    jsonObj->GetBooleanValue("verified", &m_verified);
    jsonObj->GetIntValue("verified_type", &m_verifiedType);
    jsonObj->GetStringValue("remark", &m_remark);
    jsonObj->GetBooleanValue("allow_all_comment", &m_allowAllComment);
    jsonObj->GetStringValue("avatar_large", &m_avatarLarge);
    jsonObj->GetStringValue("verified_reason", &m_verifiedReason);
    jsonObj->GetBooleanValue("follow_me", &m_followMe);
    jsonObj->GetIntValue("online_status", &m_onlineStatus);
    jsonObj->GetIntValue("bi_followers_count", &m_biFollowersCount);
    jsonObj->GetStringValue("lang", &m_lang);

    return true;
}

ParsingStatus* ParsingStatus::CreateStatus(const JsonObject* jsonObj)
{
    ParsingStatus* status = new ParsingStatus;
    if (status && status->Init(jsonObj))
    {
        return status;
    }
    if (status)
    {
        delete status;
    }

    return NULL;
}

bool ParsingStatus::Init(const JsonObject* jsonObj)
{
    if (!jsonObj)
    {
        return false;
    }
    jsonObj->GetStringValue("created_at", &m_createdAt);
    jsonObj->GetIntValue("id", &m_id);
    jsonObj->GetStringValue("text", &m_text);
    jsonObj->GetStringValue("source", &m_source);
    jsonObj->GetBooleanValue("favorited", &m_favorited);
    jsonObj->GetBooleanValue("truncated", &m_truncated);
    jsonObj->GetStringValue("in_reply_to_status_id", &m_inReplyToStatusId);
    jsonObj->GetStringValue("in_reply_to_user_id", &m_inReplyToUserId);
    jsonObj->GetStringValue("in_reply_to_screen_name", &m_inReplyToScreenName);
    jsonObj->GetStringValue("thumbnail_pic", &m_thumbnailPic);
    jsonObj->GetStringValue("bmiddle_pic", &m_bmiddlePic);
    jsonObj->GetStringValue("originale_pic", &m_originalPic);
    jsonObj->GetIntValue("reposts_count", &m_repostsCount);
    jsonObj->GetIntValue("comments_count", &m_commentsCount);
    jsonObj->GetStringValue("mid", &m_mid);
    jsonObj->GetStringValue("annotations", &m_annotations);

    JsonObjectSPtr userObj = jsonObj->GetSubObject("user");
    m_user.reset();
    if (userObj)
    {
        ParsingUserSPtr user(ParsingUser::CreateUser(userObj.get()));
        if (!user)
        {
            return false;
        }
        m_user = user;
    }

    return true;
}
}//weibo
}//dk








