#ifndef _DKREADER_UIPERSONALPUSHEDMESSAGESPAGE_H_
#define _DKREADER_UIPERSONALPUSHEDMESSAGESPAGE_H_

#include "BookStoreUI/UIBookStoreListBoxPage.h"
/*#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#endif*/

class UIPersonalPushedMessagesPage : public UIBookStoreListBoxPage
{
public:
    UIPersonalPushedMessagesPage();
    ~UIPersonalPushedMessagesPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIPersonalPushedMessagesPage";
    }

    virtual bool CreateListBoxPanel();
    virtual UISizer* CreateMidSizer();
    virtual void SetFetchStatus(FETCH_STATUS status);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void OnEnter();
    virtual void OnLeave();
    virtual void OnLoad();
    virtual void OnUnLoad();

private:
    bool OnMessagePushedMessagesUpdate(const EventArgs& args);
    bool OnMessageUnreadCountChanged(const EventArgs& args);
    bool LogDataChanged(const EventArgs& args);
    void StartGetMessages();
    void StopGetMessages();
    UITextSingleLine m_loading;
};
#endif//_DKREADER_UIPERSONALPUSHEDMESSAGESPAGE_H_

