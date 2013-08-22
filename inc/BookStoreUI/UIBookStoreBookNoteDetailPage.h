#ifndef _BOOKSTOREUI_UIBOOKSTOREBOOKNOTEDETAILPAGE_H_
#define _BOOKSTOREUI_UIBOOKSTOREBOOKNOTEDETAILPAGE_H_

#include "BookStoreUI/UIBookStoreNavigationWithFling.h"
#include "GUI/UITouchButton.h"
#include "GUI/UITextSingleLine.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/GestureDetector.h"
#endif

class UIBookStoreBookNoteDetailPage : public UIBookStoreNavigationWithFling
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookNoteDetailPage";
    }
    UIBookStoreBookNoteDetailPage(const std::string& content);
    ~UIBookStoreBookNoteDetailPage();

    UISizer* CreateMidSizer();
    void SetFetchStatus(FETCH_STATUS status);
    void UpdatePageNumber();
    bool TurnPage(bool downPage);
    
private:
    void Init();
private:
    UIText m_content;
    UITextSingleLine m_pageNumber;
};

#endif//_BOOKSTOREUI_UIBOOKSTOREBOOKNOTEDETAILPAGE_H_
