#ifndef __COMMON_DUOKANSERVER_H__
#define __COMMON_DUOKANSERVER_H__
#include <string>


class DuoKanServer
{
public:
    static std::string GetBookHost();
    static std::string GetLoginHost();
    static std::string GetOrderHost();
    static std::string GetDrmHost();
    static std::string GetCommentHost();
    static std::string GetSyncHost();
private:
    static void Init();
    static DuoKanServer* GetInstance();
    DuoKanServer();
    std::string m_bookHost;
    std::string m_loginHost;
    std::string m_orderHost;
    std::string m_drmHost;
    std::string m_syncHost;
    std::string m_commentHost;
};

class XiaomiServer
{
public:
    static std::string GetAccountHost();
    static std::string GetLoginHost();
    static std::string GetLoginUrl();
private:
    void Init();
    static XiaomiServer* GetInstance();
    XiaomiServer();

private:
    std::string m_accountHost;
    std::string m_loginHost;
    std::string m_loginUrl;
};


#endif
