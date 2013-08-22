//===========================================================================
// Summary:
//		各类重排处理器的基础部件接口。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-08-17
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef	TITANPART_INTERFACE_HEADERFILE
#define TITANPART_INTERFACE_HEADERFILE

//===========================================================================

#include "KernelRenderType.h"
#include "TitanDef.h"
#include "ILayoutEnumerator.h"

//===========================================================================

// 字体信息
struct TP_FONT
{
	TP_FONTDEFINE_TYPE nDefineType;				// 字体定义方式
	union
	{
		const TP_WCHAR* pszFaceName;			// 字体名字
		const TP_WCHAR* pszFilePath;			// 字体文件全路径名
		TP_HANDLE handle;						// 用户自定义字体句柄
	} fontDef;									// 字体定义参数

	double dFontSize;							// 字号
	TP_FONTSTYLE_TYPE nStyle;					// 字体风格
	TP_FONTWEIGHT_TYPE nWeight;					// 字体浓淡
	bool bSubScript;							// 是否下标
	bool bSuperScript;							// 是否上标，如果上下标同时为true则作为上标
	double dSmallFontSize;						// 上下标的字号

	TP_FONT& operator=(const TP_FONT& rft)
	{
		nDefineType = rft.nDefineType;
		dFontSize = rft.dFontSize;
		nStyle = rft.nStyle;
		nWeight = rft.nWeight;
		bSubScript = rft.bSubScript;
		bSuperScript = rft.bSuperScript;
		dSmallFontSize = rft.dSmallFontSize;
		
		if (nDefineType == TP_FONTDEFINE_FACENAME)
			fontDef.pszFaceName = rft.fontDef.pszFaceName;
		else if (nDefineType == TP_FONTDEFINE_FILENAME)
			fontDef.pszFilePath = rft.fontDef.pszFilePath;
		else
			fontDef.handle = rft.fontDef.handle;
		
		return *this;
	}

	TP_FONT()
		:nDefineType(TP_FONTDEFINE_FACENAME),
		 dFontSize(0.0),
		 nStyle(TP_FONTSTYLE_NORMAL),
		 nWeight(TP_FONTWEIGHT_NORMAL),
		 bSubScript(false),
		 bSuperScript(false),
		 dSmallFontSize(0.0)
	{
		fontDef.pszFaceName = NULL;
		fontDef.pszFilePath = NULL;
	}
};

struct TP_TEXTPIECE
{
    TP_TEXTPIECE_TYPE textType;                 // 文字片段的类型 
	const TP_WCHAR* pszText;					// 文字内容
	TP_FONT font;								// 字体
	double dTabStop;							// tab宽度，单位为字号大小。

	TP_TEXTPIECE()
        :textType(TP_TEXTPIECE_NORMAL),
         pszText(NULL),
		 dTabStop(0.0)
	{}
};

// 高级排版特性里面的基本原子
struct TP_BASICATOM
{
    long lStartAtomID;
	TP_PIECE_TYPE nPieceType;

	// 以下部分为TP_PIECE_TEXT时有效
	const TP_WCHAR* pszText;					// 文字内容
	TP_FONT font;								// 字体

    bool bLeftMarginSet;                        // 左外边距是否被设置
    bool bRightMarginSet;                       // 右外边距是否被设置
    bool bTopMarginSet;                         // 上外边距是否被设置
    bool bBottomMarginSet;                      // 下外边距是否被设置
    double dLeftMargin;							// 左方外边距, bLeftMarginSet为true是有效
	double dRightMargin;						// 右方外边距, bRightMarginSet为true是有效
	double dTopMargin;							// 上方外边距, bTopMarginSet为true是有效
	double dBottomMargin;						// 下方外边距, bBottomMarginSet为true是有效
    
	// 以下部分为TP_PIECE_IMAGE时有效
	TP_BOX boxBoundary;							// 盒子大小
    TP_FOLLOWIMAGE_TYPE nFollowImageType;       // 随文图的类型
	TP_BOX_VALIGN_TYPE nVAlignType;				// 盒子垂直方向的对齐方式
    double dVAlignLength;                       // 盒子底线距离当前行基线的垂直距离（基线上方为正），可为负值，当nVAlignType = TP_BOX_VALIGN_BASELINE时有效
    TP_LENGTH_UNIT nVAlignUnit;                 // 盒子底线距离当前行基线的垂直距离的单位, 当nVAlignUnit = TP_LENGTH_PERCENT时，对照值为当前盒子高，为percent时需要除以100
    
