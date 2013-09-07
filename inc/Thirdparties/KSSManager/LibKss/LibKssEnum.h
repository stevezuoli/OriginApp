/* -------------------------------------------------------------------------
//  File Name   :   LibKssEnum.h
//  Author      :   Zhang Fan
//  Create Time :   2013-3-18 16:28:52
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __LIBKSSENUM_H__
#define __LIBKSSENUM_H__

//-------------------------------------------------------------------------

typedef enum tagEnumKssProxyMode
{
    XLPM_NoProxy              = 0,
    XLPM_UsingBrowserProxy,
    XLPM_ProxyHTTP,
    XLPM_ProxySock4,
    XLPM_ProxySock5
} EnumKssProxyMode;

//--------------------------------------------------------------------------
#endif /* __LIBKSSENUM_H__ */
