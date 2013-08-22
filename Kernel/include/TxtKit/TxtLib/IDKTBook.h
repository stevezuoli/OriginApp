//===========================================================================
// Summary:
//        IDKTBook.h
// Usage:
//        txt book接口定义
// Remarks:
//        Null
// Date:
//        2011-11-14
// Author:
//        Peng Feng(pengf@duokan.com)
//===========================================================================
#ifndef __KERNEL_TXTKIT_TXTLIB_IDKTBOOK_H__
#define __KERNEL_TXTKIT_TXTLIB_IDKTBOOK_H__

#include "KernelType.h"
#include "KernelRetCode.h"
#include "IDKTPage.h"

struct DKT_PARSER_OPTION
{
    DK_BOX                      pageBox;            // 页面尺寸。
    DK_MEASURE_TYPE             msType;             // 度量单位，参见 DK_MEASURE_TYPE 枚举值说明。
    DK_LONG                     dpi;                // 输出设备的DPI值，当 msType = DK_MEASURE_PIXEL 时无效。

    DKT_PARSER_OPTION() : msType(DK_MEASURE_PIXEL),
                          dpi(RenderConst::SCREEN_DPI)
    {}
}; 

class IDKTBook
{
public:
    // 空行处理策略
    enum BLANKLINE_STRATEGY
    {
        BLANKLINE_COPY,     // 和数据源保持一致
        BLANKLINE_SINGLE,   // 多个连续空行压缩为一个空行
        BLANKLINE_IGNOREALL // 忽略所有空行
    };

    enum PAGE_POSITION_TYPE
    {
        PREV_PAGE,     // 上一页
        LOCATION_PAGE, // 根据位置跳转的页
        NEXT_PAGE,     // 下一页
    };

    // 排版方向类型
    enum DIRECTION
    {
        HORIZONTAL, // 横排
        VERTICAL    // 竖排
    };

    // 章节标题最大字符数(不包括空格与回车换行)
    enum {MAX_CHAPTER_TITLE=50};
    enum {MAX_FIND_SNIPPET_LEN = 200};
    virtual ~IDKTBook(){};

    //-------------------------------------------
    //    Summary:
    //        设置正文字号大小。
    //    Parameters:
    //        [in] fontSize        - 默认字号大小。
    //    Return Value:
    //        Null
    //    Remarks:
    //  Availability:
    //        从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetFontSize(DK_DOUBLE fontSize) = 0;

    //-------------------------------------------
    //    Summary:
    //        设置行间距。
    //    Parameters:
    //        [in] lineGap              - 行间距倍数。
    //    Return Value:
    //        Null
    //    Remarks:
    //        Null
    //  Availability:
    //        从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetLineGap(DK_DOUBLE lineGap) = 0;

    //-------------------------------------------
    //    Summary:
    //        设置段间距。
    //    Parameters:
    //        [in] paraSpacing          - 段间距倍数。
    //    Return Value:
    //        Null
    //    Remarks:
    //        Null
    //  Availability:
    //        从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetParaSpacing(DK_DOUBLE paraSpacing) = 0;

    //-------------------------------------------
    //    Summary:
    //        设置Tab大小。
    //    Parameters:
    //        [in] tabStop              - Tab的大小，以字符宽度为单位。
    //    Return Value:
    //        Null
    //    Remarks:
    //        Null
    //  Availability:
    //        从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetTabStop(DK_DOUBLE tabStop) = 0;

    //-------------------------------------------
    //    Summary:
    //        设置首行缩进。
    //    Parameters:
    //        [in] indent               - 默认字号大小。
    //    Return Value:
    //        Null
    //    Remarks:
    //        Null
    //  Availability:
    //        从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetFirstLineIndent(DK_DOUBLE indent) = 0;

    //-------------------------------------------
    //    Summary:
    //        设置排版方向。
    //    Parameters:
    //        [in] direction            - 排版方向。
    //    Return Value:
    //        Null
    //    Remarks:
    //        Null
    //  Availability:
    //        从TxtLib 2.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetDirection(DIRECTION direction) = 0;

    //-------------------------------------------
    //    Summary:
    //        设置文字颜色。
    //    Parameters:
    //        [in] textColor            - 文本颜色值。
    //    Return Value:
    //        Null
    //    Remarks:
    //        此接口将设置所有输出文字的颜色
    //        该接口无需重新解析和排版，可在 ParseContent 后调用。
    //  Availability:
    //        从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetTextColor(const DK_ARGBCOLOR& textColor) = 0;

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
    //      从TxtLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetTypeSetting(const DKTYPESETTING &typeSetting) = 0;

