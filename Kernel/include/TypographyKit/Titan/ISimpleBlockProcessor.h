//===========================================================================
// Summary:
//		基于简单块的排版处理器接口。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-08-31
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef	__SIMPLEBLOCKPROCESSOR_INTERFACE_HEADERFILE__
#define __SIMPLEBLOCKPROCESSOR_INTERFACE_HEADERFILE__

//===========================================================================

#include "ITitanPart.h"

//===========================================================================

class ISimpleBlockProcessor : public ITitanPart
{
public:
	//-------------------------------------------
	//	Summary:
	//		装载对应字符集的字体。
	//  Parameters:
	//		[in] csType				- 字符集类型。
	//		[in] pszFontFaceName	- 字体名字。
	//		[in] dFontSize			- 字体大小。
	//	Remarks:
	//		目前只支持TP_CHARSET_PRC和TP_CHARSET_ANSI字体编码，自带的字体引擎没有实现LoadFont接口。
	//		因此，使用自带的字体引擎时不要用这个接口装载字体。
	//	Return Value:
	//		如果成功则返回true，否则返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool LoadFont(TP_CHARSET_TYPE csType, const TP_WCHAR* pszFontFaceName, double dFontSize) = 0;

	//-------------------------------------------
	//	Summary:
	//		装载对应字符集的字体。
	//  Parameters:
	//		[in] csType				- 字符集类型。
	//		[in] pszFontFilePath	- 字体文件全路径名。
	//		[in] dFontSize			- 字体大小。
	//	Remarks:
	//		调用此方法将覆盖上一次装载的同字符集字体，目前只支持TP_CHARSET_PRC和TP_CHARSET_ANSI字体编码。
	//	Return Value:
	//		字体装载成功则返回true，否则返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool LoadFontFile(TP_CHARSET_TYPE csType, const TP_WCHAR* pszFontFilePath, double dFontSize) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置字号大小。
	//  Parameters:
	//		[in] dFontSize		- 字号大小。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetFontSize(double dFontSize) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置Tab制表位，单位为字号大小。
	//  Parameters:
	//		[in] dTabStop		- 制表位距离。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetTabStop(double dTabStop) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置首行缩进。
	//  Parameters:
	//		[in] dIndent		- 首行缩进距离，单位为字号大小。
	//	Remarks:
	//		如果不设置首行缩进，处理器默认不做处理。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetFirstLineIndent(double dIndent) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置对齐方式。
	//  Parameters:
	//		[in] aType			- 对齐方式。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetAlignType(TP_ALIGN_TYPE aType) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置行间距。
	//  Parameters:
	//		[in] dLineGap		- 行间距，采用行高的倍数。
	//	Remarks:
	//		如果不设置行间距，处理器将采用单倍行高作为默认行间距。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetLineGap(double dLineGap) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置段间距。
	//  Parameters:
	//		[in] dParaGap		- 段间距，采用行高的倍数。
	//	Remarks:
	//		如果不设置段间距，处理器默认段间距为0。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetParaSpacing(double dParaSpacing) = 0;

	//-------------------------------------------
	//	Summary:
	//		拉伸最大百分比，超过这个比例需要做Hyphen处理。
	//  Parameters:
	//		[in] dStretchRate		- 拉伸最大百分比。
	//	Remarks:
	//		内部调试函数。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetStretchMaxRate(double dStretchRate) = 0;

public:
	//-------------------------------------------
	//	Summary:
	//		排入文字。
	//  Parameters:
	//		[in] pszSrcText		- 待排版的文字串。
	//		[in] bFastMode		- 高速模式，该模式下不能枚举排版结果。
	//	Return Value:
	//		文字全部排入成功返回TP_LAYOUT_SUCCEED；
	//		部分排入还有后续返回TP_LAYOUT_FOLLOWAFTER；
	//		排入失败则返回TP_LAYOUT_FAIL。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual TP_LAYOUT_STATE ProcessText(const TP_WCHAR* pszSrcText, bool bFastMode = false) = 0;

	//-------------------------------------------
	//	Summary:
	//		排入文字，可以指定初始状态参数。
	//  Parameters:
	//		[in] pszSrcText		- 待排版的文字串。
	//		[in] isp			- 初始状态参数，可以指定当前块起始位置是否是新段落。
	//		[in] bFastMode		- 高速模式，该模式下不能枚举排版结果。
	//	Return Value:
	//		文字全部排入成功返回TP_LAYOUT_SUCCEED；
	//		部分排入还有后续返回TP_LAYOUT_FOLLOWAFTER；
	//		排入失败则返回TP_LAYOUT_FAIL。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual TP_LAYOUT_STATE ProcessTextEx(const TP_WCHAR* pszSrcText, const TP_INITIALSTATEPARA& isp, bool bFastMode = false) = 0;
};

//===========================================================================

#endif	//#ifndef __SIMPLEBLOCKPROCESSOR_INTERFACE_HEADERFILE__
