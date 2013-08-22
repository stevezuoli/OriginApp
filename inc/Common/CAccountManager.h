#ifndef _CACCOUNTMANAGER_H_
#define _CACCOUNTMANAGER_H_

#include <string>
#include "dkBaseType.h"
#include "singleton.h"
#include "interface.h"
#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"
#include "GUI/EventListener.h"
#include "I18n/StringManager.h"
#include "Common/UserInfo.h"
#include "Common/LockObject.h"
#include "BookStore/BookStoreTypes.h"
#include "BookStore/XiaomiUserCard.h"
#include "Thirdparties/MiMigrationManager.h"
#include "BookStore/AccountMapping.h"

using namespace dk::bookstore;

namespace dk
{

namespace bookstore
{
    class XiaomiListDataFetcher;
};

namespace account
{

class CAccountManager : public EventSet, EventListener
{
private:
    CAccountManager();
    virtual ~CAccountManager();

public:
	enum LoginStatus
	{
        Disconnected = 0,
		LoggedIn_XiaoMi,
		LoggedIn_DuoKan,
		NotLoggedIn,
		Logging,
        Registering,
        BrowserNotLaunched,
	};

    static const char* EventAccount;
    static const char* EventXiaomiUserCardUpdate;
    static const char* EventAskForMigratingToXiaomi;
    //static const char* EventDuokanLoginClosed;

public:
    static CAccountManager* GetInstance();

    // Common login functions
    bool LoginWithTokenAsync();
    bool LoginWithToken();
    bool LogoutAsync();

    // Xiaomi Account
    LoginStatus LoginXiaomiAccount(bool offline = true);
	void RegisterAccount();
	void RegisterDuoKanAccount();
    void RegisterXiaomiAccount();
    bool TransferToXiaomiAccount();
    bool LogoutXiaomiAccount();

    // Duokan Account
    bool LoginWithPasswordAsync(
            const std::string& email,
            const std::string& password);
    bool LoginWithPasswordAsync(
            const char* email,
            const char* password);
    bool RegisterAsync(
            const std::string& email,
            const std::string& aliasName,
            const std::string& password);
    bool RegisterAsync(
            const char* email,
            const char* aliasname,
            const char* password);
    bool ForgetPasswordAsync(const std::string& email);
    bool ForgetPasswordAsync(const char* email);
    bool ChangePasswordAsync(
            const std::string& email,
            const std::string& oldPassword,
            const std::string& newPassword);
    bool ChangePasswordAsync(
            const char* email,
            const char* oldPassword,
            const char* newPassword);
    bool ChangeAliasAsync(
            const std::string& email,
            const std::string& newAlias);
    bool ChangeAliasAsync(
            const char* email,
            const char* newAlias);

    std::string GetEmailFromFile();
	std::string GetDuoKanAccount();
    std::string GetAliasFromFile() ;
    std::string GetDuoKanMailFromFile();
    std::string GetTokenFromFile();
    std::string GetDuoKanMailPasswordFromFile();
	bool GetDuokanByXiaomiAccount(const char* xiaomiAccount, std::vector<std::string>* duokanAccounts);

    //判断是否已经登录过
    bool  IsLoggedIn() const
    {    
        return LoggedIn_DuoKan == m_loginStatus || LoggedIn_XiaoMi == m_loginStatus;
    }
	LoginStatus GetLoginStatus()
	{
		return m_loginStatus;
	}
    void  SetNotLoggedIn()
    {
        m_loginStatus = NotLoggedIn;
    }
	
    bool IsDuokanAccount();
private:
    enum XiaomiAccountAction
    {
        Xiaomi_Unknown = -1,
        Xiaomi_Login = 0,
        Xiaomi_Register
    };

private:
    void SetUserInfo(const UserInfo& rhs);
    UserInfo GetUserInfo() const;
    void SetMiUserInfo(const XiaomiUserInfo& rhs);
    XiaomiUserInfo GetMiUserInfo() const;
	
    // Duokan Account
    bool OnRegisterAsyncFinished(const EventArgs& args);
    bool OnLoginWithTokenAsyncFinished(const EventArgs& args);
    bool OnLoginWithPasswordAsyncFinished(const EventArgs& args);
    bool OnLogoutAsyncFinished(const EventArgs& args);
    bool OnChangePasswordAsyncFinished(const EventArgs& args);
    bool OnChangeAliasAsyncFinished(const EventArgs& args);
    bool OnForgetPasswordAsyncFinished(const EventArgs& args);

    LoginStatus LoginDuokanAccount(bool async = true);
    bool LogoutDuokanAccount();
	//bool OnDuoKanLoginCloseFinished(const EventArgs& args);

    // Xiaomi Account
    bool LaunchWebBrowser(const std::vector<std::string>& args);
    bool OnWebBrowserClosed(const EventArgs& args);
    bool OnWebBrowserLoginFinished(const EventArgs& args);
    bool OnWebBrowserRegisterFinished(const EventArgs& args);
    XiaomiAccountAction GetXiaomiActionByCommands(const std::vector<std::string>& args);

    // Get Xiaomi User card
    FetchDataResult FetchXiaomiUserCard();
    bool OnFetchXiaomiUserCardFinished(const EventArgs& args);
	void FetchUserInfo();
	void FetchAccountMapping();
	bool OnFetchAccountMappingFinished(const EventArgs& args);
	bool OnFetchUserInfoFinished(const EventArgs& args);
	bool OnLoginEvent(const EventArgs& args);

private:
    LoginStatus ImplLoginDuokanAccount(bool async);

private:
    // Login type & status
	LoginStatus m_loginStatus;

    // User Info
    UserInfo m_userInfo;
    XiaomiUserInfo m_xiaomiInfo;

    // Locks
    mutable dk::common::LockObject m_userInfoLock;
    dk::common::LockObject m_logginInfoLock;
    dk::common::LockObject m_xiaomiUserCardLock;

    // Xiaomi User Card
    std::tr1::shared_ptr<XiaomiListDataFetcher> m_pXiaomiListDataFetcher;
    model::XiaomiUserCardMap m_xiaomiUserCards;
    AccountMapping m_accountMapping;
private:
    friend class dk::thirdparties::MiMigrationManager;
};

} // namespace account
} // namespace dk

#endif

