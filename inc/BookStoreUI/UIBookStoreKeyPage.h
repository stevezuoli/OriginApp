#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREKEYDLG_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREKEYDLG_H_

#include "BookStoreUI/UIBookStoreNavigation.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITextBox.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#endif

class UIBookStoreKeyPage : public UIBookStoreNavigation
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreKeyPage";
    }

    UIBookStoreKeyPage();
    virtual ~UIBookStoreKeyPage();

    virtual UISizer* CreateMidSizer();
    void SetFetchStatus(FETCH_STATUS status);

    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    
    bool OnMessageSendBookKeyUpdate(const EventArgs& args);

private:
    void Init();
    bool UpdateSendStatus(const EventArgs& args);
    void SendBookKey();

private:
    UITextSingleLine m_titleInfo;
    UITextBox m_duokanKey;
    UIText m_duokanKeyTips1;
    UIText m_duokanKeyTips2;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnSend;
#else
    UIComplexButton m_btnSend;
#endif
};
#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREKEYDLG_H_
