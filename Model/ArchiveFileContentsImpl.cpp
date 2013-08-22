///////////////////////////////////////////////////////////////////////
// ArchiveFileContentsImpl.cpp
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "Model/ArchiveFileContentsImpl.h"
#include "../Common/FileManager_DK.h"
#include "ArchiveParser/ArchiveFactory.h"
#include "Utility.h"
#include <string>
#include <algorithm>

using DkFormat::CArchiveFactory;

ArchiveFileContentsImpl::ArchiveFileContentsImpl(LPCSTR pFilePath)
                         :m_StrFilePath(pFilePath)
                         ,m_rgPassword()
                         ,m_iChapterNum(0)
                         ,m_bPathHasPwd(FALSE)
                         ,m_bChapterHasPwd(FALSE)

{
    Init();
}

ArchiveFileContentsImpl::~ArchiveFileContentsImpl()
{

}

void ArchiveFileContentsImpl::Init()
{
    IArchiverParser *pIArchiverParser = NULL;
    ArchiveEncryption ePwdType = ARCHIVE_NO_ENCRYPTION;
    CArchiveFactory::CreateArchiverInstance(m_StrFilePath.c_str(),&pIArchiverParser);
    if(NULL == pIArchiverParser)
    {
        return;
    }

    pIArchiverParser->IsEncryption(&ePwdType);
    switch(ePwdType)
    {
    case ARCHIVE_NAME_ENCRYPTION:
        m_bPathHasPwd = true;
        break;
    case ARCHIVE_CONTENT_ENCRYPTION:
        m_bChapterHasPwd = true;
        break;
    default:
        break;
    }

    delete pIArchiverParser;
    pIArchiverParser = NULL;

 }

BOOL ArchiveFileContentsImpl::HasPassword()
{
    return m_bPathHasPwd || m_bChapterHasPwd;
}

INT32 ArchiveFileContentsImpl::GetChapterNum()
{
    return m_iChapterNum;
}

void ArchiveFileContentsImpl::SetPassword(LPCSTR StrPwd)
{
    if(StrPwd)
    {
        strncpy(m_rgPassword,StrPwd,sizeof(m_rgPassword) - 1);
    }
}

BOOL ArchiveFileContentsImpl::GetChapterList(vector <CFileChapterInfo> & ChapterList)
{
    IArchiverParser *pIArchiverParser = NULL;
    if(CArchiveFactory::CreateArchiverInstance(m_StrFilePath.c_str(),&pIArchiverParser) != S_OK)
    {
        return FALSE;
    }

    if(NULL == pIArchiverParser)
    {
        return FALSE;
    }

    if(0 != m_rgPassword[0])
    {
        pIArchiverParser->SetPassword(m_rgPassword);
    }

    int iListCount = 0;
    bool bIsfile = false;
    char *pName = NULL;
    if(S_OK != pIArchiverParser->GetFileNum(&iListCount))
    {
        delete pIArchiverParser;
        return FALSE;
    }

    for(int i=0; i < iListCount; i++)
    {
        bIsfile = false;
        pIArchiverParser->CheckIsFileByIndex(i,&bIsfile);

        if(!bIsfile)
        {
            continue;
        }

        if(pIArchiverParser->GetFileNameByIndex(i,&pName) == S_OK)
        {
            if(pName && (GetFileFormatbyExtNameForZipFile(pName)!= DFF_Unknown))
            {
                 AppendChapter(ChapterList,pName,i);
            }
        }

        if(pName)
        {
            pIArchiverParser->ReleaseFileName(pName);
            pName = NULL;
        }
    }

    delete pIArchiverParser;
    pIArchiverParser = NULL;
    std::sort(ChapterList.begin(),ChapterList.end(),SortChapterByName);
    return TRUE;
}


void  ArchiveFileContentsImpl::AppendChapter(vector <CFileChapterInfo> & ChapterList,LPSTR pTitle,INT32 index, BOOL bIsDir)
{
    if(NULL == pTitle)
    {
        return;
    }
    //    TODO:¹ýÂË¶à²ãÑ¹Ëõ
    std::string filename(pTitle);
    std::string ext = filename.substr(filename.rfind('.') == std::string::npos ? filename.length() : filename.rfind('.'));

    char   gbkname[FILENAMEMAXLEN]={0};
    strncpy(gbkname,pTitle,FILENAMEMAXLEN - 1);
    CFileChapterInfo NewChapter;
    if(bIsDir)
    {
        char *pchar  = strrchr(gbkname,'/');
        if(pchar)
        {
            pchar++;
            NewChapter.SetChapterName(pchar);
        }
        else
        {
            NewChapter.SetChapterName(gbkname);
        }
    }
    else
    {

        char *pchar = strrchr(gbkname,'.');
        if(pchar)
        {
            *pchar = '\0';
        }
        pchar = strrchr(gbkname,'/');
        if(pchar)
        {
            pchar++;
            NewChapter.SetChapterName(pchar);
        }
        else
        {
            NewChapter.SetChapterName(gbkname);
        }
        NewChapter.SetChapterExName(ext.c_str());
    }
    NewChapter.SetChapterPath(pTitle);
    NewChapter.SetChapterId(index);
    NewChapter.SetChapterDirAttr(bIsDir);
    NewChapter.SetChapterHasPwdAttr(m_bChapterHasPwd);
    ChapterList.push_back(NewChapter);
    return;
}


BOOL ArchiveFileContentsImpl::SortChapterByName(CFileChapterInfo src,CFileChapterInfo dest)
{
    LPCSTR     p = src.GetChapterName();
    LPCSTR    q = dest.GetChapterName();
    if(NULL == p || NULL == q)
    {
        return false;
    }

    return (SortString(p,q) < 0 );
}

int ArchiveFileContentsImpl::SortString(LPCSTR str,LPCSTR dest)
{
    if(NULL == str || NULL == dest)
    {
        return 0;
    }
    LPCSTR pChar = str;
    LPCSTR qChar = dest;

    BOOL fIgnoreCmp = FALSE;
    while(*pChar && *qChar)
    {
        if(*pChar >= '0' && *pChar <= '9' && *qChar >= '0' && *qChar <= '9')
        {
            if (!fIgnoreCmp)
            {
                if(atoi(pChar) < atoi(qChar))
                {
                    return -1;
                }
                else if(atoi(pChar) > atoi(qChar))
                {
                    return 1;
                }
                else
                {
                    fIgnoreCmp = TRUE;
                }
            }
        }
        else if(*pChar < *qChar)
        {
            return -1;
        }
        else if(*pChar > *qChar)
        {
            return 1;
        }
        else
        {
            fIgnoreCmp = FALSE;
        }

        qChar++;
        pChar++;
    }

    if (0 == *pChar && 0 == *qChar)
    {
        return 0;
    }

    if (0 == *pChar)
    {
        return -1;
    }

    return 1;
}




