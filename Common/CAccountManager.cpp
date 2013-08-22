#include "Common/CAccountManager.h"
#include <tr1/functional>
#include "../DK91Search/Curl_Post.h"
#include "../DK91Search/DK_XmlParse.h"
#include "Framework/CNativeThread.h"

#include "Wifi/WifiManager.h"
#include "drivers/DeviceInfo.h"
#include "I18n/StringManager.h"
#include "Common/DuoKanServer.h"
#include "drivers/DeviceInfo.h"
#include "../Net/MailService.h"
#include "../Kernel/include/CommonLibs/KernelBase/KernelEncoding.h"
#include "libxml/parser.h"
#include "BookStore/DownloadTaskBuilder.h"
#include "BookStore/XiaomiListDataFetcher.h"

#include "Utility/XmlUtil.h"
#include "Utility/StringUtil.h"
#include "Utility/HttpUtil.h"
#include "Utility/PathManager.h"
#include "Utility/FileSystem.h"
#include "Utility/SystemUtil.h"
#include "Utility/WebBrowserUtil.h"
#include "Utility/ThreadHelper.h"
#include "Utility/MD5Checker.h"
#include "GUI/GUISystem.h"
#include "GUI/EventSet.h"

#include "CommonUI/UIUtility.h"
#include "CommonUI/UIAccountRegisterDialog.h"
#include "PersonalUI/PushedMessagesManager.h"
#include "TouchAppUI/UIBookMenuDlg.h"
#include "DownloadManager/DownloadManager.h"
#include "XMLDomDocument.h"
#include "Utility.h"
#include <vector>

using namespace std;
using namespace dk::bookstore;
using namespace dk::bookstore::model;
using namespace dk::utility;
using namespace dk::common;
using namespace dk::thirdparties;


namespace
{

class Status
{
public:
    Status()
        : m_code(-1)
    {
    }
    bool Init(const XMLDomNode* domNode)
    {
        BEGIN_XML_PARSE_DATA(parseData)
            {"msg", XNDT_STRING, &m_message, false},
            {"code", XNDT_INT, &m_code, false},
        END_XML_PARSE_DATA;
        return XmlUtil::ParseXmlNode(domNode, parseData);
    }

    const std::string& GetMessage() const
    {
        return m_message;
    }

    int GetCode() const
    {
        return m_code;
    }
private:
    std::string m_message;
    int m_code;
};


class RegisterResult
{
public:
    RegisterResult(const char* result)
    {
        m_loggedIn = false;
        XMLDomDocument doc;
        doc.LoadXmlFromBuffer(result, strlen(result));
        const XMLDomNode* root = doc.RootElement();
        std::string token;
        int loggedIn = 0;
        BEGIN_XML_PARSE_DATA(parseData)
            {"status", XNDT_COMPLEX, this, false, ParseStatus},
            {"token", XNDT_STRING, &token, false},
            {"data", XNDT_COMPLEX, this, false, ParseData},
            {"loggedin", XNDT_INT, &loggedIn, false},
        END_XML_PARSE_DATA;
        XmlUtil::ParseXmlNode(root, parseData);
        m_userInfo.SetToken(token.c_str());
        m_loggedIn = loggedIn;
    }
    Status& GetStatus()
    {
        return m_status;
    }
    dk::account::UserInfo& GetUserInfo()
    {
        return m_userInfo;
    }
    bool IsLoggedIn() const
    {
        return m_loggedIn;
    }
private:
    static bool ParseStatus(const XMLDomNode* domNode, void* arg)
    {
        RegisterResult* pThis = (RegisterResult*)arg;
        return pThis->m_status.Init(domNode);
    }

    static bool ParseData(const XMLDomNode* domNode, void* arg)
    {
        RegisterResult* pThis = (RegisterResult*)arg;
        return pThis->m_userInfo.Init(domNode);
    }

    Status m_status;
    dk::account::UserInfo m_userInfo;
    bool m_loggedIn;
};

};

namespace dk
{

namespace account
{

const char* CAccountManager::EventAccount= "EventAccount";
const char* CAccountManager::EventXiaomiUserCardUpdate = "EventXiaomiUserCardUpdate";
//const char* CAccountManager::EventDuokanLoginClosed = "EventDuokanLoginClosed";


static const std::string XIAOMI_LOGIN_OPTION = "login";
static const std::string XIAOMI_REGISTER_OPTION = "register";

static CommandList GetLoginCommands()
{
    CommandList commands;
    commands.push_back(XiaomiServer::GetLoginUrl());
    commands.push_back(XIAOMI_LOGIN_OPTION);
    return commands;
}

static CommandList GetRegisterCommands()
{
    CommandList commands;
    commands.push_back(XiaomiServer::GetLoginUrl());
    commands.push_back(XIAOMI_REGISTER_OPTION);
    return commands;
}

CAccountManager* CAccountManager::GetInstance()
{
    static CAccountManager s_accountManager;
    return &s_accountManager;
}

CAccountManager::CAccountManager()
    : m_loginStatus(NotLoggedIn)
    , m_pXiaomiListDataFetcher(new XiaomiListDataFetcher(DownloadTaskBuilder::BuildXiaomiGetUserCardsTask, model::OT_XIAOMI_USER_CARD))
    , m_accountMapping()
{
    m_userInfo.LoadFromFile();
    m_xiaomiInfo.LoadFromFile(PathManager::GetXiaomiAccountPath().c_str());

    SubscribeEvent(XiaomiListDataFetcher::EventXiaomiListUpdate,
                   *m_pXiaomiListDataFetcher,
                   std::tr1::bind(
                       std::tr1::mem_fn(&CAccountManager::OnFetchXiaomiUserCardFinished),
                       this,
                       std::tr1::placeholders::_1));

    SubscribeMessageEvent(WebBrowserLauncher::EventWebBrowserClosed,
        *WebBrowserLauncher::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&CAccountManager::OnWebBrowserClosed),
        this,
        std::tr1::placeholders::_1));

