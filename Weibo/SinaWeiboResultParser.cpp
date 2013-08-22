#include "Weibo/SinaWeiboResultParser.h" 
#include "Utility/JsonObject.h"

namespace dk
{
namespace weibo
{
ParsingOauthRetSPtr SinaWeiboResultParser::DoParseOauthRet(const char* oauthRetJsonString)
{
    JsonObjectSPtr jsonObj = JsonObject::CreateFromString(oauthRetJsonString);
    if (!jsonObj)
    {
        return ParsingOauthRetSPtr();
    }

    ParsingOauthRetSPtr oauthRet(ParsingOauthRet::CreateOautRet(jsonObj.get()));
    return oauthRet ? oauthRet : ParsingOauthRetSPtr();
}

ParsingApiErrorSPtr SinaWeiboResultParser::DoParseApiError(const char* apiErrorJsonString)
{
    JsonObjectSPtr jsonObj = JsonObject::CreateFromString(apiErrorJsonString);
    if (!jsonObj)
    {
        return ParsingApiErrorSPtr();
    }

    ParsingApiErrorSPtr apiError(ParsingApiError::CreateApiError(jsonObj.get()));
    return apiError ? apiError : ParsingApiErrorSPtr();
}

ParsingUserSPtr SinaWeiboResultParser::DoParseUser(const char* userJsonString)
{
    JsonObjectSPtr jsonObj = JsonObject::CreateFromString(userJsonString);;
    if (!jsonObj)
    {
        return ParsingUserSPtr();
    }

    ParsingUserSPtr user(ParsingUser::CreateUser(jsonObj.get()));
    return user ? user : ParsingUserSPtr();
}
}//weibo
}//dk
