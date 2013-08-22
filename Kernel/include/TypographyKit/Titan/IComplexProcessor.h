//===========================================================================
//  Summary:
//      复杂排版处理器接口
//  Usage:
//      Null
//  Remarks:
//      Null
//  Date:
//      2013-01-04
//  Author:
//      Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __COMPLEXPROCESSOR_INTERFACE_HEADERFILE__
#define __COMPLEXPROCESSOR_INTERFACE_HEADERFILE__

#include "IComplexBlockProcessor.h"

class ILayoutBoundary;
class ITitanFontEngine;

class IComplexProcessor
{
public:
    virtual ~IComplexProcessor() = 0;

public: // 基础排版环境
    // 不建议使用
    virtual DK_VOID SetBoundingBox(const TP_BOX &box) = 0;

    // 设置排版区域
    virtual DK_VOID SetBoundary(const ILayoutBoundary *pLayoutBoundary) = 0;

    // 设置排版区域顶线
    virtual DK_VOID SetTopLine(double dTopLine) = 0;

    // 设置中文排版环境
    virtual DK_VOID SetCJKLayoutEnvironment(TP_CJK_LAYOUT_ENV eLayoutEnv) = 0;

    // 设置Hyphenation使用的语言
    virtual DK_VOID SetHyphenationLanguage(TP_HYPHENATION_LANGUAGE eLanguage) = 0;

    // 设置字体引擎
    virtual DK_VOID SetFontEngine(ITitanFontEngine *pFontEngine) = 0;

    // 设置排版方向
    virtual DK_VOID SetLayoutDirection(TP_DIRECTION_TYPE eDirect) = 0;

    virtual DK_VOID SetPunctuationVerticalStyle(TP_PUNCTUATION_VERTICAL_STYLE eStyle) = 0;

    // 设置是否强行排版
    virtual DK_VOID SetAtLeastOneLine(bool bAtLeastOneLine) = 0;

    // 设置是否允许出血
    virtual DK_VOID SetAllowBleed(bool bAllowBleed) = 0;

    // 设置出血长度
    virtual DK_VOID SetBleedLength(double dLength) = 0;

    // 外部设置段排版参数
    virtual DK_VOID SetCustomParaStyle(const TP_PARASTYLE &paraStyle) = 0;

    // 设置标点最大压缩程度
    virtual DK_VOID SetPuncMaxShrinkRate(double dPuncShrinkRate) = 0;

    // 设置行拉伸最大程度
    virtual DK_VOID SetMaxStretchRate(double dStretchRate) = 0;

    virtual DK_VOID SetCustomParaFont(const TP_FONT &paraFont) = 0;

    virtual DK_VOID ResetCustomParaStyle() = 0;

public:
    // 排版文字
    virtual TP_LAYOUT_STATE ProcessText(const std::vector<TP_CONTENTPIECE> &vPieces, const TP_INITIALSTATEPARA &isp) = 0;

public:
    // 获取排版结果枚举器
    virtual ILayoutEnumerator * GetLayoutEnumerator() = 0;

    // 获取当前排版高度
    virtual DK_DOUBLE GetLayoutHeight() = 0;

    // 获取最后一行的高度
    virtual DK_DOUBLE GetLastLineHeight() = 0;

    // 获取最后一行的高度
    virtual DK_DOUBLE GetLastLineGap() = 0;

public:
    virtual bool GetLinesHeight(const TP_FONT &baseFont, 
                                double lineGap, 
                                unsigned int lineCount, 
                                const TP_FONT &dropCapFont, 
                                TP_WCHAR dropCapChar, 
                                double *pDropCapFontSize) = 0;
};

//===========================================================================

#endif //#ifndef __COMPLEXPROCESSOR_INTERFACE_HEADERFILE__
