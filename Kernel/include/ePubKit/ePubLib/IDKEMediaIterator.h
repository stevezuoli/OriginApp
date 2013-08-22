//===========================================================================
// Summary:
//		页面音视频对象迭代器接口。
// Usage:
//		枚举当前页面中所有音视频的信息，如音视频的外接矩形，音视频原始数据，和音视频URL等
// Remarks:
//      NULL
// Date:
//		2012-02-14
// Author:
//		Liu Hongjia(liuhj@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEMEDIAITERATOR_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEMEDIAITERATOR_H__

//===========================================================================

#include "DKEDef.h"

class IDKEMediaIterator
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
	//		获取当前位置的音视频信息。
	//  Parameters:
	//		[out] pMediaInfo            - 当前位置视频信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		调用 FreeMediaInfo 释放 pMediaInfo
	//  Availability:
	//		从ePubLib 2.0开始支持。
	//-------------------------------------------
	virtual DK_VOID GetCurrentMediaInfo(DKE_HITTEST_OBJECTINFO* pMediaInfo) const = 0;

	//-------------------------------------------
	//	Summary:
	//		释放音视频信息。
	//  Parameters:
	//		[in] pMediaInfo             - 待释放的信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.0开始支持。
	//-------------------------------------------
    virtual DK_VOID FreeMediaInfo(DKE_HITTEST_OBJECTINFO* pMediaInfo) = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前音视频的外接矩形
	//  Parameters:
	//		null
	//	Return Value:
	//	    DK_BOX 外接矩形
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.0开始支持。
	//-------------------------------------------
    virtual DK_BOX GetCurrentMediaBox() const = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前音视频的三级索引
	//  Parameters:
	//		null
	//	Return Value:
	//	    当前音视频的三级索引
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.4.0开始支持。
	//-------------------------------------------
    virtual DK_FLOWPOSITION GetCurrentFlowPosition() const = 0;

public:
    virtual ~IDKEMediaIterator() {}
};

//===========================================================================

#endif // #ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEMEDIAITERATOR_H__
