#include "TouchAppUI/UIPersonalSettingPanel1.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "GUI/GUISystem.h"
#include "Common/SystemSetting_DK.h"
#include "Common/CAccountManager.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/UIModifyAccountInfoDlg.h"
#include "CommonUI/UIWeiboBindDlg.h"
#include "Weibo/WeiboFactory.h"
#include "Thirdparties/EvernoteManager.h"

using namespace WindowsMetrics;
using namespace dk::account;
using namespace dk::weibo;
using namespace dk::thirdparties;

UIPersonalSettingPanel1::UIPersonalSettingPanel1()
{
    Init();
}

UIPersonalSettingPanel1::~UIPersonalSettingPanel1()
{
}

void UIPersonalSettingPanel1::Init()
{
    SubscribeMessageEvent(CAccountManager::EventAccount, *CAccountManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalSettingPanel1::OnAccountLogInOut),
        this,
        std::tr1::placeholders::_1));

	SubscribeMessageEvent(CAccountManager::EventXiaomiUserCardUpdate, *CAccountManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalSettingPanel1::OnXiaomiUserCardReceived),
        this,
        std::tr1::placeholders::_1));

    AppendChild(&m_loginPanel);
    AppendChild(&m_logoutPanel);
    AppendChild(&m_shareAccountPanel);
    if (CAccountManager::GetInstance()->IsLoggedIn())
    {
        m_curPanel = &m_loginPanel;
        m_logoutPanel.SetVisible(false);
    }
    else
    {
        m_curPanel = &m_logoutPanel;
        m_loginPanel.SetVisible(false);
    }
    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    mainSizer->Add(m_curPanel);
    mainSizer->Add(&m_shareAccountPanel);
    SetSizer(mainSizer);
}

void UIPersonalSettingPanel1::InitStatus()
{
    CAccountManager* accountManager = CAccountManager::GetInstance();
    UISizer* mainSizer = GetSizer();
    m_loginPanel.UpdateData();
	m_logoutPanel.UpdateData();
    m_shareAccountPanel.UpdateData();
    if (accountManager->IsLoggedIn() && m_curPanel != &m_loginPanel)
    {
        mainSizer->Replace(m_curPanel, &m_loginPanel);
        m_curPanel = &m_loginPanel;
        m_logoutPanel.SetVisible(false);
        m_curPanel->SetVisible(true);
    }
    else if (!accountManager->IsLoggedIn() && m_curPanel != &m_logoutPanel)
    {
        mainSizer->Replace(m_curPanel, &m_logoutPanel);
        m_curPanel = &m_logoutPanel;
        m_loginPanel.SetVisible(false);
        m_curPanel->SetVisible(true);
    }
    Layout();
}

bool UIPersonalSettingPanel1::OnChildClick(UIWindow* child)
{
    switch (child->GetId())
    {
        case SBI_Logout:
            OnLogoutClick();
            break;
        case SBI_Login:
            OnLoginClick();
            break;
        case SBI_ModifyPersonalInfo:
            OnModifyPersonalInfoClick();
            break;
        case SBI_PushMailBox:
            OnPushMailBoxClick();
            break;
        case SBI_Register:
            OnRegisterClick();
            break;
        case SBI_BindWithSinaWeibo:
            OnBindWithSinaWeiboClick();
            break;
        case SBI_UnbindWithSinaWeibo:
            OnUnbindWithSinaWeiboClick();
            break;
        case SBI_SwitchWithSinaWeibo:
            OnSwitchWithSinaWeiboClick();
            break;
        default:
            break;
    }
    return true;
}

void UIPersonalSettingPanel1::OnLogoutClick()
{
    CAccountManager::GetInstance()->LogoutAsync();
    UpdateData();
}

void UIPersonalSettingPanel1::OnLoginClick()
{
	if(UIUtility::CheckAndReLogin(RELOGIN_WITH_TOKEN_ASYNC))
	{
		UpdateData();
	}
}

