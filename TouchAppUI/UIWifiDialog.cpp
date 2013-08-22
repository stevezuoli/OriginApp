////////////////////////////////////////////////////////////////////////
// UIWifiDialog.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIWifiDialog.h"
#include "Framework/CDisplay.h"
#include "Framework/CHourglass.h"
#include "GUI/UIMessageBox.h"
#include "GUI/UICompoundListBox.h"
#include "GUI/UISizer.h"
#include "interface.h"
#include "Model/WifiItemModelImpl.h"
#include "TouchAppUI/UIWifiSSIDPasswordDialog.h"
#include "TouchAppUI/UISingleTextInputDialog.h"
#include "CommonUI/UIWifiPasswordDlg.h"
#include <string>
#include <assert.h>
#include <errno.h>
#include "Wifi/WifiCfg.h"
#include "Wifi/WifiManager.h"
#include "Wifi/WiFiQueue.h"
#include "I18n/StringManager.h"
#include "SQM.h"
#include "CommonUI/UIUtility.h"
#include "DownloadManager/IDownloader.h"
#include "Model/UpgradeModelImpl.h"
#include "../SQMUploader/SQMUploader.h"
#include "../Net/MailService.h"
#include "Common/CAccountManager.h"
#include "Common/WindowsMetrics.h"

using namespace std;
using namespace DKAutoUpgrade;
using namespace dk::account;
using namespace dk::bookstore;
using namespace WindowsMetrics;

HRESULT UIWifiAddButton::DrawBackGround(DK_IMAGE drawingImg)
{
	HRESULT hr(S_OK);
	CTpGraphics grf(drawingImg);
	RTN_HR_IF_FAILED(grf.DrawLine(GetLeftMargin(), m_iHeight - 1, m_iWidth, 1, DOTTED_STROKE));
    return S_OK;
}

UIWifiDialog::UIWifiDialog(UIContainer* pParent, bool forTransfer)
    : UIDialog(pParent)
    , m_pPowerOnSizer(NULL)
    , m_btnBackToSetting(this, ID_BTN_TOUCH_BACK)
    , m_iTotalPage(1)
    , m_iCurPage(0)
    , m_iAPNum(0)
    , m_iAPNumPerPage(0)
    , m_ppCurWifiList(NULL)
	, m_bRefreshThreadOn(true)
    , m_bIsDisposed(FALSE)
	, m_bIsForTransfer(forTransfer)
	, m_bIsReentrancy(false)
    , m_refreshing(false)
{
    m_refreshingWifiText.SetFontSize(GetWindowFontSize(UITipFontIndex));
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    SetEventHook(this);
    InitUI();
}
void UIWifiDialog::HandleEvent(const UIEvent & event)
{
    if(event.GetEventId() == HIDE_EVENT)
    {
        OnDispose(m_bIsDisposed);
    }
}
UIWifiDialog::~UIWifiDialog()
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    Dispose();
}

void UIWifiDialog::Dispose()
{
    UIDialog::Dispose();
    OnDispose(m_bIsDisposed);
}

void UIWifiDialog::OnDispose(BOOL bIsDisposed)
{
    if (bIsDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;
    Finalize();
}

void UIWifiDialog::UpdateWifiUI()
{
    if(S_OK != UpdateAPList())
    {
        ShowRefreshingTip(FALSE);
        return;
    }
    ShowRefreshingTip(FALSE);
    Repaint();

    if(!WifiManager::GetInstance()->IsConnected() && m_bIsWifiPowerOn && m_iCurConnectStatus==UIWifiListItem::NoConnect) //Has wifi connected!
    {
		m_textWifiStatus.SetText(StringManager::GetPCSTRById(CONNECT_AUTO));
		m_iCurConnectStatus = UIWifiListItem::Connecting;
        WifiManager::GetInstance()->StartAutoJoin(UITitleBar::WifiStatusCallBack, UITitleBar::ConnectWifiCallBack);
    }

    UIWifiListItem* pItem = m_lstWifi.GetListItemByLabel(m_strCurEssid);
	if (pItem)
		pItem->ChangeWIFIStatus(m_iCurConnectStatus);
}

void UIWifiDialog::ShowRefreshingTip(BOOL showTip)
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if(showTip)
    {
        CString strRefresh(StringManager::GetStringById(REFRESH_WIFI_NET));
        m_refreshingWifiText.SetText(strRefresh);
        
        int stringWidth = m_refreshingWifiText.GetTextWidth();
        if (stringWidth>0)
            m_refreshingWifiText.MoveWindow(CDisplay::GetDisplay()->GetScreenWidth()/2-60, CDisplay::GetDisplay()->GetScreenHeight()/2-50, stringWidth, m_refreshingWifiText.GetTextHeight());   
    }
    m_refreshingWifiText.Show(showTip);
    m_refreshingWifiText.UpdateWindow();
}

void UIWifiDialog::UpdateWifiConnectStatus(UIWifiListItem::ConnectStatus status, string str)
{
	char cText[128] = {'\0'};
	switch (status) {
	case UIWifiListItem::Connected:
		snprintf(cText, DK_DIM(cText), "%s [%s]", StringManager::GetPCSTRById(CONNECT_ALREADY), str.c_str());
		m_textWifiStatus.SetText(cText);
		break;
	case UIWifiListItem::Connecting:
		snprintf(cText, DK_DIM(cText), "%s [%s]", StringManager::GetPCSTRById(CONNECT_WORKING), str.c_str());
		m_textWifiStatus.SetText(cText);
		break;
	case UIWifiListItem::NoConnect:
		m_textWifiStatus.SetText(StringManager::GetPCSTRById(CONNECT_NULL));
		break;		
	case UIWifiListItem::DHCP:
		snprintf(cText, DK_DIM(cText), "%s [%s]",  StringManager::GetPCSTRById(CONNECT_ASSIGN_IP), str.c_str());
		m_textWifiStatus.SetText(cText);
		break;		
	case UIWifiListItem::DHCPFail:
		snprintf(cText, DK_DIM(cText), "%s [%s]", StringManager::GetPCSTRById(CONNECT_ASSIGN_IP_FAIL), str.c_str());
		m_textWifiStatus.SetText(cText);
		break;		
	case UIWifiListItem::DHCPLost:
		snprintf(cText, DK_DIM(cText), "%s [%s]", StringManager::GetPCSTRById(CONNECT_ASSIGN_IP_LOST), str.c_str());
		m_textWifiStatus.SetText(cText);
		break;
	case UIWifiListItem::DHCPOffer:
		snprintf(cText, DK_DIM(cText), "%s [%s]", StringManager::GetPCSTRById(CONNECT_ASSIGN_IP_DHCP_OFFER), str.c_str());
		m_textWifiStatus.SetText(cText);
		break;
	case UIWifiListItem::DHCPNoDNS:
		snprintf(cText, DK_DIM(cText), "%s [%s]", StringManager::GetPCSTRById(CONNECT_ASSIGN_IP_NO_DNS), str.c_str());
		m_textWifiStatus.SetText(cText);
		break;
	case UIWifiListItem::DHCPNoGW:
		snprintf(cText, DK_DIM(cText), "%s [%s]", StringManager::GetPCSTRById(CONNECT_ASSIGN_IP_NO_GW), str.c_str());
		m_textWifiStatus.SetText(cText);
		break;
	}

    // ensure the text is updated
    CDisplay::CacheDisabler forceDraw;
	m_textWifiStatus.UpdateWindow();
	m_iCurConnectStatus = status;
	m_strCurEssid = str;

}

