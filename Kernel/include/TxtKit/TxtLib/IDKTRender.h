//===========================================================================
//  Summary:
//        IDKTRender.h
//  Usage:
//        txt轻量排版绘制接口
//  Remarks:
//        只适用于绘制单页文字
//  Date:
//        2012-06-27
//  Author:
//        Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================
#ifndef __KERNEL_TXTKIT_TXTLIB_IDKTRENDER_H__
#define __KERNEL_TXTKIT_TXTLIB_IDKTRENDER_H__

#include "KernelType.h"
#include "KernelRetCode.h"

class IDKTRender
{
public:
    virtual ~IDKTRender() {}

public:
    enum LAYOUT_ALIGN_TYPE
    {
        LAYOUT_ALIGN_LEFT, 
        LAYOUT_ALIGN_RIGHT, 
        LAYOUT_ALIGN_CENTER, 
        LAYOUT_ALIGN_JUSTIFY
    };

    // 空行处理策略
    enum BLANKLINE_STRATEGY
    {
        BLANKLINE_COPY,     // 和数据源保持一致
        BLANKLINE_SINGLE,   // 多个连续空行压缩为一个空行
        BLANKLINE_IGNOREALL // 忽略所有空行
    };

    struct LayoutOption
    {
        DK_DOUBLE          fontSize;          // 字号
        DK_DOUBLE          lineGap;           // 行间距，以行高为单位。
        DK_DOUBLE          paraSpacing;       // 段间距，以行高为单位。
        DK_DOUBLE          tabStop;           // tab大小，以fontSize为单位。
        DK_DOUBLE          indent;            // 首行缩进，以fontSize为单位。
        LAYOUT_ALIGN_TYPE  alignType;         // 对齐方式
        BLANKLINE_STRATEGY blanklineStrategy; // 空行处理

        LayoutOption()
            : fontSize(0.0)
            , lineGap(0.0)
            , paraSpacing(0.0)
            , tabStop(0.0)
            , indent(0.0)
            , alignType(LAYOUT_ALIGN_JUSTIFY)
            , blanklineStrategy(BLANKLINE_COPY)
        {}
    };

    struct RenderCharInfo
    {
        DK_WCHAR        ch;          // 字符
        DK_CHARSET_TYPE charset;     // 所属字符集
        DK_BOX          boundingBox; // 外接矩形（用于确定外接框）
        DK_DOUBLE       x, y;        // 基线坐标（用于绘制）
        DK_BOOL         hyphen;      // 是不是Hyphen
        DK_BOOL         visible;     // 是否可见
        DK_UINT         index;       // 在原文中的index，Hyphen的index和前一个字符相同

        RenderCharInfo()
            : ch(0)
            , charset(DK_CHARSET_ANSI)
            , x(0.0)
            , y(0.0)
            , hyphen(DK_FALSE)
            , visible(DK_TRUE)
            , index(0)
        {}
    };

    struct RenderLineInfo
    {
        DK_BOX  boundingBox; // 外接矩形
        DK_UINT startIndex;  // line第一个字符对应的index

