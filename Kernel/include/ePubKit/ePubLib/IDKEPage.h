//===========================================================================
//  Summary:
//      ePub图书页对象类。
//  Usage:
//      首先使用Restart，然后循环check MoveNext
//      使用Current获取当前对象，直到MoveNext返回失败为止
//      完成页面对象的枚举工作
//  Remarks:
//      Null
//  Date:
//      2011-09-16
// Author:
//      Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEPAGE_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEPAGE_H__

//===========================================================================

#include "DKEDef.h"
#include "IDKETextIterator.h"
#include "IDKEFootnoteIterator.h"
#include "IDKEImageIterator.h"
#include "IDKEMediaIterator.h"
#include "IDKEComicsFrameIterator.h"
#include "IDKEGallery.h"
#include "IDKEGalleryIterator.h"
#include "IDKEHitableObjIterator.h"
#include "IDKEPreBlockIterator.h"
#include "IDKEImgBlockIterator.h"
#include "IDKEPageUnit.h"

//===========================================================================
class IDKEPage
{
public:
    //-------------------------------------------
    //  Summary:
    //      渲染页面内容。
    //  Parameters:
    //      [in] option             - 绘制选项。
    //      [out] pResult           - 绘制结果。
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode Render(const DK_FLOWRENDEROPTION& option, DK_FLOWRENDERRESULT *pResult) = 0;


    //-------------------------------------------
    //  Summary:
    //      分步渲染接口。
    //      先调用BuildPageElements创建页面对象
    //      再调用FastRender直接绘制页面对象
    //  Parameters:
    //      [in] option             - 绘制选项。
    //      [out] pResult           - 绘制结果。
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.1.1开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode FastRender(const DK_FLOWRENDEROPTION& option, DK_FLOWRENDERRESULT *pResult) = 0;
    virtual DK_ReturnCode BuildPageElements() = 0;

    //-------------------------------------------
    //  Summary:
    //      检查页面是否已存在页面元素
    //  Parameters:
    //      无。
    //  Return Value:
    //      DK_TRUE已存在页面元素；DK_FALSE不存在
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_BOOL CheckPageElements() = 0;

    //-------------------------------------------
    //  Summary:
    //      检查上次绘制是否缺字。
    //  Parameters:
    //      无。
    //  Return Value:
    //      返回是否缺字。
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DKRS_RENDERSTATUS CheckRenderStatus() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放页面渲染后缓存的内容。
    //  Parameters:
    //      Null
    //  Return Value:
    //      Null
    //  Remarks:
    //      必须在 Render 接口调用之后使用，否则无效。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreePageCache() = 0;

