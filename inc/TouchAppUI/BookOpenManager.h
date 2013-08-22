///////////////////////////////////////////////////////////////////////
// BookOpenManager.h
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
// create by jzn
////////////////////////////////////////////////////////////////////////

#ifndef __BOOKOPENMANAGER_H__
#define __BOOKOPENMANAGER_H__

#include <vector>
#include "Utility/CString.h"
#include "dkBaseType.h"
#include "ArchiveParser/ArchiveFactory.h"
#include "dkBuffer.h"
#include "GUI/IPage.h"
#include "Common/File_DK.h"
#include "singleton.h"

//using namespace DkFormat;
using namespace std;

typedef enum
{
    E_PAGE_BOOKINFO,
    E_PAGE_BOOKMARK,
    E_PAGE_BOOKTAG,
} ESelectPage;

class BookOpenManager
{
    SINGLETON_H(BookOpenManager)

public:
    void Initialize(UIContainer *pCurPage, UINT32  iHourglassLeft, UINT32  iHourglassTop);
    void Initialize(UIContainer *pCurPage, INT32  iBookID, UINT32  iHourglassLeft, UINT32  iHourglassTop);
    void Initialize(UIContainer *pCurPage, CString strBookName, UINT32  iHourglassLeft, UINT32  iHourglassTop);

    ~BookOpenManager();

    BOOL OpenBook();
    LPCSTR GetBookName();
    LPCSTR GetBookPath();
    DkFileFormat GetBookFormat();
    BOOL CheckIsNew();
    const CHAR  *GetAuthorName();
    float GetProgress();
    LPCSTR GetFilePassword();
    static BOOL  GetFilePassword(UIContainer *pPage,INT32 nBookId,LPSTR pPassword,INT32 iPwdLen);
    static BOOL OpenBookInfo(INT32 nBookID,PCHAR pPassword = NULL, BOOL bFromReadPage = FALSE, ESelectPage eDefaultTableBox = E_PAGE_BOOKINFO);
private:
    BookOpenManager(){};
    BOOL HandleContentPage();
    BOOL OpenArchiveBook();

private:
    UIContainer *m_pCurPage;
    INT32 m_iHourglassLeft;
    INT32 m_iHourglassTop;
    INT32 m_iBookID;
};
#endif    /*__BOOKCONTENTMANAGER_H__*/

