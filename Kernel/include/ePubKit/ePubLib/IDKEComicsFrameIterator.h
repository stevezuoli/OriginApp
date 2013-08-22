//===========================================================================
// Summary:
//		漫画页面帧枚举接口
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2012-12-17
// Author:
//		Zhang JiaFang
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKECOMICSFRAMEITERATOR_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKECOMICSFRAMEITERATOR_H__

//===========================================================================

#include "DKEDef.h"

//===========================================================================

class IDKEComicsFrameIterator
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
	//		从ePubLib 2.3.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL MoveToNext() = 0;

    //-------------------------------------------
	//	Summary:
	//		移动到上一位置。
	//  Parameters:
	//		Null
	//	Return Value:
	//		如果返回 DK_FALSE，则不应该从迭代器中继续获取数据。
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.3.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL MoveToPrev() = 0;

	//-------------------------------------------
	//	Summary:
	//		获取当前位置漫画帧的以图片左上角为参照的外接矩形
	//  Parameters:
	//		Null
	//	Return Value:
	//	    外接矩形
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.3.0开始支持。
	//-------------------------------------------
	virtual DK_BOX GetCurFrameBoundaryOnImage() const = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前位置漫画帧的以页面左上角为参照的外接矩形
	//  Parameters:
	//		Null
	//	Return Value:
	//	    外接矩形
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.3.0开始支持。
	//-------------------------------------------
	virtual DK_BOX GetCurFrameBoundaryOnPage() const = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前漫画帧在漫画帧阅读索引中的位置
	//  Parameters:
	//		Null
	//	Return Value:
	//	    帧阅读缩影的位置
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.3.0开始支持。
	//-------------------------------------------
    virtual DK_LONG GetComicsFrameIndex() const = 0;

public:
    virtual ~IDKEComicsFrameIterator() {}
};

//===========================================================================

#endif // #ifndef __IDKEFOOTNOTEITERATOR_HEADERFILE__
