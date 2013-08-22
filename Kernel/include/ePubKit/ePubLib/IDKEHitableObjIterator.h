//===========================================================================
// Summary:
//		页面可点击对象的迭代器
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2013-05-21
// Author:
//		Zhang Jiafang(zhangjf@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEHITABLEOBJITERATOR_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEHITABLEOBJITERATOR_H__

//===========================================================================

#include "DKEDef.h"

//===========================================================================

class IDKEGallery;

class IDKEHitableObjIterator
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
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL MoveToNext() = 0;

	//-------------------------------------------
	//	Summary:
	//		移动到前一位置。
	//  Parameters:
	//		Null
	//	Return Value:
	//		如果返回 DK_FALSE，则不应该从迭代器中继续获取数据。
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL MoveToPrev() = 0;

    //-------------------------------------------
	//	Summary:
	//		移动到上一行的相邻位置。
	//  Parameters:
	//		Null
	//	Return Value:
	//		如果返回 DK_FALSE，则不应该从迭代器中继续获取数据。
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL MoveToPrevAdjacentLine() = 0;

    //-------------------------------------------
	//	Summary:
	//		移动到下一行的相邻位置。
	//  Parameters:
	//		Null
	//	Return Value:
	//		如果返回 DK_FALSE，则不应该从迭代器中继续获取数据。
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL MoveToNextAdjacentLine() = 0;

    //-------------------------------------------
	//	Summary:
	//		设置迭代器到最后一个位置
	//  Parameters:
	//		Null
	//	Return Value:
	//		如果返回 DK_FALSE 则移动失败
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
    virtual DK_BOOL MoveToLast() = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前所在页面对象的类型
	//  Parameters:
	//		Null
	//	Return Value:
	//		页面对象类型
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
    virtual DKE_PAGEOBJ_TYPE GetCurrentObjType() const = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前所在页面对象的流式坐标
	//  Parameters:
	//		Null
	//	Return Value:
	//		当前页面的流式坐标
	//	Remarks:
	//		画廊时返回画廊的开始坐标
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
    virtual DK_FLOWPOSITION GetCurrentFlowPosition() const = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前所在页面对象的外接矩形
	//  Parameters:
	//		Null
	//	Return Value:
	//		当前页面对象的外接矩形
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
    virtual DK_BOX GetCurrentBoundingBox() const = 0;

	//-------------------------------------------
	//	Summary:
	//		获取当前位置的字符信息。
	//  Parameters:
	//		[out] pCharInfo          - 当前位置字符信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		当前的页面对象类型为DKE_PAGEOBJ_TEXT时有效
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
	virtual DK_VOID GetCurrentCharInfo(DKE_CHAR_INFO* pCharInfo) const = 0;

    //-------------------------------------------
	//	Summary:
	//		判断当前位置的图片是否是随文富媒体对象
	//  Parameters:
	//		Null
	//	Return Value:
	//	    Null
	//	Remarks:
	//		当前的页面对象类型为DKE_PAGEOBJ_IMAGE或DKE_PAGEOBJ_AUDIO时有效
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
    virtual DK_BOOL IsCurrentFollowRichMedia() const = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前位置的富媒体对象信息
	//  Parameters:
	//		[out] pObjInfo          - 当前位置富媒体信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		当前的页面对象类型为图片（包括普通图，静态图，交互图），音频，视频时有效；调用FreeCurrentRichMediaInfo释放
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
    virtual DK_VOID GetCurrentRichMediaInfo(DKE_HITTEST_OBJECTINFO* pObjInfo) const = 0;

    //-------------------------------------------
	//	Summary:
	//		释放当前位置的富媒体对象信息
	//  Parameters:
	//		[out] pObjInfo          - 当前位置富媒体信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		null
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
    virtual DK_VOID FreeRichMediaInfo(DKE_HITTEST_OBJECTINFO* pObjInfo) = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前位置的脚注信息。
	//  Parameters:
	//		[out] pFnInfo            - 当前位置脚注信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		当前页面对象为DKE_PAGEOBJ_FOOTNOTE时有效，调用FreeFootnoteInfo释放资源
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID GetCurrentFootnoteInfo(DKE_FOOTNOTE_INFO* pFnInfo) const = 0;

    //-------------------------------------------
	//	Summary:
	//		释放脚注信息。
	//  Parameters:
	//		[in] pFnInfo             - 待释放的脚注信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
    virtual DK_VOID FreeFootnoteInfo(DKE_FOOTNOTE_INFO* pFnInfo) = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前位置的画廊对象信息
	//  Parameters:
	//		[out] ppGallery          - 当前画廊信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		当前的页面对象为DKE_PAGEOBJ_GALLERY时有效，调用FreeGalleryInfo释放资源
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
    virtual DK_VOID GetCurrentGalleryInfo(IDKEGallery** ppGallery) const = 0;

    //-------------------------------------------
	//	Summary:
	//		释放当前位置的画廊对象信息
	//  Parameters:
	//		[out] pGallery          - 当前位置画廊信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.5.0开始支持。
	//-------------------------------------------
    virtual DK_VOID FreeGalleryInfo(IDKEGallery* pGallery) = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前位置的PRE块对象信息
	//  Parameters:
	//		[out] pPreBlockInfo         - 当前PRE块信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		当前的页面对象为DKE_PAGEOBJ_PREBLOCK时有效，调用FreeGalleryInfo释放资源
	//  Availability:
	//		从ePubLib 2.7.0开始支持。
	//-------------------------------------------
    virtual DK_VOID GetCurrentPreBlockInfo(DKE_PREBLOCK_INFO* pPreBlockInfo) const = 0;

    //-------------------------------------------
	//	Summary:
	//		释放当前位置的画廊对象信息
	//  Parameters:
	//		[out] pPreBlockInfo          - 当前PRE块信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.7.0开始支持。
	//-------------------------------------------
    virtual DK_VOID FreePreBlockInfo(DKE_PREBLOCK_INFO* pPreBlockInfo) = 0;

public:
    virtual ~IDKEHitableObjIterator() {}
};

//===========================================================================

#endif // __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEHITABLEOBJITERATOR_H__
