//===========================================================================
//  Summary:
//      ePub图书对象类。
//  Usage:
//      Null
//  Remarks:
//      Null
//  Date:
//      2011-09-16
//  Author:
//      Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEBOOK_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEBOOK_H__

//===========================================================================

#include "DKEDef.h"

//===========================================================================

class IDKEPage;
class IDKETocPoint;
class IDKEComicsFrame;

//===========================================================================

class IDKEBook
{
public:
    //-------------------------------------------
    //  Summary:
    //      设置正文字号大小。
    //  Parameters:
    //      [in] fontSize        - 默认字号大小。
    //  Return Value:
    //      Null
    //  Remarks:
    //      正文字号只是一个参考值，将影响所有和此参考值相关的字号，比如标题文字。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetBodyFontSize(DK_DOUBLE fontSize) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置行间距。
    //  Parameters:
    //      [in] lineGap        - 行间距倍数。
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetLineGap(DK_DOUBLE lineGap) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置段间距。
    //  Parameters:
    //      [in] paraSpacing    - 段间距倍数。
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetParaSpacing(DK_DOUBLE paraSpacing) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置Tab大小。
    //  Parameters:
    //      [in] tabStop        - Tab的大小，以字符宽度为单位。
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetTabStop(DK_DOUBLE tabStop) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置首行缩进。
    //  Parameters:
    //      [in] indent            - 默认字号大小。
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetFirstLineIndent(DK_DOUBLE indent) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置文字颜色。
    //  Parameters:
    //      [in] textColor      - 文本颜色值。
    //  Return Value:
    //      Null
    //  Remarks:
    //      此接口将设置所有输出文字的颜色，而不采用 ePub 内指定的颜色，
    //      调用 ResetAllColor 接口将恢复 ePub 原色设置。
    //      该接口无需重新解析和排版，可在 ParseContent 后调用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
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
    //      此接口将重置所有输出文字的颜色，恢复 ePub 原色设置。
    //      该接口无需重新解析和排版，可在 ParseContent 后调用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
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
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetTypeSetting(DKTYPESETTING typeSetting) = 0;

public:
    //-------------------------------------------
    //  Summary:
    //      进行解析的初始化操作。
    //  Parameters:
    //      [in] pCallBackManager    - 回调接口管理器。
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      在执行 ParseContent 前必须调用此接口，用于回调函数设定、图书基本信息解析等操作。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode PrepareParseContent(IDKECallBackManager *pCallBackManager) = 0;

    //-------------------------------------------
    //  Summary:
    //      解析ePub图书内容。
    //  Parameters:
    //      [in] option             - 解析器选项，参见 DKE_PARSER_OPTION 结构说明。
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      必须在 PrepareParseContent 调用之后才可调用此接口，否则将失败。 
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode ParseContent(const DKE_PARSER_OPTION& option) = 0;

    //-------------------------------------------
    //  Summary:
    //      导出书籍内使用到的字体
    //  Parameters:
    //      [out] pppFontFamilys     - 查找到的font family
    //      [out] pFamilyCount       - font family的数目
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      必须在 PrepareParseContent 调用之后才可调用此接口，否则将失败。
    //      仅针对多看书有效，调用FreeBookFontFamily释放资源
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode ResolveBookFontFamily(DK_WCHAR*** pppFontFamilys, DK_UINT* pFamilyCount) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放导出书籍字体的结果
    //  Parameters:
    //      [in] ppFontFamilys     - 查找到的font family
    //      [in] familyCount       - font family的数目
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      必须在 PrepareParseContent 调用之后才可调用此接口，否则将失败。
    //      仅针对多看书有效
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeBookFontFamily(DK_WCHAR** ppFontFamilys, DK_UINT familyCount) = 0;

    //-------------------------------------------
    //  @Deprecated
    //  Summary:
    //      将流式坐标转换为章节HTML文本的原始字节偏移量
    //  Parameters:
    //      [in] position           - 流式坐标
    //      [out] chapterIndex      - 章序号，从0开始
    //      [out] offset            - 章节HTML文本的原始字节偏移量
    //  Return:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      传入的FlowPosition必须是合法的，否则会返回失败。适用于书签等单个位置的转换。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode DKFlowPosition2Offset(const DK_FLOWPOSITION &position, DK_UINT* pChapterIndex, DK_OFFSET* pOffset) = 0;

