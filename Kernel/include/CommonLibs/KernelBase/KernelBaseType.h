//===========================================================================
// Summary:
//		KernelBaseType.h
// Usage:
//		KernelBase库基本头文件，基本数据类型定义
//			...
// Remarks:
//		Null
// Date:
//		2011-09-16
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_KERNELBASE_KERNELBASETYPE_H__
#define	__KERNEL_COMMONLIBS_KERNELBASE_KERNELBASETYPE_H__

//===========================================================================

#include <cstddef>
#include <climits>
#include <cmath>
#include <string>
#include <iostream>

//===========================================================================

#ifdef _ANDROID_LINUX
    #define USE_DK_WIDE_CHAR
#else
    #ifdef USE_DK_WIDE_CHAR
        #undef USE_DK_WIDE_CHAR
    #endif
#endif

//===========================================================================
// 通用类型

typedef void			DK_VOID;

typedef char			DK_CHAR;
typedef unsigned char	DK_BYTE, DK_UCHAR;

typedef short			DK_SHORT;
typedef unsigned short	DK_USHORT;

typedef int				DK_INT;
typedef unsigned int	DK_UINT;

typedef long			DK_LONG;
typedef unsigned long	DK_ULONG;

typedef long        	DK_OFFSET;

typedef size_t			DK_SIZE_T;

typedef long long		DK_INT64;
typedef unsigned long long	DK_UINT64;

typedef float			DK_FLOAT;
typedef double			DK_DOUBLE;
typedef float			DK_REAL;

typedef bool			DK_BOOL;

const DK_CHAR DK_CHAR_MIN = SCHAR_MIN;
const DK_CHAR DK_CHAR_MAX = SCHAR_MAX;
const DK_SHORT DK_SHRT_MIN = SHRT_MIN;
const DK_SHORT DK_SHRT_MAX = SHRT_MIN;
const DK_USHORT DK_USHRT_MAX = USHRT_MAX;
const DK_INT DK_INT_MIN = INT_MIN;
const DK_INT DK_INT_MAX = INT_MAX;
const DK_UINT DK_UINT_MAX = INT_MAX;
const DK_UINT64 DK_MAXUINT64 = ((DK_UINT64)~((DK_UINT64)0));

typedef DK_CHAR DK_INT8;
typedef DK_UCHAR DK_UINT8;
typedef DK_SHORT DK_INT16;
typedef DK_USHORT DK_UINT16;
typedef DK_INT DK_INT32;
typedef DK_UINT DK_UINT32;

const DK_UINT16 DK_MAXUINT16 = ((DK_UINT16)~((DK_UINT16)0));
const DK_UINT32 DK_MAXUINT32 = ((DK_UINT32)~((DK_UINT32)0));

typedef DK_UINT16 	DK_WORD;
#ifdef _WIN32 
typedef unsigned long DK_DWORD;
#else
typedef DK_UINT32 DK_DWORD;
#endif
#ifdef _WIN32
typedef __int64 DK_INT64;
typedef unsigned __int64 DK_UINT64;
#endif

#ifdef USE_DK_WIDE_CHAR
typedef DK_INT32           DK_NATIVE_WCHAR;
#else
typedef wchar_t           DK_NATIVE_WCHAR;
#endif

#ifdef USE_DK_WIDE_CHAR
typedef DK_NATIVE_WCHAR			DK_WCHAR;
#else
typedef wchar_t     	DK_WCHAR;
#endif

typedef DK_INT64 DK_FILESIZE_T;

typedef  std::string DK_STRING;
#ifdef USE_DK_WIDE_CHAR
typedef std::basic_string<DK_WCHAR, std::char_traits<DK_WCHAR>, std::allocator<DK_WCHAR> > DK_WSTRING;
#else
typedef  std::wstring DK_WSTRING;
#endif

//===========================================================================
// const definition

#define DK_NULL			0
#define DK_TRUE			true
#define DK_FALSE		false

//===========================================================================
// float related function

const float DK_FLOAT_DELTA = 0.0001f;

template <typename T>
inline DK_BOOL DkFloatEqual(T a, T b)
{
    return fabs(a - b) <= DK_FLOAT_DELTA;
}

template <typename T>
inline DK_BOOL DkFloatEqualToZero(T v)
{
    return -DK_FLOAT_DELTA < v && v  < DK_FLOAT_DELTA;
}

// 任意值裁剪到BYTE取值范围内
template<class T> DK_BYTE DkClipToByte(T value) { return ((value < 0) ? 0 : ((value > 255) ? 255 : (DK_BYTE)value)); }

//===========================================================================
// DK数值反写相关

const unsigned int DK_MAX_F2A_PRECISION = 6u;

DK_VOID FastDouble2String(DK_DOUBLE dValue, DK_CHAR* pBuf, DK_SIZE_T bufSize, DK_UINT precision);

DK_VOID FastDouble2String(DK_DOUBLE dValue, DK_WCHAR* pBuf, DK_SIZE_T bufSize, DK_UINT precision);

//--------------------------------------------------------------------
// Summary:
// 		快速的浮点值到字符串转换函数
// Parameters:
// 		[in] dValue - 浮点数值
// 		[in] pBuf - 转换结果输出缓冲区
// 		[in] bufSize - 缓冲区字符个数
// 		[in] precision - 精度，转换后保留的小数位数
// Remarks:
//		如果缓冲区长度不足以容纳浮点值的整数部分，pBuf将返回空字符串。
//		转换结果小数部分末尾的0会被忽略，如果小数部分全是0将不输出小数点。
//--------------------------------------------------------------------
template<typename Elem>
DK_VOID FastF2A(DK_DOUBLE dValue, Elem* pBuf, DK_SIZE_T bufSize, DK_UINT precision = DK_MAX_F2A_PRECISION);

#endif
