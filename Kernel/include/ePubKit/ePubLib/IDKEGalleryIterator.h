//===========================================================================
// Summary:
//		页面画廊对象迭代器接口。
// Usage:
//		枚举当前页面中所有画廊枚举器
// Remarks:
//		Null
// Date:
//		2013-03-25
// Author:
//		Zhang Jiafang (zhangjf@duokan.com)
//===========================================================================


#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEGALLERYITERATOR_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEGALLERYITERATOR_H__

//===========================================================================

#include "DKEDef.h"

//===========================================================================

class IDKEGallery;

class IDKEGalleryIterator
{
public:
    virtual ~IDKEGalleryIterator() {}

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
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
    virtual DK_BOOL MoveToNext() = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前画廊的信息
	//  Parameters:
	//		[out] ppGallery         - 当前画廊信息
	//	Return Value:
	//		Null
	//	Remarks:
	//		调用FreeCurGallery释放资源
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
    virtual DK_VOID GetCurGallery(IDKEGallery** ppGallery) const = 0;

    //-------------------------------------------
	//	Summary:
	//		释放当前画廊的画廊信息
	//  Parameters:
	//		[in] pGallery           - 当前画廊信息
	//	Return Value:
	//		Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
    virtual DK_VOID FreeCurGallery(IDKEGallery* pGallery) = 0;
};


#endif // __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEGALLERYITERATOR_H__

