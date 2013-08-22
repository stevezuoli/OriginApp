//===========================================================================
// Summary:
//		ePubLib公共接口。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-09-15
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_DKEAPI_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_DKEAPI_H__

//===========================================================================

#include "DKEDef.h"
#include "DKEEPUBDef.h"
#include "DKEError.h"
#include "IDKEFootnoteIterator.h"
#include "IDKEGalleryIterator.h"
#include "IDKEHitableObjIterator.h"
#include "IDKEImageIterator.h"
#include "IDKEMediaIterator.h"
#include "IDKEPreBlockIterator.h"
#include "IDKETextIterator.h"
#include "IDKEImgBlockIterator.h"
#include "IDKEGallery.h"
#include "IDKEMultiCallout.h"
#include "IDKEBook.h"
#include "IDKEComicsFrame.h"
#include "IDKEComicsFrameIterator.h"
#include "IDKEFlexPage.h"
#include "IDKEPage.h"
#include "IDKEPageUnit.h"
#include "IDKESection.h"
#include "IDKETocPoint.h"

class IDkStream;

//===========================================================================

//-------------------------------------------
//	Summary:
//		初始化ePubLib，使用ePubLib中的任何接口之前必须先调用本函数。
//	Parameters:
//	    [in] szResourceDir - ePubLib资源所在路径，该路径下包含ePubLib所需的Resource目录。
//  Return Value:
//      如果成功则返回DK_TRUE，否则返回DK_FALSE。
//	Remarks:
//		szResourceDi r设置为空或不合法，则尝试选择程序模块所在目录(当前实现:Windows取DLL所在目录，Linux等平台取可执行程序目录)。
//  Availability:
//		从ePubLib 1.0 开始支持。
//-------------------------------------------
EPUBAPI DKE_Initialize(const DK_WCHAR* szResourceDir);

//-------------------------------------------
//	Summary:
//		执行清理操作。
//	Parameters:
//      Null
//  Return Value:
//      如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//		从ePubLib 1.0 开始支持。
//-------------------------------------------
EPUBAPI DKE_Destroy();

//-------------------------------------------
//  Summary:
//		打开一个 ePub 文档。
//	Parameters:
//		[in] szBookPath     - 文件路径。
//      [in] containerType  - ePub容器类型。
//	    [in] szWorkingDir   - ePub文件工作路径，一般设为系统的临时目录，该目录需要有写权限。
//  Return Value:
//      如果成功返回 ePub 文档对象句柄，否则返回 DK_NULL。
//	Remarks:
//		szWorkingDir 设置为空或不合法，则尝试使用系统临时文件目录。
//  Availability:
//		从 ePubLib 1.0 开始支持。
//-------------------------------------------
EPUBAPI_(EPUBDOCHANDLE) DKE_OpenDocument(const DK_WCHAR* szBookPath,
										 DKE_CONTAINER_TYPE containerType,
										 const DK_WCHAR* szWorkingDir);

//-------------------------------------------
//  Summary:
//		打开一个加密的 ePub 文档。
//	Parameters:
//		[in] szBookPath     - 文件路径。
//      [in] containerType  - ePub容器类型。
//	    [in] szWorkingDir   - ePub文件工作路径，一般设为系统的临时目录，该目录需要有写权限。
//      [in] contentKey     - 内容加密密钥。
//  Return Value:
//      如果成功返回 ePub 文档对象句柄，否则返回 DK_NULL。
//	Remarks:
//		szWorkingDir 设置为空或不合法，则尝试使用系统临时文件目录。
//      不论contentKey是否为空都走DKE_OpenDocument的流程。
//  Availability:
//		从 ePubLib 1.0 开始支持。
//-------------------------------------------
EPUBAPI_(EPUBDOCHANDLE) DKE_OpenDRMDocument(const DK_WCHAR* szBookPath,
                                            DKE_CONTAINER_TYPE containerType,
                                            const DK_WCHAR* szWorkingDir,
                                            const DKE_DRMKEY* contentKey);


