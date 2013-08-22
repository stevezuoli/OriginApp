//===========================================================================
// Summary:
//		基于复杂块的排版处理器接口。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-09-10
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef	__COMPLEXBLOCKPROCESSOR_INTERFACE_HEADERFILE__
#define __COMPLEXBLOCKPROCESSOR_INTERFACE_HEADERFILE__

//===========================================================================

#include "ITitanPart.h"

//===========================================================================

// 注意：一个段落有相同的行间距，段间距，首行缩进，对齐方式，行间距定义方式，现在是采用
// 一个段落的第一个块的这些值，因此在段首的块都要设置这5个变量的值。
typedef struct __TP_COMPLEXINFO
{
	TP_PIECE_TYPE nPieceType;

	TP_ALIGN_TYPE nAlignType;				// 对齐方式，取值范围参考TP_ALIGN_TYPE	
	TP_LINEGAP_TYPE nLineGapType;			// 行间距定义方式，当是多倍行距时，dLineGap存放的是倍数；当是固定值时，dLineGap存放固定的行高值。
	double dLineGap;						// 行间距
	double dParaSpacing;					// 段间距
    bool bIgnoreBeginSpace;                 // 是否忽略段开始空白字符
	double dIndent;							// 首行缩进距离，单位是字号大小

    bool bEmphasis;                         // 是否设置着重

	// 以下部分为TP_PIECE_TEXT时有效
	TP_WCHAR* pszSrcText;					// 文字内容
	TP_FONT font;							// 字体
	double dTabStop;						// tab宽度，单位是字号大小

	// 以下部分为TP_PIECE_IMAGE时有效
    TP_FOLLOWIMAGE_TYPE nFollowImageType;   // 随文图类型
	TP_BOX boxBoundary;						// 盒子大小
	double dLeftMargin;						// 盒子左方外边距
	double dRightMargin;					// 盒子右方外边距
	double dTopMargin;						// 盒子上方外边距
	double dBottomMargin;					// 盒子下方外边距
    bool bTopMarginSet;                     // 上外边距是否被设置
    bool bBottomMarginSet;                  // 下外边距是否被设置
	TP_BOX_VALIGN_TYPE nVAlignType;			// 盒子垂直方向的对齐方式
    double dVAlignLength;                   // 盒子底线距离当前行基线的垂直距离（基线上方为正），可为负值，当nVAlignType = TP_BOX_VALIGN_BASELINE时有效
    TP_LENGTH_UNIT nVAlignUnit;             // 盒子底线距离当前行基线的垂直距离的单位, 当nVAlignUnit = TP_LENGTH_PERCENT时，对照值为当前盒子高
	bool bRotate;							// 竖排时候盒子是否旋转90度

	// 以下部分为TP_PIECE_RUBY时有效
	TP_BASICATOM* pGuideText;               // 音标文字
	int	nGuideTextCount;
	TP_BASICATOM* pBaseText;                // 音标对应的正文
	int	nBaseTextCount;
	TP_RUBY_ALIGN_TYPE nRubyAlignType;      // 拼注音对齐方式
	TP_RUBY_POSITION nRubyPosition;         // 拼注音位置

	// 以下部分为TP_PIECE_HORINVER或者TP_PIECE_COMBINELINES时有效
	TP_BASICATOM* pAtomList;
	int	nAtomCount;
	// 以下部分为TP_PIECE_HORINVER时有效
	bool bFitLine;                          // 如果为 true，表示旋转后的文字要压缩到适合行高。
	// 以下部分为TP_PIECE_COMBINELINES时有效
	int nCombineLines;                      // 行数
	int nSkipNum;                           // 需要跳过的字符个数
	// 以下部分为TP_PIECE_RUBY、TP_PIECE_HORINVER或者TP_PIECE_COMBINELINES时有效
	int nInnerNum;                          // 内部字符个数

	__TP_COMPLEXINFO()
		: nPieceType(TP_PIECE_TEXT), nAlignType(TP_ALIGN_LEFT), nLineGapType(TP_LINEGAP_MULTIPLE), dLineGap(1.0), dParaSpacing(0.0), bIgnoreBeginSpace(true), dIndent(0.0), bEmphasis(false), pszSrcText(NULL), dTabStop(0.0),
		nFollowImageType(TP_FOLLOWIMAGE_IMAGE), dLeftMargin(0.0), dRightMargin(0.0), dTopMargin(0.0), dBottomMargin(0.0), bTopMarginSet(false), bBottomMarginSet(false), nVAlignType(TP_BOX_VALIGN_BASELINE), bRotate(true), pGuideText(NULL), 
		nGuideTextCount(0), pBaseText(NULL), nBaseTextCount(0), nRubyAlignType(TP_RUBY_ALIGN_CENTER), nRubyPosition(TP_RUBY_POSITION_BEFORE), pAtomList(NULL), nAtomCount(0), bFitLine(true), nCombineLines(2), nSkipNum(0), nInnerNum(0)
	{}
} TP_COMPLEXINFO;

// 复杂块输入信息
typedef struct __TP_CBLOCK_INPUTINFO
{
	TP_COMPLEXINFO* pInputInfo;
	int	nInfoCount;

	__TP_CBLOCK_INPUTINFO()
		: pInputInfo(NULL), nInfoCount(0)
	{}
} TP_CBLOCK_INPUTINFO;

//===========================================================================

class IComplexBlockProcessor : public ITitanPart
{
public:
	//-------------------------------------------
	//	Summary:
	//		排入文字。
	//  Parameters:
	//		[in] info			- 待排版的文字串。
	//		[in] bFastMode		- 高速模式，该模式下不能枚举排版结果。
	//		[in] bBleedMode		- 出血模式，该模式下最后一行可以超出排版区域。
	//	Return Value:
	//		文字全部排入成功返回TP_LAYOUT_SUCCEED；
	//		部分排入还有后续返回TP_LAYOUT_FOLLOWAFTER；
	//		排入失败则返回TP_LAYOUT_FAIL。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual TP_LAYOUT_STATE ProcessText(const TP_CBLOCK_INPUTINFO& info, bool bFastMode = false, bool bBleedMode = false) = 0;

	//-------------------------------------------
	//	Summary:
	//		排入文字。
	//  Parameters:
	//		[in] info			- 待排版的文字串。
	//		[in] isp			- 初始状态参数，可以指定当前块起始位置是否是新段落。
	//		[in] bFastMode		- 高速模式，该模式下不能枚举排版结果。
	//		[in] bBleedMode		- 出血模式，该模式下最后一行可以超出排版区域。
	//	Return Value:
	//		文字全部排入成功返回TP_LAYOUT_SUCCEED；
	//		部分排入还有后续返回TP_LAYOUT_FOLLOWAFTER；
	//		排入失败则返回TP_LAYOUT_FAIL。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual TP_LAYOUT_STATE ProcessTextEx(const TP_CBLOCK_INPUTINFO& info, const TP_INITIALSTATEPARA& isp, bool bFastMode = false, bool bBleedMode = false) = 0;
};

//===========================================================================

#endif	//#ifndef __COMPLEXBLOCKPROCESSOR_INTERFACE_HEADERFILE__
