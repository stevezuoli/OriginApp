//===========================================================================
// Summary:
//		ePub图书目录接口。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-09-16
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKETOCPOINT_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKETOCPOINT_H__

//===========================================================================

#include "DKEDef.h"

//===========================================================================

class IDKETocPoint
{
public:
	//-------------------------------------------
	//	Summary:
	//		获取目录节点文字内容。
	//  Parameters:
	//		Null
	//	Return Value:
	//		目录节点文字内容。
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetTitle() = 0;

	//-------------------------------------------
	//	Summary:
	//		获取跳转目标。
	//  Parameters:
	//		Null
	//	Return Value:
	//		目标对象的URL链接。
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetDest() const = 0;

	//-------------------------------------------
	//	Summary:
	//		获取跳转目标坐标位置。
	//  Parameters:
	//		[out] destPos           - 目标对象的流式坐标。
	//	Return Value:
	//		如果成功返回DKR_OK。
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 1.0开始支持。
	//-------------------------------------------
    virtual DK_ReturnCode GetDestPosition(DK_FLOWPOSITION& destPos) = 0;

    //-------------------------------------------
	//	Summary:
	//		获取跳转目标位置的锚点名称。
	//  Parameters:
	//		NULL
	//	Return Value:
	//		锚点名称
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.2开始支持。
	//-------------------------------------------
    virtual const DK_WCHAR* GetDestAnchor() const = 0;

	//-------------------------------------------
	//	Summary:
	//		获取当前节点的深度。
	//  Parameters:
	//		Null
	//	Return Value:
	//		节点深度。
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 1.0开始支持。
	//-------------------------------------------
    virtual DK_INT GetDepth() const = 0;

	//-------------------------------------------
	//	Summary:
	//		当前节点目标内容是否有效。
	//  Parameters:
	//		Null
	//	Return Value:
	//	    DK_TRUE/DK_FALSE
	//	Remarks:
	//		这是针对 ePub 规范的一个扩展。
	//  Availability:
	//		从ePubLib 1.0开始支持。
	//-------------------------------------------
    virtual DK_BOOL IsContentValid() const = 0;

public:
	//-------------------------------------------
	//	Summary:
	//		获取子节点个数。
	//  Parameters:
	//		Null
	//	Return Value:
	//		子节点个数。
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 1.0开始支持。
	//-------------------------------------------
    virtual DK_LONG GetChildCount() const = 0;

	//-------------------------------------------
	//	Summary:
	//		获取子节点。
	//  Parameters:
	//		[in] index              - 子节点索引，从0开始。
	//	Return Value:
	//		子节点对象接口。
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 1.0开始支持。
	//-------------------------------------------
	virtual IDKETocPoint* GetChildByIndex(DK_UINT index) const = 0;

public:
    virtual ~IDKETocPoint() {}
};

//===========================================================================

#endif // #ifndef __IDKETOCPOINT_HEADERFILE__