	SubscribeMessageEvent(CAccountManager::EventAccount, *this,
        std::tr1::bind(
        std::tr1::mem_fn(&CAccountManager::OnLoginEvent),
        this,
        std::tr1::placeholders::_1));
	
	/*SubscribeMessageEvent(CAccountManager::EventDuokanLoginClosed, *this,
        std::tr1::bind(
        std::tr1::mem_fn(&CAccountManager::OnDuoKanLoginCloseFinished),
        this,
        std::tr1::placeholders::_1));*/
}

CAccountManager::~CAccountManager()
{
}

void CAccountManager::SetUserInfo(const UserInfo& rhs)
{
    LockScope lock(m_userInfoLock);
    m_userInfo = rhs;
}

UserInfo CAccountManager::GetUserInfo() const
{
    LockScope lock(m_userInfoLock);
    return m_userInfo;
}

void CAccountManager::SetMiUserInfo(const XiaomiUserInfo& rhs)
{
    LockScope lock(m_userInfoLock);
    m_xiaomiInfo = rhs;
}

XiaomiUserInfo CAccountManager::GetMiUserInfo() const
{
    LockScope lock(m_userInfoLock);
    return m_xiaomiInfo;
}

bool CAccountManager::IsDuokanAccount()
{
    LockScope lock(m_userInfoLock);
    if (m_xiaomiInfo.IsValid())
    {
        return false;
    }
    string email = m_userInfo.GetEmail();
    return StringUtil::IsValidEmail(email.c_str());
}

std::string CAccountManager::GetEmailFromFile()
{
    LockScope lock(m_userInfoLock);
    if (m_xiaomiInfo.IsValid())
    {
        return m_xiaomiInfo.GetXiaomiID();
    }
    return m_userInfo.GetEmail();
}

std::string CAccountManager::GetDuoKanAccount()
{
	LockScope lock(m_userInfoLock);
    return m_userInfo.GetEmail();
}

std::string CAccountManager::GetAliasFromFile()
{
    XiaomiUserInfo miInfo = GetMiUserInfo();
    if (miInfo.IsValid())
    {
        LockScope lock(m_xiaomiUserCardLock);
		std::string id = miInfo.GetXiaomiID();
		string alias = id;
        if (!m_xiaomiUserCards.empty() &&
             m_xiaomiUserCards.find(id) != m_xiaomiUserCards.end())
        {
            alias = !m_xiaomiUserCards[id]->GetMiliaoNick().empty() ?
                m_xiaomiUserCards[id]->GetMiliaoNick() : m_xiaomiUserCards[id]->GetAliasNick();
        }
		if(alias.empty())
		{
			alias = id;
		}
        return alias;
    }
    LockScope lock(m_userInfoLock);
    return m_userInfo.GetAlias();
}

std::string CAccountManager::GetDuoKanMailFromFile()
{
    LockScope lock(m_userInfoLock);
    return m_userInfo.GetDuokanMail();
}

std::string CAccountManager::GetTokenFromFile()
{
    LockScope lock(m_userInfoLock);
    if (m_xiaomiInfo.IsValid())
    {
        return m_xiaomiInfo.GetToken();
    }
    return m_userInfo.GetToken();
}

std::string CAccountManager::GetDuoKanMailPasswordFromFile()
{
    LockScope lock(m_userInfoLock);
    return m_userInfo.GetDuokanMailPassword();
}

bool CAccountManager::GetDuokanByXiaomiAccount(const char* xiaomiAccount, std::vector<std::string>* duokanAccounts)
{
	if (xiaomiAccount && duokanAccounts)
	{
		return m_accountMapping.GetDuokanByXiaomiAccount(xiaomiAccount, duokanAccounts);
	}
	return false;
}

bool CAccountManager::LoginWithPasswordAsync(
        const std::string& email,
        const std::string& password)
{
    return LoginWithPasswordAsync(email.c_str(), password.c_str());
}

bool CAccountManager::LoginWithPasswordAsync(const char* email,
        const char* password)
{
    if (StringUtil::IsNullOrEmpty(email) || StringUtil::IsNullOrEmpty(password))
    {
        return false;
    }

    m_loginStatus = NotLoggedIn;
    UserInfo userInfo;
    userInfo.SetEmail(email);
    userInfo.SaveToFile();
    SetUserInfo(userInfo);
    DownloadTaskSPtr downloadTask = 
        DownloadTaskBuilder::BuildLoginWithPasswordTask(
                email,
            password);
    if (downloadTask)
    {
        m_loginStatus = Logging;
        SubscribeEvent(DownloadTask::EventDownloadTaskFinished,
                *downloadTask.get(),
                std::tr1::bind(
                    std::tr1::mem_fn(&CAccountManager::OnLoginWithPasswordAsyncFinished),
                    this,
                    std::tr1::placeholders::_1));
        DownloadManager::GetInstance()->AddTask(downloadTask);
        return true;
    }
    return false;
}

bool CAccountManager::LoginWithTokenAsync()
{
    XiaomiUserInfo miInfo = GetMiUserInfo();
    LoginStatus status = miInfo.IsValid() ? LoginXiaomiAccount(true) : LoginDuokanAccount(true);
    return (status == LoggedIn_DuoKan || status == LoggedIn_XiaoMi || status == Logging);
}

