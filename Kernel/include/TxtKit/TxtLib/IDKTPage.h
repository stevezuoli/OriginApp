//===========================================================================
// Summary:
//        IDKTPage.h
// Usage:
//        txt page接口定义
// Remarks:
//        Null
// Date:
//        2011-11-14
// Author:
//        Peng Feng(pengf@duokan.com)
//===========================================================================
#ifndef __KERNEL_TXTKIT_TXTLIB_IDKTPAGE_H__
#define __KERNEL_TXTKIT_TXTLIB_IDKTPAGE_H__

#include "IDKTTextIterator.h"

class IDKTPage
{
public:
    virtual ~IDKTPage(){};

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
    //      从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode Render(const DK_FLOWRENDEROPTION& option, DK_FLOWRENDERRESULT *result) = 0;

    //-------------------------------------------
    //  Summary:
    //		检查上次绘制是否缺字。
    //  Parameters:
    //      无。
    //	Return Value:
    //		返回是否缺字。
    //	Remarks:
    //		Null
    //  Availability:
    //		从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DKRS_RENDERSTATUS CheckRenderStatus() = 0;

    //-------------------------------------------
    //  Summary:
    //      获得字节表示的页面大小
    //  Parameters:
    //      None
    //  Return Value:
    //      页面大小
    //  Remarks:
    //      Null
    //  Availability:
    //      从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_UINT GetSizeInByte() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获得字符表示的页大小
    //  Parameters:
    //      None
    //  Return Value:
    //      页大小
    //  Remarks:
    //      Null
    //  Availability:
    //      从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_UINT GetSizeInChar() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获得页起始点相对于文件头的字节偏移量
    //  Parameters:
    //      None
    //  Return Value:
    //      偏移量
    //  Remarks:
    //      Null
    //  Availability:
    //      从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_UINT GetOffsetInByte() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获得页起始点相对于文件头的字符偏移量
    //  Parameters:
    //      None
    //  Return Value:
    //      偏移量
    //  Remarks:
    //      Null
    //  Availability:
    //      从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_UINT GetOffsetInChar() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获得当前页全部文本内容 
    //  Parameters:
    //      None
    //  Return Value:
    //      当前页全部文本, 0结尾
    //      调用者不可释放该段文本
    //  Remarks:
    //      None
    //  Availability:
    //      从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual const DK_WCHAR * GetAllText() const = 0;

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
    //      从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual IDKTTextIterator * GetTextIterator() = 0;

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
    //      从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeTextIterator(IDKTTextIterator *pDKTTextIterator) = 0;

public:
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
    //      从TxtLib 1.0开始支持。
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
    //      从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeTextContent(DK_WCHAR *pContent) = 0;

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
    //      从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetTextRects(DK_UINT startPos, DK_UINT endPos, DK_BOX **ppTextRects, DK_UINT *pRectCount) = 0;

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
    //      从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeRects(DK_BOX *pRects) = 0;


    //-------------------------------------------
    //  Summary:
    //      文字点击,如果是英文单词则选中整个单词,汉字则前后找到汉字边界,返回区间前闭后开
    //  Parameters:
    //      [in] pos                    - 点击位置
    //      [out] startOffsetInByte     - 文字区域在文件中起点
    //      [out] endOffsetInByte       - 文字区域在文件中终点
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从TxtLib 1.0开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestTextRange(const DK_POS& pos, DK_UINT* startOffsetInByte, DK_UINT* endOffsetInByte) = 0;
    
    //-------------------------------------------
    //  Summary:
    //      根据输入点返回。
    //  Parameters:
    //      [in] pos                    - 输入点坐标
    //      [in] hitTestTextMode        - 以何种模式击中文字，如整句模式，分词模式等
    //      [out] startOffsetInByte     - 起始流式坐标位置
    //      [out] endOffsetInByte       - 结束流式坐标位置
    //  Return Value:
    //      如果成功返回 DKR_OK。
    //  Remarks:
    //      必须在 Render 之后调用。
    //  Availability:
    //      从TxtLib 2.1.1开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode HitTestTextRangeByMode(const DK_POS& pos, DKT_HITTEST_TEXT_MODE hitTestTextMode, DK_UINT* startOffsetInByte, DK_UINT* endOffsetInByte) = 0;

    //-------------------------------------------
    //  Summary:
    //      文字区域选中, 为拖动引起的区域选中使用。选择模式为两点越过文字矩形中线即选中
    //  Parameters:
    //      [in] startPoint             - 开始选中的坐标点
    //      [in] endPoint               - 结束选中的坐标点
    //      [out] pStartPos             - 开始坐标点对应文字的offset
    //      [out] pEndPos               - 结束坐标点对应文字的offset
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从TxtLib 1.1.2开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetSelectionRange(const DK_POS& startPoint, 
                                            const DK_POS& endPoint, 
                                            DK_UINT* pStartByteOffset, 
                                            DK_UINT* pEndByteOffset) = 0;

    //-------------------------------------------
    //  Summary:
    //      在指定的选择模式下获取页面指定两点范围内的选择区域坐标。
    //  Parameters:
    //      [in] startPoint             - 开始选中的坐标点
    //      [in] endPoint               - 结束选中的坐标点
    //      [in] selectionMode          - 选择模式，包括越过中线选中或交叉即选中
    //      [out] pStartPos             - 开始坐标点对应文字的offset
    //      [out] pEndPos               - 结束坐标点对应文字的offset
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从TxtLib 2.1.1开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetSelectionRangeByMode(const DK_POS& startPoint, 
                                                const DK_POS& endPoint, 
                                                DKT_SELECTION_MODE selectionMode, 
                                                DK_UINT* pStartByteOffset, 
                                                DK_UINT* pEndByteOffset) = 0;
};
#endif
