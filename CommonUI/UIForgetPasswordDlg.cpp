#include "CommonUI/UIForgetPasswordDlg.h"
#include "CommonUI/UIIMEManager.h"
#include "Common/CAccountManager.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"
#include "Common/WindowsMetrics.h"
#include <tr1/functional>

using namespace dk::account;
using namespace WindowsMetrics;

UIForgetPasswordDlg::UIForgetPasswordDlg(UIContainer* pParent, const char* forgetEmail)
	: UIModalDlg(pParent, StringManager::GetPCSTRById(ACCOUNT_FORGETPASSWORD2))
    , m_pSuccessSizer(NULL)
    , m_pSendPwdSizer(NULL)
    , m_loginAction(LA_NONE)
{
	m_forgetEmail = forgetEmail;
	InitUI();
    SubscribeMessageEvent(CAccountManager::EventAccount,
            *CAccountManager::GetInstance(),
            std::tr1::bind(
                std::tr1::mem_fn(&UIForgetPasswordDlg::OnLoginEvent),
                this,
                std::tr1::placeholders::_1));
}

UIForgetPasswordDlg::~UIForgetPasswordDlg()
{
}

void UIForgetPasswordDlg::InitUI()
{
	m_editUserMail.SetTextUTF8(m_forgetEmail.c_str(), 0);
    m_editUserMail.SetFontSize(GetWindowFontSize(UIModalDialogButtonIndex)); 
    m_editUserMail.SetTextBoxStyle(ENGLISH_NORMAL);
    m_editUserMail.SetTipUTF8(StringManager::GetStringById(TOUCH_SYSTEMSETTING_INPUT_MAILADDRESS));
#ifdef KINDLE_FOR_TOUCH
    m_btnCancel.Initialize(IDCANCEL,  StringManager::GetStringById(ACTION_CANCEL), GetWindowFontSize(UIModalDialogButtonIndex), SPtr<ITpImage>());
    m_btnSendPwd.Initialize(IDOK,  StringManager::GetStringById(TOUCH_SYSTEMSETTING_SEND_PASSWORD), GetWindowFontSize(UIModalDialogButtonIndex), SPtr<ITpImage>());
    m_btnClose.Initialize(IDCANCEL, StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_CLOSE), GetWindowFontSize(UIModalDialogButtonIndex), SPtr<ITpImage>());
#else
    m_btnCancel.Initialize(IDCANCEL,  StringManager::GetStringById(ACTION_CANCEL), KEY_RESERVED, GetWindowFontSize(UIModalDialogButtonIndex), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnSendPwd.Initialize(IDOK,  StringManager::GetStringById(TOUCH_SYSTEMSETTING_SEND_PASSWORD), KEY_RESERVED, GetWindowFontSize(UIModalDialogButtonIndex), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnClose.Initialize(IDCANCEL, StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_CLOSE), KEY_RESERVED, GetWindowFontSize(UIModalDialogButtonIndex), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
	m_textTips.SetReversedColor(true);

	m_textMailHasPosted.SetText(StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_RESET_PWD_EMAIL));
	m_textMailHasPosted.SetFontSize(GetWindowFontSize(UIForgetPasswordTextIndex));

	m_textContactMail.SetText(StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_CONTACT_EMAIL));
	m_textContactMail.SetFontSize(GetWindowFontSize(UIModalDialogButtonIndex));
	m_textContactMail.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));

	AppendChild(&m_editUserMail);
	AppendChild(&m_textTips);
	AppendChild(&m_textMailHasPosted);
	AppendChild(&m_textContactMail);
    m_btnGroup.AddButton(&m_btnCancel);
    m_btnGroup.AddButton(&m_btnSendPwd);
    m_btnGroup.AddButton(&m_btnClose);
    if (m_windowSizer)
    {
        const int spacer = GetWindowMetrics(UIForgetPasswordDialogMarginIndex);
        m_editUserMail.SetMinHeight(GetWindowMetrics(UIModalDialogEditBoxIndex));
        m_pSendPwdSizer = new UIBoxSizer(dkVERTICAL);
        if (m_pSendPwdSizer)
        {
            m_pSendPwdSizer->Add(&m_editUserMail, UISizerFlags().Expand());
            m_pSendPwdSizer->AddSpacer(spacer);
            m_pSendPwdSizer->Add(&m_textTips, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pSendPwdSizer->AddSpacer(spacer);
            m_windowSizer->Add(m_pSendPwdSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)));
        }

        m_pSuccessSizer = new UIBoxSizer(dkVERTICAL);
        if (m_pSuccessSizer)
        {
            m_pSuccessSizer->Add(&m_textMailHasPosted, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
            m_pSuccessSizer->AddSpacer(spacer);
            m_pSuccessSizer->Add(&m_textContactMail, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
            m_pSuccessSizer->AddSpacer(spacer<<1);
            m_windowSizer->Add(m_pSuccessSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)));
        }
        m_windowSizer->Add(&m_btnGroup, UISizerFlags().Expand());
        int width = GetWindowMetrics(UIModalDialogWidthIndex);
        int height = m_windowSizer->GetMinSize().GetHeight();
        MakeCenterAboveIME(width, height);
        ShowSuccessWindows(false);
    }
    SetChildWindowFocus(GetChildIndex(&m_editUserMail), false);
    UIIME *pIME = UIIMEManager::GetIME(IUIIME_ENGLISH_LOWER, &m_editUserMail);
    if (pIME)
    {
        pIME->SetShowDelay(true);
    }
}