    //-------------------------------------------
    //  Summary:
    //      设置页面的某类元素不可见
    //  Parameters:
    //      [in]pageObjType     - 页面元素类型
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetInvisible(DKE_PAGEOBJ_TYPE pageObjType) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面排版后的宽度
    //  Parameters:
    //      Null
    //  Return Value:
    //      页面排版后的宽度
    //  Remarks:
    //      在BuildPageElements之后调用
    //  Availability:
    //      从ePubLib 2.2.1 开始支持。
    //-------------------------------------------
    virtual DK_DOUBLE GetLayoutWidth() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面排版后高度
    //  Parameters:
    //      Null
    //  Return Value:
    //      在页面排版之后调用
    //  Remarks:
    //      在BuildPageElements之后调用
    //  Availability:
    //      从ePubLib 2.2.1开始支持。
    //-------------------------------------------
    virtual DK_DOUBLE GetLayoutHeight() = 0;

public:
    //-------------------------------------------
    //  Summary:
    //      获取页面起止坐标位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回页面第一个元素的流式坐标。
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_FLOWPOSITION GetBeginPosition() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面结束坐标位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回页面最后一个元素的流式坐标。
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_FLOWPOSITION GetEndPosition() = 0;

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
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_WCHAR* GetTextContent() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面指定范围内的文本内容。
    //  Parameters:
    //      [in] startPos           - 起始坐标位置
    //      [in] endPos             - 结束坐标位置
    //  Return Value:
    //      返回位于 [startPos, endPos) 区间内的页面文本内容。
    //  Remarks:
    //      若startPos或endPos超出了一页的边界则将其设置到页内与其最近的点。
    //      返回的文本内容指针在使用完毕后必须调用FreeTextContent接口释放。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_WCHAR* GetTextContentOfRange(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面指定范围内的HTML富文本内容。
    //  Parameters:
    //      [in] startPos           - 起始坐标位置
    //      [in] endPos             - 结束坐标位置
    //  Return Value:
    //      返回位于 [startPos, endPos) 区间内的页面HTML富文本内容。
    //  Remarks:
    //      若startPos或endPos超出了一页的边界则将其设置到页内与其最近的点。
    //      返回的文本内容指针在使用完毕后必须调用FreeTextContent接口释放。
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_WCHAR* GetRichTextContentOfRange(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos) = 0;

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
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeTextContent(DK_WCHAR* pContent) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面指定范围内的文本内容，如果范围内出现随文元素(如随文图片等等)，则记录alt信息。
    //  Parameters:
    //      [in] startPos                      - 起始坐标位置
    //      [in] endPos                        - 结束坐标位置
    //      [out] pTextContentInfo             - 文本内容（包含随文元素信息）
    //  Return Value:
    //      DK_ReturnCode
    //  Remarks:
    //      若startPos或endPos超出了一页的边界则将其设置到页内与其最近的点。
    //      返回的文本内容指针在使用完毕后必须调用FreeTextContentEx接口释放。
    //  Availability:
    //      从ePubLib 2.2.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetTextContentOfRangeEx(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DKE_TEXT_CONTENT_INFO* pTextContentInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放文本内容（包含随文元素信息）。
    //  Parameters:
    //      [in] pTextContentInfo            - 文本内容指针
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.2.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeTextContentEx(DKE_TEXT_CONTENT_INFO* pTextContentInfo) = 0;

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
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetTextRects(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DK_BOX** ppTextRects, DK_UINT* pRectCount) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面指定范围内的行内元素的外接矩形。
    //  Parameters:
    //      [in] startPos           - 起始坐标位置
    //      [in] endPos             - 结束坐标位置
    //      [out] ppInlineEleRects  - 返回行内元素外接矩形数组
    //      [out] rectCount         - 行内元素外接矩形数组的长度
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      获取的是位于 [startPos, endPos) 区间内的行内元素外接矩形数组，使用完毕之后必须调用 FreeRects 接口释放。
    //  Availability: 
    //      从ePubLib 2.2.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetInlineEleRects(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DK_BOX** ppInlineEleRects, DK_UINT* pRectCount) = 0;

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
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeRects(DK_BOX* pRects) = 0;

public:
    //-------------------------------------------
    //  Summary:
    //      获取页面指定两点范围内的选择区域坐标。选择模式为两点越过文字矩形中线即选中
    //  Parameters:
    //      [in] startPoint         - 起始坐标点
    //      [in] endPoint           - 结束坐标点
    //      [out] startPos          - 起始流式坐标位置
    //      [out] endPos            - 结束流式坐标位置
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      必须在 Render 之后调用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetSelectionRange(const DK_POS& startPoint, const DK_POS& endPoint, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;

    //-------------------------------------------
    //  Summary:
    //      在指定的选择模式下获取页面指定两点范围内的选择区域坐标。
    //  Parameters:
    //      [in] startPoint         - 起始坐标点
    //      [in] endPoint           - 结束坐标点
    //      [in] selectionMode      - 选择模式，包括越过中线选中或交叉即选中
    //      [out] startPos          - 起始流式坐标位置
    //      [out] endPos            - 结束流式坐标位置
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      必须在 Render 之后调用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetSelectionRangeByMode(const DK_POS& startPoint, const DK_POS& endPoint, DKE_SELECTION_MODE selectionMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据输入点返回对应的页面对象信息。
    //  Parameters:
    //      [in] point              - 输入点坐标
    //      [out] pObjInfo          - 输入点对应的页面对象信息
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      获取的是位于页面 point 坐标位置的最上层页面对象信息，使用完毕之后必须调用 FreeHitTestObject 接口释放。
    //  Availability: 
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestObject(const DK_POS& point, DKE_HITTEST_OBJECTINFO* pObjInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放页面对象信息。
    //  Parameters:
    //      [in] pObjInfo            - 待释放的页面对象信息
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeHitTestObject(DKE_HITTEST_OBJECTINFO* pObjInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据输入点返回。
    //  Parameters:
    //      [in] point              - 输入点坐标
    //      [out] startPos          - 起始流式坐标位置
    //      [out] endPos            - 结束流式坐标位置
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      必须在 Render 之后调用。
    //  Availability:
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestTextRange(const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据输入点返回。
    //  Parameters:
    //      [in] point              - 输入点坐标
    //      [in] hitTestTextMode    - 以何种模式击中文字，如整句模式，分词模式等
    //      [out] startPos          - 起始流式坐标位置
    //      [out] endPos            - 结束流式坐标位置
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      必须在 Render 之后调用。
    //  Availability:
    //      从ePubLib 2.1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestTextRangeByMode(const DK_POS& point, DKE_HITTEST_TEXT_MODE hitTestTextMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;

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
    //      从ePubLib 1.0开始支持。
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
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeHitTestLink(DKE_LINK_INFO* pLinkInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据输入点返回和内容的展示方式返回脚注信息
    //  Parameters:
    //      [in] point              - 输入点坐标
    //      [out] pFootnoteInfo     - 输入点对应的页面脚注
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      获取的是位于页面 point 坐标位置的最上层页面的脚注信息，使用完毕之后必须调用 FreeHitTestFootnote 接口释放。
    //  Availability: 
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestFootnote(const DK_POS& point, DKE_FOOTNOTE_INFO* pFootnoteInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放脚注信息
    //  Parameters:
    //      [in] pFootnoteInfo     - 页面脚注信息
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeHitTestFootnote(DKE_FOOTNOTE_INFO* pFootnoteInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据footnote指向的ID和内容展示类型获取展示内容句柄
    //  Parameters:
    //      [in] pNoteId            - 脚注ID
    //      [in] showType           - 内容展示的类型
    //      [out] pHandle           - 内容展示句柄
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      使用完毕之后必须调用 FreeContentHandle 接口释放。
    //  Availability: 
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetFootnoteContentHandle(const DK_WCHAR* pNoteId, DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放内容展示句柄
    //  Parameters:
    //      [in] showType           - 展示内容的类型
    //      [in] pContentHandle     - 将要释放的句柄
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeContentHandle(DKE_SHOWCONTENT_TYPE showType, DKE_SHOWCONTENT_HANDLE* pHandle) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据输入点返回画廊信息
    //  Parameters:
    //      [in] point              - 输入点坐标
    //      [out] ppGallery         - 输入点对应的画廊
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      获取的是位于页面 point 坐标位置的最上层页面的脚注信息，使用完毕之后必须调用 FreeHitTestGallery 接口释放。
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestGallery(const DK_POS& point, IDKEGallery** ppGallery) = 0;
    
    //-------------------------------------------
    //  Summary:
    //      释放拾取的画廊信息
    //  Parameters:
    //      [in] pGallery           - 要释放的画廊
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeHitTestGallery(IDKEGallery* pGallery) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据输入点返回PRE块信息
    //  Parameters:
    //      [in] point              - 输入点坐标
    //      [out] pPreBlockInfo     - PRE块相关信息
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      获取的是位于页面 point 坐标位置的最上层页面的脚注信息，使用完毕之后必须调用 FreeHitTestPreBlock 接口释放。
    //  Availability: 
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestPreBlock(const DK_POS& point, DKE_PREBLOCK_INFO* pPreBlockInfo) = 0;
    
    //-------------------------------------------
    //  Summary:
    //      释放拾取的PRE块信息
    //  Parameters:
    //      [in] pPreBlockInfo      - 要释放的pre块信息
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeHitTestPreBlock(DKE_PREBLOCK_INFO* pPreBlockInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据输入点返回自定义图片块信息
    //  Parameters:
    //      [in] point              - 输入点坐标
    //      [out] pImgBlockInfo     - 自定义图片块相关信息
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      获取的是位于页面 point 坐标位置的最上层页面的脚注信息，使用完毕之后必须调用 FreeHitTestImgBlock 接口释放。
    //  Availability: 
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestImgBlock(const DK_POS& point, DKE_IMAGEBLOCK_INFO* pImgBlockInfo) = 0;
    
    //-------------------------------------------
    //  Summary:
    //      释放拾取的自定义图片块信息
    //  Parameters:
    //      [in] pImgBlockInfo      - 要释放的自定义图片块信息
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeHitTestImgBlock(DKE_IMAGEBLOCK_INFO* pImgBlockInfo) = 0;

public:
    //-------------------------------------------
    //  Summary:
    //      获取页面文字对象的迭代器接口。
    //  Parameters:
    //      Null
    //  Return Value:
    //      文字对象迭代器接口指针。
    //  Remarks:
    //      必须在 Render 之后调用，使用完毕之后调用 FreeTextIterator 接口释放。
    //  Availability: 
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual IDKETextIterator* GetTextIterator() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放文字对象迭代器。
    //  Parameters:
    //      [in] textIterator       - 文字对象迭代器
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeTextIterator(IDKETextIterator* pTextIterator) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面脚注的迭代器接口。
    //  Parameters:
    //      Null
    //  Return Value:
    //      页面脚注迭代器接口指针。
    //  Remarks:
    //      必须在 Render 之后调用，使用完毕之后调用 FreeFootnoteIterator 接口释放。
    //  Availability: 
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual IDKEFootnoteIterator* GetFootnoteIterator() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放页面脚注迭代器。
    //  Parameters:
    //      [in] pFootnoteIterator   - 页面脚注迭代器
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeFootnoteIterator(IDKEFootnoteIterator* pFootnoteIterator) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面图片的迭代器接口。
    //  Parameters:
    //      Null
    //  Return Value:
    //      页面图片迭代器接口指针。
    //  Remarks:
    //      必须在 Render 之后调用，使用完毕之后调用 FreeImageIterator 接口释放。
    //  Availability: 
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual IDKEImageIterator* GetImageIterator() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放页面图片迭代器。
    //  Parameters:
    //      [in] pImageIterator   - 页面图片迭代器
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeImageIterator(IDKEImageIterator* pImageIterator) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面音视频的迭代器接口。
    //  Parameters:
    //      Null
    //  Return Value:
    //      页面视频迭代器接口指针。
    //  Remarks:
    //      必须在 Render 之后调用，使用完毕之后调用 FreeMediaIterator 接口释放。
    //  Availability: 
    //      从ePubLib 2.0开始支持。
    //-------------------------------------------
    virtual IDKEMediaIterator* GetMediaIterator() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放页面视频迭代器。
    //  Parameters:
    //      [in] pMediaIterator   - 页面视频迭代器
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeMediaIterator(IDKEMediaIterator* pMediaIterator) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面画廊的迭代器接口。
    //  Parameters:
    //      Null
    //  Return Value:
    //      页面画廊迭代器接口指针。
    //  Remarks:
    //      必须在 Render 之后调用，使用完毕之后调用 FreeGalleryIterator 接口释放。
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual IDKEGalleryIterator* GetGalleryIterator() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放页面画廊迭代器。
    //  Parameters:
    //      [in] pGalleryIterator   - 页面画廊迭代器
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeGalleryIterator(IDKEGalleryIterator* pGalleryIterator) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面可点击对象的迭代器接口。
    //  Parameters:
    //      Null
    //  Return Value:
    //      可点击对象迭代器接口指针。
    //  Remarks:
    //      必须在 Render 之后调用，使用完毕之后调用 FreeHitableObjIterator 接口释放。
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual IDKEHitableObjIterator* GetHitableObjIterator() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放页面可点击对象的迭代器接口。
    //  Parameters:
    //      [in] pHitableObjIterator   - 页面可点击对象迭代器
    //  Return Value:
    //      Null
    //  Remarks:
    //      NULL
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeHitableObjIterator(IDKEHitableObjIterator* pHitableObjIterator) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面PRE块对象的迭代器接口。
    //  Parameters:
    //      Null
    //  Return Value:
    //      PRE块对象迭代器接口指针。
    //  Remarks:
    //      必须在 Render 之后调用，使用完毕之后调用 FreePreBlockIterator 接口释放。
    //  Availability: 
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual IDKEPreBlockIterator* GetPreBlockIterator() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放页面PRE块对象的迭代器接口。
    //  Parameters:
    //      [in] pPreBlockIterator      - 页面PRE块对象迭代器
    //  Return Value:
    //      Null
    //  Remarks:
    //      NULL
    //  Availability: 
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_VOID FreePreBlockIterator(IDKEPreBlockIterator* pPreBlockIterator) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面自定义图片块对象的迭代器接口。
    //  Parameters:
    //      Null
    //  Return Value:
    //      自定义图片块对象迭代器接口指针。
    //  Remarks:
    //      必须在 Render 之后调用，使用完毕之后调用 FreeImgBlockIterator 接口释放。
    //  Availability: 
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual IDKEImgBlockIterator* GetImgBlockIterator() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放页面自定义图片块对象的迭代器接口。
    //  Parameters:
    //      [in] pImgBlockIterator      - 页面页面自定义图片块对象迭代器
    //  Return Value:
    //      Null
    //  Remarks:
    //      NULL
    //  Availability: 
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeImgBlockIterator(IDKEImgBlockIterator* pImgBlockIterator) = 0;


//===========================================================================
//  漫画页面相关接口
//===========================================================================
    //-------------------------------------------
    //  Summary:
    //      获取当前漫画页上的帧数
    //  Parameters:
    //      null
    //  Return Value:
    //      帧数
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_LONG GetFrameCount() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取某一帧的外接矩形，该矩形的坐标系以图片为参照
    //  Parameters:
    //      [in] pageFrameIndex     - 页面上的帧号
    //  Return Value:
    //      帧的外接矩形
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_BOX GetFrameBoundary(DK_LONG pageFrameIndex) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取某一帧的外接矩形，该矩形的坐标系以页面为参照
    //  Parameters:
    //      [in] pageFrameIndex     - 页面上的帧号
    //  Return Value:
    //      帧的外接矩形
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.4.0开始支持。
    //-------------------------------------------
    virtual DK_BOX GetFrameBoxOnPage(DK_LONG pageFrameIndex) = 0;

    //-------------------------------------------
    //  Summary:
    //      通过页面上的帧号获取该帧在全局帧序列中序号（均从零开始）
    //  Parameters:
    //      [in] pageFrameIndex     - 页面上的帧号
    //  Return Value:
    //      帧的外接矩形
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_LONG GetGlobalFrameIndex(DK_LONG pageFrameIndex) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据页面上的某一点（坐标系参照为页面左上角）拾取改点对应的帧号
    //  Parameters:
    //      [in] hitPoint       - 拾取点
    //      [out] pFrameIndex   - 帧号
    //  Return Value:
    //      如果成功则返回DKR_OK
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestComicsFrame(DK_POS point, DK_LONG* pFrameIndex) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取当前漫画页上帧的枚举器
    //  Parameters:
    //      Null
    //  Return Value:
    //      如果成功则返回DKR_OK
    //  Remarks:
    //      调用FreeComicsFrameIterator释放返回值内存
    //  Availability: 
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual IDKEComicsFrameIterator* GetComicsFrameIterator() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放当前漫画页上帧的枚举器
    //  Parameters:
    //      Null
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.3.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeComicsFrameIterator(IDKEComicsFrameIterator* pFrameIterator) = 0;

//===========================================================================
//  页面对象输出相关
//===========================================================================
    //-------------------------------------------
    //  Summary:
    //      获取页面对象block
    //  Parameters:
    //      Null
    //  Return Value:
    //      对象block
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual IDKEPageBlock* GetPageBlockUnit() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放页面对象block
    //  Parameters:
    //      [in] pPageBlockUnit       - 对象block
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_VOID FreePageBlockUnit(IDKEPageBlock* pPageBlockUnit) = 0;

public:
    virtual ~IDKEPage() {};
};

//===========================================================================

#endif // #ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEPAGE_H__
