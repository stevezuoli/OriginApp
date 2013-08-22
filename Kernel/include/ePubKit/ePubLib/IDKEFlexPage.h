//===========================================================================
//  Summary:
//      ePub图书可伸缩页对象类。
//  Usage:
//      设置页面的宽度或高度和字号，排版后返回页面的高度，渲染页面，拾取页面对象。
//      当即不设置宽度，也不设置高度时按照宽高均不定处理
//  Remarks:
//      Null
//  Date:
//      2011-10-17
// Author:
//      Zhang Jiafang (zhangjf@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEFLEXPAGE_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEFLEXPAGE_H__

//===========================================================================

#include "DKEDef.h"

//===========================================================================


class IDKEFlexPage
{
public:
    virtual ~IDKEFlexPage() {}

public:
    //-------------------------------------------
    //  Summary:
    //      设置页面字号大小。
    //  Parameters:
    //      [in] fontSize        - 页面字号大小。
    //  Return Value:
    //      Null
    //  Remarks:
    //      页面字号只是一个参考值，将影响所有和此参考值相关的字号。
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetFontSize(DK_DOUBLE fontSize) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置页面行间距。
    //  Parameters:
    //      [in] lineGap        - 行间距倍数。
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetLineGap(DK_DOUBLE lineGap) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置页面段间距倍数。
    //  Parameters:
    //      [in] paraSpacing    - 段间距倍数。
    //  Return Value:
    //      Null
    //  Remarks:
    //      当段间距小于行（lineGap - 1）时段间距取（lineGap - 1）
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetParaSpacing(DK_DOUBLE paraSpacing) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置页面Tab大小。
    //  Parameters:
    //      [in] tabStop        - Tab的大小，以字符宽度为单位。
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetTabStop(DK_DOUBLE tabStop) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置页面首行缩进。
    //  Parameters:
    //      [in] indent            - 默认字号大小。
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetFirstLineIndent(DK_DOUBLE indent) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置章节文字颜色。
    //  Parameters:
    //      [in] textColor      - 文本颜色值。
    //  Return Value:
    //      Null
    //  Remarks:
    //      此接口将设置所有输出文字的颜色，而不采用章节内指定的颜色，
    //      调用 ResetAllColor 接口将恢复章节原色设置。
    //      该接口无需重新解析和排版，可在 ParseChapter 后调用。
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetTextColor(const DK_ARGBCOLOR& textColor) = 0;

    //-------------------------------------------
    //  Summary:
    //      重置文字颜色。
    //  Parameters:
    //      Null
    //  Return Value:
    //      Null
    //  Remarks:
    //      此接口将重置所有输出文字的颜色，恢复章节原色设置。
    //      该接口无需重新解析和排版，可在 ParseContent 后调用。
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID ResetAllTextColor() = 0;

    //-------------------------------------------
    //  Summary:
    //      设置排版模式。
    //  Parameters:
    //      [in] typeSetting            - 排版模式。
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetTypeSetting(DKTYPESETTING typeSetting) = 0;


    //-------------------------------------------
    //  Summary:
    //      设置页面的左上角坐标
    //  Parameters:
    //      [in] topLeft          - 页面左上角点
    //  Return Value:
    //      Null
    //  Remarks:
    //      若为设置页面左上角坐标，则左上角为(0,0)
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetPageTopLeft(DK_POS topLeft) = 0;

    //-------------------------------------------
    //  Summary:
    //      当需要不定高时，可设置页面宽度。
    //  Parameters:
    //      [in] pageWidth        - 页面宽度。
    //  Return Value:
    //      Null
    //  Remarks:
    //      设置页面宽度，根据该宽度排版成不限高，排完后返回真实排版高度。
    //      当即不设置宽度，也不设置高度时按照宽高均不定处理
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetPageWidth(DK_DOUBLE pageWidth) = 0;

    //-------------------------------------------
    //  Summary:
    //      当需要不定宽时，可设置页面高度。
    //  Parameters:
    //      [in] pageHeight        - 页面高度。
    //  Return Value:
    //      Null
    //  Remarks:
    //      设置页面高度，根据该高度排版成不限宽，排完后返回真实排版宽度，竖排使用与此模式
    //      当即不设置宽度，也不设置高度时按照宽高均不定处理
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetPageHeight(DK_DOUBLE pageHeight) = 0;

public:
    //-------------------------------------------
    //  Summary:
    //      对页面进行排版，生成页面对象，返回页面真实高度
    //  Parameters:
    //      Null
    //  Return Value:
    //      Null
    //  Remarks:
    //      必须在设置字号和排版宽度后才能进行排版
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID CalcPageLayout() = 0;

