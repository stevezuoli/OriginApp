#ifndef _DKREADER_UIBOOKSTORELISTBOXPAGE_H_
#define _DKREADER_UIBOOKSTORELISTBOXPAGE_H_

#include "BookStoreUI/UIBookStoreNavigation.h"

class UIBookStoreListBoxPanel;

class UIBookStoreListBoxPage : public UIBookStoreNavigation
{
public:
    UIBookStoreListBoxPage(const std::string& title = string(), bool hasBottomBar = true);
    ~UIBookStoreListBoxPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreListBoxPage";
    }

    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual bool CreateListBoxPanel();
    virtual void SetFetchStatus(FETCH_STATUS status);
    virtual UISizer* CreateMidSizer();
    virtual bool OnMessageListBoxUpdate(const EventArgs& args);
    void UpdateSectionUI(bool bIsFull = false);

protected:
    UIBookStoreListBoxPanel* m_pListBoxPanel;
};
#endif//_DKREADER_UIBOOKSTORELISTBOXPAGE_H_
