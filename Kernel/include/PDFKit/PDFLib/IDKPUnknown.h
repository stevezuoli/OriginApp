//===========================================================================
// Summary:
//		IDKPUnknown类
// Usage:
//		所有DKP接口类的基类，定义了QueryInterface接口，查询对象支持的接口。
//		用法1：
//			DKP中的某些类按照PDF规范，在逻辑上有继承关系，但是在接口定义上没有继承。
//			对这类接口，需要通过QueryInterface在逻辑上的父类和子类之间转换。
//		用法2：
//			在对应的Obj、Edit、View层对象之间转换。
// Remarks:
//		两个不同类型的DKP接口指针不能直接比较，需要查询出同样类型的指针值进行比较。
//		特别地，提供DKP_IID_NONE枚举值，所有接口指针都可以查询出一个DKP_IID_NONE对应的指针值。
// Date:
//		2011-9-15
// Author:
//		Zhang JingDan (zhangjingdan@duokan.com)
//===========================================================================

#ifndef	__KERNEL_PDFKIT_PDFLIB_IDKPUNKNOWN_H__
#define __KERNEL_PDFKIT_PDFLIB_IDKPUNKNOWN_H__

//===========================================================================

#include "KernelDef.h"

//===========================================================================

// DKP支持的接口类型Interface ID，用于QueryInterface
enum DKP_IID
{
	DKP_IID_NONE,			// 不表示任何类型，查询出的指针值可用于比较两个接口指针是否是同一个对象

	DKP_IID_IDKPDoc,
	DKP_IID_IDKPPage,
	DKP_IID_IDKPPageEx,
	DKP_IID_IDKPOutline,
	DKP_IID_IDKPPageDistiller,
	DKP_IID_IDKPImageEnumerator,
	
	DKP_IID_IDKPModifyDoc,
	DKP_IID_IDKPProcDoc,
	DKP_IID_IDKPWaterMarkDoc,

	DKP_IID_IDKPAnnotation,
	DKP_IID_IDKPAnnotList,
	DKP_IID_IDKPMarkupAnnot,
	DKP_IID_IDKPPopupAnnot,
	DKP_IID_IDKPTextAnnot,
	DKP_IID_IDKPLinkAnnot,
	DKP_IID_IDKPFreeTextAnnot,
	DKP_IID_IDKPTextMarkupAnnot,
	DKP_IID_IDKPCaretAnnot,
	DKP_IID_IDKPStampAnnot,
	DKP_IID_IDKPInkAnnot,
	DKP_IID_IDKPFileAttAnnot,
	DKP_IID_IDKPSoundAnnot,
	DKP_IID_IDKPMovieAnnot,
	DKP_IID_IDKPScreenAnnot,
	DKP_IID_IDKPWatermarkAnnot,
	DKP_IID_IDKPPolyAnnot,
	DKP_IID_IDKPRectAnnot,
	DKP_IID_IDKPLineAnnot,

	DKP_IID_IDKPAnnotationEditor,
	DKP_IID_IDKPAnnotListEditor,
	DKP_IID_IDKPMarkupAnnotEditor,
	DKP_IID_IDKPPopupAnnotEditor,
	DKP_IID_IDKPTextAnnotEditor,
	DKP_IID_IDKPLinkAnnotEditor,
	DKP_IID_IDKPFreeTextAnnotEditor,
	DKP_IID_IDKPTextMarkupAnnotEditor,
	DKP_IID_IDKPCaretAnnotEditor,
	DKP_IID_IDKPStampAnnotEditor,
	DKP_IID_IDKPInkAnnotEditor,
	DKP_IID_IDKPFileAttAnnotEditor,
	DKP_IID_IDKPSoundAnnotEditor,
	DKP_IID_IDKPWatermarkAnnotEditor,
	DKP_IID_IDKPPolyAnnotEditor,
	DKP_IID_IDKPRectAnnotEditor,
	DKP_IID_IDKPLineAnnotEditor
};

class IDKPUnknown
{
public:
	virtual ~IDKPUnknown() {}

public:
	//---------------------------------------------------------------------------
	//	Summary:
	//		查询对象是否支持指定的接口，并返回指定接口的指针。
	//	Parameters:
	//	    [in] iid - 需要的接口类型。
	//	Remarks:
	//		查询成功返回所需的对象指针，可直接转型成所需类型；失败则返回DK_NULL。
	//  Availability:
	//		从PDF 1.0开始支持。
	//---------------------------------------------------------------------------
	virtual DK_VOID* QueryInterface(DKP_IID iid) = 0;
};

//===========================================================================

#endif // #ifndef	__IDKPUNKNOWN_H__
