//===========================================================================
//  Summary:
//      ePub内容片段对象类。
//  Usage:
//      对内容片段设置字号，间距等排版信息，并对改片段进行排版，获取排版后的页对象等
//  Remarks:
//      Null
//  Date:
//      2011-10-17
//  Author:
//      Zhang Jiafang (zhangjf@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKESECTION_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKESECTION_H__


//===========================================================================

#include "DKEDef.h"

//===========================================================================

class IDKEPage;

class IDKESection
{
public:
    virtual ~IDKESection() {}

public:
    //-------------------------------------------
    //  Summary:
    //      设置片段字号大小。
    //  Parameters:
    //      [in] fontSize        - 字号大小。
    //  Return Value:
    //      Null
    //  Remarks:
    //      正文字号只是一个参考值，将影响所有和此参考值相关的字号。在parse之前设置。
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetFontSize(DK_DOUBLE fontSize) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置片段行间距。
    //  Parameters:
    //      [in] lineGap        - 行间距倍数。
    //  Return Value:
    //      Null
    //  Remarks:
    //      在parse之前设置。
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetLineGap(DK_DOUBLE lineGap) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置片段段间距倍数。
    //  Parameters:
    //      [in] paraSpacing    - 段间距倍数。
    //  Return Value:
    //      Null
    //  Remarks:
    //      当段间距小于行（lineGap - 1）时段间距取（lineGap - 1）。在parse之前设置。
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetParaSpacing(DK_DOUBLE paraSpacing) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置片段Tab大小。
    //  Parameters:
    //      [in] tabStop        - Tab的大小，以字符宽度为单位。
    //  Return Value:
    //      Null
    //  Remarks:
    //      在parse之前设置。
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetTabStop(DK_DOUBLE tabStop) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置片段首行缩进。
    //  Parameters:
    //      [in] indent            - 默认字号大小。
    //  Return Value:
    //      Null
    //  Remarks:
    //      在parse之前设置。
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetFirstLineIndent(DK_DOUBLE indent) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置片段文字颜色。
    //  Parameters:
    //      [in] textColor      - 文本颜色值。
    //  Return Value:
    //      Null
    //  Remarks:
    //      此接口将设置所有输出文字的颜色，而不采用片段内指定的颜色，
    //      调用 ResetAllColor 接口将恢复片段原色设置。
    //      该接口无需重新解析和排版，可在 ParseChapter 后调用。
    //      在parse之前设置。
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
    //      此接口将重置所有输出文字的颜色，恢复片段原色设置。
    //      该接口无需重新解析和排版，可在 ParseContent 后调用。
    //      在parse之前设置。
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
    //      在parse之前设置。
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetTypeSetting(DKTYPESETTING typeSetting) = 0;

public:
    //-------------------------------------------
    //  Summary:
    //      设置页面区域。
    //  Parameters:
    //      [in] pageBox             - 页面区域
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      在parse之前设置。
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetPageBox(const DK_BOX& pageBox) = 0;

    //-------------------------------------------
    //  Summary:
    //      解析片段内容。
    //  Parameters:
    //      [in] option             - 解析器选项，参见 DKE_PARSER_OPTION 结构说明。
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode ParseSection(DK_BOOL bDoPageBreaking = DK_TRUE) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取片段解析后的页数
    //  Parameters:
    //      Null
    //  Return Value:
    //      片段页数
    //  Remarks:
    //      需要在 ParseChapter 调用之后调用
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_LONG GetPageCount() const = 0;

    //-------------------------------------------
    //  Summary:
    //      根据页码获取页对象
    //  Parameters:
    //      [in] pageNum        - 该片段范围内的页码，从0开始。
    //  Return Value:
    //      页对象的指针
    //  Remarks:
    //      需要在 ParseChapter 调用之后调用
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual IDKEPage* GetPage(DK_LONG pageNum) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据流式坐标多对应的页码。该流式坐标落在页的区域内即可
    //  Parameters:
    //      [in] pos            - 流式坐标。
    //  Return Value:
    //      页码，从0开始
    //  Remarks:
    //      需要在 ParseChapter 调用之后调用
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_LONG GetPageNum(const DK_FLOWPOSITION& pos) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据锚点的名称获取该锚点在片段内的流式坐标
    //  Parameters:
    //      [in] pAnchor            - 锚点的名称
    //      [out] pPos              - 锚点对应的流式坐标
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      需要在 ParseChapter 调用之后调用
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetAnchorPosition(const DK_WCHAR* pAnchor, DK_FLOWPOSITION* pPos) = 0;

    //-------------------------------------------
    //  Summary:
    //      清除片段内已解析的页对象
    //  Parameters:
    //      Null
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID ClearParsedPages() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取片段的文本
    //  Parameters:
    //      Null
    //  Return Value:
    //      文本指针
    //  Remarks:
    //      需要调用FreeChapterText释放返回值的内存
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_WCHAR* GetSectionText() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取片段的文本
    //  Parameters:
    //      [in] pText            - 需要释放的文本
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeSectionText(DK_WCHAR* pText) = 0;
};

//===========================================================================

#endif // __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKECHAPTER_H__
