///////////////////////////////////////////////////////////////////////
// FileContentsFactory.cpp
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "Common/FileContentsFactory.h"
#include "Model/ArchiveFileContentsImpl.h"
#include "../Common/FileManager_DK.h"
#include "interface.h"
#include "dkWinError.h"

HRESULT CFileContentsFactory::CreateFileContentsInstance(const char *lpszFileName, IFileContentsModel **ppInstance)
{
    if(NULL == lpszFileName)
    {
        return S_FALSE;
    }
    DebugPrintf(DLC_LIUJT, "CFileContentsFactory::CreateFileContentsInstance filename=%s", lpszFileName);
//TODO:区分不同格式文件
    IFileContentsModel* pNewContents = NULL;
    
    DkFileFormat FileFormat = GetFileFormatbyExtName(lpszFileName);
    switch(FileFormat)
    {
        case DFF_ZipFile:
        case DFF_RoshalArchive:
            {
                pNewContents = new ArchiveFileContentsImpl(lpszFileName);
                DebugPrintf(DLC_LIUJT, "CFileContentsFactory::CreateFileContentsInstance ZipFile pNewContents=%d", (pNewContents!=NULL));
                if(pNewContents)
                {
                    *ppInstance = pNewContents;
                    return S_OK;
                }        
            }
            break;
        default:
            return S_FALSE;
    }

    return S_FALSE;
}