    //-------------------------------------------
    //  @Deprecated
    //  Summary:
    //      将章节HTML文本的原始字节偏移量转换为流式坐标
    //  Parameters:
    //      [in] chapterIndex           - 章序号，从0开始
    //      [in] offset                 - 章节HTML文本的原始字节偏移量
    //      [out] pPposition            - 流式坐标
    //  Return:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      传入的offset必须是合法的，否则会返回失败。适用于书签等单个位置的转换。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode DKOffset2FlowPosition(DK_UINT chapterIndex, DK_OFFSET offset, DK_FLOWPOSITION* pPosition) = 0;

    //-------------------------------------------
    //  Summary:
    //      将用flow position表示的区间转换为以章内字节偏移量表示的区间
    //  Parameters:
    //      [in] startPos                   - 区间开始flow position
    //      [in] endPos                     - 区间结束flow position
    //      [out] pStartChapterIndex        - 开始位置所在的章的序号
    //      [out] pStartOffset              - 章内开始位置的字节偏移量
    //      [out] pEndChapterIndex          - 结束位置所在的章的序号
    //      [out] pEndOffset                - 章内结束位置的字节偏移量
    //  Return:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      传入的区间为左闭右开的区间即[startPos, endPos);
    //      传出的区间也为左闭右开的区间即[(pStartChapterIndex, pStartOffset), (pEndChapterIndex, pEndOffset))。
    //      当start position非法时则向后寻到第一个合法的位置，并计算该位置的字节偏移量。
    //      当end position非法是则向前寻到第一个合法的位置，并计算该位置的后一个位置的字节偏移量。
    //      适用于书摘，批注等位置区间的转换。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode DKFlowPositionRange2OffsetRange(const DK_FLOWPOSITION& startPos, 
                                                        const DK_FLOWPOSITION& endPos, 
                                                        DK_UINT* pStartChapterIndex, 
                                                        DK_OFFSET* pStartOffset, 
                                                        DK_UINT* pEndChapterIndex, 
                                                        DK_OFFSET* pEndOffset) = 0;

    //-------------------------------------------
    //  Summary:
    //      将用章内字节偏移量表示的区间转换为以flow position表示的区间
    //  Parameters:
    //      [in] startChapterIndex          - 开始位置所在的章的序号
    //      [in] startOffset                - 章内开始位置的字节偏移量
    //      [in] endChapterIndex            - 结束位置所在的章的序号
    //      [in] endOffset                  - 章内结束位置的字节偏移量
    //      [in] pStartPos                  - 区间开始flow position
    //      [in] pEndPos                    - 区间结束flow position
    //  Return:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      传入的区间为左闭右开的区间即[(startChapterIndex, startOffset), (endChapterIndex, endOffset));
    //      传出的区间也为左闭右开的区间[pStartPos, pEndPos)。
    //      当start offset非法时则向后寻到第一个合法的位置，并计算该位置的flow position。
    //      当end offset非法是则向前寻到第一个合法的位置，并计算该位置的后一个位置的flow position。
    //      适用于书摘，批注等位置区间的转换。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode DKOffsetRange2FlowPositionRange(DK_UINT startChapterIndex, 
                                                        DK_OFFSET startOffset, 
                                                        DK_UINT endChapterIndex, 
                                                        DK_OFFSET endOffset, 
                                                        DK_FLOWPOSITION* pStartPos, 
                                                        DK_FLOWPOSITION* pEndPos) = 0;

    //-------------------------------------------
    //  Summary:
    //      通过extension.xml里面的patch信息和patch文件将旧的Offset转换到新的
    //  Parameters:
    //      [in]  pSrcRevision          - 旧书籍版本号
    //      [in]  pDestRevision         - 新书籍版本号
    //      [in]  pChapterID            - 章ID
    //      [in]  srcOffset             - 旧Offset
    //      [out] pDestOffset           - 新Offset
    //      [out] pDeleted              - 当前章是否已被删除
    //  Return:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode UpdateOffsetByPatch(const DK_WCHAR *pSrcRevision, const DK_WCHAR *pDestRevision, 
                                              const DK_WCHAR *pChapterID, DK_OFFSET srcOffset, 
                                              DK_OFFSET *pDestOffset, DK_BOOL *pDeleted) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放加载的patch信息，用于释放内存
    //  Parameters:
    //  Return:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      调用后UpdateOffsetByPatch仍可使用，此接口仅为内存开销考虑
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID ReleasePatches() = 0;

    //-------------------------------------------
    //  Summary:
    //      根据章节序号获取该章的书内标识ID
    //  Parameters:
    //      [in] chapterIndex           - 章序号，从0开始
    //  Return:
    //      如果成功返回ID字符串，否则返回NULL
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual const DK_WCHAR* GetChapterID(DK_UINT chapterIndex) const = 0;

    //-------------------------------------------
    //  Summary:
    //      根据章节的书内标识ID获取该章的序号
    //  Parameters:
    //      [in] pChapterID           - 章节的书内标识ID
    //  Return:
    //      如果成功返回章的序号，否则返回-1
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_INT GetChapterIndex(const DK_WCHAR* pChapterID) const = 0;

    //-------------------------------------------
    //  Summary:
    //      根据章节的书内URL获取该章的序号
    //  Parameters:
    //      [in] pChapterUrl           - 章节的书内URL
    //  Return:
    //      如果成功返回章的序号，否则返回-1
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_INT GetChapterIndexByURL(const DK_WCHAR* pChapterURL) const = 0;

    //-------------------------------------------
    //  Summary:
    //      重定向某个chapter id指向的chapter HTML到pChapterPath指向的HTML
    //  Parameters:
    //      [in] pChapterId         - 需要重定向的chapter id
    //      [in] pChapterPath       - 定向后的chapter HTML
    //      [in] isEncrypted        - 定向后的HTML是否被加密
    //  Return:
    //      如果成功返回DKR_OK，否则返回失败
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //      调用之前应用层必须手动清除原来章节已解析的数据。
    //      若container为folder，pChapterPath为相对于文件夹的全路径；
    //      若container为ZIP，pChapterPath为压缩包内全路径；
    //  Availability:
    //      从ePubLib 2.4.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode RedirectChapter(const DK_WCHAR* pChapterId, const DK_WCHAR* pChapterPath, DK_BOOL isEncrypted) = 0;
    
