#ifndef SINAWEIBOMANAGER_H
#define SINAWEIBOMANAGER_H

#include "IWeibo.h"
#include "GUI/EventArgs.h"
#include "Weibo/ParsingDataStruct.h"
namespace dk
{
namespace weibo
{

class WeiboResultArgs: public EventArgs
{
public:
    enum eResultType
    {
        ERT_UNKNOWN,
        ERT_CORRECT,
        ERT_NETWORKISSUE,
        ERT_ERROR
    };

    virtual EventArgs* Clone() const
    {
        return new WeiboResultArgs(*this);
    }

    eResultType mResultType;
    ParsingBaseSPtr mParsingInfo;
};

class SinaWeiboManager : public IWeibo
{
public:
    SinaWeiboManager();
    bool IsUserTokenValid() const;
    bool ClearUserInfo();

    std::string GetConsumerKey() const 
    { 
        return m_consumerkey; 
    }
    std::string GetConsumerSecret() const
    {
        return m_consumersecret;
    }
    std::string GetRedirectURL() const 
    { 
        return m_redirectURL; 
    }
    std::string GetAccessToken() const
    {
        return m_accessToken; 
    }
    std::string GetUserName() const 
    { 
        return m_userName; 
    }

    int GetShortWeiboMaxLen() const
    {
        return 140;
    }

    void SetConsumer(const char*key, const char* secret);
    void SetRedirectURL(std::string redirectURL) 
    { 
        m_redirectURL = redirectURL; 
    }
    void SetUserInfo(const char* token, int tokeTime, const char* uid);
    void SetUserName(const char* userName);

private:
    std::string m_consumerkey;
    std::string m_consumersecret;
    std::string m_redirectURL;
    std::string m_expiresIn;
    std::string m_accessToken;
    std::string m_userName;
};
}//weibo
}//dk
#endif//SINAWEIBOMANAGER_H
