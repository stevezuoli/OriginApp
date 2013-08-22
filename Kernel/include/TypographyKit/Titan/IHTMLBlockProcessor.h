//===========================================================================
// Summary:
//		基于HTML流结构的块排版处理器接口，主要用于EPUB渲染。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-09-13
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef	__HTMLBLOCKPROCESSOR_INTERFACE_HEADERFILE__
#define __HTMLBLOCKPROCESSOR_INTERFACE_HEADERFILE__

//===========================================================================

#include "ITitanPart.h"

//===========================================================================

// 内容块读取的回调接口，如果到达内容流末尾返回false，否则返回true。
typedef bool (*PFNREADHTMLCONTENTPROC)(TP_HANDLE hContentProvider, TP_CONTENTPIECE& piece);

//===========================================================================

class ILayoutBoundary;

class IHTMLBlockProcessor : public ITitanPart
{
public:
    //-------------------------------------------
    //  Summary:
    //      设置排版区域
    //  Parameters:
    //      [in] pLayoutBoundary    - 排版区域
    //  Return Value:
    //  Availability:
    //-------------------------------------------
    virtual void SetBoundary(const ILayoutBoundary *pLayoutBoundary) = 0;

	//-------------------------------------------
	//	Summary:
	//		排入内容。
	//  Parameters:
	//		Null
	//	Return Value:
	//		内容流全部排入成功返回TP_LAYOUT_SUCCEED；
	//		部分排入还有后续返回TP_LAYOUT_FOLLOWAFTER；
	//		排入失败则返回TP_LAYOUT_FAIL。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual TP_LAYOUT_STATE ProcessContent() = 0;

	//-------------------------------------------
	//	Summary:
	//		排入内容。
	//  Parameters:
	//		[in] isp			- 初始状态参数，可以指定当前块起始位置是否是新段落。
	//		[in] bFastMode		- 高速模式，该模式下不能枚举排版结果。
	//	Return Value:
	//		内容流全部排入成功返回TP_LAYOUT_SUCCEED；
	//		部分排入还有后续返回TP_LAYOUT_FOLLOWAFTER；
	//		排入失败则返回TP_LAYOUT_FAIL。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual TP_LAYOUT_STATE ProcessContentEx(const TP_INITIALSTATEPARA& isp, bool bFastMode = false) = 0;

	//-------------------------------------------
	//	Summary:
	//		排入内容，不强制换行，不支持绕排，枚举器中的结果是宽度和高度无限大的时候的结果。
	//  Parameters:
	//		[in] isp			- 初始状态参数，可以指定当前块起始位置是否是新段落。
	//		[out] dMinLineLen	- 最小行宽度。
	//		[out] dMaxLineLine	- 最大行宽度。
	//	Return Value:
	//		内容流全部排入成功返回TP_LAYOUT_SUCCEED；
	//		部分排入还有后续返回TP_LAYOUT_FOLLOWAFTER；
	//		排入失败则返回TP_LAYOUT_FAIL。
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual TP_LAYOUT_STATE ProcessContentNotForcedBreakLine(const TP_INITIALSTATEPARA& isp, double& dMinLineLen, double& dMaxLineLen) = 0;

    //-------------------------------------------
	//	Summary:
	//		设置读取内容流数据的回调接口。
	//  Parameters:
	//		[in] hContentProvider			- 内容流提供对象句柄。
	//		[in] pfnReadHTMLContentProc     - 读取内容流数据的回调函数入口。
    //  Return Value:
    //      Null
	//  Availability:
	//		从Titan 1.0开始支持。
	//-------------------------------------------
	virtual void SetReadHTMLContentProc(TP_HANDLE hContentProvider, PFNREADHTMLCONTENTPROC pfnReadHTMLContentProc) = 0;

    //-------------------------------------------
    //	Summary:
    //		获取当前排版高度，快排可用。
    //  Parameters:
    //      Null
    //  Return Value:
    //      当前排版高度。
    //  Availability:
    //		从Titan 1.0开始支持。
    //-------------------------------------------
    virtual double GetLayoutHeight() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取当前排版结果最后一行的高度，快排可用。
    //  Parameters:
    //      Null
    //  Return Value:
    //      最后一行的高度。
    //  Availability:
    //      从Titan 1.0开始支持。
    //-------------------------------------------
    virtual double GetLastLineHeight() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取当前排版结果最后一行的行间距，快排可用。
    //  Parameters:
    //      Null
    //  Return Value:
    //      最后一行的行间距。
    //  Availability:
    //      从Titan 1.0开始支持。
    //-------------------------------------------
    virtual double GetLastLineGap() = 0;

    //-------------------------------------------
    //	Summary:
    //		是否至少排一行，即当排版区域高度不足以排下一行时，允许排下一行。
    //  Parameters:
    //		[bool] bAtLeastProcessOneLine - 是否至少排一行。
    //-------------------------------------------
    virtual void SetAtLeastProcessOneLine(bool bAtLeastProcessOneLine) = 0;

    //-------------------------------------------
    //	Summary:
    //		设置最大出血长度，即超出排版区域的长度。
    //		默认是-1，当值为负数则不考虑最大出血长度的限制。
    //		流式块在绕排时候设置这个变量防止绕排字符串超出了流式块排版区域的下边界。
    //      在绕排排版结束后需要重新调用这个接口赋值为默认值。
    //  Parameters:
    //		[double] dMaxBleedLength	- 超出排版区域的长度。
    //-------------------------------------------
    virtual void SetMaxBleedLength(double dMaxBleedLength) = 0;

    //-------------------------------------------
    //	Summary:
    //		设置强制出血模式，内部ePublib使用。
    //  Parameters:
    //		[bool] bForceBleed	- 是否强制出血。
    //-------------------------------------------
    virtual void SetForceBleed(bool bForceBleed) = 0;
};

//===========================================================================

#endif	//#ifndef __HTMLBLOCKPROCESSOR_INTERFACE_HEADERFILE__