    unsigned int nBoxAscent;					// NOTE：可删除
	
    bool bRotate;								// 竖排时候盒子是否旋转90度

    // NOTE: 删除赋值重载
	TP_BASICATOM& operator=(const TP_BASICATOM& rtf)
	{		
		nPieceType = rtf.nPieceType;
		pszText = rtf.pszText;
		font = rtf.font;
		boxBoundary = rtf.boxBoundary;
		dLeftMargin = rtf.dLeftMargin;
		dRightMargin = rtf.dRightMargin;
		dTopMargin = rtf.dTopMargin;
		dBottomMargin = rtf.dBottomMargin;
        bLeftMarginSet = rtf.bLeftMarginSet;
        bRightMarginSet = rtf.bRightMarginSet;
        bTopMarginSet = rtf.bTopMarginSet;
        bBottomMarginSet = rtf.bBottomMarginSet;
        nFollowImageType = rtf.nFollowImageType;
		nVAlignType = rtf.nVAlignType;
        dVAlignLength = rtf.dVAlignLength;
        nVAlignUnit = rtf.nVAlignUnit;
		bRotate = rtf.bRotate;

        nBoxAscent = rtf.nBoxAscent;

		return *this;
	}

	TP_BASICATOM()
		:lStartAtomID(-1),
         nPieceType(TP_PIECE_TEXT),
		 pszText(NULL),
         bLeftMarginSet(false),
         bRightMarginSet(false),
         bTopMarginSet(false),
         bBottomMarginSet(false),
		 dLeftMargin(0.0),
		 dRightMargin(0.0),
		 dTopMargin(0.0),
		 dBottomMargin(0.0),
         nFollowImageType(TP_FOLLOWIMAGE_IMAGE),
		 nVAlignType(TP_BOX_VALIGN_BASELINE),
         dVAlignLength(0),
         nVAlignUnit(TP_LENGTH_PX),
		 nBoxAscent(1000),
		 bRotate(false)
	{}
};

// RUBY拼注音块，拼注音块不能拆行
struct TP_RUBYPIECE
{
	TP_BASICATOM* pGuideText;						// 音标文字
	int	nGuideTextCount;
	TP_BASICATOM* pBaseText;						// 音标对应的正文
	int	nBaseTextCount;
	TP_RUBY_ALIGN_TYPE nRubyAlignType;				// 拼注音对齐方式
	TP_RUBY_POSITION nRubyPosition;					// 拼注音位置

	TP_RUBYPIECE()
		:pGuideText(NULL),
		 nGuideTextCount(0),
		 pBaseText(NULL),
		 nBaseTextCount(0),
		 nRubyAlignType(TP_RUBY_ALIGN_CENTER),
		 nRubyPosition(TP_RUBY_POSITION_BEFORE)
	{}
};

// 纵中横块
struct TP_HORINVERPIECE
{
	TP_BASICATOM* pAtomList;
	int	nAtomCount;
	bool bFitLine;			// 如果为 true，表示旋转后的文字要压缩到适合行高。

	TP_HORINVERPIECE()
		:pAtomList(NULL), nAtomCount(0), bFitLine(true)
	{}
};

// 分行缩排块
struct TP_COMBINELINESPIECE
{
	TP_BASICATOM* pAtomList;
	int	nAtomCount;
	int nLineCount;							// 分行缩排中文字行数，只可取2或3
	int nSkipNum;							// 需要跳过的字符个数

	TP_COMBINELINESPIECE()
		:pAtomList(NULL), nAtomCount(0), nLineCount(2), nSkipNum(0)
	{}
};

// 段落样式
struct TP_PARASTYLE
{
	TP_ALIGN_TYPE nAlignType;				// 对齐方式，取值范围参考 TP_ALIGN_TYPE
    int headLevel;                          // 标题层级，对应 h1 - h6，如果为 -1 则表示不是标题
    bool bIgnoreBeginSpace;                 // 是否忽略段开始空白字符
 	double dIndent;							// 首行缩进距离，单位为字号大小
    double dDuokanIndent;                   // 多看自定义的首行缩进
	TP_LINEGAP_TYPE nLineGapType;			// 行间距定义方式，当是多倍行距时，dLineGap 存放的是倍数；当是固定值时，dLineGap 存放固定的行高值
	double dLineGap;						// 行间距
	double dParaSpacing;					// 段间距