void UIWifiDialog::OnWifiMessage(SNativeMessage event)
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if(!m_bIsVisible || !m_bIsWifiPowerOn)
    {
        return;
    }
    
    if(ALLRECEIVER != event.iParam1)
    {
        LPSTR pReceiver = (LPSTR)event.iParam1;
        
        if(!pReceiver || strcmp(GetClassName(),pReceiver) != 0)
        {
            return;
        }
    }

	//stopRefreshThreads();
    switch(event.iParam2)
    {
    case AUTO_REFRESH:
        {
            DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s  AUTO_REFRESH", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            UpdateWifiUI();
        }
        break;
    case AUTO_REFRESH_SHOWTIP:
        {
            DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s  AUTO_REFRESH_SHOWTIP", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            ShowRefreshingTip(TRUE);
        }
        break;
    case UPDATE_WIFI_STATUS:
        {
            DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s  UPDATE_WIFI_STATUS start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            ShowRefreshingTip(FALSE);
            UINT32 uType = event.iParam3;
            if(uType == WIFIUNAVAILABLE || uType == WIFIAVAILABLE)//若wifi突然断掉，或者连上，如果当前page是wifi对话框，则更新状态
            {
                UpdateWifiUI();
                IDownloader* downloadManager = IDownloader::GetInstance();
                if(uType == WIFIAVAILABLE)
                {
                    // Download manager will resume tasks in wifi queue, don't do it here. 
                    /*if(downloadManager)
                    {
                        downloadManager->ResumeAllPauses();
                    }
					WiFiQueue::GetInstance()->StartHandleQueueTask();*/

					//m_bIsWifiPowerOn = true;
					AccessPoint* pAP = CWifiBaseImpl::GetInstance()->GetCurAP();
					if (pAP) {
						m_strCurEssid = pAP->SSID;
						m_iCurConnectStatus = UIWifiListItem::Connected;
						UIWifiListItem* pItem = m_lstWifi.GetListItemByLabel(m_strCurEssid);
						if (pItem)
							pItem->ChangeWIFIStatus(UIWifiListItem::Connected);
                        
                        // start executing wifi queue.
                        WiFiQueue::GetInstance()->StartHandleQueueTask();
						UpdateWifiConnectStatus(UIWifiListItem::Connected, pAP->SSID);
					}
				}
                else
                {
                    if(downloadManager)
                    {
                        downloadManager->PauseAllTasks();
                    }
                    CAccountManager* pManager = CAccountManager::GetInstance();
                        
                    if(pManager)
                    {
                        if(pManager->IsLoggedIn())
                        {
                            MailService::GetInstance()->StopCheckThread();
                        }
                        pManager->SetNotLoggedIn();
                    }
					//m_bIsWifiPowerOn = false;
					UIWifiListItem* pItem = m_lstWifi.GetListItemByLabel(m_strCurEssid);
					if (pItem)
						pItem->ChangeWIFIStatus(UIWifiListItem::NoConnect);
					UpdateWifiConnectStatus(UIWifiListItem::NoConnect, "");
                }
            }
			UpdateTitleBar();
			Repaint();
        }
        break;
    case CONNECT_WIFI:
        {
            DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s  CONNECT_WIFI", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
			
			bool status_changed = false;
			UIWifiListItem::ConnectStatus status_dhcp;
			
			switch( WifiManager::GetInstance()->GetWiFiStatus())
			{
			case CONNECT_WIFI_DHCP:
				status_changed = true;
				status_dhcp = UIWifiListItem::DHCP;
				break;
			case CONNECT_WIFI_DHCP_FAIL:
				status_dhcp = UIWifiListItem::DHCPFail;
				status_changed = true;
				break;
			case CONNECT_WIFI_DHCP_LOST:
				status_dhcp = UIWifiListItem::DHCPLost;
				status_changed = true;
				break;
			case CONNECT_WIFI_DHCP_OFFER:
				status_dhcp = UIWifiListItem::DHCPOffer;
				status_changed = true;
				break;
			case CONNECT_WIFI_NO_DNS:
				status_dhcp =  UIWifiListItem::DHCPNoDNS;
				status_changed = true;
				break;				
			case CONNECT_WIFI_NO_GW:
				status_dhcp =  UIWifiListItem::DHCPNoGW;
				status_changed = true;
				break;
			case CONNECT_WIFI:
				status_dhcp = UIWifiListItem::Connected;
				status_changed = false;
				break;
			default:
				status_dhcp = UIWifiListItem::Connecting;
				status_changed = false;
				break;
			}

            ConnectionMessage* cm = (ConnectionMessage*)event.iParam3;
            if(cm)
            {
                BOOL connected = cm->GetWifiConnected();
                string ssid = cm->GetWifiSSID();
                string password = cm->GetWifiPassword();
                string strIdentity = cm->GetWifiIdentity();

                delete cm;
                cm = NULL;

                UIWifiListItem* listItemControl = m_lstWifi.GetListItemByLabel(ssid);
				
				if ( !connected && status_changed )
				{
					if (listItemControl)
					{
						listItemControl->ChangeWIFIStatus(status_dhcp);
					}
					UpdateWifiConnectStatus(status_dhcp, ssid);
					return;
				}

                if(!connected && ssid.empty()) //If connection failed when auto join, just return!!!!
                {
					if (listItemControl)
						listItemControl->ChangeWIFIStatus(UIWifiListItem::NoConnect);
					listItemControl = m_lstWifi.GetSelectedItem();
					if (listItemControl)
						listItemControl->ChangeWIFIStatus(UIWifiListItem::NoConnect);
					UpdateWifiConnectStatus(UIWifiListItem::NoConnect, "");
					//startRefreshThreads();
					return;
                }
				
				if (!connected && m_strCurEssid!=ssid)
				{
					if (listItemControl)
					{
						listItemControl->SetIsConn(FALSE);
						listItemControl->ChangeWIFIStatus(UIWifiListItem::NoConnect);
					}
					UpdateWifiConnectStatus(UIWifiListItem::NoConnect, ssid);
					return;
				}
				
				if(NULL == listItemControl)
                {
                    if (!connected)
                    {
                        DebugPrintf(DLC_UIWIFIDIALOG, "No lineitem for ssid: %s", ssid.c_str());
					    //startRefreshThreads();
					    UpdateWifiConnectStatus(UIWifiListItem::NoConnect, "");
                        return;
                    }
                    else
                    {
                        // Successfully connected without displaying any APs, return directly
                        UpdateWifiConnectStatus(UIWifiListItem::Connected, ssid);
                        UpdateTitleBar();
                        WiFiQueue::GetInstance()->StartHandleQueueTask();

                        usleep(1000000);
                        if (m_bIsForTransfer)
						    EndDialog(1);
                        break;
                    }
                }

                m_lstWifi.ClearConnectingStatus();
                if(connected)
                {
                    SQM::GetInstance()->IncCounter(SQM_ACTION_WIFIPAGE_CONNECT_SUCCEED);

					bool check = WifiManager::GetInstance()->IsConnected();

					if(check)
					{
						listItemControl->SetIsConn(true);
						listItemControl->ChangeWIFIStatus(UIWifiListItem::Connected);
                        WiFiQueue::GetInstance()->StartHandleQueueTask();
						UpdateWifiConnectStatus(UIWifiListItem::Connected, ssid);

					}else 
					{
						listItemControl->SetIsConn(false);
						listItemControl->ChangeWIFIStatus(UIWifiListItem::NoConnect);
						UpdateWifiConnectStatus(UIWifiListItem::NoConnect, ssid);
						DebugPrintf(DLC_DIAGNOSTIC, "UIWifiDialog::OnWifiMessage(), check connected ERROR %d" , (int)check);
					}

                    UpdateTitleBar();
                    usleep(500000);

					if (m_bIsForTransfer)
						EndDialog(1);
				}
                else
                {
                    SQM::GetInstance()->IncCounter(SQM_ACTION_WIFIPAGE_CONNECT_FAILED);


                    if(password.empty())
                    {
                        UIMessageBox messagebox(this, StringManager::GetStringById(CONNECT_FAILED), MB_OK | MB_CANCEL);
                        messagebox.SetText(StringManager::GetStringById(CONNECT_RETRY));
						
						int retn = messagebox.DoModal();
						UpdateWindow();
						
						if (MB_OK != retn)
                        {
                            //listItemControl->ChangeWIFIStatus("");
                            listItemControl->ChangeWIFIStatus(UIWifiListItem::NoConnect);
							UpdateWifiConnectStatus(UIWifiListItem::NoConnect, ssid);
							//listItemControl->ChangeWIFIStatus("");
                            listItemControl->SetIsConn(FALSE);
                        }
                        else
                        {

                            listItemControl->ChangeWIFIStatus(UIWifiListItem::Connecting);
                            //listItemControl->ChangeWIFIStatus(StringManager::GetPCSTRById(CONNECT_WORKING));
							UpdateWifiConnectStatus(UIWifiListItem::Connecting, ssid);
                            WifiManager::GetInstance()->StartJoinAPAsync(UITitleBar::WifiStatusCallBack, UITitleBar::ConnectWifiCallBack, ssid);
                        }
                    }
                    else
					{
						char cText[100] = {'\0'};
                        snprintf(cText, DK_DIM(cText), "%s%s", ssid.c_str(), StringManager::GetPCSTRById(CONNECT_FAILED_TRY_AGAIN));
                        eSecurityType security = RESET;
                        CWifiBaseImpl* pWifiBase = CWifiBaseImpl::GetInstance();
                        if (pWifiBase)
                        {
                            pWifiBase->GetSecurityTypeFromSSID(ssid, &security);
                        }
                        UIWifiPasswordDlg wifipasswordDialog(this, cText, (security == IEEE8021X));
                        INT32 endcode = wifipasswordDialog.DoModal();	

						UpdateWindow();

						if( endcode<= 0 )
                        {
                            DebugPrintf(DLC_UIWIFIDIALOG, "UIWifiDialog::OnNotify Break loop because of endcode =%d", endcode);
                            listItemControl->SetIsConn(FALSE);
                            listItemControl->ChangeWIFIStatus(UIWifiListItem::NoConnect);
							UpdateWifiConnectStatus(UIWifiListItem::NoConnect, ssid);
							//listItemControl->ChangeWIFIStatus("");
                        }
                        else
                        {
                            strIdentity.assign(wifipasswordDialog.GetIdentity());
                            password.assign(wifipasswordDialog.GetPassWord());

                            listItemControl->ChangeWIFIStatus(UIWifiListItem::Connecting);
                            //listItemControl->ChangeWIFIStatus(StringManager::GetPCSTRById(CONNECT_WORKING));
							UpdateWifiConnectStatus(UIWifiListItem::Connecting, ssid);
							WifiManager::GetInstance()->StartJoinAPAsync(UITitleBar::WifiStatusCallBack, UITitleBar::ConnectWifiCallBack, ssid, password, strIdentity);
                        }
                        //if(pPassword)
                        //{
                        //    delete[] pPassword;
                        //    pPassword = NULL;
                        //}
                    }

                }
            }
            UpdateTitleBar();
        }
        break;
	case POWERONOFF_WIFI:
		{
            // TODO(pengf): notify all title bar
			//UITitleBar::GetInstance()->UpdateSignal();
			UpdateWifiUI();
			WifiManager* wifiManager = WifiManager::GetInstance();
			if(wifiManager->IsPowerOn())
			{
				if(!wifiManager->IsConnected())
				{
					//m_textWifiStatus.SetText(StringManager::GetPCSTRById(CONNECT_AUTO));
					m_iCurConnectStatus = UIWifiListItem::Connecting;
                    // TODO(pengf): renew wifi status of title bar
					//wifiManager->StartAutoJoin(UITitleBar::WifiStatusCallBack, UITitleBar::GetInstance()->ConnectWifiCallBack);
				}
				else
				{
				}
			}
			else
			{
			}
		}
		break;
				
    case MAN_REFRESH:
        {
            ManUpdateWifiUI();
            m_refreshing = false;
        }
        break;
    case MAN_REFRESH_SHOWTIP:
        {
            ManShowRefreshingTip(S_OK);
            m_refreshing = false;
			// do nothing
        }
        break;
	case CONNECT_WIFI_DHCP:
		break;
    case CONNECT_WIFI_DHCP_FAIL:
		break;
    default:
        break;
    }

    //TODO
    //fix temporarily,passing the event if another page showing
    UIPage* currentPage = CDisplay::GetDisplay()->GetCurrentPage();
    UIWindow* parentWindow = GetParent();
    if (currentPage && parentWindow && currentPage!=parentWindow)
    {
        currentPage->OnWifiMessage(event);
    }
}

