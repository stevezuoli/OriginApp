//===========================================================================
//  Summary:
//      排版边界定义。
//  Usage:
//      Null
//  Remarks:
//      Null
//  Date:
//      2012-06-19
//  Author:
//      Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __TITAN_LAYOUTBOUNDARY_ILAYOUTBOUNDARY_H__
#define __TITAN_LAYOUTBOUNDARY_ILAYOUTBOUNDARY_H__

#include "TitanDef.h"
#include "KernelRenderType.h"

class ILayoutBoundary
{
public:
    enum TYPE
    {
        RECT_BOUNDARY, 
        FOLD_LINE_BOUNDARY, 

        UNKNOWN_BOUNDARY
    };

    virtual TYPE BoundaryType() const = 0;

    virtual ILayoutBoundary * CreateCopy() const = 0;

    virtual void Transform(TP_MATRIX &matrix) const = 0;

public:
    //-------------------------------------------
    //  Summary:
    //      返回排版区域上/下边界。
    //  Parameters:
    //      Null
    //  Remarks:
    //      外部检查是否有效
    //  Return:
    //      上/下边界。
    //-------------------------------------------
    virtual double GetTopBound() const = 0;
    virtual double GetBottomBound() const = 0;

    //-------------------------------------------
    //  Summary:
    //      返回排版区域最大宽度。
    //  Parameters:
    //      Null
    //  Remarks:
    //      外部检查是否有效
    //  Return:
    //      最大宽度。
    //-------------------------------------------
    virtual double GetMaxWidth() const = 0;

    //-------------------------------------------
    //  Summary:
    //      查找可排位置的上边界。
    //  Parameters:
    //      [in] dLineWidth   - 目标宽度
    //      [in] dLineHeight  - 目标高度
    //      [in] dBaseTopLine - 查找的起始位置
    //  Remarks:
    //      外部检查是否有效
    //  Return:
    //      查找到的上边界。
    //-------------------------------------------
    virtual double SearchTopLine(double dLineWidth, double dLineHeight, double dBaseTopLine) const = 0;

    //-------------------------------------------
    //  Summary:
    //      返回指定行区域左/右边界。
    //  Parameters:
    //      [in] dTopLine    - 行区域上边界
    //      [in] dBottomLine - 行区域下边界
    //  Remarks:
    //      外部保证传入参数有效性
    //  Return:
    //      左/右边界。
    //-------------------------------------------
    virtual double GetLeftBound(double dTopLine, double dBottomLine) const = 0;
    virtual double GetRightBound(double dTopLine, double dBottomLine) const = 0;

public:
    virtual ~ILayoutBoundary() {}
};

class LayoutBoundaryFactory
{
public:
    static ILayoutBoundary * CreateRectBoundary(double dTop, double dBottom, double dLeft, double dRight);
    static ILayoutBoundary * CreateFoldLineBoundary(double dTop, 
                                                    double dBottom, 
                                                    const std::vector<DK_POS> &vLeftPoints, 
                                                    const std::vector<DK_POS> &vRightPoints);

private:
    LayoutBoundaryFactory();
};

//===========================================================================

#endif // __TITAN_LAYOUTBOUNDARY_ILAYOUTBOUNDARY_H__
