//===========================================================================
// Summary:
//		类型定义
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-9-13
// Author:
//		Zhang Jingdan (zhangjingdan@duokan.com)
//===========================================================================

#ifndef __KERNEL_PDFKIT_PDFLIB_DKPTYPEDEF_H__
#define __KERNEL_PDFKIT_PDFLIB_DKPTYPEDEF_H__

//===========================================================================

#include "KernelDef.h"
#include "DKPKernel.h"
#include <map>
#include <vector>
#include "KernelType.h"
#include "KernelBaseType.h"
#include "KernelRenderType.h"

//===========================================================================
// 基本类型

// 矩阵操作类
typedef struct __DKPMATRIX
{
	DK_DOUBLE a11;
	DK_DOUBLE a12;
	DK_DOUBLE a21;
	DK_DOUBLE a22;
} DKPMATRIX;

typedef DK_VOID* DKP_HANDLE;
typedef DKP_HANDLE DKP_HDEV;
typedef DK_UINT DKP_TEXTCHAR;


// 单位类型
enum DKP_UNIT_TYPE
{
    DKP_UNIT_UNKNOWN = 0,
    DKP_UNIT_PX,                                 // 像素
    DKP_UNIT_PERCENT                             // 百分比
};

/////////////////////////////////////////////////////////////////////////////

// 同步操作
typedef DKP_HANDLE DKP_HLOCK;
typedef DKP_HANDLE DKP_HEVENT;
// 线程的句柄
typedef DKP_HANDLE DKP_HTHREAD;

/////////////////////////////////////////////////////////////////////////////

// Complex type
typedef struct	__DKP_DRECT
{
	DK_DOUBLE	left;
	DK_DOUBLE	top;
	DK_DOUBLE	right;
	DK_DOUBLE	bottom;

	__DKP_DRECT& operator = (const __DKP_DRECT &dRect)
	{
		this->left = dRect.left;
		this->top = dRect.top;
		this->right = dRect.right;
		this->bottom = dRect.bottom;

		return *this;
	}
    
    bool operator == (const __DKP_DRECT &dRect)
	{
		return (this->left == dRect.left && this->top == dRect.top &&
		        this->right == dRect.right && this->bottom == dRect.bottom);
    }

    bool operator != (const __DKP_DRECT &dRect)
    {
        return (this->left != dRect.left || this->top != dRect.top ||
            this->right != dRect.right || this->bottom != dRect.bottom);
    }

    DK_BOOL IsEmpty() const
    {
        return (left == right && top == bottom);
    }

} DKP_DRECT;

struct DKP_RECT
{
	DK_LONG	left;
	DK_LONG	top;
	DK_LONG	right;
    DK_LONG	bottom;

    DK_LONG Height() const
    {
        return abs(top - bottom);
    }

    DK_LONG Width() const
    {
        return abs(right - left);
    }

    DK_BOOL IsEmpty() const
    {
        return (left == right && top == bottom);
    }
};

// 四边形:逆时针顺序四点坐标
typedef struct __DKP_DQUAD
{
	DK_DOUBLE x1;
	DK_DOUBLE y1;
	DK_DOUBLE x2;
	DK_DOUBLE y2;
	DK_DOUBLE x3;
	DK_DOUBLE y3;
	DK_DOUBLE x4;
	DK_DOUBLE y4;

	__DKP_DQUAD& operator = (const __DKP_DQUAD &dQuad)
	{
		this->x1 = dQuad.x1;
		this->x2 = dQuad.x2;
		this->x3 = dQuad.x3;
		this->x4 = dQuad.x4;
		this->y1 = dQuad.y1;
		this->y2 = dQuad.y2;
		this->y3 = dQuad.y3;
		this->y4 = dQuad.y4;

		return *this;
	}

	__DKP_DQUAD& operator = (const __DKP_DRECT &dRect)
	{
		this->x1 = dRect.left;
		this->x2 = dRect.right;
		this->x3 = dRect.left;
		this->x4 = dRect.right;
		this->y1 = dRect.top;
		this->y2 = dRect.top;
		this->y3 = dRect.bottom;
		this->y4 = dRect.bottom;

		return *this;
	}

	DK_BOOL IsInRect(const DKP_DRECT &rect) const
	{
		if (x1 < rect.left || x1 > rect.right) return DK_FALSE;
		if (x2 < rect.left || x2 > rect.right) return DK_FALSE;
		if (x3 < rect.left || x3 > rect.right) return DK_FALSE;
		if (x4 < rect.left || x4 > rect.right) return DK_FALSE;
		if (y1 < rect.bottom || y1 > rect.top) return DK_FALSE;
		if (y2 < rect.bottom || y2 > rect.top) return DK_FALSE;
		if (y3 < rect.bottom || y3 > rect.top) return DK_FALSE;
		if (y4 < rect.bottom || y4 > rect.top) return DK_FALSE;

		return DK_TRUE;
	}
} DKP_DQUAD;

