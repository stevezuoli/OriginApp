//===========================================================================
// Summary:
//		ContainerRetCode.h
// Usage:
//		关于Container错误信息的定义。
//			...
// Remarks:
//		Null
// Date:
//		2011-09-16
// Author:
//		Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_CONTAINERRETCODE_H__
#define __KERNEL_COMMONLIBS_CONTAINERRETCODE_H__

#include "KernelRetCode.h"

//===========================================================================
// typedef long DK_ReturnCode;
// #define DKR_CONTAINER_BASEVALUE        5000L	// Container返回值
// #define DKR_CONTAINER_MAXVALUE         5499L
//===========================================================================

#define DKR_CONTAINER_NOT_SUPPORT DKR_CONTAINER_BASEVALUE // 未支持的功能

#endif	// __KERNEL_COMMONLIBS_CONTAINERRETCODE_H__
