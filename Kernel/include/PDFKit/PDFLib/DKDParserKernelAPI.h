//===========================================================================
// Summary:
//      DKDParserKernelAPI.h
//      Kernel 输出 API 入口
// Usage:
//      提供 KernelAPI 函数的入口，使用 Kernel 只需要包含这个文件即可。
//      在使用任何接口之前，首先要调用 DKP_Initialize 函数进行初始化，
//      使用完毕后调用 DKP_Destroy 函数卸载。
//      在 Linux 平台和 Mac 相关平台要定义 _LINUX 宏，并且在 Mac 平台
//      要额外定义_ MACOS 宏
// Remarks:
//      Null
// Date:
//      2011-9-16
// Author:
//      Liu Hongjia (liuhj@duokan.com)
//===========================================================================

#ifndef __KERNEL_PDFKIT_PDFLIB_DKDPARSERKERNELAPI_H__
#define __KERNEL_PDFKIT_PDFLIB_DKDPARSERKERNELAPI_H__

#include "KernelRetCode.h"

// 类型接口定义
#include "DKPTypeDef.h"

// 文档层相关接口类
#include "IDKPDoc.h"
#include "IDKPPage.h"
#include "IDKPOutline.h"
#include "IDKPPageDistiller.h"

#ifndef _LINUX
// 文档修改操作相关内容
class IDKPModifyDoc;
class IDKPProcDoc;
class IDKPWaterMarkDoc;
#endif

//-------------------------------------------
// Summary:
//      初始化DKDParserKernel。使用 DKDParserKernel 中的任何函数或类之前必须先调用本函数。
// Parameters:
//      [in] pResourceDir - DKP Kernel 资源所在路径，该路径下包含 DKP Kernel 所需的
//                          Resource 目录。
// Remarks:
//      pResourceDir 设置为空或不合法，则尝试选择程序模块所在目录(当前实现:Windows取DLL
//      所在目录，Linux等平台取可执行程序目录)。
// SDK Version:
//      从 DKP 1.0 开始支持
//-------------------------------------------
DKPKERNELAPI DKP_Initialize(const DK_WCHAR* pResourceDir = DK_NULL);

//-------------------------------------------
// Summary:
//      执行清理。在不再使用 DKDParserKernel 后调用。
// SDK Version:
//      从 DKP 1.0 开始支持
//-------------------------------------------
DKPKERNELAPI DKP_Destroy();

//-------------------------------------------
// Summary:
//      注册字体文件。
// Parameters:
//      [in] pFontFaceName - 字体的 Facename，用于做字体匹配。
//      [in] pFontFile     - 字体文件的绝对路径。
// SDK Version:
//      从 DKP 1.0 开始支持
//-------------------------------------------
DKPKERNELAPI DKP_RegisterFontFaceName(const DK_WCHAR* pFontFaceName, const DK_WCHAR* pFontFile);

//-------------------------------------------
// Summary:
//      获取和设置显示文本时是否做简繁转换，该选项对打开的所有文档有效。
// Parameters:
//      [in] bGBToBig5 - 是否做简繁转换。
// SDK Version:
//      从 DKP 1.0 开始支持
//-------------------------------------------
DKPKERNELAPI DKP_GetGBToBig5(DK_BOOL &gbToBig5);
DKPKERNELAPI DKP_SetGBToBig5(DK_BOOL gbToBig5);

//-------------------------------------------
// Summary:
//      是否使用颜色转换表，如果不调用改函数，默认使用。
// Parameters:
//      [in] useColorCMS - 是否使用颜色转换表。
// SDK Version:
//      从 DKP 1.0 开始支持
//-------------------------------------------
DKPKERNELAPI DKP_SetUseColorCMS(DK_BOOL useColorCMS);

//-------------------------------------------
// Summary:
//		设置是否使用图书原有风格，包括行间距、段间距、行首缩进、空格策略
// Parameters:
//		[in] bUseBookStyle      - 是否使用图书原有风格。
//  Return Value:
//		Null
//  Remarks:
//      Null
//  Availability:
//		从 PDFLib 2.6.0 开始支持。
//-------------------------------------------
DKPKERNELAPI DKP_SetUseBookStyle(DK_BOOL bUseBookStyle);

//-------------------------------------------
// Summary:
//      创建文档对象。
// Parameters:
//      [out] pDoc - 通过 DKP_CreateDoc 创建的 Doc 对象。
// SDK Version:
//      从 DKP 1.0 开始支持
//-------------------------------------------
DKPKERNELAPI DKP_CreateDoc(IDKPDoc*& pDoc);

//-------------------------------------------
// Summary:
//      销毁文档对象。
// Parameters:
//      [out] pDoc - 通过 DKP_CreateDoc 创建的 Doc 对象。
// SDK Version:
//      从 DKP 1.0 开始支持
//-------------------------------------------
DKPKERNELAPI DKP_DestroyDoc(IDKPDoc*& pDoc);

#endif // #ifndef __KERNEL_PDFKIT_PDFLIB_DKDPARSERKERNELAPI_H__
