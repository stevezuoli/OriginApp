/* -------------------------------------------------------------------------
//  File Name   :   RemoteFS.h
//  Author      :   Zhang Fan
//  Create Time :   2012-3-23 14:11:46
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __REMOTEFS_H__
#define __REMOTEFS_H__

#include "../../../LibKss/CrossPlatformConfig.h"
#include "../../../LibKss/LibKssEnum.h"
#include "../../../LibKss/LibKssError.h"
#include "../../../LibKss/LibKssStruct.h"
#include "../../../LibCurlHttp/LibCurlHttpBase.h"
#include "../../../LibCurlHttp/HttpHlp.h"
#include "../../../JsonCpp/json.h"
#include "../../../LibCurlHttp/FileProgressDelegate.h"
#include "../../InternalStruct.h"

//-------------------------------------------------------------------------

class CurlAutoInitGlobal
{
public:
    CurlAutoInitGlobal();
    ~CurlAutoInitGlobal();
};

////////////////////////////////////////////////////////////////////////////
//
class CRemoteFS
{
public:
    ErrorNo requestUpload(
        // IN
        LibcURL::CancelDelegate     cancel,
        const std::string&          customerID,
        const std::string&          clientIP,
        const std::string&          kssMasterAddr,
        const KSS::BlockUploadInfoList&   blockInfos,
        const UInt32                timeoutSec,
        // OUT
        std::string&                storeID,
        std::vector<std::string>&   serverNodes,
        std::string&                secureKey,
        std::string&                fileMeta,
        KSS::BlockMetaList&         blockMetas);

    ErrorNo commitUpload(
        // IN
        LibcURL::CancelDelegate         cancel,
        const std::string&              customerID,
        const std::string&              kssMasterAddr,
        const std::string&              fileMeta,
        const std::vector<std::string>& commitMetas,
        const UInt32                    timeoutSec,
        // OUT
        std::string&                    storeID);

    ErrorNo requestDownload(
        // IN
        LibcURL::CancelDelegate         cancel,
        const std::string&              customerID,
        const std::string&              kssMasterAddr,
        const std::string&              clientIP,
        const std::string&              storeID,
        const UInt32                    timeoutSec,
        // OUT
        KSS::BlockDownloadInfoList&     blockInfos,
        std::string&                    secureKey);

    ErrorNo deleteFile(
        // IN
        LibcURL::CancelDelegate         cancel,
        const std::string&              customerID,
        const std::string&              kssMasterAddr,
        const std::string&              storeID,
        UInt32                          timeoutSec);

    ErrorNo uploadBlock(
        // IN
        LibcURL::CancelDelegate     cancel,
        FileProgressDelegate        progress,
        const std::string&          serverNode,
        const std::string&          fileMeta,
        const std::string&          blockMeta,
        const std::string&          encryptedBlockPath,
        const std::string&          encryptedBlockSum,
        // OUT
        std::string&                commitMeta);

    ErrorNo downloadBlock(
        // IN
        LibcURL::CancelDelegate         cancel,
        FileProgressDelegate            progress,
        const std::string               url,
        const std::string&              rawBlockPath);

private:
    inline ErrorNo _requestJSON(
        LibcURL::CancelDelegate cancel, 
        const std::string&      url, 
        const Json::Value&      send, 
        const UInt32            timeoutSec,
        Json::Value&            result);
};

//--------------------------------------------------------------------------
#endif /* __REMOTEFS_H__ */
