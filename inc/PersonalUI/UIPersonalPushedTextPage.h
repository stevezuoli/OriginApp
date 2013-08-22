#ifndef _DKREADER_UIPERSONALPUSHEDTEXTPAGE_H_
#define _DKREADER_UIPERSONALPUSHEDTEXTPAGE_H_

#include "BookStoreUI/UIBookStoreNavigationWithFling.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "GUI/UIText.h"

class UIPersonalPushedTextPage : public UIBookStoreNavigationWithFling
{
public:
    UIPersonalPushedTextPage(const std::string& title, const std::string& content);
    ~UIPersonalPushedTextPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIPersonalPushedTextPage";
    }

    virtual void SetFetchStatus(FETCH_STATUS status);
    virtual UISizer* CreateMidSizer();

    void UpdatePageNumber();
    bool TurnPage(bool downPage);
private:
    void UpdateChildWindow(bool showCover);
private:
    UIText m_content;
    UITextSingleLine m_pageNumber;
};

#endif//_DKREADER_UIPERSONALPUSHEDTEXTPAGE_H_

