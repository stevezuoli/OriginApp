///////////////////////////////////////////////////////////////////////
// UIAccountLoginDialog.cpp
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "CommonUI/UIAccountLoginDialog.h"
#include "GUI/CTpGraphics.h"
#include "I18n/StringManager.h"
#include "Common/CAccountManager.h"
#include "Common/WindowsMetrics.h"
#include "Framework/CDisplay.h"
#include "Framework/CMessageHandler.h"
#include "Framework/CNativeThread.h"
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
using namespace WindowsMetrics;
using namespace dk::bookstore;

UIAccountLoginDialog::UIAccountLoginDialog(UIContainer* pParent, LPCSTR title)
    : UIModalDlg(pParent, title)
    , m_bIsDisposed (FALSE)
    , m_loginAction(LA_NONE)
{
    InitUI();
    SubscribeMessageEvent(CAccountManager::EventAccount,
            *CAccountManager::GetInstance(),
            std::tr1::bind(
                std::tr1::mem_fn(&UIAccountLoginDialog::OnLoginEvent),
                this,
                std::tr1::placeholders::_1));
}

UIAccountLoginDialog::~UIAccountLoginDialog ()
{
    Dispose ();
}

void UIAccountLoginDialog::InitUI ()
{
	CAccountManager* accountManager = CAccountManager::GetInstance();
    string emailAccount;
    if (accountManager && accountManager->IsDuokanAccount())
    {
        emailAccount = accountManager->GetEmailFromFile();
    }
    m_editAccount.SetFontSize(GetWindowFontSize(UIModalDialogButtonIndex)); 
    m_editAccount.SetTextUTF8(emailAccount.c_str());
    m_editAccount.SetTipUTF8(StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_INPUT_MAILADDRESS),0);
    m_editAccount.SetTextBoxStyle(ENGLISH_NORMAL);
	
    m_editPassword.SetTextBoxStyle(ENGLISH_NORMAL);
	m_editPassword.SetFontSize(GetWindowFontSize(UIModalDialogButtonIndex)); 
	m_editPassword.SetTipUTF8(StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_INPUT_PASSWORD),0);
    m_editPassword.SetId(ID_TEXTBOX_PASSWORD);
#ifdef KINDLE_FOR_TOUCH	
	m_btnOK.Initialize(IDOK,  StringManager::GetStringById(ACCOUNT_LOGIN), GetWindowFontSize(UIModalDialogButtonIndex), SPtr<ITpImage>());
    m_btnCancel.Initialize(IDCANCEL,  StringManager::GetStringById(ACTION_CANCEL), GetWindowFontSize(UIModalDialogButtonIndex), SPtr<ITpImage>());

	m_btnXiaoMiLogin.Initialize(ID_BTN_ACCOUNT_XIAOMI_LOGIN,  StringManager::GetStringById(TOUCH_SYSTEMSETTING_LOGIN_XIAOMI_ACCOUNT), GetWindowFontSize(UIModalDialogButtonIndex), SPtr<ITpImage>());
    m_btnXiaoMiLogin.ShowBorder(false);

    m_btnFindPwd.Initialize(ID_BTN_ACCOUNT_FINDPASSWORD,  StringManager::GetStringById(ACCOUNT_FORGETPASSWORD), GetWindowFontSize(UIModalDialogButtonIndex), SPtr<ITpImage>());
    m_btnFindPwd.ShowBorder(false);
#else
	m_btnOK.Initialize(IDOK,  StringManager::GetStringById(ACCOUNT_LOGIN),
		KEY_RESERVED, GetWindowFontSize(UIModalDialogButtonIndex),
		ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnCancel.Initialize(IDCANCEL,  StringManager::GetStringById(ACTION_CANCEL),
		KEY_RESERVED, GetWindowFontSize(UIModalDialogButtonIndex),
		ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));

	m_btnXiaoMiLogin.Initialize(ID_BTN_ACCOUNT_XIAOMI_LOGIN,
		StringManager::GetStringById(TOUCH_SYSTEMSETTING_LOGIN_XIAOMI_ACCOUNT), 
		KEY_RESERVED,
		GetWindowFontSize(UIModalDialogButtonIndex),
		ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnFindPwd.Initialize(ID_BTN_ACCOUNT_FINDPASSWORD,
		StringManager::GetStringById(ACCOUNT_FORGETPASSWORD),
		KEY_RESERVED,
		GetWindowFontSize(UIModalDialogButtonIndex),
		ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
    m_textTips.SetText(StringManager::GetPCSTRById(ACCOUNT_EMPTY_MAIL_PWD));
    m_textTips.SetFontSize(GetWindowFontSize(UIModalDialogButtonIndex));
    m_textTips.SetReversedColor(true);
	m_textTips.SetVisible(false);

    AppendChild(&m_editAccount);
    AppendChild(&m_editPassword);
    AppendChild(&m_textTips);
    AppendChild(&m_btnFindPwd);
	AppendChild(&m_btnXiaoMiLogin);
    m_btnGroup.AddButton(&m_btnCancel);
    m_btnGroup.AddButton(&m_btnOK);
    m_btnGroup.SetBottomLinePixel(GetWindowMetrics(UIButtonGroupBottomLinePixelIndex));
    bool hasAccount = !emailAccount.empty();

    if (m_windowSizer)
    {
        m_editAccount.SetMinHeight(GetWindowMetrics(UIModalDialogEditBoxIndex));
        m_editPassword.SetMinHeight(GetWindowMetrics(UIModalDialogEditBoxIndex));
        m_windowSizer->Add(&m_editAccount, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)));
        m_windowSizer->AddSpacer(GetWindowMetrics(UIAccountLoginDialogMarginIndex));
        m_windowSizer->Add(&m_editPassword, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)));
        m_windowSizer->AddSpacer(GetWindowMetrics(UIAccountLoginDialogMarginIndex)/2);

        UISizer* middleSizer = new UIBoxSizer(dkHORIZONTAL);
        middleSizer->Add(&m_textTips, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        middleSizer->AddStretchSpacer();
        middleSizer->Add(&m_btnFindPwd, UISizerFlags().Border(dkLEFT, GetWindowMetrics(UIPixelValue8Index)).Align(dkALIGN_RIGHT | dkALIGN_CENTER_VERTICAL).ReserveSpaceEvenIfHidden());
        m_windowSizer->Add(middleSizer, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)).Expand());

		UISizer* xiaomiSizer = new UIBoxSizer(dkHORIZONTAL);
		xiaomiSizer->SetMinHeight(m_btnGroup.GetMinHeight() - 10);
        xiaomiSizer->Add(&m_btnXiaoMiLogin, UISizerFlags(1).Center());
		m_windowSizer->AddSpacer(GetWindowMetrics(UIAccountLoginDialogMarginIndex)/2);
        m_windowSizer->Add(&m_btnGroup, UISizerFlags().Expand());
		m_windowSizer->Add(xiaomiSizer, UISizerFlags(1).Expand().Border(dkALL, 5));
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

