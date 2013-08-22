//===========================================================================
// Summary:
//		IDKOutputPen.h
//      Pen的抽象
// Usage:
//	    Null
// Remarks:
//		Null
// Date:
//		2011-09-11
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_RENDER_IDKOUTPUTPEN_H__
#define	__KERNEL_COMMONLIBS_RENDER_IDKOUTPUTPEN_H__

#include "KernelType.h"


class IDKOutputBrush;


class IDKOutputPen
{
public:
	virtual ~IDKOutputPen() {}

public:
	// 设置线宽和颜色
	virtual DK_VOID SetPenWidth(DK_DOUBLE dWidth) = 0;
	virtual DK_VOID SetPenColor(const DK_ARGBCOLOR& clr) = 0;

	// 设置线头形状
	virtual DK_VOID SetLineCap(DK_LINECAP_TYPE startCap) = 0;
	virtual DK_VOID SetLineCap(DK_LINECAP_TYPE startCap, DK_LINECAP_TYPE endCap, DK_DASHCAP_TYPE dashCap) = 0;

	// 设置MiterLimit
	virtual DK_VOID SetLineMiterLimit(DK_DOUBLE dLimit) = 0;

	// 设置线交点形状
	virtual DK_VOID SetLineJoin(DK_LINEJOIN_TYPE join) = 0;

	// 设置自定义Dash
	// 注意：pdDashArry中的值就是Dash, Sapce的显示长度（已包含与线宽的乘积），nCount是pdDashArry数组的大小
	// SetDashPatternAndOffset调用前应保证已调用SetPenWidth设置过线宽。
	// SetDashPatternAndOffset 前或后建议调用SetDashStyle(DK_DASHSTYLE_CUSTOM)，否则可能不起效果。
	virtual DK_VOID SetDashPatternAndOffset(DK_DOUBLE *pdDashArry, DK_INT nCount, DK_DOUBLE dOffset) = 0;
	virtual DK_VOID SetDashStyle(DK_DASHSTYLE_TYPE dashstyle) = 0;

	// 用刷子填充线
	virtual DK_VOID SetLineBrush(IDKOutputBrush *pBrush) = 0;

	// 设置Pen的变换矩阵
	virtual DK_VOID SetTransform(const DK_MATRIX& matrix) = 0;
};
#endif