bool CAccountManager::LoginWithToken()
{
    XiaomiUserInfo miInfo = GetMiUserInfo();
	LoginStatus status = miInfo.IsValid() ? LoginXiaomiAccount(false) : LoginDuokanAccount(false);
    return (status == LoggedIn_DuoKan || status == LoggedIn_XiaoMi || status == Logging);
}

bool CAccountManager::LogoutAsync()
{
    if (!LogoutXiaomiAccount())
    {
        LogoutDuokanAccount();
    }
}

bool CAccountManager::LogoutDuokanAccount()
{
    string email = GetEmailFromFile();
    if (email.empty())
    {
        return false;
    }

    PushedMessagesManager* pPushedMessagesMgr = PushedMessagesManager::GetInstance();
    if (pPushedMessagesMgr)
    {
        pPushedMessagesMgr->StopGetMessages();
    }

    //DownloadTaskSPtr downloadTask = 
    //    DownloadTaskBuilder::BuildLogoutTask(email.c_str());
    //if (downloadTask)
    //{
        //SubscribeEvent(DownloadTask::EventDownloadTaskFinished,
        //        *downloadTask.get(),
        //        std::tr1::bind(
        //            std::tr1::mem_fn(&CAccountManager::OnLogoutAsyncFinished),
        //            this,
        //            std::tr1::placeholders::_1));
        //DownloadManager::GetInstance()->AddTask(downloadTask);
        
        UserInfo userInfo;
        userInfo.SetEmail(email.c_str());
        userInfo.SaveToFile();
        SetUserInfo(userInfo);
        m_loginStatus = NotLoggedIn;
        AccountEventArgs args;
        args.logStatus = AccountEventArgs::LS_LOGOUT;
        FireEvent(EventAccount, args);
        return true;
    //}
    //return false;
}

bool CAccountManager::RegisterAsync(
        const std::string& email,
        const std::string& aliasName,
        const std::string& password)
{
    return RegisterAsync(email.c_str(), aliasName.c_str(), password.c_str());
}

bool CAccountManager::RegisterAsync(
        const char* email,
        const char* aliasName,
        const char* password)
{
    if (StringUtil::IsNullOrEmpty(email)
            || StringUtil::IsNullOrEmpty(aliasName)
            || StringUtil::IsNullOrEmpty(password))
    {
        return false;
    }
    DownloadTaskSPtr downloadTask = 
        DownloadTaskBuilder::BuildRegisterTask(email, aliasName, password);
    if (downloadTask)
    {
        SubscribeEvent(DownloadTask::EventDownloadTaskFinished,
                *downloadTask.get(),
                std::tr1::bind(
                    std::tr1::mem_fn(&CAccountManager::OnRegisterAsyncFinished),
                    this,
                    std::tr1::placeholders::_1));
        DownloadManager::GetInstance()->AddTask(downloadTask);
        return true;
    }
    return false;
}

bool CAccountManager::ForgetPasswordAsync(const std::string& email)
{
    return ForgetPasswordAsync(email.c_str());
}

bool CAccountManager::ForgetPasswordAsync(
        const char* email)
{
    if (StringUtil::IsNullOrEmpty(email))
    {
        return false;
    }
    DownloadTaskSPtr downloadTask = 
        DownloadTaskBuilder::BuildForgetPasswordTask(email);
    if (downloadTask)
    {
        SubscribeEvent(DownloadTask::EventDownloadTaskFinished,
                *downloadTask.get(),
                std::tr1::bind(
                    std::tr1::mem_fn(&CAccountManager::OnForgetPasswordAsyncFinished),
                    this,
                    std::tr1::placeholders::_1));
        DownloadManager::GetInstance()->AddTask(downloadTask);
        return true;
    }
    return false;
}

bool CAccountManager::ChangePasswordAsync(
        const std::string& email,
        const std::string& oldPassword,
        const std::string& newPassword)
{
    return ChangePasswordAsync(
            email.c_str(),
            oldPassword.c_str(),
            newPassword.c_str());
}

bool CAccountManager::ChangePasswordAsync(
        const char* email,
        const char* oldPassword,
        const char* newPassword)
{
    if (StringUtil::IsNullOrEmpty(email)
            || StringUtil::IsNullOrEmpty(oldPassword)
            || StringUtil::IsNullOrEmpty(newPassword))
    {
        return false;
    }
    DownloadTaskSPtr downloadTask = 
        DownloadTaskBuilder::BuildChangePasswordTask(email, oldPassword, newPassword);
    if (downloadTask)
    {
        SubscribeEvent(DownloadTask::EventDownloadTaskFinished,
                *downloadTask.get(),
                std::tr1::bind(
                    std::tr1::mem_fn(&CAccountManager::OnChangePasswordAsyncFinished),
                    this,
                    std::tr1::placeholders::_1));
        DownloadManager::GetInstance()->AddTask(downloadTask);
        return true;
    }
    return false;
}

bool CAccountManager::ChangeAliasAsync(
        const std::string& email,
        const std::string& newAlias)
{
    return ChangeAliasAsync(email.c_str(), newAlias.c_str());
}


bool CAccountManager::ChangeAliasAsync(
        const char* email,
        const char* newAlias)
{
    if (StringUtil::IsNullOrEmpty(email)
            || StringUtil::IsNullOrEmpty(newAlias))
    {
        return false;
    }
    DownloadTaskSPtr downloadTask = 
        DownloadTaskBuilder::BuildChangeAliasTask(email, newAlias);
    if (downloadTask)
    {
        SubscribeEvent(DownloadTask::EventDownloadTaskFinished,
                *downloadTask.get(),
                std::tr1::bind(
                    std::tr1::mem_fn(&CAccountManager::OnChangeAliasAsyncFinished),
                    this,
                    std::tr1::placeholders::_1));
        DownloadManager::GetInstance()->AddTask(downloadTask);
        return true;
    }
    return false;
}



