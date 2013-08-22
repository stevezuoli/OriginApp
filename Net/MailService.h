#ifndef __MAILSERVICE_H__
#define __MAILSERVICE_H__

#include "Net/IMailService.h"
#include "singleton.h"
#include "Utility/ThreadHelper.h"
#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"
#include "GUI/EventListener.h"

#ifdef _X86_LINUX
#define PYTHON_PATH "/usr/bin/python "
#else
#define PYTHON_PATH     "/mnt/us/DK_System/xKindle/python/bin/python "
#endif
#define SMTP_SCRIPT     "/mnt/us/DK_System/xKindle/smtp.py "
#define POP3_SCRIPT         "/mnt/us/DK_System/xKindle/pop3.py "
#define SYNC_PATH         "/mnt/us/DK_Sync/"
#define KILL_PYTHON         "killall -9 python"

class EventMailRecvArgs: public EventArgs
{
public:
    virtual EventArgs* Clone() const
    {
        return new EventMailRecvArgs(*this);
    }
};

class MailService 
    : public IMailService, public EventSet, public EventListener
{
        SINGLETON_H(MailService)

public:
    static const char* EventMailRecv;
    MailService();
    
    virtual bool Config(std::string host, std::string username, std::string password, std::string localpath);
    virtual void Send(std::string des_address, std::string att_file);
    virtual void CheckSync();
    virtual void MailSyncHandle();
    virtual void StartCheckThread();
    virtual void StopCheckThread();
    bool OnAccountLogInOut(const EventArgs& args);
    std::string GetMailCheckInfo() const;
    bool SetMailCheckText(const std::string& text);
    bool IsCheckOver(const std::string& text) const;
    bool CheckPythonExist(string& text);

    virtual ~MailService();
    static void ClearMailCheckText(void*);   
    static void* CheckSyncThread(void * pUseless);    

private:
    std::string m_host;
    std::string m_username;
    std::string m_password;
    std::string m_localpath;
    std::string m_mailCheckText;
    pthread_t  m_pMailCheckThread;
};

#endif// __MAILSERVICE_H__

