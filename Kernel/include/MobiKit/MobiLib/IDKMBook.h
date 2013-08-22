//===========================================================================
//  Summary:
//      Mobi图书对象类。
//  Usage:
//      Null
//  Remarks:
//      Null
//  Date:
//      2012-02-25
//  Author:
//      Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_MOBIKIT_MOBILIB_EXPORT_IDKMBOOK_H__
#define __KERNEL_MOBIKIT_MOBILIB_EXPORT_IDKMBOOK_H__

#include "DKMDef.h"

class IDKMPage;
class IDKMTocPoint;

struct DKM_PARSER_OPTION
{
    DK_DOUBLE       bodyFontSize;    // 正文字号（参考值，会影响其他字号）
    DK_DOUBLE       lineGap;         // 行间距（倍数）
    DK_DOUBLE       paraSpacing;     // 段间距（倍数）
    DK_DOUBLE       tabStop;         // TAB大小
    DK_DOUBLE       firstLineIndent; // 首行缩进
    DK_BOX          pageBox;         // 页面尺寸
    DK_MEASURE_TYPE msType;          // 度量单位，参见 DK_MEASURE_TYPE 枚举值说明
    DK_LONG         dpi;             // 输出设备的DPI值，当 msType = DK_MEASURE_PIXEL 时无效

    DKM_PARSER_OPTION() 
        : bodyFontSize(0.0)
        , lineGap(0.0)
        , paraSpacing(0.0)
        , tabStop(0.0)
        , firstLineIndent(0.0)
        , pageBox(0.0, 0.0, 0.0, 0.0)
        , msType(DK_MEASURE_PIXEL)
        , dpi(0)
    {}
};

//===========================================================================

//////////////////////////////////////
// class IDKMBook
//    NOTICE: 现在此接口只支持单线程调用，不能一线程CreatePage,另一线程Render
/////////////////////////////////////
class IDKMBook
{
public:
    virtual ~IDKMBook() {}
public:
    virtual DK_ReturnCode Initialize() = 0;
    virtual DK_VOID SetParseOption(const DKM_PARSER_OPTION &option) = 0;
    virtual DK_VOID SetTextColor(const DK_ARGBCOLOR &textColor) = 0;
    virtual DK_VOID ResetTextColor() = 0;
    virtual DK_VOID SetTypeSetting(const DKTYPESETTING &typeSetting) = 0;
    virtual DK_UINT GetTocOffset() const = 0;
    virtual DK_UINT GetTextOffset() const = 0;
    virtual const IDKMTocPoint * GetTocRoot() = 0;
public:
    enum PAGE_POSITION_TYPE
    {
        PREV_PAGE,     // 上一页
        LOCATION_PAGE, // 根据位置跳转的页
        NEXT_PAGE,     // 下一页
    };
    //-------------------------------------------
    //  Summary:
    //      获得主文件长度，以字节为单位
    //  Parameters:
    //      [in]:bytesPerLogicalPage  - bytes per logial page
    //  Return Value:
    //      count of logical pages = [file length in bytes / bytes per logical page]
    //  Remarks:
    //      从MobiLib 1.0.8开始支持
    //-------------------------------------------
    virtual DK_UINT GetMainFileLength() const = 0;

    //-------------------------------------------
    //  Summary:
    //      Create page at position
    //  Parameters:
    //      [in]:pagePosition         - page position in bytes
    //      [in]:type                 - PREV, LOCATION, NEXT
    //  Return Value:
    //      page created, DK_NULL if failed
    //  Remarks:
    //      for LOCATION and NEXT, start at page position
    //      LOCATION need start trying at a position before the page position
    //      and adjust the start pos slightly to make page up/down more fluently
    //      NEXT can start at the position
    //      for PREV, page returned ends at page position(may be adjusted slightly)
    //      从MobiLib 1.0.8开始支持
    //-------------------------------------------
    virtual IDKMPage * CreatePage(DK_UINT pagePosition, PAGE_POSITION_TYPE type) const = 0;