typedef struct __DKP_DPOINT
{
	DK_DOUBLE x;
	DK_DOUBLE y;

	__DKP_DPOINT()
	{
		this->x = 0.0;
		this->y = 0.0;
	}

	__DKP_DPOINT& operator = (const __DKP_DPOINT &dPt)
	{
		this->x = dPt.x;
		this->y = dPt.y;

		return *this;
	}
} DKP_DPOINT;

//===========================================================================
// 对象

// 路径节点数组结束时的绘制类型
enum DKP_PATHEND_TYPE
{
	DKP_PATHEND_DEFAULT	= 0,
	DKP_PATHEND_n		= 1,
	DKP_PATHEND_s		= 2,
	DKP_PATHEND_f		= 3,
	DKP_PATHEND_sf		= 4
};

//===========================================================================
// 页面绘制与打印，以及搜索线程参数

#define PDFFuncMaxInputs  8
#define PDFFuncMaxOutputs 8

enum DKP_PAGEBOX_TYPE 
{ 
	DKP_PAGEBOX_MEDIABOX, 
	DKP_PAGEBOX_CROPBOX, 
	DKP_PAGEBOX_CROPBOX_ORGINAL 
};

// 页面的Box
typedef struct __DKPPAGEBOXEX
{
	DKP_DRECT mediaBox;
	DKP_DRECT cropBox;
	DKP_DRECT bleedBox;
	DKP_DRECT trimBox;
} DKPPAGEBOXEX, *PDKPPAGEBOXEX;

// 简繁搜索查找的标记
enum  DKP_SEARCH_TYPE
{
	DKP_SEARCH_SAMETOUI,			// 与文本相同
	DKP_SEARCH_GB,					// GB码
	DKP_SEARCH_BIG5					// Big5码
};

// 查找搜索选项
typedef struct __DKP_SEARCHOPTION
{
	const DK_WCHAR*		szPattern;			// 要查找的字符串
	DK_BOOL			bCaseSensitive;		// 是否大小写敏感，默认False
	DK_BOOL			bWholeWord;			// 是否整字匹配，默认为False
	DK_BOOL			bMultiMatch;		// 是否多词匹配，默认为False
	DK_INT			nContextLen;		// 所需匹配字符串上下文的长度（单方向），-1表示不返回匹配文字信息，默认为-1
	DKP_SEARCH_TYPE nLangType;			// 语言选项，默认为DKP_SEARCH_SAMETOUI

	__DKP_SEARCHOPTION()
		: szPattern(DK_NULL), bCaseSensitive(DK_FALSE), bWholeWord(DK_FALSE), bMultiMatch(DK_FALSE), nContextLen(-1), nLangType(DKP_SEARCH_SAMETOUI)
	{}
} DKP_SEARCHOPTION;

// 查找搜索结果
typedef struct __DKP_SEARCHRESULT
{
    DKP_DRECT*          pRects;                 // 匹配项所包含的外接矩形
    DK_UINT             nRectCount;             // 外接矩形个数
    DK_WCHAR*           szText;                 // 匹配的文字及上下文
    DK_INT              nMatchBegin;            // 匹配文字在szText中的位置
    DK_INT              nMatchLength;           // 匹配文字的长度
    DK_FLOWPOSITION     matchStartFlowPos;      // 匹配文字在文档中的起始索引位置
    DK_FLOWPOSITION     matchEndFlowPos;        // 匹配文字在文档中的结束索引位置，左闭右开.

    __DKP_SEARCHRESULT()
        : pRects(DK_NULL), nRectCount(0), szText(DK_NULL), nMatchBegin(0)
    {}
} DKP_SEARCHRESULT;

typedef struct __DKPDOCPOS
{
	DK_INT nPage;
	DK_INT nStringNum;
	DK_INT nCharNum;
} DKPDOCPOS;

// 拷贝简繁转换标记
enum DKP_COPY_TYPE
{
	DKP_COPY_NOTRANS,
	DKP_COPY_TOGB,
	DKP_COPY_TOBIG5
};

enum DKP_TEXTSELECTION_TYPE 
{
	DKP_TEXTSELECTION_RAW = 1,
	DKP_TEXTSELECTION_RECT	
};

enum DKP_TEXTCONTENT_TYPE
{
	DKP_TEXTCONTENT_RAW = 1,
	DKP_TEXTCONTENT_TYPESET
};

