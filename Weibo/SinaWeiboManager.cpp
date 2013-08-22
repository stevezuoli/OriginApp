#include "Weibo/SinaWeiboManager.h"
#include "Weibo/SinaWeiboMethodImpl.h"
#include <string>
#include "Common/SystemSetting_DK.h"
#include "Utility/SystemUtil.h"
#include "Utility/EncodeUtil.h"
namespace dk
{
namespace weibo
{
SinaWeiboManager::SinaWeiboManager()
{
    m_consumerkey = "2974426448";
    m_consumersecret = "b3c5af204429b3780a4575ed32550c65";
    m_redirectURL = "http://www.duokan.com";
    m_weiboMethod = new SinaWeiboMethodImpl();
    m_accessToken = utility::EncodeUtil::DecodeToken(SystemSettingInfo::GetInstance()->GetSinaWeiboToken());
    if (!m_accessToken.empty())
    {
        m_userName = SystemSettingInfo::GetInstance()->GetSinaWeiboUserName();
    }
}

void SinaWeiboManager::SetConsumer(const char*key, const char* secret)
{
    m_consumerkey = key;
    m_consumersecret = secret;
}

void SinaWeiboManager::SetUserInfo(const char* token, int tokenTime, const char* uid)
{
    m_accessToken.assign(token);
    SystemSettingInfo::GetInstance()->SetSinaWeiboToken(utility::EncodeUtil::EncodeToken(token).c_str());
    SystemSettingInfo::GetInstance()->SetSinaWeiboUid(uid);
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    int tokenUtilTime = curTime.tv_sec + tokenTime;
    SystemSettingInfo::GetInstance()->SetSinaWeiboTokenTime(tokenUtilTime);
}

void SinaWeiboManager::SetUserName(const char* userName)
{
    m_userName.assign(userName);
    SystemSettingInfo::GetInstance()->SetSinaWeiboUserName(userName);
}

bool SinaWeiboManager::IsUserTokenValid() const
{
    if (!m_accessToken.empty())
    {
        struct timeval curTime;
        gettimeofday(&curTime, NULL);
        int tokenUtilTime = SystemSettingInfo::GetInstance()->GetSinaWeiboTokenTime();
        DebugPrintf(DLC_DIAGNOSTIC, "SinaWeiboManager::IsUserTokenValid cur:%d, tt: %d", curTime.tv_sec, tokenUtilTime);
        return (tokenUtilTime > curTime.tv_sec);//token in expires or not.
    }

    return false;//token empty
}

bool SinaWeiboManager::ClearUserInfo()
{
    m_userName.clear();
    m_expiresIn.clear();
    m_accessToken.clear();

    return SystemSettingInfo::GetInstance()->SetSinaWeiboToken("") &&
           SystemSettingInfo::GetInstance()->SetSinaWeiboUid("") &&
           SystemSettingInfo::GetInstance()->SetSinaWeiboTokenTime(0) &&
           SystemSettingInfo::GetInstance()->SetSinaWeiboUserName("");
}
}//weibo
}//dk