void UIForgetPasswordDlg::OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam)
{
	switch (dwCmdId) 
	{
	case IDCANCEL:
		EndDialog(0);
		break;
	case IDOK:
		ReGetPassWord();
		break;
	}
}

void UIForgetPasswordDlg::ReGetPassWord()
{
	CDisplay::CacheDisabler cache;
	m_textTips.SetVisible(false);
	m_textTips.UpdateWindow();
	std::string userMail = m_editUserMail.GetTextUTF8();
	if (!userMail.empty())
	{
		CAccountManager* pManager = CAccountManager::GetInstance();
		//TODO
		 pManager->ForgetPasswordAsync(userMail.c_str());
         m_loginAction = LA_FORGETTING_PASSWORD;
         m_btnSendPwd.SetEnable(false);
         UpdateWindow();

	}
	else
	{
		m_editUserMail.SetTipUTF8(StringManager::GetPCSTRById(EMAILCANNOTEMPTY),0);
		m_editUserMail.SetFocus(TRUE);
		m_editUserMail.UpdateWindow();
	}
}

void UIForgetPasswordDlg::ShowSuccessWindows(bool showSuccess)
{
    m_btnCancel.SetVisible(!showSuccess);
    m_btnSendPwd.SetVisible(!showSuccess);
    m_btnClose.SetVisible(showSuccess);
#ifndef KINDLE_FOR_TOUCH
    if(showSuccess)
    {
        m_btnClose.SetFocus(true);
    }
#endif
    if (m_pSuccessSizer)
    {
        m_pSuccessSizer->Show(showSuccess);
    }
    if (m_pSendPwdSizer)
    {
        m_pSendPwdSizer->Show(!showSuccess);
    }
    int width = GetWindowMetrics(UIModalDialogWidthIndex);
    int height = m_windowSizer->GetMinSize().GetHeight();
    MakeCenterAboveIME(width, height);
    Layout();
    m_textTips.SetVisible(false);
	UpdateWindow();
}

bool UIForgetPasswordDlg::OnLoginEvent(const EventArgs& args)
{
    if (m_loginAction != LA_FORGETTING_PASSWORD)
    {
        return false;
    }

    const AccountEventArgs& accountEventArgs = 
        (const AccountEventArgs&)args;
    if (AccountEventArgs::LS_RESEND_PASSWORD_FAILED != accountEventArgs.logStatus
            && AccountEventArgs::LS_RESEND_PASSWORD_SUCCEEDED != accountEventArgs.logStatus)
    {
        return false;
    }


    if (0 == accountEventArgs.errorCode
            && AccountEventArgs::LS_RESEND_PASSWORD_SUCCEEDED == accountEventArgs.logStatus)
    {
        ShowSuccessWindows();
        m_loginAction = LA_NONE;
        m_btnSendPwd.SetEnable(true);
    }
    else
    {
        m_textTips.SetText(accountEventArgs.errorMessage.c_str());
        m_textTips.SetVisible(true);
        m_btnSendPwd.SetEnable(true);
        Layout();
        UpdateWindow();
    }
    return true;
}
