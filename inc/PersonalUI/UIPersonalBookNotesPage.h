#ifndef _PERSONALUI_UIPERSONALBOOKNOTESPAGE_H_
#define _PERSONALUI_UIPERSONALBOOKNOTESPAGE_H_
#include "BookStoreUI/UIBookStoreNavigation.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "GUI/UITouchCommonListBox.h"
#include "Model/IBookmarkItemModel.h"

class DKXBlock;
class UIPersonalBookNotesPage : public UIBookStoreNavigation
{
public:
    UIPersonalBookNotesPage(const std::string& bookId,
                            const std::string& bookTitle,
                            const std::string& bookAuthor,
                            const std::string& bookCoverUrl,
                            const std::string& updateTime,
                            const int bookNoteCount);
    virtual ~UIPersonalBookNotesPage();

    virtual LPCSTR GetClassName() const
    {
        return "UIPersonalBookNotesPage";
    }

    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual UISizer* CreateMidSizer();
    virtual dk::bookstore::FetchDataResult FetchInfo();
    bool OnGetReadingDataUpdate(const EventArgs& args);
private:
    virtual void OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam);
#ifdef KINDLE_FOR_TOUCH
    bool OnMessageListBoxTurnPage(const EventArgs& args);
#endif
    bool OnMessageBookCoverTouch(const EventArgs& args);
    bool OnMessageListBoxSelectedItemChanged(const EventArgs& args);
    bool OnMessageListBoxSelectedItemOperation(const EventArgs& args);

    // exporting handlers
    bool OnEvernoteAuthFinished(const EventArgs& args);
    bool OnEvernoteGetUserFinished(const EventArgs& args);
    bool OnEvernoteExportFinished(const EventArgs& args);

    bool TurnPageUpOrDown(bool pageDown);

    void InitUI();
    void UpdateListBox();
    void UpdatePageNum();
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    void ShowNoteDeleteMsgBox();
    void DeleteBookNote();
    void ShowEmptyNotesMsgBox();
    void EmptyAllBookNote();
    bool OpenBook();
    void GotoNoteInBook();
    void ShareToWeibo();
    void ShowMenuDlg();
    void ExportToEvernote();
    bool ImplementExportToEvernote();

private:
    std::string m_bookId;
    std::string m_bookTitle;
    std::string m_bookAuthor;
    std::string m_bookCoverUrl;
    std::string m_updateTime;
    int m_iSelectedItemIndex;
    int m_bookNoteCount;
    int m_currentPage;
    int m_itemsPerPage;
    int m_itemHeight;
    PCDKFile m_fileInfo;
    DKXBlock* m_dkxBlock;

    UIBookCoverView m_bookCover;
    UITextSingleLine m_labelBookTitle;
    UITextSingleLine m_labelBookAuthor;
    UITextSingleLine m_labelBookNoteCount;
    UITextSingleLine m_labelUpdateTime;
    UITextSingleLine m_labelItemCount;
    UITextSingleLine m_labelPageNum;
    UITouchCommonListBox m_lstBookNotes;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnEmptyAllNotes;
    UITouchComplexButton m_btnExportAllNotes;
#else
    UIComplexButton m_btnEmptyAllNotes;
    UIComplexButton m_btnExportAllNotes;
#endif

    ICT_ReadingDataItemSPtrList m_objectList;
    dk::bookstore::sync::ReadingBookInfo m_readingBookInfo;

    // flag for exporting to evernote
    bool isWaitingToExport;
};//UIPersonalBookNotesPage
#endif//_PERSONALUI_UIPERSONALBOOKNOTESPAGE_H_
