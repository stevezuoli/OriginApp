//===========================================================================
//  Summary:
//      页面文字对象迭代器接口。
//  Usage:
//      Null
//  Remarks:
//      Null
//  Date:
//      2011-12-09
// Author:
//      Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef __KERNEL_MOBIKIT_MOBILIB_EXPORT_IDKMTEXTITERATOR_H__
#define __KERNEL_MOBIKIT_MOBILIB_EXPORT_IDKMTEXTITERATOR_H__

#include "DKMDef.h"

class IDKMTextIterator
{
public:
    //-------------------------------------------
    //  Summary:
    //      移动到下一位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      如果返回 DK_FALSE，则不应该从迭代器中继续获取数据。
    //  Remarks:
    //      Null
    //  Availability:
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_BOOL MoveToNext() = 0;

    //-------------------------------------------
    //  Summary:
    //      移动到前一位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      如果返回 DK_FALSE，则不应该从迭代器中继续获取数据。
    //  Remarks:
    //      Null
    //  Availability:
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_BOOL MoveToPrev() = 0;

    //-------------------------------------------
    //  Summary:
    //      移动到上一行的相邻位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      如果返回 DK_FALSE，则不应该从迭代器中继续获取数据。
    //  Remarks:
    //      Null
    //  Availability:
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_BOOL MoveToPrevAdjacentLine() = 0;

    //-------------------------------------------
    //  Summary:
    //      移动到下一行的相邻位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      如果返回 DK_FALSE，则不应该从迭代器中继续获取数据。
    //  Remarks:
    //      Null
    //  Availability:
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_BOOL MoveToNextAdjacentLine() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取当前位置的字符信息。
    //  Parameters:
    //      [out] pCharInfo          - 当前位置字符信息。
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从MobiLib 1.0.8开始支持。
    //-------------------------------------------
    virtual DK_VOID GetCurrentCharInfo(DKM_CHAR_INFO *pCharInfo) const = 0;

public:
    virtual ~IDKMTextIterator() {}
};

#endif // __KERNEL_MOBIKIT_MOBILIB_EXPORT_IDKMTEXTITERATOR_H__
