///////////////////////////////////////////////////////////////////////
// UIAccountRegisterDialog.cpp
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "CommonUI/UIAccountRegisterDialog.h"
#include "GUI/CTpGraphics.h"
#include "I18n/StringManager.h"
#include "Common/CAccountManager.h"
#include "Common/WindowsMetrics.h"
#include "Framework/CDisplay.h"
#include "interface.h"
#include "GUI/UISizer.h"
#include "Utility/StringUtil.h"
#include "drivers/DeviceInfo.h"
#include <tr1/functional>
#include "CommonUI/UIIMEManager.h"
#include "CommonUI/UIUtility.h"
#include "BookStore/BookStoreErrno.h"

using namespace dk::account;
using namespace dk::utility;
using namespace dk::bookstore;
using namespace WindowsMetrics;

UIAccountRegisterDialog::UIAccountRegisterDialog(UIContainer* pParent, LPCSTR title)
    : UIModalDlg(pParent, title)
{
    InitUI();
    SubscribeMessageEvent(CAccountManager::EventAccount,
            *CAccountManager::GetInstance(),
            std::tr1::bind(
                std::tr1::mem_fn(&UIAccountRegisterDialog::OnLoginEvent),
                this,
                std::tr1::placeholders::_1));
}

UIAccountRegisterDialog::~UIAccountRegisterDialog ()
{
}

void UIAccountRegisterDialog::InitUI ()
{
	m_editAccount.SetTipUTF8(StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_REGISTER_MAILADDRESS),0);
    m_editAccount.SetFontSize(GetWindowFontSize(UIModalDialogButtonIndex)); 
    m_editAccount.SetTextBoxStyle(ENGLISH_NORMAL);
	
    m_editPassword.SetTextBoxStyle(ENGLISH_NORMAL);
	m_editPassword.SetFontSize(GetWindowFontSize(UIModalDialogButtonIndex)); 
	m_editPassword.SetTipUTF8(StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_INPUT_PASSWORD),0);
    m_editPassword.SetId(ID_TEXTBOX_PASSWORD);

	m_btnOK.Initialize(IDOK,  StringManager::GetStringById(ACCOUNT_REGISTER), GetWindowFontSize(UIModalDialogButtonIndex), SPtr<ITpImage>());
    m_btnCancel.Initialize(IDCANCEL,  StringManager::GetStringById(ACTION_CANCEL), GetWindowFontSize(UIModalDialogButtonIndex), SPtr<ITpImage>());

	m_btnRegisterXiaomi.Initialize(ID_BTN_ACCOUNT_XIAOMI_REGISTER,
		StringManager::GetStringById(TOUCH_SYSTEMSETTING_REGISTER_XIAOMI_ACCOUNT),
		GetWindowFontSize(UIModalDialogButtonIndex), SPtr<ITpImage>());
    m_btnRegisterXiaomi.ShowBorder(false);

    m_textTips.SetText(StringManager::GetPCSTRById(ACCOUNT_EMPTY_MAIL_PWD));
    m_textTips.SetFontSize(GetWindowFontSize(UIModalDialogButtonIndex));
    m_textTips.SetReversedColor(true);
	m_textTips.SetVisible(false);

    AppendChild(&m_editAccount);
    AppendChild(&m_editPassword);
    AppendChild(&m_textTips);
	AppendChild(&m_btnRegisterXiaomi);
    m_btnGroup.AddButton(&m_btnCancel);
    m_btnGroup.AddButton(&m_btnOK);
    m_btnGroup.SetBottomLinePixel(GetWindowMetrics(UIButtonGroupBottomLinePixelIndex));
    bool hasAccount = false;

    if (m_windowSizer)
    {
        m_editAccount.SetMinHeight(GetWindowMetrics(UIModalDialogEditBoxIndex));
        m_editPassword.SetMinHeight(GetWindowMetrics(UIModalDialogEditBoxIndex));
        m_windowSizer->Add(&m_editAccount, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)));
        m_windowSizer->AddSpacer(GetWindowMetrics(UIAccountLoginDialogMarginIndex));
        m_windowSizer->Add(&m_editPassword, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)));
        m_windowSizer->AddSpacer(GetWindowMetrics(UIAccountLoginDialogMarginIndex)/2);

        UISizer* middleSizer = new UIBoxSizer(dkHORIZONTAL);
        middleSizer->Add(&m_textTips, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).ReserveSpaceEvenIfHidden());
        middleSizer->AddStretchSpacer();

		UISizer* xiaomiSizer = new UIBoxSizer(dkVERTICAL);
		xiaomiSizer->SetMinHeight(m_btnGroup.GetMinHeight() - 10);
        xiaomiSizer->Add(&m_btnRegisterXiaomi, UISizerFlags(1).Expand());
		
        m_windowSizer->Add(middleSizer, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)).Expand());
		m_windowSizer->AddSpacer(GetWindowMetrics(UIAccountLoginDialogMarginIndex)/2);
        m_windowSizer->Add(&m_btnGroup, UISizerFlags().Expand());
		m_windowSizer->Add(xiaomiSizer, UISizerFlags().Expand().Center().Border(dkALL, 5));
        int width = GetWindowMetrics(UIModalDialogWidthIndex);
        int height = m_windowSizer->GetMinSize().GetHeight();

        MakeCenterAboveIME(width, height);

		
    }

    UITextBox* pFocusedWnd = hasAccount ? &m_editPassword : &m_editAccount;
    SetChildWindowFocus(GetChildIndex(pFocusedWnd), false);
    UIIME* pIME = UIIMEManager::GetIME(IUIIME_ENGLISH_LOWER, pFocusedWnd);
    if (pIME)
    {
        pIME->SetShowDelay(true);
    }
}