void UIWifiDialog::RefreshCallBack()
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s", __FILE__,  __LINE__, "UIWifiDialog", __FUNCTION__);
    SNativeMessage msg;
    msg.iType = KMessageWifiChange;
    msg.iParam1 = (UINT32)GetClassNameStatic();
    msg.iParam2 = AUTO_REFRESH;
    msg.iParam3 = (UINT32)DEFAULT;
    INativeMessageQueue::GetMessageQueue()->Send(msg);
}

void UIWifiDialog::ShowRefreshTipCallBack()
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s", __FILE__,  __LINE__, "UIWifiDialog", __FUNCTION__);
    SNativeMessage msg;
    msg.iType = KMessageWifiChange;
    msg.iParam1 = (UINT32)GetClassNameStatic();
    msg.iParam2 = AUTO_REFRESH_SHOWTIP;
    msg.iParam3 = (UINT32)DEFAULT;
    INativeMessageQueue::GetMessageQueue()->Send(msg);
}

void UIWifiDialog::InitUI()
{
    m_bIsDisposed = FALSE;

	m_textTitle.SetText(StringManager::GetPCSTRById(WIFI_TITLE));
	m_textTitle.SetFontSize(GetWindowFontSize(UIWifiDialogTitleIndex));
	m_textTitle.SetAlign(ALIGN_CENTER);

	m_textWifiStatusTitle.SetText(StringManager::GetPCSTRById(WIFI_STATUS));
	m_textWifiStatusTitle.SetFontSize(GetWindowFontSize(UIWifiDialogSubTitleIndex));
	m_textWifiStatusTitle.SetAlign(ALIGN_LEFT);

	m_btnWifiPowerSwitch.Initialize(ID_BTN_TOUCH_WIFI_SWITCHOFFON, "", GetWindowFontSize(UIWifiDialogButtonIndex));
	m_btnWifiPowerSwitch.SetIcon(ImageManager::GetImage(IMAGE_TOUCH_SWITCH_OFF));
    m_btnWifiPowerSwitch.ShowBorder(false);
	WifiManager* wifiBase = WifiManager::GetInstance();
	if (wifiBase)
	{
		m_bIsWifiPowerOn = wifiBase->IsPowerOn();
	}

	m_textWifiStatus.SetText(StringManager::GetPCSTRById(CONNECT_POWEROFF));
	m_textWifiStatus.SetFontSize(GetWindowFontSize(UIPageNumIndex));
    m_textWifiStatus.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));

	m_textWifiChoices.SetText(StringManager::GetPCSTRById(WIFI_CHOICE));
	m_textWifiChoices.SetFontSize(GetWindowFontSize(UIWifiDialogSubTitleIndex));

    m_btnRefresh.Initialize(ID_BTN_REFRESH, StringManager::GetStringById(REFRESH_NETWORK), GetWindowFontSize(UIWifiDialogButtonIndex));
    m_btnManualAddAP.Initialize(ID_BTN_MANUAL_ADD,StringManager::GetStringById(ADD_WIFI) ,GetWindowFontSize(UIWifiDialogButtonIndex));
    m_btnManualAddAP.SetElemSpacing(0);
    m_btnManualAddAP.SetAlign(ALIGN_LEFT);
	m_btnManualAddAP.SetFontSize(GetWindowFontSize(UIWifiDialogListSSIDIndex));

    m_TotalWifi.SetFontSize(GetWindowFontSize(UIPageNumIndex));

    m_txtPageNo.SetFontSize(GetWindowFontSize(UIPageNumIndex));
	m_txtPageNo.SetEnglishGothic();
    m_txtPageNo.SetAlign(ALIGN_RIGHT);
    m_txtPageNo.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));

	AppendChild(&m_textTitle);
	AppendChild(&m_textWifiStatusTitle);
	AppendChild(&m_btnWifiPowerSwitch);
	AppendChild(&m_textWifiStatus);
	AppendChild(&m_textWifiChoices);
    AppendChild(&m_btnRefresh);
    AppendChild(&m_lstWifi);
    AppendChild(&m_btnManualAddAP);
    AppendChild(&m_txtPageNo);
    AppendChild(&m_titleBar);
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        m_btnRefresh.SetMinSize(dkSize(GetWindowMetrics(UIWifiDialogRefreshBtnWidthIndex),GetWindowMetrics(UIWifiDialogRefreshBtnHeightIndex)));
        m_lstWifi.SetItemHeight(GetWindowMetrics(UIWifiListItemHeightIndex));
        m_lstWifi.SetItemWidth(CDisplay::GetDisplay()->GetScreenWidth() - (GetWindowMetrics(UIHorizonMarginIndex)>>1));

        if (mainSizer)
        {
            mainSizer->Add(&m_titleBar);
            UISizer* topSizer = new UIBoxSizer(dkHORIZONTAL);
            if (topSizer)
            {
                topSizer->Add(&m_btnBackToSetting, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
                topSizer->Add(&m_textTitle, UISizerFlags(1).Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIBackBtnRightBorderIndex)).Align(dkALIGN_CENTER_VERTICAL));
                topSizer->AddSpacer(m_btnBackToSetting.GetMinWidth());
                topSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));
                mainSizer->Add(topSizer, UISizerFlags().Expand());
            }

            mainSizer->AddSpacer(GetWindowMetrics(UIWifiDialogTitleBottomMarginIndex));
            UISizer* wifiPowerSizer = new UIBoxSizer(dkHORIZONTAL);
            if (wifiPowerSizer)
            {
                wifiPowerSizer->Add(&m_textWifiStatusTitle, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
                wifiPowerSizer->Add(&m_btnWifiPowerSwitch, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
                mainSizer->Add(wifiPowerSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
            }

            mainSizer->AddSpacer(GetWindowMetrics(UIWifiDialogWifiStatusLabelTopMarginIndex));
            mainSizer->Add(&m_textWifiStatus,  UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
            mainSizer->AddSpacer(GetWindowMetrics(UIWifiDialogWifiStatusLabelBottomMarginIndex));

            if (!m_pPowerOnSizer)
            {
                m_pPowerOnSizer = new UIBoxSizer(dkVERTICAL);
                if (m_pPowerOnSizer)
                {
                    UISizer* middleSizer = new UIBoxSizer(dkHORIZONTAL);
                    if (middleSizer)
                    {
                        middleSizer->Add(&m_textWifiChoices, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
                        middleSizer->Add(&m_btnRefresh);
                        m_pPowerOnSizer->Add(middleSizer, UISizerFlags().Expand());
                    }

                    m_pPowerOnSizer->AddSpacer(GetWindowMetrics(UIWifiDialogListTopMarginIndex));
                    m_pPowerOnSizer->Add(&m_lstWifi, UISizerFlags(1).Expand());
                    m_pPowerOnSizer->Add(&m_txtPageNo, UISizerFlags().Align(dkALIGN_RIGHT));
                    m_pPowerOnSizer->AddSpacer(GetWindowMetrics(UIWifiDialogBottomHeightIndex));
                    mainSizer->Add(m_pPowerOnSizer, UISizerFlags(1).Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)).Expand());
                }
            }
            SetSizer(mainSizer);
        }
    }
}

