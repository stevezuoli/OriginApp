//===========================================================================
// Summary:
//		FontEngine接口。
// Usage:
//		
//		
// Remarks:
//		Null
// Date:
//		2011-09-19
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_RENDER_IDKFONTENGINE_H__
#define	__KERNEL_COMMONLIBS_RENDER_IDKFONTENGINE_H__

#include "KernelType.h"
#include "DkRenderDef.h"

//===========================================================================

#include <map>
#include <vector>

//===========================================================================

class IDKFontManager;
class IDKOutputPath;

typedef std::map<DK_WSTRING, DK_CHARSET_TYPE> StringToCharsetMap;

#ifndef StringArray
typedef std::vector<DK_WSTRING> StringArray;
#endif

typedef std::map<DK_INT, DK_INT> DKCODEMAP;
/////////////////////////////////////////////////////////////////////////////


#define DK_ENC_TAG( value, a, b, c, d )         \
	value = ( ( (DK_UINT)(a) << 24 ) |  \
	( (DK_UINT)(b) << 16 ) |  \
	( (DK_UINT)(c) <<  8 ) |  \
	(DK_UINT)(d)         )

enum DK_ENCODING_TYPE 
{
	DK_ENC_TAG( DK_ENCODING_NONE, 0, 0, 0, 0 ),
	DK_ENCODING_WINANIS,
	DK_ENCODING_ZAPFDDINGBATS,

	DK_ENC_TAG( DK_ENCODING_MS_SYMBOL, 's', 'y', 'm', 'b' ),
	DK_ENC_TAG( DK_ENCODING_UNICODE,   'u', 'n', 'i', 'c' ),

	DK_ENC_TAG( DK_ENCODING_GB2312,  'g', 'b', ' ', ' ' ),
	DK_ENC_TAG( DK_ENCODING_BIG5,    'b', 'i', 'g', '5' ),
	// 	FT_ENC_TAG( DFE_ENCODING_WANSUNG, 'w', 'a', 'n', 's' ),
	// 	FT_ENC_TAG( DFE_ENCODING_JOHAB,   'j', 'o', 'h', 'a' ),
	// 	FT_ENC_TAG( DFE_ENCODING_SJIS,    's', 'j', 'i', 's' ),

	/* for backwards compatibility */
	// 	DFE_ENCODING_MS_SJIS    = DFE_ENCODING_SJIS,
	DK_ENCODING_MS_GB2312  = DK_ENCODING_GB2312,
	DK_ENCODING_MS_BIG5    = DK_ENCODING_BIG5,
	// 	DFE_ENCODING_MS_WANSUNG = DFE_ENCODING_WANSUNG,
	// 	DFE_ENCODING_MS_JOHAB   = DFE_ENCODING_JOHAB,

	DK_ENC_TAG( DK_ENCODING_ADOBE_STANDARD, 'A', 'D', 'O', 'B' ),
	DK_ENC_TAG( DK_ENCODING_APPLE_EXPERT,   'A', 'D', 'B', 'E' ),
	// 	FT_ENC_TAG( FT_ENCODING_ADOBE_CUSTOM,   'A', 'D', 'B', 'C' ),
	// 	FT_ENC_TAG( FT_ENCODING_ADOBE_LATIN_1,  'l', 'a', 't', '1' ),
	// 
	// 	FT_ENC_TAG( FT_ENCODING_OLD_LATIN_2, 'l', 'a', 't', '2' ),

	DK_ENC_TAG( DK_ENCODING_APPLE_ROMAN, 'a', 'r', 'm', 'n' ),
};
typedef enum PIXELMODE_ {
	PIXEL_MODE_NONE = 0,
	PIXEL_MODE_MONO,        // 单色位图
	PIXEL_MODE_PAL4,        // 4-BIT PALETTED - 16 COLORS
	PIXEL_MODE_PAL8,        // 8-BIT PALETTED - 256 COLORS
	PIXEL_MODE_GRAY,        // 8-BIT GRAY LEVELS
	PIXEL_MODE_RGB555,      // 15-BITS MODE - 32768 COLORS
	PIXEL_MODE_RGB565,      // 16-BITS MODE - 65536 COLORS
	PIXEL_MODE_RGB24,       // 24-BITS MODE - 16 MILLION COLORS
	PIXEL_MODE_RGB32,       // 32-BITS MODE - 16 MILLION COLORS
	PIXEL_MODE_MAX          // DON'T REMOVE
} PIXELMODE;