void UIPersonalSettingPanel1::OnModifyPersonalInfoClick()
{
    if(!UIUtility::CheckAndReConnectWifi())
    {
        return;
    }

    CAccountManager* pManager = CAccountManager::GetInstance();
    if(!pManager)
    {
        return;
    }

    if (m_pParent)
    {
        UIModifyAccountInfoDlg modifyAccountInfoDialog(GUISystem::GetInstance()->GetTopFullScreenContainer());
        modifyAccountInfoDialog.DoModal();
		m_loginPanel.UpdateData();
    }
}

void UIPersonalSettingPanel1::OnPushMailBoxClick()
{
    CAccountManager* pManager = CAccountManager::GetInstance();
    if(!pManager)
    {
        return;
    }

    string sDKMail = pManager->GetDuoKanMailFromFile();
    string text(StringManager::GetPCSTRById(ACCOUNT_YOURDKMAIL));
    text.append(sDKMail).append(StringManager::GetPCSTRById(ACCOUNT_DKMAILFUNC));
    UIMessageBox messagebox(this, text.c_str(), MB_OK);
    messagebox.DoModal();
}

void UIPersonalSettingPanel1::OnRegisterClick()
{
    if(!UIUtility::CheckAndReConnectWifi())
    {
        return;
    }

    CAccountManager* accountManager = CAccountManager::GetInstance();
    // TODO: check return value to notify user
    accountManager->RegisterAccount();
    UpdateData();
}

void UIPersonalSettingPanel1::OnBindWithSinaWeiboClick()
{
    if(!UIUtility::CheckAndReConnectWifi())
    {
        return;
    }

    if (WeiboFactory::GetSinaWeibo()->GetUserName().empty())
    {
        UIWeiboBindDlg weiboBindDlg(GUISystem::GetInstance()->GetTopFullScreenContainer());
        weiboBindDlg.DoModal();
    }
    UpdateData();
}

void UIPersonalSettingPanel1::OnUnbindWithSinaWeiboClick()
{
    WeiboFactory::GetSinaWeibo()->ClearUserInfo();
    UIMessageBox messagebox(this, StringManager::GetPCSTRById(UNBIND_SUCCESS), MB_OK);
    messagebox.DoModal();
    UpdateData();
}

void UIPersonalSettingPanel1::OnSwitchWithSinaWeiboClick()
{
    if(!UIUtility::CheckAndReConnectWifi())
    {
        return;
    }

    UIWeiboBindDlg weiboBindDlg(GUISystem::GetInstance()->GetTopFullScreenContainer());
    weiboBindDlg.DoModal();
    UpdateData();
}

void UIPersonalSettingPanel1::UpdateData()
{
    CAccountManager* accountManager = CAccountManager::GetInstance();
    UISizer* mainSizer = GetSizer();
    m_loginPanel.UpdateData();
	m_logoutPanel.UpdateData();
    m_shareAccountPanel.UpdateData();
    if (accountManager->IsLoggedIn() && m_curPanel != &m_loginPanel)
    {
        mainSizer->Replace(m_curPanel, &m_loginPanel);
        m_curPanel = &m_loginPanel;
        m_logoutPanel.SetVisible(false);
        m_curPanel->SetVisible(true);
    }
    else if (!accountManager->IsLoggedIn() && m_curPanel != &m_logoutPanel)
    {
        mainSizer->Replace(m_curPanel, &m_logoutPanel);
        m_curPanel = &m_logoutPanel;
        m_loginPanel.SetVisible(false);
        m_curPanel->SetVisible(true);
    }
    Layout();
    Repaint();
}

bool UIPersonalSettingPanel1::OnAccountLogInOut(const EventArgs& args)
{
    InitStatus();
    UIWindow* pParent = this;
    while (pParent && pParent->GetParent())
    {
        pParent = pParent->GetParent();
    }

    if (GUISystem::GetInstance()->GetTopFullScreenContainer() == pParent)
    {
        UpdateWindow();
    }
    return true;
}

bool UIPersonalSettingPanel1::OnXiaomiUserCardReceived(const EventArgs& args)
{
    UpdateData();
    return true;
}
