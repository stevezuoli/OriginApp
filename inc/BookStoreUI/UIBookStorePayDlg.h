#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREPAYDLG_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREPAYDLG_H_

#include "GUI/UIModalDialog.h"
#include "GUI/UITextBox.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#endif

class UIBookStorePayDlg : public UIModalDlg
{
public:
    enum Action
    {
        NONE,
        SEND_MAIL,
        BUY_FROM_OTHER_DEVICE,
        BUY_FROM_WEBSITE
    };
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStorePayDlg";
    }

    UIBookStorePayDlg(UIContainer* pParent, BOOL enableMail = TRUE);
    virtual ~UIBookStorePayDlg();

    void SetOrderId(const char* orderId)
    {
        m_orderId = orderId;
    }

    Action GetAction() const
    {
        return m_action;
    }
protected:
    void Dispose ();

private:
    bool OnMailOrderUpdate(const EventArgs& args);
    void Init ();
    void OnDispose (BOOL bDisposed);
    bool SendLinkToMyMailBox();
    bool BuyInOtherDevice();
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

private:
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnSendMail;
    UITouchComplexButton m_btnBuyInOtherDev;
    UITouchComplexButton m_btnBuyFromWebsite;
#else
    UIComplexButton m_btnSendMail;
    UIComplexButton m_btnBuyInOtherDev;
    UIComplexButton m_btnBuyFromWebsite;
#endif
    BOOL m_bEnableMail;
    BOOL m_bIsDisposed;
    std::string m_orderId;
    Action m_action;
};
#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREPAYDLG_H_