void UIWifiDialog::MoveWindow(INT32 _iLeft, INT32 _iTop, INT32 _iWidth, INT32 _iHeight)
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UIWindow::MoveWindow(_iLeft, _iTop, _iWidth, _iHeight);
    Layout();
    if(S_OK != InitMembers())
    {
        return;
    };
}

HRESULT UIWifiDialog::Draw(DK_IMAGE drawingImg)
{
    //DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!m_bIsVisible)
    {
        return S_FALSE;
    }

    if (drawingImg.pbData == NULL)// || !m_IsChanged)
    {
        return E_FAIL;
    }

    UpdateTotalWifiNumber();
    UpdateNavigationButton();

    return UIDialog::Draw(drawingImg);
}

void UIWifiDialog::Finalize()
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_ppCurWifiList != NULL)
    {
        for (INT32 i = 0; i < m_iAPNumPerPage; i++)
        {
            if (m_ppCurWifiList[i] != NULL)
            {
                delete m_ppCurWifiList[i];
            }
        }
        delete[] m_ppCurWifiList;
        m_ppCurWifiList = NULL;
    }
    RemoveAllAP(m_rgpAPList);
    m_iAPNum = 0;
	CHourglass::GetInstance()->Stop();
    DKWifiCfgManager::GetInstance()->AddUserAPToFile(m_userAddedAPs);
    RemoveAllAP(m_userAddedAPs);
}

string UIWifiDialog::GetStrFixLen(string str, const UINT32 length)
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s", __FILE__,  __LINE__, "UIWifiDialog", __FUNCTION__);
    UINT32 size =str.size();

    if(size <= length)
    {
        return str;
    }

    string target = str.substr(0,length-1);

    return target.append("*");
}

