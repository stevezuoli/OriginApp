#include <tr1/functional>
#include "Common/CAccountManager.h"
#include "BookStoreUI/UIBookStorePage.h"
#include "AppUI/UISystemSettingPage.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/CPageNavigator.h"
#include "BookStoreUI/UIBookStorePersonCenteredPage.h"
#include "AppUI/UIWifiDialog.h"
#include "AppUI/UIAccountLoginDialog.h"
#include "AppUI/UIDownloadPage.h"
#include "Wifi/WifiManager.h"
#include "DownloadManager/DownloaderImpl.h"
#include "Framework/CDisplay.h"
#include "PersonalUI/UIPersonalPage.h"
using namespace dk::account;

UIBookStorePage::UIBookStorePage()
	: UIPage()
    , m_mnuMain(this)
{
    m_pMainMenu = &m_mnuMain;

    m_mnuMain.MoveWindow(MAINMENU_LEFTMARGIN, MAINMENU_TOPMARGIN, MAINMENU_WIDTH, MAINMENU_HEIGHT);
    m_mnuMain.AppendMenu(ID_MNU_ENABLE_WIFI, 0, CString(StringManager::GetStringById(OPEN_WIFI_NET)), IMAGE_BUTTON_HEAD_24, 'A', TRUE);
    m_mnuMain.AppendMenu(ID_MNU_DOWNLOAD_MANAGER, 0, CString(StringManager::GetStringById(DOWNLOAD_BOOK_MANAGER)), IMAGE_BUTTON_HEAD_24, 'B', TRUE);
    m_mnuMain.AppendMenu(ID_MNU_USERDOWNLOAD, 0, StringManager::GetPCSTRById(USERSETTING_USERDOWNLOAD), IMAGE_BUTTON_HEAD_24, 'C', TRUE);
    m_mnuMain.AppendMenu(ID_MNU_MANAGE_WIFI, 0, CString(StringManager::GetStringById(MANAGE_WIFI)), IMAGE_BUTTON_HEAD_24, 'M', TRUE);
	m_mnuMain.AppendMenu(ID_MNU_GOTO_ACCOUNTCENTER, 0, StringManager::GetStringById(ACCOUNTCENTER), IMAGE_BUTTON_HEAD_24, 'U', TRUE);
    m_mnuMain.AppendMenu(ID_MNU_SYSTEM_SETTINGS, ALT_DOWN | KEY_SYM, CString(StringManager::GetStringById(SYSTEM_SETTING)), IMAGE_BUTTON_HEAD_24, 'S', TRUE);
    AppendChild(&m_mnuMain);

    m_title.MoveWindow(0, 0, CDisplay::GetDisplay()->GetScreenWidth(), 30);
    m_pTitleBar = &m_title;
    AppendChild(&m_title);
}

UIBookStorePage::~UIBookStorePage()
{
}

