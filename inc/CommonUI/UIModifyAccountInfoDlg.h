#ifndef UIMODIFYACCOUNTINFODLG_H
#define UIMODIFYACCOUNTINFODLG_H

#include "GUI/UIModalDialog.h"
#include "GUI/UITextBox.h"

class UISizer;
class UIModifyAccountInfoDlg : public UIModalDlg
{
public:

    virtual LPCSTR GetClassName() const
    {
        return "UIModifyAccountInfoDlg";
    }

	UIModifyAccountInfoDlg(UIContainer* pParent);
	virtual ~UIModifyAccountInfoDlg();

    virtual void OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam);

private:
	void InitUI();
	void ResizeDialog();
	void Modify();
	void ModifyAlias(string alias);
	void ModifyPassword(string oldPwd, string newPwd);
	void SetTips(LPCSTR);
    bool OnLoginEvent(const EventArgs& args);

private:
    enum LoginAction
    {
        LA_NONE = 0,
        LA_CHANGING_ALIAS = 0x1,
        LA_CHANGING_PASSWORD = 0x2,
        LA_CHANGING_ALIAS_FAILED = 0x4,
        LA_CHANGING_PASSWORD_FAILED = 0x8,
        LA_CHANGING = LA_CHANGING_ALIAS | LA_CHANGING_PASSWORD
    };

	UITextSingleLine m_textAlias;
	UITextSingleLine m_textTips;
	UITextBox		 m_editAlias;
	UITextBox		 m_editOldPwd;
	UITextBox		 m_editNewPwd;
    UISizer* m_pModifyPwdSizer;

	UITouchComplexButton m_btnModifyPassword;
	UITouchComplexButton m_btnCancel;
	UITouchComplexButton m_btnConfirm;
	bool			m_bIsModifyPwd;
    int m_loginAction;
};//UIModifyAccountInfoDlg
#endif//UIMODIFYACCOUNTINFODLG_H
