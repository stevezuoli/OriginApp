#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTORETABLEPAGE_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTORETABLEPAGE_H_

#include "CommonUI/UITableBarController.h"
#include "BookStoreUI/UIBookStoreNavigation.h"

/********************************************
* UIBookStoreTablePage
********************************************/
class UITablePanel;
class UIBookStoreTablePage : public UIBookStoreNavigation
{
public:
    UIBookStoreTablePage(const std::string& title, bool hasBottomBar = true, bool showFetchInfo = true, bool showRefresh = true);
    ~UIBookStoreTablePage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreTablePage";
    }
    virtual void SetTableController();
    virtual void SetFetchStatus(FETCH_STATUS status);
    virtual UISizer* CreateMidSizer();
    virtual bool OnMessageListBoxUpdate(const EventArgs& args);
    virtual bool OnMessagePanelInfoUpdate(const EventArgs& args);
    BOOL OnKeyPress(INT32 iKeyCode);

    void UpdateSectionUI(bool bIsFull = false);
protected:
    UITableController m_tableController;
    UITablePanel* m_pCurPanel;
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTORETABLEPAGE_H_

