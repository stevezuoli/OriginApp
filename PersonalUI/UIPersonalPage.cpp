#include <tr1/functional>
#include "PersonalUI/UIPersonalPage.h"
#include "PersonalUI/UIPersonalExperiencePage.h"
#include "I18n/StringManager.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/UIUtility.h"
#include "Framework/CDisplay.h"
#include "CommonUI/CPageNavigator.h"
#include "Utility/ColorManager.h"
#include "GUI/UISizer.h"
#include "GUI/GUISystem.h"
#include "Common/CAccountManager.h"
#include "BookStoreUI/UIBookStorePersonCenteredPage.h"
#include "PersonalUI/UIPersonalCommentsPage.h"
#include "PersonalUI/UIPersonalMessagesPage.h"
//#include "PersonalUI/UIPersonalPushedMessagesPage.h"
#include "PersonalUI/UIPersonalFavouritesPage.h"
#include "PersonalUI/UIPersonalNotesSummaryPage.h"
#include "PersonalUI/PushedMessagesManager.h"
#include "BookStore/BookStoreOrderManager.h"
#include "CommonUI/UIForgetPasswordDlg.h"

using namespace std;
using namespace WindowsMetrics;
using namespace dk::account;

UIPersonalPage::UIPersonalPage()
    :UIPage()
    , m_buttonsplitLine1(this)
    , m_buttonsplitLine2(this)
    , m_buttonsplitLine3(this)
{
    SubscribeMessageEvent(CAccountManager::EventAccount, *CAccountManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalPage::LogDataChanged),
        this,
        std::tr1::placeholders::_1));

    SubscribeMessageEvent(CAccountManager::EventXiaomiUserCardUpdate, *CAccountManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalPage::OnXiaomiUserCardReceived),
        this,
        std::tr1::placeholders::_1));

    SubscribeMessageEvent(PushedMessagesManager::EventMessageUnreadCountUpdate,
        *PushedMessagesManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalPage::OnMessageUnreadCountChanged),
        this,
        std::tr1::placeholders::_1));

    CAccountManager* accountManager = CAccountManager::GetInstance();
    if (!accountManager->IsLoggedIn())
    {
        accountManager->LoginWithTokenAsync();
    }

    InitUI();
}

UIPersonalPage::~UIPersonalPage()
{
}

void UIPersonalPage::OnEnter()
{
    UIPage::OnEnter();
    UpdateData();
}

void UIPersonalPage::OnLeave()
{
    UIPage::OnLeave();
    StopGetMessages();
}

void UIPersonalPage::OnLoad()
{
    UIPage::OnLoad();
    UpdateData();
}

void UIPersonalPage::OnUnLoad()
{
    UIPage::OnUnLoad();
    StopGetMessages();
}

void UIPersonalPage::StartGetMessages()
{
    PushedMessagesManager* pPushedMgr = PushedMessagesManager::GetInstance();
    if (pPushedMgr && pPushedMgr->ResetCurrentUserID())
    {
        pPushedMgr->StartGetMessages();
    }
}

void UIPersonalPage::StopGetMessages()
{
    PushedMessagesManager* pPushedMgr = PushedMessagesManager::GetInstance();
    if (pPushedMgr)
    {
        pPushedMgr->StopGetMessages(false);
    }
}

void UIPersonalPage::MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height)
{
    UIPage::MoveWindow(left, top, width, height);
    Layout();
}

void UIPersonalPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
        case ID_BTN_PERSONAL_REGISTER:
            OnRegisterClick();
            break;
        case ID_BTN_PERSONAL_LOGIN:
            OnLoginClick();
            break;
        case ID_BTN_PERSONAL_BOOK:
            OnPersonalBookClick();
            break;
        case ID_BTN_PERSONAL_COMMENT_SQUARE:
            OnCommentSquareClick();
            break;
        case ID_BTN_PERSONAL_EXPERIENCE:
            OnPersonalExperienceClick();
            break;
        case ID_BTN_PERSONAL_MESSAGE:
            OnPersonalMessageClick();
            break;
        case ID_BTN_PERSONAL_KEY:
            OnPersonalKeyClick();
            break;
        case ID_BTN_PERSONAL_NOTES:
            OnPersonalNotesClick();
            break;
        case ID_BTN_PERSONAL_FAVOURITE:
            OnPersonalFavouriteClick();
            break;
        default:
            break;
    }
}

void UIPersonalPage::OnPersonalBookClick()
{
    if(LoginWithTip())
    {
        UIBookStorePersonCenteredPage* personalBookPage = new UIBookStorePersonCenteredPage();
        personalBookPage->MoveWindow(0, 0, CDisplay::GetDisplay()->GetScreenWidth(), CDisplay::GetDisplay()->GetScreenHeight());
        CPageNavigator::Goto(personalBookPage);
    }
}

