//===========================================================================
// Summary:
//		排版结果查询枚举器基础接口。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-08-19
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef	__LAYOUTENUMERATOR_INTERFACE_HEADERFILE__
#define __LAYOUTENUMERATOR_INTERFACE_HEADERFILE__

//===========================================================================

#include "TitanDef.h"

//===========================================================================

// 行信息
typedef struct __TP_LINEINFO
{
	int nStartPos;				// 行首字符在输入字符串中的位置，从0开始，不包括Hyphen。
	int nEnumPos;				// 行首字符在字符枚举器中的位置，从0开始，包括Hyphen。
	int nCharCount;				// 行字符个数，包括不可见字符，包括Hyphen。
	double dXStart;				// 行基线起始x坐标，取第一个可见字符的x坐标
	double dXEnd;				// 行基线终止x坐标，取最后一个可见字符的x坐标加上它的字宽
	double dYStart;				// 行基线起始y坐标。
	double dMaxAscent;			// 行基线以上高度最大值。
	double dMaxDescent;			// 行基线以下高度最大值。

	__TP_LINEINFO()
		: nStartPos(-1), nEnumPos(-1), nCharCount(0), dXStart(0.0), dXEnd(0.0), dYStart(0.0), dMaxAscent(0.0), dMaxDescent(0.0)
	{}

	__TP_LINEINFO& operator=(const __TP_LINEINFO& li)
	{
		if (&li != this)
		{
			nStartPos = li.nStartPos;
			nEnumPos = li.nEnumPos;
			nCharCount = li.nCharCount;
			dXStart = li.dXStart;
			dXEnd = li.dXEnd;
			dYStart = li.dYStart;
			dMaxAscent = li.dMaxAscent;
			dMaxDescent = li.dMaxDescent;
		}
		return *this;
	}
} TP_LINEINFO;
typedef std::vector<TP_LINEINFO> LineInfoVec;

// 块原子信息，竖排时候变量x, y, boxBoundary, nGravity都是相对旋转后的区域中的值,
// 需要进行坐标变换得到在原始排版区域中的坐标。
struct TP_PIECEATOMINFO
{
	long lAtomID;						// 原子ID，从0开始，和TP_CONTENTPIECE中的lStartAtomID对应，
										// 注意：Hyphen字符的这个值和它后面一个字符相同。
    bool bHyphen;                       // 字符是否是Hyphen
	int nPieceType;						// 原子所属块的类型，取值范围参考TP_PIECE_TYPE
	bool bVisible;						// 是否可见

	TP_WCHAR ch;						// 字符，当nPieceType = TP_PIECE_TEXT时有效
                                        // 该字符可能和传入不一致，例如竖排时的标点替换
	double x;							// 字符基线起点的x坐标，当nPieceType = TP_PIECE_TEXT时有效
	double y;							// 字符基线起点的y坐标，当nPieceType = TP_PIECE_TEXT时有效
	unsigned int dwGlyphIndex;			// 字形变换后的字形索引，没有字形变换这个值取默认值0
                                        // 当index不为0时，应优先使用index索取字形，例如竖排且取到竖排字型时
	double dHScale;						// 字形在字符方向的放缩比，默认值为1.0

    bool bRubyGuide;                    // 是否ruby音标
    bool bRubyBase;                     // 是否ruby正文

	TP_BOX boxBoundary;					// 盒子位置，也是字符外接矩形
	TP_GRAVITY_DIRECTION nGravity;		// 原子的重力方向
	TP_GRAVITY_DIRECTION nRealGravity;	// 原子真实的重力方向，即在原始排版区域的重力。

	TP_PIECEATOMINFO()
		: lAtomID(0), bHyphen(false), nPieceType(TP_PIECE_TEXT), bVisible(true), ch(0), x(0.0), y(0.0), dwGlyphIndex(0), dHScale(1.0), bRubyGuide(false), bRubyBase(false), nGravity(TP_GRAVITY_DOWN), nRealGravity(TP_GRAVITY_DOWN)
	{}

	TP_PIECEATOMINFO& operator=(const TP_PIECEATOMINFO& pai)
	{
		if (&pai != this)
		{
			lAtomID = pai.lAtomID;
            bHyphen = pai.bHyphen;
			nPieceType = pai.nPieceType;
			bVisible = pai.bVisible;
            bRubyGuide = pai.bRubyGuide;
            bRubyBase = pai.bRubyBase;
			nGravity = pai.nGravity;
			nRealGravity = pai.nRealGravity;
			boxBoundary = pai.boxBoundary;
			switch (pai.nPieceType)
			{
			case TP_PIECE_TEXT:
				ch = pai.ch;
				x = pai.x;
				y = pai.y;			
				dwGlyphIndex = pai.dwGlyphIndex;
				dHScale = pai.dHScale;
				break;
			default:
				break;
			}
		}
		return *this;
	}
};

typedef std::vector<TP_PIECEATOMINFO> PieceAtomInfoVec;

// 行信息
struct TP_PIECELINEINFO
{
	long lStartAtomID;				// 行首起始原子ID，从0开始
	int nAtomCount;					// 行内原子个数
	double dXStart;					// 行首起始x坐标
	double dXEnd;					// 行末结束x坐标
	double dYBaseLine;				// 行内基线y坐标
	double dYTop;					// 行内上边界y坐标
	double dYBottom;				// 行内下边界y坐标
	int nSpanLines;					// 绕排时盒子跨越的行数

	TP_PIECELINEINFO()
		: lStartAtomID(0), nAtomCount(0), dXStart(0.0), dXEnd(0.0), dYBaseLine(0.0), dYTop(0.0), dYBottom(0.0), nSpanLines(0)
	{}

	TP_PIECELINEINFO& operator=(const TP_PIECELINEINFO& pli)
	{
		if (&pli != this)
		{
			lStartAtomID = pli.lStartAtomID;
			nAtomCount = pli.nAtomCount;
			dXStart = pli.dXStart;
			dXEnd = pli.dXEnd;
            dYBaseLine = pli.dYBaseLine;
			dYTop = pli.dYTop;
			dYBottom = pli.dYBottom;
			nSpanLines = pli.nSpanLines;
		}
		return *this;
	}
};
typedef std::vector<TP_PIECELINEINFO> PieceLineInfoVec;

//===========================================================================

class ILayoutEnumerator
{
public:
	//-------------------------------------------
	//	Summary:
	//		开始枚举过程，枚举器内部指针重置。
	//  Parameters:
	//		Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void BeginEnum() = 0;

	//-------------------------------------------
	//	Summary:
	//		终止枚举过程，清空排版结果。
	//  Parameters:
	//		Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void EndEnum() = 0;

	//-------------------------------------------
	//	Summary:
	//		得到坐标变换矩阵，枚举器中的坐标需要经过坐标变换得到实际排版区域上的坐标。
	//  Parameters:
	//		Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual TP_MATRIX GetTransformMatrix() = 0;

protected:
	//-------------------------------------------
	//	Summary:
	//		虚析构函数。
	//-------------------------------------------
	virtual ~ILayoutEnumerator() {}
};

//===========================================================================

#endif	//#ifndef __LAYOUTENUMERATOR_INTERFACE_HEADERFILE__