    //-------------------------------------------
    //  Summary:
    //      Destroy a page
    //  Parameters:
    //      [in]:page position    -
    //  Return Value:
    //      count of logical pages = [file length in bytes / bytes per logical page]
    //  Remarks:
    //      从MobiLib 1.0.8开始支持
    //-------------------------------------------
    virtual DK_VOID DestroyPage(IDKMPage *pPage) const = 0;

public:
    //-------------------------------------------
    //  Summary:
    //      Get book info
    //  Parameters:
    //      Null
    //  Return Value:
    //      book info, DK_NULL if failed
    //  Remarks:
    //      从MobiLib 1.0.8开始支持
    //-------------------------------------------
    virtual const DKMBookInfo * GetBookInfo() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取图书封面数据。
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回图书封面数据信息，如果失败则返回DK_NULL。
    //  Remarks:
    //      在Initialize接口调用之后即可使用。
    //  Availability:
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual const DKFILEINFO* GetBookCover() const = 0;
public:
    //-------------------------------------------
    //  Summary:
    //      在全书内查找文本，必须在 Initialize 之后调用。
    //  Parameters:
    //      [in] startPos               - 开始搜索的位置
    //      [in] text                   - 搜索文本
    //      [in] maxResultCount         - 最大结果数目
    //      [out] findResults           - 搜索结果，为一系列 [start, end) 区间对
    //      [out] resultCount           - 搜索结果数目
    //  Return Value:
    //      正常情况即使没有找到结果也返回 DKR_OK，以 resultCount 判断查找是否有结果，
    //      出错返回错误码。
    //  Remarks:
    //      findResults 中元素的个数应该为 2*resultCount，
    //      使用完毕之后必须调用 FreeFindTextResults 接口释放。
    //  Availability:
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode FindText(DK_UINT startPos, const DK_WCHAR *text, DK_UINT maxResultCount, 
                                   DK_UINT** findResults, DK_UINT* resultCount) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放文本查找结果
    //  Parameters:
    //      [in] findResults            - 搜索结果
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeFindTextResults(DK_UINT* findResults) = 0;

    //-------------------------------------------
    //  Summary:
    //      获得查找结果的上下文片段。
    //  Parameters:
    //      [in] pos                    - 查找结果的位置
    //      [in] text                   - 查找之文本
    //      [in] ellipsis               - 省略符号
    //      [in] snippetLen             - snippet 长度，不包括结束符
    //      [out] result                - 接受返回 snippet 的 buffer
    //      [out] startPosInSnippet     - 搜索串在 snippet 中的起始位置
    //      [out] endPosInSnippet       - 搜索串在 snippet 中的结束位置
    //  Return Value:
    //      出错返回错误码。
    //  Remarks:
    //      当上下文片段不在段落边界终止时，将加入指定的省略符号。
    //      result buffer的长度至少需要保持在 snippetLen + 2*ellipsisLen + 1 以上。
    //  Availability:
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetFindTextSnippet(DK_UINT pos, const DK_WCHAR* text, const DK_WCHAR* ellipsis, DK_UINT snippetLen, 
                                             DK_WCHAR* result, DK_UINT* startPosInSnippet, DK_UINT* endPosInSnippet) = 0;

    //-------------------------------------------
    //  Summary:
    //      通过起始位置和结束位置获得文本(前闭后开)
    //  Parameters:
    //      [in] startPos               - 章内起始流式坐标
    //      [in] endPos                 - 章内结束流式坐标
    //      [out] ppResultText          - 接收查找的文本
    //  Return Value:
    //      出错返回错误码,正确返回DKR_OK
    //  Remarks:
    //      startPos与endPos为左闭右开区间。
    //      必须调用FreeText释放获取的文本。
    //  Availability:
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetTextOfRange(DK_UINT startPos, DK_UINT endPos, DK_WCHAR** ppResultText) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放由GetTextOfRange获取的文本
    //  Parameters:
    //      [in] resultText             - 待释放的文本
    //  Return Value:
    //      None
    //  Remarks:
    //      None
    //  Availability:
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeText(DK_WCHAR* resultText) = 0;

#if DKE_SUPPORT_DUMP
    //-------------------------------------------
    //  Summary:
    //      Dump辅助，便于调试诊断
    //  Parameters:
    //      None
    //  Remarks:
    //      从MobiLib 1.0.8开始支持
    //-------------------------------------------
    virtual DK_VOID Dump() = 0;
#endif
};

//===========================================================================

#endif // #__KERNEL_MOBIKIT_MOBILIB_EXPORT_IDKMBOOK_H__