//===========================================================================
// 路径图形类型

// 路径线型种类
enum DKP_GRAOP_TYPE
{
	DKP_GRAOP_m		= 0,
	DKP_GRAOP_l		= 1,
	DKP_GRAOP_c		= 2,
	DKP_GRAOP_v		= 3,
	DKP_GRAOP_y		= 4,
	DKP_GRAOP_re	= 5,
	DKP_GRAOP_cp	= 6,
	DKP_GRAOP_ep	= 7,
	DKP_GRAOP_eoep  = 8,
	DKP_GRAOP_S		= 9,
	DKP_GRAOP_F		= 10,
	DKP_GRAOP_f_	= 11
};

// 路径线型信息
typedef struct __DKPLINE
{
	DK_DOUBLE		fPoint[6];			// 路径控制点坐标
	DKP_GRAOP_TYPE	graOP;				// 路径图形操作类型	
	__DKPLINE*		pNextNode;			// 下一个线型信息
	__DKPLINE(){};
	__DKPLINE(const __DKPLINE &ctnod)
	{
		fPoint[0] = ctnod.fPoint[0];
		fPoint[1] = ctnod.fPoint[1];
		fPoint[2] = ctnod.fPoint[2];
		fPoint[3] = ctnod.fPoint[3];
		fPoint[4] = ctnod.fPoint[4];
		fPoint[5] = ctnod.fPoint[5];
		graOP = ctnod.graOP;
		pNextNode = ctnod.pNextNode;
	}
} DKPLINE, *PDKPLINE;

//===========================================================================
// 图像信息
typedef struct __DKPIMAGECONTENT
{
	DK_INT			nImageType;					// 图像类型
	DKP_DRECT		rcImageBBox;				// 图像外接矩形
	DK_BITMAPINFO*	pBitmapInfo;				// 图像头信息
	DK_BYTE*		pBitmapData;				// 图像数据
	DK_DOUBLE		szCTM[6];					// 图像绘制时的坐标变换矩阵
	DKP_DRECT		rcClipBox;					// 图像的裁剪区
	DK_INT			nMaskType;					// 图像掩膜的类型：
	DK_BITMAPINFO*	pMaskBmpInfo;				// Mask图像头信息
	DK_BYTE*		pMaskBmpData;				// 掩模图像数据
	DK_RGBQUAD		clrMaskMin;					// 掩膜的颜色区间
	DK_RGBQUAD		clrMaskMax;
	PDKPLINE		pPathClipList;

	__DKPIMAGECONTENT* pNextNode;
} DKPIMAGECONTENT, *PDKPIMAGECONTENT;

//===========================================================================
// 文字类型

// 文字的类型
enum DKP_FONT_TYPE 
{
	//----- PDF8BitFont
	DKP_FONT_UNKNOWN,
	DKP_FONT_TYPE1,
	DKP_FONT_TYPE1C,
	DKP_FONT_TYPE3,
	DKP_FONT_TRUETYPE,
	//----- PDFType0Font
	DKP_FONT_TYPE0,
	DKP_FONT_CIDTYPE0,
	DKP_FONT_CIDTYPE0C,
	DKP_FONT_CIDTYPE2
};

// 字宽属性
typedef struct __DKPCHARWIDTHS
{
	DK_INT nWidth;
	DK_INT nHeight;
	DK_INT nVx;
	DK_INT nVy;
} DKPCHARWIDTHS;

typedef struct __DKPDISPLAYCHAR
{
	DK_UINT		uCode;					// 字符编码
	DK_UINT		uUnicode[8];			// 字符的Unicode码
	DK_UINT		uGid;					// 字符的Glyph index
	DK_WCHAR*	szCharName;				// 字符的名字
	DK_DOUBLE	dXPos;					// 字符位置的x坐标
	DK_DOUBLE	dYPos;					// 字符位置的y坐标
	DKP_DRECT	rcBox;					// 字符的矩形
	DKPCHARWIDTHS charWidth;			// 字宽数据
	__DKPDISPLAYCHAR(){};
} DKPDISPLAYCHAR;

typedef struct __DKPDISPLAYCHARNODE
{
	DKPDISPLAYCHAR			charNode;
	__DKPDISPLAYCHARNODE*	pNext;
} DKPDISPLAYCHARNODE, *PDKPDISPLAYCHARNODE;

