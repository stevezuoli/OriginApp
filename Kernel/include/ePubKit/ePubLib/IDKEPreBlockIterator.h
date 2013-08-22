//===========================================================================
// Summary:
//		页面Pre块对象迭代器接口。
// Usage:
//		枚举当前页面中所有Pre块枚举器
// Remarks:
//		Null
// Date:
//		2013-07-17
// Author:
//		Zhang Jiafang (zhangjf@duokan.com)
//===========================================================================


#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEPREBLOCKITERATOR_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEPREBLOCKITERATOR_H__

//===========================================================================

#include "DKEDef.h"

//===========================================================================

class IDKEPreBlockIterator
{
public:
    virtual ~IDKEPreBlockIterator() {}

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
	//		从ePubLib 2.7.0开始支持。
	//-------------------------------------------
    virtual DK_BOOL MoveToNext() = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前Pre块的信息
	//  Parameters:
	//		[out] pPreBlockInfo         - 当前Pre块信息
	//	Return Value:
	//		Null
	//	Remarks:
	//		调用FreeCurPreBlockInfo释放资源
	//  Availability:
	//		从ePubLib 2.7.0开始支持。
	//-------------------------------------------
    virtual DK_VOID GetCurPreBlockInfo(DKE_PREBLOCK_INFO* pPreBlockInfo) const = 0;

    //-------------------------------------------
	//	Summary:
	//		释放当前Pre块的信息
	//  Parameters:
	//		[in] pPreBlockInfo           - 当前Pre块信息
	//	Return Value:
	//		Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.7.0开始支持。
	//-------------------------------------------
    virtual DK_VOID FreeCurPreBlockInfo(DKE_PREBLOCK_INFO* pPreBlockInfo) = 0;

    //-------------------------------------------
	//	Summary:
	//		获取当前页面上的Pre块的流式坐标区间
	//  Parameters:
	//		[in] pStartPos              - 当前Pre块的起始坐标
    //      [in] pEndPos                - 当前Pre块的结束坐标
	//	Return Value:
	//		Null
	//	Remarks:
	//		Null
	//  Availability:
	//		从ePubLib 2.7.0开始支持。
	//-------------------------------------------
    virtual DK_VOID GetFlowPositionRange(DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos) const = 0;
};


#endif // __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEPREBLOCKITERATOR_H__

