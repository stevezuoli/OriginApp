//===========================================================================
// Summary:
//		IDKOutputSystem.h
//      OutputSystem的抽象
// Usage:
//	    Null
// Remarks:
//		Null
// Date:
//		2011-09-17
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_RENDER_IDKOUTPUTSYSTEM_H__
#define	__KERNEL_COMMONLIBS_RENDER_IDKOUTPUTSYSTEM_H__

//===========================================================================

#include "KernelType.h"
#include "DkRenderDef.h"

//===========================================================================

class IDKOutputImage;
class IDKOutputPath;
class IDKOutputRegion;
class IDKOutputPen;
class IDKOutputBrush;

class IDKOutputSystem
{
public:
    virtual ~IDKOutputSystem() {}

	//-------------------------------------------
	// Summary:
	//		获取绘制引擎的类型
	//-------------------------------------------
	virtual DK_OUTPUTSYSTEM_TYPE GetType() = 0;

	//===========================================================================
    // System
	//===========================================================================
public:
	// 创建一个图像，并开始在图像上绘制。创建的图像由调用方负责释放。
	virtual IDKOutputImage* BeginDrawOnImage(DK_INT nWidth, DK_INT nHeight) = 0;
	// 在图像上绘制结束
	virtual DK_VOID EndDrawOnImage() = 0;

	// 从DC创建DeviceCanvas
	virtual DK_VOID* CreateDeviceCanvas(DK_OUTPUTDEV_TYPE devType, DK_VOID* pDevice, DK_DOUBLE* dxPPI, DK_DOUBLE* dyPPI) = 0;
	virtual	DK_VOID ReleaseDeviceCanvas(DK_VOID* pDeviceCanvas) = 0;
	
	virtual DK_VOID* BeginComplexBlendMode(DK_VOID* pDeviceCanvas, DK_BOX rcPageBox) = 0;
	virtual DK_VOID* EndComplexBlendMode() = 0;

	virtual DK_VOID SetOrigin(DK_VOID* pDispContext, DK_INT x, DK_INT y) = 0;

    virtual DK_VOID SetBlendMode(DK_VOID* pDispContext, RM_DPK_BLENDMODE bmMode) = 0;

	// 输出设备状态的压栈与恢复
	virtual DK_VOID* SaveDevState(DK_VOID* pDispContext) = 0;
	virtual DK_VOID RestoreDevState(DK_VOID* pDispContext, DK_VOID* pDevState) = 0;

	//===========================================================================
    // Attribute
	//===========================================================================
public:
	virtual DK_BOOL IsComplexBlendMode() = 0;

    // y轴方向是否向下
    virtual DK_BOOL IsUpsideDown() = 0;

    // 设置文字绘制的Gamma参数
    virtual DK_VOID SetTextGamma(DK_DOUBLE dTextGamma) = 0;
    virtual DK_DOUBLE GetTextGamma() const = 0;

    // 现用于设置css transform，配合SaveDevState， RestoreDevState使用
    virtual DK_VOID SetTransform(DK_VOID* pDispContext, const DK_MATRIX* pTransform) = 0;

	//===========================================================================
    // Path
	//===========================================================================
public:
	// Path相关
	// 路径的创建与销毁，默认填充模式为DK_FILLMODE_ALTERNATE
	virtual IDKOutputPath* CreatePath(const IDKOutputPath* pPath = DK_NULL) = 0;

	// 设置矩形裁减区域
	virtual DK_VOID SetClipDRect(DK_VOID* pDispContext, const DK_BOX& rcClip, 
		DK_COMBINEMODE_TYPE modeClip) = 0;

	// 设置路径裁减区域
	virtual DK_VOID SetClipPath(DK_VOID* pDispContext, const IDKOutputPath* pPath, 
		DK_COMBINEMODE_TYPE clipmode, DK_FILLMODE_TYPE fillmode, DK_SMOOTHMODE_TYPE smoothmode, DK_INT nFlatness) = 0;

	// 设置多边形裁减区
	virtual DK_VOID SetClipPolygon(DK_VOID* pDispContext, const DK_POINT* pPoint, 
		const DK_INT* pLength, DK_INT nCount, DK_FILLMODE_TYPE fillmode) = 0;

	// 获取裁减外围矩形
	virtual DK_VOID GetClipBounds(DK_VOID* pDispContext, DK_BOX *pRect) = 0;

	// 绘制路径 (2.2)
	virtual DK_VOID DrawPath(DK_VOID* pDispContext, IDKOutputPen* pPen, const IDKOutputPath *pPath, 
		DK_SMOOTHMODE_TYPE smoothmode) = 0;