// 文件中包含的字符串信息
typedef struct __DKPTEXTINFO
{
	DK_WCHAR*			strContent;			// 文字内容
	PDKPDISPLAYCHARNODE pCodeContent;		// 文字的编码信息
	DK_WCHAR*			strFontName;		// 字体
	DK_WCHAR*			strFontNameInUse;	// 正在使用中的字体名称
    DK_WCHAR*           validFontName;      // 可供外部排版引擎实用的字体名
    DK_WCHAR*           validFontFilePath;  
	DK_VOID*			pXRef;
	DK_INT				nObjNum;
	DK_CHAR*			pFontTag;			// tag
	DK_FONT_STYLE		nFontStyle;			// 风格
	DK_DOUBLE			dXFontSize;			// x方向字号
	DK_DOUBLE			dYFontSize;			// y方向的字号
	DK_BOOL				bIsEmbFont;			// 是否为内嵌字体
	DKP_FONT_TYPE		fontType;			// 字体类型, 以便重排时跳过Type3字体, 待Type3字体重排支持后可去掉
    DK_VOID*            pFontInfo;          // 内嵌字体信息
	DK_DOUBLE			dCharSpace;			// 字符间距
	DK_DOUBLE			dWordSpace;			// 词间距
	DK_INT				nRenderMode;		// RenderMode
	DK_INT				nWritingMode;		// WritingMode  0:横排;1:竖排
	DK_DWORD			rgbTextColor;		// 颜色
	DK_RGBCOLOR			rgbFillColor;		// 文字填充颜色
	DK_RGBCOLOR			rgbStrokeColor;		// 文字勾边颜色
	DK_DOUBLE			dLineWith;			// 文字勾边线宽
	DKP_DRECT			rcBBox;				// 外接矩形
	DK_DOUBLE			szCTM[6];			// 坐标变换矩阵
	DK_WCHAR*			strEncoding;		// 字体的编码类型
	__DKPLINE*			pClip;
    DK_BOOL             bIsFirstTextOfTJ;   // 是否是Tj操作符作用参数的第一个文本
	__DKPTEXTINFO()
	{
		strContent = DK_NULL;
        pCodeContent = DK_NULL;
		strFontName = DK_NULL;
		strFontNameInUse = DK_NULL;
        validFontName = DK_NULL;
        validFontFilePath = DK_NULL;
		pFontTag = DK_NULL;
		strEncoding = DK_NULL;
		pClip = DK_NULL;
        pXRef = DK_NULL;
		fontType = DKP_FONT_UNKNOWN;
        pFontInfo = DK_NULL;
        bIsFirstTextOfTJ = DK_FALSE;        
        nFontStyle = DK_FontStyleRegular;
	};
	__DKPTEXTINFO(const __DKPTEXTINFO &textinfo)
	{
		strContent = textinfo.strContent;
		pCodeContent = textinfo.pCodeContent;
		strFontName = textinfo.strFontName;
		strFontNameInUse = textinfo.strFontNameInUse;
        validFontName = textinfo.validFontName;
        validFontFilePath = textinfo.validFontFilePath;
		pXRef = textinfo.pXRef;
		nObjNum = textinfo.nObjNum;
		pFontTag = textinfo.pFontTag;
		nFontStyle = textinfo.nFontStyle;
		dXFontSize = textinfo.dXFontSize;
		dYFontSize = textinfo.dYFontSize;
		dCharSpace = textinfo.dCharSpace;
		dWordSpace = textinfo.dWordSpace;
		nRenderMode = textinfo.nRenderMode;
		nWritingMode = textinfo.nWritingMode;
		rgbTextColor = textinfo.rgbTextColor;
		rgbFillColor = textinfo.rgbFillColor;
		rgbStrokeColor = textinfo.rgbStrokeColor;
		rcBBox = textinfo.rcBBox;
		bIsEmbFont = textinfo.bIsEmbFont;
		strEncoding = textinfo.strEncoding;
		pClip = textinfo.pClip;
		fontType = textinfo.fontType;
        pFontInfo = textinfo.pFontInfo;
        bIsFirstTextOfTJ = textinfo.bIsFirstTextOfTJ;
	};
} DKPTEXTINFO, *PDKPTEXTINFO;

// 文字流节点
typedef struct __DKPTEXTINFONODE
{
	DKPTEXTINFO		Node;
    DK_FLOWPOSITION pos;
	__DKPTEXTINFONODE*	pNext;
	__DKPTEXTINFONODE()
    {
        pNext = DK_NULL;
    };
	__DKPTEXTINFONODE(const __DKPTEXTINFONODE &ctnod)
	{
		Node = ctnod.Node;
        pos = ctnod.pos;
		pNext = ctnod.pNext;
	}
} DKPTEXTINFONODE, *PDKPTEXTINFONODE;

