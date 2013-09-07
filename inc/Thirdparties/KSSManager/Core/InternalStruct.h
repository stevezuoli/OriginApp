/* -------------------------------------------------------------------------
//  File Name   :   InternalStruct.h
//  Author      :   Zhang Fan
//  Create Time :   2013-3-18 18:22:47
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __INTERNALSTRUCT_H__
#define __INTERNALSTRUCT_H__

#include "../LibKss/CrossPlatformConfig.h"

//-------------------------------------------------------------------------

namespace KSS
{
    struct BlockMeta
    {
        std::string blockMeta;

        bool        existed;
        std::string commitMeta;
    };
    typedef std::vector<BlockMeta> BlockMetaList;

    struct BlockDownloadInfo
    {
        std::string                 SHA1;
        UInt32                      size;
        std::vector<std::string>    urls;
    };

    typedef std::vector<BlockDownloadInfo> BlockDownloadInfoList;
}

//--------------------------------------------------------------------------
#endif /* __INTERNALSTRUCT_H__ */