//-------------------------------------------
//  Summary:
//		通过结构化数据打开ePub文档
//	Parameters:
//		[in] szBookPath     - 文件路径。
//      [in] containerType  - ePub容器类型。
//	    [in] szWorkingDir   - ePub文件工作路径，一般设为系统的临时目录，该目录需要有写权限。
//      [in] opfData        - OPF数据
//      [in] pTocData       - toc的XML文件文本，必须UTF8编码
//      [in] encData        - 加密数据
//      [in] extData        - 多看扩展数据
//  Return Value:
//      如果成功返回 ePub 文档对象句柄，否则返回 DK_NULL。
//	Remarks:
//		szWorkingDir 设置为空或不合法，则尝试使用系统临时文件目录。
//  Availability:
//		从 ePubLib 1.0 开始支持。
//-------------------------------------------
EPUBAPI_(EPUBDOCHANDLE) DKE_OpenDocumentByStructData(const DK_WCHAR* szBookPath, 
                                                    DKE_CONTAINER_TYPE containerType, 
                                                    const DK_WCHAR* szWorkingDir,
                                                    DKE_PACK_MODE packMode,
                                                    const DKEOPFData& opfData,
                                                    const DK_CHAR* pTocData,
                                                    const DKEEncryptionData& encData,
                                                    const DKEExtensionData& extData);

//-------------------------------------------
//  Summary:
//		关闭一个 ePub 文档。
//	Parameters:
//		[in] hFile	- ePub 文档对象句柄。
//  Return Value:
//      如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//		从ePubLib 1.0 开始支持。
//-------------------------------------------
EPUBAPI DKE_CloseDocument(EPUBDOCHANDLE hFile);

#if DKE_SUPPORT_HTML
//-------------------------------------------
//  Summary:
//		打开一个 html 文档。
//	Parameters:
//		[in] szBookPath     - 文件路径。
//	    [in] szWorkingDir   - ePub文件工作路径，一般设为系统的临时目录，该目录需要有写权限。
//  Return Value:
//      如果成功返回 html 文档对象句柄，否则返回 DK_NULL。
//	Remarks:
//		szWorkingDir 设置为空或不合法，则尝试使用系统临时文件目录。
//  Availability:
//		从 ePubLib 1.1.1 开始支持。
//-------------------------------------------
EPUBAPI_(HTMLDOCHANDLE) DKE_OpenHTMLDocument(const DK_WCHAR* szBookPath,
										 const DK_WCHAR* szWorkingDir);
#endif

//-------------------------------------------
//  Summary:
//		打开一个HTML流片段
//	Parameters:
//		[in] pHTMLStream    - HTML流片段
//	    [in] pHTMLPath      - HTML的基础路径，用于获取HTML中链接的CSS，图像等链接数据
//      [in] showType       - 数据展现方式，包括翻页模式和滚屏模式
//  Return Value:
//      如果成功返回改HTML片段对应的展现句柄，否则返回 DK_NULL。
//	Remarks:
//		如果不设置pHTMLPath，则片段中链接的外部资源数据无效。
//      需要根据showType使用DKE_SHOWCONTENT_HANDLE中的返回值
//  Availability:
//		从 ePubLib 2.3.0 开始支持。
//-------------------------------------------
EPUBAPI_(DKE_SHOWCONTENT_HANDLE) DKE_OpenHTMLFragment(IDkStream* pHTMLStream, 
                                                    const DK_WCHAR* pHTMLPath,
                                                    DKE_HTMLSTYLE_TYPE htmlStyle,
                                                    DKE_SHOWCONTENT_TYPE showType);

//-------------------------------------------
//  Summary:
//		关闭一个HTML片段的句柄
//	Parameters:
//		[in] pHandle    - HTML流片段对应的解析后的句柄
//  Return Value:
//      如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//	Remarks:
//		Null
//  Availability:
//		从 ePubLib 2.3.0 开始支持。
//-------------------------------------------
EPUBAPI DKE_CloseHTMLFragment(DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE handle);


//-------------------------------------------
//	Summary:
//		设置字体名字到字体文件的对应关系，调用该接口后将影响相应字体的显示。
//	Parameters:
//		[in] szFontFaceName     - 字体名字。
//		[in] szFontFile         - 字体文件的路径。
//  Return Value:
//		如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//		从ePubLib 1.0 开始支持。
//-------------------------------------------
EPUBAPI DKE_RegisterFont(const DK_WCHAR* szFontFaceName, const DK_WCHAR* szFontFilePath);

//-------------------------------------------
//	Summary:
//		注册字体族。
//	Parameters:
//		[in] pFontFamily     - 字体族。
//		[in] nCount          - 字体族数。
//  Return Value:
//		如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//		从ePubLib 2.1.1 开始支持。
//-------------------------------------------
EPUBAPI DKE_RegisterFontFamily(DK_FONTFAMILY* pFontFamily, DK_INT nCount);

