#include "CommonUI/UIWeiboBindDlg.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIMessageBox.h"
#include "Common/Defs.h"
#include "Common/SystemSetting_DK.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIIMEManager.h"
#include "CommonUI/UIUtility.h"
#include "Weibo/WeiboFactory.h"
#include "I18n/StringManager.h"
using namespace WindowsMetrics;
using namespace dk::weibo;

UIWeiboBindDlg::UIWeiboBindDlg(UIContainer* pParent)
    : UIDialog(pParent)
#ifdef KINDLE_FOR_TOUCH
    , m_backButton(this)
#else
    , m_backButton(this, IDSTATIC)
#endif
    , m_topSizer(NULL)
{

    SubscribeMessageEvent(SinaWeiboMethodImpl::EventOauthAchieved, 
        *(WeiboFactory::GetSinaWeibo()->GetMethod()),
        std::tr1::bind(
        std::tr1::mem_fn(&UIWeiboBindDlg::OnWeiboLoginUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    SubscribeMessageEvent(UIAbstractTextEdit::EventTextEditChange, 
        m_usernameBox,
        std::tr1::bind(
        std::tr1::mem_fn(&UIWeiboBindDlg::OnInputChanges),
        this,
        std::tr1::placeholders::_1)
        );
    SubscribeMessageEvent(UIAbstractTextEdit::EventTextEditChange, 
        m_passwordBox,
        std::tr1::bind(
        std::tr1::mem_fn(&UIWeiboBindDlg::OnInputChanges),
        this,
        std::tr1::placeholders::_1)
        );

    InitUI();
}

void UIWeiboBindDlg::LoginWeibo()
{
    string userName = m_usernameBox.GetTextUTF8();
    string password = m_passwordBox.GetTextUTF8();
    if (userName.empty() || password.empty())
    {
        UIMessageBox msgBox(this, StringManager::GetPCSTRById(ACCOUNT_EMPTYPARAMETER), MB_OK);
        msgBox.DoModal();
        UpdateWindow();
        return;
    }
    m_loginButton.SetEnable(false);
    UpdateWindow();
    WeiboFactory::GetSinaWeibo()->GetMethod()->Oauth2(userName.c_str(), password.c_str());
}

bool UIWeiboBindDlg::OnInputChanges(const EventArgs& args)
{
    bool enable = m_loginButton.IsEnable();
    if (m_usernameBox.GetTextUTF8().empty() || m_passwordBox.GetTextUTF8().empty())
    {
        m_loginButton.SetEnable(false);
    }
    else
    {
        m_loginButton.SetEnable(true);
    }
    if (enable != m_loginButton.IsEnable())
    {
        m_loginButton.UpdateWindow();
    }
    return true;
}

bool UIWeiboBindDlg::OnWeiboLoginUpdate(const EventArgs& args)
{
    const WeiboResultArgs& weiboResultArgs = (const WeiboResultArgs&)args;
    m_loginButton.SetEnable(true);
    if (weiboResultArgs.mResultType == WeiboResultArgs::ERT_CORRECT)
    {
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(BIND_SINAWEIBO_SUCCESS));
        SystemSettingInfo::GetInstance()->SetSinaWeiboUserMail(m_usernameBox.GetTextUTF8().c_str());
        EndDialog(0);
        return true;
    }
    else if (weiboResultArgs.mResultType == WeiboResultArgs::ERT_NETWORKISSUE)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "UIWeiboBindDlg::OnWeiboLoginUpdate, ACCOUNT_NETWORKISSUE");
        UIMessageBox msgBox(this, StringManager::GetPCSTRById(ACCOUNT_NETWORKISSUE), MB_OK);
        msgBox.DoModal();
    }
    else if (weiboResultArgs.mResultType == WeiboResultArgs::ERT_ERROR)
    {
        ParsingApiErrorSPtr apiError = std::tr1::dynamic_pointer_cast<ParsingApiError>(weiboResultArgs.mParsingInfo);
        if (apiError)
        {
            DebugPrintf(DLC_DIAGNOSTIC, "UIWeiboBindDlg::OnWeiboLoginUpdate, %s", apiError->GetErrorMessage().c_str());
            UIMessageBox msgBox(this, apiError->GetErrorMessage().c_str(), MB_OK);
            msgBox.DoModal();
        }
    }
    UpdateWindow();

    return true;
}

