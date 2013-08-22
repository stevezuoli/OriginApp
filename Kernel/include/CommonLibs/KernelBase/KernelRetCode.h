//===========================================================================
// Summary:
//      KernelRetCode.h
// Usage:
//      关于错误信息的定义。
//          ...
// Remarks:
//      Null
// Date:
//      2011-09-15
// Author:
//      Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef __KERNELRETCODE_HEADERFILE__
#define __KERNELRETCODE_HEADERFILE__

//===========================================================================

//-------------------------------------------
//  Summary:
//      Kernel返回值。
//  Values:
//      0   - 成功。
//      <0  - 非法值
//      >0  - 错误码
//-------------------------------------------
typedef long DK_ReturnCode;

//===========================================================================
// 不同功能模块使用的返回值基数

#define DKR_PDF_BASEVALUE           1000L   // PDFLib返回值
#define DKR_PDF_MAXVALUE            1999L
#define DKR_EPUB_BASEVALUE          2000L   // ePubLib返回值
#define DKR_EPUB_MAXVALUE           2999L
#define DKR_TXT_BASEVALUE           3000L   // TxtLib返回值
#define DKR_TXT_MAXVALUE            3999L

#define DKR_CONTAINER_BASEVALUE     5000L   // Container返回值
#define DKR_CONTAINER_MAXVALUE      5499L
#define DKR_IO_BASEVALUE            5500L   // IOBase返回值
#define DKR_IO_MAXVALUE             5999L
#define DKR_FILTER_BASEVALUE        6000L   // Filter返回值
#define DKR_FILTER_MAXVALUE         6499L
#define DKR_XML_BASEVALUE           6500L   // xml返回值
#define DKR_XML_MAXVALUE            6999L

#define DKR_TEST_BASEVALUE          9000L   // Unit Test 返回值
#define DKR_TEST_MAXVALUE           9999L

//===========================通用返回值======================================
// 成功
#define DKR_OK                      0L
// 失败
#define DKR_FAILED                  1L
// 无法预知的失败
#define DKR_UNEXPECTED_ERROR        2L
// 没有实现
#define DKR_NOIMPLEMENT             3L
// 执行体未初始化或为空
#define DKR_UNINIT                  4L
// 无效传入参数
#define DKR_INVALIDINPARAM          5L
// 无效传出参数
#define DKR_INVALIDOUTPARAM         6L
// 不支持的操作
#define DKR_UNSUPPORTED             7L
// 分配内存失败
#define DKR_ALLOCTMEMORY_FAIL       8L
// 此版本不支持字体下载
#define DKR_NO_FONTDOWNLOAD         9L
// 未初始化内核
#define DKR_UNINITIALIZED           10L
// 权限认证失败
#define DKR_AUTHENTICATE_FAIL       50L
// 没有操作权限
#define DKR_NORIGHT                 51L

//===========================绘制结果状态使用返回值==============================

typedef long DKRS_RENDERSTATUS;
#define DKRS_OK                     0L
#define DKRS_MISSCHAR               1L

//===========================================================================

#endif  //#ifndef __KERNELRETCODE_HEADERFILE__
