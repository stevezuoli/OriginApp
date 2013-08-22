#ifndef UIFORGETPASSWORDDLG_H
#define UIFORGETPASSWORDDLG_H

#include "GUI/UIModalDialog.h"
#include "GUI/UITextBox.h"

class UISizer;
class UIForgetPasswordDlg : public UIModalDlg
{
public:

    virtual LPCSTR GetClassName() const
    {
        return "UIForgetPasswordDlg";
    }

	UIForgetPasswordDlg(UIContainer* pParent, const char* forgetEmail);
	virtual ~UIForgetPasswordDlg();
    virtual void OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam);

private:
	void InitUI();

	void ReGetPassWord();
	void ShowSuccessWindows(bool r=true);
    bool OnLoginEvent(const EventArgs& args);

private:
	UITextSingleLine m_textTips;
	UITextSingleLine m_textMailHasPosted;
	UITextSingleLine m_textContactMail;
	UITextBox		 m_editUserMail;

	UITouchComplexButton m_btnCancel;
	UITouchComplexButton m_btnSendPwd;
	UITouchComplexButton m_btnClose;

    UISizer* m_pSuccessSizer;
    UISizer* m_pSendPwdSizer;
    enum LoginAction
    {
        LA_NONE,
        LA_FORGETTING_PASSWORD
    };
    LoginAction m_loginAction;

	string m_forgetEmail;
};
#endif//UIFORGETPASSWORDDLG_H
