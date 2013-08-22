//===========================================================================
// Summary:
//		基于HTML流结构的块排版结果查询枚举器接口，主要用于EPUB渲染。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-09-13
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef	__HTMLBLOCK_LAYOUTENUMERATOR_INTERFACE_HEADERFILE__
#define __HTMLBLOCK_LAYOUTENUMERATOR_INTERFACE_HEADERFILE__

//===========================================================================

#include "ILayoutEnumerator.h"

//===========================================================================

class IHTMLBlockLayoutEnumerator : public ILayoutEnumerator
{
public:
	//-------------------------------------------
	//	Summary:
	//		获取源字符串中已经排版的原子的个数。
	//	Return Value:
	//		字符个数。
	//	Remark:
	//		注意这个返回值表示的是源流式块中已经排版的原子个数，不是枚举器中的原子个数。
	//		请用接口MoveToNextAtom和GetCurrentAtom遍历枚举器中的原子。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual long GetAtomCount() = 0;

	//-------------------------------------------
	//	Summary:
	//		移动到下一个原子位置。
	//	Return:
	//		如果返回false表示已到结束位置。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool MoveToNextAtom() = 0;

	//-------------------------------------------
	//	Summary:
	//		取得当前位置的原子信息。
	//  Parameters:
	//		[out] pai	- 当前位置原子信息。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void GetCurrentAtom(TP_PIECEATOMINFO& pai) = 0;

	//-------------------------------------------
	//	Summary:
	//		获取排版结果中行数。
	//	Return Value:
	//		行数。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual long GetLineCount() = 0;

	//-------------------------------------------
	//	Summary:
	//		移动到下一行的起始位置。
	//	Return Value:
	//		如果返回false表示已到最后一行。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool MoveToNextPieceLine() = 0;

	//-------------------------------------------
	//	Summary:
	//		取得当前行的信息。
	//  Parameters:
	//		[out] pli	- 当前行信息。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void GetCurrentPieceLine(TP_PIECELINEINFO& pli) = 0;

	//-------------------------------------------
	//	Summary:
	//		取得排版高度，即排版结果中最后一行的下边界。
	//	Return Value:
	//		返回排版高度。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual double GetLayoutHeight() = 0;

    //-------------------------------------------
    //	Summary:
    //		取得排版高度，即排版结果宽度。
    //	Return Value:
    //		返回排版宽度。
    //  Availability:
    //		从Titan 1.0开始支持。
    //-------------------------------------------
    virtual double GetLayoutWidth() = 0;

    //-------------------------------------------
    //  Summary:
    //      取得枚举器中全部字符的外框矩形。
    //  Parameters:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从Titan 2.0开始支持。
    //-------------------------------------------
    virtual TP_BOX GetBoxBoundary() const = 0;
};

//===========================================================================

#endif	//#ifndef __HTMLBLOCK_LAYOUTENUMERATOR_INTERFACE_HEADERFILE__