void UIBookStorePage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
	switch (dwCmdId)
    {
    case ID_MNU_ENABLE_WIFI:
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s ID_MNU_ENABLE_WIFI", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            WifiManager* wifiBase = WifiManager::GetInstance();
            if(wifiBase->IsConnected())
            {
                DownloaderImpl::GetInstance()->PauseAllTasks();
                wifiBase->StartPowerOffAsync(WifiStatusCallBack,WifiStatusCallBack);
            }
            else
            {
                wifiBase->StartPowerOnAsync(WifiStatusCallBack,WifiPowerOnCallBack);
            }
        }
        break;
    case ID_MNU_MANAGE_WIFI:
        {
            if(WifiManager::GetInstance()->IsConnected())//只有当已Wifi连接时才可能出现管理Wifi界面
            {
                UIWifiDialog m_WifiDialog(this);
                CDisplay* pDisplay = CDisplay::GetDisplay();
                if(pDisplay)
                {
                    m_WifiDialog.MoveWindow(0,0,pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
                    m_WifiDialog.DoModal();
                }
            }
        }
        break;
    case ID_MNU_SYSTEM_SETTINGS:
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s ID_MNU_SYSTEM_SETTINGS", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            UISystemSettingPage *pPage = new UISystemSettingPage();
            if(pPage)
            {
                CDisplay *pDisplay = CDisplay::GetDisplay();
                if(pDisplay)
                {
                    pPage->MoveWindow(0, 0, pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
                    CPageNavigator::Goto(pPage);
                }
            }
        }
        break;
    case ID_MNU_USERDOWNLOAD:
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s ID_MNU_USERDOWNLOAD", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            if (UIUtility::CheckAndReLogin())
            {
                CDisplay* pDisplay = CDisplay::GetDisplay();
                UIBookStorePersonCenteredPage *pUserPage = new UIBookStorePersonCenteredPage();
                if (pDisplay && pUserPage)
                {
                    pUserPage->MoveWindow(0,0,pDisplay->GetScreenWidth(),pDisplay->GetScreenHeight());
                    CPageNavigator::Goto(pUserPage);
                }
            }
        }
        break;
    case ID_MNU_DOWNLOAD_MANAGER:
        {
            UIDownloadPage *pPage = new UIDownloadPage();
            if(pPage)
            {
                CDisplay *pDisplay = CDisplay::GetDisplay();
                if(pDisplay)
                {
                    pPage->MoveWindow(0, 0, pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
                    CPageNavigator::Goto(pPage);
                }
            }
        }
        break;
	case ID_MNU_GOTO_ACCOUNTCENTER:
		{
			UIPersonalPage *pPage = new UIPersonalPage();
			if(pPage)
			{
				CDisplay *pDisplay = CDisplay::GetDisplay();
				if(pDisplay)
				{
					pPage->MoveWindow(0, 0, pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
					CPageNavigator::Goto(pPage);
				}
			}
		}
    default:
		break;
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UIBookStorePage::SetWifiMenuItem()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    WifiManager* wifiBase = WifiManager::GetInstance();
    eWifiManagerWorking working = wifiBase->GetCurWorking();
    DebugPrintf(DLC_BOOKSTORE, "UIHomePage::SetWifiMenuItem working=%d",working);
    switch (working)
    {
    case PowerOnIsWorking:
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s PowerOnIsWorking", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        m_mnuMain.SetItemLabel(ID_MNU_ENABLE_WIFI,StringManager::GetStringById(OPEN_WORKING_IN));
        m_mnuMain.SetItemActive(ID_MNU_ENABLE_WIFI, FALSE);
        m_mnuMain.SetItemVisible(ID_MNU_MANAGE_WIFI, FALSE);
        break;
    case PowerOffIsWorking:
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s PowerOffIsWorking", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        m_mnuMain.SetItemLabel(ID_MNU_ENABLE_WIFI, StringManager::GetStringById(CLOSE_WORKING_IN));
        m_mnuMain.SetItemVisible(ID_MNU_ENABLE_WIFI, TRUE);
        m_mnuMain.SetItemActive(ID_MNU_ENABLE_WIFI, FALSE);
        m_mnuMain.SetItemVisible(ID_MNU_MANAGE_WIFI, FALSE);
        break;
    case Refreshing:
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s Refreshing", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        m_mnuMain.SetItemLabel(ID_MNU_ENABLE_WIFI, StringManager::GetStringById(REFRESH_WORKING_IN));
        m_mnuMain.SetItemVisible(ID_MNU_ENABLE_WIFI, TRUE);
        m_mnuMain.SetItemActive(ID_MNU_ENABLE_WIFI, FALSE);
        m_mnuMain.SetItemVisible(ID_MNU_MANAGE_WIFI, FALSE);
        break;
    case Connecting:
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s Connecting", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        m_mnuMain.SetItemLabel(ID_MNU_ENABLE_WIFI, StringManager::GetStringById(CONNECT_WORKING_IN));
        m_mnuMain.SetItemVisible(ID_MNU_ENABLE_WIFI, TRUE);
        m_mnuMain.SetItemActive(ID_MNU_ENABLE_WIFI, FALSE);
        m_mnuMain.SetItemVisible(ID_MNU_MANAGE_WIFI, FALSE);
        break;
    case Nothing:
        if(wifiBase->IsConnected())
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s Nothing/IsConnected", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            m_mnuMain.SetItemLabel(ID_MNU_ENABLE_WIFI, StringManager::GetStringById(CLOSE_WIFI_NET));
            m_mnuMain.SetItemVisible(ID_MNU_ENABLE_WIFI, TRUE);
            m_mnuMain.SetItemActive(ID_MNU_ENABLE_WIFI, TRUE);
            m_mnuMain.SetItemVisible(ID_MNU_MANAGE_WIFI, TRUE);
        }
        else
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s Nothing/!IsConnected", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            m_mnuMain.SetItemLabel(ID_MNU_ENABLE_WIFI, StringManager::GetStringById(OPEN_WIFI_NET));
            m_mnuMain.SetItemVisible(ID_MNU_ENABLE_WIFI, TRUE);
            m_mnuMain.SetItemActive(ID_MNU_ENABLE_WIFI, TRUE);
            m_mnuMain.SetItemVisible(ID_MNU_MANAGE_WIFI, FALSE);
        }
        break;
    default:
        DebugPrintf(DLC_BOOKSTORE, "UIHomePage::SetWifiMenuItem Unknown status: %d", working);
        break;
    }
    m_mnuMain.UpdateWindow();

    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UIBookStorePage::ConnectWifiCallBack(BOOL connected, string ssid, string password, string strIdentity)
{
    ConnectionMessage* cm = new ConnectionMessage(connected, ssid, password, strIdentity);
    if(!cm)
    {
        DebugPrintf(DLC_BOOKSTORE, "Failed to create ConnectionMessage for (connected=%d, ssid=%s, password=%s)", connected, ssid.c_str(), password.c_str());
        return;
    }
    SNativeMessage msg;
    msg.iType = KMessageWifiChange;
    msg.iParam1 = (UINT32)GetClassNameStatic();
    msg.iParam2 = CONNECT_WIFI;
    msg.iParam3 = (UINT32)cm;
    INativeMessageQueue::GetMessageQueue()->Send(msg);
}