//-------------------------------------------
//	Summary:
//		反注册字体名
//	Parameters:
//		[in] szFontFaceName     - 字体名字。
//  Return Value:
//		如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//  Remarks:
//      如果该 face name 在接口调用之前是默认字体，则该 face name 对应的默认字体设置将失效，
//      需要设置新的默认字体
//  Availability:
//		从ePubLib 2.1.1 开始支持。
//-------------------------------------------
EPUBAPI DKE_UnregisterFont(const DK_WCHAR* szFontFaceName);

//-------------------------------------------
// Summary:
//		设置显示使用的默认字体名字，对于不存在的字体，绘制时将使用该默认字体。
// Parameters:
//		[in] szDefaultFontFaceName      - 默认字体名字。
//		[in] charset                    - 字符集，参见 DK_CHARSET_TYPE 枚举值说明。
//  Return Value:
//		如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//		从ePubLib 1.0 开始支持。
//-------------------------------------------
EPUBAPI DKE_SetDefaultFont(const DK_WCHAR* szDefaultFontFaceName, DK_CHARSET_TYPE charset);

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
//		从ePubLib 1.0 开始支持。
//-------------------------------------------
EPUBAPI_(void) DKE_SetUseBookStyle(DK_BOOL bUseBookStyle);

//-------------------------------------------
//	Summary:
//		获取内核支持的最高文档版本号。
//	Parameters:
//		[in] nMajorVersion      - 主版本号。
//		[in] nMinorVersion      - 次版本号。
//		[in] nLastVersion       - 小版本号。
//  Return Value:
//      Null
//  Remarks:
//      Null
//  Availability:
//		从ePubLib 2.3.0 开始支持。
//-------------------------------------------
EPUBAPI_(void) DKE_GetSupportVersion(DK_INT& nMajorVersion, DK_INT& nMinorVersion, DK_INT& nLastVersion);

//-------------------------------------------
//	Summary:
//		设置显示文本时是否做简繁转换。
//	Parameters:
//		[in] bGBToBig5 - 是否做简繁转换。
//  Return Value:
//      Null
//  Remarks:
//      Null
//  Availability:
//		从ePubLib 2.1.0 开始支持。
//-------------------------------------------
EPUBAPI_(void) DKE_SetGBToBig5(DK_BOOL bGBToBig5);

//-------------------------------------------
//	Summary:
//		获取显示文本时是否做简繁转换。
//	Parameters:
//	    [out] bGBToBig5 - 是否做简繁转换。
//  Return Value:
//      Null
//  Remarks:
//      Null
//  Availability:
//		从ePubLib 2.1.0 开始支持。
//-------------------------------------------
EPUBAPI_(void) DKE_GetGBToBig5(DK_BOOL& bGBToBig5);

//-------------------------------------------
//	Summary:
//		验证指定字体中，是否存在传入文本所有字符的字形
//	Parameters:
//	    [in] szFontFaceName  - 待验证字体的 face name。
//	    [in] szText          - 用来验证的文本。
//  Return Value:
//      如果 szFontFaceName 对应字体文件中，包含 szText 中所有字符的字形，
//      则返回 DK_TRUE, 否则返回 DK_FALSE
//  Remarks:
//      Null
//  Availability:
//		从ePubLib 2.1.1 开始支持。
//-------------------------------------------
EPUBAPI DKE_VerifyFontFaceByText(const DK_WCHAR* szFontFaceName, const DK_WCHAR* szText);

//-------------------------------------------
//	Summary:
//		设置设备参数
//	Parameters:
//	    [in] deviceWidth    - 设备宽度
//	    [in] deviceHeight   - 设备高度
//	    [in] colorIndex     - 设备颜色数
//	    [in] resolution     - 设备分辨率
//  Return Value:
//      Null
//  Remarks:
//      Null
//  Availability:
//		从ePubLib 2.2.1 开始支持。
//-------------------------------------------
EPUBAPI_(void) DKE_SetDeviceParams(DK_INT deviceWidth, DK_INT deviceHeight, DK_INT colorIndex, DK_INT resolution);

//-------------------------------------------
//	Summary:
//		设置是否使用可交互的pre块
//	Parameters:
//	    [out] bUseIteractivePre - 是否使用pre块
//  Return Value:
//      Null
//  Remarks:
//      Null
//  Availability:
//		从ePubLib 2.7.0 开始支持。
//-------------------------------------------
EPUBAPI_(void) DKE_SetUseInteractivePre(DK_BOOL bUseIteractivePre);

//===========================================================================

#endif // #ifndef __DKEAPI_HEADERFILE__
