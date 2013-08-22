//===========================================================================
// Summary:
//		页面脚注对象迭代器接口。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2012-12-12
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEFOOTNOTEITERATOR_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEFOOTNOTEITERATOR_H__

//===========================================================================

#include "DKEDef.h"

//===========================================================================

class IDKEFootnoteIterator
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
	//		获取当前位置的脚注信息。
	//  Parameters:
	//		[out] pFnInfo            - 当前位置脚注信息。
	//	Return Value:
	//	    Null
	//	Remarks:
	//		按照可伸缩页的展现方式（DKE_SHOWCONTENT_FLEXPAGE）处理
	//  Availability:
	//		从ePubLib 1.0开始支持。
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
	//		从ePubLib 1.0开始支持。
	//-------------------------------------------
    virtual DK_VOID FreeFootnoteInfo(DKE_FOOTNOTE_INFO* pFnInfo) = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前脚注的三级索引
	//  Parameters:
	//		Null
	//	Return Value:
	//	    当前脚注的三级索引
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.4.0开始支持
	//-------------------------------------------
    virtual DK_FLOWPOSITION GetCurrentFlowPosition() const = 0;

public:
    virtual ~IDKEFootnoteIterator() {}
};

//===========================================================================

#endif // #ifndef __IDKEFOOTNOTEITERATOR_HEADERFILE__
