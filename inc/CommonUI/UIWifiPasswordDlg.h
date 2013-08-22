#ifndef UIWIFIPASSWORDDLG_H
#define UIWIFIPASSWORDDLG_H

#include "GUI/UIModalDialog.h"
#include "GUI/UITextBox.h"

class UIWifiPasswordDlg : public UIModalDlg
{
public:
    virtual LPCSTR GetClassName() const
    {
        return ("UIWifiPasswordDlg");
    }

	UIWifiPasswordDlg(UIContainer* pParent, LPCSTR title, bool showUserName);
    ~UIWifiPasswordDlg();

    std::string GetIdentity();
	std::string GetPassWord();
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

private:
	void InitUI();

private:
    UITextSingleLine m_textIdentity;
    UITextBox m_inputIdentity;
	UITextSingleLine	 m_textPassword;
	UITextBox			 m_inputPassword;
#ifdef KINDLE_FOR_TOUCH
	UITouchComplexButton m_btnConfirm;
	UITouchComplexButton m_btnCancel;
#else
    UIComplexButton m_btnConfirm;
    UIComplexButton m_btnCancel;
#endif
	bool				 m_bIsClearText;
};















#endif//UIWIFIPASSWORDDLG_H