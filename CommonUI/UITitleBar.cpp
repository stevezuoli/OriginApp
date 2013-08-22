
////////////////////////////////////////////////////////////////////////
// UITitleBar.cpp
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include <tr1/functional>
#include "Wifi/WifiManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/FontManager.h"
#include "Framework/CDisplay.h"
#include "Utility/ThreadHelper.h"
#include "Utility/SystemUtil.h"
#include "PowerManager/PowerManager.h"
#include "Common/ConfigInfo.h"
#include "Common/CAccountManager.h"
#include "I18n/StringManager.h"
#include "Common/WindowsMetrics.h"
#include "../Net/MailService.h"
#include "CommonUI/UITitleBar.h"
#include "TouchAppUI/UIWifiDialog.h"
#include "TouchAppUI/UIShortCutPage.h"
#include "Utility/PathManager.h"
#include "BookStore/BookDownloadController.h"
#include "GUI/GUISystem.h"

using namespace std;
using namespace dk::account;
using namespace dk::bookstore;
using namespace dk::utility;
using namespace WindowsMetrics;

/****************************************************************
* UITitleBar
****************************************************************/
    
UITitleBar::UITitleBar()
    : UIWindow(NULL, IDSTATIC)
    , m_iWifiLevel(0)
    , m_iBatteryLevel(1)
    , m_iWifiMaxLevel(5)
    , m_iBatteryMaxLevel(10)
    , m_iDownLoadMaxStatus(10)
    , m_iAliasUsableWidth(-1)
    , m_imgWifiIcon()
    , m_imgDownLoadIcon()
    , m_iDownLoadStatus(0)
{
    m_bIsVisible = true;
    m_bIsTabStop = true;
    m_iLeft = 0;
    m_iTop = 0;
    m_iWidth = CDisplay::GetDisplay()->GetScreenWidth();
    m_iHeight = GetWindowMetrics(UITitleBarHeightIndex);
    m_minWidth = m_iWidth;
    m_minHeight = m_iHeight;

    m_iTopOffset = GetWindowMetrics(UITitleBarTopOffSetIndex);
    m_iElementSpace = GetWindowMetrics(UITitleBarElementSpaceIndex);
    m_imgMails = ImageManager::GetImage(IMAGE_TOUCH_ICON_PUSH_MESSAGE);
    m_imgBatteryIcon = ImageManager::GetImage(IMAGE_TOUCH_ICON_STATUS_BATTERY1);
    m_imgWifiIcon = ImageManager::GetImage(IMAGE_TOUCH_ICON_STATUS_WIFI);
    m_imgDownLoadIcon = ImageManager::GetImage(IMAGE_TITLEBAR_DOWNLOADSTATUSICON);
    m_imgLogo = ImageManager::GetImage(IMAGE_TOUCH_ICON_STATUS_DUOKAN);

    m_flingGestureListener.SetTitleBar(this);
    m_gestureDetector.SetListener(&m_flingGestureListener);
    SetLeftMargin(GetWindowMetrics(UIHorizonMarginIndex));

    SubscribeMessageEvent(MailService::EventMailRecv, *MailService::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UITitleBar::OnSignalChanged),
        this,
        std::tr1::placeholders::_1));

    SubscribeMessageEvent(CAccountManager::EventAccount, *CAccountManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UITitleBar::OnLogStatusUpdate),
        this,
        std::tr1::placeholders::_1));

    SubscribeMessageEvent(CAccountManager::EventXiaomiUserCardUpdate, *CAccountManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UITitleBar::OnXiaomiUserCardReceived),
        this,
        std::tr1::placeholders::_1));

    SubscribeMessageEvent(
            BookDownloadController::EventDownloadBookUpdated,
            *BookDownloadController::GetInstance(),
            std::tr1::bind(
                std::tr1::mem_fn(&UITitleBar::OnDownloadUpdate),
                this,
                std::tr1::placeholders::_1));
}

