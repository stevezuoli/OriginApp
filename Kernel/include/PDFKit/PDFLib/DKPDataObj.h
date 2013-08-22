//===========================================================================
// Summary:
//		CDataObj是所有提取的PDF内容对象数据的基类。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2009-09-16
// Author:
//		Zhang Jingdan (zhangjingdan@duokan.com)
//===========================================================================

#ifndef __KERNEL_PDFKIT_PDFLIB_DKPDATAOJB_H__
#define __KERNEL_PDFKIT_PDFLIB_DKPDATAOJB_H__

//===========================================================================

#include <vector>
#include <map>
#include "KernelRenderType.h"
#include "KernelType.h"
#include "DKPTypeDef.h"

//===========================================================================

struct DKP_DATA_PATH;
struct DKP_DATA_TEXT;
struct DKP_DATA_OBJ;
struct DKP_DATA_PATTERN;
class DKPDataObjs;

//===========================================================================

enum DKP_CLIP_TYPE
{
	DKP_CLIP_PATH = 1,
	DKP_CLIP_TEXT = 2,
	DKP_CLIP_ALL  = 3
};

struct DKP_DATA_CLIP
{
	DKP_CLIP_TYPE type;		// 裁剪区类型
	DKPDataObjs* pObjs;
	DK_BOX box;				// Clip的外接矩形
	DKP_DATA_CLIP()
	{
		type = DKP_CLIP_PATH;
		pObjs = DK_NULL;
	}
};

struct DKP_DATA_STATE
{
	DKP_DATA_CLIP* pClip;		// Clip对象的指针
	DK_RGBCOLOR rgbFillColor;	// 填充颜色
	DK_RGBCOLOR rgbStrokeColor;	// 勾边颜色
	
	DK_DOUBLE dLineWidth;		// 线宽
	DK_INT nLineCap;			// line cap
	DK_INT nLineJoin;			// line join
	DK_DOUBLE dMiterLimit;		// Miter Limit
	DK_DOUBLE szDash[3];		// dash pattern

	DK_DOUBLE dFillOpacity;		// 填充透明度
	DK_DOUBLE dStrokeOpacity;	// 勾边透明度
	DKP_DATA_STATE()
	{
		pClip = DK_NULL;
		dLineWidth = 0.0;
		nLineJoin = 0;
		nLineCap = 0;
		dMiterLimit = 0.0;
		memset(szDash, 0, 3 * sizeof(DK_DOUBLE));
		dFillOpacity = 0.0;
		dStrokeOpacity = 0.0;
	}
};

//===========================================================================

struct DKP_DATA_OBJ
{
	DKP_OBJ_TYPE type;		// 对象类型
	DK_POS pos;				// 对象的位置
	DK_BOX box;				// 外接矩形
	DK_DOUBLE szCTM[6];		// 坐标变换矩阵

	DKP_DATA_STATE* pStats;		// 状态数据
	DKP_DATA_PATTERN* pFillPattern;
	DKP_DATA_PATTERN* pStrokPattern;

	DKP_DATA_OBJ()
	{
		type = DKP_OBJ_NULL;
		memset(szCTM, 0, 6 * sizeof(DK_DOUBLE));
		pStats = DK_NULL;
		pFillPattern = DK_NULL;
		pStrokPattern = DK_NULL;
	}
};

typedef std::vector<DKP_DATA_OBJ*> DKPOBJVEC;

class /*DKPKERNEL_API*/ DKPDataObjs
{
public:
	DKPDataObjs(DKPOBJVEC* pVec);
	DKP_DATA_OBJ* GetObjAt(DK_INT nIndex);
	DK_INT GetObjCount();

	DKPOBJVEC* GetObjVec();
	DK_VOID AddObj(DKP_DATA_OBJ* pObj);
private:
	DKPOBJVEC* m_pObjVec;
};

struct DKP_DATA_TEXT : public DKP_DATA_OBJ
{
	DK_WCHAR* strContent;				// 文字内容
	DK_WCHAR* strFontName;				// 文字名
	PDKPDISPLAYCHARNODE pCodeContent;	// 文字编码信息
	DK_INT nObjNum;						// 字体对象号
	DK_VOID* pXRef;						// 字体对象的Ref
	DK_BOOL bEmbed;						// 字体是否内嵌
	DK_DOUBLE dCharSpace;				// 字间距
	DK_DOUBLE dWordSpace;				// 词间距
	DK_DOUBLE dXFontSize;				// x方向字号
	DK_DOUBLE dYFontSize;				// y方向字号
	DK_INT nRenderMode;					// 渲染模式0-7
	DK_INT nWritingMode;				// 0：横排； 1：竖排
	DKP_DATA_TEXT()
	{
		type = DKP_OBJ_TEXT;
		pStats = DK_NULL;
		strContent = DK_NULL;
		strFontName = DK_NULL;
        pXRef = DK_NULL;
		nObjNum = 0;
		bEmbed = DK_FALSE;
		dCharSpace = 0.0;
		dWordSpace = 0.0;
		dXFontSize = 0.0;
		dYFontSize = 0.0;
		nRenderMode = 0;
		nWritingMode = 0;
	}
};