void UIBookStorePage::WifiStatusCallBack()
{
    SNativeMessage msg;
    msg.iType = KMessageWifiChange;
    msg.iParam1 = (UINT32)GetClassNameStatic();
    msg.iParam2 = (UINT32)UPDATE_WIFI_STATUS;
    msg.iParam3 = (UINT32)DEFAULT;
    INativeMessageQueue::GetMessageQueue()->Send(msg);
}

void UIBookStorePage::WifiPowerOnCallBack()
{
    SNativeMessage msg;
    msg.iType = KMessageWifiChange;
    msg.iParam1 = (UINT32)GetClassNameStatic();
    msg.iParam2 = POWERONOFF_WIFI;
    INativeMessageQueue::GetMessageQueue()->Send(msg);
}

void UIBookStorePage::UpdateTitleBar()
{
    UITitleBar *pTitleBar = (CDisplay::GetDisplay()->GetCurrentPage())->GetTitleBar();
    if(pTitleBar && pTitleBar->IsVisible())
    {
        pTitleBar->UpdateSignal();
    }
}

void UIBookStorePage::OnLoad()
{
    SetVisible(TRUE);
    UIPage::OnLoad();
}

void UIBookStorePage::OnUnLoad()
{
    SetVisible(FALSE);
    UIPage::OnUnLoad();

}

void UIBookStorePage::OnWifiMessage(SNativeMessage event)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if(!m_bIsVisible)
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s if(!m_bIsVisible)", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return;
    }

    switch(event.iParam2)
    {
    case UPDATE_WIFI_STATUS:
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s UPDATE_WIFI_STATUS", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            UpdateTitleBar();
            SetWifiMenuItem();
        }
        break;
    case POWERONOFF_WIFI:
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s POWERONOFF_WIFI", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            UpdateTitleBar();
            SetWifiMenuItem();
            WifiManager* wifiManager = WifiManager::GetInstance();
            if(wifiManager->IsPowerOn())
            {
                if(!wifiManager->IsConnected())
                {
                    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s No wifi connected, begin to auto join!", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
                    wifiManager->StartAutoJoin(WifiStatusCallBack, ConnectWifiCallBack);
                }
                else
                {
                    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s Has one wifi connected!!!", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
                }
            }
            else
            {
                DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s Has no power now!!!", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            }
        }
        break;
    case CONNECT_WIFI:
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s CONNECT_WIFI", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            UpdateTitleBar();
            SetWifiMenuItem();
            ConnectionMessage* cm = (ConnectionMessage*)event.iParam3;
            if(cm && !cm->GetWifiConnected())
            {
                delete cm;
                cm = NULL;

                if(m_pMainMenu->IsVisible())
                {
                    m_pMainMenu->Show(FALSE);
                    m_pMainMenu->UpdateWindow();
                }
                UIWifiDialog m_WifiDialog(this);
                CDisplay* pDisplay = CDisplay::GetDisplay();
                if(pDisplay)
                {
                    m_WifiDialog.MoveWindow(0,0,pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
                    m_WifiDialog.DoModal();
                }
            }
        }
        break;
    default:
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s default", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        break;
    }

    WifiManager* wifiManager = WifiManager::GetInstance();
    if(wifiManager->IsPowerOn() && wifiManager->IsConnected())
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s RequestPage", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        RequestPage();
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

BOOL UIBookStorePage::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch(iKeyCode)
    {
    case KEY_MENU:
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s KEY_MENU", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            SetWifiMenuItem();
        }
        break;
    case KEY_HOME:
        CPageNavigator::BackToHome();
        return FALSE;
    case KEY_BACK:
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s BACK", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        if (m_mnuMain.IsVisible())
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s if (m_mnuMain.IsVisible())", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            m_mnuMain.Show(FALSE);
        }
        else if (SendKeyToChildren(iKeyCode))
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s (!m_mnuMain.IsVisible())", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            CPageNavigator::BackToPrePage();
        }
        return FALSE;
    default:
        if(!SendHotKeyToChildren(iKeyCode))
        {
            return FALSE;
        }
        break;
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
#endif
    return UIPage::OnKeyPress(iKeyCode);
}
