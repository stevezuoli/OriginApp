//===========================================================================
// Summary:
//		设备Image
// Usage:
//		
//		
// Remarks:
//		Null
// Date:
//		2011-09-15
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_RENDER_IDKOUTPUTIMAGE_H__
#define	__KERNEL_COMMONLIBS_RENDER_IDKOUTPUTIMAGE_H__

#include "KernelType.h"

class IDKOutputImage
{
public:
	virtual ~IDKOutputImage() {}

public:
	//-------------------------------------------
	//	Summary:
	//		创建一个拷贝。
	//-------------------------------------------
	virtual IDKOutputImage* Clone() = 0;

	//-------------------------------------------
	//	Summary:
	//		获取Image对象的指针。
	//-------------------------------------------
	virtual DK_VOID* GetHandle() = 0;

	//-------------------------------------------
	//	Summary:
	//		获取图像的宽。
	//-------------------------------------------
	virtual DK_UINT GetWidth() = 0;
	
	//-------------------------------------------
	//	Summary:
	//		获取图像的高。
	//-------------------------------------------
	virtual DK_UINT GetHeight() = 0;

	//-------------------------------------------
	//	Summary:
	//		获取一点的颜色。
	//-------------------------------------------
	virtual DK_VOID GetPixel(DK_INT nX, DK_INT nY, DK_RGBQUAD& color) = 0;
	
	//-------------------------------------------
	//	Summary:
	//		设置一点的颜色。
	//	Remarks:
	//		要求Alpha值大于或等于RGB值。
	//-------------------------------------------
	virtual DK_VOID SetPixel(DK_INT nX, DK_INT nY, const DK_RGBQUAD& color) = 0;

	//-------------------------------------------
	//	Summary:
	//		颜色是否预先乘了Alpha通道。
	//-------------------------------------------
	virtual DK_BOOL UsePMColor() = 0;

	//-------------------------------------------
	//	Summary:
	//		用指定数据做Alpha掩模。
	//	Parameters:
	//		[in] nWidth			- 掩模的宽度
	//		[in] nHeight		- 掩模的高度
	//		[in] nBPC			- 掩模数据每通道的位数
	//		[in] pbyMaskData	- 原始掩模数据
	//		[in] nDataLen		- 掩模数据长度
	//-------------------------------------------
	virtual DK_VOID MaskByAlphaArray(DK_UINT nWidth, DK_UINT nHeight, DK_UINT nBPC, DK_BYTE* pbyMaskData, DK_UINT nDataLen) = 0;

	//-------------------------------------------
	//	Summary:
	//		创建一个拷贝并使用掩模(灰度值)调整透明度。
	//	Parameters:
	//		[in] pMaskImg		- 掩模
	//		[in] pSrcOffset		- 图像上进行掩模处理的区域
	//		[in] pMaskOffset	- 掩模上对应的区域
	//		[in] pFunc			- 灰度调整
	//	Remarks:
	//		pMaskImg为空时仅提供图像拷贝操作，忽略其它参数。
	//		pSrcOffset内部全部执行操作(在掩模外时为全透明)，外部保持原样。
	//		pSrcOffset或pMaskOffset为空时表示整个图像或掩模操作。
	//-------------------------------------------
	virtual IDKOutputImage* CreateAlphaMaskedImage(IDKOutputImage *pMaskImg = DK_NULL, DK_BOX* pSrcOffset = DK_NULL, DK_BOX* pMaskOffset = DK_NULL, DK_LPGRAYMSKFUNCS pFunc = DK_NULL) = 0;

    //-------------------------------------------
	//	Summary:
	//		将图像保存为位图缓存。可选方法，可以不实现，直接返回 false。
	//	Parameters:
	//		[in] pBitmapBuf		- 位图缓存。
	//	Return Value:
	//		是否成功。
	//	Remarks:
	//	    调用FreeBitmapBuf释放内存
	//-------------------------------------------
    virtual DK_BOOL DumpAsBitmapBuf(DK_BITMAPBUFFER_DEV* pBitmapBuf) = 0;

    //-------------------------------------------
	//	Summary:
	//		释放Dump出来的位图缓存
	//	Parameters:
	//		[in] pBitmapBuf		- 位图缓存。
	//	Return Value:
	//		是否成功。
	//	Remarks:
	//	    NULL
	//-------------------------------------------
    virtual DK_VOID FreeBitmapBuf(DK_BITMAPBUFFER_DEV* pBitmapBuf) = 0;

	//-------------------------------------------
	//	Summary:
	//		将图像保存为文件。可选方法，可以不实现，直接返回 false。
	//	Parameters:
	//		[in] pFileName		- 文件名。
	//	Return Value:
	//		是否成功。
	//	Remarks:
	//		图像格式可以从文件扩展名推导。但是实现不必遵循扩展名指定的格式，可以输出任意格式。
	//-------------------------------------------
	virtual DK_BOOL DumpAsFile(const DK_CHAR* pFileName) { DK_UNUSED(pFileName); return DK_FALSE; }
};

//===========================================================================

#endif