struct DKP_DATA_TEXT_TYPE3 : public DKP_DATA_OBJ
{
	DK_INT nFontRef;
	DK_DOUBLE dFontSize;
	DK_UINT nUnicode;
	DK_DOUBLE dCharWithdAndBox[6];
	DKPDataObjs* pObjs;
};

struct DKP_DATA_IMAGE : public DKP_DATA_OBJ
{
	DK_INT nObjNum;						// 图像的对象号
	DK_BITMAPINFO* pInfo;				// inline image图头信息, 非inline时为空
	DK_BYTE* pBuffer;					// inline image图数据，非inline时为空
	DK_INT nLen;

	DKP_MASK_TYPE nMaskType;			// 图像掩模类型：0：无mask	1：image mask	2：soft mask	3：颜色通道mask	4：颜色mask
	DK_BITMAPINFO* pMaskInfo;			// 掩模图头信息
	DK_BYTE* pMaskBuffer;				// 掩模图数据
	DK_RGBQUAD clrMaskMin;				// 颜色区间最小值
	DK_RGBQUAD clrMaskMax;				// 颜色区间最大值
	DKP_DATA_IMAGE()
	{
		type = DKP_OBJ_IMAGE;
		pStats = DK_NULL;
		nObjNum = 0;
		pInfo = DK_NULL;
		pBuffer = DK_NULL;
		nLen = 0;
		nMaskType = DKP_MASK_NONE;
		pMaskInfo = DK_NULL;
		pMaskBuffer = DK_NULL;
	}
};

struct DKP_DATA_PATH : public DKP_DATA_OBJ
{
	PDKPLINE pPathContent;				// Path 内容
	DKP_PATHEND_TYPE endType;			// 结束时的绘制类型（默认0）	
	DK_BOOL bNoneZero;					// 填充奇偶性

	DKP_DATA_PATH()
	{
		type = DKP_OBJ_IMAGE;
		pStats = DK_NULL;
		pPathContent = DK_NULL;
		endType = DKP_PATHEND_DEFAULT;
		bNoneZero = DK_TRUE;
	}
};

struct DKP_DATA_FORM : public DKP_DATA_OBJ
{
	DKPDataObjs* pContentVec;				// Form对象内容
	DK_BOOL bSoftMask;					// Form对象是否为SoftMask
	DKP_DATA_FORM()
	{
		type = DKP_OBJ_FORM;
		pStats = DK_NULL;
		pContentVec = DK_NULL;
		bSoftMask = DK_FALSE;
	}
};

struct DKP_DATA_SHD : public DKP_DATA_OBJ
{
	DK_INT nInputSize;
	DK_INT nOutputSize;
	DK_DOUBLE pFuncDomain[8*2];
	DK_DOUBLE pFuncRange[8*2];
	DKP_DATA_SHD()
	{
		nInputSize = 0;
		nOutputSize = 0;
		memset(pFuncDomain, 0, 16 * sizeof(DK_DOUBLE));
		memset(pFuncRange, 0, 16 * sizeof(DK_DOUBLE));
	}
};

struct DKP_DATA_SHD_AXIAL : public DKP_DATA_SHD
{
	DK_INT nExtend;
	DK_RGBCOLOR rgbStart;
	DK_RGBCOLOR rgbEnd;
	DK_DOUBLE x0;
	DK_DOUBLE y0;
	DK_DOUBLE x1;
	DK_DOUBLE y1;
	DKP_DATA_SHD_AXIAL() : DKP_DATA_SHD()
	{
		type = DKP_OBJ_AXIALSHD;
		pStats = DK_NULL;
		nExtend = 0;
		x0 = y0 = x1 = y1 = 0.0;
	}
};

