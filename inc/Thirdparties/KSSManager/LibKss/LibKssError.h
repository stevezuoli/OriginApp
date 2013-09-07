/* -------------------------------------------------------------------------
//  File Name   :   LibKssError.h
//  Author      :   Zhang Fan
//  Create Time :   2013-3-18 16:29:48
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __LIBKSSERROR_H__
#define __LIBKSSERROR_H__

#include "CrossPlatformConfig.h"

//-------------------------------------------------------------------------

// Category Mask
const UInt32  KSS_LOGIC_MASK	= 0x00000000U;
const UInt32  KSS_CURL_MASK		= 0x00010000U;
const UInt32  KSS_HTTP_MASK		= 0x00020000U;
const UInt32  KSS_LOCAL_FS_MASK = 0X00040000U;

// Error No
enum ErrorNo
{
    // basic
    KSS_OK                          = KSS_LOGIC_MASK,
    KSS_FAIL                        ,
    KSS_CURL_INTERERROR             ,
    KSS_HTTP_NORETURN               ,
    KSS_SERVER_UNKNOWN_ERROR        ,

    KSS_BLOCK_SHA1_CHECK_FAIL       ,

    // remote fs
    KSS_REMOTE_FILE_EXIST           ,
    KSS_BLOCK_COMPLETED             ,

    // local
    KSS_LOCAL_FS_ERROR               = KSS_LOCAL_FS_MASK,
};

#define IS_CURL_ERROR(err)                      (err & KSS_CURL_MASK)
#define IS_HTTP_ERROR(err)                      (err & KSS_HTTP_MASK)
#define IS_NET_ERROR(err)                       ((err & KSS_CURL_MASK) || (err & KSS_HTTP_MASK))
#define IS_LOCAL_FS_ERROR(err)                  (err & KSS_LOCAL_FS_MASK)

#define ERRORNO_TO_CURL(err)                    (err & (~KSS_CURL_MASK))
#define ERRORNO_TO_HTTP(err)                    (err & (~KSS_HTTP_MASK))
#define ERRORNO_TO_LOCAL_FS(err)                (err & (~KSS_LOCAL_FS_MASK))

//--------------------------------------------------------------------------
#endif /* __LIBKSSERROR_H__ */
