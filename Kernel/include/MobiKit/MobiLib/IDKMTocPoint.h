//===========================================================================
//  Summary:
//      ePub图书目录接口。
//  Usage:
//      Null
//  Remarks:
//      Null
//  Date:
//      2011-09-16
//  Author:
//      Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_MOBIKIT_MOBILIB_EXPORT_IDKMTOCPOINT_H__
#define __KERNEL_MOBIKIT_MOBILIB_EXPORT_IDKMTOCPOINT_H__

#include "KernelBaseType.h"

class IDKMTocPoint
{
public:
    //-------------------------------------------
    //  Summary:
    //      获取目录节点文字内容。
    //  Parameters:
    //      Null
    //  Return Value:
    //      目录节点文字内容。
    //  Remarks:
    //      Null
    //  Availability:
    //      从MobiLib 1.0.9开始支持。
    //-------------------------------------------
    virtual const DK_WCHAR * GetTitle() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取跳转目标位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      目标位置
    //  Remarks:
    //      Null
    //  Availability:
    //      从MobiLib 1.0.9开始支持。
    //-------------------------------------------
    virtual DK_UINT GetDestPosition() const = 0;

public:
    //-------------------------------------------
    //  Summary:
    //      获取子节点个数。
    //  Parameters:
    //      Null
    //  Return Value:
    //      子节点个数。
    //  Remarks:
    //      Null
    //  Availability:
    //      从MobiLib 1.0.9开始支持。
    //-------------------------------------------
    virtual DK_UINT GetChildCount() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取子节点。
    //  Parameters:
    //      [in] index              - 子节点索引，从0开始。
    //  Return Value:
    //      子节点对象接口。
    //  Remarks:
    //      Null
    //  Availability:
    //      从MobiLib 1.0.9开始支持。
    //-------------------------------------------
    virtual const IDKMTocPoint * GetChildByIndex(DK_UINT index) const = 0;

public:
    virtual ~IDKMTocPoint() {}
};

#endif // __KERNEL_MOBIKIT_MOBILIB_EXPORT_IDKMTOCPOINT_H__