void UIWeiboBindDlg::InitUI()
{
#ifdef KINDLE_FOR_TOUCH
    m_loginButton.Initialize(ID_BTN_SINAWEIBO_LOGIN, StringManager::GetPCSTRById(ACCOUNT_LOGIN), GetWindowFontSize(FontSize22Index));
    m_loginButton.SetBackground(ImageManager::GetImage(IMAGE_TOUCH_WEIBO_SHARE));
#else
    m_loginButton.Initialize(ID_BTN_SINAWEIBO_LOGIN, StringManager::GetPCSTRById(ACCOUNT_LOGIN), KEY_RESERVED, GetWindowFontSize(FontSize22Index), SPtr<ITpImage>());
#endif
    m_loginButton.SetEnable(false);
    m_titleBindLabel.SetText(StringManager::GetPCSTRById(BIND_SINAWEIBO));
    m_titleBindLabel.SetAlign(ALIGN_CENTER);
    m_titleBindLabel.SetFontSize(GetWindowFontSize(FontSize24Index));
    string sinaWeiboUserMail = SystemSettingInfo::GetInstance()->GetSinaWeiboUserMail();
    if (sinaWeiboUserMail.empty())
    {
        m_usernameBox.SetTipUTF8(StringManager::GetPCSTRById(SINAWEIBO_USERNAME_TYPE_TIPS));
    }
    else
    {
        m_usernameBox.SetTextUTF8(sinaWeiboUserMail.c_str());
    }
    m_passwordBox.SetTipUTF8(StringManager::GetPCSTRById(SINAWEIBO_PASSWORD_TYPE_TIPS));
    m_passwordBox.SetId(ID_BTN_SINAWEIBO_LOGIN);
    m_passwordBox.SetEchoMode(UITextBox::PasswordEchoOnEdit);
    m_usernameBox.SetTextBoxStyle(ENGLISH_NORMAL);
    m_passwordBox.SetTextBoxStyle(ENGLISH_NORMAL);
    m_logoDuoKan.SetAutoSize(true);
    m_logoSina.SetAutoSize(true);
    m_imgConnectd.SetAutoSize(true);
#ifdef KINDLE_FOR_TOUCH
    m_logoDuoKan.SetImage(ImageManager::GetImage(IMAGE_TOUCH_DUOKAN_LOGO));
    m_logoSina.SetImage(ImageManager::GetImage(IMAGE_TOUCH_SINA_LOGO));
    m_imgConnectd.SetImage(ImageManager::GetImage(IMAGE_TOUCH_WEIBO_GO));
#else
    m_logoDuoKan.SetImage(ImageManager::GetImage(IMAGE_DUOKAN_LOGO));
    m_logoSina.SetImage(ImageManager::GetImage(IMAGE_SINA_LOGO));
    m_imgConnectd.SetImage(ImageManager::GetImage(IMAGE_WEIBO_GO));
#endif
    SetSize(dkSize(GetScreenWidth(), GetScreenHeight()));
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        if (mainSizer)
        {
            m_usernameBox.SetMinHeight(GetWindowMetrics(UIWeiboBindDlgEditBoxHeightIndex));
            m_passwordBox.SetMinHeight(GetWindowMetrics(UIWeiboBindDlgEditBoxHeightIndex));
            mainSizer->Add(&m_titleBar);
            m_topSizer = new UIBoxSizer(dkHORIZONTAL);
            if (m_topSizer)
            {
                m_topSizer->Add(&m_backButton, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
                m_topSizer->Add(&m_titleBindLabel, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
#ifdef KINDLE_FOR_TOUCH
                m_topSizer->AddSpacer(GetWindowMetrics(UIBackButtonWidthIndex));
                m_topSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));
                mainSizer->Add(m_topSizer, UISizerFlags().Expand());
#else
                m_topSizer->AddSpacer(m_backButton.GetMinWidth());
                mainSizer->Add(m_topSizer, 
                    UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
#endif
            }
            mainSizer->AddSpacer(2);
            UISizer* dkToSinaSizer = new UIBoxSizer(dkHORIZONTAL);
            if (dkToSinaSizer)
            {
                dkToSinaSizer->AddStretchSpacer();
                dkToSinaSizer->Add(&m_logoDuoKan, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
                dkToSinaSizer->Add(&m_imgConnectd, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
                dkToSinaSizer->Add(&m_logoSina, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, GetWindowMetrics(UIPixelValue10Index)));
                dkToSinaSizer->AddStretchSpacer();
                mainSizer->Add(dkToSinaSizer, UISizerFlags().Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIWeiboBindDlgLogoVPaddingIndex)).Expand());
            }
            mainSizer->Add(&m_usernameBox, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)).Expand());
            mainSizer->AddSpacer(GetWindowMetrics(UIWeiboBindDlgButtonTopPaddingIndex));
            mainSizer->Add(&m_passwordBox, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)).Expand());
            mainSizer->AddSpacer(GetWindowMetrics(UIWeiboBindDlgButtonBottomPaddingIndex));