public:
    //-------------------------------------------
    //  Summary:
    //      获取文档信息。
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回图书元数据。
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual const DKEBOOKINFO* GetBookInfo() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取书籍的ID
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回书籍ID字串。
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual const DK_WCHAR* GetBookID() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取书籍的分卷文件ID
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回书籍的分卷文件ID字串。
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual const DK_WCHAR* GetResourceID() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取书籍的修订版本号
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回书籍修订版本号字串。
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual const DK_WCHAR* GetBookRevision() const = 0;

    //-------------------------------------------
    //  @Deprecated
    //  Summary:
    //      获取书籍的编辑工具dobby的版本号
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回书籍的编辑工具dobby的版本号字串。
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。但接口已被废弃，建议使用GetStudioVersion接口
    //-------------------------------------------
    virtual const DK_WCHAR* GetDobbyVersion() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取书籍的编辑工具的版本号
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回书籍的编辑工具的版本号字串。
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual const DK_WCHAR* GetStudioVersion() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取书籍版本号。
    //  Parameters:
    //      [in] majorVersion      - 主版本号。
    //      [in] minorVersion      - 次版本号。
    //  Return Value:
    //      DKR_OK     : 成功
    //      DKR_FAILED : 失败
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.1.2 开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetDuokanBookVersion(DK_INT &majorVersion, DK_INT &minorVersion) const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取书籍的内容的类型
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回书籍的内容的类型的枚举。
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DKE_BOOKCONTENT_TYPE GetBookContentType() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取书籍排版方向
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回书籍的内容的类型的枚举。排版方向
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DKEWRITINGOPT GetBookWritingOpt() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取内容书籍展现方式
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回书籍的内容的展现方式的枚举和标识量
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DKEDISPLAYOPT GetBookDisplayOpt() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取图书封面数据。
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回图书封面数据信息，如果失败则返回DK_NULL。
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual const DKFILEINFO* GetBookCover() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取图书章节总数。
    //  Parameters:
    //      Null
    //  Return Value:
    //      章节总数。
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_LONG GetChapterCount() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取文档的某一页。
    //  Parameters:
    //      [in] pageNum        - 整书范围内的页码，从0开始。
    //  Return Value:
    //      返回页对象指针。
    //  Remarks:
    //      该接口在 DKE_PARSER_PAGETABLE_CHAPTER 模式下无效。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual IDKEPage* GetPage(DK_LONG pageNum) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据流式坐标获取某一页。该流式坐标落在页的区域内即可
    //  Parameters:
    //      [in] pos        - 流式坐标。
    //  Return Value:
    //      返回页对象指针。
    //  Remarks:
    //      如果指定的位置还未完成分页点计算，则将返回DK_NULL。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual IDKEPage* GetPage(const DK_FLOWPOSITION& pos) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取文档的某一页。
    //  Parameters:
    //      [in] chapterIndex       - 章节索引，从0开始。
    //      [in] pageNumOfChapter    - 该章节范围内的页码，从0开始。
    //  Return Value:
    //      返回页对象指针。
    //  Remarks:
    //      如果指定的位置还未完成分页点计算，则将返回DK_NULL。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual IDKEPage* GetPageOfChapter(DK_LONG chapterIndex, DK_LONG pageNumOfChapter) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页数。
    //  Parameters:
    //      Null
    //  Return Value:
    //      如果成功返回总页数，否则将返回-1。
    //  Remarks:
    //      该接口在 DKE_PARSER_PAGETABLE_CHAPTER 模式下无效。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_LONG GetPageCount() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页数。
    //  Parameters:
    //      [in] chapterIndex       - 章节索引，从0开始。
    //  Return Value:
    //      如果成功返回该章节的页数，否则将返回-1。
    //  Remarks:
    //      如果指定章节未完成分页点计算，则将返回-1。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_LONG GetPageCountOfChapter(DK_LONG chapterIndex) const = 0;

    //-------------------------------------------
    //  Summary:
    //      通过流式坐标获取章内页码。流式坐标落在页的区间即可
    //  Parameters:
    //      [in] pos            - 流式坐标。
    //  Return Value:
    //      如果成功返回章内页码，否则将返回-1。页码从0开始。
    //  Remarks:
    //      如果指定章节未完成分页点计算，则将返回-1。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_LONG GetPageNumOfChapter(const DK_FLOWPOSITION& pos) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取文档的目录结构。
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回目录接口指针。
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual IDKETocPoint* GetTocRoot() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取文档某一章节的类型，如插图页
    //  Parameters:
    //      [in] chapterIndex       - 章节索引，从0开始。
    //  Return Value:
    //      DKE_CHAPTER_TYPE 章节类型的枚举
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DKE_CHAPTER_TYPE GetChapterType(DK_LONG chapterIndex) const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取文档某一章节的文件大小
    //  Parameters:
    //      [in] chapterIndex       - 章节索引，从0开始。
    //  Return Value:
    //      文件大小，如果失败（如不存在）返回-1
    //  Remarks:
    //      在PrepareParseContent接口调用之后即可使用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_LONG GetChapterSize(DK_LONG chapterIndex) const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取文档某一章节内的某个锚点对应的流式坐标
    //  Parameters:
    //      [in] chapterIndex       - 章节索引，从0开始。
    //      [in] pAnchor            - 锚点的名称
    //      [out] pPos              - 锚点对应的流式坐标
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      返回的流式坐标的chapterIndex与传入参数chapterIndex必须一致。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetAnchorPosition(DK_LONG chapterIndex, const DK_WCHAR* pAnchor, DK_FLOWPOSITION* pPos) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取文档某一章节内的某个锚点对应的流式坐标区间
    //  Parameters:
    //      [in] chapterIndex       - 章节索引，从0开始。
    //      [in] pAnchor            - 锚点的名称
    //      [out] pStartPos         - 锚点对应的起始流式坐标
    //      [out] pEndPos           - 锚点对应的结束流式坐标
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      返回的流式坐标的chapterIndex与传入参数chapterIndex必须一致。
    //  Availability:
    //      从ePubLib 2.7.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetAnchorPosRange(DK_LONG chapterIndex, const DK_WCHAR* pAnchor, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取文档某一章节内的MediaOverlay信息
    //      暂时只支持同步字幕音轨信息
    //  Parameters:
    //      [in] chapterIndex       - 章节索引，从0开始。
    //      [out] pParInfos         - 同步字幕音轨信息
    //  Return Value:
    //      同步字幕音轨信息数量，0表示没有。
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6开始支持。
    //-------------------------------------------
    virtual DK_UINT GetChapterMediaOverlayParInfo(DK_LONG chapterIndex, DKE_SMIL_PAR_INFO*& pParInfos) = 0;
    virtual DK_VOID FreeParInfo(DKE_SMIL_PAR_INFO* pParInfos, DK_UINT nCount) = 0;
    virtual DK_BOOL GetChapterMediaByUrl(DK_LONG chapterIndex, const DK_WCHAR* url, DKSTREAMINFO*& mediaInfo) = 0;
    virtual DK_VOID FreeChapterMediaInfo(DKSTREAMINFO* mediaInfo) = 0;

