//===========================================================================
// Summary:
//		页面自定义图片块枚举器
// Usage:
//		枚举当前页面中所有自定义图片块
// Remarks:
//		被枚举的自定义图片块包括：STATIC, SINGLE, CROSSPAGE, MULTICALLOUT
// Date:
//		2013-08-12
// Author:
//		Zhang Jiafang (zhangjf@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEIMGBLOCKITERATOR_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEIMGBLOCKITERATOR_H__

#include "DKEDef.h"

class IDKEMultiCallout;

class IDKEImgBlockIterator
{
public:
    virtual ~IDKEImgBlockIterator() {}

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
	//		从ePubLib 2.6.1开始支持。
	//-------------------------------------------
    virtual DK_BOOL MoveToNext() = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前图片块的类型
	//  Parameters:
	//		Null
	//	Return Value:
	//		当前图片块的类型
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.6.1开始支持。
	//-------------------------------------------
    virtual DKE_IMAGEBLOCK_TYPE GetCurImgBlockType() const = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前位置图片块的相关信息
	//  Parameters:
	//		[out]pImgBlockInfo     - 图片块信息
	//	Return Value:
	//		Null
	//	Remarks:
	//		调用FreeCurImgBlockInfo释放资源
	//  Availability:
	//		从ePubLib 2.6.1开始支持。
	//-------------------------------------------
    virtual DK_VOID GetCurImgBlockInfo(DKE_IMAGEBLOCK_INFO* pImgBlockInfo) const = 0;

    //-------------------------------------------
	//	Summary:
	//		释放图片块的相关信息
	//  Parameters:
	//		[out]pImgBlockInfo     - 图片块信息
	//	Return Value:
	//		Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.6.1开始支持。
	//-------------------------------------------
    virtual DK_VOID FreeImgBlockInfo(DKE_IMAGEBLOCK_INFO* pImgBlockInfo) = 0;
};

#endif // __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEIMGBLOCKITERATOR_H__
