#ifndef __IXIAOMI_SERVICE_H__
#define __IXIAOMI_SERVICE_H__

#include <string>
#include <map>
#include "DownloadManager/DownloadTask.h"

namespace xiaomi
{
class IXiaoMiService
{
public:
    IXiaoMiService()
    {
    }
    virtual ~IXiaoMiService()
    {
    }

    std::map<std::string, std::string> GetCookies() const
    {
        return m_cookies;
    }

    std::string GetSecurity() const
    {
        return m_security;
    }

    std::string GetUserID() const
    {
        std::map<std::string, std::string>::const_iterator iter = m_cookies.find("userId");
        if (iter != m_cookies.end())
        {
            return iter->second;
        }

        return "";
    }

    virtual bool Init() = 0;
    virtual bool Destory() = 0;
protected:
    virtual bool CheckIfHttpErrorExists(dk::DownloadTask* task);
    virtual bool IsPassportValid()
    {
        return !m_security.empty() && !m_cookies.empty();
    }

protected:
    std::map<std::string ,std::string> m_cookies;
    std::string m_security;
};//IXiaoMiService
}//xiaomi

#endif//__IXIAOMI_SERVICE_H__
