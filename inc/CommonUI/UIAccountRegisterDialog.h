///////////////////////////////////////////////////////////////////////
// UIAccountRegisterDialog.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
///////////////////////////////////////////////////////////////////////

#ifndef __UIACCOUNTREGISTERDIALOG_H__
#define __UIACCOUNTREGISTERDIALOG_H__

#include "GUI/UIModalDialog.h"
#include "GUI/UIImage.h"
#include "Utility/ImageManager.h"
#include "GUI/UITextBox.h"
#include "GUI/UITextSingleLine.h"
#include "Utility/ColorManager.h"

//using namespace DkFormat;

class UIAccountRegisterDialog : public UIModalDlg
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIAccountRegisterDialog";
    }

    UIAccountRegisterDialog(UIContainer* pParent, LPCSTR title);
    virtual ~UIAccountRegisterDialog();
    virtual void MoveWindow(INT32 _iLeft, INT32 _iTop, INT32 _iWidth, INT32 _iHeight);
	
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
	void OnRegisterClick();
	void OnXiaomiRegisterClick();
	void Register(const char* account, const char* password);
	void SetTips(LPCSTR);

private:
    UITouchComplexButton    m_btnOK;
    UITouchComplexButton    m_btnCancel;
	UITouchComplexButton    m_btnRegisterXiaomi;
	UITextSingleLine		m_textTips;
    UITextBox               m_editPassword;
    UITextBox               m_editAccount;
};

#endif //__UIAccountLoginDialog_H__
