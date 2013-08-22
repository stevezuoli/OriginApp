//===========================================================================
// Summary:
//		页面图片对象迭代器接口。
// Usage:
//		枚举当前页面中所有图片的信息，如图片的外接矩形，图片原始数据和外部数据等
// Remarks:
//		图片对象包括普通图片：DKE_PAGEOBJ_IMAGE 和 可点击图片：DKE_PAGEOBJ_INTERACIVEIMAGE
// Date:
//		2012-02-14
// Author:
//		Zhang Jiafang (zhangjf@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEIMAGEITERATOR_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEIMAGEITERATOR_H__

//===========================================================================

#include "DKEDef.h"

//===========================================================================

class IDKEImageIterator
{
public:
	//-------------------------------------------
	//	Summary:
	//		移动到下一位置。
	//  Parameters:
	//		Null
	//	Return Value:
	//		如果返回 DK_FALSE，则不应该从迭代器中继续获取数据。
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL MoveToNext() = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前位置的图片的类型，如普通图，静态图，交互图等。
	//  Parameters:
	//		Null
	//	Return Value:
	//	    DKE_PAGEOBJ_TYPE
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.1开始支持。
	//-------------------------------------------
    virtual DKE_PAGEOBJ_TYPE GetCurrentImageType() const = 0;

	//-------------------------------------------
	//	Summary:
	//		获取当前位置的图片信息。
	//  Parameters:
	//		[out] pImageInfo            - 当前位置图片信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		调用FreeImageInfo释放pImageInfo hold的资源
	//  Availability:
	//		从ePubLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_VOID GetCurrentImageInfo(DKE_HITTEST_OBJECTINFO* pImageInfo) const = 0;

	//-------------------------------------------
	//	Summary:
	//		释放图片信息。
	//  Parameters:
	//		[in] pImageInfo             - 待释放的信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 1.0开始支持。
	//-------------------------------------------
    virtual DK_VOID FreeImageInfo(DKE_HITTEST_OBJECTINFO* pImageInfo) = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前图片的外接矩形
	//  Parameters:
	//		null
	//	Return Value:
	//	    DK_BOX 外接矩形
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 1.0开始支持。
	//-------------------------------------------
    virtual DK_BOX GetCurrentImageBox() const = 0;

    //-------------------------------------------
    //	Summary:
    //		获取当前图片的裁剪区
    //  Parameters:
    //		null
    //	Return Value:
    //	    DK_BOX 裁剪区
    //	Remarks:
    //		无裁剪区时等于图像区域
    //  Availability:
    //		从ePubLib 2.4.0开始支持。
    //-------------------------------------------
    virtual DK_BOX GetCurrentImageClipBox() const = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前图片的原始大小
	//  Parameters:
	//		null
	//	Return Value:
	//	    DK_BOX 原始大小，左上点为0，右下点为宽高
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.1.0开始支持。
	//-------------------------------------------
    virtual DK_VOID GetCurrentImageSize(DK_UINT* pWidth, DK_UINT* pHeight) const = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前图片的包内全路径
	//  Parameters:
	//		null
	//	Return Value:
	//	    包内全路径
	//	Remarks:
	//		调用FreeCurrentImagePath释放返回值内存
	//  Availability:
	//		从ePubLib 2.1.0开始支持。
	//-------------------------------------------
    virtual DK_WCHAR* GetCurrentImagePath() const = 0;

    //-------------------------------------------
	//	Summary:
	//		释放当前图片路径的内存
	//  Parameters:
	//		[in]pImagePath          - 当前图片的包内全路径
	//	Return Value:
	//	    包内全路径
	//	Remarks:
    //      null
	//  Availability:
	//		从ePubLib 2.1.0开始支持。
	//-------------------------------------------
    virtual DK_VOID FreeCurrentImagePath(DK_WCHAR* pImagePath) = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前图片的标题，包括主标题和副标题
	//  Parameters:
	//		[in]ppMainTitle          - 主标题
    //      [in]ppSubTitle           - 副标题
	//	Return Value:
	//	    Null
	//	Remarks:
    //      图片可能不存在任何标题，也可能只有主标题，也可能主副标题均存在。
    //      调用FreeCurrentImageTitle释放占用的内存
	//  Availability:
	//		从ePubLib 2.2.0开始支持。
	//-------------------------------------------
    virtual DK_VOID GetCurrentImageTitle(RichTextString** ppMainTitle, RichTextString** ppSubTitle) const = 0;

    //-------------------------------------------
	//	Summary:
	//		释放当前图片的标题，包括主标题和副标题
	//  Parameters:
	//		[in]pMainTitle          - 主标题
    //      [in]pSubTitle           - 副标题
	//	Return Value:
	//	    Null
	//	Remarks:
    //      Null
	//  Availability:
	//		从ePubLib 2.2.0开始支持。
	//-------------------------------------------
    virtual DK_VOID FreeCurrentImageTitle(RichTextString* pMainTitle, RichTextString* pSubTitle) = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前图片的三级索引
	//  Parameters:
    //      Null
	//	Return Value:
	//	    当前图片的三级索引
	//	Remarks:
    //      Null
	//  Availability:
	//		从ePubLib 2.4.0开始支持。
	//-------------------------------------------
    virtual DK_FLOWPOSITION GetCurrentFlowPosition() const = 0;

public:
    virtual ~IDKEImageIterator() {}
};

//===========================================================================

#endif // #ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEIMAGEITERATOR_H__
