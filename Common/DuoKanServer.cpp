#include "Common/DuoKanServer.h"
#include "Utility/ConfigFile.h"
#include "Utility/PathManager.h"

using namespace dk::utility;

#define BOOK_HOST "http://book.duokan.com"
#define LOGIN_HOST "http://api.duokan.com"
#define SYNC_HOST "http://r.duokan.com"
#define COMMENT_HOST "http://book.duokan.com"
#define ORDER_HOST BOOK_HOST

#define XIAOMI_ACCOUNT_HOST "http://api.account.xiaomi.com"
#define XIAOMI_LOGIN_HOST   "http://login.dushu.xiaomi.com"
//#define XIAOMI_LOGIN_HOST   "http://dkmars"
#define XIAOMI_WEB_REGISTER "/dk_id/api/xiaomi_web_reg"

DuoKanServer::DuoKanServer()
    : m_bookHost(BOOK_HOST)
    , m_loginHost(LOGIN_HOST)
    , m_orderHost(BOOK_HOST)
    , m_drmHost(BOOK_HOST)
    , m_syncHost(SYNC_HOST)
    , m_commentHost(COMMENT_HOST)
{
}

std::string DuoKanServer::GetBookHost()
{
    Init();
    return GetInstance()->m_bookHost;
}

std::string DuoKanServer::GetLoginHost()
{
    Init();
    return GetInstance()->m_loginHost;
}

std::string DuoKanServer::GetOrderHost()
{
    Init();
    return GetInstance()->m_orderHost;
}

std::string DuoKanServer::GetDrmHost()
{
    Init();
    return GetInstance()->m_drmHost;
}

std::string DuoKanServer::GetCommentHost()
{
    Init();
    return GetInstance()->m_commentHost;
}

std::string DuoKanServer::GetSyncHost()
{
    Init();
    return GetInstance()->m_syncHost;
}

DuoKanServer* DuoKanServer::GetInstance()
{
    static DuoKanServer duokanServer;
    return &duokanServer;
}

void DuoKanServer::Init()
{
#if 1
    static bool init = false;
    if (init)
    {
        return;
    }
    init = true;
    ConfigFile config;
    if (!config.LoadFromFile(PathManager::GetCheatFilePath().c_str()))
    {
        return;
    }
    DuoKanServer* duokanServer = GetInstance();
    config.GetString("book_host", &duokanServer->m_bookHost);
    config.GetString("login_host", &duokanServer->m_loginHost);
    config.GetString("order_host", &duokanServer->m_orderHost);
    config.GetString("drm_host", &duokanServer->m_drmHost);
    config.GetString("sync_host", &duokanServer->m_syncHost);
    config.GetString("comment_host", &duokanServer->m_commentHost);
#endif
}

std::string XiaomiServer::GetAccountHost()
{
    return GetInstance()->m_accountHost;
}

std::string XiaomiServer::GetLoginHost()
{
    return GetInstance()->m_loginHost;
}

std::string XiaomiServer::GetLoginUrl()
{
    return GetInstance()->m_loginUrl;
}

void XiaomiServer::Init()
{
    ConfigFile config;

    // TODO. Use xiaomi's standalone cheat file instead.
    if (!config.LoadFromFile(PathManager::GetCheatFilePath().c_str()))
    {
        return;
    }
    config.GetString("login_host", &m_loginHost);
    m_loginUrl = m_loginHost + XIAOMI_WEB_REGISTER;
}

XiaomiServer* XiaomiServer::GetInstance()
{
    static XiaomiServer xiaomiServer;
    return &xiaomiServer;
}

XiaomiServer::XiaomiServer()
    : m_accountHost(XIAOMI_ACCOUNT_HOST)
    , m_loginHost(XIAOMI_LOGIN_HOST)
    , m_loginUrl(XIAOMI_LOGIN_HOST)
{
    m_loginUrl.append(XIAOMI_WEB_REGISTER);
    Init();
}