// FreeType支持的字库类型
typedef enum FTFONTTYPE_ {
	FT_TYPE1,
	FT_TRUETYPE,
	FT_TYPE0,
	FT_CFF,
	FT_TYPE3,
	FT_MAXTYPE
} FTFONTTYPE;

typedef struct FTBITMAP_ {
	DK_INT			nRows;				// 像素高度
	DK_INT			nWidth;				// 像素宽度
	DK_INT			nPitch;				// + or - the number of bytes per row
	PIXELMODE		mode;				// pixel mode of bitmap buffer
	DK_INT			nGrays;				// number of grays in palette for PAL8 mode. 0 otherwise
	DK_BYTE*		pBuffer;			// pointer to pixel buffer
	FTBITMAP_ ()
	{
		nRows = 0;
		nWidth = 0;
		nPitch = 0;
		mode = PIXEL_MODE_NONE;
		nGrays = 0;
		pBuffer = DK_NULL;
	};
} FTBITMAP;

const DK_UINT FF_FILEFACE = 1;				// 表明字型存在于外部文件中
const DK_UINT FF_EMBEDDED_TYPE1 = 2;		// 表明是下载的Type1字体
const DK_UINT FF_EMBEDDED_TRUETYPE = 3;		// 表明是下载的TrueType字体
const DK_UINT FF_EMBEDDED_TYPE0 = 4;		// 表明是下载的Type0字体
const DK_UINT FF_EMBEDDED_CFF = 5;			// 表明是下载的CFF字体


// Unicode character.
typedef DK_UINT DK_UNICODE;

// Character ID for CID character collections.
typedef DK_UINT DK_CID;

// This is large enough to hold any of the following:
// - 8-bit char code
// - 16-bit CID
// - Unicode
typedef DK_UINT DK_CHARCODE;
// 为显示时定义的字体属性
typedef struct _DK_DISPLAYFONT
{
	DK_WSTRING		strFontName;
	DK_BOOL			bEmbeddedFont;
	DK_VOID*	    pXRef;
	DK_INT			nObjNum;
	DK_CHARSET_TYPE Charset;
	DK_INT			nVert;
	DK_FONT_STYLE	FontStyle;
	DK_SLRGB		rgbColor;
	DK_MATRIX		TMatrix;
	DK_DOUBLE		xSize;
	DK_DOUBLE		ySize;
} DK_DISPLAYFONT;

// 为显示时定义的字符属性
typedef struct _DK_DISPLAYCHAR
{
	DK_CHARCODE		uCode; // none use for none pdf
	DK_UNICODE		uUnicode[8]; 
	DK_CID			uGid; // glyph id
	DK_CHAR*		szCharName; // ignore by ePub
	DK_POS			ptOrg; // logic coordinate
	_DK_DISPLAYCHAR()
	{
		uCode = 0;
        DkZero(uUnicode);
		uGid = 0;
		szCharName = DK_NULL;
	}
} DK_DISPLAYCHAR;

// 路径节点数组结束时的绘制类型
enum DK_PATHEND_TYPE
{
	DK_PATHEND_DEFAULT	= 0,
	DK_PATHEND_n		= 1,
	DK_PATHEND_s		= 2,
	DK_PATHEND_f		= 3,
	DK_PATHEND_sf		= 4
};

typedef struct	__DK_DRECT
{
	DK_DOUBLE	left;
	DK_DOUBLE	top;
	DK_DOUBLE	right;
	DK_DOUBLE	bottom;
} DK_DRECT;