    //-------------------------------------------
    //  Summary:
    //      清除当前的排版结果
    //  Parameters:
    //      Null
    //  Return Value:
    //      Null
    //  Remarks:
    //      清除排版结果后，需要重新排版才能进行页面的渲染，拾取等操作
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID ClearLayoutResult() = 0;

    //-------------------------------------------
    //  Summary:
    //      测量可伸缩页的宽高，期间不保存页面元素
    //  Parameters:
    //      Null
    //  Return Value:
    //      Null
    //  Remarks:
    //      必须在设置字号和排版宽度后才能进行测量, 
    //      通过GetLayoutWidth和GetLayoutHeight获取测量结果
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_VOID MeasurePageSize() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面排版后的宽度
    //  Parameters:
    //      Null
    //  Return Value:
    //      页面排版后的宽度
    //  Remarks:
    //      在不定宽时，需要在调用CalcPageLayout后才知道页面的宽度
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_DOUBLE GetLayoutWidth() = 0;
    
    //-------------------------------------------
    //  Summary:
    //      获取页面排版后的高度
    //  Parameters:
    //      Null
    //  Return Value:
    //      页面排版后的高度
    //  Remarks:
    //      在不定高时，需要在调用CalcPageLayout后才知道页面的高度
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_DOUBLE GetLayoutHeight() = 0;

    //-------------------------------------------
    //  Summary:
    //      对页面进行渲染
    //  Parameters:
    //      [in] option             - 绘制选项。
    //      [out] pResult           - 绘制结果。
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      必须在CalcPageLayout调用之后才能进行渲染
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode Render(const DK_FLOWRENDEROPTION& option, DK_FLOWRENDERRESULT *pResult) = 0 ;

    //-------------------------------------------
    //  Summary:
    //      对页面进行局部渲染
    //  Parameters:
    //      [in] option             - 绘制选项。
    //      [in] areaBox            - 绘制的局部区域
    //      [out] pResult           - 绘制结果。
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      必须在CalcPageLayout调用之后才能进行渲染
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode RenderArea(const DK_FLOWRENDEROPTION& option, const DK_BOX& areaBox, DK_FLOWRENDERRESULT *pResult) = 0 ;

public:
    //-------------------------------------------
    //  Summary:
    //      获取页面文本内容。
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回页面文本内容，如果失败则返回 DK_NULL。
    //  Remarks:
    //      返回的文本内容指针在使用完毕后必须调用 FreeTextContent 接口释放。
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_WCHAR* GetTextContent() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放文本内容。
    //  Parameters:
    //      [in] content            - 文本内容指针
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeTextContent(DK_WCHAR* pText) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面指定范围内的文本外接矩形。
    //  Parameters:
    //      [in] startPos           - 起始坐标位置
    //      [in] endPos             - 结束坐标位置
    //      [out] textRects         - 返回文本外接矩形数组
    //      [out] rectCount         - 文本外接矩形数组的长度
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      获取的是位于 [startPos, endPos) 区间内的文本外接矩形数组，使用完毕之后必须调用 FreeRects 接口释放。
    //  Availability: 
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetTextRects(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DK_BOX** ppTextRects, DK_UINT* pRectCount) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放矩形区域。
    //  Parameters:
    //      [in] rects            - 待释放的矩形数组
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeRects(DK_BOX* pRects) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据输入点返回对应的页面中的链接信息
    //  Parameters:
    //      [in] point              - 输入点坐标
    //      [out] pLinkInfo         - 输入点对应的页面链接信息
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      获取的是位于页面 point 坐标位置的最上层页面的链接信息，使用完毕之后必须调用 FreeHitTestLink 接口释放。
    //  Availability: 
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestLink(const DK_POS& point, DKE_LINK_INFO* pLinkInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放页面链接信息。
    //  Parameters:
    //      [in] pLinkInfo            - 待释放的页面链接信息
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeHitTestLink(DKE_LINK_INFO* pLinkInfo) = 0;
};

//===========================================================================

#endif //__KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEFLEXPAGE_H__
