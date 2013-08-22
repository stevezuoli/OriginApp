#ifndef _FILECONTENTSFACTORY_H_
#define _FILECONTENTSFACTORY_H_

#include "Model/IFileContentsModel.h"

class CFileContentsFactory
{
public:
    static HRESULT CreateFileContentsInstance(const char *lpszFileName, IFileContentsModel **ppInstance);
};

#endif // _FILECONTENTSFACTORY_H_