// 页面内容类型
enum DKPCONTENTTYPE
{
    DKP_CONTENT_UNKNOWN,
	DKP_CONTENT_TEXT,		// 文字
	DKP_CONTENT_GRAPH,	    // 图形
	DKP_CONTENT_IMAGE,		// 图像
};

// 图像元素类型
enum DKP_IMG_ELEM_TYPE
{
    DKP_IMG_ELEM_UNKNOWN,
    DKP_IMG_ELEM_TEXT,      // 文字（扫描图重排时，切割后的图像为文字类图像）
    DKP_IMG_ELEM_PIC        // 图片
};

// 页面内容流节点, 标记在原文档位置
struct DKPPAGECONTENTFLOWNODE
{
    DKPCONTENTTYPE          type;
    DKP_IMG_ELEM_TYPE       imgSubType;   // 图像节点子类型，仅当type为DKP_CONTENT_IMAGE时有效
    DK_BOOL                 isExtended;   // 是否是插入到链表中的扩展节点（如换行节点插入，空格补全节点插入）
    union
    {
	    DKPTEXTINFO*		pTextNode;
        DKPIMAGECONTENT*    pImageNode;
    };
    DK_FLOWPOSITION         pos;
	DKPPAGECONTENTFLOWNODE*	pNext;
    DK_BOOL                 holdImgContent; // 是否负责释放 pImageNode 中记录的 bitmap info 和 bitmap data。（当 type 为 DKP_CONTENT_IMAGE 时有效）

	DKPPAGECONTENTFLOWNODE()
        : type(DKP_CONTENT_TEXT),
          imgSubType(DKP_IMG_ELEM_UNKNOWN),
          isExtended(DK_FALSE),
          pTextNode(DK_NULL),
          pNext(DK_NULL),
          holdImgContent(DK_FALSE)
    {
    };
};

// 定义重排页面文字枚举时获取当前字符信息
struct DKP_CHAR_INFO
{
    DK_BOX boundingBox;                 // 外接矩形
    DK_WCHAR charCode;                  // 文字的 Unicode 编码
    DK_FLOWPOSITION pos;                // 文字的索引位置
};

typedef struct __DKPFONTDATA
{
	DK_BYTE*	byteFontBuf;			// 字体数据
	DK_INT		nLength;				// 数据长度
	__DKPFONTDATA()
	{
		byteFontBuf = DK_NULL;
		nLength = 0;
	}
} DKPFONTDATA;

// 字体信息链表
typedef struct __DKPFONTDATANODE
{
	DKPFONTDATA			ctsFont_Data;
	__DKPFONTDATANODE *	pNext;
} DKPFONTDATANODE;

typedef struct __DKPDOCFONTINFO
{
	DK_WCHAR*	szFontFaceName;			// 字体名字
	DK_INT		nFontType;				// 字体类型
	DK_BOOL		bEmbedded;				// 是否为内嵌字体
	DK_INT		nEncodingType;			// 编码种类
	DK_WCHAR*	szEncoding;				// 编码类型
	__DKPDOCFONTINFO *pActualFontInfo;	// 实际字体
} DKPDOCFONTINFO;

//===========================================================================

typedef struct __DKPMETADATA
{
	DK_WCHAR* pszTitle;				// 文档的题目
	DK_WCHAR* pszAuthor;			// 文档的作者
	DK_WCHAR* pszSubject;			// 文档的主题
	DK_WCHAR* pszKeywords;			// 文档的关键字
	DK_WCHAR* pszCreator;			// 创建文档的应用程序
	DK_WCHAR* pszProducer;			// PDF加工程序
	DK_WCHAR* pszCreationDate;		// 文档的创建时间
	DK_WCHAR* pszModDate;			// 文档的修改时间
	__DKPMETADATA()
	{
		pszTitle = DK_NULL;
		pszAuthor = DK_NULL;
		pszSubject = DK_NULL;
		pszKeywords = DK_NULL;
		pszCreator = DK_NULL;
		pszProducer = DK_NULL;
		pszCreationDate = DK_NULL;
		pszModDate = DK_NULL;
	}
} DKPMETADATA, *PDKPMETADATA;

//===========================================================================
// Outline

#define CAPTION_MAX_LENGTH 250

// 目录项结构
struct DKPOUTLINEITEMINFO
{
	DK_WORD		dwIndex;					// 结点索引
	DK_WCHAR*	pwszTitle;					// 标题
	DK_BOOL		bHasChild;					// 是否有子结点
	DK_BOOL		bStartOpen;					// 在显示时是否需要展开子结点
};

// 目录项映射到树形结构
enum DKP_TREEQUERY_TYPE 
{
	DKP_TQ_FIRSTCHILD,						// 第一个孩子节点
	DKP_TQ_NEXTSIBLING,						// 下一个兄弟节点
	DKP_TQ_PARENT							// 双亲节点
};

