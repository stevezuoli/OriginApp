#ifndef __COMMON__USERINFO_H__
#define __COMMON__USERINFO_H__

#include <string>
#include <map>
#include "GUI/EventArgs.h"
#include "XMLDomDocument.h"

namespace dk
{
namespace account
{

class XiaomiUserInfo
{
public:
    XiaomiUserInfo();

    std::string GetXiaomiID();
    std::string GetToken();
    std::string GetMsg();
    int GetCode();

    bool IsValid();
    void Clear();
    bool LoadFromFile(const char* path);
private:
    std::map<std::string, std::string> m_infoMap;
};

class UserInfo
{
public:
    bool Init(const XMLDomNode* domNode);

    const std::string& GetAlias() const;

    void SetEmail(const char* email);

    const std::string& GetEmail() const;

    const std::string& GetDuokanMail() const;

    const std::string& GetDuokanMailPassword() const;

    const std::string& GetToken() const;

    void SetToken(const char* token);
    void SetAlias(const char* alias);

    void LoadFromFile();

    void SaveToFile() const;

private:
    std::string m_alias;
    std::string m_email;
    std::string m_duokanMail;
    std::string m_duokanMailPassword;
    std::string m_token;
};

class AccountEventArgs: public EventArgs
{
public:
    enum LogStatus
    {
        LS_PASSWORD_SUCCEEDED,
        LS_PASSWORD_FAILED,
        LS_TOKEN_SUCCEEDED,
        LS_TOKEN_FAILED,
        LS_LOGOUT,
        LS_CHANGE_ALIAS_SUCCEEDED,
        LS_CHANGE_ALIAS_FAILED,
        LS_CHANGE_PASSWORD_SUCCEEDED,
        LS_CHANGE_PASSWORD_FAILED,
        LS_RESEND_PASSWORD_SUCCEEDED,
        LS_RESEND_PASSWORD_FAILED,
        LS_WEBBROSER_FAILED,
    };
    enum ErrorType
    {
    	ET_NONE,
        ET_USER_INPUT,// 用户输入错误
        ET_NETWORK, // 网络异常
        ET_SERVER // 服务器端传回的error code
    };

    AccountEventArgs()
    {
    }
    AccountEventArgs(LogStatus status)
        : logStatus(status)
        , errorType(ET_NONE)
        , errorCode(0)
    {

    }

    virtual EventArgs* Clone() const
    {
        return new AccountEventArgs(*this);
    }

    bool IsLoggedIn() const
    {
        return LS_PASSWORD_SUCCEEDED == logStatus 
            || LS_TOKEN_SUCCEEDED == logStatus;
    }

    LogStatus logStatus;
    ErrorType errorType;
    int errorCode;
    std::string errorMessage;
};



class MigrateResult
{
public:
    MigrateResult();

    std::string GetXiaomiID();
    std::string GetToken();
    std::string GetMsg();
    std::string GetMigratedUserID();
    void Clear();
    bool LoadFromFile(const char* path);
private:
    std::map<std::string, std::string> m_infoMap;
};

class DuokanLoginClosedArgs: public EventArgs
{
public:
    virtual EventArgs* Clone() const
    {
        return new DuokanLoginClosedArgs(*this);
    }
    std::string state;
};

} // namespace account
} // namespace dk
#endif
