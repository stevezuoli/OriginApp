#include "Common/SystemOperation.h"
#include "../DK91Search/Curl_Post.h"
#include "Wifi/WifiManager.h"
#include "drivers/Usb.h"
#include "Framework/INativeMessageQueue.h"
#include "DownloadManager/IDownloader.h"
#include "Framework/CHourglass.h"
#include "Framework/CDisplay.h" 
#ifndef KINDLE_FOR_TOUCH
#include "CommonUI/UITitleBar.h"
#endif
#include "interface.h"
#include "AudioPlayer/AudioPlayer.h"

#define     REBOOT              6
#define     SWITCHTOKINDLE      4
#define     SWITCHTODUOKAN      5
#define     UNINSTALL           7
#define     UPGRADE             10
#define     EBOOKPATH           "/mnt/us/DK_System/bin/ebook"

using namespace DK91SearchAPI;

void  SystemOperation::CleanupEnv()
{
    DebugPrintf(DLC_LIUJT, "SystemOperation::CleanupEnv begin");
    
    AudioPlayer* pAudioPlayer = AudioPlayer::GetInstance();
    if (NULL != pAudioPlayer)
        pAudioPlayer->Stop();
    
    if( WifiManager::GetInstance()->IsPowerOn())
    {
        WifiManager::GetInstance()->PowerOff();
    }

    IDownloader *pDownloadManager = IDownloader::GetInstance();
    if(pDownloadManager)
    {
        pDownloadManager->StopDownload();
        pDownloadManager->SaveHistroy();
    }

    //停止刷新时间、电池电量、wifi信号
    TitleBarSignalManager::GetTitleBarSignalManager()->Stop();

    UITitleBar *pTitleBar = (CDisplay::GetDisplay()->GetCurrentPage())->GetTitleBar();
    if(pTitleBar && pTitleBar->IsVisible())
    {
        pTitleBar->ClearTitleBar();
    }

    //停止沙漏，有可能
    CHourglass* pIns = CHourglass::GetInstance();
    if(pIns)
    {
        pIns->Stop();
    }
    DebugPrintf(DLC_LIUJT, "SystemOperation::CleanupEnv end!");
}

void  SystemOperation::ExitApp(int nExitParam)
{
    DebugPrintf(DLC_LIUJT, "SystemOperation::ExitApp Entering with %d", nExitParam);    

    CUsb *lpUsb = CUsb::GetInstance();
    lpUsb->Release();

    CleanupEnv();
    DebugPrintf(DLC_LIUJT, "SystemOperation::ExitApp delete singleton done!");
    SNativeMessage exitMsg;
    exitMsg.iType = KMessageExit;
    INativeMessageQueue::GetMessageQueue()->Send(exitMsg);
    DebugPrintf(DLC_LIUJT, "SystemOperation::ExitApp send KMessageExit done!");
    chdir("/");
    sync();
    DebugPrintf(DLC_DIAGNOSTIC, "Exit %d", nExitParam); 
    exit(nExitParam);
}

void SystemOperation::Reboot()
{
    DebugPrintf(DLC_DIAGNOSTIC, "ExitApp: Reboot"); 
    ExitApp(REBOOT);
}

void SystemOperation::Upgrade()
{
	DebugPrintf(DLC_DIAGNOSTIC, "ExitApp: Upgrade"); 
    ExitApp(UPGRADE);
}

void SystemOperation::SwitchToKindle()
{
    DebugPrintf(DLC_DIAGNOSTIC, "ExitApp: Switch to kindle"); 
    ExitApp(SWITCHTOKINDLE);
}

void SystemOperation::SwitchToDuoKan()
{
    ExitApp(SWITCHTODUOKAN);
}

int SystemOperation::CheckUpdate()
{
    return 1;
}

void SystemOperation::Uninstall()
{
    ExitApp(UNINSTALL);
}

bool SystemOperation::ViewRightInfo()
{
    return FALSE;
}

bool SystemOperation::CheckEbook()
{
    if(access (EBOOKPATH, F_OK))
    {
        return FALSE;
    }
    return TRUE;
}

BOOL SystemOperation::SniffNetwork(string url, UINT32 uTimeout)
{
    DebugPrintf(DLC_LIUJT, "SystemOperation::SniffNetwork Entering");
    if(url.empty())
    {
        DebugPrintf(DLC_LIUJT, "SystemOperation::SniffNetwork End with bad url!");
        return FALSE;
    }
    if(!uTimeout)
    {
        uTimeout = 3;
    }
    int errorCode = 0;
    string sResult = Get(url,"",&errorCode, "", (long)uTimeout);
    if(!errorCode && !sResult.empty())
    {
        DebugPrintf(DLC_LIUJT, "SystemOperation::SniffNetwork End with TRUE");
        return TRUE;
    }
    return FALSE;
}

