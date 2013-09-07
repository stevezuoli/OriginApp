/* -------------------------------------------------------------------------
//  File Name   :   LibKssStruct.h
//  Author      :   Zhang Fan
//  Create Time :   2013-3-18 16:30:36
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __LIBKSSSTRUCT_H__
#define __LIBKSSSTRUCT_H__

#include "CrossPlatformConfig.h"
#include "LibKssEnum.h"

#include <string>
#include <stddef.h>
#include <vector>

//-------------------------------------------------------------------------

namespace KSS
{
    struct ProxyInfo
    {
        EnumKssProxyMode    m_mode;
        std::string         m_ip;
        UInt32              m_port;
        bool                m_havepwd;
        std::string         m_user;
        std::string         m_passwd;

        ProxyInfo()
            : m_mode(XLPM_NoProxy)
            , m_port(0)
            , m_havepwd(false)
        { }

        void reset()
        {
            m_mode      = XLPM_NoProxy;
            m_ip        = "";
            m_port      = 0;
            m_havepwd   = false;
            m_user      = "";
            m_passwd    = "";
        }
    };

    struct BlockUploadInfo
    {
        std::string SHA1;
        std::string MD5;
        UInt32      size;

        BlockUploadInfo()
            : size(0)
        { }
    };

    typedef std::vector<BlockUploadInfo> BlockUploadInfoList;
}
//--------------------------------------------------------------------------
#endif /* __LIBKSSSTRUCT_H__ */