void UIWifiDialog::OnNotify(UIEvent event)
{
    //DebugLog(DLC_MESSAGE, " UIWifiDialog::OnNotify start");
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if(event.GetEventId() != CLICK_LISTBOX || event.GetSender() != &m_lstWifi)
    {
        return;
    }

    INT32 selectedItemIndex = event.GetParam();


    INT32 connectedItemIndex = event.GetParam2();



    UIWifiListItem * pConnectedItem = m_lstWifi.GetListItemByIndex(connectedItemIndex);

    UIWifiListItem * listItemControl = m_lstWifi.GetListItemByIndex(selectedItemIndex); //Get the list item control;

    if(NULL == listItemControl || TRUE == listItemControl->GetIsConn())
    {
        return;
    }
    string ssid = listItemControl->GetSSID(); //Get Wifi SSID which will be connected to

    if(FALSE == listItemControl->GetIsProtected())
    {
        if(pConnectedItem)
        {
            pConnectedItem->SetIsConn(FALSE);
            pConnectedItem->ChangeWIFIStatus(UIWifiListItem::NoConnect);
            //pConnectedItem->ChangeWIFIStatus("");
        }
        listItemControl->ChangeWIFIStatus(UIWifiListItem::Connecting);
        //listItemControl->ChangeWIFIStatus(StringManager::GetPCSTRById(CONNECT_WORKING));
		UpdateWifiConnectStatus(UIWifiListItem::Connecting, ssid);

        WifiManager::GetInstance()->StartJoinAPAsync(UITitleBar::WifiStatusCallBack,UITitleBar::ConnectWifiCallBack,ssid);
        return;
    }
    string password = DKWifiCfgManager::GetInstance()->GetPassword(ssid);
    string strIdentity = DKWifiCfgManager::GetInstance()->GetIdentity(ssid);


    BOOL needDlg = password.empty();
    INT32 dlgOK = -1;
    if(needDlg)
    {
		char cText[100] = {'\0'};
        snprintf(cText, DK_DIM(cText), "%s%s", StringManager::GetPCSTRById(CONNECT_TO), ssid.c_str());
        eSecurityType security = RESET;
        CWifiBaseImpl* pWifiBase = CWifiBaseImpl::GetInstance();
        if (pWifiBase)
        {
            pWifiBase->GetSecurityTypeFromSSID(ssid, &security);
        }
        UIWifiPasswordDlg wifidialog(this, cText, (security == IEEE8021X));
        dlgOK = wifidialog.DoModal();
        if (dlgOK == 1)
        {
            password.assign(wifidialog.GetPassWord());
            strIdentity.assign(wifidialog.GetIdentity());
        }
        else
        {
            listItemControl->ChangeWIFIStatus(UIWifiListItem::NoConnect);
            return;
        }
    }

    if (FALSE == needDlg ||( TRUE == needDlg && dlgOK == 1))
    {
        if(pConnectedItem)
        {
            pConnectedItem->SetIsConn(FALSE);
            pConnectedItem->ChangeWIFIStatus(UIWifiListItem::NoConnect);
			//pConnectedItem->ChangeWIFIStatus("");
        }

        listItemControl->ChangeWIFIStatus(UIWifiListItem::Connecting);
        //listItemControl->ChangeWIFIStatus(StringManager::GetPCSTRById(CONNECT_WORKING));
		UpdateWifiConnectStatus(UIWifiListItem::Connecting, ssid);
        DebugPrintf(DLC_UIWIFIDIALOG, "UIWifiDialog::OnNotify selectedItem=%d, SSID=%s, password = %s", selectedItemIndex, ssid.c_str(), password.c_str());
        WifiManager::GetInstance()->StartJoinAPAsync(UITitleBar::WifiStatusCallBack,UITitleBar::ConnectWifiCallBack, ssid, password, strIdentity);
        return;
    }
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

HRESULT UIWifiDialog::InitMembers()
{
    m_iCurPage = 0;
    m_iAPNum = 0;

    m_iTotalPage = 1;

    m_iAPNumPerPage = m_lstWifi.GetMaxDisplayNum();

    assert(m_iAPNumPerPage > 0);

    if(NULL == m_ppCurWifiList)
    {
        m_ppCurWifiList = new CWifiItemModelImpl *[m_iAPNumPerPage];

        if (NULL == m_ppCurWifiList)
        {
            return E_OUTOFMEMORY;
        }
    }
    memset(m_ppCurWifiList, 0, m_iAPNumPerPage * sizeof(CWifiItemModelImpl *));

    m_userAddedAPs = DKWifiCfgManager::GetInstance()->GetUserAPFromFile();
	if (m_bIsWifiPowerOn) 
    {
		UpdateWifiUI();
	}
	UpdateWifiPowerStatus();

    return S_OK;
}

void UIWifiDialog::PowerOnWifi()
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    WifiManager* wifiBase = WifiManager::GetInstance();

    if (!wifiBase->IsPowerOn()) //If wifi hasn't been powered on, power on it!
    {
        wifiBase->StartPowerOnAsync(UITitleBar::WifiStatusCallBack,UITitleBar::WifiStatusCallBack); //time consuming method.
    }

    DebugPrintf(DLC_UIWIFIDIALOG, "UIWifiDialog::PowerOnWifi  END!!!");
}

void UIWifiDialog::UpdateNavigationButton()
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CHAR str[50];
    memset(str, 0, 50);
    sprintf(str, "%d/%d  %s", m_iCurPage + 1, m_iTotalPage,StringManager::GetPCSTRById(BOOK_PAGE));
    m_txtPageNo.SetText(CString(str));
	int iWidth = m_txtPageNo.GetTextWidth();
	int iHeight = m_txtPageNo.GetTextHeight();
	int iLeft = m_iWidth - 30 - iWidth; 
	int iBottom = m_iHeight - 30 - iHeight;
	m_txtPageNo.MoveWindow(iLeft, iBottom, iWidth, iHeight);
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UIWifiDialog::UpdateWifiPowerStatus()
{
	m_btnWifiPowerSwitch.SetIcon(m_bIsWifiPowerOn? ImageManager::GetImage(IMAGE_TOUCH_SWITCH_ON) : ImageManager::GetImage(IMAGE_TOUCH_SWITCH_OFF));
	m_textWifiChoices.SetVisible(m_bIsWifiPowerOn);
	m_btnRefresh.SetVisible(m_bIsWifiPowerOn);
	m_lstWifi.SetVisible(m_bIsWifiPowerOn);
	m_txtPageNo.SetVisible(m_bIsWifiPowerOn);
	if (!m_bIsWifiPowerOn)
	{
		m_btnManualAddAP.SetVisible(false);
	}
	if (!m_bIsWifiPowerOn)
		CHourglass::GetInstance()->Stop();

	if (!m_bIsWifiPowerOn) 
    {
		UIWifiListItem *pItem = m_lstWifi.GetListItemByLabel(m_strCurEssid);
		if (pItem) {
			pItem->ChangeWIFIStatus(UIWifiListItem::NoConnect);
		}
		m_strCurEssid.clear();
		m_iCurConnectStatus = UIWifiListItem::NoConnect;
		m_textWifiStatus.SetText(StringManager::GetPCSTRById(CONNECT_POWEROFF));
		Repaint();
	} 
    else if (!WifiManager::GetInstance()->IsConnected() || !CWifiBaseImpl::GetInstance()->GetCurAP()) 
    {
		m_strCurEssid.clear();
		UpdateWifiConnectStatus(UIWifiListItem::NoConnect, "");
		UpdateWifiUI();
	} 
    else 
    {
		AccessPoint* pAP = CWifiBaseImpl::GetInstance()->GetCurAP();
		if (pAP) {
			m_strCurEssid = pAP->SSID;
			//TODO, set m_iCurConnectStatus by pAP->fIsConnected?
			m_iCurConnectStatus = UIWifiListItem::Connected;
			UpdateWifiConnectStatus(UIWifiListItem::Connected, pAP->SSID);
			UpdateWifiUI();
		}
	}
}

void UIWifiDialog::UpdateTotalWifiNumber()
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CHAR str[50];
    memset(str, 0, 50);
    sprintf(str, "%s %d %s", StringManager::GetPCSTRById(BOOK_TOTAL),m_iAPNum,StringManager::GetPCSTRById(USABLE_NETWORKS));
    m_TotalWifi.SetText(CString(str));
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