// 路径图形中包含的绘制信息
typedef struct __DKDRAWPATHINFO
{
	DK_DOUBLE			dWidth;						// 路径宽度
	DK_BOOL				bNoneZero;					// 填充奇偶性
	DK_DWORD			dFillColor;					// 填充颜色
	DK_DWORD			dStrokeColor;				// 勾边颜色
	DK_DOUBLE			dFillOpacity;				// 填充透明度
	DK_DOUBLE			dStrokeOpacity;				// 勾边透明度
	DK_INT				nLineJoin;					// linejoin
	DK_INT				nLineCap;					// linecap
	DK_PATHEND_TYPE		endType;					// 结束时的绘制类型（默认0）	
	DK_DOUBLE			szDash[3];					// 虚线式样
	DK_DRECT			rcBBox;						// 外接矩形
	__DKDRAWPATHINFO() {};
	__DKDRAWPATHINFO(const __DKDRAWPATHINFO &drawpathinfo)
	{
		dWidth = drawpathinfo.dWidth;
		bNoneZero = drawpathinfo.bNoneZero;
		dFillColor = drawpathinfo.dFillColor;
		dStrokeColor = drawpathinfo.dStrokeColor;
		dFillOpacity = drawpathinfo.dFillOpacity;
		dStrokeOpacity = drawpathinfo.dStrokeOpacity;
		nLineJoin = drawpathinfo.nLineJoin;
		nLineCap = drawpathinfo.nLineCap;
		endType = drawpathinfo.endType;
		szDash[0] = drawpathinfo.szDash[0];
		szDash[1] = drawpathinfo.szDash[1];
		szDash[2] = drawpathinfo.szDash[2];
		rcBBox = drawpathinfo.rcBBox;
	};
} DKDRAWPATHINFO;

enum DK_GRAOPEX_TYPE
{
	DK_GRAOPEX_m		= 0,
	DK_GRAOPEX_l		= 1,
	DK_GRAOPEX_c		= 2,
	DK_GRAOPEX_v		= 3,
	DK_GRAOPEX_y		= 4,
	DK_GRAOPEX_re		= 5,
	DK_GRAOPEX_cp		= 6,
	DK_GRAOPEX_ep		= 7,
	DK_GRAOPEX_eoep		= 8
};

// 路径线型信息
typedef struct __DKGRACONTENT
{
	DK_DOUBLE			fPoint[6];		// 路径控制点坐标
	DK_GRAOPEX_TYPE		graOP;			// 路径图形操作类型
	__DKGRACONTENT*	pNextNode;
} DKGRACONTENT;

// 路径图形流节点
typedef struct __DKPATHCONTENTNODE
{
	DKDRAWPATHINFO			drawpathinfo;
	DKGRACONTENT*			pPathContent;		// 路径信息
	DKGRACONTENT*			pClip;				// 路径裁剪区链表
	DK_VOID*				pFillPattern;
	DK_VOID*				pStrokePattern;
	DK_INT					nPathsID;			// 保存该Path节点被Build Path后，所分配的ID值，以便写Piece时用到  	
	DK_DRECT				rcBBox;				// 外接矩形
	__DKPATHCONTENTNODE*	pNext;				// 下一个路径节点
	__DKPATHCONTENTNODE(){};
	__DKPATHCONTENTNODE(const __DKPATHCONTENTNODE &ctnod)
	{
		drawpathinfo = ctnod.drawpathinfo;
		pPathContent = ctnod.pPathContent;
		pClip = ctnod.pClip;
		pFillPattern = ctnod.pFillPattern;
		pStrokePattern = ctnod.pStrokePattern;
		nPathsID = ctnod.nPathsID;
		rcBBox = ctnod.rcBBox;
		pNext = ctnod.pNext;
	}
} DKPATHCONTENTNODE;

//===========================================================================

enum FONTFACE_STYLE_TYPE
{
    FONTFACE_STYLE_NORMAL,
    FONTFACE_STYLE_BOLD,
    FONTFACE_STYLE_ITALIC,
    FONTFACE_STYLE_BOLDITALIC,
    FONTFACE_STYLE_OBLIQUE
};

