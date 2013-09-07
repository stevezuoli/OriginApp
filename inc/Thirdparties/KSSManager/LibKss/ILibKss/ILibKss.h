/* -------------------------------------------------------------------------
//  File Name   :   ILibKss.h
//  Author      :   Zhang Fan
//  Create Time :   2013-3-18 16:27:30
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __ILIBKSS_H__
#define __ILIBKSS_H__

#include "../CrossPlatformConfig.h"
#include "IKssMaster.h"
#include "../../LibCurlHttp/LibCurlHttpBase.h"
#include "../../LibCurlHttp/FileProgressDelegate.h"

//---------------------------------------------------------------------------

KSS_INTERFACE KSS_API ILibKss
{
    // config KSS master
    virtual void setKssMaster(KSSMaster::IKSSMaster* kssMaster) = 0;

    // config environment param, input string must UTF8 encoded
    virtual void setTempFolder(const std::string& folder)       = 0;
    virtual void setProxyInfo(KSS::ProxyInfo proxyInfo)         = 0;

    // upload & download
    virtual ErrorNo uploadFile(LibcURL::CancelDelegate cancel, FileProgressDelegate progress, const std::string& filePath,
        std::string& fileMeta, std::vector<std::string>& commitMetas)            = 0;
    virtual ErrorNo downloadFile(LibcURL::CancelDelegate cancel, FileProgressDelegate progress, const std::string& filePath, const std::string& storeID)    = 0;
};

class KSS_API SingletonFactory
{
public:
    static ILibKss*	getLibKss();
    static void		destoryLibKss();

private:
    static ILibKss*   s_pLibKss;
};

//--------------------------------------------------------------------------
#endif /* __ILIBKSS_H__ */