HRESULT UIWifiDialog::UpdateAPList()
{
    //DebugPrintf(DLC_DIAGNOSTIC, "UIWifiDialog::UpdateAPList()");
    //DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    RemoveAllAP(m_rgpAPList);

    PowerOnWifi();

    DebugPrintf(DLC_UIWIFIDIALOG, "GetAsyncAPList start size = %d", m_rgpAPList.size());
    WifiManager* wifiBase = WifiManager::GetInstance();
    if (wifiBase)
    {
        INT32 _AP_num = 0;
        AccessPoint** _AP_list = NULL;
        wifiBase->GetAsyncAPList(&_AP_list , &_AP_num);     //Get the wifi list after refreshing
        for (int i = 0; i < _AP_num; ++i)
        {
            AccessPoint* curAP = _AP_list[i];
            if(NULL != curAP && !curAP->SSID.empty())
            {
                m_rgpAPList.push_back(curAP);
            }
        }
        _AP_list = NULL;
    }
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s GetAsyncAPList end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    DebugPrintf(DLC_UIWIFIDIALOG, "GetHistoryWifis start size = %d", m_rgpAPList.size());
    DKWifiCfgManager* pWifiCfgMgr = DKWifiCfgManager::GetInstance();
    if (pWifiCfgMgr)
    {
        vector<WifiAccess*> HistoryAPs = pWifiCfgMgr->GetHistoryWifis();
        size_t historyAPNum = HistoryAPs.size();
        for(size_t i = 0; i < historyAPNum; i++)
        {
            WifiAccess * wa = HistoryAPs[i];
            if (wa && (wa->GetSSID().empty()))
            {
                AccessPoint* newAP = new AccessPoint();
                if(NULL != newAP)
                {
                    newAP->SSID = wa->GetSSID();
                    newAP->Quality = 0;
                    newAP->fPublic = (wa->GetSecurityType() == NONE);
                    m_rgpAPList.push_back(newAP);
                }
            }
            if (wa)
            {
                delete wa;
            }
        }
        HistoryAPs.clear();
    }
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s GetHistoryWifis end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    DebugPrintf(DLC_UIWIFIDIALOG, "m_userAddedAPs size = %d, m_rgpAPList size = %d", m_userAddedAPs.size(), m_rgpAPList.size());
    for (vector<AccessPoint *>::iterator viFrom = m_userAddedAPs.begin(); viFrom != m_userAddedAPs.end(); viFrom++)
    {
        AccessPoint* curAP = (*viFrom);
        DEBUG_POS;
        if(NULL == curAP || curAP->SSID.empty())
        {
            continue;
        }
        AccessPoint *newAP = new AccessPoint(curAP);
        if (newAP)
        {
            m_rgpAPList.push_back(newAP);
        }
    }
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s m_userAddedAPs end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    DebugPrintf(DLC_UIWIFIDIALOG, "DeleteExtraAP start size = %d", m_rgpAPList.size());
    DeleteExtraAP(m_rgpAPList);
    DebugPrintf(DLC_UIWIFIDIALOG, "DeleteExtraAP end size = %d", m_rgpAPList.size());

    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s sort start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    m_iAPNum = m_rgpAPList.size();
    sort(m_rgpAPList.begin(), m_rgpAPList.end(), CompareAPQuality);
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s sort end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
	if (m_iAPNumPerPage==0)
		m_iAPNumPerPage = m_lstWifi.GetMaxDisplayNum();

    if (m_iAPNum <= 0)
    {
        m_iTotalPage = 1;
        m_iCurPage = 0;
    }
    else
    {
        //One rare case here: Before this update, m_iCurPage is the last page(m_iCurPage=1, total page =2)
        //After this update, the total page becomes 1, so the m_iCurPage should change at the same time!
        m_iTotalPage = (m_iAPNum + m_iAPNumPerPage - 1 + 1) / m_iAPNumPerPage;//+1 for m_btnManualAddAP
        if (m_iCurPage >= m_iTotalPage)
        {
            m_iCurPage = m_iTotalPage -1;
        }
    }

    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    INT32 iIndex = 0;
    INT32 iNexPageIndex = (m_iCurPage + 1) == m_iTotalPage ? (m_iAPNum) : (m_iCurPage + 1) * m_iAPNumPerPage;

    for (INT32 i = m_iCurPage * m_iAPNumPerPage; i < iNexPageIndex; i++)
    {
        if (NULL == m_ppCurWifiList[iIndex])
        {
            CWifiItemModelImpl *pAPItemModel = new CWifiItemModelImpl();
            if(NULL == pAPItemModel)
            {
                return E_OUTOFMEMORY;
            }
            m_ppCurWifiList[iIndex] = pAPItemModel;
        }
        if(m_rgpAPList[i] && !m_rgpAPList[i]->SSID.empty())
        {
            SetWifiItemModel(m_ppCurWifiList[iIndex], m_rgpAPList[i]);
            iIndex++;
        }
    }

	if (m_iCurPage == (m_iTotalPage-1))
	{
		int height = m_lstWifi.GetAbsoluteY() + (iNexPageIndex - m_iCurPage*m_iAPNumPerPage)*m_lstWifi.GetItemHeight();
		SPtr<ITpImage> pimgProtectedAP = ImageManager::GetImage(IMAGE_PROCTED_AP);
        int left = pimgProtectedAP->GetWidth() + GetWindowMetrics(UIWifiListItemStatusImgRightMarginIndex) + GetWindowMetrics(UIHorizonMarginIndex);
		m_btnManualAddAP.MoveWindow(left,  height, m_iWidth - left - GetWindowMetrics(UIHorizonMarginIndex), m_lstWifi.GetItemHeight());
		m_btnManualAddAP.SetVisible(true);
	}
	else
	{
		m_btnManualAddAP.SetVisible(false);
	}


    //DebugPrintf(DLC_UIWIFIDIALOG, "UIWifiDialog::UpdateAPList END, m_iAPNum=%d!", m_iAPNum);
    HRESULT ret = m_lstWifi.SetWifiList((IWifiItemModel **)m_ppCurWifiList, iIndex);

    //DebugPrintf(DLC_DIAGNOSTIC, "UIWifiDialog::UpdateAPList END, m_iAPNum=%d!", m_iAPNum);
	return ret;
}

BOOL UIWifiDialog::IsWifiInfoRecorded(string ssid)
{
    vector<WifiAccess*> historyAPs = DKWifiCfgManager::GetInstance()->GetHistoryWifis();
    if (ssid.empty() || historyAPs.empty())
    {
        return FALSE;
    }

    for (UINT32 i = 0; i < historyAPs.size(); i++)
    {
        WifiAccess* ap = historyAPs[i];
        if (NULL != ap && ap->GetSSID() == ssid)
        {
            return TRUE;
        }
    }
    return FALSE;
}

void UIWifiDialog::RemoveWifiInfoRecord(string ssid)
{
    DebugPrintf(DLC_UIWIFIDIALOG, "UIWifiDialog::RemoveUserAddedWifi entering with ssid=%s, size=%d", ssid.c_str(), m_rgpAPList.size());
    if(ssid.empty())
    {
        return;
    }

    RemoveAP(m_rgpAPList, ssid);
    DKWifiCfgManager *pWifiCfgMgr = DKWifiCfgManager::GetInstance();
    if (pWifiCfgMgr)
    {
        WifiAccess* wa = new WifiAccess(ssid, "");
        if(wa)
        {
            pWifiCfgMgr->RemoveWifiConfig(wa);
        }
    }
    DebugPrintf(DLC_UIWIFIDIALOG, "UIWifiDialog::RemoveUserAddedWifi end with size=%d", m_rgpAPList.size());
}

