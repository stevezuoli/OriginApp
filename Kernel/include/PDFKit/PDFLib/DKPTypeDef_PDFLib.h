//===========================================================================
// Summary:
//		接口文件中的基本数据结构
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-9-13
// Author:
//		Zhang Jingdan (zhangjingdan@duokan.com)
//===========================================================================

#ifndef __KERNEL_PDFKIT_PDFLIB_DKPTYPEDEF_PDFLIB_H__
#define __KERNEL_PDFKIT_PDFLIB_DKPTYPEDEF_PDFLIB_H__

//===========================================================================
#include "KernelType.h"
#include "DKPTypeDef.h"

enum DKP_GRAOPEX_TYPE
{
	DKP_GRAOPEX_m		= 0,
	DKP_GRAOPEX_l		= 1,
	DKP_GRAOPEX_c		= 2,
	DKP_GRAOPEX_v		= 3,
	DKP_GRAOPEX_y		= 4,
	DKP_GRAOPEX_re		= 5,
	DKP_GRAOPEX_cp		= 6,
	DKP_GRAOPEX_ep		= 7,
	DKP_GRAOPEX_eoep	= 8
};

// 路径线型信息
typedef struct __DKPGRACONTENT
{
	DK_DOUBLE			fPoint[6];		// 路径控制点坐标
	DKP_GRAOPEX_TYPE	graOP;			// 路径图形操作类型
	__DKPGRACONTENT*	pNextNode;
} DKPGRACONTENT, *PDKPGRACONTENT;

//===========================================================================

typedef struct __DKPIMAGEOBJ
{
	DK_INT			nRefNum;
	DK_INT			nImageType;					// 图像类型
	DKP_DRECT		rcImageBBox;				// 图像外接矩形
	DK_BITMAPINFO*	pBmpInfo;					// 图像头信息
	DK_BYTE*		pBMPbuffer;					// 图像数据
	DK_INT			nImageDataLen;				// 图像数据长度
	DK_DOUBLE		dFillOpacity;				// 透明度
	DK_DOUBLE		szCTM[6];					// 图像绘制时的坐标变换矩阵
	DKP_DRECT		rcClipBox;					// 图像的裁剪区

	DK_INT			nMaskType;					// 图像掩膜的类型：	0:无Mask 1:image mask 2:soft mask
												//						3:颜色通道mask	 4:颜色mask
	DK_BITMAPINFO*	pBmpInfoMask;				// Mask图像头信息
	DK_BYTE*		pMaskBMPbuffer;				// Mask掩模图数据
	DK_RGBQUAD		clrCurFill;					// 当前填充颜色
	DK_RGBQUAD		clrMaskMin;					// 掩膜的颜色区间
	DK_RGBQUAD		clrMaskMax;
	__DKPIMAGEOBJ()
	{
		nRefNum = -1;
		pBmpInfo = DK_NULL;
		pBMPbuffer = DK_NULL;
		dFillOpacity = 1;
		nMaskType = 0;
		pBmpInfoMask = DK_NULL;
		pMaskBMPbuffer = DK_NULL;
	}
	// 裁剪区
	__DKPGRACONTENT* pClipArea;
} DKPIMAGEOBJ, *PDKPIMAGEOBJ;

// Form对象
typedef struct __DKPFORMOBJ
{
	DKP_DRECT			rcFormBBox;				// 外接矩形
	DK_DOUBLE			szCTM[6];				// 绘制时的坐标变换矩阵
	PDKPOBJNODE			pContent;				// 对象链表
	DK_BOOL				bSoftMask;				// Form对象是否为SoftMask
	DK_DOUBLE			dStrokAlpha;			// Strok Alpha
	DK_DOUBLE			dFillAlpha;				// Fill Alpha
	PDKPGRACONTENT		pClipArea;				// 裁剪区
} DKPFORMOBJ;

struct __DKPPATTERNNODE;

// 路径图形流节点
typedef struct __DKPPATHCONTENTNODE
{
	DKPDRAWPATHINFO			drawpathinfo;
	PDKPGRACONTENT			pPathContent;		// 路径信息
	PDKPGRACONTENT			pClip;				// 路径裁剪区链表
	__DKPPATTERNNODE*		pFillPattern;
	__DKPPATTERNNODE*		pStrokePattern;
	DK_INT					nPathsID;			// 保存该Path节点被Build Path后，所分配的ID值，以便写Piece时用到  	
	DKP_DRECT				rcBBox;				// 外接矩形
	__DKPPATHCONTENTNODE*	pNext;				// 下一个路径节点
	__DKPPATHCONTENTNODE(){};
	__DKPPATHCONTENTNODE(const __DKPPATHCONTENTNODE &ctnod)
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
} DKPPATHCONTENTNODE, *PDKPPATHCONTENTNODE;

