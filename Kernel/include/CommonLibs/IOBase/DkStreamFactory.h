//===========================================================================
// Summary:
//     Stream的工厂类
// Usage:
//     Null
// Remarks:
//     Null
// Date:
//     2011-09-21
// Author:
//     Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_CONTAINER_DKSTREAMFACTORY_H__
#define __KERNEL_COMMONLIBS_CONTAINER_DKSTREAMFACTORY_H__

#include "DkStream.h"

class UnicodeString;

class DkStreamFactory
{
public:
    static IDkStream * GetFileStream(const DK_CHAR *pFilePathName);
    static IDkStream * GetFileStream(const UnicodeString *pFilePathName);
    static IDkStream * GetFileStream(const DK_WCHAR *pWideFilePathName);
    static IDkStream * GetMemoryStream(DK_VOID *pBuffer, DK_FILESIZE_T bufSize, DK_FILESIZE_T dataLen);
    static IDkStream * GetAutoMemStream();
    static IDkStream * GetSubStream(IDkStream *pBaseStream, DK_FILESIZE_T baseOff, DK_FILESIZE_T baseSize, IDkStream::ReleseMethod method = IDkStream::RELESE_INSIDE);
};

#endif // __KERNEL_COMMONLIBS_CONTAINER_DKSTREAMFACTORY_H__
