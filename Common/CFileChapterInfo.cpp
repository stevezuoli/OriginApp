///////////////////////////////////////////////////////////////////////
// CFileChapterInfo.cpp
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "Common/IFileChapterInfo.h"

CFileChapterInfo::CFileChapterInfo()
                 :m_Name()
                 ,m_Path()
                 ,m_cstrExt()
                 ,m_id()
                 ,m_bIsDir(FALSE)
                 ,m_bHasPwd(FALSE)
{

}


CFileChapterInfo::~CFileChapterInfo()
{

}

LPCSTR     CFileChapterInfo::GetChapterName()
{
    return (m_Name.c_str());
}

LPCSTR     CFileChapterInfo::GetChapterPath()
{
    return (m_Path.c_str());
}

LPCSTR    CFileChapterInfo::GetChapterExName()
{
    return (m_cstrExt.c_str());
}

INT32    CFileChapterInfo::GetChapterId()
{
    return m_id;
}

BOOL     CFileChapterInfo::ChapterIsDIR()
{
    return m_bIsDir;
}

BOOL     CFileChapterInfo::ChapterHasPwd()
{
    return m_bHasPwd;
}


void     CFileChapterInfo::SetChapterName(LPCSTR pName)
{
    if(pName)
    {
        m_Name = string(pName);
    }
}

void     CFileChapterInfo::SetChapterPath(LPCSTR pPath)
{
    if(pPath)
    {
        m_Path = string(pPath);
    }
}

void     CFileChapterInfo::SetChapterExName(LPCSTR pExName)
{
    if(pExName)
    {
        m_cstrExt = string(pExName);
    }
}

void    CFileChapterInfo::SetChapterId(INT32 Id)
{
    if(Id >= 0)
    {
        m_id = Id;
    }
}

void     CFileChapterInfo::SetChapterDirAttr(BOOL bIsDir)
{
    m_bIsDir = bIsDir;
}

void     CFileChapterInfo::SetChapterHasPwdAttr(BOOL bHasPwd)
{
    m_bHasPwd = bHasPwd;
}
