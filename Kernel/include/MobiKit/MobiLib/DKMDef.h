//===========================================================================
// Summary:
//      DKMDef.h
// Usage:
//      Mobi相对于Html的扩展定义，MobiLib的公共头文件
// Remarks:
//      Null
// Date:
//      2012-02-25
// Author:
//      Zhai Guanghe(zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_MOBIKIT_MOBILIB_EXPORT_DKMDEF_H__
#define __KERNEL_MOBIKIT_MOBILIB_EXPORT_DKMDEF_H__

#include "DKEDef.h" // 暂时引用ePub的定义，以后改为Html的
#include "DKMRetCode.h"

// MOBI文件的基本信息 // TODO 根据Mobi调整
struct DKMBookInfo
{
    DK_WCHAR* pTitle;
    DK_WCHAR* pAuthor;
    DK_WCHAR* pSubject;
    DK_WCHAR* pPublisher;
    DK_WCHAR* pDescription;
    DK_WCHAR* pSource;
    DK_WCHAR* pDate;
    DK_WCHAR* pRights;
    DK_WCHAR* pContributor;

    DKMBookInfo()
        : pTitle(DK_NULL)
        , pAuthor(DK_NULL)
        , pSubject(DK_NULL)
        , pPublisher(DK_NULL)
        , pDescription(DK_NULL)
        , pSource(DK_NULL)
        , pDate(DK_NULL)
        , pRights(DK_NULL)
        , pContributor(DK_NULL)
    {
        // do nothing
    }
};

struct DKM_CHAR_INFO
{
    DK_BOX   boundingBox; // 外接矩形
    DK_WCHAR charCode;    // 文字的 Unicode 编码
    DK_UINT  offset;      // 文字的索引位置
};

#define MOBIAPI        extern "C" EPUB_API DK_BOOL
#define MOBIAPI_(type) extern "C" EPUB_API type

#ifndef MOBIDOCHANDLE // 暂时会有redefine的问题
#define MOBIDOCHANDLE  DK_VOID *
#endif // MOBIDOCHANDLE

#endif // __KERNEL_MOBIKIT_MOBILIB_EXPORT_DKMDEF_H__