#ifdef KINDLE_FOR_TOUCH
            mainSizer->Add(&m_loginButton, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)).Expand());
#else
            mainSizer->Add(&m_loginButton, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)).Align(dkALIGN_RIGHT));
#endif
            SetSizer(mainSizer);
            Layout();
        }
    }
    AppendChild(&m_titleBar);
    AppendChild(&m_titleBindLabel);
    AppendChild(&m_usernameBox);
    AppendChild(&m_passwordBox);
    AppendChild(&m_loginButton);
    AppendChild(&m_logoDuoKan);
    AppendChild(&m_logoSina);
    AppendChild(&m_imgConnectd);
#ifdef KINDLE_FOR_TOUCH
    AppendChild(&m_backButton);
#endif
    UIAbstractTextEdit* imeTarget = (strlen(SystemSettingInfo::GetInstance()->GetSinaWeiboUserMail()) == 0) ? &m_usernameBox : &m_passwordBox;
    SetChildWindowFocus(GetChildIndex(imeTarget), false);
    UIIME *pIME = UIIMEManager::GetIME(IUIIME_ENGLISH_LOWER, imeTarget);
    if (pIME)
    {
        pIME->SetShowDelay(true);
    }
}

void UIWeiboBindDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    if (dwCmdId == ID_BTN_SINAWEIBO_LOGIN)
    {
        LoginWeibo();
    }
#ifdef KINDLE_FOR_TOUCH
    else if (dwCmdId == ID_BTN_TOUCH_BACK)
    {
        EndDialog(0);
    }
#endif
}

HRESULT UIWeiboBindDlg::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
#ifdef KINDLE_FOR_TOUCH
    grf.EraserBackGround(ColorManager::knWhite);
    dkSize topSize = m_topSizer->GetSize();
    dkPoint topPoint = m_topSizer->GetPosition();
    int horizonMargin = GetWindowMetrics(UIHorizonMarginIndex);
    hr = grf.DrawLine(topPoint.x + horizonMargin,
            topPoint.y + topSize.GetHeight(),
            topSize.GetWidth() - horizonMargin * 2, 
            2,
            SOLID_STROKE);
#else
    grf.EraserBackGround();
    int horizonMargin = GetWindowMetrics(UIHorizonMarginIndex);
    hr = grf.DrawLine(horizonMargin, m_titleBindLabel.GetY() + m_titleBindLabel.GetHeight() + GetWindowMetrics(UIWeiboSharePageTitleVPaddingIndex),
            m_iWidth - (horizonMargin << 1), 2, SOLID_STROKE);
#endif
    return hr;
}