/*
 *
 http://172.27.11.77/wiki/index.php?title=%E9%9D%9E%E6%B5%8F%E8%A7%88%E5%99%A8%E6%8E%A5%E5%8F%A3#.E6.8E.A5.E5.8F.A3.E5.9C.B0.E5.9D.80_4
 *<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
 <result>
     <status>
         <code>0</code>
         <msg>成功</msg>
     </status>
     <loggedin>1</loggedin>
 </result>
 */

bool CAccountManager::OnLoginWithTokenAsyncFinished(const EventArgs& args)
{
    LockScope scope(m_logginInfoLock);
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    std::string errorMessage;
    AccountEventArgs loginEventArgs;
    loginEventArgs.logStatus = AccountEventArgs::LS_TOKEN_FAILED;
    if (downloadTaskFinishedArgs.succeeded)
    {
        DownloadTask* downloadTask = downloadTaskFinishedArgs.downloadTask;
        std::string result = downloadTask->GetString();
        DebugPrintf(DLC_DIAGNOSTIC, "CAccountManager::OnLoginWithTokenAsyncFinished(), %s", result.c_str());
        RegisterResult registerResult(result.c_str());
        Status& status = registerResult.GetStatus();

        loginEventArgs.errorType = AccountEventArgs::ET_SERVER;
        loginEventArgs.errorCode = status.GetCode();
        loginEventArgs.errorMessage = StringManager::GetLoginErrorMessage(
                status.GetCode(),
                status.GetMessage().c_str());
        if (0 == status.GetCode())
        {
            if (registerResult.IsLoggedIn())
            {
                // clear xiaomi info if exists
                XiaomiUserInfo emptyMiInfo;
                SetMiUserInfo(emptyMiInfo);

                m_loginStatus = LoggedIn_DuoKan;
                loginEventArgs.logStatus = AccountEventArgs::LS_TOKEN_SUCCEEDED;
                SNativeMessage msg;
                msg.iType = KMessageDuoKanIDLogin;
                CNativeThread::Send(msg);
            }
            else
            {
                m_loginStatus = NotLoggedIn;
                loginEventArgs.errorMessage = StringManager::GetPCSTRById(ACCOUNT_NOTLOGGEDIN);
            }
        }
    }
    else
    {
    	m_loginStatus = NotLoggedIn;
        loginEventArgs.errorType = AccountEventArgs::ET_NETWORK;
        loginEventArgs.errorCode = -1;
        loginEventArgs.errorMessage = 
            StringManager::GetPCSTRById(ACCOUNT_NETWORKISSUE);

    }

    FireEvent(EventAccount, loginEventArgs);
    return true;
}

bool CAccountManager::OnLoginWithPasswordAsyncFinished(const EventArgs& args)
{
    LockScope scope(m_logginInfoLock);
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    std::string errorMessage;
    AccountEventArgs loginEventArgs;
    loginEventArgs.logStatus = AccountEventArgs::LS_PASSWORD_FAILED;
    if (downloadTaskFinishedArgs.succeeded)
    {
        DownloadTask* downloadTask = downloadTaskFinishedArgs.downloadTask;
        std::string result = downloadTask->GetString();
        DebugPrintf(DLC_DIAGNOSTIC, "CAccountManager::OnLoginWithPasswordAsyncFinished(), %s",
                result.c_str());
        RegisterResult registerResult(result.c_str());
        Status& status = registerResult.GetStatus();

        loginEventArgs.errorType = AccountEventArgs::ET_SERVER;
        loginEventArgs.errorCode = status.GetCode();
        loginEventArgs.errorMessage = StringManager::GetLoginErrorMessage(
                status.GetCode(),
                status.GetMessage().c_str());
        if (0 == status.GetCode())
        {
            // clear xiaomi info if exists
            XiaomiUserInfo emptyMiInfo;
            SetMiUserInfo(emptyMiInfo);

            registerResult.GetUserInfo().SaveToFile();
            SetUserInfo(registerResult.GetUserInfo());
            m_loginStatus = LoggedIn_DuoKan;
            loginEventArgs.logStatus = AccountEventArgs::LS_PASSWORD_SUCCEEDED;
            SNativeMessage msg;
            msg.iType = KMessageDuoKanIDLogin;
            CNativeThread::Send(msg);
        }
		else
		{
			m_loginStatus = NotLoggedIn;
		}
    }
    else
    {
		m_loginStatus = NotLoggedIn;
        loginEventArgs.errorType = AccountEventArgs::ET_NETWORK;
        loginEventArgs.errorCode = -1;
        loginEventArgs.errorMessage = 
            StringManager::GetPCSTRById(ACCOUNT_NETWORKISSUE);

    }

    FireEvent(EventAccount, loginEventArgs);
    return true;
}

bool CAccountManager::OnRegisterAsyncFinished(const EventArgs& args)
{
    return OnLoginWithPasswordAsyncFinished(args);
}

bool CAccountManager::OnLogoutAsyncFinished(const EventArgs& args)
{
    // don't care about the result
    return true;
}

