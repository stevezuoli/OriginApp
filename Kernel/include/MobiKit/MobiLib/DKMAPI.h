//===========================================================================
//  Summary:
//      MobiLib公共接口。
//  Usage:
//      Null
//  Remarks:
//      Null
//  Date:
//      2011-09-15
//  Author:
//      Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_MOBIKIT_MOBILIB_EXPORT_DKMAPI_H__
#define __KERNEL_MOBIKIT_MOBILIB_EXPORT_DKMAPI_H__

#include "DKMDef.h"

//-------------------------------------------
//  Summary:
//      初始化MobiLib，使用MobiLib中的任何接口之前必须先调用本函数。
//  Parameters:
//      [in] pResourceDir - ePubLib资源所在路径，该路径下包含ePubLib所需的Resource目录。
//  Return Value:
//      如果成功则返回DK_TRUE，否则返回DK_FALSE。
//  Remarks:
//      pResourceDir设置为空或不合法，则尝试选择程序模块所在目录(当前实现:Windows取DLL所在目录，Linux等平台取可执行程序目录)。
//  Availability:
//      从MobiLib 1.0.8开始支持。
//-------------------------------------------
MOBIAPI DKM_Initialize(const DK_WCHAR *pResourceDir);

//-------------------------------------------
//  Summary:
//      执行清理操作。
//  Parameters:
//      Null
//  Return Value:
//      如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//      从MobiLib 1.0.8开始支持。
//-------------------------------------------
MOBIAPI DKM_Destroy();

//-------------------------------------------
//  Summary:
//        打开一个Mobi文档。
//    Parameters:
//        [in] pBookPath     - 文件路径。
//        [in] pWorkingDir   - Mobi文件工作路径，一般设为系统的临时目录，该目录需要有写权限。
//  Return Value:
//      如果成功返回Mobi文档对象句柄，否则返回 DK_NULL。
//    Remarks:
//        pWorkingDir设置为空或不合法，则尝试使用系统临时文件目录。
//  Availability:
//        从MobiLib 1.0.8开始支持。
//-------------------------------------------
MOBIAPI_(MOBIDOCHANDLE) DKM_OpenDocument(const DK_WCHAR *pBookPath, const DK_WCHAR *pWorkingDir);

//-------------------------------------------
//  Summary:
//        关闭一个Mobi文档。
//    Parameters:
//        [in] hFile    - Mobi文档对象句柄。
//  Return Value:
//      如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//        从MobiLib 1.0.8开始支持。
//-------------------------------------------
MOBIAPI DKM_CloseDocument(MOBIDOCHANDLE hFile);

//-------------------------------------------
//    Summary:
//        设置字体名字到字体文件的对应关系，调用该接口后将影响相应字体的显示。
//    Parameters:
//        [in] pFontFaceName     - 字体名字。
//        [in] pFontFilePath     - 字体文件的路径。
//  Return Value:
//        如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//        从MobiLib 1.0.8开始支持。
//-------------------------------------------
MOBIAPI DKM_RegisterFont(const DK_WCHAR *pFontFaceName, const DK_WCHAR *pFontFilePath);

//-------------------------------------------
// Summary:
//        设置显示使用的默认字体名字，对于不存在的字体，绘制时将使用该默认字体。
// Parameters:
//        [in] pDefaultFontFaceName       - 默认字体名字。
//        [in] charset                    - 字符集，参见 DK_CHARSET_TYPE 枚举值说明。
//  Return Value:
//        如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//        从MobiLib 1.0.8开始支持。
//-------------------------------------------
MOBIAPI DKM_SetDefaultFont(const DK_WCHAR *pDefaultFontFaceName, DK_CHARSET_TYPE charset);

//-------------------------------------------
// Summary:
//		设置是否使用图书原有风格，包括行间距、段间距、行首缩进。
// Parameters:
//		[in] bUseBookStyle      - 是否使用图书原有风格。
//  Return Value:
//		Null
//  Remarks:
//      Null
//  Availability:
//		从MobiLib 1.0 开始支持。
//-------------------------------------------
EPUBAPI_(void) DKM_SetUseBookStyle(DK_BOOL bUseBookStyle);

//===========================================================================

#endif // #__KERNEL_MOBIKIT_MOBILIB_EXPORT_DKMAPI_H__