	// 填充矩形 (2.3)
	virtual DK_VOID FillRectangle(DK_VOID* pDispContext, IDKOutputBrush* pBrush, 
		const DK_BOX& rc, DK_FILLMODE_TYPE fillmode, DK_SMOOTHMODE_TYPE smoothmode) = 0;

	// 填充路径 (2.4)
	virtual DK_VOID FillPath(DK_VOID* pDispContext, IDKOutputBrush* pBrush, 
		const IDKOutputPath *pPath, DK_FILLMODE_TYPE fillmode, DK_SMOOTHMODE_TYPE smoothmode, DK_BOOL bCharGlyph = DK_FALSE, DK_BOOL bSmoothGap = DK_FALSE) = 0;

	// 填充路径 (2.4)
	virtual DK_VOID DrawPathWithTiling(DK_VOID* pDispContext, IDKOutputBrush* pBrush, IDKOutputPen* pPen, 
		const IDKOutputPath *pPath, DK_FILLMODE_TYPE fillmode, DK_SMOOTHMODE_TYPE smoothmode, DK_BOOL bCharGlyph = DK_FALSE) = 0;
	//===========================================================================
    // Image
	//===========================================================================
public:
	// 指定宽度和高度及颜色位数的图像的创建和销毁
	virtual IDKOutputImage* CreateImage(DK_INT nWidth, DK_INT nHeight) = 0;
	virtual IDKOutputImage* CreateImage(DK_BITMAPINFO* lpBmpInfo, DK_VOID* lpBmpData) = 0;

    // 将提供的Image数据绘制到指定大小、位置和透明度
    virtual DK_VOID OutputImage(DK_VOID* pDispContext, IDKOutputImage* pImage, const DK_POS& pos, DK_FLOAT fAlpha = 1.0, const DK_MATRIX* pTransform = DK_NULL) = 0;
    virtual DK_VOID OutputImage(DK_VOID* pDispContext, IDKOutputImage* pImage, const DK_BOX& box, DK_FLOAT fAlpha = 1.0, const DK_MATRIX* pTransform = DK_NULL) = 0;

	// 允许图像缩放和裁减 (1.1)
	// 图像从其左上顶点开始输出，宽度与高度为：dSrcWidth, dSrcHeight；dpt0: 图像左上顶点在输出设备上的坐标。
	virtual DK_VOID DrawImage(DK_VOID* pDispContext, const DK_BOX& rcDst, 
		DK_DOUBLE dSrcX, DK_DOUBLE dSrcY, DK_DOUBLE dSrcWidth, DK_DOUBLE dSrcHeight, 
		const DK_BITMAPINFO* pInfo, const DK_BYTE* pData) = 0;

	// 允许图像缩放和裁减，且有透明处理 (1.2)
	// minClr, maxClr: 作透明处理的颜色范围。其余参数同(1.1)。
	virtual DK_VOID DrawImage(DK_VOID* pDispContext, const DK_RGBQUAD& minClr, const DK_RGBQUAD& maxClr, 
		const DK_MATRIX* pTransform, const DK_BITMAPINFO* pInfo, const DK_BYTE* pData) = 0;

	// 图像直接显示 (1.5)
	virtual DK_VOID DrawImage(DK_VOID* pDispContext, DK_DOUBLE dDstX, DK_DOUBLE dDstY,
		const DK_BITMAPINFO* pInfo, const DK_BYTE* pData) = 0;

	// 图像直接透明显示 (1.6)
	virtual DK_VOID DirectDrawImage(DK_VOID* pDispContext, DK_DOUBLE dDstX, DK_DOUBLE dDstY,
		const DK_BITMAPINFO* pInfo, const DK_BYTE* pData, const DK_RGBQUAD& rgbFill, DK_DWORD dwRop) = 0;

	virtual DK_VOID DirectDrawImage(DK_VOID* pDispContext, const DK_MATRIX* pTransform,
		const DK_BITMAPINFO* pInfo, const DK_BYTE* pData, const DK_RGBQUAD& rgbFill, DK_DWORD dwRop) = 0;

	// 掩模图像显示 (1.7)
	virtual DK_VOID DrawMaskImageByPos(DK_VOID* pDispContext, 
		DK_DOUBLE dDstX, DK_DOUBLE dDstY, const DK_BITMAPINFO* pInfo, const DK_BYTE* pData, 
		DK_DOUBLE dDstMaskX, DK_DOUBLE dDstMaskY, const DK_BITMAPINFO* pMaskInfo, const DK_BYTE* pMaskData, 
		const DK_RGBQUAD& rgbFill) = 0;

	// 图像通过alpha变换后显示 (1.8)
	// alhpa值在[0.0, 1.0]中。
	virtual DK_VOID DrawImage(DK_VOID* pDispContext, DK_DOUBLE dDstX, DK_DOUBLE dDstY, DK_DOUBLE dAlpha,
		IDKOutputImage* pImage) = 0;