UITitleBar::~UITitleBar()
{ 
    UIWindow::Dispose();
}

void UITitleBar::UpdateTime()
{
    DebugPrintf(DLC_UITITLEBAR, "%s, %d, %s, %s", __FILE__,  __LINE__, "UITitleBar", __FUNCTION__);
    time_t rawtime;
    struct tm *info = NULL;
    char timestr[16] = {0};
    time ( &rawtime );
    info = localtime ( &rawtime );
    if(SystemSettingInfo::GetInstance()->m_Time.GetTimeSystem())
    {
        strftime(timestr,16,"%H:%M",info);
    }
    else
    {
        strftime(timestr,16,"%I:%M",info);
    }

    m_strTime.assign(timestr);
    return;
}

void UITitleBar::UpdateMailRecvInfo()
{
    m_strMailInfo = MailService::GetInstance()->GetMailCheckInfo();
}

void UITitleBar::UpdateWifiSignalLevel()
{
    // m_iWifiLevel should biger zero and lower than m_iWifiMaxLevel
    m_iWifiLevel = WifiManager::GetInstance()->GetActiveSignalLevel();;
    return ;
}

void UITitleBar::UpdateAccountAlias()
{
    CAccountManager* pManager = CAccountManager::GetInstance();
	if(pManager)
	{
		CAccountManager::LoginStatus curStatus = pManager->GetLoginStatus();
		switch(curStatus)
		{
			case CAccountManager::LoggedIn_XiaoMi:
			case CAccountManager::LoggedIn_DuoKan:
				{
					m_strAlias = "  |  ";
	        		m_strAlias.append(pManager->GetAliasFromFile());
				}
				break;
			case CAccountManager::Logging:
				{
					m_strAlias = "  |  ";
	        		m_strAlias.append(StringManager::GetStringById(ACCOUNT_LOGINING_TITLEBAR_TIP));
				}
				break;
            case CAccountManager::NotLoggedIn:
            default:
                {
                    m_strAlias.clear();
                }
                break;
		}
	}
}

void UITitleBar::UpdateBatteryLevel()
{
    const int batteryRatio = PowerManagement::DKPowerManagerImpl::GetInstance()->GetPowerLevel();
    if(SystemSettingInfo::GetInstance()->GetBatteryRatioMode())
    {
        char cRatio[10] = {0};
        snprintf(cRatio, 10, "%d%%",batteryRatio);
        m_strBatteryRatio.assign(cRatio);
    }
    else
    {
        m_strBatteryRatio.clear();
    }

    if(PowerManagement::DKPowerManagerImpl::GetInstance()->IsCharging())
    {
        m_iBatteryLevel = m_iBatteryMaxLevel;
    }
    else
    {
        if (batteryRatio > 90)
        {
            m_iBatteryLevel = m_iBatteryMaxLevel - 1;
        }
        else
        {
            m_iBatteryLevel =  (m_iBatteryMaxLevel - 1) * batteryRatio / 100 ;
            m_iBatteryLevel += 1;
        }
    }

    return ;
}

int UITitleBar::GetCurrentDownloadNum()
{
    return BookDownloadController::GetInstance()->GetAllActiveDownloads();
}

