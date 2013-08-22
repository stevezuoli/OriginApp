//===========================================================================
// Summary:
//      IDKPPageDistiller.h
//      负责页面内容提取器，获取页面内容提取的接口类。 
// Usage:
//      Null
// Remarks:
//      Null
// Date:
//      2011-09-14
// Author:
//      Liu Hongjia(liuhj@duokan.com)
//===========================================================================

#ifndef __KERNEL_PDFKIT_PDFLIB_IDKPPAGEDISTILLER_H__
#define __KERNEL_PDFKIT_PDFLIB_IDKPPAGEDISTILLER_H__

#include "DKPTypeDef.h"

class IDKPPageDistiller
{
public:
    //-------------------------------------------
    // Summary:
    //      判断是否为按原始排版顺序选择。
    // Parameters:
    //      [in] startPoint - 起始点坐标。
    //      [in] endPoint   - 终止点坐标。
    // Returns:
    //      是则返回DK_TRUE，否则为矩形区域选择返回DK_FALSE。
    // SDK Version:
    //      从DKP 1.0开始支持
    //-------------------------------------------
    virtual DK_BOOL IsRawSelectionType(DK_POINT startPoint, DK_POINT endPoint) = 0;

    //-------------------------------------------
    // Summary:
    //      获取指定区域文字的排版方向。
    // Parameters:
    //      [in] startPoint - 起始点坐标。
    //      [in] endPoint   - 终止点坐标。
    // Returns:
    //      返回旋转角度（0-360）。
    // SDK Version:
    //      从DKP 1.0开始支持
    //-------------------------------------------
    virtual DK_DOUBLE GetTextRawDirection(DK_POINT startPoint, DK_POINT endPoint) = 0;

    //-------------------------------------------
    // Summary:
    //      判断输入点是否位于文本区域。
    // Parameters:
    //      [in] point - 点坐标。
    // Returns:
    //      是则返回DK_TRUE，否则返回DK_FALSE。
    // SDK Version:
    //      从DKP 1.0开始支持
    //-------------------------------------------
    virtual DK_BOOL CheckPointOnText(DK_POINT point) = 0;

    //-------------------------------------------
    // Summary:
    //      判断输入点是否位于图像区域。
    // Parameters:
    //      [in] point - 点坐标.
    // Returns:
    //      是则返回DK_TRUE，否则返回DK_FALSE。
    // SDK Version:
    //      从DKP 1.0开始支持
    //-------------------------------------------
    virtual DK_BOOL CheckPointOnImage(DK_POINT point) = 0;

    //-------------------------------------------
    // Summary:
    //      提取输入点所在图像的各种信息。
    // Parameters:
    //      [in] point          - 点坐标.
    //      [out] pImageContent - 图像信息.
    // Returns:
    //      成功则返回DK_TRUE，否则返回DK_FALSE。
    // SDK Version:
    //      从DKP 1.0开始支持
    //-------------------------------------------
    virtual DK_BOOL PickupSelectedImageInfo(DK_POINT point, DKPIMAGECONTENT* pImageContent) = 0;

    //-------------------------------------------
    // Summary:
    //      判断输入点是否位于注释对象上。
    // Parameters:
    //      [in] point       - 点坐标。
    //      [out] annotsInfo - 注释对象的外接矩形和类型的信息。
    // Returns:
    //      是则返回DK_TRUE，否则返回DK_FALSE。
    // SDK Version:
    //      从DKP 2.2.0开始支持
    //-------------------------------------------
    virtual DK_BOOL CheckPointOnClickable(DK_POINT point, DK_CLICKINFO& annotsInfo) = 0;

    //-------------------------------------------
    // Summary:
    //        获取注释对象的动作。
    // Parameters:
    //        [in] point        - 点坐标。
    //        [in] eventType    - 动作类型。
    //        [out] pActionData - 动作的信息。
    // Returns:
    //        成功则返回DK_TRUE，否则返回DK_FALSE。
    // SDK Version:
    //        从DKP 2.2.0开始支持
    //-------------------------------------------
    virtual DK_BOOL GetClickableActionData(DK_POINT point, DK_CLICKEVENT_TYPE eventType, DK_VOID* pActionData) = 0;

public:
    virtual ~IDKPPageDistiller() {}
};

#endif // #ifndef __KERNEL_PDFKIT_PDFLIB_IDKPPAGEDISTILLER_H__
