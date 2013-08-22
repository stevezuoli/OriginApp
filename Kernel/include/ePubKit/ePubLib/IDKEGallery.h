//===========================================================================
//  Summary:
//      ePub图书画廊对象
//  Usage:
//      通过页面拾取逻辑获取当前画廊对象，然后通过该画廊对象操作画廊
//  Remarks:
//      Null
//  Date:
//      2013-03-22
// Author:
//      Zhang Jiafang (zhangjf@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEGALLERY_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEGALLERY_H__

#include "DKEDef.h"

class IDKEFootnoteIterator;
class IDKEHitableObjIterator;
class IDKEImageIterator;

class IDKEGallery
{
public:
    virtual ~IDKEGallery() {}

public:
    //-------------------------------------------
    //  Summary:
    //      获取画廊的起始位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      起始位置
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_FLOWPOSITION GetBeginPosition() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊的结束位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      结束位置
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_FLOWPOSITION GetEndPosition() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊在当前页面的外接矩形。
    //  Parameters:
    //      Null
    //  Return Value:
    //      外接矩形
    //  Remarks:
    //      以页面左上角为原点。
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_BOX GetBoundary() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊图像在当前页面的外接矩形。
    //  Parameters:
    //      Null
    //  Return Value:
    //      外接矩形
    //  Remarks:
    //      以页面左上角为原点。
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_BOX GetImageBoundary() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊图像在当前画廊区域内的外接矩形。
    //  Parameters:
    //      Null
    //  Return Value:
    //      外接矩形
    //  Remarks:
    //      以画廊区域左上角为原点。
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_BOX GetImageBoundaryInGallery() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊第一个出现的标题在当前页面的外接矩形。
    //  Parameters:
    //      Null
    //  Return Value:
    //      外接矩形
    //  Remarks:
    //      以页面左上角为原点。当画廊titleCount==0时无效，titleCount>=1时有效
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_BOX GetFirstTitleBoundary() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊第一个出现的标题在当前画廊区域内的外接矩形。
    //  Parameters:
    //      Null
    //  Return Value:
    //      外接矩形
    //  Remarks:
    //      以画廊区域左上角为原点。当画廊titleCount==0时无效，titleCount>=1时有效
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_BOX GetFirstTitleBoundaryInGallery() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊第二个出现的标题在当前页面的外接矩形。
    //  Parameters:
    //      Null
    //  Return Value:
    //      外接矩形
    //  Remarks:
    //      以页面左上角为原点。当画廊titleCount<=1时无效，titleCount==2时有效
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_BOX GetSecondTitleBoundary() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊第二个出现的标题在当前画廊区域内的外接矩形。
    //  Parameters:
    //      Null
    //  Return Value:
    //      外接矩形
    //  Remarks:
    //      以画廊区域左上角为原点。当画廊titleCount<=1时无效，titleCount==2时有效
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_BOX GetSecondTitleBoundaryInGallery() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊内子单元的个数
    //  Parameters:
    //      Null
    //  Return Value:
    //      字单元的个数
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_INT GetCellCount() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊标题的个数，包括主标题和副标题
    //  Parameters:
    //      Null
    //  Return Value:
    //      字单元的个数
    //  Remarks:
    //      画廊标题的个数最大为2最小为0
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_INT GetTitleCount() const = 0;

    //-------------------------------------------
    //  Summary:
    //      设置当前被激活的画廊子单元
    //  Parameters:
    //      [in] cellIndex          - 画廊子单元的序号， 从0开始
    //  Return Value:
    //      Null
    //  Remarks:
    //      在设置前默认被激活的单元为0号，每切换一个单元必须重新设置当前被激活的单元
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetCurActiveCell(DK_INT cellIndex) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取当前激活的画廊单元的序号
    //  Parameters:
    //      Null
    //  Return Value:
    //      画廊单元的序号，从0开始
    //  Remarks:
    //      若返回-1,则失败
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_INT GetCurActiveCell() const = 0;

    //-------------------------------------------
    //  Summary:
    //      在传入的位图上渲染指定的画廊单元
    //  Parameters:
    //      [in] cellIndex          - 画廊子单元的序号， 从0开始
    //      [in] option             - 渲染选项
    //  Return Value:
    //      如果成功则返回DKR_OK; 否则失败
    //  Remarks:
    //      以画廊的左上点为原点开始渲染
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode RenderCell(DK_INT cellIndex, const DK_FLOWRENDEROPTION& option) = 0;

    //-------------------------------------------
    //  Summary:
    //      在传入的位图上渲染指定的画廊单元的图片
    //  Parameters:
    //      [in] cellIndex          - 画廊子单元的序号， 从0开始
    //      [in] option             - 渲染选项
    //  Return Value:
    //      如果成功则返回DKR_OK; 否则失败
    //  Remarks:
    //      以图片的左上点为原点开始渲染
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode RenderCellImage(DK_INT cellIndex, const DK_FLOWRENDEROPTION& option) = 0;

    //-------------------------------------------
    //  Summary:
    //      在传入的位图上渲染指定的画廊单元的第一个出现的标题
    //  Parameters:
    //      [in] cellIndex          - 画廊子单元的序号， 从0开始
    //      [in] option             - 渲染选项
    //  Return Value:
    //      如果成功则返回DKR_OK; 否则失败
    //  Remarks:
    //      以第一个出现的标题的左上点为原点开始渲染，需要titleCount>=1才有效
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode RenderCellFirstTitle(DK_INT cellIndex, const DK_FLOWRENDEROPTION& option) = 0;

