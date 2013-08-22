//===========================================================================
// Summary:
//		复杂块的排版结果查询枚举器接口。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-09-10
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef	__COMPLEXBLOCK_LAYOUTENUMERATOR_INTERFACE_HEADERFILE__
#define __COMPLEXBLOCK_LAYOUTENUMERATOR_INTERFACE_HEADERFILE__

//===========================================================================

#include "ILayoutEnumerator.h"

//===========================================================================

// 字符信息，竖排时候变量x, y, boxBoundary, nGravity都是相对旋转后的区域中的值,
// 需要进行坐标变换得到在原始排版区域中的坐标。
typedef struct __TP_COMPLEX_CHARINFO
{
	TP_WCHAR ch;                        // 字符
	bool bVisible;                      // 字符是否可见
	bool bHyphen;                       // 字符是否是Hyphen
	double x;                           // 字符基线起点的x坐标
	double y;                           // 字符基线起点的y坐标
	unsigned int dwGlyphIndex;	        // 竖排字形变换后的字形索引，没有字形变换这个值取默认值0
	double dHScale;				        // 字形在字符方向的放缩比，默认值为1.0

    bool bRubyGuide;                    // 是否ruby音标
    bool bRubyBase;                     // 是否ruby正文

	TP_PIECE_TYPE nPieceType;
	TP_BOX boxBoundary;			        // 盒子位置，当nPieceType = TP_PIECE_GRAPH | TP_PIECE_IMAGE时有效
	long nPos;					        // 字符或者盒子在输入块中的索引，从0开始
	TP_GRAVITY_DIRECTION nGravity;		// 字符或者盒子的重力方向
	TP_GRAVITY_DIRECTION nRealGravity;	// 字符或者盒子真实的重力方向，即在原始排版区域的重力。

	__TP_COMPLEX_CHARINFO()
		: ch(0), bVisible(true), bHyphen(false), x(0.0), y(0.0), dwGlyphIndex(0), dHScale(1.0), bRubyGuide(false), bRubyBase(false), nPieceType(TP_PIECE_TEXT), nPos(0), nGravity(TP_GRAVITY_DOWN), nRealGravity(TP_GRAVITY_DOWN)
	{}

	__TP_COMPLEX_CHARINFO& operator=(const __TP_COMPLEX_CHARINFO& ci)
	{
		if (&ci != this)
		{
			ch = ci.ch;			
			bVisible = ci.bVisible;
			bHyphen = ci.bHyphen;
            bRubyGuide = ci.bRubyGuide;
            bRubyBase = ci.bRubyBase;
			nPieceType = ci.nPieceType;
			nPos = ci.nPos;
			nGravity = ci.nGravity;
			nRealGravity = ci.nRealGravity;
			boxBoundary = ci.boxBoundary;			
			if (ci.nPieceType == TP_PIECE_TEXT)
			{
				x = ci.x;
				y = ci.y;
				dwGlyphIndex = ci.dwGlyphIndex;
				dHScale = ci.dHScale;
			}
		}
		return *this;
	}
} TP_COMPLEX_CHARINFO;
typedef std::vector<TP_COMPLEX_CHARINFO> ComplexCharInfoVec;

//===========================================================================

class IComplexBlockLayoutEnumerator : public ILayoutEnumerator
{
public:
	//-------------------------------------------
	//	Summary:
	//		获取源字符串中已经排版的字符的个数。
	//	Return Value:
	//		字符个数。
	//	Remarks:
	//		注意这个返回值表示的是源字符串中已经排版的字符个数，不是枚举器中的字符个数。
	//		请用接口MoveToNextChar和GetCurrentChar遍历枚举器中的字符。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual long GetCharCount() = 0;

	//-------------------------------------------
	//	Summary:
	//		移动到下一个字符位置。
	//	Return Value:
	//		如果返回false表示已到最后一个字符。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool MoveToNextChar() = 0;

	//-------------------------------------------
	//	Summary:
	//		取得当前位置的字符信息。
	//  Parameters:
	//		[out] ci	- 当前位置字符信息。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void GetCurrentChar(TP_COMPLEX_CHARINFO& ci) = 0;

	//-------------------------------------------
	//	Summary:
	//		移动到下一行的起始位置。
	//	Return Value:
	//		如果返回false表示已到最后一行。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual bool MoveToNextLine() = 0;

	//-------------------------------------------
	//	Summary:
	//		取得当前行的信息。
	//  Parameters:
	//		[out] li	- 当前行信息。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void GetCurrentLine(TP_LINEINFO& li) = 0;

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

#endif	//#ifndef __COMPLEXBLOCK_LAYOUTENUMERATOR_INTERFACE_HEADERFILE__
