//===========================================================================
// Summary:
//		自定义字体引擎接口。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-08-27
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef	__FONTENGINE_INTERFACE_HEADERFILE__
#define __FONTENGINE_INTERFACE_HEADERFILE__

//===========================================================================

#include "TitanDef.h"

//===========================================================================

class ITitanFontEngine
{
public:
    //-------------------------------------------
	//	Summary:
	//		虚析构函数。
	//-------------------------------------------
    virtual ~ITitanFontEngine(){}

	//-------------------------------------------
	//	Summary:
	//		初始化字体引擎。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void Init() = 0;
	
	//-------------------------------------------
	//	Summary:
	//		销毁字体引擎。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void Destroy() = 0;

	//-------------------------------------------
	//	Summary:
	//		载入字体。
	//  Parameters:
	//		[in] pszFontFaceName	- 字体名字。
	//		[in] dFontSize			- 字体大小。
	//		[in] nSytle				- 字体风格。
	//		[in] nWeight			- 字体浓淡。
	//	Remarks:
	//		排版引擎自带的字体引擎没有实现这个函数。
	//	Return Value:
	//		如果成功则返回true，否则返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool LoadFont(const TP_WCHAR* pszFontFaceName, double dFontSize,
						  TP_FONTSTYLE_TYPE nSytle = TP_FONTSTYLE_NORMAL,
						  TP_FONTWEIGHT_TYPE nWeight = TP_FONTWEIGHT_NORMAL) = 0;

	//-------------------------------------------
	//	Summary:
	//		载入字体文件。
	//  Parameters:
	//		[in] pszFontFilePath	- 字体文件路径。
	//		[in] dFontSize			- 字体大小。
	//		[in] nSytle				- 字体风格。
	//		[in] nWeight			- 字体浓淡。
	//	Return Value:
	//		如果成功则返回true，否则返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool LoadFontFile(const TP_WCHAR* pszFontFilePath, double dFontSize,
							  TP_FONTSTYLE_TYPE nSytle = TP_FONTSTYLE_NORMAL,
							  TP_FONTWEIGHT_TYPE nWeight = TP_FONTWEIGHT_NORMAL) = 0;

	//-------------------------------------------
	//	Summary:
	//		载入自定义字体句柄。
	//  Parameters:
	//		[in] hFont				- 自定义字体句柄。
	//		[in] dFontSize			- 字体大小。
	//		[in] nSytle				- 字体风格。
	//		[in] nWeight			- 字体浓淡。
	//	Remarks:
	//		排版引擎自带的字体引擎没有实现这个函数。
	//	Return Value:
	//		如果成功则返回true，否则返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool LoadFontHandle(TP_HANDLE hFont, double dFontSize,
								TP_FONTSTYLE_TYPE nSytle = TP_FONTSTYLE_NORMAL,
								TP_FONTWEIGHT_TYPE nWeight = TP_FONTWEIGHT_NORMAL) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置当前激活的字体。
	//  Parameters:
	//		[in] pszFontFaceName	- 字体名字。
	//	Remarks:
	//		排版引擎自带的字体引擎没有实现这个函数。
	//	Return Value:
	//		如果成功则返回true，否则返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool SetActiveFont(const TP_WCHAR* pszFontFaceName) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置当前激活的字体句柄。
	//  Parameters:
	//		[in] hFont				- 字体句柄。
	//	Remarks:
	//		排版引擎自带的字体引擎没有实现这个函数。
	//	Return Value:
	//		如果成功则返回true，否则返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool SetActiveFontHandle(TP_HANDLE hFont) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置需要支持的字符集。
	//  Parameters:
	//		[in] charset			- 字体需要支持的字符集，可以设为多个值的组合。
	//	Remarks:
	//		具体实现类可以根据此参数决定字体的某些属性，例如当要读入的字体不存在时，
	//		根据此参数选择默认字体。
	//	Return Value:
	//		如果成功则返回true，否则返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool SetSupportedCharset(TP_FONTCHARSET_TYPE charset) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置当前字体大小。
	//  Parameters:
	//		[in] dFontSize			- 字体大小。
	//	Return Value:
	//		如果成功则返回true，否则返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool SetFontSize(double dFontSize) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置当前字体风格。
	//  Parameters:
	//		[in] nSytle			- 字体风格。
	//	Remarks:
	//		排版引擎自带的字体引擎没有实现这个函数。
	//	Return Value:
	//		如果成功则返回true，否则返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool SetFontStyle(TP_FONTSTYLE_TYPE nSytle) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置当前字体浓淡。
	//  Parameters:
	//		[in] nWeight			- 字体浓淡。
	//	Remarks:
	//		排版引擎自带的字体引擎没有实现这个函数。
	//	Return Value:
	//		如果成功则返回true，否则返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool SetFontWeight(TP_FONTWEIGHT_TYPE nWeight) = 0;

	//-------------------------------------------
	//	Summary:
	//		获取当前字体指定字符基线以上部分的高度。
	//  Parameters:
	//		[in] ch				- 字符编码。
	//	Return Value:
	//		如果成功则返回当前字体字符基线以上部分的高度。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual double GetCharAscent(TP_WCHAR ch) = 0;

	//-------------------------------------------
	//	Summary:
	//		获取当前字体指定字符基线以下部分的高度。
	//  Parameters:
	//		[in] ch				- 字符编码。
	//	Return Value:
	//		如果成功则返回当前字体字符基线以下部分的高度。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual double GetCharDescent(TP_WCHAR ch) = 0;
	
	//-------------------------------------------
	//	Summary:
	//		获取当前字体最大Ascent高度。
	//	Return Value:
	//		如果成功则返回当前字体的最大Ascent。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------	
	virtual double GetMaxAscent() = 0;

	//-------------------------------------------
	//	Summary:
	//		获取当前字体最大Descent高度。
	//	Return Value:
	//		如果成功则返回当前字体的最大Descent。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------	
	virtual double GetMaxDescent() = 0;

	//-------------------------------------------
	//	Summary:
	//		获取当前字体最大高度。
	//	Return Value:
	//		如果成功则返回当前字体的最大高度。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------	
	virtual double GetMaxHeight() = 0;

	//-------------------------------------------
	//	Summary:
	//		获取当前字体最大宽度。
	//	Return Value:
	//		如果成功则返回当前字体的最大宽度。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------	
	virtual double GetMaxWidth() = 0;

	//-------------------------------------------
	//	Summary:
	//		获取当前字体指定字符的宽度。
	//  Parameters:
	//		[in] ch				- 字符编码。
	//	Return Value:
	//		如果成功则返回字符宽度。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual double GetCharWidth(TP_WCHAR ch) = 0;

	//-------------------------------------------
	//	Summary:
	//		当前字体是否有Kerning。
	//	Return Value:
	//		有返回true，没有返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool HasKerning() = 0;

	//-------------------------------------------
	//	Summary:
	//		获取2个字符的字距调整信息。
	//  Parameters:
	//		[in] ch				- 第一个字符编码。
	//		[in] ch				- 第二个字符编码。
	//		[out] dx			- x方向字距调整。
	//		[out] dy			- y方向字距调整。
	//	Return Value:
	//		如果成功则返回true，否则返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool GetKerning(TP_WCHAR chFirst, TP_WCHAR chSecond, double& dx, double& dy) = 0;

	//-------------------------------------------
	//	Summary:
	//		当前字体是否支持竖排。
	//	Return Value:
	//		支持返回true，否则返回false。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool HasVertical() = 0;

	//-------------------------------------------
	//	Summary:
	//		获取当前字体指定字符的字形度量。
	//  Parameters:
	//		[in]  ch				- 字符编码。
	//		[out] glyphMetrics		- 字形度量。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void GetCharMetrics(TP_WCHAR ch, TP_GLYPHMETRICS& glyphMetrics) = 0; 

	//-------------------------------------------
	//	Summary:
	//		获取当前字体指定字形索引的字形度量。
	//  Parameters:
	//		[in]  dwGlyphIndex		- 字形索引。
	//		[out] glyphMetrics		- 字形度量。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void GetGlyphMetrics(unsigned int dwGlyphIndex, TP_GLYPHMETRICS& glyphMetrics) = 0; 

	//-------------------------------------------
	//	Summary:
	//		获取当前字体指定字形索引的字形度量。
	//  Parameters:
	//		[in]  ch			- 字符编码。
	//		[out]  bChanged		- 竖排时候字形是否改变。
	//	Return Value:
	//		得到字符竖排时候的字形索引，可能是经过字形变换后新的字形的索引。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual unsigned int GetVerticalGlyphIndex(TP_WCHAR ch, bool& bChanged) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置备用字体文件
	//  Parameters:
	//		[in]   backupFontFile - 字符编码。
	//	Return Value:
	//		None
	//  Availability:
	//		从Titan 1.0.6开始支持。
	//-------------------------------------------
    virtual void SetBackupFontFile(const TP_WCHAR* backupFontFile) = 0;
};

//===========================================================================

#endif	//#ifndef __FONTENGINE_INTERFACE_HEADERFILE__
