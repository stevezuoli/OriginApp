//===========================================================================
// Summary:
//     IOBaseRetCode.h
// Usage:
//     关于IOBase错误信息的定义。
// Remarks:
//     Null
// Date:
//     2011-09-16
// Author:
//     Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_IOBASERETCODE_H__
#define __KERNEL_COMMONLIBS_IOBASERETCODE_H__

#include "KernelRetCode.h"

//===========================================================================
// typedef long DK_ReturnCode;
// #define DKR_IO_BASEVALUE               5500L // IOBase返回值
// #define DKR_IO_MAXVALUE                5999L
//===========================================================================

// 公用返回值
#define DKR_IO_STREAM_NOT_SUPPORT          DKR_IO_BASEVALUE          // 未支持的功能
#define DKR_IO_STREAM_ERR_MODE            (DKR_IO_BASEVALUE + 1L)    // 错误的打开模式
#define DKR_IO_STREAM_OPENED              (DKR_IO_BASEVALUE + 2L)    // 流已经打开
#define DKR_IO_STREAM_CLOSED              (DKR_IO_BASEVALUE + 3L)    // 流已经关闭
#define DKR_IO_STREAM_OPEN_FAIL           (DKR_IO_BASEVALUE + 4L)    // 流打开失败
#define DKR_IO_STREAM_ERR_SEEK_OFFSET     (DKR_IO_BASEVALUE + 5L)    // 非法的Seek位置
#define DKR_IO_STREAM_ERR_SEEK_ORIGIN     (DKR_IO_BASEVALUE + 6L)    // 非法的SeekOrigin
#define DKR_IO_STREAM_NOT_ENOUGH_DATA     (DKR_IO_BASEVALUE + 7L)    // 要求读取了过多数据
#define DKR_IO_STREAM_CANNOT_READ         (DKR_IO_BASEVALUE + 8L)    // 流不能读
#define DKR_IO_STREAM_CANNOT_WRITE        (DKR_IO_BASEVALUE + 9L)    // 流不能写

#endif // __KERNEL_COMMONLIBS_IOBASERETCODE_H__
