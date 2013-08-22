//===========================================================================
// Summary:
//     Filter Encode & Decode
// Usage:
//     Null
// Remarks:
//     Null
// Date:
//     2011-09-01
// Author:
//     Zhai Guanghe(zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_FILTER_DKFILTER_H__
#define __KERNEL_COMMONLIBS_FILTER_DKFILTER_H__

#include "FilterParameter.h"
#include "FilterRetCode.h"

//-------------------------------------------
// Summary:
//     读取 Encode & Decode 输入数据的回调函数。
// Parameters:
//     [in]  pSrc     : 输入数据
//     [in]  pDest    : 读取数据指针
//     [in]  destLen  : 期望读取长度
//     [out] pReadLen : 实际读取长度
// ReturnValue:
//     DKR_OK : 读取到了非零长度的数据
//     others : 失败
//-------------------------------------------
typedef DK_ReturnCode (*FilterReadFun)(DK_VOID *pSrc, DK_VOID *pDest, DK_UINT destLen, DK_UINT *pReadLen);

typedef DK_BOOL (*FilterTellFun)(DK_VOID *pSrc, DK_UINT *pOffset);
typedef DK_BOOL (*FilterSeekFun)(DK_VOID *pSrc, DK_UINT offset);

class IDkDecode
{
public:
    virtual ~IDkDecode(){}

    //-------------------------------------------
    // Summary:
    //     初始化Decode接口并分配内存。
    // Parameters:
    //     [in] pSrc        : 输入数据
    //     [in] funRead     : Callback 输入数据读取函数
    //     [in] filterParam : Decode 参数
    //-------------------------------------------
    virtual DK_ReturnCode Initialize(DK_VOID *pSrc, FilterReadFun funRead, const FilterParam *pFilterParam) = 0;

    //-------------------------------------------
    // Summary:
    //     估算Decode数据的长度上界等信息。
    // Parameters:
    //     [in]  srcLen      : 输入数据的长度或长度上界
    //     [out] pFilterInfo : Decode数据的信息
    // ReturnValue:
    //     估算的上界，返回0表示估算失败
    //-------------------------------------------
    virtual DK_ReturnCode GetFilterInfo(DK_UINT srcLen, FilterInfo *pFilterInfo) = 0;

    //-------------------------------------------
    // Summary:
    //     Decode指定长度的数据。
    // Parameters:
    //     [in]  pDest    : 输出数据指针
    //     [in]  destLen  : 期望输出长度
    //     [out] pReadLen : 实际输出长度
    // Remarks:
    //     如果pReadLen传DK_NULL，
    //     只有在解压长度恰为destLen时返回成功
    //-------------------------------------------
    virtual DK_ReturnCode Decode(DK_BYTE *pDest, DK_UINT destLen, DK_UINT *pDecodeLen = DK_NULL) = 0;

    //-------------------------------------------
    // Summary:
    //     清理Decode接口并释放内存。
    //-------------------------------------------
    virtual DK_ReturnCode Destroy() = 0;
};

// 接口说明同上
class IDkEncode
{
public:
    virtual ~IDkEncode(){}
    virtual DK_ReturnCode Initialize(DK_VOID *pSrc, FilterReadFun funRead, const FilterParam *pFilterParam) = 0;
    virtual DK_ReturnCode GetFilterInfo(DK_UINT srcLen, FilterInfo *pFilterInfo) = 0;
    virtual DK_ReturnCode Encode(DK_BYTE *pDest, DK_UINT destLen, DK_UINT *pEncodeLen = DK_NULL) = 0;
    virtual DK_ReturnCode Destroy() = 0;
};

#endif // __KERNEL_COMMONLIBS_FILTER_DKFILTER_H__
