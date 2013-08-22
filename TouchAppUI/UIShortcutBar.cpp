
////////////////////////////////////////////////////////////////////////
// UIShortcutBar.cpp
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////
#include "Common/CAccountManager.h"
#include "Common/WindowsMetrics.h"
#include "TouchAppUI/UIShortcutBar.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/CPageNavigator.h"
#include "GUI/CTpGraphics.h"
#include "GUI/FontManager.h"
#include "Framework/CDisplay.h"
#include "I18n/StringManager.h"
#include "FtpService/FtpService.h"
#include "Wifi/WifiManager.h"
#include "DownloadManager/IDownloader.h"
#include "TouchAppUI/UIWifiDialog.h"
#include "GUI/GUISystem.h"
#include "Utility/WebBrowserUtil.h"
#include <tr1/functional>

using namespace std;
using namespace dk::account;
using namespace dk::utility;
using namespace WindowsMetrics;

/****************************************************************
* UIShortcutBar
****************************************************************/
// #define UILOGINDIALOG_HEIGHT            200
// #define UILOGINDIALOG_WIDTH             450
// #define UISHORTCUTBAR_DEFAULT_FONTSIZE  18

SINGLETON_CPP(UIShortcutBar)

UIShortcutBar::UIShortcutBar()
	: UIButtonGroup(NULL)
{
    m_btnWLAN.Initialize(ID_BTN_TOUCH_WLAN, StringManager::GetPCSTRById(TOUCH_TITLEBAR_WLAN), GetWindowFontSize(UIShortCutBarBtnIndex));
    m_btnWLAN.SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_STATUSBAR00), ImageManager::GetImage(IMAGE_TOUCH_ICON_STATUSBAR00_WHITE), UIComplexButton::ICON_TOP);
	m_btnWLAN.ShowBorder(false);
    
    m_btnWifiTransfer.Initialize(ID_BTN_TOUCH_WIFI_TRANSFER, StringManager::GetPCSTRById(TOUCH_TITLEBAR_WIFITRANSFER), GetWindowFontSize(UIShortCutBarBtnIndex));
    m_btnWifiTransfer.SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_STATUSBAR01), ImageManager::GetImage(IMAGE_TOUCH_ICON_STATUSBAR01_WHITE), UIComplexButton::ICON_TOP);
	m_btnWifiTransfer.ShowBorder(false);

    m_btnUserLogin.Initialize(ID_BTN_TOUCH_ACCOUNT_LOGIN, StringManager::GetPCSTRById(TOUCH_TITLEBAR_USERLOGIN), GetWindowFontSize(UIShortCutBarBtnIndex));
    m_btnUserLogin.SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_LOGIN), ImageManager::GetImage(IMAGE_TOUCH_ICON_LOGIN), UIComplexButton::ICON_TOP);
	m_btnUserLogin.ShowBorder(false);
	
	m_btnWebBrowser.Initialize(ID_BTN_TOUCH_WEB_BROWSER,
                               StringManager::GetPCSTRById(TOUCH_TITLEBAR_WEB_BROWSER),
                               GetWindowFontSize(UIShortCutBarBtnIndex));
    m_btnWebBrowser.SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_WEB_BROWSER),
                             ImageManager::GetImage(IMAGE_TOUCH_ICON_WEB_BROWSER),
                             UIComplexButton::ICON_TOP);
	m_btnWebBrowser.ShowBorder(false);

    AddButton(&m_btnWLAN);
    AddButton(&m_btnWifiTransfer);
    AddButton(&m_btnUserLogin);
    AddButton(&m_btnWebBrowser);

	CDisplay *pDisplay = CDisplay::GetDisplay();
	SetMinSize(dkSize(pDisplay->GetScreenWidth(), GetWindowMetrics(UIShortCutBarHeightIndex)));
    SetSplitLineHeight(GetWindowMetrics(UIShortCutBarHeightIndex) - 20);
	MoveWindow(0, GetWindowMetrics(UITitleBarHeightIndex), m_minWidth, m_minHeight);
	Layout();
	UpdateButtonStatus();

    SubscribeMessageEvent(
                WifiManager::WifiAvailableEvent,
                *(WifiManager::GetInstance()), 
                std::tr1::bind(
                    std::tr1::mem_fn(&UIShortcutBar::OnWifiMessageEvent),
                    this,
                    std::tr1::placeholders::_1));
    SubscribeMessageEvent(
                WifiManager::WifiUnavailableEvent,
                *(WifiManager::GetInstance()), 
                std::tr1::bind(
                    std::tr1::mem_fn(&UIShortcutBar::OnWifiMessageEvent),
                    this,
                    std::tr1::placeholders::_1));
    SubscribeMessageEvent(
                CAccountManager::EventAccount,
                *(CAccountManager::GetInstance()), 
                std::tr1::bind(
                    std::tr1::mem_fn(&UIShortcutBar::OnAccountMessageEvent),
                    this,
                    std::tr1::placeholders::_1));
}

