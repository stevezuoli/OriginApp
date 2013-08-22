#include "ArchiveParser/ArchiveFactory.h"
#include <string.h>
#include "CZipHandler.h"
#include "CRarHandler.h"
#include "Utility.h"

using namespace DkFormat;

HRESULT CArchiveFactory::CreateArchiverInstance(const char *lpszFileName, IArchiverParser **ppInstance)
{
    if(NULL == lpszFileName || NULL == ppInstance)
    {
        return E_INVALIDARG;
    }

    *ppInstance = NULL;

    if(CUtility::StringEndWith((LPCSTR)lpszFileName, ".rar", TRUE))
    {
        *ppInstance = new CRarHandler(lpszFileName);
    }
    else if(CUtility::StringEndWith((LPCSTR)lpszFileName, ".zip", TRUE))
    {
        *ppInstance = new CZipHandler(lpszFileName);
    }
    else
    {
        return E_FAIL;
    }

    if(NULL == *ppInstance)
    {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

