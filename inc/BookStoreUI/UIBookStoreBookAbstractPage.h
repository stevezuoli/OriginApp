#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKABSTRACTPAGE_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKABSTRACTPAGE_H_

#include "BookStoreUI/UIBookStoreNavigationWithFling.h"
#include "GUI/UITouchButton.h"
#include "GUI/UITextSingleLine.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/GestureDetector.h"
#endif

class UIBookStoreBookAbstractPage : public UIBookStoreNavigationWithFling
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookAbstractPage";
    }
    UIBookStoreBookAbstractPage(const std::string& content);
    ~UIBookStoreBookAbstractPage();

    UISizer* CreateMidSizer();
    void SetFetchStatus(FETCH_STATUS status);
    void UpdatePageNumber();
    bool TurnPage(bool downPage);
    
private:
    void Init();
private:
    UIText m_abstractText;
    UITextSingleLine m_pageNumber;
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKABSTRACTPAGE_H_
