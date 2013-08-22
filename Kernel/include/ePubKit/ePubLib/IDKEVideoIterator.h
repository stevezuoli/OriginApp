//===========================================================================
// Summary:
//		页面视频对象迭代器接口。
// Usage:
//		枚举当前页面中所有视频的信息，如视频的外接矩形，视频原始数据，和视频URL等
// Remarks:
//      NULL
// Date:
//		2012-02-14
// Author:
//		Liu Hongjia(liuhj@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEVIDEOITERATOR_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEVIDEOITERATOR_H__

//===========================================================================

#include "DKEDef.h"

class IDKEVideoIterator
{
public:
	//-------------------------------------------
	//	Summary:
	//		移动到下一位置。
	//  Parameters:
	//		Null
	//	Return Value:
	//		如果返回 DK_FALSE，则不应该从迭代器中继续获取数据。
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL MoveToNext() = 0;

	//-------------------------------------------
	//	Summary:
	//		获取当前位置的视频信息。
	//  Parameters:
	//		[out] pVideoInfo            - 当前位置视频信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		调用 FreeVideoInfo 释放 pVideoInfo
	//  Availability:
	//		从ePubLib 2.0开始支持。
	//-------------------------------------------
	virtual DK_VOID GetCurrentVideoInfo(DKE_HITTEST_OBJECTINFO* pVideoInfo) const = 0;

	//-------------------------------------------
	//	Summary:
	//		释放视频信息。
	//  Parameters:
	//		[in] pVideoInfo             - 待释放的信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.0开始支持。
	//-------------------------------------------
    virtual DK_VOID FreeVideoInfo(DKE_HITTEST_OBJECTINFO* pVideoInfo) = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前视频的外接矩形
	//  Parameters:
	//		null
	//	Return Value:
	//	    DK_BOX 外接矩形
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.0开始支持。
	//-------------------------------------------
    virtual DK_BOX GetCurrentVideoBox() const = 0;

public:
    virtual ~IDKEVideoIterator() {}
};

//===========================================================================

#endif // #ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEVIDEOITERATOR_H__