void UITitleBar::UpdateDownLoadStauts()
{
    // m_iDownLoadStatus should biger zero and lower than 10 (1~10)
    //IDownloader *_pDownloadManager = DownloaderImpl::GetInstance();
    //if(_pDownloadManager)
    //{
        //int downLoadingProgress = 0;
        //m_iDownLoadStatus = _pDownloadManager->GetDownloadTaskNumberByType(downLoadingProgress,
        //    IDownloadTask::WAITING | IDownloadTask::WORKING | IDownloadTask::PAUSED | IDownloadTask::UPDATED | IDownloadTask::CURL_DONE,
        //    IDownloadTask::BOOK | IDownloadTask::VIDEO | IDownloadTask::AUDIO | IDownloadTask::UPGRADEPACKAGE);
    
        m_iDownLoadStatus = GetCurrentDownloadNum();
        int currentDownloadProgress = BookDownloadController::GetInstance()->GetCurrentDownloadingProgress();
        if(m_iDownLoadStatus > m_iDownLoadMaxStatus)
        {
            m_iDownLoadStatus = m_iDownLoadMaxStatus;
        }
        else if(m_iDownLoadStatus < 0)
        {
            m_iDownLoadStatus = 0;
        }

        if (currentDownloadProgress > 0 && currentDownloadProgress < 100)
        {
            char progress[10] = {0};
            snprintf(progress, 10, "%d%%", currentDownloadProgress);
            m_strDownLoadingProgress.assign(progress);
        }
        else
        {
            m_strDownLoadingProgress.clear();
        }
    //}
    //else
    //{
    //    m_iDownLoadStatus = 0;
    //    m_strDownLoadingProgress.clear();
    //}
    return ;
}

void UITitleBar::PrepareSourcePosition()
{
    DKFontAttributes fontattr;
    fontattr.SetFontAttributes(0, 0, GetWindowFontSize(FontSize18Index));
    ITpFont* pFont = FontManager::GetInstance()->GetFont(fontattr, ColorManager::knWhite);
    if (!pFont)
    {
        return;
    }

    for ( int i=0; i<ElementsCounts; ++i)
    {
        m_elementsXCoordinate[i] = -1;
    }

    int left = GetWindowMetrics(UIHorizonMarginIndex);
    int right = m_iWidth - GetWindowMetrics(UIHorizonMarginIndex);
    const int batteryRatioLeftMargin = m_iElementSpace + 1;
    const int batteryImageRightMargin = GetWindowMetrics(UIPixelValue5Index);

    if (SystemSettingInfo::GetInstance()->GetBatteryRatioMode())
    {
        right -= pFont->StringWidth(m_strBatteryRatio.c_str());
        m_elementsXCoordinate[BatteryInfo] = right;
        right -= batteryRatioLeftMargin;
    }

    right -= m_imgBatteryIcon->bmWidth;
    m_elementsXCoordinate[BatteryImage] = right;
    right -= batteryImageRightMargin;

    right -= pFont->StringWidth(m_strTime.c_str());
    m_elementsXCoordinate[TimeInfo] = right;
    right -= m_iElementSpace;

    if (m_imgWifiIcon && m_iWifiLevel)
    {
        right -= m_imgWifiIcon->bmWidth;
        m_elementsXCoordinate[WifiImage] = right;
        right -= (m_iElementSpace << 1);
    }

    if (m_strDownLoadingProgress.length() > 0)
    {
        right -= pFont->StringWidth(m_strDownLoadingProgress.c_str());
        m_elementsXCoordinate[DownloadInfo] = right;
        right -= m_iElementSpace;
    }

    if (m_imgDownLoadIcon && m_iDownLoadStatus)
    {
        right -= m_imgDownLoadIcon->bmWidth;
        m_elementsXCoordinate[DownloadImage] = right;
        right -= (m_iElementSpace << 1);
    }

    if (m_imgLogo)
    {
        m_elementsXCoordinate[LogoImage] = left;
        left += m_imgLogo->bmWidth;
    }

    int accountUsedWidth = 0;
    if (m_strAlias.length() > 0)
    {
        m_elementsXCoordinate[AccountInfo] = left;
        accountUsedWidth = pFont->StringWidth(m_strAlias.c_str());
    }

    int mailUsedWidth = 0;
    int imgMailsWidth = m_imgMails->GetWidth();
    if (WifiManager::GetInstance()->IsConnected() && CAccountManager::GetInstance()->IsLoggedIn())
    {
        mailUsedWidth += (m_iElementSpace << 1);
        if (m_imgMails)
        {
            m_elementsXCoordinate[MailImage] = left + accountUsedWidth + mailUsedWidth;
            mailUsedWidth += (imgMailsWidth + m_iElementSpace);
        }
        if (m_strMailInfo.length() > 0)
        {
            m_elementsXCoordinate[MailInfo] = left + accountUsedWidth + mailUsedWidth;
            mailUsedWidth += pFont->StringWidth(m_strMailInfo.c_str());
        }
    }

    if ((left + accountUsedWidth + mailUsedWidth) < right)
    {
        SetLeftMargin(GetWindowMetrics(UIHorizonMarginIndex));
        m_iAliasUsableWidth = -1;
    }
    else
    {
        int shrinkMargin = GetWindowMetrics(UIHorizonMarginIndex) >> 1;
        SetLeftMargin(shrinkMargin);
        m_iAliasUsableWidth = right - left - mailUsedWidth;
        m_iAliasUsableWidth += (shrinkMargin << 1);//缩小的两侧的边距值增加到这里
        m_elementsXCoordinate[LogoImage] -= shrinkMargin;
        m_elementsXCoordinate[AccountInfo] -= shrinkMargin;
        m_elementsXCoordinate[MailImage] = m_elementsXCoordinate[AccountInfo] + m_iAliasUsableWidth + (m_iElementSpace << 1);
        m_elementsXCoordinate[MailInfo] = m_elementsXCoordinate[MailImage] + imgMailsWidth + m_iElementSpace;

        for ( int i=DownloadImage; i<ElementsCounts; ++i)
        {
            m_elementsXCoordinate[i] += shrinkMargin;
        } 
    }
}