void UIPersonalPage::OnCommentSquareClick()
{
    if(LoginWithTip())
    {
        UIPersonalCommentsPage* personalCommentsPage = new UIPersonalCommentsPage();
        if (personalCommentsPage)
        {
            personalCommentsPage->MoveWindow(0, 0, CDisplay::GetDisplay()->GetScreenWidth(), CDisplay::GetDisplay()->GetScreenHeight());
            CPageNavigator::Goto(personalCommentsPage);
        }
    }
}

void UIPersonalPage::OnPersonalExperienceClick()
{
    UIPersonalExperiencePage* personalExperiencePage = new UIPersonalExperiencePage();
    personalExperiencePage->MoveWindow(0, 0, CDisplay::GetDisplay()->GetScreenWidth(), CDisplay::GetDisplay()->GetScreenHeight());
    CPageNavigator::Goto(personalExperiencePage);
}

void UIPersonalPage::OnPersonalMessageClick()
{
    if(LoginWithTip())
    {
        UIPersonalMessagesPage* personalMessagesPage = new UIPersonalMessagesPage();
        //UIPersonalPushedMessagesPage* personalMessagesPage = new UIPersonalPushedMessagesPage();
        personalMessagesPage->MoveWindow(0, 0, CDisplay::GetDisplay()->GetScreenWidth(), CDisplay::GetDisplay()->GetScreenHeight());
        CPageNavigator::Goto(personalMessagesPage);
    }
}

void UIPersonalPage::OnPersonalFavouriteClick()
{
    if (UIUtility::CheckAndReConnectWifi())
    {
        UIBookStorePersonalFavouritesPage* personalFavouritesPage = new UIBookStorePersonalFavouritesPage();
        personalFavouritesPage->MoveWindow(0, 0, CDisplay::GetDisplay()->GetScreenWidth(), CDisplay::GetDisplay()->GetScreenHeight());
        CPageNavigator::Goto(personalFavouritesPage);
    }
}

void UIPersonalPage::OnPersonalKeyClick()
{
}

void UIPersonalPage::OnRegisterClick()
{
    if(!UIUtility::CheckAndReConnectWifi())
    {
        return;
    }
	CAccountManager::GetInstance()->RegisterAccount();
}

void UIPersonalPage::OnLoginClick()
{
    if(LoginWithTip())
    {
        UpdateData();
    }
}

void UIPersonalPage::OnPersonalNotesClick()
{
    if(LoginWithTip())
    {
        UIPersonalNotesSummaryPage* pPage = new UIPersonalNotesSummaryPage();
        pPage->MoveWindow(0, 0, CDisplay::GetDisplay()->GetScreenWidth(), CDisplay::GetDisplay()->GetScreenHeight());
        CPageNavigator::Goto(pPage);
    }
}