//===========================================================================
// 文件信息

typedef struct __DKPFILEINFO
{
	DK_LONG lFileDataLength;
	DK_BYTE* pFileDataBuf;

	__DKPFILEINFO()
	{
		lFileDataLength = 0;
		pFileDataBuf = DK_NULL;
	};

	// 释放数据
	DK_VOID FreeData()
	{
		if (pFileDataBuf)
		{
			delete[] pFileDataBuf;
			pFileDataBuf = DK_NULL;
		}
		lFileDataLength = 0;
	}
} DKPFILEINFO, *PDKPFILEINFO;

// PDF线帽风格
enum DKP_LINE_CAP_TYPE
{
	DKP_LINE_CAP_BUTT,
	DKP_LINE_CAP_ROUND,
	DKP_LINE_CAP_PROJECTING_SQAURE
};

// PDF线联接风格
enum DKP_LINE_JOIN_TYPE
{
	DKP_LINE_JOIN_MITER,
	DKP_LINE_JOIN_ROUND,
	DKP_LINE_JOIN_BEVEL
};

// PDF色彩混合模式
enum DKP_BLEND_MODE_TYPE
{
	DKP_BLEND_MODE_NORMAL,
	DKP_BLEND_MODE_MULTIPLY,
// 	DKP_BLEND_MODE_SCREEN,
// 	DKP_BLEND_MODE_OVERLAY,
// 	DKP_BLEND_MODE_DARKEN,
// 	DKP_BLEND_MODE_LIGHTEN,
// 	DKP_BLEND_MODE_COLORDORGE,
// 	DKP_BLEND_MODE_COLORBURN,
// 	DKP_BLEND_MODE_HARDLIGHT,
// 	DKP_BLEND_MODE_SOFTLIGHT,
// 	DKP_BLEND_MODE_DIFFERENCE,
// 	DKP_BLEND_MODE_EXCLUSION
};

// 对象修改状态
enum DKP_MODIFY_STATE_TYPE
{
	DKP_MODIFY_STATE_NORMAL,
	DKP_MODIFY_STATE_MODIFIED,
	DKP_MODIFY_STATE_ADDED,
	DKP_MODIFY_STATE_DELETED,
};

//===========================================================================
// 注释相关

// 注释外观状态类型
enum DKP_ANNOT_AS_TYPE
{
	DKP_ANNOT_AS_UNDEFINED,

	DKP_ANNOT_AS_NORMAL,
	DKP_ANNOT_AS_ROLLOVER,
	DKP_ANNOT_AS_DOWN
};

// 注释回复类型
enum DKP_ANNOT_REPLY_TYPE
{
	DKP_ANNOT_REPLY_R,
	DKP_ANNOT_REPLY_GROUP
};

// 脱字注释的符号类型
enum DKP_CARET_ANNOT_SYMBOL_TYPE
{
	DKP_CARET_ANNOT_SYMBOL_NONE,
	DKP_CARET_ANNOT_SYMBOL_P
};

// PDF1.7中定义的7种文本注释标准图标类型，实际应用中会有更多图标类型。
enum DKP_TEXT_ANNOT_ICON_TYPE
{
	DKP_TEXT_ANNOT_ICON_NOTE,
	DKP_TEXT_ANNOT_ICON_KEY,
	DKP_TEXT_ANNOT_ICON_COMMENT,
	DKP_TEXT_ANNOT_ICON_HELP,
	DKP_TEXT_ANNOT_ICON_NEWPARAGRAGH,
	DKP_TEXT_ANNOT_ICON_PARAGRAPH,
	DKP_TEXT_ANNOT_ICON_INSERT,
};

// 文本注释状态类型
enum DKP_TEXT_ANNOT_STATE_TYPE
{
	DKP_TEXT_ANNOT_STATE_UNDEFINED,

	DKP_TEXT_ANNOT_STATE_NONE,
	DKP_TEXT_ANNOT_STATE_ACCEPTED,
	DKP_TEXT_ANNOT_STATE_REJECTED,
	DKP_TEXT_ANNOT_STATE_CANCELLED,
	DKP_TEXT_ANNOT_STATE_COMPLETED,

	DKP_TEXT_ANNOT_STATE_UNMARKED,
	DKP_TEXT_ANNOT_STATE_MARKED
};

// 文本注释状态模型类型
enum DKP_TEXT_ANNOT_STATE_MODEL_TYPE
{
	DKP_TEXT_ANNOT_STATE_MODEL_UNDEFINED,