void UITitleBar::PrepareSource()
{
    UpdateTime();
    UpdateWifiSignalLevel();
    UpdateBatteryLevel();
    UpdateDownLoadStauts();
    UpdateMailRecvInfo();
    UpdateAccountAlias();
    PrepareSourcePosition();
    return;
}

HRESULT UITitleBar::ClearTitleBar()
{
    DebugPrintf(DLC_UITITLEBAR, "%s, %d, %s, %s", __FILE__,  __LINE__, "UITitleBar", __FUNCTION__);
    CDisplay* display = CDisplay::GetDisplay();
    if(display)
    {
        DK_IMAGE pGraphics = display->GetMemDC();
        if (pGraphics.pbData == NULL)
        {
            return E_FAIL;
        }
        HRESULT hr(S_OK);
        DK_IMAGE imgSelf;
        DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
        RTN_HR_IF_FAILED(CropImage(
            pGraphics,
            rcSelf,
            &imgSelf
            ));

        CTpGraphics grf(imgSelf);
        grf.FillRect(0,0,m_iWidth, m_iHeight, ColorManager::knWhite);

        
        display->StopFullRepaintOnce();
        Repaint();
        return hr;
    }
    return E_FAIL;
}

HRESULT UITitleBar::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
    {
        return S_OK;
    }
    if (drawingImg.pbData == NULL)
    {
        return E_FAIL;
    }
    PrepareSource();

    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        drawingImg,
        rcSelf,
        &imgSelf
        ));

    CTpGraphics grf(imgSelf);
    grf.FillRect(0,0,m_iWidth,m_iHeight, ColorManager::knBlack);
    
    DKFontAttributes fontattr;
    fontattr.SetFontAttributes(0,0,GetWindowFontSize(FontSize18Index));
    ITpFont* pFont = FontManager::GetInstance()->GetFont(fontattr, ColorManager::knWhite);
    if (NULL == pFont)
        return E_FAIL;

    const int iFontHeight = pFont->GetHeight();  
    const int iStringTop  = (m_iHeight > iFontHeight) ? ((m_iHeight - iFontHeight) >> 1) : 0; 
    if(SystemSettingInfo::GetInstance()->GetBatteryRatioMode())
    {
        // 电量百分比显示
        grf.DrawStringUtf8(m_strBatteryRatio.c_str(), 
            m_elementsXCoordinate[BatteryInfo],
            iStringTop + m_iTopOffset, 
            pFont); 
    }
    
    // 电量图标显示 
    const int iBatteryImgHeight = (m_imgBatteryIcon->bmHeight + m_iBatteryMaxLevel - 1)/m_iBatteryMaxLevel;
    const int iBatteryImgWidth  = m_imgBatteryIcon->bmWidth;
    const int iBatteryTop = (m_iHeight - iBatteryImgHeight) >> 1;
    RTN_HR_IF_FAILED(grf.DrawImageBlend(m_imgBatteryIcon.Get(),
        m_elementsXCoordinate[BatteryImage],
        iBatteryTop,
        0,
        iBatteryImgHeight * (m_iBatteryLevel - 1),
        iBatteryImgWidth,
        iBatteryImgHeight));
    
    // 时间显示 
    grf.DrawStringUtf8(m_strTime.c_str(), m_elementsXCoordinate[TimeInfo],iStringTop + m_iTopOffset,pFont);
    
    // wifi显示
    if(m_imgWifiIcon && m_iWifiLevel)
    {
        const int iWifiIconWidth  = m_imgWifiIcon->bmWidth;
        const int iWifiIconHeight = (m_imgWifiIcon->bmHeight + m_iWifiMaxLevel - 1)/ m_iWifiMaxLevel;
        const int iWifiIconTop    = (m_iHeight > iWifiIconHeight) ? ((m_iHeight - iWifiIconHeight) >> 1) : 0; 
        RTN_HR_IF_FAILED(grf.DrawImageBlend(m_imgWifiIcon.Get(),
            m_elementsXCoordinate[WifiImage],
            iWifiIconTop,
            0,
            iWifiIconHeight * (m_iWifiLevel - 1),
            iWifiIconWidth,
            iWifiIconHeight));
    }   
    
    // 下载进度显示
    if (m_strDownLoadingProgress.length() > 0)
    {
        RTN_HR_IF_FAILED(grf.DrawStringUtf8(m_strDownLoadingProgress.c_str(), m_elementsXCoordinate[DownloadInfo], iStringTop + m_iTopOffset, pFont));
    }

    // 下载显示
    if(m_imgDownLoadIcon && m_iDownLoadStatus)
    {
        const int iDownLoadWidth = m_imgDownLoadIcon->bmWidth;
        const int iDownLoadHeight = (m_imgDownLoadIcon->bmHeight) /m_iDownLoadMaxStatus;
        const int iDownLoadTop    = (m_iHeight > iDownLoadHeight) ? ((m_iHeight - iDownLoadHeight) >> 1) : 0; 
        RTN_HR_IF_FAILED(grf.DrawImageBlend(m_imgDownLoadIcon.Get(),
            m_elementsXCoordinate[DownloadImage],
            iDownLoadTop,
            0,
            iDownLoadHeight * (m_iDownLoadStatus -1),
            iDownLoadWidth,
            iDownLoadHeight));
    }

    //logo 显示
    if (m_imgLogo)
    {
        const int iLogoTop = (m_iHeight > (int)m_imgLogo->bmHeight) ? ((m_iHeight - (int)m_imgLogo->bmHeight) >> 1) : 0;
        RTN_HR_IF_FAILED(grf.DrawImageBlend(m_imgLogo.Get(),
            m_elementsXCoordinate[LogoImage],
            iLogoTop,
            0,
            0,
            m_imgLogo->bmWidth,
            m_imgLogo->bmHeight));
    }

    // 账户名显示
    if (m_iAliasUsableWidth == -1)
    {
        if(m_strAlias.length() && pFont)
        {
            grf.DrawStringUtf8(m_strAlias.c_str(), m_elementsXCoordinate[AccountInfo], iStringTop, pFont);
        }
    }
    else
    {
        DK_IMAGE imgAccountSelf;
        DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_elementsXCoordinate[AccountInfo] + m_iAliasUsableWidth, m_iTop + m_iHeight};
        RTN_HR_IF_FAILED(CropImage(
                    drawingImg,
                    rcSelf,
                    &imgAccountSelf
                    ));
        CTpGraphics grfAccount(imgAccountSelf);
        if(m_strAlias.length() && pFont)
        {
            grfAccount.DrawStringUtf8(m_strAlias.c_str(), m_elementsXCoordinate[AccountInfo], iStringTop, pFont);
        }
    }

    //推送显示
    if (WifiManager::GetInstance()->IsConnected() && CAccountManager::GetInstance()->IsLoggedIn() && !m_strMailInfo.empty())
    {
        if (m_imgMails)
        {
            int imgMailsWidth = m_imgMails->GetWidth();
            int imgMailsHeight = m_imgMails->GetHeight();
            int imgMailsTop = m_iHeight > imgMailsHeight ? (m_iHeight - imgMailsHeight) >> 1 : 0;
            grf.DrawImageBlend(m_imgMails.Get(), m_elementsXCoordinate[MailImage], imgMailsTop, 0, 0, imgMailsWidth, imgMailsHeight);
        }
        if (m_strMailInfo.length() > 0)
        {
            grf.DrawStringUtf8(m_strMailInfo.c_str(), m_elementsXCoordinate[MailInfo], iStringTop + 1, pFont);
        }
    }
    
    return hr;
}

