//===========================================================================
// Summary:
//		IDKPImageEnumerator是枚举图像的接口类。 
// Usage:
//		由IDKPPage中获得。
// Remarks:
//		Null
// Date:
//		2011-10-31
// Author:
//		Zhang JingDan (zhangjingdan@duokan.com)
//===========================================================================

#ifndef __KERNEL_PDFKIT_PDFLIB_IDKPIMAGEENUMERATOR_H__
#define __KERNEL_PDFKIT_PDFLIB_IDKPIMAGEENUMERATOR_H__

//===========================================================================

#include "DKPTypeDef.h"

//===========================================================================

class IDKPImageEnumerator
{
public:
	//----------------------------------------------------
	// Summary:
	//		回到开始位置，每一次枚举开始都要使用此函数如
	//		果返回DK_FALSE，不应该再从枚举器取任何数据。
	// SDK Version:
	//		从DKP 5.0.0开始支持
	//----------------------------------------------------
	virtual DK_BOOL Reset() = 0;

	//----------------------------------------------------
	// Summary:
	// 		前进到下一个位置，如果返回DK_FALSE，不
	//		应该再从枚举器取任何数据。
	// SDK Version:
	//		从DKP 5.0.0开始支持
	//----------------------------------------------------
	virtual DK_BOOL MoveNext() = 0;

	//-------------------------------------------
	// Summary:
	//		取得当前的图像信息。
	// Parameters:
	//		[out] imageInfo - 输出的图像信息。
	// SDK Version:
	//		从DKP 5.0.0开始支持
	//-------------------------------------------
	virtual DK_BOOL Current(DKPIMAGECONTENT& imageInfo) = 0;

public:
	virtual ~IDKPImageEnumerator() {}
};

//===========================================================================

#endif
