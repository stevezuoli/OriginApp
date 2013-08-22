#ifndef __APPUI_UIBOOKSTOREPAYINFODLG_H__
#define __APPUI_UIBOOKSTOREPAYINFODLG_H__

#include "GUI/UIModalDialog.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITextBox.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#endif

class UIBookStorePayInfoDlg : public UIModalDlg
{
public:
    enum PayMode
    {
        PM_OTHER_DEVICE,
        PM_EMAIL,
        PM_WEBSITE
    };

    UIBookStorePayInfoDlg(UIContainer* parent,
                          const LPCSTR szTitle,
                          PayMode mode,
                          const char* mailBox = 0,
                          const char* orderId = 0);
    virtual ~UIBookStorePayInfoDlg();
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    bool IsMailSended() const
    {
        return m_sendingStatus == SS_SENDED;
    }
protected:
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    enum SendingStatus
    {
        SS_NONE,
        SS_SENDING,
        SS_SENDED
    };

private:
    bool OnMailOrderUpdate(const EventArgs& eventArgs);
    bool OnCloseClick(const EventArgs&);
    void InitUI();
    UITextSingleLine m_footnoteLine;
    UIText m_hintText;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnLeft;
    UITouchComplexButton m_btnRight;
#else
    bool OnOtherDeviceKeyPress(int keyCode);
    bool OnMailKeyPress(int keyCode);
    UIComplexButton m_btnLeft;
    UIComplexButton m_btnRight;
#endif
    std::string m_mailBox;
    std::string m_orderId;
    SendingStatus m_sendingStatus;
	PayMode m_payMode;
};
#endif
