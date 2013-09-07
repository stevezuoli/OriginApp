#ifndef __DK_KSS_MASTER_H__
#define __DK_KSS_MASTER_H__

#include "LibKss/ILibKss/IKssMaster.h"
#include "LibKss/CrossPlatformConfig.h"
#include "LibCurlHttp/LibCurlHttpBase.h"
#include "LibKss/LibKssStruct.h"
#include "Core/InternalStruct.h"

using namespace KSSMaster;

//-------------------------------------------------------------------------

namespace DKKSSMaster
{
    struct BlockMetaList
        : public IBlockMetaList
    {
        KSS::BlockMetaList m_list;

        UInt32      count();
        const char* blockMeta(UInt32 index);
        bool        existed(UInt32 index);
        const char* commitMeta(UInt32 index);
        void        release();
    };

    struct BlockDownloadInfoList
        : public IBlockDownloadInfoList
    {
        KSS::BlockDownloadInfoList m_list;

        UInt32      count();
        const char* SHA1(UInt32 index);
        UInt32      size(UInt32 index);
        UInt32      downloadUrlCount(UInt32 index);
        const char* downloadUrl(UInt32 index, UInt32 urlIndex);
        void        release();
    };

    struct OutputString
        : public IOutputString
    {
        std::string m_str;

        const char* c_str();
        void        release();
    };

    struct StringList
        : public IStringList
    {
        std::vector<std::string> m_list;

        UInt32      count();
        const char* item(UInt32 index);
        void        release();
    };

    struct CKSSMaster
        : public IKSSMaster
    {
        ErrorNo requestUpload(
            // IN
            LibcURL::CancelDelegate         cancel,
            const KSS::BlockUploadInfoList& blockInfos,
            const UInt32                    timeoutSec,
            // OUT
            //IOutputString*&                 storeID,
            IStringList*&                   serverNodes,
            IOutputString*&                 secureKey,
            IOutputString*&                 fileMeta,
            IBlockMetaList*&                blockMetas);

        //ErrorNo commitUpload(
        //    // IN
        //    LibcURL::CancelDelegate         cancel,
        //    const std::string&              fileMeta,
        //    const std::vector<std::string>& commitMetas,
        //    const UInt32                    timeoutSec,
        //    // OUT
        //    IOutputString*&                 storeID);

        ErrorNo requestDownload(
            // IN
            LibcURL::CancelDelegate         cancel,
            const std::string&              storeID,
            const UInt32                    timeoutSec,
            // OUT
            IBlockDownloadInfoList*&        blockInfos,
            IOutputString*&                 secureKey);

        bool setupForUpload(const std::string& requestUploadInfo);
        bool setupForDownload(const std::string& requestDownloadInfo);

        private:
            std::string m_requestUploadInfo;
            std::string m_requestDownloadInfo;
    }; 
}

//--------------------------------------------------------------------------
#endif /* __DKKSSMASTER_H__ */
