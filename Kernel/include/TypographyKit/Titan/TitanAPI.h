//===========================================================================
// Summary:
//		Titan是一个轻量级、高效的排版引擎。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-08-18
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef	__TITANAPI_HEADERFILE__
#define __TITANAPI_HEADERFILE__

//===========================================================================

#include "TitanDef.h"
#include "ITitanPart.h"
#include "ILayoutEnumerator.h"
#include "ITitanFontEngine.h"
#include "ISimpleBlockLayoutEnumerator.h"
#include "ISimpleBlockProcessor.h"
#include "IComplexBlockLayoutEnumerator.h"
#include "IComplexBlockProcessor.h"
#include "IHTMLBlockLayoutEnumerator.h"
#include "IHTMLBlockProcessor.h"

//===========================================================================

//---------------------------------------------------------------------------
//	Summary:
//		初始化Titan，使用TitanProcessor中的任何函数或类之前必须先调用本函数。
//	Parameters:
//	    [in] szResourceDir - TitanProcessor资源所在路径，该路径下包含TitanProcessor所需的Resource目录。
//	Remarks:
//		szResourceDir设置为空或不合法，则尝试选择程序模块所在目录(当前实现:Windows取DLL所在目录，Linux等平台取可执行程序目录)。
//  Availability:
//		从Titan 1.0开始支持。
//---------------------------------------------------------------------------
TITANAPI TP_Initialize(TP_WCHAR* szResourceDir = NULL);

//---------------------------------------------------------------------------
//	Summary:
//		执行清理，在不再使用TitanProcessor后调用。
//  Availability:
//		从Titan 1.0开始支持。
//---------------------------------------------------------------------------
TITANAPI TP_Destroy();

//===========================================================================

//---------------------------------------------------------------------------
//	Summary:
//		创建简单块的排版处理器。
//	Parameters:
//		[in] rcBlock					- 块边界矩形。
//		[in] msType						- 处理器采用的度量单位。
//		[in] nDPI						- DPI参数，当msType = TP_MEASURE_PIXEL时无效。
//		[in] pCustomFontEngine			- 用户自定义字体引擎。
//	Remarks:
//		rcBlock单位必须是msType，枚举器中的数据单位也是msType。
//		pCustomFontEngine=NULL时使用排版引擎自带的字体引擎。
//	Return Value:
//		如果成功返回块处理器指针，否则返回Null。
//  Availability:
//		从Titan 1.0开始支持。
//---------------------------------------------------------------------------
TITANAPI_(ISimpleBlockProcessor*) CreateSimpleBlockProcessor(const TP_BOX& rcBlock,
                                                             TP_MEASURE_TYPE msType,
                                                             int nDPI,
                                                             ITitanFontEngine* pCustomFontEngine);

//---------------------------------------------------------------------------
//	Summary:
//		删除简单块的排版处理器。
//	Parameters:
//		[in] pProcessor		- 待删除的块处理器指针。
//	Return Value:
//		如果成功删除返回true，否则返回false。
//  Availability:
//		从Titan 1.0开始支持。
//---------------------------------------------------------------------------
TITANAPI DestroySimpleBlockProcessor(ISimpleBlockProcessor* pProcessor);

//---------------------------------------------------------------------------
//	Summary:
//		创建复杂块的排版处理器。
//	Parameters:
//		[in] rcBlock					- 块边界矩形。
//		[in] msType						- 处理器采用的度量单位。
//		[in] nDPI						- DPI参数，当msType = TP_MEASURE_PIXEL时无效。
//	Remarks:
//		rcBlock单位必须是msType，枚举器中的数据单位也是msType。
//		pCustomFontEngine=NULL时使用排版引擎自带的字体引擎。
//	Return Value:
//		如果成功返回块处理器指针，否则返回Null。
//  Availability:
//		从Titan 1.0开始支持。
//---------------------------------------------------------------------------
TITANAPI_(IComplexBlockProcessor*) CreateComplexBlockProcessor(const TP_BOX& rcBlock,
                                                               TP_MEASURE_TYPE msType,
                                                               int nDPI,
                                                               ITitanFontEngine* pCustomFontEngine);

//---------------------------------------------------------------------------
//	Summary:
//		删除复杂块的排版处理器。
//	Parameters:
//		[in] pProcessor		- 待删除的块处理器指针。
//	Return Value:
//		如果成功删除返回true，否则返回false。
//  Availability:
//		从Titan 1.0开始支持。
//---------------------------------------------------------------------------
TITANAPI DestroyComplexBlockProcessor(IComplexBlockProcessor* pProcessor);

//---------------------------------------------------------------------------
//	Summary:
//		创建基于流结构的块排版处理器。用于EPUB转换。
//	Parameters:
//		[in] rcBlock					- 块边界矩形。
//		[in] msType						- 处理器采用的度量单位。
//		[in] nDPI						- DPI参数，当msType = TP_MEASURE_PIXEL时无效。
//		[in] hContentProvider			- 内容流提供对象句柄。
//		[in] pfnReadHTMLContentProc		- 读取内容流数据的回调函数入口。
//		[in] pCustomFontEngine			- 用户自定义字体引擎。
//	Remarks:
//		rcBlock单位必须是msType，枚举器中的数据单位也是msType。
//		pCustomFontEngine=NULL时使用排版引擎自带的字体引擎。
//		用户需要自己实现读取内容流数据的回调函数。
//	Return Value:
//		如果成功返回块处理器指针，否则返回Null。
//  Availability:
//		从Titan 1.0开始支持。
//---------------------------------------------------------------------------
TITANAPI_(IHTMLBlockProcessor*) CreateHTMLBlockProcessor(const TP_BOX& rcBlock,
                                                         TP_MEASURE_TYPE msType,
                                                         int nDPI,
                                                         TP_HANDLE hContentProvider,
                                                         PFNREADHTMLCONTENTPROC pfnReadHTMLContentProc,
                                                         ITitanFontEngine* pCustomFontEngine);

//---------------------------------------------------------------------------
//	Summary:
//		删除基于流结构的块排版处理器。用于EPUB转换。
//	Parameters:
//	   [in] pProcessor		- 待删除的块处理器指针。
//	Return Value:
//		如果成功删除返回true，否则返回false。
//  Availability:
//		从Titan 1.0开始支持。
//---------------------------------------------------------------------------
TITANAPI DestroyHTMLBlockProcessor(IHTMLBlockProcessor* pProcessor);

//===========================================================================

#endif	//#ifndef __TITANAPI_HEADERFILE__
