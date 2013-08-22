#include "TouchAppUI/UISystemSettingPanel1.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "GUI/GUISystem.h"
#include "Common/SystemSetting_DK.h"
#include "GUI/UIMessageBox.h"
#include "Common/SystemOperation.h"
#include "Framework/CDisplay.h"
#include "Model/UpgradeModelImpl.h"
#include "CommonUI/UIUtility.h"
#include "DownloadManager/IDownloader.h"
#include "Framework/CNativeThread.h"
#include "TouchAppUI/UIWifiDialog.h"
#include "SQM.h"
#include "FtpService/FtpService.h"

using namespace WindowsMetrics;
using namespace DKAutoUpgrade;

UISystemSettingPanel1::UISystemSettingPanel1()
{
    Init();
}

UISystemSettingPanel1::~UISystemSettingPanel1()
{
}

void UISystemSettingPanel1::Init()
{
    //SystemSettingInfo* systemSettingInfo = SystemSettingInfo::GetInstance();
    AddSectionTitle(StringManager::GetPCSTRById(TOUCH_RESTART_SWITCH_SYS));
    m_titleUpSpacing -= m_itemVertSpacing;
    
    AddPushButton(
            StringManager::GetPCSTRById(SYS_RESTART),
            SBI_Restart,
            StringManager::GetPCSTRById(RESTART));
    AddPushButton(
            StringManager::GetPCSTRById(TOUCH_SWI_KINDLE),
            SBI_SwitchToKindle,
            StringManager::GetPCSTRById(SWITCH_SWITCH));
    AddPushButton(
            StringManager::GetPCSTRById(TOUCH_SYSTEM_UPDATE),
            SBI_CheckUpdate,
            StringManager::GetPCSTRById(CHECK_UPDATE));

    AddSectionTitle(StringManager::GetPCSTRById(WIFI_TITLE));
    AddPushButton(GetWifiItemText().c_str(),
        SBI_WifiSetting,
        StringManager::GetPCSTRById(WORD_SET), NULL, &m_wifiStatusLabel);
	m_wifiStatusLabel->SetMinWidth(GetWindowMetrics(UISettingPanelWifiItemWidthIndex));
    AddPushButton(StringManager::GetPCSTRById(DATA_WIFI_TRANSFER),
        SBI_WifiTransfer,
        StringManager::GetPCSTRById(DATA_WIFI_TRANSFER_OPEN));
    AddSectionTitle(StringManager::GetPCSTRById(POWER_MANAGEMENT));
    AddPushButton(StringManager::GetPCSTRById(BATTERYRATIO_MODE),
            SBI_BatteryRatioMode,
            "");

    m_batteryRatioButton = 
        (UITouchComplexButton*)m_dynamicCreatedWindows.back();
    m_batteryRatioButton->ShowBorder(false);
    m_batteryRatioButton->SetLeftMargin(0);
    m_batteryRatioButton->SetTopMargin(0);
    UpdateRatioButtonIcon();
}

void UISystemSettingPanel1::UpdateRatioButtonIcon()
{
    m_batteryRatioButton->SetIcon(
            m_systemSettingInfo->GetBatteryRatioMode()
            ? ImageManager::GetImage(IMAGE_TOUCH_SWITCH_ON)
            : ImageManager::GetImage(IMAGE_TOUCH_SWITCH_OFF));
}

bool UISystemSettingPanel1::OnChildClick(UIWindow* child)
{
    switch (child->GetId())
    {
        case SBI_Restart:
            OnRestartClick();
            break;
        case SBI_SwitchToKindle:
            OnSwitchToKindleClick();
            break;
        case SBI_CheckUpdate:
            OnCheckUpdateClick();
            break;
        case SBI_WifiSetting:
            OnWifiSettingClick();
            break;
        case SBI_WifiTransfer:
            OnWifiTransferClick();
            break;
        case SBI_BatteryRatioMode:
            OnBatteryRatioModeClick();
            break;
        default:
            break;
    }
    return true;
}
void UISystemSettingPanel1::OnBatteryRatioModeClick()
{
    m_systemSettingInfo->SetBatteryRatioMode(
            !m_systemSettingInfo->GetBatteryRatioMode());
    SNativeMessage msg;
    msg.iType = KMessageUpdateTitleBar;
    CNativeThread::Send(msg);
    UpdateRatioButtonIcon();
    UpdateWindow();
}

void UISystemSettingPanel1::OnWifiTransferClick()
{
	FtpService::EnableWifiTransfer();
}

void UISystemSettingPanel1::OnWifiSettingClick()
{
	UIWifiDialog WifiDialog(GUISystem::GetInstance()->GetTopFullScreenContainer());
    WifiDialog.MoveWindow(0, 0, CDisplay::GetDisplay()->GetScreenWidth(), CDisplay::GetDisplay()->GetScreenHeight());
	WifiDialog.DoModal();
}