    //-------------------------------------------
    //    Summary:
    //        设置空行处理策略。
    //  Parameters:
    //        [in] blankLineStrategy    -   空行处理策略。
    //    Remarks:
    //        如果不设置空行处理策略，处理器默认为TP_BLANKLINE_COPY。
    //  Availability:
    //        从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetBlankLineStrategy(BLANKLINE_STRATEGY blankLineStrategy) = 0;

    //-------------------------------------------
    //  Summary:
    //      分页之前的准备，如编码检测等,只需调用一次
    //  Parameters:
    //      [in]: charsPerLogicalPage   -   每页字符数,页是逻辑页，不一定对应屏幕一页
    //                                      必须保证任何排版参数下不小于屏幕一页,
    //                                      因为代码会根据该参数计算前翻页
    //  Return:
    //      DKR_OK表示成功，否则返回对应错误码
    //-------------------------------------------
    virtual DK_ReturnCode PrepareParseContent(DK_UINT charsPerLogicalPage) = 0;

    //-------------------------------------------
    //  Summary:
    //      获得页数目,必须在ParseContent之后调用
    //  Parameters:
    //      None
    //  Return:
    //      页数目
    //-------------------------------------------
    virtual DK_UINT GetLogicalPageCount() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获得对应逻辑页的offset
    //  Parameters:
    //      [in] pageIndex:             -   页码,从0开始
    //  Return:
    //      对应页指针，出错返回DK_NULL
    //-------------------------------------------
    virtual DK_UINT GetLogicalPageOffsetInBytes(DK_LONG pageIndex) = 0;


    //-------------------------------------------
    //  Summary:
    //     查找文本,必须在PrepareParseContent之后调用
    //  Parameters:
    //      [in] text:                  -   文本
    //      [in] startOffsetInBytes:    -   开始搜索的位置，相对于文件开始，单位是字节
    //                                      必须要求在字符开始位置，不得分在字符中间
    //      [in] maxResultCount:        -   最大返回搜索结果数
    //      [out] resultsOffsetInBytes: -   搜索结果，以到文件头的偏移量表示,长度为resultCount*2
    //                                      第2i与2i+1个结果表示第i个搜索结果的起始和结束位置
    //      [out] resultCount:          -   搜索结果数目
    //  Return:
    //      正常情况即使没有找到结果也返回DKR_OK,以resultCount判断查找是否有结果
    //      出错返回错误码
    //-------------------------------------------
    virtual DK_ReturnCode Find(DK_UINT startOffsetInBytes,  const DK_WCHAR* text, DK_UINT maxResultCount, DK_UINT** resultsOffsetInBytes, DK_UINT* resultCount) = 0;

    //-------------------------------------------
    //  Summary:
    //     释放文本查找结果
    //  Parameters:
    //      [in] resultsOffsetInBytes: -   搜索结果，以到文件头的偏移量表示
    //  Remarks:
    //      释放resultsOffsetInBytes
    //-------------------------------------------
    virtual DK_VOID FreeFindResults(DK_UINT* resultsOffsetInBytes) = 0;

    //-------------------------------------------
    //  Summary:
    //     获取查找的结果展示文本
    //  Parameters:
    //      [in] offsetInBytes:       -   搜索结果偏移量
    //      [in] text:                -   搜索文本
    //      [in] ellipsis:            -   省略号,可以为DK_NULL
    //      [in] snippetLen:          -   预期的展示文本长度
    //      [out] result:             -   展示文本输出指针,必须大于等于snippetLen + 1 + 2*dk_wcslen(ellipsis)个DK_WCHAR长度
    //      [out] startPosInSnippet   -   text在snippet的起始位置
    //      [out] endPosInSnippet     -   text在snippet的结束位置(前闭后开)
    //  Return:
    //      成功返回DKR_OK
    //-------------------------------------------
    virtual DK_ReturnCode GetFindResultSnippet(DK_UINT offsetInBytes, const DK_WCHAR* text,  const DK_WCHAR* ellipsis, DK_UINT snippetLen, DK_WCHAR* result, DK_UINT* startPosInSnippet, DK_UINT* endPosInSnippet) = 0;

    //-------------------------------------------
    //  Summary:
    //      创建页
    //  Parameters:
    //      [in] parseOption:         -   排版选项
    //      [in] pageOffsetInBytes:   -   页相对于文件首的偏移
    //      [in] positionType:        -   页偏移类型
    //      [out] ppPage:             -   出参，接受结果页, 必须由DestroyPage销毁
    //  Return:
    //      成功返回DKR_OK
    //  Remarks:
    //      由于positionType不同，pageOffsetInBytes有不同含义
    //      PREV_PAGE: offset表示待创建页的下一页的起点， 创建的新页会向上排一页
    //                 并让pageOffsetInBytes位置排在该页末一行
    //      LOCATION_PAGE: offset表示待创建页起始位置，实际处理中会从该位置向上找到一个新段开始排版
    //                     并让pageOffsetInBytes处于该页第一行
    //      NEXT_PAGE: offset表示待创建页起始位置，即上一页结束位置
    //                     不须向上搜索到新段开始
    //-------------------------------------------
    virtual DK_ReturnCode CreatePage(const DKT_PARSER_OPTION& parseOption,
                                        DK_UINT pageOffsetInBytes,
                                        PAGE_POSITION_TYPE positionType,
                                        IDKTPage** ppPage) = 0;