struct FONTFACELITE
{
    DK_WSTRING		    strFilePathName;			// 字体文件全路径名
    DK_WSTRING		    strFontFaceName;			// 字体的Face名，用于在同一个字体文件内存在多余一个的字体的情况
    DK_INT			    nFaceIndex;		            // 针对多Face ttc
    DK_INT			    nCMapIndex;		            // cmap index
    FONTFACE_STYLE_TYPE nType;                      // 字体风格

    DK_VOID Reset()
    {
        strFilePathName.clear();
        strFontFaceName.clear();
        nFaceIndex = 0;
        nCMapIndex = 0;
        nType = FONTFACE_STYLE_NORMAL;
    }
};

//===========================================================================

#define USEBYTECODE(code) (code >= 0x2E80)

//===========================================================================

class IDKFontEngine
{
public:
	virtual ~IDKFontEngine(){};

	// 初始化FreeType库
	virtual DK_VOID InitFreeTypeLib() = 0;
	// 结束FreeType库的调用
	virtual DK_VOID DoneFreeType() = 0;

	// 设置字体管理模块
    virtual DK_VOID SetFontManager(IDKFontManager* pFontManager) = 0;

	// 加载下载字体
    virtual DK_BOOL LoadEmbeddedFont(const DK_WSTRING& pszEmbeddedFontName, DK_BYTE* pBuffer, DK_DWORD dwSize, DK_INT ftFontType,  DK_STRING ftFontFlag, DK_VOID* pXRef, DK_INT nObjNum, DK_ENCODING_TYPE encoding = DK_ENCODING_NONE) = 0;
	// 加载非下载字体
    virtual DK_BOOL LoadFileFont(DK_WSTRING& strFaceName, DK_FONT_STYLE& FTStyle, DK_CHARSET_TYPE Charset, DK_INT nWMode, DK_BOOL bNeedSynch = DK_TRUE) = 0;

	// 设置缺省字库名
    virtual DK_BOOL SetDefaultFontFaceName(const DK_WCHAR* pszDefaultGBFontFaceName, DK_CHARSET_TYPE CharSet) = 0;

    virtual DK_BOOL GetCharStrokePath(const DK_DISPLAYFONT& DisplayFont, const DK_DISPLAYCHAR& Displaychar,
						IDKOutputPath* pOutputPath, const DK_MATRIX& ctm) = 0;
    virtual DK_BOOL GetCharPath(const DK_DISPLAYFONT& DisplayFont, const DK_DISPLAYCHAR& Displaychar,
						DKPATHCONTENTNODE* pPathContentNode) = 0;
    virtual DK_BOOL GetGlyphBBox(const DK_DISPLAYFONT& DisplayFont, const DK_DISPLAYCHAR& Displaychar,
        DK_BOX& cBox) = 0;

    virtual DK_BOOL IsFontFaceAlready()  = 0;
	// 重置当前字体
    virtual DK_BOOL ResetCurrentFont() = 0;

	// 判断简繁转换时是否需要转换
    virtual DK_BOOL IsTrans(const DK_DISPLAYCHAR Displaychar) = 0;
	// 判断简繁转换时不需要转换的字体
    virtual DK_BOOL IsTransFontName(const DK_WCHAR* pszFaceName) = 0;
	// 通过字符编码在当前字体中计算字形索引
    virtual DK_INT CalcGlyphIndex(DK_CHARCODE uCode, DK_UNICODE uUincode, DK_CID uGid, const DK_CHAR* pszCharName, DK_INT nVert = 0) = 0;
	// 通过字符编码在指定字体中计算字形索引
    virtual DK_INT CalcGlyphIndex(const DK_DISPLAYFONT& displayFont, const DK_DISPLAYCHAR& displayChar) = 0;
	// 设置当前字型
    virtual DK_BOOL SetCurrentFontFace(const DK_WCHAR* strFaceName, DK_BOOL bFileFace, DK_VOID* pXRef = DK_NULL, DK_INT nObjNum = -1, DK_BOOL bTrans = DK_FALSE) = 0;
	// 设置默认字体为当前显示字体
    virtual DK_BOOL SetDefaultFontAsCurrentFontFace(DK_CHARSET_TYPE charset) = 0;
	// 设置完字体属性后进行的操作
    virtual DK_BOOL EndUpdateFontProperty() = 0;