        RenderLineInfo()
            : startIndex(0)
        {}
    };

public:
    //-------------------------------------------
    //  Summary:
    //      获取用于Render的排版结果。
    //  Parameters:
    //      [in]  pText             - 排版文字
    //      [in]  layoutOption      - 排版参数
    //      [in]  pGbFontFile       - 中文字体，要求全路径
    //      [in]  pAnsiFontFile     - 英文字体，要求全路径
    //      [in]  width             - 排版区宽度
    //      [in]  height            - 排版区高度
    //      [in]  endWithEllipsis   - 如果排不下，结尾是否添加省略号
    //      [out] pFollowAfter      - 是否还有后续没排完的文本
    //      [out] ppRenderCharInfo  - 返回的字符信息，包括每个字符的外接矩形，基线等信息
    //      [out] pCharInfoCount    - 返回的字符数（因为hyphen原因，可能比原文本要长
    //      [out] ppRenderLineInfo  - 返回的行信息
    //      [out] pLineInfoCount    - 返回的行数目
    //  Return Value:
    //      DK_ReturnCode
    //  Remarks:
    //      如果末尾添加省略号，其index和前一个字符相同（同Hyphen）
    //  Availability:
    //      从TxtLib 2.0.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode CreateRenderInfoByHeight(const DK_WCHAR *     pText, 
                                                   const LayoutOption & layoutOption, 
                                                   const DK_WCHAR *     pGbFontFile, 
                                                   const DK_WCHAR *     pAnsiFontFile, 
                                                   DK_DOUBLE            width, 
                                                   DK_DOUBLE            height, 
                                                   DK_BOOL              endWithEllipsis, 
                                                   DK_BOOL *            pFollowAfter, 
                                                   RenderCharInfo **    ppRenderCharInfo, 
                                                   DK_UINT *            pCharInfoCount, 
                                                   RenderLineInfo **    ppRenderLineInfo, 
                                                   DK_UINT *            pLineInfoCount) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取用于Render的排版结果。
    //  Parameters:
    //      [in]  pText             - 排版文字
    //      [in]  layoutOption      - 排版参数
    //      [in]  pGbFontFile       - 中文字体，要求全路径
    //      [in]  pAnsiFontFile     - 英文字体，要求全路径
    //      [in]  width             - 排版区宽度
    //      [in]  maxlineCount      - 排版最大行数
    //      [in]  endWithEllipsis   - 如果排不下，结尾是否添加省略号
    //      [out] pFollowAfter      - 是否还有后续没排完的文本
    //      [out] ppRenderCharInfo  - 返回的字符信息，包括每个字符的外接矩形，基线等信息
    //      [out] pCharInfoCount    - 返回的字符数（因为hyphen原因，可能比原文本要长
    //      [out] ppRenderLineInfo  - 返回的行信息
    //      [out] pLineInfoCount    - 返回的行数目
    //  Return Value:
    //      DK_ReturnCode
    //  Remarks:
    //      如果末尾添加省略号，其index和前一个字符相同（同Hyphen）
    //  Availability:
    //      从TxtLib 2.0.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode CreateRenderInfoByLineCount(const DK_WCHAR *     pText, 
                                                      const LayoutOption & layoutOption, 
                                                      const DK_WCHAR *     pGbFontFile, 
                                                      const DK_WCHAR *     pAnsiFontFile, 
                                                      DK_DOUBLE            width, 
                                                      DK_UINT              maxLineCount, 
                                                      DK_BOOL              endWithEllipsis,
                                                      DK_BOOL *            pFollowAfter, 
                                                      RenderCharInfo **    ppRenderCharInfo, 
                                                      DK_UINT *            pCharInfoCount, 
                                                      RenderLineInfo **    ppRenderLineInfo, 
                                                      DK_UINT *            pLineInfoCount) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据排版结果绘制。
    //  Parameters:
    //      [in] opton         - 渲染选项
    //      [in] convertToCHT  - 是否转换为繁体绘制
    //      [in] offsetX       - 绘制相对于device左上角的x偏移
    //      [in] offsetY       - 绘制相对于device左上角的y偏移
    //      [in] textColor     - 字体颜色
    //      [in] pGbFontFile   - 中文字体全路径
    //      [in] pAnsiFontFile - 英文字体全路径
    //      [in] fontSize      - 字号
    //      [in] pRenderStart  - 绘制起点
    //      [in] pRenderEnd    - 绘制终点（该点不会被绘制）
    //  Return Value:
    //      DK_ReturnCode
    //  Remarks:
    //      Null
    //  Availability:
    //      从TxtLib 2.0.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode RenderByInfo(const DK_FLOWRENDEROPTION & option, 
                                       DK_BOOL                     convertToCHT, 
                                       DK_DOUBLE                   offsetX, 
                                       DK_DOUBLE                   offsetY, 
                                       const DK_ARGBCOLOR &        textColor, 
                                       const DK_WCHAR *            pGbFontFile, 
                                       const DK_WCHAR *            pAnsiFontFile, 
                                       DK_DOUBLE                   fontSize, 
                                       const RenderCharInfo *      pRenderStart, 
                                       const RenderCharInfo *      pRenderEnd) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放排版结果。
    //  Parameters:
    //      [in] pRenderCharInfo  - 字符信息
    //      [in] pRenderLineInfo  - 行信息
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从TxtLib 2.0.0开始支持。
    //-------------------------------------------
    virtual DK_VOID ReleaseRenderInfo(RenderCharInfo *pRenderCharInfo, RenderLineInfo *pRenderLineInfo) = 0;
};

#endif // __KERNEL_TXTKIT_TXTLIB_IDKTRENDER_H__
