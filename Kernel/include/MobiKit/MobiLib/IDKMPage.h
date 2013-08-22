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
//      2012-02-27
//  Author:
//      Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_MOBIKIT_MOBILIB_EXPORT_IDKMPAGE_H__
#define __KERNEL_MOBIKIT_MOBILIB_EXPORT_IDKMPAGE_H__

#include "DKMDef.h"

class IDKMTextIterator;
class IDKEImageIterator;

class IDKMPage
{
public:
    virtual ~IDKMPage() {}
public:
    virtual DK_ReturnCode Render(const DK_FLOWRENDEROPTION &option, DK_FLOWRENDERRESULT *pResult) = 0;

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
    //      从MobiLib 1.0.8开始支持。
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
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_VOID FreePageCache() = 0;

public:
    //-------------------------------------------
    //  Summary:
    //      获取页面起始坐标位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      返回页面第一个元素的流式坐标。
    //  Remarks:
    //      Null
    //  Availability:
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_UINT GetBeginPosition() = 0;

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
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_UINT GetEndPosition() = 0;

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
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetTextRects(DK_UINT startPos, DK_UINT endPos, DK_BOX** ppTextRects, DK_UINT* pRectCount) = 0;

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
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeRects(DK_BOX* pRects) = 0;

public:
    //-------------------------------------------
    //  Summary:
    //      获取页面指定两点范围内的选择区域坐标。为越过中线即选中
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
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetSelectionRange(const DK_POS& startPoint, const DK_POS& endPoint, DK_UINT *pStartPos, DK_UINT *pEndPos) = 0;

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
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetSelectionRangeByMode(const DK_POS& startPoint, const DK_POS& endPoint, DKE_SELECTION_MODE selectionMode, DK_UINT* pStartPos, DK_UINT* pEndPos) = 0;

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
    //      从MobiLib 1.0.8开始支持。
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
    //      从MobiLib 1.0.8开始支持。
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
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestTextRange(const DK_POS& point, DK_UINT *pStartPos, DK_UINT *pEndPos) = 0;

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
    virtual DK_ReturnCode HitTestTextRangeByMode(const DK_POS& point, DKE_HITTEST_TEXT_MODE hitTestTextMode, DK_UINT* pStartPos, DK_UINT* pEndPos) = 0;

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
    //      从MobiLib 1.0.8开始支持。
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
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeHitTestLink(DKE_LINK_INFO* pLinkInfo) = 0;

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
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual IDKMTextIterator * GetTextIterator() = 0;

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
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeTextIterator(IDKMTextIterator *pTextIterator) = 0;

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
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual IDKEImageIterator * GetImageIterator() = 0;

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
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeImageIterator(IDKEImageIterator *pImageIterator) = 0;

    //-------------------------------------------
    //  Summary:
    //      判断page是否为第一页
    //  Parameters:
    //      None
    //  Return Value:
    //      True if is first page, false otherwise
    //  Remarks:
    //      Null
    //  Availability: 
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_BOOL IsFirstPage() const = 0;

    //-------------------------------------------
    //  Summary:
    //      判断page是否为最后一页
    //  Parameters:
    //      None
    //  Return Value:
    //      True if is last page, false otherwise
    //  Remarks:
    //      Null
    //  Availability: 
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_BOOL IsLastPage() const = 0;
};

//===========================================================================

#endif // __KERNEL_MOBIKIT_MOBILIB_EXPORT_IDKMPAGE_H__