	// 图像通过alpha变换后显示 (1.9)
	// alhpa值在[0.0, 1.0]中,可缩放
	virtual DK_VOID DrawImage(DK_VOID* pDispContext, const DK_BOX& dstBox, DK_DOUBLE dAlpha,
		IDKOutputImage* pImage) = 0;

	// 图像通过alpha变换后显示 (1.9)
	// alhpa值在[0.0, 1.0]中,可缩放, 裁减
	virtual DK_VOID DrawImage(DK_VOID* pDispContext, const DK_BOX& dstBox, const DK_BOX& srcBox, DK_DOUBLE dAlpha,
		IDKOutputImage* pImage) = 0;

	virtual DK_VOID MultiplyDrawImage(DK_VOID* pDispContext, DK_DOUBLE dDstX, DK_DOUBLE dDstY, DK_DOUBLE dAlpha,
		IDKOutputImage* pImage) = 0;
    
	// 图像通过blur后显示 
	virtual DK_VOID DrawImage(DK_VOID* pDispContext, DK_DOUBLE dDstX, DK_DOUBLE dDstY, DK_DOUBLE dAlpha, DK_DOUBLE blur,
		IDKOutputImage* pImage) = 0;
	//===========================================================================
    // Text
	//===========================================================================
public:
	// 输出文字位图
	virtual DK_VOID DrawTextImg(DK_VOID* pDispContext, const DK_BITMAPGRAY8& bitmap, DK_INT nTopX, DK_INT nTopY, const DK_ARGBCOLOR& rgbFill) = 0;
    
	// 输出模糊处理的文字位图
	virtual DK_VOID DrawTextImg(DK_VOID* pDispContext, const DK_BITMAPGRAY8& bitmap, DK_INT nTopX, DK_INT nTopY, const DK_ARGBCOLOR& rgbFill, DK_DOUBLE blurRadius) = 0;
    
	//===========================================================================
    // Misc
	//===========================================================================
public:
	// 输出设备刷子的创建与销毁
	virtual IDKOutputBrush* CreateSolidBrush(const DK_RGBQUAD& clr) = 0;

	//-------------------------------------------
	//	Summary:
	//		创建tiling pattern（瓦片底纹）的画刷。
	//	Parameters:
	//		[in] pImage	- 瓦片图像。
	//		[in] tilingtype - 类型，暂时没有实现。
	//		[in] dTopX - pImage 中用于创建画刷的部分是（dTopX, dTopY, dWidth, dHeight），按像素计算，不得超过pImage本身的大小。
	//	Return Value:
	//		返回创建的画刷。
	//	Remarks:
	//		pImage 画到设备时不进行变换，因此pImage绘制的原点是设备的(0，0)点。
	//		deprecated.此方法不建议使用，请改用CreateImagePatternBrush(IDKOutputImage*, DK_IMAGEPATTERN_TILING_TYPE,const DK_MATRIX&)。
	//-------------------------------------------
	virtual IDKOutputBrush* CreateImagePatternBrush(IDKOutputImage* pImage, DK_IMAGEPATTERN_TILING_TYPE tilingtype,
		DK_DOUBLE dTopX, DK_DOUBLE dTopY, DK_DOUBLE dWidth, DK_DOUBLE dHeight) = 0;

	//-------------------------------------------
	//	Summary:
	//		创建tiling pattern（瓦片底纹）的画刷。
	//	Parameters:
	//		[in] pImage	- 瓦片图像。此方法使用整个图像做单元，x-step 和 y-step 就是图像的宽和高。
	//		[in] tilingtype - 类型，暂时没有实现。
	//		[in] transform - pImage 画到设备时要进行的变换。原点、x-step 和 y-step 等皆受影响。
	//		这个变换不影响设备本身的当前变换，也不影响使用该画刷绘制的图元本身的变换。
	//	Return Value:
	//		返回创建的画刷。
	//	Remarks:
	//		Null。
	//-------------------------------------------
	virtual IDKOutputBrush* CreateImagePatternBrush(IDKOutputImage* pImage, DK_IMAGEPATTERN_TILING_TYPE tilingtype,
		const DK_MATRIX& transform) = 0;

	// 输出设备笔的创建与销毁
	virtual IDKOutputPen* CreatePen(DK_VOID* pDispContext) = 0;

#ifdef DKRENDERDEBUGLOG	// 记录Log
	virtual DK_VOID LogFuncCallCt(DK_LONG lPageNum, std::vector<DK_LONG>* pVecTotal, LPCDKWSTR szFilePath, DK_BOOL bReset) = 0;
#endif

};

#endif
