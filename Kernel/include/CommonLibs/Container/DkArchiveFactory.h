//===========================================================================
// Summary:
//     Archive的工厂类
// Usage:
//     Null
// Remarks:
//     Null
// Date:
//     2011-09-19
// Author:
//     Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_CONTAINER_DKARCHIVEFACTORY_H__
#define __KERNEL_COMMONLIBS_CONTAINER_DKARCHIVEFACTORY_H__

#include "DkArchive.h"

class UnicodeString;

class DkArchiveFactory
{
public:
    static IDkArchive * CreateInstance(IDkArchive::TYPE type, const DK_CHAR *pFilePathName);
    static IDkArchive * CreateInstance(IDkArchive::TYPE type, const UnicodeString &filePathName);
    // 当从一个流创建压缩包时，该流会在压缩包析构时释放掉，如果不希望被释放，应传入pStream->CreateCopy()
    static IDkArchive * CreateInstance(IDkArchive::TYPE type, IDkStream *pStream);
};

#endif // __KERNEL_COMMONLIBS_CONTAINER_DKARCHIVEFACTORY_H__