bool CAccountManager::OnChangePasswordAsyncFinished(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = 
        (const DownloadTaskFinishArgs&)args;
    std::string errorMessage;
    AccountEventArgs loginEventArgs;
    loginEventArgs.logStatus = AccountEventArgs::LS_CHANGE_PASSWORD_FAILED;
    if (downloadTaskFinishedArgs.succeeded)
    {
        DownloadTask* downloadTask = downloadTaskFinishedArgs.downloadTask;
        std::string result = downloadTask->GetString();
        DebugPrintf(DLC_DIAGNOSTIC, 
                "CAccountManager::OnChangePasswordAsyncFinished(), %s",
                result.c_str());
        RegisterResult registerResult(result.c_str());
        Status& status = registerResult.GetStatus();

        loginEventArgs.errorType = AccountEventArgs::ET_SERVER;
        loginEventArgs.errorCode = status.GetCode();
        loginEventArgs.errorMessage = StringManager::GetLoginErrorMessage(
                status.GetCode(),
                status.GetMessage().c_str());
        if (0 == status.GetCode())
        {
            loginEventArgs.logStatus = 
                AccountEventArgs::LS_CHANGE_PASSWORD_SUCCEEDED;
            SNativeMessage msg;
            msg.iType = KMessageDuoKanIDLogin;
            CNativeThread::Send(msg);
        }
    }
    else
    {
        loginEventArgs.errorType = AccountEventArgs::ET_NETWORK;
        loginEventArgs.errorCode = -1;
        loginEventArgs.errorMessage = 
            StringManager::GetPCSTRById(ACCOUNT_NETWORKISSUE);
    }
    FireEvent(EventAccount, loginEventArgs);
    return true;
}

bool CAccountManager::OnChangeAliasAsyncFinished(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    std::string errorMessage;
    AccountEventArgs loginEventArgs;
    loginEventArgs.logStatus = AccountEventArgs::LS_CHANGE_ALIAS_FAILED;
    if (downloadTaskFinishedArgs.succeeded)
    {
        DownloadTask* downloadTask = downloadTaskFinishedArgs.downloadTask;
        std::string result = downloadTask->GetString();
        DebugPrintf(DLC_DIAGNOSTIC, "CAccountManager::OnChangeAliasAsyncFinished(), %s",
                result.c_str());
        RegisterResult registerResult(result.c_str());
        Status& status = registerResult.GetStatus();

        loginEventArgs.errorType = AccountEventArgs::ET_SERVER;
        loginEventArgs.errorCode = status.GetCode();
        loginEventArgs.errorMessage = StringManager::GetLoginErrorMessage(
                status.GetCode(),
                status.GetMessage().c_str());
        if (0 == status.GetCode())
        {
            std::string newAlias = HttpUtil::GetParaFromUrl(
                    downloadTask->GetUrl(),
                    "newalias");
            UserInfo userInfo = GetUserInfo();
            userInfo.SetAlias(newAlias.c_str());
            userInfo.SaveToFile();
            SetUserInfo(userInfo);
            DebugPrintf(DLC_ACCOUNT, "NewAlias: %s", newAlias.c_str());
            loginEventArgs.logStatus = AccountEventArgs::LS_CHANGE_ALIAS_SUCCEEDED;
            SNativeMessage msg;
            msg.iType = KMessageDuoKanIDLogin;
            CNativeThread::Send(msg);
        }
    }
    else
    {
        loginEventArgs.errorType = AccountEventArgs::ET_NETWORK;
        loginEventArgs.errorCode = -1;
        loginEventArgs.errorMessage = 
            StringManager::GetPCSTRById(ACCOUNT_NETWORKISSUE);
    }
    FireEvent(EventAccount, loginEventArgs);
    return true;
}

bool CAccountManager::OnForgetPasswordAsyncFinished(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    std::string errorMessage;
    AccountEventArgs loginEventArgs;
    loginEventArgs.logStatus = AccountEventArgs::LS_RESEND_PASSWORD_FAILED;
    if (downloadTaskFinishedArgs.succeeded)
    {
        DownloadTask* downloadTask = downloadTaskFinishedArgs.downloadTask;
        std::string result = downloadTask->GetString();
        DebugPrintf(DLC_DIAGNOSTIC, "CAccountManager::OnForgetPasswordAsyncFinished(), %s",
                result.c_str());
        RegisterResult registerResult(result.c_str());
        Status& status = registerResult.GetStatus();

        loginEventArgs.errorType = AccountEventArgs::ET_SERVER;
        loginEventArgs.errorCode = status.GetCode();
        loginEventArgs.errorMessage = StringManager::GetLoginErrorMessage(
                status.GetCode(),
                status.GetMessage().c_str());
        if (0 == status.GetCode())
        {
            loginEventArgs.logStatus = AccountEventArgs::LS_RESEND_PASSWORD_SUCCEEDED;
        }
    }
    else
    {
        loginEventArgs.errorType = AccountEventArgs::ET_NETWORK;
        loginEventArgs.errorCode = -1;
        loginEventArgs.errorMessage = 
            StringManager::GetPCSTRById(ACCOUNT_NETWORKISSUE);
    }
    FireEvent(EventAccount, loginEventArgs);
    return true;
}

CAccountManager::LoginStatus CAccountManager::ImplLoginDuokanAccount(bool async)
{
    UserInfo userInfo = GetUserInfo();
    string email;
    std::string token;
    if(!WifiManager::GetInstance()->IsConnected())
    {
        m_loginStatus = NotLoggedIn;
        return m_loginStatus;
    }
    if (IsLoggedIn()) // TODO. How to do if it is logging now? Caller should wait until logging finishes
    {
        return m_loginStatus;
    }
    
    email = userInfo.GetEmail();
    token = userInfo.GetToken();
    if(email.empty() || token.empty())
    {
        m_loginStatus = NotLoggedIn;
		AccountEventArgs loginEventArgs;
    	loginEventArgs.logStatus = AccountEventArgs::LS_TOKEN_FAILED;
        loginEventArgs.errorMessage = StringManager::GetPCSTRById(ACCOUNT_NOTLOGGEDIN);
    	FireEvent(EventAccount, loginEventArgs);
        return m_loginStatus;
    }

    DownloadTaskSPtr downloadTask = DownloadTaskBuilder::BuildLoginWithTokenTask(
            email.c_str(),
            token.c_str());
    if (downloadTask)
    {
    	m_loginStatus = Logging;
        SubscribeEvent(DownloadTask::EventDownloadTaskFinished,
                *downloadTask.get(),
                std::tr1::bind(
                    std::tr1::mem_fn(&CAccountManager::OnLoginWithTokenAsyncFinished),
                    this,
                    std::tr1::placeholders::_1));
        if (!async)
            DownloadManager::GetInstance()->RunTask(downloadTask);
        else
            DownloadManager::GetInstance()->AddTask(downloadTask);
    }
    return m_loginStatus;
}

