//===========================================================================
// Summary:
//     FilterRetCode.h
// Usage:
//     关于Filter错误信息的定义。
// Remarks:
//     Null
// Date:
//     2011-09-16
// Author:
//     Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_FILTERRETCODE_H__
#define __KERNEL_COMMONLIBS_FILTERRETCODE_H__

#include "KernelRetCode.h"

//===========================================================================
// typedef long DK_ReturnCode;
// #define DKR_FILTER_BASEVALUE        6000L   // Filter返回值
// #define DKR_FILTER_MAXVALUE         6999L
//===========================================================================

#define DKR_FILTER_NOT_SUPPORT          DKR_FILTER_BASEVALUE       // 未支持的功能
#define DKR_FILTER_NOT_INIT            (DKR_FILTER_BASEVALUE + 1L) // 未初始化
#define DKR_FILTER_ALREADY_INIT        (DKR_FILTER_BASEVALUE + 2L) // 已初始化

#endif // __KERNEL_COMMONLIBS_FILTERRETCODE_H__