public:
    //-------------------------------------------
    //  Summary:
    //      创建内置分页表数据。
    //  Parameters:
    //      [in] infoFilePath       - 内置分页表数据文件路径。
    //      [in] msType             - 度量单位。
    //      [in] dpi                - 设备的 dpi 信息。
    //      [in] envArray           - 环境参数队列。
    //      [in] envCount           - 环境队列参数队列长度。
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      必须在 PrepareParseContent 调用之后才可调用此接口，否则将失败。
    //      必须保证调用该接口之前完成了所有排版相关参数的设定，否则生成的分页表数据无效。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode CreatePageBreakInfo(const DK_WCHAR* infoFilePath,
                                              DK_MEASURE_TYPE msType,
                                              DK_LONG dpi,
                                              const DKE_PAGEBREAK_ENV* envArray,
                                              DK_UINT envCount) = 0;

    //-------------------------------------------
    //  Summary:
    //      输出加密的图书文件。
    //  Parameters:
    //      [in] encryptedBookPath  - 加密。
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode ExportEncryptedBook(const DK_WCHAR* encryptedBookPath, const DKE_DRMKEY *pKey = DK_NULL, DK_BOOL compressFirst = DK_FALSE) = 0;

public:
    //-------------------------------------------
    //  Summary:
    //      在全书内查找文本，必须在 PrepareParseContent 之后调用。
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
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode FindTextInBook(const DK_FLOWPOSITION& startPos,
                                         const DK_WCHAR* text,
                                         DK_UINT maxResultCount,
                                         DK_FLOWPOSITION** findResults,
                                         DK_UINT* resultCount) = 0;

    //-------------------------------------------
    //  Summary:
    //      在某章节内查找文本，必须在 PrepareParseContent 之后调用。
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
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode FindTextInChapter(const DK_FLOWPOSITION& startPos,
                                            const DK_WCHAR* text,
                                            DK_UINT maxResultCount,
                                            DK_FLOWPOSITION** findResults,
                                            DK_UINT* resultCount) = 0;

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
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeFindTextResults(DK_FLOWPOSITION* findResults) = 0;

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
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetFindTextSnippet(const DK_FLOWPOSITION& pos,
                                             const DK_WCHAR* text,
                                             const DK_WCHAR* ellipsis,
                                             DK_UINT snippetLen,
                                             DK_WCHAR* result,
                                             DK_UINT* startPosInSnippet,
                                             DK_UINT* endPosInSnippet) = 0;

    //-------------------------------------------
    //  Summary:
    //      获得一章的全部文本
    //  Parameters:
    //      [in] chapterIndex           - 章节索引号,0开始
    //      [out] resultText            - 接收查找的文本
    //  Return Value:
    //      出错返回错误码,正确返回DKR_OK
    //  Remarks:
    //      必须调用FreeChapterText释放获取的文本。      
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetChapterText(DK_LONG chapterIndex, DK_WCHAR** resultText) = 0;

    //-------------------------------------------
    //  Summary:
    //      通过章内起始流式坐标和结束流式坐标获得一章的某段文本
    //  Parameters:
    //      [in] startPos               - 章内起始流式坐标
    //      [in] endPos                 - 章内结束流式坐标
    //      [out] ppResultText          - 接收查找的文本
    //  Return Value:
    //      出错返回错误码,正确返回DKR_OK
    //  Remarks:
    //      startPos与endPos为左闭右开区间。
    //      startPos与endPos的chapterIndex必须一致，否则返回错误。
    //      必须调用FreeChapterText释放获取的文本。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetChapterTextOfRange(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DK_WCHAR** ppResultText) = 0;

    //-------------------------------------------
    //  Summary:
    //      通过章内起始流式坐标和结束流式坐标获得一章的某段HTML富文本
    //  Parameters:
    //      [in] startPos               - 章内起始流式坐标
    //      [in] endPos                 - 章内结束流式坐标
    //      [out] ppRichText            - 导出的富文本
    //  Return Value:
    //      出错返回错误码,正确返回DKR_OK
    //  Remarks:
    //      startPos与endPos为左闭右开区间。
    //      startPos与endPos的chapterIndex必须一致，否则返回错误。
    //      必须调用FreeChapterText释放获取的文本。
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetChapterRichTextOfRange(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DK_WCHAR** ppRichText) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放由GetChapterText获取的文本
    //  Parameters:
    //      [in] resultText             - 待释放的文本
    //  Return Value:
    //      None
    //  Remarks:
    //      None
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeChapterText(DK_WCHAR* resultText) = 0;

    //-------------------------------------------
    //  Summary:
    //      通过章内起始流式坐标和结束流式坐标获得一章的某段文本，如果范围内出现随文元素(如随文图片等等)，则记录alt信息。
    //  Parameters:
    //      [in] startPos                      - 起始坐标位置
    //      [in] endPos                        - 结束坐标位置
    //      [out] pTextContentInfo             - 文本内容（包含随文元素信息）
    //  Return Value:
    //      DK_ReturnCode
    //  Remarks:
    //      若startPos或endPos超出了一页的边界则将其设置到页内与其最近的点。
    //      返回的文本内容指针在使用完毕后必须调用FreeChapterTextEx接口释放。
    //  Availability:
    //      从ePubLib 2.2.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetChapterTextOfRangeEx(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DKE_TEXT_CONTENT_INFO* pTextContentInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放由GetChapterTextOfRangeEx获取的文本内容（包含随文元素信息）。
    //  Parameters:
    //      [in] pTextContentInfo            - 文本内容指针
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.2.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeChapterTextEx(DKE_TEXT_CONTENT_INFO* pTextContentInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据字符索引返回对应的DK_FLOWPOSITION
    //  Parameters:
    //      [in] chapterIndex           -   章节索引
    //      [in] charOffset:            -   到章首的字符偏移量
    //      [out] flowPosition          -   索引对应的flow position
    //                                      正常情况即使越界也返回
    //                                      本章最后一个字符对应的flow position,
    //  Return:
    //      DKR_OK
    //      出错返回错误码
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetFlowPositionByCharOffsetForChapter(DK_LONG chapterIndex, DK_UINT charOffset, DK_FLOWPOSITION* flowPosition) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置停止分页的标志
    //  Parameters:
    //      None
    //  Return:
    //      None
    //  Remarks:
    //      该标志设置不代表parse线程会立即返回,
    //      但parse线程会在每排一页后检测该标志量
    //      为DK_TRUE则中断返回,应用需等待parse线程终止后再在book上进行重排或关闭等操作
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetStopParsingFlag() = 0;

    //-------------------------------------------
    //  Summary:
    //      清除某章已排版的页面     
    //  Parameters:
    //      [in] chapterIndex       - 章节索引
    //  Return Value:
    //      Null。
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID ClearParsedPagesForChapter(DK_LONG chapterIndex) = 0;

    //-------------------------------------------
    //  Summary:
    //      清除某章已排版的页面并该章的内部解析数据     
    //  Parameters:
    //      [in] chapterIndex       - 章节索引
    //  Return Value:
    //      Null。
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0.9开始支持。
    //-------------------------------------------
    virtual DK_VOID ClearChapterData(DK_LONG chapterIndex) = 0;

    //-------------------------------------------
    //  Summary:
    //      清除所有已排版的页面     
    //  Parameters:
    //      Null
    //  Return Value:
    //      Null。
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID ClearAllParsedPages() = 0;

    //-------------------------------------------
    //  Summary:
    //     校验分页表数据是否合法     
    //  Parameters:
    //      [in] pageBreakFile      - 分页表文件
    //      [in] msType             - 度量单位。
    //      [in] dpi                - 设备的 dpi 信息。
    //  Return Value:
    //      Null。
    //  Remarks:
    //      调用此函数前其它排版参数必须与分页表生成时一致，否则会失败
    //  Availability:
    //      从ePubLib 1.0.10开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode VerifyPageBreakFile(const DK_WCHAR* pageBreakFile, DK_MEASURE_TYPE msType, DK_LONG dpi) = 0;

#if DKE_SUPPORT_FONT_DOWNLOAD
	//-------------------------------------------
	//	Summary:
	//	    生成下载字体 	
	//	Parameters:
	//		[in] downloadFontDir        - 生成下载字体的目录,必须存在
	//	Return Value:
	//		成功return DKR_OK,失败其它
	//	Remarks:
	//		在此之前需要调用ParseContent,解析完全书
	//  Availability:
	//		从ePubLib 1.0.6开始支持。
	//-------------------------------------------
    virtual DK_ReturnCode GenerateDownloadFonts(const DK_WCHAR* downloadFontDir) = 0;
#endif

#if DKE_SUPPORT_DUMP
	//-------------------------------------------
	//	Summary:
	//	    Dump辅助,便于调试诊断 	
	//	Parameters:
    //      None
    //  Remarks:
	//		从ePubLib 1.0.7开始支持。
	//-------------------------------------------
    virtual DK_VOID Dump() = 0;
#endif

    //-------------------------------------------
	//	Summary:
	//	    检查书籍内部CSS文件的语法错误
	//	Parameters:
    //      [out]ppErrors           - 检查到的语法错误的数组
    //      [out]pErrorCount        - 检查到的语法错误的个数
    //  Remarks:
	//		必须在 PrepareParseContent 调用之后才可调用此接口。
    //      调用FreeCSSGrammarError释放ppErrors数组
    //  Availability:
    //      从ePubLib 2.0.1开始支持
	//-------------------------------------------
    virtual DK_VOID CheckBookCSSGrammar(DKE_CSS_ERROR** ppErrors, DK_UINT* pErrorCount) = 0;

    //-------------------------------------------
	//	Summary:
	//	    释放检查到的CSS文件的语法错误
	//	Parameters:
    //      [in]pErrors             - CSS语法错误的数组
    //      [in]errorCount          - 数组的大小
    //  Remarks:
	//		必须在 CheckBookCSSGrammar 调用之后才可调用此接口。
    //  Availability:
    //      从ePubLib 2.0.1开始支持
	//-------------------------------------------
    virtual DK_VOID FreeCSSGrammarError(DKE_CSS_ERROR* pErrors, DK_UINT errorCount) = 0;

    //-------------------------------------------
	//	Summary:
	//	    检查书籍内部无用的文件
	//	Parameters:
    //      [out]pppFilePaths       - 检查到的无用的文件路径的数组
    //      [out]pFileCount        - 检查到的无用的文件的个数
    //  Remarks:
	//		必须在 PrepareParseContent 调用之后才可调用此接口。
    //      调用FreeUnusedFiles释放pppFilePaths数组
    //  Availability:
    //      从ePubLib 2.0.1开始支持
	//-------------------------------------------
    virtual DK_VOID CheckBookUnusedFiles(DK_WCHAR*** pppFilePaths, DK_UINT* pFileCount) = 0;

    //-------------------------------------------
	//	Summary:
	//	    释放检测到的无用文件的路径
	//	Parameters:
    //      [in]ppFilePaths       - 检查到的无用的文件路径的数组
    //      [in]fileCoount        - 检查到的无用的文件的个数
    //  Remarks:
	//		必须在 CheckBookUnusedFiles 调用之后才可调用此接口。
    //  Availability:
    //      从ePubLib 2.0.1开始支持
	//-------------------------------------------
    virtual DK_VOID FreeUnusedFiles(DK_WCHAR** ppFilePaths, DK_UINT fileCount) = 0;

    //-------------------------------------------
	//	Summary:
	//	    渲染包内图片到指定的位图和指定的大小
	//	Parameters:
    //      [in]pImagePath          - 包内图片文件全路径
    //      [in]option              - 渲染选项
    //      [in]dstBox              - 目标矩形
    //      [in]alpha               - 透明度,0.0-1.0, 0全透明,1不透明
    //  Remarks:
	//		NULL
    //  Availability:
    //      从ePubLib 2.1.0开始支持
	//-------------------------------------------
    virtual DK_ReturnCode RenderImage(const DK_WCHAR* pImagePath, const DK_FLOWRENDEROPTION& option, const DK_BOX& srcBox, const DK_BOX& dstBox) = 0;

    //-------------------------------------------
	//	Summary:
	//	    获取漫画图书所有帧的个数
	//	Parameters:
    //      Null
    //  Remarks:
	//		在PrepareParseContent之后调用
    //  Availability:
    //      从ePubLib 2.3.1开始支持
	//-------------------------------------------
    virtual DK_LONG GetComicsFrameCount() = 0;

    //-------------------------------------------
	//	Summary:
	//	    根据帧的序号获取帧 （从零开始）
	//	Parameters:
    //      [in] frameIndex         - 帧序号
    //  Remarks:
	//		在ParseContent之后调用
    //  Availability:
    //      从ePubLib 2.3.1开始支持
	//-------------------------------------------
    virtual IDKEComicsFrame* GetComicsFrame(DK_LONG frameIndex) = 0;

    //-------------------------------------------
	//	Summary:
	//	    根据章节序号判断该章节是否是漫画章节
	//	Parameters:
    //      [in] chapterIndex       - 章序号
    //  Remarks:
	//		在ParseContent之后调用
    //  Availability:
    //      从ePubLib 2.3.1开始支持
	//-------------------------------------------
    virtual DK_BOOL IsComicsChapter(DK_LONG chapterIndex) = 0;

    //-------------------------------------------
	//	Summary:
	//	    根据帧的序号获取该帧所在章节的序号 （全部从零开始）
	//	Parameters:
    //      [in] frameIndex       - 帧序号
    //  Remarks:
	//		在ParseContent之后调用
    //  Availability:
    //      从ePubLib 2.3.1开始支持
	//-------------------------------------------
    virtual DK_LONG GetChapterIndexOfFrame(DK_LONG frameIndex) = 0;

    //-------------------------------------------
	//	Summary:
	//	    清除所有已生成的漫画帧
	//	Parameters:
    //      Null
    //  Remarks:
	//		在ParseContent之后调用
    //  Availability:
    //      从ePubLib 2.3.1开始支持
	//-------------------------------------------
    virtual DK_VOID ClearAllComicsFrame() = 0;

    //-------------------------------------------
	//	Summary:
	//	    清除某一章节已生成的漫画帧
	//	Parameters:
    //      Null
    //  Remarks:
	//		在ParseContent之后调用
    //  Availability:
    //      从ePubLib 2.3.1开始支持
	//-------------------------------------------
    virtual DK_VOID ClearComicsFrameForChapter(DK_LONG chapterIndex) = 0;

public:
    virtual ~IDKEBook() {};
};

//===========================================================================

#endif // #ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEBOOK_H__
