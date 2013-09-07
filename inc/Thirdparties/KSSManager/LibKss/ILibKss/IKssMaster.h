/* -------------------------------------------------------------------------
//  File Name   :   IKssMaster.h
//  Author      :   Zhang Fan
//  Create Time :   2013-4-10 22:21:05
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __IKSSMASTER_H__
#define __IKSSMASTER_H__

#include "../CrossPlatformConfig.h"
#include "../../LibCurlHttp/LibCurlHttpBase.h"
#include "../../LibCurlHttp/FileProgressDelegate.h"
#include "../LibKssError.h"
#include "../LibKssStruct.h"

//-------------------------------------------------------------------------

namespace KSSMaster
{
    KSS_INTERFACE IBlockMetaList
    {
        virtual UInt32      count()                     = 0;
        virtual const char* blockMeta(UInt32 index)     = 0;
        virtual bool        existed(UInt32 index)       = 0;
        virtual const char* commitMeta(UInt32 index)    = 0;

        virtual void        release()                   = 0;
    };

    KSS_INTERFACE IBlockDownloadInfoList
    {
        virtual UInt32      count()                                     = 0;
        virtual const char* SHA1(UInt32 index)                          = 0;
        virtual UInt32      size(UInt32 index)                          = 0;
        virtual UInt32      downloadUrlCount(UInt32 index)              = 0;
        virtual const char* downloadUrl(UInt32 index, UInt32 urlIndex)  = 0;

        virtual void        release() = 0;
    };

    KSS_INTERFACE IOutputString
    {
        virtual const char* c_str()   = 0;
        virtual void        release() = 0;
    };

    KSS_INTERFACE IStringList
    {
        virtual UInt32      count()            = 0;
        virtual const char* item(UInt32 index) = 0;

        virtual void        release()          = 0;
    };

    KSS_INTERFACE IKSSMaster
    {
        virtual ErrorNo requestUpload(
            // IN
            LibcURL::CancelDelegate         cancel,
            const KSS::BlockUploadInfoList& blockInfos,
            const UInt32                    timeoutSec,
            // OUT
            //IOutputString*&                 storeID,
            IStringList*&                   serverNodes,
            IOutputString*&                 secureKey,
            IOutputString*&                 fileMeta,
            IBlockMetaList*&                blockMetas) = 0;

        //virtual ErrorNo commitUpload(
        //    // IN
        //    LibcURL::CancelDelegate         cancel,
        //    const std::string&              fileMeta,
        //    const std::vector<std::string>& commitMetas,
        //    const UInt32                    timeoutSec,
        //    // OUT
        //    IOutputString*&                 storeID) = 0;

        virtual ErrorNo requestDownload(
            // IN
            LibcURL::CancelDelegate         cancel,
            const std::string&              storeID,
            const UInt32                    timeoutSec,
            // OUT
            IBlockDownloadInfoList*&        blockInfos,
            IOutputString*&                 secureKey) = 0;
        bool setupForUpload(const std::string& requestUploadInfo);
        bool setupForDownload(const std::string& requestDownloadInfo);
    };
}

//--------------------------------------------------------------------------
#endif /* __IKSSMASTER_H__ */
