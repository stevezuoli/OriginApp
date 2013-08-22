#include "CommonUI/UIWeiboSharePage.h"
#include "CommonUI/UIWeiboBindDlg.h"
#include <unistd.h>
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "GUI/UIMessageBox.h"
#include "GUI/CTpGraphics.h"
#include "Common/Defs.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "Weibo/WeiboFactory.h"
#include "I18n/StringManager.h"
#include "Utility/StringUtil.h"
#include "Utility/EncodeUtil.h"
#include "Utility/FileSystem.h"
using namespace WindowsMetrics;
using namespace dk::weibo;
using namespace dk::utility;

UIWeiboSharePage::UIWeiboSharePage(const string& sharedContent)
    : m_weiboSharedContent(sharedContent)
    , m_autoDeletePictureFile(false)
#ifdef KINDLE_FOR_TOUCH
    , m_backButton(this)
#else
    , m_backButton(this, IDSTATIC)
#endif
	, m_pictureSizer(NULL)
	, m_sharePicture(this)
{
    SubscribeMessageEvent(SinaWeiboMethodImpl::EventPostUpdateAchieved, 
        *(WeiboFactory::GetSinaWeibo()->GetMethod()),
        std::tr1::bind(
        std::tr1::mem_fn(&UIWeiboSharePage::OnWeiboShareUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    EventListener::SubscribeEvent(
        UIAbstractTextEdit::EventTextEditChange,
        m_weiboContentEdit, 
        std::tr1::bind(
        std::tr1::mem_fn(&UIWeiboSharePage::OnContentEditChanged),
        this,
        std::tr1::placeholders::_1));

    InitUI();
}

UIWeiboSharePage::~UIWeiboSharePage()
{
    if (m_autoDeletePictureFile && !m_weiboSharedPicturePath.empty())
    {
        FileSystem::DeleteFile(m_weiboSharedPicturePath.c_str());
    }
}

bool UIWeiboSharePage::InitUI()
{
	m_sharePicture.ShowBorder(true);
    m_sharePicture.SetEnable(false);
	m_userNameLabel.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
	m_wordCountTipsLabel.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
	int contentHeight = GetWindowMetrics(UIWeiboSharePageContentHeightIndex);
    m_weiboContentEdit.SetTextUTF8(m_weiboSharedContent.c_str());
    m_weiboContentEdit.SetFontSize(GetWindowFontSize(FontSize22Index));
#ifdef KINDLE_FOR_TOUCH
    m_weiboSharedButton.Initialize(ID_BTN_SINAWEIBO_SHARE, StringManager::GetPCSTRById(SHARE), GetWindowFontSize(FontSize22Index));
    m_weiboSharedButton.SetBackground(ImageManager::GetImage(IMAGE_TOUCH_WEIBO_SHARE));
#else
    m_weiboSharedButton.Initialize(ID_BTN_SINAWEIBO_SHARE, StringManager::GetPCSTRById(SHARE), KEY_RESERVED, GetWindowFontSize(FontSize22Index), SPtr<ITpImage>());
#endif
    string userName = WeiboFactory::GetSinaWeibo()->GetUserName();
    if (userName.empty())
    {
        m_userNameLabel.SetText(StringManager::GetPCSTRById(NOBIND));
    }
    else
    {
        m_userNameLabel.SetText(userName.c_str());
    }
    m_titleShareLabel.SetAlign(ALIGN_CENTER);
    m_titleShareLabel.SetFontSize(GetWindowFontSize(FontSize24Index));
    m_titleShareLabel.SetText(StringManager::GetPCSTRById(SHARE_BOOK_TO_SINAWEIBO));
    m_wordCountTipsLabel.SetAlign(ALIGN_RIGHT);
    const int weiboContentMaxLen = WeiboFactory::GetSinaWeibo()->GetShortWeiboMaxLen();
    char buf[128] = {0};
    int curDataLen = m_weiboContentEdit.GetCurDataLen();
    snprintf(buf, DK_DIM(buf), 
            StringManager::GetPCSTRById(TYPE_WORD_COUNTS),
            weiboContentMaxLen - curDataLen);
    m_wordCountTipsLabel.SetText(buf);

    if (!m_windowSizer)
    {
        SetSize(dkSize(GetScreenWidth(),GetScreenHeight()));
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        if (mainSizer)
        {
            m_weiboContentEdit.SetMinHeight(contentHeight);
            mainSizer->Add(&m_titleBar);
            m_topSizer = new UIBoxSizer(dkHORIZONTAL);
            if (m_topSizer)
            {
                m_topSizer->Add(&m_backButton, 
                    UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
                m_topSizer->Add(&m_titleShareLabel,
                    UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
#ifdef KINDLE_FOR_TOUCH
                m_topSizer->AddSpacer(GetWindowMetrics(UIBackButtonWidthIndex));
                mainSizer->Add(m_topSizer, UISizerFlags().Expand());
                m_topSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));
#else
                m_topSizer->AddSpacer(m_backButton.GetMinWidth());
                mainSizer->Add(m_topSizer,
                    UISizerFlags().
                    Expand().
                    Border(dkLEFT | dkRIGHT,
                    GetWindowMetrics(UIHorizonMarginIndex)));
#endif
            }
            mainSizer->AddSpacer(2);
            UISizer* userNameSizer = new UIBoxSizer(dkHORIZONTAL);
            if (userNameSizer)
            {
                userNameSizer->Add(&m_userNameLabel);
                userNameSizer->Add(&m_wordCountTipsLabel,1);
                mainSizer->Add(userNameSizer, UISizerFlags().
                        Expand().
                        Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)).
                        Border(dkTOP, GetWindowMetrics(UIWeiboSharePageContentEditVPaddingIndex)));
            }

			UISizer* shareContent = new UIBoxSizer(dkHORIZONTAL);
			shareContent->Add(&m_weiboContentEdit, UISizerFlags(1).Expand());

			m_pictureSizer = new UIBoxSizer(dkHORIZONTAL);
			m_pictureSizer->Show(false);
			m_sharePicture.SetMaxSize(dkSize(GetWindowMetrics(UIWeiboSharePagePictureWidthIndex), contentHeight));
			m_pictureSizer->AddStretchSpacer();
			m_pictureSizer->Add(&m_sharePicture, UISizerFlags().Align(dkALIGN_BOTTOM | dkALIGN_CENTER_HORIZONTAL));
			m_pictureSizer->AddStretchSpacer();
			shareContent->Add(m_pictureSizer, UISizerFlags().Align(dkALIGN_RIGHT));
            mainSizer->Add(shareContent, UISizerFlags().Expand()
					.Border(dkTOP, GetWindowMetrics(UIWeiboSharePageContentEditVPaddingIndex))
					.Border(dkBOTTOM, 2*GetWindowMetrics(UIWeiboSharePageContentEditVPaddingIndex))
					.Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
			
#ifdef KINDLE_FOR_TOUCH
            mainSizer->Add(&m_weiboSharedButton, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)).Expand());
#else
            mainSizer->Add(&m_weiboSharedButton, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)).Align(dkALIGN_RIGHT));
#endif
            SetSizer(mainSizer);
        }
    }

    AppendChild(&m_titleBar);
    AppendChild(&m_titleShareLabel);
    AppendChild(&m_userNameLabel);
    AppendChild(&m_wordCountTipsLabel);
    AppendChild(&m_weiboContentEdit);
    AppendChild(&m_weiboSharedButton);
	AppendChild(&m_sharePicture);