	DKP_TEXT_ANNOT_STATE_MODEL_REVIEW,
	DKP_TEXT_ANNOT_STATE_MODEL_MARKED
};

// 链接注释高亮类型
enum DKP_LINK_ANNOT_HIGHLIGHT_TYPE
{
	DKP_LINK_ANNOT_HIGHLIGHT_I,		// 反色显示注释内容。默认类型
	DKP_LINK_ANNOT_HIGHLIGHT_N,		// 无特殊显示
	DKP_LINK_ANNOT_HIGHLIGHT_O,		// 反色显示注释边框
	DKP_LINK_ANNOT_HIGHLIGHT_P		// 使注释呈现“按下”效果
};

// PDF1.7中定义的14种印章注释标准图标类型，实际应用中会有更多图标类型。
enum DKP_STAMP_ANNOT_ICON_TYPE
{
	DKP_STAMP_ANNOT_ICON_DRAFT,
	DKP_STAMP_ANNOT_ICON_APPROVED,
	DKP_STAMP_ANNOT_ICON_EXPERIMENTAL,
	DKP_STAMP_ANNOT_ICON_NOTAPPROVED,
	DKP_STAMP_ANNOT_ICON_ASIS,
	DKP_STAMP_ANNOT_ICON_EXPIRED,
	DKP_STAMP_ANNOT_ICON_NOTFORPUBLICRELEASE,
	DKP_STAMP_ANNOT_ICON_CONFIDENTIAL,
	DKP_STAMP_ANNOT_ICON_FINAL,
	DKP_STAMP_ANNOT_ICON_SOLD,
	DKP_STAMP_ANNOT_ICON_DEPARTMENTAL,
	DKP_STAMP_ANNOT_ICON_FORCOMMENT,
	DKP_STAMP_ANNOT_ICON_TOPSECRET,
	DKP_STAMP_ANNOT_ICON_FORPUBLICRELEASE
};

// PDF1.7中定义的4种附件注释标准图标类型，实际应用中会有更多图标类型。
enum DKP_FILEATT_ANNOT_ICON_TYPE
{
	DKP_FILEATT_ANNOT_ICON_PUSHPIN,
	DKP_FILEATT_ANNOT_ICON_GRAPH,
	DKP_FILEATT_ANNOT_ICON_PAPERCLIP,
	DKP_FILEATT_ANNOT_ICON_TAG
};

enum DKP_SOUND_ANNOT_ICON_TYPE
{
	DKP_SOUND_ANNOT_ICON_SPEAKER,
	DKP_SOUND_ANNOT_ICON_MIC
};

enum DKP_SOUND_ANNOT_ENCODING_TYPE
{
	DKP_SOUND_ANNOT_ENCODING_RAW,
	DKP_SOUND_ANNOT_ENCODING_SIGNED,
	DKP_SOUND_ANNOT_ENCODING_MULAW,
	DKP_SOUND_ANNOT_ENCODING_ALAW
};

enum DKP_FREETEXT_ANNOT_QUADDING_TYPE
{
	DKP_FREETEXT_ANNOT_QUADDING_LEFT,
	DKP_FREETEXT_ANNOT_QUADDING_CENTERED,
	DKP_FREETEXT_ANNOT_QUADDING_RIGHT
};

enum DKP_LINE_ENDING_TYPE
{
	DKP_LINE_ENDING_NONE,
	DKP_LINE_ENDING_SQUARE,
	DKP_LINE_ENDING_CIRCLE,
	DKP_LINE_ENDING_DIAMOND,
	DKP_LINE_ENDING_OPENARROW,
	DKP_LINE_ENDING_CLOSEDARROW,
	DKP_LINE_ENDING_BUTT,
	DKP_LINE_ENDING_ROPENARROW,
	DKP_LINE_ENDING_RCLOSEDARROW,
	DKP_LINE_ENDING_SLASH
};

//===========================================================================
enum DKP_MASK_TYPE
{
	DKP_MASK_NONE = 0,
	DKP_MASK_IMAGE,
	DKP_MASK_SOFT,
	DKP_MASK_BYCOLOR, 
	DKP_MASK_BYPOS
};

// 解析类型标记
enum DFE_PARSE_FLAG { 
	PARSE_FLAG_NORMAL, 
	PARSE_FLAG_TEXTCONTENT, 
	PARSE_FLAG_TEXTCONTENTSTREAM, 
	PARSE_FLAG_GRACONTENTSTREAM, 
	PARSE_FLAG_IMAGECONTENTSTREAM
};

// PDF注释
enum DKP_ANNOT_TYPE
{
	DKP_ANNOT_UNKNOWN,
	DKP_ANNOT_TEXT,
	DKP_ANNOT_POPUP,
	DKP_ANNOT_LINK,
	DKP_ANNOT_FREETEXT,