CAccountManager::LoginStatus CAccountManager::LoginDuokanAccount(bool async)
{
    if(!WifiManager::GetInstance()->IsConnected())
    {
        m_loginStatus = NotLoggedIn;
        return m_loginStatus;
    }
    if (IsLoggedIn()) // TODO. How to do if it is logging now? Caller should wait until logging finishes
    {
        return m_loginStatus;
    }
    
    string empty;
    string migrateNotice = MiMigrationManager::GetInstance()->GetMigrateStage(empty, "login");
    if (migrateNotice == "open" || migrateNotice == "warn")
    {
        return ImplLoginDuokanAccount(async);
    }

    // ask user to migrate or login with Xiaomi account
    //m_loginStatus = NotLoggedIn;
    //DuokanLoginClosedArgs args;
    //FireEvent(EventDuokanLoginClosed, args);
    SNativeMessage msg;
    msg.iType = KMessageDuokanLoginFailed;
    msg.iParam1 = BSE_DUOKAN_ACCOUNT_LOGIN_CLOSED;
    CNativeThread::Send(msg);
    return m_loginStatus;
}

CAccountManager::LoginStatus CAccountManager::LoginXiaomiAccount(bool offline)
{
    if(!WifiManager::GetInstance()->IsConnected())
    {
        m_loginStatus = Disconnected;
        return m_loginStatus;
    }
    if (LoggedIn_XiaoMi == m_loginStatus)
    {
        return m_loginStatus;
    }
    
    // Read the xml every time?
    XiaomiUserInfo miInfo = GetMiUserInfo();
    string miID = miInfo.GetXiaomiID();
    string token = miInfo.GetToken();

    if(!offline && (miID.empty() || token.empty()))
    {
        m_loginStatus = Logging;

        // Launch web browser to login.
        // Web Browser exists when login finishes.
        // Account manager should load xiaomi user info from token file construct the event args
        if (!WebBrowserLauncher::GetInstance()->Exec(GetLoginCommands()))
        {
            m_loginStatus = BrowserNotLaunched;
        }
        return m_loginStatus;
    }

    AccountEventArgs loginEventArgs;
    if (!miID.empty() && !token.empty())
    {
        // the token and id exists
        loginEventArgs.logStatus = AccountEventArgs::LS_TOKEN_SUCCEEDED;
        loginEventArgs.errorCode = miInfo.GetCode();
        loginEventArgs.errorMessage = StringManager::GetLoginErrorMessage(
            miInfo.GetCode(),
            miInfo.GetMsg().c_str());
        loginEventArgs.errorType = AccountEventArgs::ET_NONE;
        m_loginStatus = LoggedIn_XiaoMi;
    
        DebugPrintf(DLC_DIAGNOSTIC, "\n Already Logged to Xiaomi Account, %s, %s \n", miID.c_str(), token.c_str());
        DebugPrintf(DLC_DIAGNOSTIC, "\n Xiaomi loginEventArgs, %d, %s \n", loginEventArgs.errorCode, loginEventArgs.errorMessage.c_str());
    }
    else
    {
        loginEventArgs.logStatus = AccountEventArgs::LS_TOKEN_FAILED;
        loginEventArgs.errorCode = miInfo.GetCode();
        loginEventArgs.errorMessage = StringManager::GetLoginErrorMessage(
            miInfo.GetCode(),
            miInfo.GetMsg().c_str());
        loginEventArgs.errorType = AccountEventArgs::ET_NONE;
        m_loginStatus = NotLoggedIn;
    }
    FireEvent(EventAccount, loginEventArgs);
    return m_loginStatus;
}

bool CAccountManager::LogoutXiaomiAccount()
{
    XiaomiUserInfo miInfo = GetMiUserInfo();
    if (!miInfo.IsValid())
    {
        return false;
    }

    // Remove login file and clear user info
    if (!FileSystem::DeleteFile(PathManager::GetXiaomiAccountPath().c_str()))
    {
        DebugPrintf(DLC_DIAGNOSTIC, "\n Delete token file failed \n");
        return false;
    }

    // clear mi info safely
    XiaomiUserInfo emptyMiInfo;
    SetMiUserInfo(emptyMiInfo);
    m_loginStatus = NotLoggedIn;
    AccountEventArgs args;
    args.logStatus = AccountEventArgs::LS_LOGOUT;
    FireEvent(EventAccount, args);
    return true;
}

void CAccountManager::RegisterAccount()
{
	if(!WifiManager::GetInstance()->IsConnected())
    {
        m_loginStatus = Disconnected;
        return;
    }

    string empty;
    string stage = MiMigrationManager::GetInstance()->GetMigrateStage(empty, "reg");
    if (stage == "open" || stage.empty())
    {
        RegisterDuoKanAccount();
    }
    else
    {
        RegisterXiaomiAccount();
    }
}

