//===========================================================================
// Summary:
//		页面文字对象迭代器接口。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-12-19
// Author:
//		Peng Feng (pengf@duokan.com)
//===========================================================================

#ifndef __KERNEL_TXTKIT_TXTLIB_EXPORT_IDKTTEXTITERATOR_H__
#define __KERNEL_TXTKIT_TXTLIB_EXPORT_IDKTTEXTITERATOR_H__

//===========================================================================

#include "DKTDef.h"
#include "KernelRetCode.h"

//===========================================================================


class IDKTTextIterator
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
	//		除了段末的回车和页末之外,光标只应停留在可见字符前
	//  Availability:
	//		从TxtLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL MoveToNext() = 0;

	//-------------------------------------------
	//	Summary:
	//		移动到前一位置。
	//  Parameters:
	//		Null
	//	Return Value:
	//		如果返回 DK_FALSE，则不应该从迭代器中继续获取数据。
	//	Remarks:
	//		Null
	//  Availability:
	//		从TxtLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL MoveToPrev() = 0;

    //-------------------------------------------
	//	Summary:
	//		移动到上一行的相邻位置。
	//  Parameters:
	//		Null
	//	Return Value:
	//		如果返回 DK_FALSE，则不应该从迭代器中继续获取数据。
	//	Remarks:
	//		Null
	//  Availability:
	//		从TxtLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL MoveToPrevAdjacentLine() = 0;

    //-------------------------------------------
	//	Summary:
	//		移动到下一行的相邻位置。
	//  Parameters:
	//		Null
	//	Return Value:
	//		如果返回 DK_FALSE，则不应该从迭代器中继续获取数据。
	//	Remarks:
	//		Null
	//  Availability:
	//		从TxtLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL MoveToNextAdjacentLine() = 0;

	//-------------------------------------------
	//	Summary:
	//		获取当前位置的字符信息。
	//  Parameters:
	//		[out] pCharInfo          - 当前位置字符信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从TxtLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_ReturnCode GetCurrentCharInfo(DKT_CHAR_INFO* pCharInfo) const = 0;

public:
    virtual ~IDKTTextIterator() {}
};

//===========================================================================

#endif // #ifndef __IDKETEXTITERATOR_HEADERFILE__
