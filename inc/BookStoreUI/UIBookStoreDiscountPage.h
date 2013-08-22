#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREDISCOUNTPAGE_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREDISCOUNTPAGE_H_

#include "BookStoreUI/UIBookStoreTablePage.h"

class UIBookStoreListBoxPanel;

class UIBookStoreDiscountPage : public UIBookStoreTablePage
{
public:
    UIBookStoreDiscountPage();
    ~UIBookStoreDiscountPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreDiscountPage";
    }

    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

    virtual dk::bookstore::FetchDataResult FetchInfo();
    virtual void SetTableController();

private:
    bool OnMessageDiscountListUpdate(const EventArgs& args);
    bool OnMessageFreeListUpdate(const EventArgs& args);
    
private:
    UIBookStoreListBoxPanel* m_pDiscountPanel;
    UIBookStoreListBoxPanel* m_pFreePanel;
};
#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREDISCOUNTPAGE_H_