#ifdef KINDLE_FOR_TOUCH
    AppendChild(&m_backButton);
#else
    m_weiboSharedButton.SetFocus(TRUE);
#endif
    Layout();

    return true;
}

bool UIWeiboSharePage::SetSharedContent(const string& sharedContent)
{
    m_weiboSharedContent.assign(sharedContent);
    m_weiboContentEdit.SetTextUTF8(sharedContent.c_str());
    return true;
}

bool UIWeiboSharePage::SetSharedPicture(const char* absoluteFilePath, bool autoDeleteFile)
{
    if (access(absoluteFilePath, R_OK) == 0)
    {
        string filePath = StringUtil::ToLower(absoluteFilePath);
        if (StringUtil::EndWith(filePath.c_str(), "jpeg") 
                || StringUtil::EndWith(filePath.c_str(), "gif")
                || StringUtil::EndWith(filePath.c_str(), "jpg")
                || StringUtil::EndWith(filePath.c_str(), "png"))
        {
        	m_pictureSizer->SetMinSize(dkSize(GetWindowMetrics(UIWeiboSharePagePictureWidthIndex),
							GetWindowMetrics(UIWeiboSharePageContentHeightIndex)));
        	m_pictureSizer->Show(true);
			m_pictureSizer->PrependSpacer(GetWindowMetrics(UIWeiboSharePageContentHorzPaddingIndex));
            m_weiboSharedPicturePath.assign(absoluteFilePath);
			m_sharePicture.SetAutoSize(true);
			m_sharePicture.SetImageFile(m_weiboSharedPicturePath);
			Layout();
        }

        m_autoDeletePictureFile = autoDeleteFile;
    }
    return false;
}