    //-------------------------------------------
    //  Summary:
    //      在传入的位图上渲染指定的画廊单元的第二个出现的标题
    //  Parameters:
    //      [in] cellIndex          - 画廊子单元的序号， 从0开始
    //      [in] option             - 渲染选项
    //  Return Value:
    //      如果成功则返回DKR_OK; 否则失败
    //  Remarks:
    //      以第二个出现的标题的左上点为原点开始渲染，需要titleCount==2才有效
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode RenderCellSecondTitle(DK_INT cellIndex, const DK_FLOWRENDEROPTION& option) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取指定画廊单元的图片信息
    //  Parameters:
    //      [in] cellIndex              - 画廊子单元的序号， 从0开始
    //      [in] pImageInfo             - 图片信息
    //  Return Value:
    //      如果成功则返回DKR_OK; 否则失败
    //  Remarks:
    //      调用FreeCellImageInfo释放资源
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetCellImageInfo(DK_INT cellIndex, DKE_HITTEST_OBJECTINFO* pImageInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放指定画廊单元的图片信息
    //  Parameters:
    //      [in] pImageInfo             - 图片信息
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeCellImageInfo(DKE_HITTEST_OBJECTINFO* pImageInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊当前激活单元内指定范围内的行内元素的外接矩形。
    //  Parameters:
    //      [in] startPos           - 起始坐标位置
    //      [in] endPos             - 结束坐标位置
    //      [out] ppRects           - 返回行内元素外接矩形数组
    //      [out] pCount            - 行内元素外接矩形数组的长度
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      获取的是位于 [startPos, endPos) 区间内的行内元素外接矩形数组，使用完毕之后必须调用 FreeRects 接口释放。
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetInlineEleRects(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DK_BOX** ppRects, DK_UINT* pCount) = 0;

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
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeRects(DK_BOX* pRects) = 0;

    //-------------------------------------------
    //  Summary:
    //      在指定的选择模式下获取画廊当前激活单元内指定两点范围内的选择区域坐标。
    //  Parameters:
    //      [in] startPoint         - 起始坐标点，以页面左上点为原点
    //      [in] endPoint           - 结束坐标点，以页面左上点为原点
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
    virtual DK_ReturnCode GetSelectionRangeByMode(const DK_POS& startPoint, const DK_POS& endPoint, DKE_SELECTION_MODE selectionMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据输入点和击中模式返回点在画廊当前激活单元内部的文字区域。
    //  Parameters:
    //      [in] point              - 输入点坐标，以页面左上点为原点
    //      [in] hitTestTextMode    - 以何种模式击中文字，如整句模式，分词模式等
    //      [out] startPos          - 起始流式坐标位置
    //      [out] endPos            - 结束流式坐标位置
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      null
    //  Availability:
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestTextRangeByMode(const DK_POS& point, DKE_HITTEST_TEXT_MODE hitTestTextMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据输入点返回和内容的展示方式返回画廊当前激活单元内部脚注信息
    //  Parameters:
    //      [in] point              - 输入点坐标，以页面左上点为原点
    //      [out] pFootnoteInfo     - 输入点对应的页面脚注
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      使用完毕之后必须调用 FreeHitTestFootnote 接口释放。
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
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
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeHitTestFootnote(DKE_FOOTNOTE_INFO* pFootnoteInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊当前激活单元内部脚注的迭代器接口。
    //  Parameters:
    //      Null
    //  Return Value:
    //      脚注迭代器接口指针。
    //  Remarks:
    //      使用完毕之后调用 FreeFootnoteIterator 接口释放。
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual IDKEFootnoteIterator* GetFootnoteIterator() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放画廊内部脚注迭代器。
    //  Parameters:
    //      [in] pFootnoteIterator   - 页面脚注迭代器
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeFootnoteIterator(IDKEFootnoteIterator* pFootnoteIterator) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据输入点返回对应的画廊当前激活单元中的链接信息
    //  Parameters:
    //      [in] point              - 输入点坐标，相对于页面左上角
    //      [out] pLinkInfo         - 输入点对应的页面链接信息
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      使用完毕之后必须调用 FreeHitTestLink 接口释放。
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
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
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeHitTestLink(DKE_LINK_INFO* pLinkInfo) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊当前激活单元内部所有可点击对象的迭代器
    //  Parameters:
    //      Null
    //  Return Value:
    //      可点击对象的迭代器
    //  Remarks:
    //      调用FreeHitableObjIterator释放资源
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual IDKEHitableObjIterator* GetHitableObjIterator() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放画廊内部所有可点击对象的迭代器
    //  Parameters:
    //      [in] pHitableObjIterator        - 可点击对象的迭代器
    //  Return Value:
    //      Null
    //  Remarks:
    //      调用FreeHitableObjIterator释放资源
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeHitableObjIterator(IDKEHitableObjIterator* pHitableObjIterator) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取画廊所有单元的可交互图片合在一起后的枚举器
    //  Parameters:
    //      Null
    //  Return Value:
    //      图片枚举器
    //  Remarks:
    //      调用FreeAllCellImageIterator释放资源
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual IDKEImageIterator* GetAllCellImageIterator() = 0;
    
    //-------------------------------------------
    //  Summary:
    //      释放画廊所有单元的可交互图片合在一起后的枚举器
    //  Parameters:
    //      [in] pImageIterator         - 图片枚举器
    //  Return Value:
    //      图片枚举器
    //  Remarks:
    //      null
    //  Availability: 
    //      从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeAllCellImageIterator(IDKEImageIterator* pImageIterator) = 0;

};

#endif // __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEGALLERY_H__

