///////////////////////////////////////////////////////////////////////
// ArchiveFileContentsImpl.h
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef __ARCHIVEFILECONTENTSIMPL_H__
#define __ARCHIVEFILECONTENTSIMPL_H__



#include "Model/IFileContentsModel.h"
#include "Common/IFileChapterInfo.h"

using namespace std;

class ArchiveFileContentsImpl : public IFileContentsModel
{
public:
    ArchiveFileContentsImpl(LPCSTR pFilePath);
    virtual ~ArchiveFileContentsImpl();

    virtual BOOL    HasPassword();
    virtual INT32    GetChapterNum();
    virtual void    SetPassword(LPCSTR StrPwd);
    virtual BOOL    GetChapterList(vector <CFileChapterInfo> & ChapterList);

protected:
    void  Init();
    void  AppendChapter(vector <CFileChapterInfo> & ChapterList,LPSTR pTitle,INT32 index, BOOL bIsDir = FALSE);
    static BOOL SortChapterByName(CFileChapterInfo src,CFileChapterInfo dest);
    static int SortString(LPCSTR str,LPCSTR dest);

private:
    std::string          m_StrFilePath;
    char                  m_rgPassword[256];
    INT32                 m_iChapterNum;
    BOOL                 m_bPathHasPwd;
    BOOL                 m_bChapterHasPwd;
};


#endif  /*__ARCHIVEFILECONTENTSIMPL_H__*/