bool UIWeiboSharePage::OnContentEditChanged(const EventArgs& args)
{
    if (m_bIsVisible)
    {
        const int weiboContentMaxLen = WeiboFactory::GetSinaWeibo()->GetShortWeiboMaxLen();
        char buf[128] = {0};
        int curDataLen = m_weiboContentEdit.GetCurDataLen();
        if (curDataLen > 0 && curDataLen <= weiboContentMaxLen)
        {
            snprintf(buf, DK_DIM(buf), StringManager::GetPCSTRById(TYPE_WORD_COUNTS), weiboContentMaxLen - curDataLen);
            m_weiboSharedButton.SetEnable(true);
        }
        else if (curDataLen > weiboContentMaxLen)
        {
            snprintf(buf, DK_DIM(buf), StringManager::GetPCSTRById(TYPE_WORD_COUNTS_OVER), curDataLen - weiboContentMaxLen);
            m_weiboSharedButton.SetEnable(false);
        }
        else
        {
            m_weiboSharedButton.SetEnable(false);
        }
        m_wordCountTipsLabel.SetText(buf);
        m_wordCountTipsLabel.UpdateWindow();
        m_weiboSharedButton.UpdateWindow();
    }

    return true;
}

void UIWeiboSharePage::ShareWeibo()
{
    if (!UIUtility::CheckAndReConnectWifi())
    {
        return;
    }
    if (!WeiboFactory::GetSinaWeibo()->IsUserTokenValid())
    {
        UIWeiboBindDlg weiboBindDlg(this);
        weiboBindDlg.DoModal();
        string userName = WeiboFactory::GetSinaWeibo()->GetUserName();
        if (!userName.empty())
        {
            m_userNameLabel.SetText(userName.c_str());
        }
    }
    if (WeiboFactory::GetSinaWeibo()->IsUserTokenValid())
    {
        m_weiboSharedButton.SetEnable(false);
        m_weiboSharedButton.UpdateWindow();
        if (m_weiboSharedPicturePath.empty())
        {
            WeiboFactory::GetSinaWeibo()->GetMethod()->PostStatusesUpdate(m_weiboContentEdit.GetTextUTF8().c_str());
        }
        else
        {
            WeiboFactory::GetSinaWeibo()->GetMethod()->PostStatusesUpload(m_weiboContentEdit.GetTextUTF8().c_str(), m_weiboSharedPicturePath.c_str());
        }
    }
}

void UIWeiboSharePage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
        case ID_BTN_SINAWEIBO_SHARE:
            ShareWeibo();
            break;
#ifdef KINDLE_FOR_TOUCH
        case ID_BTN_TOUCH_BACK:
            CPageNavigator::BackToPrePage();
            break;
#endif
    }
}

bool UIWeiboSharePage::OnWeiboShareUpdate(const EventArgs& args)
{
    m_weiboSharedButton.SetEnable(true);
    const WeiboResultArgs& weiboResultArgs = (const WeiboResultArgs&)args;
    if (weiboResultArgs.mResultType == WeiboResultArgs::ERT_CORRECT)
    {
        //ParsingOauthRetSPtr oauthRet = std::tr1::dynamic_pointer_cast<ParsingOauthRet>(weiboResultArgs.mParsingInfo);
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(SHARED_SUCCESS));
        CPageNavigator::BackToPrePage();
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
            if (apiError->GetErrorCode() == WREC_LOGIN_EXPIRED)
            {
                DebugPrintf(DLC_DIAGNOSTIC, "UIWeiboBindDlg::OnWeiboLoginUpdate, expired");
                WeiboFactory::GetSinaWeibo()->ClearUserInfo();
                ShareWeibo();
            }
            else
            {
                DebugPrintf(DLC_DIAGNOSTIC, "UIWeiboBindDlg::OnWeiboLoginUpdate, %s", apiError->GetErrorMessage().c_str());
                UIMessageBox msgBox(this, apiError->GetErrorMessage().c_str(), MB_OK);
                msgBox.DoModal();
            }
        }
    }

    return true;
}

HRESULT UIWeiboSharePage::DrawBackGround(DK_IMAGE drawingImg)
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
    hr = grf.DrawLine(horizonMargin, m_titleShareLabel.GetY() + m_titleShareLabel.GetHeight() + GetWindowMetrics(UIWeiboSharePageTitleVPaddingIndex),
            m_iWidth - (horizonMargin << 1), 2, SOLID_STROKE);
#endif
    return hr;
}


string UIWeiboSharePage::PruneStringForDKComment(const string& bookName, const string& originalText)
{
    int weiboContentMaxLen = dk::weibo::WeiboFactory::GetSinaWeibo()->GetShortWeiboMaxLen();
    string dkComment = StringManager::GetPCSTRById(DUOKANSHUZHAI);
    string dkBookName = string(" #").append(bookName).append(1, '#');
    wstring wText = EncodeUtil::ToWString(originalText);
    int textLength = wcslen(wText.c_str());
    int remainWordNumber = weiboContentMaxLen 
        - wcslen(EncodeUtil::ToWString(dkComment).c_str()) - wcslen(EncodeUtil::ToWString(dkBookName).c_str());
    if (textLength > remainWordNumber)
    {
        wstring elipses = L"...";
        wText = wText.substr(0, remainWordNumber - wcslen(elipses.c_str())).append(elipses);
    }

    return dkComment.append(EncodeUtil::ToString(wText)).append(dkBookName);
}