// 文件中包含的字符串信息，不是unicode版本，只管string
typedef struct __DKPTEXTINFOEX
{
	DK_WCHAR*			pContent;			// 文字内容
	PDKPDISPLAYCHARNODE pCodeContent;		// 文字的编码信息
	DK_WCHAR*			pFontName;			// 字体
	DK_VOID*			pXRef;
	DK_INT				nOBJNum;			// Font OBJ ID
	DK_FONT_STYLE		nFontStyle;			// 风格
	DK_DOUBLE			dXFontSize;			// 字号
	DK_DOUBLE			dYFontSize;
	DK_DOUBLE			dCharSpace;			// 字符间距
	DK_DOUBLE			dWordSpace;			// 词间距
	DK_INT				nRenderMode;		// RenderMode
	DK_INT				nWritingMode;		// WritingMode  0:横排;1:竖排
	DK_DWORD			rgbTextColor;		// 颜色
	DK_RGBCOLOR			rgbFillColor;		// 文字勾边颜色
	DK_RGBCOLOR			rgbStrokeColor;		// 文字勾边颜色
	DK_DOUBLE			dLineWith;			// 文字勾边线宽
	DKP_DRECT			rcBBox;				// 外接矩形
	DK_DOUBLE			szCTM[6];			// 坐标变换矩阵
	DK_WCHAR*			pEncoding;			// 字体的编码类型
	PDKPGRACONTENT		pClip;
	DK_INT				lMainID;				 	
	DK_INT				lSubID;				  	
	__DKPTEXTINFOEX(){};
	__DKPTEXTINFOEX(const __DKPTEXTINFOEX &textinfo)
	{
		pContent = textinfo.pContent;
		pFontName = textinfo.pFontName;
		nOBJNum = textinfo.nOBJNum;
		nFontStyle = textinfo.nFontStyle;
		dXFontSize = textinfo.dXFontSize;
		dYFontSize = textinfo.dYFontSize;
		rgbTextColor = textinfo.rgbTextColor;
		rgbFillColor = textinfo.rgbFillColor;
		rgbStrokeColor = textinfo.rgbStrokeColor;
		dCharSpace = textinfo.dCharSpace;
		dWordSpace = textinfo.dWordSpace;
		nRenderMode = textinfo.nRenderMode;
		nWritingMode = textinfo.nWritingMode;
		rcBBox = textinfo.rcBBox;
		pEncoding = textinfo.pEncoding;
		pClip = textinfo.pClip;
		lMainID = textinfo.lMainID;
		lSubID = textinfo.lSubID;
	};
} DKPTEXTINFOEX, *PDKPTEXTINFOEX;

// 文字流节点
typedef struct __DKPTEXTINFONODEEX
{
	DKPTEXTINFOEX Node;
	__DKPTEXTINFONODEEX* pNext;
	__DKPTEXTINFONODEEX(){};
	__DKPTEXTINFONODEEX(const __DKPTEXTINFONODEEX &ctnod)
	{
		Node = ctnod.Node;
		pNext = ctnod.pNext;
	}
} DKPTEXTINFONODEEX, *PTEXTINFONODEEX;

typedef struct __DKPAXIALSHDNODEEX
{
	DKPAXIALSHDINFO			Node;
	PDKPGRACONTENT			pClip;
	DK_INT					nAXShdID;
	DKP_DRECT				rcBBox;
	__DKPAXIALSHDNODEEX(){};
	__DKPAXIALSHDNODEEX(const __DKPAXIALSHDNODEEX &ctnod)
	{
		Node = ctnod.Node;
		pClip = ctnod.pClip;
		nAXShdID = ctnod.nAXShdID;
		rcBBox = ctnod.rcBBox;
	}
} DKPAXIALSHDNODEEX, *PDKPAXIALSHDNODEEX;

typedef struct __DKPRADIALSHDNODEEX
{
	DKPRADIALSHDINFO		Node;
	PDKPGRACONTENT			pClip;
	DK_INT					nRDShdID;
	DKP_DRECT				rcBBox;
	__DKPRADIALSHDNODEEX(){};
	__DKPRADIALSHDNODEEX(const __DKPRADIALSHDNODEEX &ctnod)
	{
		Node = ctnod.Node;
		pClip = ctnod.pClip;
		nRDShdID = ctnod.nRDShdID;
		rcBBox = ctnod.rcBBox;
	}
} DKPRADIALSHDNODEEX, *PDKPRADIALSHDNODEEX;

typedef struct __DKPTRISHDNODEEX
{
	DK_DOUBLE*				pParsedData;
	DK_INT					nDataNum;
	PDKPGRACONTENT			pClip;
	DK_INT					nTriShdID;
	DKP_DRECT				rcBBox;
	__DKPTRISHDNODEEX(){};
	__DKPTRISHDNODEEX(const __DKPTRISHDNODEEX &ctnod)
	{
		pParsedData = ctnod.pParsedData;
		nDataNum = ctnod.nDataNum;
		pClip = ctnod.pClip;
		nTriShdID = ctnod.nTriShdID;
		rcBBox = ctnod.rcBBox;
	}
} DKPTRISHDNODEEX, *PDKPTRISHDNODEEX;

typedef struct __DKPPATTERNNODE
{
	DK_DOUBLE			szContentSize[4];
	DK_DOUBLE			szCTM[6];				// 坐标变换矩阵
	PDKPOBJNODE			pContent;
	__DKPPATTERNNODE(){};
	__DKPPATTERNNODE(const __DKPPATTERNNODE &ctnod)
	{
		szContentSize[0] = ctnod.szContentSize[0];
		szContentSize[1] = ctnod.szContentSize[1];
		szContentSize[2] = ctnod.szContentSize[2];
		szContentSize[3] = ctnod.szContentSize[3];
		pContent = ctnod.pContent;
	}
} DKPPATTERNNODE, *PDKPPATTERNNODE;

//===========================================================================

#endif// __KERNEL_PDFKIT_PDFLIB_DKPTYPEDEF_PDFLIB_H__
