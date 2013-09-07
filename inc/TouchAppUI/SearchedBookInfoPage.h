////////////////////////////////////////////////////////////////////////
// SearchedBookInfoPage.h
// Contact: xukai
////////////////////////////////////////////////////////////////////////

#ifndef _BOOKINFOPAGE_H_
#define _BOOKINFOPAGE_H_

#include <string>
#include <limits.h>
#include "../GUI/UIPage.h"
#include "../GUI/UIImage.h"
#include "../GUI/UIComplexButton.h"
#include "../GUI/UITextSingleLine.h"
#include "../GUI/UIText.h"
#include "Model/91SearchItemModelImpl.h"
#include "Common/FileManager_DK.h"

enum SearchedDialogUpdateStatus
{
    No_Local_Book,
    No_Local_Book_Downloading,
    Local_Book_Updatable,
    Local_Book_Updating,
    Local_Book_Updated,
};

class CSearchedBookInfoPage : public UIPage
{
public:
    CSearchedBookInfoPage(int ImodleId, INT32 iLeft = 0, INT32 iTop = 0);
    CSearchedBookInfoPage(IOnlineBookStoreItemModel *_pItem,std::string _userAgent, INT32 iLeft = 0, INT32 iTop = 0);
    ~CSearchedBookInfoPage();
    virtual LPCSTR GetClassName() const
    {
        return ("CSearchedBookInfoPage");
    }

    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void Show(BOOL bIsShow, BOOL bIsRepaint = TRUE);
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    void UpdateSectionUI(bool bIsFull = false);
    BOOL IsSerializedBook();
    BOOL IsSerializedBookExists();
    BOOL IsSerializedBookDownloadedCompleted();
    string GetBookURL();
    string GetSerializedBookName();
    string GetFlatBookName();    
    BOOL IsBookDownloaded();
    BOOL IsFlatBookExist();
    PCDKFile GetDKFile();
    void OnSerializedBookUpdate(SNativeMessage event);
    void OnFailedToConnectServer(SNativeMessage event);
private:
    void Init();
    void InitializedStatus();
    int CurUrlIsDownloaded(const char *szUrl);
    BOOL CurUrlIsDownloading(const char *szUrl);
    void UpdateDownloadButton();
    void ShowHourglass();

    char m_bookname[NAME_MAX];
    UIImage m_imgSearchlog;
    UIImage m_imgBookCover;

    UIComplexButton m_btnDownloadBook;
    UIComplexButton m_btnReadBook;
    UIComplexButton m_btnRemoveBook;
    UIComplexButton m_btnUpdateBook;
    
    UIComplexButton m_btnSearch;

    // нд╠╬©Р
    UIText m_briefIntroduction;
    UITextSingleLine m_lblBookName;
    UITextSingleLine m_lblNewChapterText;
    UITextSingleLine m_lblAuthor;
    UITextSingleLine m_lblPageTitle;
    UITextSingleLine m_lblFileSize;
    UITextSingleLine m_lblIntroduction;
    UITextSingleLine m_tslsource;
//    UITextSingleLine m_tslDownLoading;
    IOnlineBookStoreItemModel *m_pCurSearchItemInfo;

    SearchedDialogUpdateStatus m_UpdateStatus;
    std::string m_UserAgent;

    INT32             m_iHourglassLeft;
    INT32             m_iHourglassTop;    
};

#endif //_BOOKINFOPAGE_H_