void UIWifiDialog::SetWifiItemModel(CWifiItemModelImpl *pModel, AccessPoint* pAp)
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (NULL == pModel || NULL == pAp || pAp->SSID.empty())
    {
        return;
    }
	
	// 确认当前连接状态, 防止 “未连接” 显示 “已连接”
    pAp->fIsConnected = WifiManager::GetInstance()->IsConnected();

	if(pAp->fIsConnected)
	{
		pAp->fIsConnected = CWifiBaseImpl::GetInstance()->IsConnectedtoLAN();
	}
	
    pModel->SetIsConn(pAp->fIsConnected);
    pModel->SetIsProtected(!pAp->fPublic);
    pModel->SetQualityLv(WifiManager::GetInstance()->QualityToLevel(pAp->Quality));
    pModel->SetSSID(pAp->SSID);
	DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UIWifiDialog::HandlePageUpDown(BOOL fPageDown)
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (fPageDown)
    {
        m_iCurPage++;
    }
    else
    {
        m_iCurPage--;
    }

    if (m_iCurPage < 0)
    {
        m_iCurPage = m_iTotalPage - 1;
    }
    else if (m_iCurPage == m_iTotalPage)
    {
        m_iCurPage = 0;
    }
    m_lstWifi.SetSelectedItem(0);
    UpdateWifiUI();
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UIWifiDialog::WifiPowerSwitch()
{
	m_bIsWifiPowerOn = !m_bIsWifiPowerOn;
	WifiManager* wifiBase = WifiManager::GetInstance();
	if (wifiBase)
	{
		if(wifiBase->IsConnected() || wifiBase->IsPowerOn())
		{
			IDownloader* pDownload = IDownloader::GetInstance();
			if (pDownload)
				pDownload->PauseAllTasks();
			wifiBase->StartPowerOffAsync(UITitleBar::WifiStatusCallBack, UITitleBar::WifiStatusCallBack);
		}
		else
		{
			//WifiManager::GetInstance()->StartRefreshAsync(ShowRefreshTipCallBack,RefreshCallBack);
			wifiBase->StartPowerOnAsync(UITitleBar::WifiStatusCallBack, UITitleBar::WifiPowerOnCallBack);
		}
	}
	UpdateWifiPowerStatus();
	//Repaint();
}

void UIWifiDialog::ManualAddWifi()
{
	SQM::GetInstance()->IncCounter(SQM_ACTION_WIFIPAGE_MANUAL_ADD);
	string ssid, identity, password, sIP, sIPMask, sGateway, sDNS;

	UIWifiSSIDPasswordDialog wifidialog(this);
	INT32 endcode = wifidialog.DoModal();
    ssid = wifidialog.GetSSID();
    password = wifidialog.GetPasword();
	if (!ssid.empty())
	{
		if (endcode ==1)
		{
			//Write wificonfig user added
			WifiAccess* tmpWA = new WifiAccess(ssid, password);
			if(NULL != tmpWA)
			{
                if (!wifidialog.isDynamicIP())
                {
                    sIP = wifidialog.GetIP();
                    sIPMask = wifidialog.GetIPMask();
                    sGateway = wifidialog.GetIPGate();
                    sDNS = wifidialog.GetIPDNS();
                    if(!sIP.empty() && !sIPMask.empty() && !sGateway.empty() && !sDNS.empty())
                    {
                        tmpWA->SetStaticIP(TRUE);
                        tmpWA->SetStaticIPAddress(sIP);
                        tmpWA->SetMask(sIPMask);
                        tmpWA->SetGateway(sGateway);
                        tmpWA->SetDNS(sDNS);
                    }
                }
                DKWifiCfgManager::GetInstance()->SaveWifiConfig(tmpWA);
            }
            //add the new wifi into the wifilist listbox
            AccessPoint * ap = new AccessPoint(ssid, password.empty());
            if(ap != NULL)
            {
            	RemoveAP(m_userAddedAPs, ssid);
                m_userAddedAPs.push_back(ap);
                DKWifiCfgManager *pWifiCfgMgr = DKWifiCfgManager::GetInstance();
                if (pWifiCfgMgr)
                {
                    pWifiCfgMgr->AddUserAPToFile(m_userAddedAPs);
                }
                UpdateWifiUI();
            }

            DebugPrintf(DLC_DIAGNOSTIC, "will connect to ssid=%s, pwd=%s, address=%s, mask=%s, gateway=%s, dns=%s",
                                ssid.c_str(), password.c_str(), sIP.c_str(),
                                sIPMask.c_str(), sGateway.c_str(), sDNS.c_str());
                                m_lstWifi.ClearConnectingStatus();
            UIWifiListItem* pItem = m_lstWifi.GetListItemByLabel(ssid);
            if (pItem)
            {
                pItem->ChangeWIFIStatus(UIWifiListItem::Connecting);
            }
            UpdateWifiConnectStatus(UIWifiListItem::Connecting, ssid);
            WifiManager::GetInstance()->StartJoinAPAsync(
                        UITitleBar::WifiStatusCallBack,
                        UITitleBar::ConnectWifiCallBack, 
                        ssid, 
                        password,
                        identity,
                        UNKNOWN_WIFI_SECURITY_TYPE, 
                        sIP, 
                        sIPMask, 
                        sGateway, 
                        sDNS);
        }
    }
}

void UIWifiDialog::DeleteWifi(int index)
{
    UIWifiListItem *pItem = m_lstWifi.GetListItemByIndex(index);
    UIContainer *pPage = CDisplay::GetDisplay()->GetCurrentPage();
    if (pItem && pItem->IsVisible())
    {
        string ssid = pItem->GetSSID();
        if(!pItem->GetStateMess().empty())
        {
            UIMessageBox messagebox(pPage, StringManager::GetStringById(CONNECTEDWIFINOTDELETABLE), MB_TIPMSG);
            messagebox.DoModal();
        }
        else if(IsWifiAlreadyAdded(ssid))
        {
            UIMessageBox messagebox(pPage, StringManager::GetStringById(WIFI_DELETE_CONFIRM), MB_OK | MB_CANCEL);
            if(MB_OK == messagebox.DoModal())
            {
                RemoveUserAddedWifi(ssid);
                RemoveWifiInfoRecord(ssid);
                UpdateWifiUI();
            }
        }
        else if (IsWifiInfoRecorded(ssid))
        {
            UIMessageBox messagebox(pPage, StringManager::GetStringById(WIFI_DELETE_PWD), MB_OK | MB_CANCEL);
            if(MB_OK == messagebox.DoModal())
            {
                RemoveWifiInfoRecord(ssid);
                UpdateWifiUI();
            }
        }
        else
        {
            UIMessageBox messagebox(pPage, StringManager::GetStringById(WIFINOTDELETABLE), MB_TIPMSG);
            messagebox.DoModal();
        }
        Repaint();
    }
}

