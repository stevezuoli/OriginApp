#ifndef __IMAILSERVICE_H__
#define __IMAILSERVICE_H__
#include <string>

class IMailService
{
public:
    virtual bool  Config(std::string host, std::string username, std::string password, std::string localpath) = 0;
    virtual void Send(std::string des_address, std::string att_file) = 0;
    virtual void CheckSync() = 0;
    virtual void MailSyncHandle()=0;
    virtual void StartCheckThread() = 0;
    virtual void StopCheckThread() = 0;

    virtual  ~IMailService(){};
};

#endif// __MAILSERVICE_H__