	TP_PARASTYLE()
		:nAlignType(TP_ALIGN_LEFT),
         headLevel(-1),
         bIgnoreBeginSpace(true), 
		 dIndent(0.0),
         dDuokanIndent(0.0),
		 nLineGapType(TP_LINEGAP_MULTIPLE),
		 dLineGap(1.0),
		 dParaSpacing(0.0)
    {}
};


// 注意：一个段落有相同的行间距，段间距，首行缩进，对齐方式，现在是采用
// 一个段落的第一个块的这些值，因此在段首的块都要设置这4个变量的值。
struct TP_CONTENTPIECE
{
	long lStartAtomID;						// 原子起始ID，从0开始
	TP_PIECE_TYPE nPieceType;				// Piece类型，取值范围参考TP_PIECE_TYPE
	TP_ALIGN_TYPE nAlignType;				// 对齐方式，取值范围参考TP_ALIGN_TYPE
	TP_TEXTPIECE tpElement;					// 文字内容，当nPieceType = TP_PIECE_TEXT时有效	
	TP_PARASTYLE paraStyle;					// 段落样式

    bool bLeftMarginSet;                    // 左外边距是否被设置
    bool bRightMarginSet;                   // 右外边距是否被设置
    bool bTopMarginSet;                     // 上外边距是否被设置
    bool bBottomMarginSet;                  // 下外边距是否被设置
	double dLeftMargin;						// 左方外边距, bLeftMarginSet为TRUE时有效
	double dRightMargin;					// 右方外边距, bRightMarginSet为TRUE时有效
	double dTopMargin;						// 上方外边距, bTopMarginSet为TRUE时有效
	double dBottomMargin;					// 下方外边距，bBottomMarginSet为TRUE时有效

	TP_BOX boxBoundary;						// 盒子大小，当nPieceType = TP_PIECE_GRAPH | TP_PIECE_IMAGE时有效
	TP_WRAPPING_TYPE nWrappingType;			// 文字绕排方式，取值范围参考TP_WRAPPING_TYPE
    TP_FOLLOWIMAGE_TYPE nFollowImageType;   // 随文图类型，当nPieceType = TP_PIECE_IMAGE并且nWrappingType = TP_WRAPPING_FOLLOW时有效
	TP_BOX_VALIGN_TYPE nVAlignType;			// 盒子垂直方向的对齐方式
    double dVAlignLength;                   // 盒子底线距离当前行基线的垂直距离（基线上方为正），可为负值，当nVAlignType = TP_BOX_VALIGN_BASELINE时有效
    TP_LENGTH_UNIT nVAlignUnit;             // 盒子底线距离当前行基线的垂直距离的单位, 当nVAlignUnit = TP_LENGTH_PERCENT时，对照值为当前盒子高，为percent时需要除以100
	bool bRotate;							// 竖排时候盒子是否旋转90度，只对随文图有效，绕排和独占图固定不旋转。

	void* pAdvElement;						// NOTE:可删除 高级排版属性，比如分行缩排、纵中横、拼注音等,类型视type而定。

    bool bNoBreakBefore;                    // 前置不可拆
    bool bNoBreakInside;                    // piece不可拆
    bool bNoBreakAfter;                     // 后置不可拆
    bool bEmphasis;                         // 只支持下着重

    TP_RUBYPIECE* pRubyPiece;               // ruby piece的指针

	TP_CONTENTPIECE()
		:lStartAtomID(-1),
		 nPieceType(TP_PIECE_TEXT),
		 nAlignType(TP_ALIGN_LEFT),
         bLeftMarginSet(false),
         bRightMarginSet(false),
         bTopMarginSet(false),
         bBottomMarginSet(false),
		 dLeftMargin(0.0),
		 dRightMargin(0.0),
		 dTopMargin(0.0),
		 dBottomMargin(0.0),
         nWrappingType(TP_WRAPPING_ALONE),
         nFollowImageType(TP_FOLLOWIMAGE_IMAGE),
		 nVAlignType(TP_BOX_VALIGN_BASELINE),
         dVAlignLength(0.0),
         nVAlignUnit(TP_LENGTH_PX),
		 bRotate(true),
		 pAdvElement(NULL), 
         bNoBreakBefore(false), 
         bNoBreakInside(false), 
         bNoBreakAfter(false), 
         bEmphasis(false),
         pRubyPiece(NULL)
	{}
};