void UIPersonalPage::InitUI()
{
    m_loginTip.SetFontSize(GetWindowFontSize(FontSize28Index));
    m_loginTip.SetText(StringManager::GetPCSTRById(PERSONAL_NOTLOGIN));
    m_loginTip.SetAlign(ALIGN_CENTER);

    m_loginName.SetFontSize(GetWindowFontSize(FontSize28Index));
    m_loginName.SetAlign(ALIGN_CENTER);
    
    m_loginEmail.SetFontSize(GetWindowFontSize(FontSize20Index));
    m_loginEmail.SetAlign(ALIGN_CENTER);

#ifdef KINDLE_FOR_TOUCH
    m_btnRegister.Initialize(ID_BTN_PERSONAL_REGISTER, StringManager::GetPCSTRById(ACCOUNT_REGISTER), GetWindowFontSize(FontSize20Index));
    m_btnLogin.Initialize(ID_BTN_PERSONAL_LOGIN, StringManager::GetPCSTRById(ACCOUNT_LOGIN), GetWindowFontSize(FontSize20Index));
    m_btnRegister.ShowBorder(false);
    m_btnLogin.ShowBorder(false);
#else
    m_btnRegister.Initialize(ID_BTN_PERSONAL_REGISTER, StringManager::GetPCSTRById(ACCOUNT_REGISTER), KEY_RESERVED, GetWindowFontSize(FontSize20Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnLogin.Initialize(ID_BTN_PERSONAL_LOGIN, StringManager::GetPCSTRById(ACCOUNT_LOGIN), KEY_RESERVED, GetWindowFontSize(FontSize20Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif

    SOURCESTRINGID titelId[s_itemCount] = {PERSONAL_BOOK, PERSONAL_COMMENTSSQUARE, PERSONAL_EXPERIENCE, PERSONAL_MESSAGE, PERSONAL_NOTES, PERSONAL_FAVOURITES};
    SOURCESTRINGID abustructId[s_itemCount] = {PERSONAL_BOOK_SUBTITLE, PERSONAL_COMMENT_SUBTITLE, PERSONAL_EXPERIENCE_SUBTITLE, PERSONAL_MESSAGE_SUBTITLE, PERSONAL_NOTES_SUBTITLE, PERSONAL_FAVOURITES_SUBTITLE};
    DWORD wdItemId[s_itemCount] = {ID_BTN_PERSONAL_BOOK, ID_BTN_PERSONAL_COMMENT_SQUARE, ID_BTN_PERSONAL_EXPERIENCE, ID_BTN_PERSONAL_MESSAGE, ID_BTN_PERSONAL_NOTES, ID_BTN_PERSONAL_FAVOURITE};
    
    for(int i = 0; i < s_itemCount; i++)
    {
        //m_itemList[i].SetMinWidth(GetWindowMetrics(UIPersonalPageIndexPageItemWidthIndex));
        //m_itemList[i].SetMinSize(dkSize(GetWindowMetrics(UIPersonalPageIndexPageItemWidthIndex), GetWindowMetrics(UIPersonalPageIndexPageItemHeightIndex)));
        m_itemList[i].InitItem(wdItemId[i], StringManager::GetPCSTRById(titelId[i]), StringManager::GetPCSTRById(abustructId[i]));
        AppendChild(&m_itemList[i]);
    }
    
    const int elementSpacing = GetWindowMetrics(UIPersonalExperienceElementSpacingIndex);
    const int lineHeight = GetWindowMetrics(UIPersonalExperienceLineMinHeightIndex);
    m_buttonsplitLine1.SetMinHeight(lineHeight);
    m_buttonsplitLine2.SetMinWidth(lineHeight);
    m_buttonsplitLine3.SetMinHeight(lineHeight);

    AppendChild(&m_btnRegister);
    AppendChild(&m_btnLogin);
    AppendChild(&m_loginTip);
    AppendChild(&m_loginName);
    AppendChild(&m_loginEmail);
    AppendChild(&m_titleBar);
    AppendChild(&m_buttonsplitLine1);
    AppendChild(&m_buttonsplitLine2);
    AppendChild(&m_buttonsplitLine3);

#ifdef KINDLE_FOR_TOUCH
    AppendChild(m_pBottomBar);
    const int btnHeight = GetWindowMetrics(UIPersonalPageButtonHeightIndex);
    m_btnRegister.SetMinHeight(btnHeight);
    m_btnRegister.SetMinWidth(GetWindowMetrics(UIPersonalPageButtonWidthIndex));
    m_btnLogin.SetMinHeight(btnHeight);
    m_btnLogin.SetMinWidth(GetWindowMetrics(UIPersonalPageButtonWidthIndex));
#else
    m_titleBar.MoveWindow(0, 0, 600, GetWindowMetrics(UITitleBarHeightIndex));
#endif

    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        
        m_loginSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* buttonSizer = new UIBoxSizer(dkHORIZONTAL);
        buttonSizer->AddStretchSpacer();
        buttonSizer->Add(&m_btnRegister, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL));
        buttonSizer->AddStretchSpacer();
        buttonSizer->Add(&m_buttonsplitLine2, UISizerFlags().Expand());
        buttonSizer->AddStretchSpacer();
        buttonSizer->Add(&m_btnLogin, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL));
        buttonSizer->AddStretchSpacer();
        buttonSizer->SetMinHeight(GetWindowMetrics(UIPersonalPageButtonHeightIndex));

        const int loginSizerHeight = GetWindowMetrics(UIPersonalPageUserInfoHeightIndex);
        m_loginSizer->AddStretchSpacer();
        m_loginSizer->Add(&m_loginTip, UISizerFlags(1).Expand().Align(dkALIGN_CENTER));
        m_loginSizer->AddStretchSpacer();
        m_loginSizer->Add(&m_buttonsplitLine1, UISizerFlags().Expand().Border(dkBOTTOM, elementSpacing));
        m_loginSizer->Add(buttonSizer, UISizerFlags().Expand().Align(dkALIGN_BOTTOM));
        m_loginSizer->SetMinHeight(loginSizerHeight);

        m_logoutSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* userInfoSizer = new UIBoxSizer(dkVERTICAL);
        userInfoSizer->Add(&m_loginName, UISizerFlags().Expand().Border(dkBOTTOM, GetWindowMetrics(UIPersonalPageUserInfoMarginIndex)));
        userInfoSizer->Add(&m_loginEmail, UISizerFlags().Expand());
        m_logoutSizer->SetMinHeight(loginSizerHeight);
        m_logoutSizer->AddStretchSpacer();
        m_logoutSizer->Add(userInfoSizer, UISizerFlags(1).Align(dkALIGN_CENTER));
        m_logoutSizer->AddStretchSpacer();
        UISizer* itemSizer = new UIGridSizer(s_itemCol, GetWindowMetrics(UIPixelValue10Index), GetWindowMetrics(UIPersonalUILabelVerticalSpacingIndex));
        for ( int i = 0; i < s_itemCount; ++i)
        {
            itemSizer->Add(m_itemList + i, UISizerFlags().Expand());
        }

        UISizer* topSizer = new UIBoxSizer(dkVERTICAL);
        topSizer->Add(m_loginSizer, UISizerFlags().Expand());
        topSizer->Add(m_logoutSizer, UISizerFlags().Expand());
        
        mainSizer->Add(&m_titleBar, UISizerFlags().Expand());
        mainSizer->Add(topSizer, UISizerFlags().Expand().Border(dkBOTTOM, elementSpacing));
        mainSizer->Add(&m_buttonsplitLine3, UISizerFlags().Expand());

        const int margin = GetWindowMetrics(UIPixelValue30Index);
        mainSizer->AddSpacer(margin);
        mainSizer->Add(itemSizer, UISizerFlags(1).Align(dkALIGN_CENTER).Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)).Expand());
        mainSizer->AddSpacer(margin);
#ifdef KINDLE_FOR_TOUCH
        mainSizer->Add(m_pBottomBar);
#endif
        SetSizer(mainSizer);
    }
    UpdateData();
}

