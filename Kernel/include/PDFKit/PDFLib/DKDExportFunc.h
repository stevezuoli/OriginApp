//===========================================================================
// Summary:
//      DKDExportFunc.h
//      DKD的输出函数，可供外部调用
// Usage:
// Remarks:
//      Null
// Date:
//      2011-9-28
// Author:
//      Liu Hongjia (liuhj@duokan.com)
//===========================================================================
 
#ifndef __KERNEL_PDFKIT_PDFLIB_DKDEXPORTFUNC_H__
#define __KERNEL_PDFKIT_PDFLIB_DKDEXPORTFUNC_H__

#include "DKPRetCode.h"
#include "DKPTypeDef.h"

// 判断两个矩形是否相交
DK_ReturnCode IsRectCut(const DKP_RECT* pRc1, const DKP_RECT* pRc2);
DK_ReturnCode IsDRectCut(const DKP_DRECT* pRc1, const DKP_DRECT* pRc2);

// 判断两个矩形是否有相交面积，即相交面积是否大于0
DK_ReturnCode IsIntersectRect(const DKP_RECT* pRc1, const DKP_RECT* pRc2);
DK_ReturnCode IsIntersectDRect(const DKP_DRECT* pRc1, const DKP_DRECT* pRc2);

// 规则矩形
DK_ReturnCode NormalizeDRect(DKP_DRECT* pDRect);

DK_ReturnCode IsValidDRect(const DKP_DRECT* pDRect);

#endif // #ifndef __KERNEL_PDFKIT_PDFLIB_DKDEXPORTFUNC_H__