struct DKP_DATA_SHD_RADIAL : public DKP_DATA_SHD
{
	DK_INT nExtend0;
	DK_INT nExtend1;
	DK_RGBCOLOR rgbStart;
	DK_RGBCOLOR rgbEnd;
	DK_DOUBLE x0;
	DK_DOUBLE y0;
	DK_DOUBLE x1;
	DK_DOUBLE y1;
	DK_DOUBLE r0;
	DK_DOUBLE r1;
	DK_DOUBLE dEccentricity;
	DK_DOUBLE dAngle;
	DKP_DATA_SHD_RADIAL() : DKP_DATA_SHD()
	{
		type = DKP_OBJ_RADIALSHD;
		pStats = DK_NULL;
		nExtend0 = DK_NULL;
		nExtend1 = DK_NULL;
		x0 = y0 = x1 = y1 = 0.0;
		r0 = r1 = 0.0;
		dEccentricity = 0.0;
		dAngle = 0.0;
	}
};

struct DKP_DATA_SHD_TRI : public DKP_DATA_SHD
{
	DK_DOUBLE* pParsedData;
	DK_INT nDataNum;
	DKP_DATA_SHD_TRI() : DKP_DATA_SHD()
	{
		type = DKP_OBJ_TRISHD;
		pStats = DK_NULL;
		pParsedData = DK_NULL;
		nDataNum = 0;
	}
};

struct DKP_DATA_PATTERN : public DKP_DATA_OBJ
{
	DK_DOUBLE szContentSize[4];
	DK_DOUBLE szCTM[6];				// 坐标变换矩阵
	DKPDataObjs* pObjs;
	DKP_DATA_PATTERN()
	{
		type = DKP_OBJ_PATTERN;
		pStats = DK_NULL;
		memset(szContentSize, 0, 4 * sizeof(DK_DOUBLE));
		memset(szCTM, 0, 6 * sizeof(DK_DOUBLE));
		pObjs = DK_NULL;
	}
};

//===========================================================================

struct DKP_DATA_FONT
{
	DK_INT nObjNum;						// 字体对象号
	DK_BOOL bEmbeded;					// 是否内嵌
	DK_WCHAR* strFontName;				// 字体名字
	DK_WCHAR* strFontNameInUse;			// 使用的名字
	DK_WCHAR* strFontFamily;			// 字体族名称
	DKP_FONT_TYPE fontType;				// Font的种类
	DK_FONT_STYLE styleFont;			// 字体风格
// 	DK_BYTE pFontTag;					// 字体标记
	DK_WCHAR* strEncoding;				// 字体编码类型
	DK_BOOL bHasUnicode;				// 是否有ToUnicode表
	DK_BOOL bUseDefaultFont;			// 使用默认字体
	DK_INT nEmbFontFileRefObj;			// 内嵌字体数据的Obj号
	DK_BYTE* pBuffer;					// 字体数据
	DK_INT nBufferLen;					// 字体数据长度
	DK_WCHAR* strFontfilePath;			// 字体路径
	DK_INT nDefWidth;					// default char width
	DK_INT nDefHeight;					// default char height
	DK_INT nDefVY;						// default origin position
	DKP_DATA_FONT()
	{
		nObjNum = 0;
		bEmbeded = DK_FALSE;
		strFontName = DK_NULL;
		strFontNameInUse = DK_NULL;
		fontType = DKP_FONT_UNKNOWN;
		styleFont = DK_FontStyleRegular;
		strEncoding = DK_NULL;
		bUseDefaultFont = DK_FALSE;
		bHasUnicode = DK_FALSE;
		pBuffer = DK_NULL;
		nBufferLen = 0;
		strFontfilePath = DK_NULL;
		nDefWidth = 0;
		nDefHeight = 0;
		nDefVY = 0;
	}
};

typedef std::map<DK_INT, DKP_DATA_FONT*> DKP_DATARES_FONTMAP;

class /*DKPKERNEL_API*/ DKPFontResMap
{
public:
	DKPFontResMap(DKP_DATARES_FONTMAP* pMapFont);
	DKP_DATA_FONT* GetAtByObjnum(DK_INT nObjnum);
	DKP_DATA_FONT* GetAt(DK_INT nIndex);
	DK_INT GetCount();
private:
	DKP_DATARES_FONTMAP* m_pMapFont;
};

struct DKP_DATA_IMAGEDATA
{
	DK_INT nObjNum;						// 图像的对象号
	DK_BITMAPINFO* pBMPInfo;			// 图像头信息
	DK_BYTE* pBMPBuffer;				// 图像数据
	DK_INT nDataLen;					// 数据长度
};

typedef std::map<DK_INT, DKP_DATA_IMAGEDATA*> DKP_DATARES_IMAGEMAP;

class /*DKPKERNEL_API*/ DKPImageResMap
{
public:
	DKPImageResMap(DKP_DATARES_IMAGEMAP* pMapImage);
	DKP_DATA_IMAGEDATA* GetAt(DK_INT nIndex);
private:
	DKP_DATARES_IMAGEMAP* m_pMapImage;
};
//===========================================================================

#endif
