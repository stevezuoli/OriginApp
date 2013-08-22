#include "AudioPlayer/AudioPlayer.h"
//#include "Framework/CNativeThread.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include "FtpService/FtpService.h"
#include "PowerManager/PowerManager.h"
#include "dkWinError.h"
#include "CommonUI/UIUtility.h"
#include "GUI/UIMessageBox.h"
#include "Wifi/WifiManager.h"
#include "../Common/FileManager_DK.h"
#include "I18n/StringManager.h"
#include "Utility/SystemUtil.h"

using dk::utility::SystemUtil;

using std::string;

//TODO, maybe integrated with more detail
#define START_FTP_CMD  "cd /DuoKan && ./miniftpd"

void FtpService::Start()
{
    DebugPrintf(DLC_DIAGNOSTIC, "FTPService::Start");
    string cmd = START_FTP_CMD;
    SystemUtil::ExecuteCommand(cmd.c_str());
    PowerManagement::DKPowerManagerImpl::GetInstance()->AccquireLock(PM_LOCK_SCREEN, MODULE_FTPSERVICE);
}


void FtpService::Stop()
{
    DebugPrintf(DLC_DIAGNOSTIC, "FTPService::Stop");
    string cmd = "killall -9 miniftpd";
    SystemUtil::ExecuteCommand(cmd.c_str());
    PowerManagement::DKPowerManagerImpl::GetInstance()->ReleaseLock(PM_LOCK_SCREEN, MODULE_FTPSERVICE);
}

void FtpService::EnableWifiTransfer()
{
    bool bWifiConnected = UIUtility::CheckAndReConnectWifi();
            
    if(bWifiConnected)
    {
        string strIp = CWifiBaseImpl::GetInstance()->FindIP();

		FtpService::Start();
		UIMessageBox* pBox = UIUtility::CreateMessageBox(MB_CANCEL);
        if(pBox)
        {
            pBox->SetText(StringManager::GetStringById(DATA_WIFI_TRANSFER_HLEP) + CString(strIp.c_str()));
            pBox->SetButtonText(MB_CANCEL, StringManager::GetPCSTRById(WIFITRANSFER_CLOSE));
            pBox->DoModal();
            delete pBox;
        }

        FtpService::Stop();
        CDKFileManager* FileManger = CDKFileManager::GetFileManager();
        FileManger->ReLoadFileToFileManger(SystemSettingInfo::GetInstance()->GetMTDPathLP());
        SystemSettingInfo::GetInstance()->ResetFontSize();
    }

}

/* End of implementation class template. */