void UIWifiDialog::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
	case ID_BTN_TOUCH_BACK:
		UpdateTitleBar();
		EndDialog(1);
		break;
	case ID_BTN_TOUCH_WIFI_SWITCHOFFON:
		WifiPowerSwitch();
		break;
    case ID_BTN_REFRESH:
        if (!m_refreshing)
        {
			// 手动刷新WiFi时， 用户选择连接WiFi，而不是自动连接。 
            m_refreshing = true;
            SQM::GetInstance()->IncCounter(SQM_ACTION_WIFIPAGE_REFRESH);
			WifiManager::GetInstance()->StartRefreshAsync(ManShowRefreshTipCallBack,ManRefreshCallBack);
        }
        break;
    case ID_BTN_MANUAL_ADD:
        {
			ManualAddWifi();
            break;
        }
    case ID_LIST_PAGEUP:
        HandlePageUpDown(false);
        break;
    case ID_LIST_PAGEDOWN:
        HandlePageUpDown(true);
        break;
    case ID_TOUCH_DELETE_USERDEFINEDWIFI:
        if (pSender == &m_lstWifi)
        {
            DeleteWifi((int)dwParam);
        }
        break;
    default:
        break;
    }
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

BOOL UIWifiDialog::IsWifiAlreadyAdded(string ssid)
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if(ssid.empty() || m_userAddedAPs.empty())
    {
        return FALSE;
    }
    for(UINT32 i = 0; i < m_userAddedAPs.size(); i++)
    {
        AccessPoint * ap = m_userAddedAPs[i];
        if(NULL != ap && ap->SSID == ssid)
        {
            return TRUE;
        }
    }
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return FALSE;
}

void UIWifiDialog::RemoveAP(std::vector<AccessPoint*>& APList, std::string ssid)
{
    if(ssid.empty())
    {
        return;
    }

    for (std::vector<AccessPoint *>::iterator vi = APList.begin(); vi != APList.end();)
    {
        if ((*vi) && (0 == strcmp((*vi)->SSID.c_str(), ssid.c_str())))
        {
            delete (*vi);
            (*vi) = NULL;
            APList.erase(vi);
            continue;
        }
        vi++; 
    }
}

void UIWifiDialog::RemoveAllAP(std::vector<AccessPoint*>& APList)
{
    for (std::vector<AccessPoint *>::iterator vi = APList.begin(); vi != APList.end(); vi++)
    {
        delete (*vi);
        (*vi) = NULL;
    }
    APList.clear();
}

void UIWifiDialog::DeleteExtraAP(std::vector<AccessPoint*>& APList)
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s start size = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, APList.size());
    for (std::vector<AccessPoint *>::iterator vi = APList.begin(); vi != APList.end();)
    {
        if ((NULL == (*vi)) || ((*vi)->SSID.empty()))
        {
            delete (*vi);
            APList.erase(vi);
        }
        else
        {
            for (std::vector<AccessPoint *>::iterator viNext = (vi + 1); viNext != APList.end();)
            {
                if ((NULL == (*viNext)) || ((*viNext)->SSID.empty()) || (0 == strcmp((*vi)->SSID.c_str(), (*viNext)->SSID.c_str())))
                {
                    delete (*viNext);
                    APList.erase(viNext);
                }
                else
                {
                    viNext++;
                }
            }
            vi++; 
        }
    }
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s end size = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, APList.size());
}

bool UIWifiDialog::CompareAPQuality(AccessPoint* AP1, AccessPoint* AP2)
{
    if (AP1 && AP2)
    {
        return (AP1->Quality >= AP2->Quality);
    }
    return false;
}

void UIWifiDialog::RemoveUserAddedWifi(string ssid)
{
    DebugPrintf(DLC_UIWIFIDIALOG, "UIWifiDialog::RemoveUserAddedWifi entering with ssid=%s, size=%d", ssid.c_str(), m_userAddedAPs.size());
    if(ssid.empty() || m_userAddedAPs.empty())
    {
        return;
    }

    RemoveAP(m_userAddedAPs, ssid);
    DKWifiCfgManager *pWifiCfgMgr = DKWifiCfgManager::GetInstance();
    if (pWifiCfgMgr)
    {
        pWifiCfgMgr->AddUserAPToFile(m_userAddedAPs);
    }

    DebugPrintf(DLC_UIWIFIDIALOG, "UIWifiDialog::RemoveUserAddedWifi end with size=%d", m_userAddedAPs.size());
}

void UIWifiDialog::OnLoad()
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    SetVisible(TRUE);
    if(S_OK != UpdateAPList())
    {
        DebugPrintf(DLC_UIWIFIDIALOG, "UIWifiDialog::OnLoad something is wrong with UpdateAPList, may be out of memory!!!");
        return;
    }
}

void UIWifiDialog::OnUnLoad()
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    SetVisible(FALSE);
}

void UIWifiDialog::UpdateTitleBar()
{
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UITitleBar *pTitleBar = &m_titleBar;
    if(pTitleBar && pTitleBar->IsVisible())
    {
        pTitleBar->UpdateSignal();
    }
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UIWifiDialog::ManUpdateWifiUI()
{
	HRESULT retn = UpdateAPList();
	int errCode = (int)retn;

	switch(retn)
	{
	case S_OK:
		m_refreshingWifiText.Show(FALSE);
		m_refreshingWifiText.UpdateWindow();
		break;
	case E_OUTOFMEMORY:
		DebugPrintf(DLC_DIAGNOSTIC, "UIWifiDialog::ManUpdateWifiUI(), UpdateAPList() OUT OF MEMORY %08X", retn);
		ManShowRefreshingTip(errCode);
		return;
	case E_FAIL:
		DebugPrintf(DLC_DIAGNOSTIC, "UIWifiDialog::ManUpdateWifiUI(), UpdateAPList() E_FAIL %08X", retn);
		ManShowRefreshingTip(errCode);
		return;
	default:
		DebugPrintf(DLC_DIAGNOSTIC, "UIWifiDialog::ManUpdateWifiUI(), UpdateAPList() ERROR %08X" , retn);
		ManShowRefreshingTip(errCode);
		return;
	}
}

void UIWifiDialog::ManShowRefreshingTip(int errCode)
{
	SOURCESTRINGID stringid;

	switch(errCode)
	{
	case S_OK:
		stringid = REFRESH_WIFI_NET;
		break;
	case E_OUTOFMEMORY:		
		stringid = REFRESH_WIFI_NOMEM;
		break;
	case E_FAIL:
		stringid = REFRESH_WIFI_FAIL;
		break;
	default:
		stringid = REFRESH_WIFI_FAIL;
		break;
	}
	
	CString strRefresh(StringManager::GetStringById(stringid));
	m_refreshingWifiText.SetText(strRefresh);
	int stringWidth = m_refreshingWifiText.GetTextWidth();

	m_refreshingWifiText.MoveWindow(CDisplay::GetDisplay()->GetScreenWidth()/2-60, CDisplay::GetDisplay()->GetScreenHeight()/2-50, stringWidth, m_refreshingWifiText.GetTextHeight());
    m_refreshingWifiText.Show(TRUE);
    m_refreshingWifiText.UpdateWindow();
	
}


void UIWifiDialog::ManRefreshCallBack()
{
    SNativeMessage msg;
    msg.iType = KMessageWifiChange;
    msg.iParam1 = (UINT32)GetClassNameStatic();
    msg.iParam2 = MAN_REFRESH;
    msg.iParam3 = (UINT32)DEFAULT;
    INativeMessageQueue::GetMessageQueue()->Send(msg);
}

void UIWifiDialog::ManShowRefreshTipCallBack()
{
    SNativeMessage msg;
    msg.iType = KMessageWifiChange;
    msg.iParam1 = (UINT32)GetClassNameStatic();
    msg.iParam2 = MAN_REFRESH_SHOWTIP;
    msg.iParam3 = (UINT32)DEFAULT;
    INativeMessageQueue::GetMessageQueue()->Send(msg);
}