void CAccountManager::RegisterDuoKanAccount()
{
	UIAccountRegisterDialog registerDialog(
            GUISystem::GetInstance()->GetTopFullScreenContainer(),
            StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_REGISTER_ACCOUNT));
    registerDialog.DoModal();
}

void CAccountManager::RegisterXiaomiAccount()
{
    // Always Launch web browser to register.
    m_loginStatus = Registering;
    if (!WebBrowserLauncher::GetInstance()->Exec(GetRegisterCommands()))
    {
        m_loginStatus = BrowserNotLaunched;
    }
}

/*
bool CAccountManager::OnDuoKanLoginCloseFinished(const EventArgs& args)
{
	std::vector<int> btnIDs;
    btnIDs.push_back(ID_BTN_ACCOUNT_XIAOMI_LOGIN);
    btnIDs.push_back(ID_BIN_ACCOUNT_XIAOMI_MIGRATE);
    btnIDs.push_back(ID_INVALID);

    std::vector<int> strIDs;
    strIDs.push_back(TOUCH_SYSTEMSETTING_LOGIN_XIAOMI_ACCOUNT);
    strIDs.push_back(ACCOUNT_XIAOMI_MIGRATE);
    strIDs.push_back(-1);

    UIBookMenuDlg dlgMenu(GUISystem::GetInstance()->GetTopFullScreenContainer(), btnIDs, strIDs); 
    dlgMenu.MakeCenter(dlgMenu.GetWidth(), dlgMenu.GetHeight());
	if (dlgMenu.DoModal() == IDOK)
    {
    	switch(dlgMenu.GetCommandId())
		{
			case ID_BTN_ACCOUNT_XIAOMI_LOGIN:
				{
					m_loginStatus = Logging;
			        if (!WebBrowserLauncher::GetInstance()->Exec(GetLoginCommands()))
			        {
			            m_loginStatus = BrowserNotLaunched;
			        }
				}
				break;
			case ID_BIN_ACCOUNT_XIAOMI_MIGRATE:
				{
					MiMigrationManager::GetInstance()->StartMigrate();
				}
				break;
		}
    }
	return true;
}
*/

bool CAccountManager::TransferToXiaomiAccount()
{
    // TODO. Launch web browser to register
    return false;
}

bool CAccountManager::LaunchWebBrowser(const dk::utility::CommandList& args)
{
    return WebBrowserLauncher::GetInstance()->Exec(args);
}

CAccountManager::XiaomiAccountAction CAccountManager::GetXiaomiActionByCommands(const dk::utility::CommandList& args)
{
    if (args.empty())
    {
        return Xiaomi_Unknown;
    }

    std::string url = args[0];
    if (args.size() >= 2)
    {
        if (url.find(XiaomiServer::GetLoginUrl()) != std::string::npos)
        {
            // login or register
            std::string option = args[1];
            if (option.find(XIAOMI_LOGIN_OPTION) != std::string::npos)
            {
                return Xiaomi_Login;
            }
            else if(option.find(XIAOMI_REGISTER_OPTION) != std::string::npos)
            {
                return Xiaomi_Register;
            }
        }
    }

    // TODO. support xiaomi account transfer
    return Xiaomi_Unknown;
}

bool CAccountManager::OnWebBrowserClosed(const EventArgs& args)
{
    const WebBrowserArgs& webArgs = (const WebBrowserArgs&)args;
    CommandList commands = webArgs.startArguments;
    if (!commands.empty())
    {
        XiaomiAccountAction action = GetXiaomiActionByCommands(commands);
        switch (action)
        {
        case Xiaomi_Login:
            return OnWebBrowserLoginFinished(args);
        case Xiaomi_Register:
            return OnWebBrowserRegisterFinished(args);
        default:
            break;
        }
    }
    return true;
}

bool CAccountManager::OnWebBrowserLoginFinished(const EventArgs& args)
{
    // Token exists, return the status directly
    // NOTE: Need a logic to handle the expiration
    AccountEventArgs loginEventArgs;

    // Reload Xiaomi Account Info from xml
    XiaomiUserInfo miInfo;
    miInfo.LoadFromFile(PathManager::GetXiaomiAccountPath().c_str());
    SetMiUserInfo(miInfo);

    loginEventArgs.errorCode = miInfo.GetCode();
    loginEventArgs.errorMessage = StringManager::GetLoginErrorMessage(
        miInfo.GetCode(),
        miInfo.GetMsg().c_str());
    loginEventArgs.errorType = AccountEventArgs::ET_NONE;

    if (loginEventArgs.errorCode == 0)
    {
        m_loginStatus = LoggedIn_XiaoMi;
        loginEventArgs.logStatus = AccountEventArgs::LS_TOKEN_SUCCEEDED;
    }
    else
    {
    	loginEventArgs.logStatus = AccountEventArgs::LS_WEBBROSER_FAILED;
        m_loginStatus = NotLoggedIn;
    }

    FireEvent(EventAccount, loginEventArgs);
    return true;
}

bool CAccountManager::OnWebBrowserRegisterFinished(const EventArgs& args)
{
    AccountEventArgs loginEventArgs;
    loginEventArgs.logStatus = AccountEventArgs::LS_PASSWORD_FAILED;

    // Reload Xiaomi Account Info from xml
    XiaomiUserInfo miInfo;
    miInfo.LoadFromFile(PathManager::GetXiaomiAccountPath().c_str());
    SetMiUserInfo(miInfo);


    loginEventArgs.errorCode = miInfo.GetCode();
    loginEventArgs.errorMessage = StringManager::GetLoginErrorMessage(
        miInfo.GetCode(),
        miInfo.GetMsg().c_str());
    loginEventArgs.errorType = AccountEventArgs::ET_NONE;

    if (loginEventArgs.errorCode == 0)
    {
        m_loginStatus = LoggedIn_XiaoMi;
        loginEventArgs.logStatus = AccountEventArgs::LS_PASSWORD_SUCCEEDED;        
    }
    else
    {
        m_loginStatus = NotLoggedIn;
    }

    FireEvent(EventAccount, loginEventArgs);
    return true;
}



