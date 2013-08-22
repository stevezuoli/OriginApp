////////////////////////////////////////////////////////////////////////
// IFileContentsModel.h
// Contact: wangh
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __IFILECHAPTERINFO_H__
#define __IFILECHAPTERINFO_H__

#include "dkBaseType.h"
#include <string>
using namespace std;

class CFileChapterInfo
{
public:
    CFileChapterInfo();
    ~CFileChapterInfo();


public:
    LPCSTR         GetChapterName();
    LPCSTR         GetChapterPath();
    LPCSTR         GetChapterExName();
    INT32         GetChapterId();
    BOOL         ChapterIsDIR();
    BOOL         ChapterHasPwd();


    void         SetChapterName(LPCSTR pName);
    void         SetChapterPath(LPCSTR pPath);
    void         SetChapterExName(LPCSTR pExName);
    void         SetChapterId(INT32 Id);
    void         SetChapterDirAttr(BOOL bIsDir);
    void         SetChapterHasPwdAttr(BOOL bHasPwd);
public:
    string       m_Name;
    string      m_Path;
    string       m_cstrExt;
    INT32       m_id;
    BOOL           m_bIsDir;
    BOOL         m_bHasPwd;
};

#endif  /*__IFILECHAPTERINFO_H__*/