void UIAccountLoginDialog::MoveWindow(int _iLeft, int _iTop, int _iWidth, int _iHeight)
{
    UIModalDlg::MoveWindow(_iLeft, _iTop, _iWidth, _iHeight);
    Layout();
}


void UIAccountLoginDialog::Dispose ()
{
    OnDispose (this->m_bIsDisposed);
}

void UIAccountLoginDialog::OnDispose (BOOL bDisposed)
{
    if (bDisposed)
    {
        return;
    }

    this->m_bIsDisposed = TRUE;
}

void UIAccountLoginDialog::SetInitText(CString text, BOOL bPwdLabel)
{
    if(!text.Length())
    {
        return;
    }
    if(bPwdLabel)
    {
        m_editPassword.SetTextUTF8(text.GetBuffer());
    }
    else
    {
        m_editAccount.SetTextUTF8(text.GetBuffer());
    }
}

void UIAccountLoginDialog::Login(const char* account, const char* password)
{
    CDisplay::CacheDisabler cache;
    UIUtility::SetScreenTips(StringManager::GetPCSTRById(ACCOUNT_LOGIN_LOGINING), 80000);

    m_loginAction = LA_LOGINING;
    CAccountManager::GetInstance()->LoginWithPasswordAsync(account, password);
    m_btnOK.SetEnable(false);
    m_btnCancel.SetEnable(false);
}

void UIAccountLoginDialog::OnDuoKanLoginClick()
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
        Login(account.c_str(), password.c_str());
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

void UIAccountLoginDialog::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
    case IDOK:
        OnDuoKanLoginClick();
        break;
    case ID_TEXTBOX_PASSWORD:
        OnDuoKanLoginClick();
        break;
    case IDCANCEL:
        EndDialog(IDCANCEL); 
        break;
    case ID_BTN_ACCOUNT_FINDPASSWORD:
        //This is a special return value!!!! DO NOT CHANGE IT!!!
        {
            m_loginAction = LA_FORGETTING;
            EndDialog(IDCANCEL); 
        }
        break;
	case ID_BTN_ACCOUNT_XIAOMI_LOGIN:
		{
            m_loginAction = LA_LOGINING;
            EndDialog(IDCANCEL); 
			CAccountManager::GetInstance()->LoginXiaomiAccount(false);
		}
		break;
    default:
        break;
    }
}

void UIAccountLoginDialog::SetTips(LPCSTR tips)
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

bool UIAccountLoginDialog::OnLoginEvent(const EventArgs& args)
{
    if (m_loginAction != LA_REGISTERING && m_loginAction != LA_LOGINING)
    {
        return false;
    }

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
    else if (BSE_DUOKAN_ACCOUNT_LOGIN_CLOSED == accountEventArgs.errorCode)
    {
        EndDialog(IDCANCEL);
        return true;
    }
    else
    {
        DebugPrintf(DLC_DIAGNOSTIC, "login error: %d, %s, device id: %s, token: %s",
                accountEventArgs.errorCode, accountEventArgs.errorMessage.c_str(),
                DeviceInfo::GetDeviceID().c_str(),
                CAccountManager::GetInstance()->GetTokenFromFile().c_str());
        SetTips(accountEventArgs.errorMessage.c_str());
        m_loginAction = LA_NONE;
        m_btnOK.SetEnable(true);
        m_btnCancel.SetEnable(true);
    }
    return true;
}

void UIAccountLoginDialog::EndDialog(INT32 iEndCode,BOOL fRepaintIt)
{
    CDisplay* pDisplay = CDisplay::GetDisplay();
    if (pDisplay)
    {
        pDisplay->ClearScreen();
    }

    UIDialog::EndDialog(iEndCode, fRepaintIt);
}

bool UIAccountLoginDialog::ForgettingPassword() const
{
    return LA_FORGETTING == m_loginAction;
}

string UIAccountLoginDialog::GetUserAccount()
{
    return m_editAccount.GetTextUTF8();
}