    // 获取Freetype输出字符灰度图
    virtual DK_BOOL GetDisplayCharBitmap(const DK_DISPLAYFONT& DisplayFont,
                                         const DK_DISPLAYCHAR& Displaychar,
                                         DK_FONT_SMOOTH_TYPE fontSmoothType,
                                         DK_BITMAPGRAY8* textBitmap,
                                         DK_DOUBLE* dTopX,
                                         DK_DOUBLE* dTopY,
                                         DK_BOOL bUseGrayMode) = 0;
    virtual DK_BOOL GetDisplayCharBitmap(DK_UNICODE uChar, 
                                         DK_CHARSET_TYPE charset,
                                         const DK_WCHAR* fontName,
                                         DK_SIZE_T fontSize,
                                         DK_FONT_STYLE fontStyle,
                                         DK_FONT_SMOOTH_TYPE fontSmoothType,
                                         DK_BITMAPGRAY8* textBitmap,
                                         DK_DOUBLE* topX,
                                         DK_DOUBLE* topY,
                                         DK_BOOL bUseGrayMode) = 0;

    virtual DK_VOID ReleaseCharBitmap(DK_BITMAPGRAY8* textBitmap) = 0;
};

class IDKFontEngineLite
{
public:
    virtual ~IDKFontEngineLite() {}

public:
    // 初始化FreeType库
    virtual DK_VOID InitFreeTypeLib() = 0;
    // 结束FreeType库的调用
    virtual DK_VOID DoneFreeType() = 0;

    virtual DK_BOOL AddFaceName(const DK_WCHAR* pszFaceName, const DK_WCHAR* pszFontFile) = 0;
    virtual DK_BOOL AddFaceFamily(DK_FONTFAMILY* pFontFamily, DK_INT nCount) = 0;
    virtual DK_BOOL RemoveFaceName(const DK_WCHAR* pszFaceName) = 0;
    virtual DK_BOOL SetDefaultFontFace(const DK_WCHAR* szDefaultFontFaceName, DK_CHARSET_TYPE charset) = 0;

    virtual const DK_WCHAR* GetDefaultFontFace(DK_CHARSET_TYPE charset) = 0;
    virtual const DK_WCHAR* GetDefaultFontFile(DK_CHARSET_TYPE charset) = 0;
    virtual const DK_WCHAR* FontFileFromFace(const DK_WCHAR* fontFace) = 0;
    virtual const DK_WCHAR* FontFaceFromFile(const DK_WCHAR* fontFile) = 0;

    virtual DK_BOOL GetDisplayCharBitmap(DK_UINT uChar, 
                                         DK_CHARSET_TYPE charset, 
                                         const DK_WCHAR* fontFaceName, 
                                         DK_SIZE_T fontSize, 
                                         DK_FONT_STYLE fontStyle, 
                                         DK_FONT_SMOOTH_TYPE fontSmoothType, 
                                         DK_BITMAPGRAY8* textBitmap, 
                                         DK_DOUBLE* topX, 
                                         DK_DOUBLE* topY, 
                                         DK_BOOL bUseGrayMode, 
                                         const DK_MATRIX *pMatrix = DK_NULL) = 0;

    virtual DK_BOOL GetDisplayGlyphBitmap(DK_DWORD dwGlyphIndex, 
                                          DK_CHARSET_TYPE charset, 
                                          const DK_WCHAR* fontFaceName, 
                                          DK_SIZE_T fontSize, 
                                          DK_FONT_STYLE fontStyle, 
                                          DK_FONT_SMOOTH_TYPE fontSmoothType, 
                                          DK_BITMAPGRAY8* textBitmap, 
                                          DK_DOUBLE* topX, 
                                          DK_DOUBLE* topY, 
                                          DK_BOOL bUseGrayMode, 
                                          const DK_MATRIX *pMatrix = DK_NULL) = 0;
    
    virtual DK_VOID ReleaseCharBitmap(DK_BITMAPGRAY8* textBitmap) = 0;
};

#endif
