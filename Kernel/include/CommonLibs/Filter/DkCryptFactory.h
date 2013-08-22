//===========================================================================
// Summary:
//     Crypt的工厂类
// Usage:
//     Null
// Remarks:
//     Null
// Date:
//     2011-11-21
// Author:
//     Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_CONTAINER_DKCRYPTFACTORY_H__
#define __KERNEL_COMMONLIBS_CONTAINER_DKCRYPTFACTORY_H__

#include "DkFilter.h"

class IDkStream;

class DkCryptFactory
{
public:
    static IDkDecode * CreateDecrypter(FilterAlgorithm type);
    static IDkEncode * CreateEncrypter(FilterAlgorithm type);
    static IDkStream * GetDecryptStream(FilterAlgorithm type, IDkStream *pStream, FilterParam filterParam);
    static IDkStream * GetEncryptStream(FilterAlgorithm type, IDkStream *pStream, FilterParam filterParam);
};

#endif // __KERNEL_COMMONLIBS_CONTAINER_DKCRYPTFACTORY_H__