FetchDataResult CAccountManager::FetchXiaomiUserCard()
{
    if(LoggedIn_XiaoMi != m_loginStatus)
	{
        return FDR_FAILED;
    }
    XiaomiUserInfo miInfo = GetMiUserInfo();
    std::string userID = miInfo.GetXiaomiID();
    if (userID.empty())
    {
        return FDR_FAILED;
    }

    std::vector<std::string> userIDs;
    userIDs.push_back(userID);
    return m_pXiaomiListDataFetcher->FetchData(userIDs);
}

bool CAccountManager::OnFetchXiaomiUserCardFinished(const EventArgs& args)
{
    if (m_loginStatus != LoggedIn_XiaoMi)
    {
        return false;
    }

    // Cache the books
    const XiaomiDataUpdateArgs& dataUpdateArgs = (const XiaomiDataUpdateArgs&)args;
    if (dataUpdateArgs.succeeded)
    {
        LockScope lock(m_xiaomiUserCardLock);
        
        // Update cache
        m_xiaomiUserCards = XiaomiUserCard::FromBasicObjectList(dataUpdateArgs.dataList);
    }
    
    FireEvent(EventXiaomiUserCardUpdate, args);
    return true;
}

void CAccountManager::FetchUserInfo()
{
    if(LoggedIn_XiaoMi != m_loginStatus)
	{
        return;
    }

    DownloadTaskSPtr downloadTask = DownloadTaskBuilder::BuildGetUserInfoTask(GetEmailFromFile().c_str());
    if (downloadTask)
    {
        SubscribeEvent(DownloadTask::EventDownloadTaskFinished,
                *downloadTask.get(),
                std::tr1::bind(
                    std::tr1::mem_fn(&CAccountManager::OnFetchUserInfoFinished),
                    this,
                    std::tr1::placeholders::_1));
        DownloadManager::GetInstance()->AddTask(downloadTask);
    }
}

bool CAccountManager::OnFetchUserInfoFinished(const EventArgs& args)
{
    if (m_loginStatus != LoggedIn_XiaoMi)
    {
        return false;
    }

    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    if (downloadTaskFinishedArgs.succeeded)
    {
        DownloadTask* downloadTask = downloadTaskFinishedArgs.downloadTask;
        const std::string& result = downloadTask->GetString();
        RegisterResult registerResult(result.c_str());
        if (0 == registerResult.GetStatus().GetCode())
        {
            registerResult.GetUserInfo().SaveToFile();
            SetUserInfo(registerResult.GetUserInfo());
        }
    }
	return true;
}


bool CAccountManager::OnLoginEvent(const EventArgs& args)
{
    const AccountEventArgs& accountEventArgs = (const AccountEventArgs&)args;
	if(IsLoggedIn())
	{
		if(LoggedIn_XiaoMi == m_loginStatus)
		{
			// fetch user card
	        FetchXiaomiUserCard();
			FetchUserInfo();
			FetchAccountMapping();
		}
	}
	else
	{
		if((accountEventArgs.logStatus == AccountEventArgs::LS_TOKEN_FAILED ||
            accountEventArgs.errorCode == BSE_DUOKAN_ACCOUNT_MIGRATED) &&
           accountEventArgs.errorCode != BSE_DUOKAN_ACCOUNT_LOGIN_CLOSED)
		{
            if (accountEventArgs.errorCode == BSE_DUOKAN_ACCOUNT_MIGRATED)
            {
                LogoutDuokanAccount();
            }
			UIUtility::ShowAccountLoginDlg();
		}
	}
}

void CAccountManager::FetchAccountMapping()
{
    if(LoggedIn_XiaoMi != m_loginStatus)
	{
        return;
    }

    DownloadTaskSPtr downloadTask = DownloadTaskBuilder::BuildGetAccountMappingTask();
    if (downloadTask)
    {
        SubscribeEvent(DownloadTask::EventDownloadTaskFinished,
                *downloadTask.get(),
                std::tr1::bind(
                    std::tr1::mem_fn(&CAccountManager::OnFetchAccountMappingFinished),
                    this,
                    std::tr1::placeholders::_1));
        DownloadManager::GetInstance()->AddTask(downloadTask);
    }
}

bool CAccountManager::OnFetchAccountMappingFinished(const EventArgs& args)
{
    if (m_loginStatus != LoggedIn_XiaoMi)
    {
        return false;
    }

	const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    if (downloadTaskFinishedArgs.succeeded)
    {
        DownloadTask* downloadTask = downloadTaskFinishedArgs.downloadTask;
        const std::string& result = downloadTask->GetString();
		JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
	    if (jsonObj.get())
	    {
	    	JsonObjectSPtr jsonStatus = jsonObj->GetSubObject("status");
	        if (jsonStatus.get())
	        {
	        	std::string code("");
	            if(jsonStatus->GetStringValue("code", &code) && code == "0")
	        	{
					JsonObjectSPtr jsonUsers = jsonObj->GetSubObject("dk_users");
					if(jsonUsers.get())
					{
						m_accountMapping.AddMapping(GetEmailFromFile().c_str(), jsonUsers);
					}
					
	        	}
	        }
	    }
    }
	return true;
}

} // namespace account
} // namespace dk