void UIPersonalPage::UpdateData()
{
    CAccountManager* accountManager = CAccountManager::GetInstance();
    if (accountManager->IsLoggedIn())
    {
#ifndef KINDLE_FOR_TOUCH
        m_itemList[0].SetFocus(true);
#endif
        m_logoutSizer->Show(true);
        m_loginSizer->Show(false);
        m_loginEmail.SetText(accountManager->GetEmailFromFile().c_str());
        m_loginName.SetText(accountManager->GetAliasFromFile().c_str());
        StartGetMessages();
        UpdateMessagesUnreadCount();
    }
    else
    {
        m_loginSizer->Show(true);
        m_logoutSizer->Show(false);
#ifndef KINDLE_FOR_TOUCH
        m_btnLogin.SetFocus(true);
#endif
    }
    Layout();
    Repaint();
}

bool UIPersonalPage::OnXiaomiUserCardReceived(const EventArgs& args)
{
    CAccountManager* accountManager = CAccountManager::GetInstance();
    m_loginName.SetText(accountManager->GetAliasFromFile().c_str());
    Layout();
    Repaint();
    return true;
}

bool UIPersonalPage::LogDataChanged(const EventArgs& args)
{
    const AccountEventArgs& accountEventArgs = (const AccountEventArgs&)args;
	if(!accountEventArgs.IsLoggedIn() && accountEventArgs.errorType == AccountEventArgs::ET_NETWORK)
	{
		UIUtility::SetScreenTips(StringManager::GetPCSTRById(ACCOUNT_NETWORKISSUE));
	}
	UpdateData();
    return true;
}

bool UIPersonalPage::OnMessageUnreadCountChanged(const EventArgs& args)
{
    UpdateMessagesUnreadCount();
    return true;
}

void UIPersonalPage::UpdateMessagesUnreadCount()
{
    PushedMessagesManager* messageMgr = PushedMessagesManager::GetInstance();
    UIBookStoreIndexPageItem* item = GetItemByID(ID_BTN_PERSONAL_MESSAGE);
    if (messageMgr && item)
    {
        item->SetUnreadCount(messageMgr->GetBoxMessagesUnreadCount());
    }
}

UIBookStoreIndexPageItem* UIPersonalPage::GetItemByID(DWORD id)
{
    for(int i = 0; i < s_itemCount; i++)
    {
        if (m_itemList[i].GetId() == id)
        {
            return &m_itemList[i];
        }
    }
    return NULL;
}

bool UIPersonalPage::LoginWithTip()
{
	CAccountManager* accountManager = CAccountManager::GetInstance();
    if (accountManager->IsLoggedIn())
    {
        return true;
    }
	if(UIUtility::CheckAndReLogin(RELOGIN_WITH_TOKEN_ASYNC))
	{
		UIUtility::SetScreenTips(StringManager::GetPCSTRById(ACCOUNT_LOGIN_LOGINING));
	}
	return accountManager->IsLoggedIn();
}