void UITitleBar::UpdateSignal()
{
    // 某些时候UITitleBar只是被声明，并没有被AddChild加到某个container中，此时不应Update
    if (NULL == GetParent())
    {
        return;
    }
#if 1
    UIWindow* top = this;
    while (top != NULL && top->GetParent() != NULL)
    {
        top = top->GetParent();
    }
    // 如果不是顶层窗口，则不repaint
    if (top != NULL && CPageNavigator::IsInStack(top) && !CPageNavigator::IsOnTop(top))
    {
        return;
    }
#endif
    CDisplay* display = CDisplay::GetDisplay();
    if(display)
    {
        DK_IMAGE pGraphics = display->GetMemDC();
        Draw(pGraphics);
        display->StopFullRepaintOnce();
        Repaint();
    }
}

bool UITitleBar::OnSignalChanged(const EventArgs& args)
{
    UpdateSignal();
    return true;
}

bool UITitleBar::OnDownloadUpdate(const EventArgs& args)
{
    static time_t last_update_time = 0;
    static const int UPDATE_OVERDUE = 1500;
    if (!IsDisplay())
    {
        return false;
    }
    const BookDownloadStateUpdateArgs& updateArgs = (const BookDownloadStateUpdateArgs&)args;
    string urlID = updateArgs.task_id;
    int    state = updateArgs.state;
    int    download_progress = updateArgs.progress;

    bool needUpdateByState = (m_currentDownloadTaskID == urlID &&
                              (state & (IDownloadTask::CANCELED |
                                        IDownloadTask::PAUSED |
                                        IDownloadTask::FAILED |
                                        IDownloadTask::WAITING_QUEUE)));

    if ((download_progress >= 0 && !urlID.empty()) || needUpdateByState)
    {
        m_currentDownloadTaskID = urlID;
        time_t current_time = SystemUtil::GetUpdateTimeInMs();
        if (last_update_time == 0 ||
            current_time - last_update_time >= UPDATE_OVERDUE ||
            download_progress >= 100)
        {
            last_update_time = current_time;
            UpdateSignal();
        }
    }
    return true;

}

