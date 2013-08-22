///////////////////////////////////////////////////////////////////////
// UIAccountLoginDialog.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
///////////////////////////////////////////////////////////////////////

#ifndef __UIACCOUNTLOGINDIALOG_H__
#define __UIACCOUNTLOGINDIALOG_H__

#include "GUI/UIModalDialog.h"
#include "GUI/UIImage.h"
#include "Utility/ImageManager.h"
#include "GUI/UITextBox.h"
#include "GUI/UITextSingleLine.h"
#include "Utility/ColorManager.h"

//using namespace DkFormat;

class UIAccountLoginDialog : public UIModalDlg
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIAccountLoginDialog";
    }

    UIAccountLoginDialog(UIContainer* pParent, LPCSTR title);
    virtual ~UIAccountLoginDialog();
    void SetInitText(CString text, BOOL bPwdLabel);
    void ChangeHintText(CString text, BOOL bPwdLabel);

    virtual void MoveWindow(INT32 _iLeft, INT32 _iTop, INT32 _iWidth, INT32 _iHeight);
    virtual void EndDialog(INT32 iEndCode,BOOL fRepaintIt = TRUE);
    bool ForgettingPassword() const; 

	string GetUserAccount();
protected:
    void Dispose ();

private:
    bool OnLoginEvent(const EventArgs& args);
    enum LoginAction
    {
        LA_NONE,
        LA_REGISTERING,
        LA_LOGINING,
        LA_FORGETTING,
    };
    void InitUI ();
    void OnDispose (BOOL bDisposed);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
	void OnDuoKanLoginClick();
	void Login(const char* account, const char* password);
	void SetTips(LPCSTR);

    UITouchComplexButton    m_btnOK;
    UITouchComplexButton    m_btnCancel;
    UITouchComplexButton    m_btnFindPwd;
	UITouchComplexButton    m_btnXiaoMiLogin;
	UITextSingleLine		m_textTips;

    UITextBox          m_editPassword;
    UITextBox          m_editAccount;
    BOOL               m_bIsDisposed;
    LoginAction m_loginAction;
};

#endif //__UIAccountLoginDialog_H__