UIShortcutBar::~UIShortcutBar()
{
}

void UIShortcutBar::OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam)
{
    switch(_dwCmdId)
    {
    case ID_BTN_TOUCH_WLAN:
		{
			OnWifiPowerClicked();
		}
        break;
    case ID_BTN_TOUCH_WIFI_TRANSFER:
        {
			FtpService::EnableWifiTransfer();
        }
        break;
    case ID_BTN_TOUCH_ACCOUNT_LOGIN:
        {
			OnUserLogClicked();
        }
        break;
    case ID_BTN_TOUCH_WEB_BROWSER:
        {
            OnWebBrowserClicked();
        }
        break;
    default :
        break;
    }
}

bool UIShortcutBar::OnShadowTouch(GESTURE_EVENT _ge, int _x, int _y)
{
    DebugPrintf(DLC_UITITLEBAR, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if ((GESTURE_START == _ge))
        this->Show(false);
    return true;
}

void UIShortcutBar::UpdateButtonStatus()
{
    if (!CAccountManager::GetInstance()->IsLoggedIn())
	{
		m_btnUserLogin.SetText(StringManager::GetPCSTRById(TOUCH_TITLEBAR_USERLOGIN));
	}
	else
	{
		m_btnUserLogin.SetText(StringManager::GetPCSTRById(TOUCH_TITLEBAR_USERLOGOUT));
	}

	if (WifiManager::GetInstance()->GetInstance()->IsPowerOn())
	{
		m_btnWLAN.SetText(StringManager::GetPCSTRById(TOUCH_TITLEBAR_WLANOFF));
	}
	else
	{
		m_btnWLAN.SetText(StringManager::GetPCSTRById(TOUCH_TITLEBAR_WLAN));
	}
}

void UIShortcutBar::OnUserLogClicked()
{
	if (!CAccountManager::GetInstance()->IsLoggedIn())
	{
		UserLogin();
	}
	else
	{
		UserLogOut();
	}
	//UpdateButtonStatus();
	//this->UpdateWindow();
}

void UIShortcutBar::OnWifiPowerClicked()
{
	DebugPrintf(DLC_UITITLEBAR, "%s, %d, %s, %s ID_BTN_TOUCH_WLAN", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
	WifiManager* wifiBase = WifiManager::GetInstance();
	if (wifiBase)
	{
		if(wifiBase->IsPowerOn())
		{
			IDownloader* pDownload = IDownloader::GetInstance();
			if (pDownload)
				pDownload->PauseAllTasks();
			wifiBase->StartPowerOffAsync(UITitleBar::WifiStatusCallBack, UITitleBar::WifiStatusCallBack);
		}
		else
		{
			if (FALSE == wifiBase->IsPowerOn())
			{
				wifiBase->PowerOn();
			}
			CDisplay* pDisplay = CDisplay::GetDisplay();
			if(pDisplay)
			{
				UIWifiDialog pWifiDlg(CDisplay::GetDisplay()->GetCurrentPage(), true);
				pWifiDlg.MoveWindow(0, 0, pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
				pWifiDlg.DoModal();
			}
		}
		UpdateButtonStatus();
		this->UpdateWindow();
	}
}

void UIShortcutBar::UserLogin()
{
	UIUtility::CheckAndReLogin();
}

void UIShortcutBar::UserLogOut()
{
	//longout
	CAccountManager::GetInstance()->LogoutAsync();
}

bool UIShortcutBar::OnWifiMessageEvent(const EventArgs& args)
{
    DebugPrintf(DLC_UITITLEBAR, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!m_bIsVisible)
    {
        DebugPrintf(DLC_UITITLEBAR, "%s, %d, %s, %s !m_bIsVisible", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return true;
    }

    WifiEventArgs wifiArgs = (const WifiEventArgs&)args;
    if (ALLRECEIVER != wifiArgs.m_receiver)
    {
        DebugPrintf(DLC_UITITLEBAR, "%s, %d, %s, %s m_receiver = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, wifiArgs.m_receiver);
        return false;
    }
    UpdateButtonStatus();
    DebugPrintf(DLC_UITITLEBAR, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

bool UIShortcutBar::OnAccountMessageEvent(const EventArgs& args)
{
    DebugPrintf(DLC_UITITLEBAR, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!m_bIsVisible)
    {
        DebugPrintf(DLC_UITITLEBAR, "%s, %d, %s, %s !m_bIsVisible", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return true;
    }

    //AccountEventArgs wifiArgs = (const AccountEventArgs&)args;
    UpdateButtonStatus();
    DebugPrintf(DLC_UITITLEBAR, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

static CommandList GetWebBrowserCommands()
{
    CommandList commands;
    //commands.push_back("book.duokan.com");
    return commands;
}

bool UIShortcutBar::OnWebBrowserClicked()
{
    if (UIUtility::CheckAndReConnectWifi())
    {
        return WebBrowserLauncher::GetInstance()->Exec(GetWebBrowserCommands());
    }
    return false;
}