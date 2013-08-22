///////////////////////////////////////////////////////////////////////
// DkReaderPage.h
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef __DKREADERPAGE_H__
#define __DKREADERPAGE_H__

#include "TouchAppUI/UIImageReaderContainer.h"
#include "TouchAppUI/UIBookReaderContainer.h"
#include "GUI/UIPage.h"


using DkFormat::IBookmark;

typedef enum
{
    READER_PAGE_INIT_NONE,
    READER_PAGE_INIT_SINGLEFILE,
    READER_PAGE_INIT_DKBUFFER,
    READER_PAGE_INIT_FILELIST
}ReaderPageInitType;

typedef enum
{
    NONE_FOCUSED,
    BOOK_READER_CONTAINER,
    IMAGE_READER_CONTAINER
}FocusedContaner;

class DkReaderPage : public UIPage
{
public:
    DkReaderPage();
    virtual ~DkReaderPage();

    virtual LPCSTR GetClassName() const
    {
        return "DkReaderPage";
    }
    
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    
    
    BOOL Initialize (LPCSTR strFilePath, LPCSTR strBookName, int iBookId);

    BOOL Initialize (UINT nFileId, INT iBookId, IDkStream *pStream, PCCH pstrExtension, PCCH pPassword = NULL);
    
    BOOL Initialize(SPtr<BookFileList> spFileList, UINT uCurFileIndex = 0, BOOL fIsAllImage = FALSE, const char *pPassword = NULL);
    
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    
    void GotoBookmark(ICT_ReadingDataItem *pBookmark);

    BOOL GotoImgPageNo(INT iPageNo);

    BOOL IsCurDownFinished();

    BOOL IsCurUpFinished();

    BOOL GotoCurLastPage();
    
    virtual void OnLoad();
    
    virtual void OnUnLoad();

    virtual void OnEnter();
    
    virtual void OnLeave();    

    virtual void OnTTSMessage(SNativeMessage* msg);

    UINT GetCurFileId();
    virtual UITitleBar *GetTitleBar() { return NULL; }
    
protected:
    void Dispose();
private:
    BOOL InitCurFile();
    BOOL GetAchiveFileContent(CDKFile *pDkFile, IDkStream *&pStream, CHAR **ppExtension,  PUINT puFileId);
    void Finalize();
    void ReleaseContainers();
    void ResetContainers();
    LPCSTR GetCurBookPath();
    INT LoadImgReadingInfo();
    void SaveImgReadingInfo();
    INT GetImgReadingIndex(IBookmark *pBookmark);

    void SetCurDownFinished(); 
    void SetCurUpFinished(); 
private:
    BOOL m_bIsDisposed;
    ReaderPageInitType m_eInitType;
    SPtr<BookFileList> m_spFileList;
    UINT m_uCurIndex;
    BOOL m_fIsAllImage;
    PCHAR m_pPassword;   
    UIBookReaderContainer *m_pBookReaderContainer;
    UIImageReaderContainer *m_pImageReaderContainer;
    FocusedContaner m_eFocusedContainer;
};

#endif

