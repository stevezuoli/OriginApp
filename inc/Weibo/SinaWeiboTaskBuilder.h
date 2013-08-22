#ifndef SINAWEIBOTASKBUILDER_H
#define SINAWEIBOTASKBUILDER_H

#include <string>
#include "Weibo/IWeiboDef.h"
#include "DownloadManager/DownloadTask.h"

namespace dk
{
namespace weibo
{
class SinaWeiboTaskBuilder
{
public:
    static DownloadTaskSPtr BuildAuthorizeTask();
    static DownloadTaskSPtr BuildOauth2CodeTask(const char* authCode, const char* url);
    static DownloadTaskSPtr BuildOauth2UsernamePwdTask(const char* userName, const char* password);
    static DownloadTaskSPtr BuildGetTokenInfo();
    static DownloadTaskSPtr BuildStatusUpdate(const char* status);
    static DownloadTaskSPtr BuildStatusUpload(const char* status, const char* picPath);
    static DownloadTaskSPtr BuildGetUsersShow(const char* uid);

private:
    static std::string GetMethodBaseURL(const enHostType type);
    static void GetMethodURLFromOption(unsigned int methodOption, std::string &baseURL, eWeiboRequestFormat format = WRF_JSON);
    static const char* GetConsumerKey();
    static const char* GetConsumerSecret();
    static const char* GetRedirectURL();
    static const char* GetAccessToken();
};//WeiboTaskBuilder
}//weibo
}//dk
#endif//SINAWEIBOTASKBUILDER_H
