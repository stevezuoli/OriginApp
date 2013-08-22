//===========================================================================
// Summary:
//		DKTError.h
// Usage:
//	    txt lib错误码定义
// Remarks:
//		Null
// Date:
//		2011-11-14
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================
#ifndef __KERNEL_TXTKIT_TXTLIB_DKTERROR_H__
#define __KERNEL_TXTKIT_TXTLIB_DKTERROR_H__

#include "KernelRetCode.h"

#define DKR_TXT_OPENFILE_ERR (DKR_TXT_BASEVALUE + 1L)
#define DKR_TXT_READFILE_ERR (DKR_TXT_BASEVALUE + 2L)
#define DKR_TXT_TYPOGRAPHY_ERR (DKR_TXT_BASEVALUE + 3L)
#define DKR_TXT_UNINIT_ERR (DKR_TXT_BASEVALUE + 4L)
#define DKR_TXT_ENCODING_CONVERT_ERR (DKR_TXT_BASEVALUE + 5L)
#define DKR_TXT_FINDSTR_TOO_LONG_ERR (DKR_TXT_BASEVALUE + 6L)
#define DKR_TXT_FINDSNIPPET_TOO_LONG_ERR (DKR_TXT_BASEVALUE + 7L)

#endif