// 排版处理器初始状态参数
struct TP_INITIALSTATEPARA
{
	bool bNewLineStart;                         // 是否以新行开始，如果bNewParagraphStart = true则被忽略
    bool bNewParagraphStart;					// 是否以新段落开始
    bool bReachParagraphEnd;                    // 传入Piece是否到达段末尾，只有TP_LAYOUT_SUCCEED时使用，影响最后一行的拉伸处理

	TP_INITIALSTATEPARA()
		: bNewLineStart(false)
        , bNewParagraphStart(false)
        , bReachParagraphEnd(true)
	{}

    TP_INITIALSTATEPARA& operator=(const TP_INITIALSTATEPARA& isp)
	{
		bNewLineStart = isp.bNewLineStart;
		bNewParagraphStart = isp.bNewParagraphStart;
        bReachParagraphEnd = isp.bReachParagraphEnd;

		return *this;
	}
};

//===========================================================================

class ITitanPart
{
public:
	//-------------------------------------------
	//	Summary:
	//		设置排版区域。
	//  Parameters:
	//		[in] rcBlock		- 块边界矩形。
	//	Remarks:
	//		排版区域左上角坐标是(0, 0)点，右下角坐标是(rcBlock.Width(), rcBlock.Height())。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetBoundingBox(const TP_BOX& rcBlock) = 0;

	//-------------------------------------------
	//	Summary:
	//		获取排版结果枚举器。
	//  Parameters:
	//		Null
	//	Return Value:
	//		如果成功则返回枚举器接口指针，否则返回NULL。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual ILayoutEnumerator* GetLayoutEnumerator() = 0;

public:
	//-------------------------------------------
	//	Summary:
	//		设置空行处理策略。
	//  Parameters:
	//		[in] nBlankLineStrategy		- 空行处理策略。
	//	Remarks:
	//		如果不设置空行处理策略，处理器默认为MP_BLANKLINE_COPY。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetBlankLineStrategy(TP_BLANKLINE_STRATEGY nBlankLineStrategy) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置用连字符号连接时使用的语言。
	//  Parameters:
	//		[in] nLang				- Hyphenation使用语言。
	//	Remarks:
	//		如果不设置，处理器默认为TP_HYPHENATION_LANGUAGE_UNDEFINED，不进行
	//		连字符号连接操作。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetHyphenationLanguage(TP_HYPHENATION_LANGUAGE nLang) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置标点符号竖排时候的样式。
	//  Parameters:
	//		[in] nStyle				- 标点符号竖排时候的样式。
	//	Remarks:
	//		当标点没有字形替换时，使用指定的竖排样式调整标点符号的位置。
	//		参考WORD里面竖排时候的标点符号的样式。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetPunctuationVerticalStyle(TP_PUNCTUATION_VERTICAL_STYLE nStyle) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置排版方向。
	//  Parameters:
	//		[in] direct		- 排版方向，默认是横排。
	//	Remarks:
	//		Null
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetLayoutDirection(TP_DIRECTION_TYPE direct) = 0;

	//-------------------------------------------
	//	Summary:
	//		设置中文排版环境。
	//  Parameters:
	//		[in] nLayoutEnv	- 中文排版环境设置，可以为组合值。
	//	Remarks:
    //		默认的设置是都不打开。
    //      只有在设置了SHRINK_CJK_PUNCTUATION的情况下才能设置SHRINK_LINE_BEGIN_PUNCTUATION，
    //      单独设置SHRINK_LINE_BEGIN_PUNCTUATION是没有效果的。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetCJKLayoutEnvironment(TP_CJK_LAYOUT_ENV nLayoutEnv) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置标点最大压缩程度。
    //  Parameters:
    //      [double] dPuncShrinkRate - 标点最大压缩比例。
    //-------------------------------------------
    virtual void SetPuncMaxShrinkRate(double dPuncShrinkRate) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置行拉伸最大程度。
    //  Parameters:
    //      [double] dStretchRate - 行拉伸的最大比例，为负表示不限制。
    //-------------------------------------------
    virtual void SetMaxStretchRate(double dStretchRate) = 0;

public:
    virtual bool GetLinesHeight(const TP_FONT &baseFont, 
                                double lineGap, 
                                unsigned int lineCount, 
                                const TP_FONT &dropCapFont, 
                                TP_WCHAR dropCapChar, 
                                double *pDropCapFontSize) = 0;

protected:
	//-------------------------------------------
	//	Summary:
	//		虚析构函数。
	//-------------------------------------------
	virtual ~ITitanPart() {}
};

//===========================================================================

#endif	//#ifndef TITANPART_INTERFACE_HEADERFILE