    //-------------------------------------------
    //  Summary:
    //      创建页
    //  Parameters:
    //      [in] parseOption:          -   排版选项
    //      [in] pageOffsetInBytes:    -   页相对于文件首的偏移
    //      [in] pageEndOffsetInBytes: -   页的字节大小
    //      [out] ppPage:              -   出参，接受结果页, 必须由DestroyPage销毁
    //  Return:
    //      成功返回DKR_OK
    //  Remarks:
    //-------------------------------------------
    virtual DK_ReturnCode CreatePage(const DKT_PARSER_OPTION& parseOption,
                                     DK_UINT pageOffsetInBytes,
                                     DK_UINT pageEndOffsetInBytes,
                                     IDKTPage** ppPage) = 0;

    //-------------------------------------------
    //  Summary:
    //      销毁页
    //  Parameters:
    //      [in] pPage                  -   待销毁页
    //  Return:
    //      None
    //-------------------------------------------
    virtual DK_VOID DestroyPage(IDKTPage* pPage) = 0;

    //-------------------------------------------
    //  Summary:
    //      计算下一页起始位置的字节偏移
    //  Parameters:
    //      [in] parseOption:               -   排版选项
    //      [in] pageOffsetInBytes:         -   页相对于文件首的偏移
    //      [out] pNextPageOffsetInBytes:   -   出参，下一页起始位置的字节偏移
    //  Return:
    //      成功返回DKR_OK，若失败（比如入参 pageOffsetInBytes 已越过文件结尾）则返回失败值 （DK_FAILED 或 DKR_INVALIDINPARAM）
    //  Remarks:
    //-------------------------------------------
    virtual DK_ReturnCode CalcNextPageOffset(const DKT_PARSER_OPTION& parseOption, DK_UINT pageOffsetInBytes, DK_UINT* pNextPageOffsetInBytes) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据物理位置获取逻辑页编号
    //  Parameters:
    //      [in] pageOffsetInByte       -   相对文件起始位置的字节偏移量
    //  Return:
    //      该偏移量对应的逻辑页编号（0起点)
    //  Remarks:
    //      需要在PrepareParseContent之后调用
    //-------------------------------------------
    virtual DK_LONG GetLogicalPageIndexByOffsetInByte(DK_UINT pageOffsetInByte) = 0;

    //-------------------------------------------
    //  Summary:
    //      智能断章
    //  Parameters:
    //      [out] chapterOffsets         -   *chapterOffset指向一个数组，数组中每一项是该章相对文件起始位置的字节偏移量
    //                                       需要使用 FreeTOC释放
    //      [out] chapterCount           -   章节数
    //  Return:
    //      成功DKR_OK,否则错误码
    //  Remarks:
    //-------------------------------------------
    virtual DK_ReturnCode GetTOC(DK_UINT** chapterOffsets, DK_UINT* chapterCount) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放TOC内容
    //  Parameters:
    //      [in]chapterOffsets          -   待释放的章节位置信息
    //  Return:
    //  Remarks:
    //-------------------------------------------
    virtual DK_VOID FreeTOC(DK_UINT* chapterOffsets) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取智能断章的章节标题
    //  Parameters:
    //      [in] chapterOffset          -   章节标题到文件头的字节偏移
    //      [out] chapterText           -   章节标题，必须能包含至少MAX_CHAPTER_TITLE+1个字符
    //  Return:
    //      成功DKR_OK,否则错误码
    //  Remarks:
    //-------------------------------------------
    virtual DK_ReturnCode GetChapterTitle(DK_UINT chapterOffset, DK_WCHAR* chapterText) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面指定范围内的文本内容。
    //  Parameters:
    //      [in] startPos           - 起始坐标位置
    //      [in] endPos             - 结束坐标位置
    //  Return Value:
    //      返回位于 [startPos, endPos) 区间内的页面文本内容。
    //  Remarks:
    //      返回的文本内容指针在使用完毕后必须调用FreeTextContent接口释放。
    //  Availability:
    //      从TxtLib 1.1.2开始支持。
    //-------------------------------------------
    virtual DK_WCHAR * GetTextContentOfRange(DK_UINT startPos, DK_UINT endPos) = 0;

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
    //      从TxtLib 1.1.1开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeTextContent(DK_WCHAR *pContent) = 0;
};
#endif
