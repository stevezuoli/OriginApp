#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKINFORELATEDLIST_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKINFORELATEDLIST_H_

#include "GUI/UIText.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIContainer.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIImage.h"
#endif
#include "I18n/StringManager.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "BookStore/BookStoreInfoManager.h"

class UIBookStoreBookInfoRelatedList : public UIContainer
{
    static const int RELATED_BOOK_COUNT = 8;
public:
    UIBookStoreBookInfoRelatedList(int rows = 1, int columns = 4,
        const CString& label = StringManager::GetStringById(TOUCH_ALSOVIEW));
    virtual ~UIBookStoreBookInfoRelatedList();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookInfoRelatedList";
    }
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    BOOL OnKeyPress(INT32 iKeyCode);
    void SetBookInfo(dk::bookstore::model::BookInfoSPtr bookInfo, bool reverse = false);
    void SetFocus(BOOL bIsFocus);
    HRESULT DrawBackGround(DK_IMAGE drawingImg);

private:
    bool OnMessageRelatedBookCoverTouch(const EventArgs& args);
    void UpdateRelatedInfo();
    bool TurnPageUpOrDown(bool pageUp);

private:
    dk::bookstore::model::BookInfoList m_relatedList;
    UIBookCoverView m_relatedBookCoverList[RELATED_BOOK_COUNT];
    UIText m_relatedBookTitleList[RELATED_BOOK_COUNT];
    UITextSingleLine m_relatedBookLabel;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_prevWnd;
    UITouchComplexButton m_nextWnd;
#else
    UIImage m_prevWnd;
    UIImage m_nextWnd;
#endif
    int m_startIndex;
    int m_rows;
    int m_columns;
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKINFORELATEDLIST_H_

