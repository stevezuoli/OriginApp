#ifndef _PERSONALUI_UIBOOKSTORECOMMENTSPAGE_H_
#define _PERSONALUI_UIBOOKSTORECOMMENTSPAGE_H_

#include "BookStoreUI/UIBookStoreTablePage.h"

class UIBookStoreListBoxWithBtn;
class UIPersonalCommentsPage : public UIBookStoreTablePage
{
public:
    UIPersonalCommentsPage();
    ~UIPersonalCommentsPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIPersonalCommentsPage";
    }

    //virtual bool CreateListBoxPanel();
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual dk::bookstore::FetchDataResult FetchInfo();
    virtual void SetTableController();
#ifndef KINDLE_FOR_TOUCH
    virtual BOOL OnKeyPress(INT32 iKeyCode);
#endif

private:
    bool OnMessageHotCommentsListUpdate(const EventArgs& args);
    bool OnMessageOwnCommentsListUpdate(const EventArgs& args);

private:
    UIBookStoreListBoxWithBtn* m_pHotCommentsPanel;
    UIBookStoreListBoxWithBtn* m_pOwnCommentsPanel;
};

#endif//_PERSONALUI_UIBOOKSTORECOMMENTSPAGE_H_

