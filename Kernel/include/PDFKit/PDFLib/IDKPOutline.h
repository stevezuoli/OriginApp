//===========================================================================
// Summary:
//      IDKPOutline.h
//      负责文档目录提取的接口类。
// Usage:
//      Null
// Remarks:
//      Null
// Date:
//      2011-10-24
// Author:
//      Liu Hongjia(liuhj@duokan.com)
//===========================================================================
#ifndef __KERNEL_PDFKIT_PDFLIB_IDKPOUTLINE_H__
#define __KERNEL_PDFKIT_PDFLIB_IDKPOUTLINE_H__

//===========================================================================

#include "ActionTypeDef.h"
#include "DKPTypeDef.h"

//===========================================================================

class IDKPOutline
{
public:
    //-------------------------------------------
    // Summary:
    //    获得打开的目录项数量。
    // Returns:
    //    成功则返回打开目录项数量，失败则返回-1。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_INT GetOutlineOpenCount() = 0;

    //-------------------------------------------
    // Summary:
    //    获得指定序号的目录项内容。
    // Parameters:
    //    [in] itemIndex    - 目录项序号，从1开始。
    //    [out] pItemInfo   - 目录项内容。
    // Returns:
    //    成功则返回DK_TRUE，失败则返回DK_FALSE。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_BOOL GetOutlineItemByIndex(DK_INT itemIndex, DKPOUTLINEITEMINFO* pItemInfo) = 0;

    //-------------------------------------------
    // Summary:
    //    获取目录项的Action的数量。
    // Parameters:
    //    [in] itemIndex - 目录项序号，从1开始。
    // Returns:
    //    返回Action的数量。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_INT GetOutlineItemActionCount(DK_INT itemIndex) = 0;

    //-------------------------------------------
    // Summary:
    //    获取动作的内容。
    // Parameters:
    //    [in] itemIndex    - 目录项序号，从1开始。
    //    [in] actionIndex  - Action的序号。
    //    [out] actionType  - 目录动作的类型。
    //    [out] pActionInfo - 动作的内容。
    // Returns:
    //    成功则返回DK_TRUE,失败返回DK_FALSE。
    // Remarks:
    //    该接口调用两次，第一次pActionInfo为DK_NULL,获取actionType，
    //    第二次根据actionType获取pActionInfo的内容。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_BOOL GetOutlineItemAction(DK_INT itemIndex, DK_INT actionIndex, DK_ACTION_TYPE& actionType, DK_VOID* pActionInfo) = 0;

    //-------------------------------------------
    // Summary:
    //    获取目录项指向的页码。
    // Parameters:
    //    [in] itemIndex - 目录项序号，从1开始。
    // Returns:
    //    成功则返回页码，失败则返回-1。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_INT GetOutlineDestPageNum(DK_INT itemIndex) = 0;

    //-------------------------------------------
    // Summary:
    //    获取指向指定页码的第一个目录项序号。
    // Parameters:
    //    [in] pageNum - 页码，从1开始。
    // Returns:
    //    成功则返回目录项序号，失败则返回-1。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_INT GetOutlineIndexByPageNum(DK_INT pageNum) = 0;

    //-------------------------------------------
    // Summary:
    //    获取目录项。
    // Parameters:
    //    [in] itemIndex    - 目录项索引。
    //    [out] pItemInfo   - 目录项内容。
    //    [in] queryType    - 查询类型。
    // Return:
    //    查到返回DK_TURE,失败返回DK_FALSE。
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_BOOL GetOutlineNextItemInfo(const DK_WORD itemIndex, DKPOUTLINEITEMINFO* pItemInfo, DKP_TREEQUERY_TYPE queryType) = 0;

    //-------------------------------------------
    // Summary:
    //    释放目录
    // SDK Version:
    //    从DKP 1.0.0开始支持
    //-------------------------------------------
    virtual DK_INT Release() = 0;

public:
    virtual ~IDKPOutline() {}
};

//===========================================================================

#endif
