//===========================================================================
// Summary:
//		IDKOutputRegion类
// Usage:
//		设备抽象区域
//			...
// Remarks:
//		Null
// Date:
//		2011-09-16
// Author:
//		pengf (pengf@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_RENDER_IDKOUTPUTREGION_H__
#define	__KERNEL_COMMONLIBS_RENDER_IDKOUTPUTREGION_H__

//===========================================================================

#include "KernelType.h"

class IDKOutputPath;
//===========================================================================

// Interface IDKOutputRegion
class IDKOutputRegion
{
public:
	virtual ~IDKOutputRegion() {}

public:
	// 重置为无穷大的区域
	virtual DK_VOID Reset() = 0;

	// 求与矩形的相交区域
	virtual DK_VOID Intersect(const DK_BOX& box) = 0;

	// 求与路径的相交区域
	virtual DK_VOID Intersect(const IDKOutputPath* path) = 0;

	// 求与区域的相交区域
	virtual DK_VOID Intersect(const IDKOutputRegion* region) = 0;

	// 区域是否和矩形相交
	virtual DK_BOOL IsVisible(const DK_BOX& box) const = 0;

	// 区域是否和点相交
	virtual DK_BOOL IsVisible(const DK_POS& pt) const = 0;

	// 矩形是否完全包含在区域内
	virtual DK_BOOL ContainsRect(const DK_BOX& box) const = 0;

	// 求与矩形的并集
	virtual DK_VOID Union(const DK_BOX& box) = 0;

	// 求与路径的并集
	virtual DK_VOID Union(const IDKOutputPath* path) = 0;

	// 求与区域的并集
	virtual DK_VOID Union(const IDKOutputRegion* region) = 0;

	// 获取区域的外接矩形。
	// 如果区域表示无穷大，返回DK_BOX(0, 0, -1, -1)
	// 如果区域为空，返回DK_BOX(0, 0, 0, 0)
	virtual DK_BOX GetBounds() const = 0;
};

#endif //#ifndef __IDKOUTPUTREGION_HEADERFILE__
