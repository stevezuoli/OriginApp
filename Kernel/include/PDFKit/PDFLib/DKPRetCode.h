//===========================================================================
// Summary:
//      DKPRetCode.h
// Usage:
//      关于错误信息的定义。
//      ...
// Remarks:
//      Null
// Date:
//      2011-9-28
// Author:
//      Liu Hongjia (liuhj@duokan.com)
//===========================================================================

#ifndef __KERNEL_PDFKIT_PDFLIB_DKPRETCODE_H__
#define __KERNEL_PDFKIT_PDFLIB_DKPRETCODE_H__

#include "KernelRetCode.h"

//===========================================================================
// typedef long DK_ReturnCode;
// #define DKR_PDF_BASEVALUE            1000L    // PDFLib返回值
// #define DKR_PDF_MAXVALUE             1999L
//===========================================================================

#define DKR_PDF_NOT_SUPPORT       DKR_PDF_BASEVALUE          // 未支持的功能
//===========================================================================
// 密码验证 1001L~1010L
//===========================================================================
#define DKR_PDF_OWNER_PASSWORD_NOT_SET (DKR_PDF_BASEVALUE + 1L)
#define DKR_PDF_USER_PASSWORD_NOT_SET  (DKR_PDF_BASEVALUE + 2L)
#define DKR_PDF_WRONG_REVISION         (DKR_PDF_BASEVALUE + 3L)
#define DKR_PDF_GET_BASE_KEY_FAIL      (DKR_PDF_BASEVALUE + 4L)
#define DKR_PDF_RC4_CRYPT_FAIL         (DKR_PDF_BASEVALUE + 5L)
#define DKR_PDF_AES_CBC_CRYPT_FAIL     (DKR_PDF_BASEVALUE + 6L)
#define DKR_PDF_MD5_HASH_FAIL          (DKR_PDF_BASEVALUE + 7L)
#define DKR_PDF_SHA_HASH_FAIL          (DKR_PDF_BASEVALUE + 7L)
#define DKR_PDF_GET_USER_PASSWORD_FAIL (DKR_PDF_BASEVALUE + 9L)
#define DKR_PDF_GET_KEY_FAIL           (DKR_PDF_BASEVALUE + 10L)

//===========================================================================
// 页面重排
//===========================================================================
#define DKR_PDF_FAILED_TO_FILL_PREV_PAGE    (DKR_PDF_BASEVALUE + 11L)

//===========================================================================
// OpenDoc
//===========================================================================
#define DKR_PDF_NEED_READ_PASSWORD          (DKR_PDF_BASEVALUE + 12L)
#define DKR_PDF_READ_PASSWORD_INCORRECT     (DKR_PDF_BASEVALUE + 13L)

#endif // #ifndef __KERNEL_PDFKIT_PDFLIB_DKPRETCODE_H__
