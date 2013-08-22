//===========================================================================
// Summary:
//      IDKPPageEx.h
//      负责 PDF 重排页面数据获取，渲染
// Usage:
//      Null
// Remarks:
//      Null
// Date:
//      2012-01-10
// Author:
//      Liu Hongjia(liuhj@duokan.com)
//===========================================================================

#ifndef __KERNEL_PDFKIT_PDFLIB_IDKPPAGEEX_H__
#define __KERNEL_PDFKIT_PDFLIB_IDKPPAGEEX_H__

#include "DKPRetCode.h"
#include "DKPTypeDef.h"

class IDKPTextIterator;

class IDKPPageEx
{
public:
    virtual ~IDKPPageEx() {}

public:    
    enum {REARRANGE_DEFAULT_INDENT = 50};   // 重排效果默认缩进的固定值
    enum {MAX_INVOLVED_PAGES = 15};         // 拼接重排页时最多使用的原始页面数 
    enum {MAX_SUM_OF_RGB = 500};            // 文本颜色 RGB 三色之和大于改值时，强制设为黑色    

    //-------------------------------------------
    //  Summary:
    //      渲染页面内容。
    //  Parameters:
    //      [in] option             - 绘制选项。
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      Null
    //  Availability:
    //      从 PDFLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode RenderPage (const DK_FLOWRENDEROPTION& option) = 0;
    
    //-------------------------------------------
    //  Summary:
    //      设置重排页面起始位置。
    //  Parameters:
    //      [in] pos - 重排页面第一个元素对应的原文档位置。
    //  Return Value:
    //  Remarks:
    //      Null
    //  Availability:
    //      从 PDFLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetPageStartPos (const DK_FLOWPOSITION& pos) = 0;
    
    //-------------------------------------------
    //  Summary:
    //      设置重排页面结束位置。
    //  Parameters:
    //      [in] pos - 对应重排页面最后无法显示的第一个元素，可做为下一个重排页的起始位置。
    //  Return Value:
    //  Remarks:
    //      Null
    //  Availability:
    //      从 PDFLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetPageEndPos (const DK_FLOWPOSITION& pos) = 0;
    
    //-------------------------------------------
    //  Summary:
    //      获得重排页面起始位置。
    //  Parameters:
    //      [out] pPos - 重排页面第一个元素对应的原文档位置。
    //  Return Value:
    //      获取成功则返回 DK_TRUE，否则返回 DK_FALSE。
    //  Remarks:
    //      Null
    //  Availability:
    //      从 PDFLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_BOOL GetPageStartPos (DK_FLOWPOSITION* pPos) = 0;
    
    //-------------------------------------------
    //  Summary:
    //      获得重排页面结束位置。
    //  Parameters:
    //      [out] pPos - 对应重排页面最后无法显示的第一个元素，可做为下一个重排页的起始位置。
    //  Return Value:
    //      获取成功则返回 DK_TRUE，否则返回 DK_FALSE。
    //  Remarks:
    //      Null
    //  Availability:
    //      从 PDFLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_BOOL GetPageEndPos (DK_FLOWPOSITION* pPos) = 0;

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
    //      从 PDFLib 1.0开始支持。
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
    //      返回的文本内容指针在使用完毕后必须调用FreeTextContent接口释放。
    //  Availability:
    //      从 PDFLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_WCHAR* GetTextContentOfRange(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos) = 0;
    
    //-------------------------------------------
    //  Summary:
    //      释放文本内容。
    //  Parameters:
    //      [in] pContent            - 文本内容指针
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从 PDFLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeTextContent(DK_WCHAR* pContent) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取页面指定范围内的文本外接矩形。
    //  Parameters:
    //      [in] startPos           - 起始坐标位置
    //      [in] endPos             - 结束坐标位置
    //      [out] ppTextRects       - 返回文本外接矩形数组
    //      [out] pRectCount        - 文本外接矩形数组的长度
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      获取的是位于 [startPos, endPos) 区间内的文本外接矩形数组，使用完毕之后必须调用 FreeRects 接口释放。
    //  Availability: 
    //      从 PDFLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetTextRects(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DK_BOX** ppTextRects, DK_UINT* pRectCount) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放矩形区域。
    //  Parameters:
    //      [in] pRects            - 待释放的矩形数组
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从 PDFLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeRects(DK_BOX* pRects) = 0;
    
    //-------------------------------------------
    //  Summary:
    //      获取页面文字对象的迭代器接口。
    //  Parameters:
    //      Null
    //  Return Value:
    //      文字对象迭代器接口指针。
    //  Remarks:
    //      使用完毕之后调用 FreeTextIterator 接口释放。
    //  Availability: 
    //      从 PDFLib 1.0开始支持。
    //-------------------------------------------
    virtual IDKPTextIterator* GetTextIterator() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放文字对象迭代器。
    //  Parameters:
    //      [in] pDKPTextIterator       - 文字对象迭代器
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability: 
    //      从 PDFLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeTextIterator(IDKPTextIterator* pDKPTextIterator) = 0;
    
    //-------------------------------------------
    //  Summary:
    //      根据输入点返回。
    //  Parameters:
    //      [in] point               - 输入点坐标
    //      [out] pStartPos          - 起始流式坐标位置
    //      [out] pEndPos            - 结束流式坐标位置
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      Null
    //  Availability:
    //      从 PDFLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestTextRange(const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;

    //-------------------------------------------
    //  Summary:
    //      根据输入点返回。
    //  Parameters:
    //      [in] point               - 输入点坐标
    //      [in] hitTestTextMode        - 以何种模式击中文字，如整句模式，分词模式等
    //      [out] pStartPos          - 起始流式坐标位置
    //      [out] pEndPos            - 结束流式坐标位置
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      Null
    //  Availability:
    //      从 PDFLib 2.2.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestTextRangeByMode(const DK_POS& point, DKP_HITTEST_TEXT_MODE hitTestTextMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) = 0;

    //-------------------------------------------
    //  Summary:
    //      在指定的选择模式下获取页面指定两点范围内的选择区域坐标。
    //  Parameters:
    //      [in] startPoint             - 开始选中的坐标点
    //      [in] endPoint               - 结束选中的坐标点
    //      [in] selectionMode          - 选择模式，包括越过中线选中或交叉即选中
    //      [out] pStartPos             - 开始坐标点对应文字流式坐标
    //      [out] pEndPos               - 结束坐标点对应文字流式坐标
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从PDFLib 2.1.1开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetSelectionRangeByMode(const DK_POS& startPoint, 
                                                const DK_POS& endPoint, 
                                                DKP_SELECTION_MODE selectionMode, 
                                                DK_FLOWPOSITION* pStartPos, 
                                                DK_FLOWPOSITION* pEndPos) = 0;
};

#endif // #ifndef __KERNEL_PDFKIT_PDFLIB_IDKPPAGEEX_H__