bool UITitleBar::OnLogStatusUpdate(const EventArgs& args)
{
    UpdateAccountAlias();
    CDisplay* display = CDisplay::GetDisplay();
    if(display)
    {
        DK_IMAGE pGraphics = display->GetMemDC();
        Draw(pGraphics);
        display->StopFullRepaintOnce();
        Repaint();
    }
    return true;
}

bool UITitleBar::OnXiaomiUserCardReceived(const EventArgs& args)
{
    UpdateAccountAlias();
    CDisplay* display = CDisplay::GetDisplay();
    if(display)
    {
        DK_IMAGE pGraphics = display->GetMemDC();
        Draw(pGraphics);
        display->StopFullRepaintOnce();
        Repaint();
    }
    return true;
}

void UITitleBar::OnWifiMessage(SNativeMessage _clsMsg)
{
    DebugPrintf(DLC_UITITLEBAR, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if(!m_bIsVisible)
    {
        DebugPrintf(DLC_UITITLEBAR, "%s, %d, %s, %s is not visible", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return;
    }

    if(ALLRECEIVER != _clsMsg.iParam1)
    {
        LPSTR pReceiver = (LPSTR)_clsMsg.iParam1;

        if(!pReceiver || strcmp(this->GetClassName(),pReceiver) != 0)
        {
            return;
        }
    }

    switch(_clsMsg.iParam2)
    {
    case UPDATE_WIFI_STATUS:
        {
            UpdateSignal();
        }
        break;
    case POWERONOFF_WIFI:
        {
            UpdateSignal();
            WifiManager* wifiManager = WifiManager::GetInstance();
            if(wifiManager->IsPowerOn())
            {
                if(!wifiManager->IsConnected())
                {
                    DebugPrintf(DLC_UITITLEBAR, "UIHomePage::OnWifiMessage  No wifi connected, begin to auto join!");
                    wifiManager->StartAutoJoin(WifiStatusCallBack, ConnectWifiCallBack);
                }
                else
                {
                    DebugPrintf(DLC_UITITLEBAR, "UIHomePage::OnWifiMessage  Has one wifi connected!!!");
                }
            }
            else
            {
                DebugPrintf(DLC_UITITLEBAR, "UIHomePage::OnWifiMessage  Has no power now!!!");
            }
        }
        break;
    case CONNECT_WIFI:
        {
            DebugPrintf(DLC_UITITLEBAR, "UIHomePage::OnWifiMessage  CONNECT_WIFI");
            UpdateSignal();
            ConnectionMessage* cm = (ConnectionMessage*)_clsMsg.iParam3;
            if(cm && !cm->GetWifiConnected())
            {
                delete cm;
                cm = NULL;
                UIContainer* pParent = GetParent();
                if (pParent)
                {
                    UIWifiDialog m_WifiDialog(pParent);
                    CDisplay* pDisplay = CDisplay::GetDisplay();
                    if(pDisplay)
                    {
                        m_WifiDialog.MoveWindow(0,0,pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
                        m_WifiDialog.DoModal();
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}

void UITitleBar::ConnectWifiCallBack(BOOL _bConnected, std::string _strSSID, std::string _strPWD, string strIdentity)
{
    ConnectionMessage* pCM = new ConnectionMessage(_bConnected, _strSSID, _strPWD, strIdentity);
    if(!pCM)
    {
        DebugPrintf(DLC_UITITLEBAR, "Failed to create ConnectionMessage for (connected=%d, ssid=%s, password=%s)", _bConnected, _strSSID.c_str(), _strPWD.c_str());
        return;
    }
    DebugPrintf(DLC_ZHY, "enter UITitleBar::ConnectWifiCallBack %d, connected: %d, essid: %s, pwd: %s", pCM, _bConnected, _strSSID.c_str(), _strPWD.c_str());
    SNativeMessage msg;
    msg.iType = KMessageWifiChange;
    msg.iParam1 = 0;
    msg.iParam2 = CONNECT_WIFI;
    msg.iParam3 = (UINT32)pCM;
    INativeMessageQueue::GetMessageQueue()->Send(msg);
}

void UITitleBar::WifiStatusCallBack()
{
    SNativeMessage msg;
    msg.iType = KMessageWifiChange;
    msg.iParam1 = 0;
    msg.iParam2 = (UINT32)UPDATE_WIFI_STATUS;
    msg.iParam3 = (UINT32)DEFAULT;
    INativeMessageQueue::GetMessageQueue()->Send(msg);
}

void UITitleBar::WifiPowerOnCallBack()
{
    SNativeMessage msg;
    msg.iType = KMessageWifiChange;
    msg.iParam1 = 0;
    msg.iParam2 = POWERONOFF_WIFI;
    msg.iParam3 = 0;
    INativeMessageQueue::GetMessageQueue()->Send(msg);
}

//TODO:修改
BOOL ReadCmd(LPCSTR Cmd,LPSTR lpResult,int iSize)
{

    BOOL bRet = false;
    if(Cmd == NULL || lpResult == NULL || iSize <=0 )
    {
        return false;
    }
    FILE *fp = NULL;
    if((fp = popen(Cmd, "r")) == NULL)
    {
        return false;
    }
    if(fread(lpResult,1,iSize - 1,fp) > 0)
    {
        bRet = true;
    }
    pclose(fp);
    return bRet;

}

static TitleBarSignalManager  m_gTitleBarSignalManger;

TitleBarSignalManager::TitleBarSignalManager()
                    :m_PthreadId(0)
{
}

TitleBarSignalManager::~TitleBarSignalManager()
{
}

void TitleBarSignalManager::CreateTitleBarSignalManager()
{
    return;
}

TitleBarSignalManager* TitleBarSignalManager::GetTitleBarSignalManager()
{
    return &m_gTitleBarSignalManger;
}

void TitleBarSignalManager::Init()
{
    Start();
}

static bool s_cacelUpdateThread = 0;

void *TitleBarSignalManager::UpdateTitleBarSingalProc(void *lparam)
{
    TitleBarSignalManager* pUpdate =(TitleBarSignalManager*)lparam;     
    while(true)
    {
        if(!pUpdate)
        {
            //DebugPrintf(DLC_DIAGNOSTIC, "TitleBarSignalManager::UpdateTitleBarSingalProc  break out");
            break;
        }
        SNativeMessage TitleBarMsg;
        TitleBarMsg.iType = KMessageUpdateTitleBar;
        INativeMessageQueue::GetMessageQueue()->Send(TitleBarMsg); 
        //DebugPrintf(DLC_DIAGNOSTIC, "TitleBarSignalManager::UpdateTitleBarSingalProc ing...");
        int i = 0;
        while (i < 300)
        {
            usleep(200000);
            if (s_cacelUpdateThread)
            {
                s_cacelUpdateThread = false;
                return NULL;
            }
            ++i;
        }
    }

    return NULL;
}

void TitleBarSignalManager::Start()
{
    //DebugPrintf(DLC_DIAGNOSTIC, "TitleBarSignalManager::Start Entering to start titlebar refresh");    
    if(!m_PthreadId)
    {
        ThreadHelper::CreateThread(&m_PthreadId, TitleBarSignalManager::UpdateTitleBarSingalProc, (void*)this, "UITitleBarSignalManager::Update");
    }
}

void TitleBarSignalManager::Stop()
{
    //DebugPrintf(DLC_DIAGNOSTIC, "TitleBarSignalManager::Stop Entering to stop titlebar refresh");
    if(m_PthreadId){
        s_cacelUpdateThread = true;
        ThreadHelper::JoinThread(m_PthreadId, NULL);
        m_PthreadId = 0;
    }
}

void TitleBarSignalManager::Destory()
{
    Stop();
    return ;
}

bool UITitleBar::OnTouchEvent(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_GESTURE, "UITitleBar::OnTouchEvent()");
    m_gestureDetector.OnTouchEvent(moveEvent);
    return true;
}


bool UITitleBar::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{
    if (direction == FD_DOWN)
    {
        UIShortCutPage* pPage = new UIShortCutPage();
        if (pPage)
        {
            CDisplay* pDisplay = CDisplay::GetDisplay();
            if (pDisplay)
            {
                pPage->MoveWindow(0, 0, pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
                CPageNavigator::Goto(pPage);
                return true;
            }
        }
    }
    return true;
}