	// 以下四种是文本批注注释
	DKP_ANNOT_HIGHLIGHT,
	DKP_ANNOT_UNDERLINE,
	DKP_ANNOT_SQUIGGLY,
	DKP_ANNOT_STRIKEOUT,

	DKP_ANNOT_CARET,
	DKP_ANNOT_STAMP,
	DKP_ANNOT_INK,
	DKP_ANNOT_LINE,
	DKP_ANNOT_FILEATTACHMENT,
	DKP_ANNOT_SOUND,
	DKP_ANNOT_MOVIE,
	DKP_ANNOT_SCREEN,
	DKP_ANNOT_WATERMARK,

	DKP_ANNOT_POLYGON,
	DKP_ANNOT_POLYLINE,

	DKP_ANNOT_SQUARE,
	DKP_ANNOT_CIRCLE
};

// 重排模式
enum DKP_REARRANGE_MODE
{
    DKP_REARRANGE_MODE_UNKNOWN,
    DKP_REARRANGE_MODE_CONTENTSTREAM,   // 提取页面中的内容（如文本、图形、图像等），并重排。常用于非扫描版PDF文档。
    DKP_REARRANGE_MODE_IMAGESEGMENT     // 对整体页面图像进行图像分割，并重排。常用于扫描版PDF文档。
};

// 重排时需要提取的页面内容
enum DKP_REARRANGE_CONTENT
{
    DKP_REARRANGE_CONTENT_TEXT = 1,
    DKP_REARRANGE_CONTENT_IMAGE = 2,
    DKP_REARRANGE_CONTENT_GRAPH = 4
};

enum DKP_SELECTION_MODE
{
    DKP_SELECTION_UNKNOWN,
    DKP_SELECTION_MIDDLECROSS,                      // 当坐标点越过中线时即选中当前文字 
    DKP_SELECTION_INTERCROSS,                       // 当坐标与当前字有交叉时即选中当前文字
    DKP_SELECTION_INTERCROSS_WORD                   // 当坐标点与当前字有交叉时，选中包含当前字在内的词
};

enum DKP_HITTEST_TEXT_MODE
{
    DKP_HITTEST_TEXT_UNKNOWN,
    DKP_HITTEST_TEXT_SENTENCE,                      // 以整句文本方式 HitTest 页面文字
    DKP_HITTEST_TEXT_WORD                           // 以文本分词方式 HitTest 页面文字
};

// 重排页面解析参数
struct DKP_REARRANGE_PARSER_OPTION
{
    DK_BOX                      pageBox;            // 页面尺寸。
    DKP_UNIT_TYPE               subPageBoxUnitType; // （对应原页面尺寸）需重排矩形取值单位。
    DKP_DRECT                   subPageBoxOdd;      // 奇数页（对应原页面尺寸）需重排矩形，单位由 DKP_UNIT_TYPE 定义。
    DKP_DRECT                   subPageBoxEven;     // 偶数页（对应原页面尺寸）需重排矩形，单位由 DKP_UNIT_TYPE 定义。如未设置，将使用奇数页矩形 subPageBoxOdd。
    DK_MEASURE_TYPE             msType;             // 度量单位，参见 DK_MEASURE_TYPE 枚举值说明。
    DK_LONG                     dpi;                // 输出设备的DPI值，当 msType = DK_MEASURE_PIXEL 时无效。
    DK_DOUBLE                   scale;              // 字号缩放比
    DK_DOUBLE                   lineGap;            // 行间距， 采用多倍行距
    DK_DOUBLE                   paraSpacing;        // 段间距
    DKP_REARRANGE_CONTENT       contentFilter;      // （当rearrangeMode为DKP_REARRANGE_MODE_CONTENTSTREAM时有效）选择需要重排的内容，如文本，图像等。
    DK_BOOL                     fastMode;           // 如果为DK_TRUE，获取到的页面对象将不记录页面元素，仅记录页面起始和结束位置

    DKP_REARRANGE_PARSER_OPTION()
        : subPageBoxUnitType(DKP_UNIT_PX),
        msType(DK_MEASURE_PIXEL),
        dpi(RenderConst::SCREEN_DPI),
        scale (1.0),
        lineGap (1.0),
        paraSpacing (1.0),
        contentFilter(DKP_REARRANGE_CONTENT_TEXT),
        fastMode (DK_FALSE)
    {
        memset(&subPageBoxOdd, 0, sizeof(DKP_DRECT));
        memset(&subPageBoxEven, 0, sizeof(DKP_DRECT));
    }
};

#endif
