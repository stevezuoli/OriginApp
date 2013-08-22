#include "I18n/StringManager.h"
#include "CommonUI/UIWifiPasswordDlg.h"
#include "CommonUI/UIIMEManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Framework/CDisplay.h"

using namespace WindowsMetrics;

UIWifiPasswordDlg::UIWifiPasswordDlg(UIContainer* pParent, LPCSTR title, bool showUserName) :
    UIModalDlg(pParent, title) ,
    m_bIsClearText(true)
{
    m_textIdentity.SetVisible(showUserName);
    m_inputIdentity.SetVisible(showUserName);
    this->InitUI();
}

UIWifiPasswordDlg::~UIWifiPasswordDlg()
{
}

void UIWifiPasswordDlg::InitUI()
{
    const int defaultFontSize = GetWindowFontSize(UIModalDialogButtonIndex);
    m_textIdentity.SetText(StringManager::GetPCSTRById(WIFI_IDENTIFY));
    m_textIdentity.SetFontSize(defaultFontSize);
    m_textIdentity.SetAlign(ALIGN_LEFT);
    m_inputIdentity.SetTextBoxStyle(ENGLISH_NORMAL);

    m_textPassword.SetText(StringManager::GetPCSTRById(PASSWORD_WITH_COLON));
    m_textPassword.SetFontSize(defaultFontSize);
    m_textPassword.SetAlign(ALIGN_LEFT);
    
#ifdef KINDLE_FOR_TOUCH
    m_btnConfirm.Initialize(IDOK, StringManager::GetPCSTRById(WIFI_CONNECT), defaultFontSize);
    m_btnCancel.Initialize(IDCANCEL, StringManager::GetPCSTRById(TOUCH_ACTION_CANCEL), defaultFontSize);
#else
    m_btnConfirm.Initialize(IDOK, StringManager::GetPCSTRById(WIFI_CONNECT), KEY_RESERVED, defaultFontSize, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnCancel.Initialize(IDCANCEL, StringManager::GetPCSTRById(TOUCH_ACTION_CANCEL), KEY_RESERVED, defaultFontSize, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
    m_inputPassword.SetTextBoxStyle(ENGLISH_NORMAL);

    AppendChild(&m_textIdentity);
    AppendChild(&m_inputIdentity);
    AppendChild(&m_textPassword);
    AppendChild(&m_inputPassword);
    m_btnGroup.AddButton(&m_btnConfirm);
    m_btnGroup.AddButton(&m_btnCancel);

    if (m_windowSizer)
    {
        const int minHeight = GetWindowMetrics(UIModalDialogEditBoxIndex);
        const int inputWidth = GetWindowMetrics(UIPixelValue400Index);
        const int horizontalMargin = GetWindowMetrics(UIModalDialogHorizonMarginIndex);
        const int elemSpacing = GetWindowMetrics(UIPixelValue16Index);
        m_inputIdentity.SetMinSize(dkSize(inputWidth, minHeight));
        m_inputPassword.SetMinSize(dkSize(inputWidth, minHeight));
#ifdef KINDLE_FOR_TOUCH
        m_btnCancel.SetMinHeight(minHeight);
        m_btnConfirm.SetMinHeight(minHeight);
#endif
        UISizer* pUserNameSizer = new UIBoxSizer(dkHORIZONTAL);
        if (pUserNameSizer)
        {
            pUserNameSizer->Add(&m_textIdentity, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            pUserNameSizer->AddStretchSpacer();
            pUserNameSizer->Add(&m_inputIdentity, UISizerFlags(1).Expand());
            m_windowSizer->Add(pUserNameSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin).Border(dkTOP, elemSpacing));
        }

        UISizer* pPWDSizer = new UIBoxSizer(dkHORIZONTAL);
        if (pPWDSizer)
        {
            pPWDSizer->Add(&m_textPassword, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            pPWDSizer->AddStretchSpacer();
            pPWDSizer->Add(&m_inputPassword, UISizerFlags(1).Expand());
            m_windowSizer->Add(pPWDSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin).Border(dkTOP, elemSpacing));
        }
        m_windowSizer->AddSpacer(GetWindowMetrics(UIPixelValue23Index));
        m_windowSizer->Add(&m_btnGroup, UISizerFlags().Expand());
    }

    if (m_windowSizer)
    {
        const int width = GetWindowMetrics(UIModalDialogWidthIndex);
        const int height = m_windowSizer->GetMinSize().GetHeight();
        MakeCenterAboveIME(width, height);
        Layout();
    }

    UITextBox* pFocusedWnd = m_inputIdentity.IsVisible() ? &m_inputIdentity : &m_inputPassword;
    SetChildWindowFocus(GetChildIndex(pFocusedWnd), false);
    UIIME* pIME = UIIMEManager::GetIME(IUIIME_ENGLISH_LOWER, pFocusedWnd);
    if (pIME)
    {
        pIME->SetShowDelay(true);
    }
}

std::string UIWifiPasswordDlg::GetIdentity()
{
    return m_inputIdentity.GetTextUTF8();
}

std::string UIWifiPasswordDlg::GetPassWord()
{
    return m_inputPassword.GetTextUTF8();
}

void UIWifiPasswordDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId) {
    case IDOK:
        EndDialog(1);
        break;
    case IDCANCEL:
        EndDialog(0);
        break;
    }
}