void UISystemSettingPanel1::OnRestartClick()
{
    UIMessageBox messagebox(this, StringManager::GetStringById(SURE_RESTART), MB_OK | MB_CANCEL);
    
    if(MB_OK == messagebox.DoModal())
    {
        SQM::GetInstance()->IncCounter(SQM_ACTION_SETTINGPAGE_SYSTEM_REBOOT);   
        CDisplay::GetDisplay()->DrawFullScreenPicture("/DuoKan/res/reboot.png");
        SystemOperation::Reboot();
    }
}

void UISystemSettingPanel1::OnSwitchToKindleClick()
{
    UIMessageBox messagebox(this, StringManager::GetStringById(SURE_SWITCH_KINDLE_SYSTEM), MB_OK | MB_CANCEL);
    if(MB_OK == messagebox.DoModal())
    {
        SQM::GetInstance()->IncCounter(SQM_ACTION_SETTINGPAGE_SYSTEM_SWITCH_KINDLE);
        SystemOperation::SwitchToKindle();
    }
}

void UISystemSettingPanel1::OnCheckUpdateClick()
{
    SQM::GetInstance()->IncCounter(SQM_ACTION_SETTINGPAGE_UPDATE);
    UpgradeModelImpl* modelImpl = UpgradeModelImpl::GetInstance();
    modelImpl->SetAutoDetect(FALSE);
    bool bConnected = UIUtility::CheckAndReConnectWifi();
    if(bConnected)
    {
        CDisplay::CacheDisabler cache;
        UITextSingleLine* uiTip = UIUtility::CreateTip(StringManager::GetStringById(UPGRADE_GettingNewRelease),this);

        if(!uiTip)
        {
            modelImpl->SetAutoDetect(TRUE);
            return;
        }
        UIUtility::ShowTip(uiTip, TRUE);
        int errorCode = 0;
        if(modelImpl->HasNewRelease(&errorCode))//Has new release, notify user to upgrade
        {
            UIUtility::ShowTip(uiTip, FALSE);
            
            ReleaseInfo* release = modelImpl->GetUpgradeCandidate();
            IDownloader* downloadManager = IDownloader::GetInstance();
            if(modelImpl->IsReleaseDownloaded(release))
            {
                modelImpl->HandleDownloadedRelease(release, FALSE);
                modelImpl->SetAutoDetect(TRUE);                
            }
            
            if(downloadManager->IsDownloading(release->GetUrl()))
            {
                UIMessageBox messagebox(this, StringManager::GetStringById(UPGRADE_NewVersionDOWNLOADING), MB_TIPMSG);
                messagebox.DoModal();
                modelImpl->SetAutoDetect(TRUE);
            }
            else
            {
                SNativeMessage msg;
                msg.iType = KMessageNewReleaseDownloadNotify;
                msg.iParam1 = (UINT32)(release);
                msg.iParam2 = ManualDetection;
                CNativeThread::Send(msg);
                //modelImpl->SetAutoDetect(TRUE); //This call must be commented, and it must be called on CMessageHander!!!
                //Because it will help us avoid to start thread to check new release after wifi available!!!!
            }
        }
        else//No release, prompt user
        {
            UIUtility::ShowTip(uiTip, FALSE);
            if(0 == errorCode)
            {
                UIMessageBox messagebox(this, StringManager::GetStringById(ALREADY_LATEST_VERSION), MB_OK);
                messagebox.DoModal();
            }
            else
            {
                UIMessageBox messagebox(this, StringManager::GetStringById(FAILEDTOCHECKNEWVERSION), MB_OK);
                messagebox.DoModal();
            }
            modelImpl->SetAutoDetect(TRUE);
        }
        
        delete uiTip;
        uiTip = NULL;
    }
}


std::string UISystemSettingPanel1::GetWifiItemText() const
{
    if (!IsWifiOn())
    {
        return StringManager::GetPCSTRById(CONNECT_POWEROFF);
    }
    else if(IsWifiConnected())
    {
        std::string apName = GetApName();
        if (!apName.empty())
        {
            char buf[100];
            snprintf(buf, DK_DIM(buf), "%s%s",
                    StringManager::GetPCSTRById(CONNECT_ALREADY), apName.c_str());
            return buf;
        }
    }

    return StringManager::GetPCSTRById(CONNECT_NULL);
}

void UISystemSettingPanel1::InitStatus()
{
    if (m_wifiStatusLabel)
    {
        m_wifiStatusLabel->SetText(GetWifiItemText());
    }
}