void UIAccountRegisterDialog::MoveWindow(int _iLeft, int _iTop, int _iWidth, int _iHeight)
{
    UIModalDlg::MoveWindow(_iLeft, _iTop, _iWidth, _iHeight);
    Layout();
}

void UIAccountRegisterDialog::Register(const char* account, const char* password)
{
    CAccountManager::GetInstance()->RegisterAsync(account, account, password);
    m_btnOK.SetEnable(false);
    m_btnCancel.SetEnable(false);
    UpdateWindow();
}

void UIAccountRegisterDialog::OnRegisterClick()
{
    CDisplay::CacheDisabler cache;
    m_textTips.SetVisible(false);
    m_textTips.UpdateWindow();
    std::string account = m_editAccount.GetTextUTF8();
    unsigned int pos = account.find_last_not_of(' ');
    if (account.size() > 0 && pos != (account.size() - 1))
    {
        account = account.substr(0, pos+1);
    }
    std::string password = m_editPassword.GetTextUTF8();
    if(!account.empty() && !password.empty() && StringUtil::IsValidEmail(account.c_str()))
    {
        Register(account.c_str(), password.c_str());
    }
    else if (account.empty())
    {
        m_editAccount.SetTipUTF8(StringManager::GetPCSTRById(EMAILCANNOTEMPTY),0);
        m_editAccount.SetFocus(TRUE);
        m_editAccount.UpdateWindow();
    }
    else if(!StringUtil::IsValidEmail(account.c_str()))
    {
        SetTips(StringManager::GetPCSTRById(ACCOUNT_EMAIL_INVALID));
        m_editAccount.SetFocus(TRUE);
    }
    else
    {
        SetTips(StringManager::GetPCSTRById(ACCOUNT_EMPTY_MAIL_PWD));
    }
}

void UIAccountRegisterDialog::OnXiaomiRegisterClick()
{
	EndDialog(IDCANCEL);
	CAccountManager::GetInstance()->RegisterXiaomiAccount();
}

void UIAccountRegisterDialog::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
    case IDOK:
	case ID_TEXTBOX_PASSWORD:
        OnRegisterClick();
        break;
    case IDCANCEL:
        EndDialog(IDCANCEL); 
        break;
	case ID_BTN_ACCOUNT_XIAOMI_REGISTER:
		OnXiaomiRegisterClick();
		break;
    default:
        break;
    }
}

void UIAccountRegisterDialog::SetTips(LPCSTR tips)
{
    if (!tips)
    {
        return;
    }
    m_textTips.SetText(tips);
    m_textTips.SetVisible(true);
    Layout();
    UpdateWindow();
}

bool UIAccountRegisterDialog::OnLoginEvent(const EventArgs& args)
{
    CDisplay* pDisplay = CDisplay::GetDisplay();
    if (pDisplay)
    {
        pDisplay->ClearScreen();
    }
    const AccountEventArgs& accountEventArgs = (const AccountEventArgs&)args;
	if (0 == accountEventArgs.errorCode
        && AccountEventArgs::LS_PASSWORD_SUCCEEDED == accountEventArgs.logStatus)
    {
        EndDialog(IDOK);
        return true;
    }
    else if(BSE_DUOKAN_ACCOUNT_REGISTER_CLOSED == accountEventArgs.errorCode)
    {
        EndDialog(IDCANCEL);
        return true;
    }
    else if(AccountEventArgs::LS_PASSWORD_FAILED == accountEventArgs.logStatus)
    {
        SetTips(accountEventArgs.errorMessage.c_str());
        m_btnOK.SetEnable(true);
        m_btnCancel.SetEnable(true);
    }
	return true;
}

