////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/core/src/Framework/CMessageHandler.cpp $
// $Revision: 21 $
// Contact: yuzhan
// Latest submission: $Date: 2008/08/01 13:57:50 $ by $Author: fareast\\xzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////
#include "PowerManager/PowerManager.h"
#include "AudioPlayer/AudioPlayer.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#ifdef KINDLE_FOR_TOUCH
#include "TouchAppUI/UIUnlockDlg.h"
#include "TouchAppUI/BookOpenManager.h"
#include "TouchAppUI/UIDKLowPowerAlarmPage.h"
#else
#include "TouchAppUI/UIUnlockDlg.h"
#endif
#include "Framework/INativeDisplay.h"
#include "GUI/ITpGraphics.h"
#include "DkSPtr.h"
#include "FontManager/DKFont.h"
#include "Framework/INativeMessageQueue.h"
#include "Framework/CMessageHandler.h"
#include "Framework/CNativeThread.h"
#include "Framework/CDisplay.h"
#include "Framework/CRunnableDepot.h"
#include "Framework/BackgroundLoader.h"
#include "Framework/KernelGear.h"
#include "SQM.h"
#include "interface.h"
#include "drivers/Usb.h"
#include "GUI/FCFontManager.h"
#include "GUI/FontManager.h"
#include "Common/ConfigInfo.h"
#include "../Common/FileManager_DK.h"
#include "DownloadManager/IDownloader.h"
#include "Model/UpgradeModelImpl.h"
#include "Utility/FileSystem.h"
#include "I18n/StringManager.h"
#include "Utility/Misc.h"
#include <string>
#include "../SQMUploader/SQMUploader.h"
#include "StopWatch.h"
#include "ScreenSaverUpdate/ScreenSaverUpdateInfo.h"
#include "GUI/GUIHelper.h"
#include "Utility/Dictionary.h"
#include "Wifi/WifiManager.h"
#include "Wifi/WiFiQueue.h"
#include "../BookStore/QiushibaikeImpl.h"
#include "../Net/MailService.h"
#include "Common/CAccountManager.h"
#include "TTS/TTS.h"
#include "drivers/MoveEvent.h"

#include "GUI/UIGesture.h"
#include "GUI/GUISystem.h"
#include "GUI/GlobalEventListener.h"
#include "Utility/TimerThread.h"

#include "drivers/DeviceInfo.h"
#include "drivers/FrontLight.h"
#include "drivers/TouchDrivers.h"
#include "Utility/SystemUtil.h"
#include "Utility/WebBrowserUtil.h"
#include "Thirdparties/EvernoteManager.h"
#include "Thirdparties/MiMigrationManager.h"

using namespace std;
using namespace DkFormat;
using namespace dk::account;
using namespace dk::utility;
using namespace dk::thirdparties;
using namespace DKAutoUpgrade;

#define DK_SCREENSAVER_DIR "/mnt/us/DK_System/xKindle/res/ScreenSaver/"
#define USER_SCREENSAVER_DIR "/mnt/us/DK_System/xKindle/res/UserScreenSaver/"
#define NO_SCREENSAVER_IMAGE "/DuoKan/res/noscreensaver.png"
#define SCREEN_BLACK_IMAGE "/DuoKan/res/screen_black.png"
#define SCREEN_WHITE_IMAGE "/DuoKan/res/screen_white.png"
#define SYNC_PATH "/mnt/us/DK_Sync/"
#define DK_NEWS_PATH "/mnt/us/DK_News/"
#define POWERLIMIT_MPLAYER 20
#define POWERLIMIT_TTS     20
extern char g_szAppPath[PATH_MAX];

time_t CMessageHandler::m_lastMsgTime;
SNativeMessage CMessageHandler::m_lastMsg;

static void WiFiPowerOffThread(void *para)
{
	// 当前不在屏保状态， 无需操作，直接返回
	if(GUIHelper::GetScreenSaverStatus() == 0)
	{
		return;
	}

	//DebugPrintf(DLC_DIAGNOSTIC, "WiFiPowerOffThread() Begin");
	WifiManager::GetInstance()->PowerOff();
	//DebugPrintf(DLC_DIAGNOSTIC, "WiFiPowerOffThread() End");
}

// 推迟关闭WiFi， 以等待上传阅读数据
static void WiFiPowerOffGracefully(long delayms)
{
	void *data = NULL;
	int64_t ms = delayms +  SystemUtil::GetUpdateTimeInMs();
	bool repeat = false;
	long intervalInMs = 0;
	bool handleInMsgThread = false;
	
	TimerThread::GetInstance()->AddTimer(WiFiPowerOffThread, data, ms, repeat, handleInMsgThread, intervalInMs);
}

static void WiFiPowerOnAndJoin(void *para)
{
	if (!WifiManager::GetInstance()->IsPowerOn())
	{
		WifiManager::GetInstance()->PowerOn();
	}
	
	DebugPrintf(DLC_DIAGNOSTIC, "wifi power status: %d", WifiManager::GetInstance()->IsPowerOn());
	
	if (!WifiManager::GetInstance()->IsConnected())
	{
		//DebugPrintf(DLC_DIAGNOSTIC, "start auto join");
		WifiManager::GetInstance()->StartAutoJoin(UITitleBar::WifiStatusCallBack, UITitleBar::ConnectWifiCallBack);
	}
	DebugPrintf(DLC_DIAGNOSTIC, "wifi connect status: %d", WifiManager::GetInstance()->IsConnected());
}

static void WiFiPowerOnAndJoinGracefully(long delayms)
{
    void *data = NULL;
	int64_t ms = delayms +  SystemUtil::GetUpdateTimeInMs();
	bool repeat = false;
	long intervalInMs = 0;
	bool handleInMsgThread = false;
	
	TimerThread::GetInstance()->AddTimer(WiFiPowerOnAndJoin, data, ms, repeat, handleInMsgThread, intervalInMs);
}


static void FrontLightResetThread(void *para)
{
	FrontLight::Reset();
}

static void FrontLightResetGracefully(long delayms)
{
	void *data = NULL;
	int64_t ms = delayms +  SystemUtil::GetUpdateTimeInMs();
	bool repeat = false;
	long intervalInMs = 0;
	bool handleInMsgThread = false;
	int icount=3;

	// 为避免被powerd影响亮度，尝试设置3次
	while(icount--)
	{
		TimerThread::GetInstance()->AddTimer(FrontLightResetThread, data, ms, repeat, handleInMsgThread, intervalInMs);
		ms += 1000;
	}
}

const char* GetNativeMessageName(ENativeMessageType message)
{
    switch(message)
    {
        case KMessageNothing:
            return "KMessageNothing";
            break;
        case KMessageStartApp:
            return "KMessageStartApp";
            break;
        case KMessagePauseApp:
            return "KMessagePauseApp";
            break;
        case KMessageDestroyApp:
            return "KMessageDestroyApp";
            break;
        case KMessageKeyboard:
            return "KMessageKeyboard";
            break;
        case KMessagePaint:
            return "KMessagePaint";
            break;
        case KMessagePointer:
            return "KMessagePointer";
            break;
        case KMessageSynchronousCall:
            return "KMessageSynchronousCall";
            break;
        case KMessageSynchronizedCall:
            return "KMessageSynchronizedCall";
            break;
        case KMessageUsbSingle:
            return "KMessageUsbSingle";
            break;
        case KMessageUpdateUI:
            return "KMessageUpdateUI";
            break;
        case KMessageUpdateTitleBar:
            return "KMessageUpdateTitleBar";
            break;
        case KMessageExit:
            return "KMessageExit";
            break;
        case KMessageScreenSaverIn:
            return "KMessageScreenSaverIn";
            break;
        case KMessageScreenSaverOut:
            return "KMessageScreenSaverOut";
            break;
        case KMessageDownloadStatusUpdate:
            return "KMessageDownloadStatusUpdate";
            break;
        case KMessageNewReleaseDownloadNotify:
            return "KMessageNewReleaseDownloadNotify";
            break;
        case KMessageNewReleaseUpgradeNotify:
            return "KMessageNewReleaseUpgradeNotify";
            break;
        case KMessageGoHome:
            return "KMessageGoHome";
            break;
        case KMessageGoPageStackEntry:
            return "KMessageGoPageStackEntry";
            break;
        case KMessageWifiChange:
            return "KMessageWifiChange";
            break;
        case KMessageEndCachePaint:
            return "KMessageEndCachePaint";
            break;
        case KMessageQiuShiBaiKe:
            return "KMessageQiuShiBaiKe";
            break;
        case KMessageBatteryUpdate:
            return "KMessageBatteryUpdate";
            break;
        case KMessageTTSChange:
            return "KMessageTTSChange";
            break;
        case KMessageMailSyncUp:
            return "KMessageMailSyncUp";
            break;
        case KMessageDuoKanIDRegister:
            return "KMessageDuoKanIDRegister";
            break;
        case KMessageDuoKanIDLogin:
            return "KMessageDuoKanIDLogin";
            break;
        case KMessageDuoKanIDLogout:
            return "KMessageDuoKanIDLogout";
            break;
        case KMessageTouchEvent:
            return "KMessageTouchEvent";
            break;
     	case KMessageScreenSaverDownloaded:
			return "KMessageScreenSaverDownloaded";
     	case KMessageEvent:
			return "KMessageEvent";
     	case KMessageTimerEvent:
			return "KMessageTimerEvent";
        case KMessageMoveEvent:
            return "KMessageMoveEvent";
        case KMessagePrintScreen:
            return "KMessagePrintScreen";
        case KMessageWebBrowserStart:
            return "KMessageWebBrowserStart";
        case KMessageWebBrowserStop:
            return "KMessageWebBrowserStop";
        case KMessageThirdPartyAppFinished:
            return "KMessageThirdPartyAppFinished";
        case KMessageDuokanLoginFailed:
            return "KMessageDuokanLoginFailed";
        default:
            return "Unknown";
            break;
    }
}

bool CMessageHandler::CanResponseCommonMessage()
{
    CUsb *lpUsb = CUsb::GetInstance();
    return lpUsb && lpUsb->IsUsbConnected() != 1 && GUIHelper::GetScreenSaverStatus()==0;
}

CMessageHandler::EHandlerReturnType CMessageHandler::HandleMessage()
{
    SNativeMessage msg;
    GetMessage(msg);
    return DispatchMessage(msg);
}


void CMessageHandler::GetMessage(SNativeMessage &msg)
{
    INativeMessageQueue* iMessageQueue = INativeMessageQueue::GetMessageQueue();
    iMessageQueue->Receive(msg);
}

CMessageHandler::EHandlerReturnType CMessageHandler::DispatchMessage(SNativeMessage &msg)
{
    DebugPrintf(DLC_MESSAGE, "Begin process msg: %d: %s begin", msg.iType, GetNativeMessageName(msg.iType));
    static BOOL fDisablePaint = FALSE;
	static BOOL fWifiPowerOn  = FALSE;
    static bool fWebBrowserExec = false;
	UIPage *curPage;
	
	time_t curTime = time(NULL);
	SetLastDispatchMessage(msg, curTime);


    // 目前暂时的调试， WebBrowser运行后，只响应WebBrowserStop消息。其他消息丢弃掉。
    // 需要改进

#if 1
    if(fWebBrowserExec) 
    {
        if(msg.iType != KMessageWebBrowserStop)
            return CONTINUE_EXECUTION;
    }
#endif

    switch (msg.iType)
    {
    case KMessageStartApp:
        {
        }
        break;
    case KMessagePauseApp:
        {
        }
        break;
    case KMessageDestroyApp:
        {
        }
        break;
    case KMessageGoHome:
        {
            CPageNavigator::BackToEntryPage(msg.iParam1,msg.iParam2,msg.iParam3);
        }
        break;
    case KMessageGoPageStackEntry:
        {
            CPageNavigator::BackToEntryPageOnType(msg.iParam1,msg.iParam2,msg.iParam3);
        }
    case  KMessageGoPrePage:
        {
             CPageNavigator::BackDangerous();
        }
        break;
    case KMessageKeyboard:
        {
            CUsb *lpUsb = CUsb::GetInstance();
            if( lpUsb->IsUsbConnected() != 1)
            {
                INT32 keycode = (INT32)msg.iParam1;
                EKeyboardEventType type = (EKeyboardEventType)msg.iParam2;
                if(type == DK_KEYTYPE_DOWN)
                {
                    switch(keycode)
                    {
                        case KEY_LPAGEUP:
                            SQM::GetInstance()->IncCounter(SQM_ACTION_KEYBOARD_USAGE_LEFTPAGEUP);
                            break;
                        case KEY_LPAGEDOWN:
                            SQM::GetInstance()->IncCounter(SQM_ACTION_KEYBOARD_USAGE_LEFTPAGEDOWN);
                            break;
                        case KEY_RPAGEUP:
                            SQM::GetInstance()->IncCounter(SQM_ACTION_KEYBOARD_USAGE_RIGHTPAGEUP);
                            break;
                        case KEY_RPAGEDOWN:
                            SQM::GetInstance()->IncCounter(SQM_ACTION_KEYBOARD_USAGE_RIGHTPAGEDOWN);
                            break;
                        case KEY_VOLUMEUP:
                        case KEY_VOLUMEDOWN:
                        {
                            AudioPlayer *pAudioPlayer = AudioPlayer::GetInstance();
                            if(pAudioPlayer != NULL && pAudioPlayer->GetPlayStatus() == PLAYER_STATUS_PLAY)
                            {
                                int iVolume = pAudioPlayer->GetVolumeValue();
                                iVolume += (KEY_VOLUMEUP == keycode) ? 1 : -1;
                                pAudioPlayer->SetVolume(iVolume);
                            }
                            break;
                        }
                        case KEY_HOME:
                            {
                                if (CNativeThread::GetKeyHomeDealedAlready())
                                {
                                    if (CNativeThread::GetScreenShotted())
                                    {
                                        UIContainer* pCur = GUISystem::GetInstance()->GetTopFullScreenContainer();
                                        if (pCur)
                                        {
                                            UIMessageBox messagebox(pCur,StringManager::GetPCSTRById(TOUCH_SCREENSHOTTIPS), MB_OK);
                                            messagebox.DoModal();
                                        }
                                    }
                                    CNativeThread::SetKeyHomeDealedAlready(false);
                                    CNativeThread::SetScreenShotted(false);
                                    return CONTINUE_EXECUTION;
                                }
                            }
                        default:
                            break;
                    }
                }
                CDisplay::GetDisplay()->GetMessageDispatcher().OnKeyEvent(keycode, type);
            }
        }
        break;
    case KMessageTTSChange:
        {
            UIContainer* pCur = UIUtility::GetCurDisplay();
            CTextToSpeech* pTTS = CTextToSpeech::GetInstance();
            if(NULL != pCur && NULL != pTTS)
            {
                if (TTS_MESSAGE_STOP == msg.iParam2)
                {
                    pTTS->TTS_Stop();
                    pTTS->TTS_UnInit();
                }
                else
                {
                    pCur->OnTTSMessage(&msg);
                }
            }
        }
        break;
    case KMessageWifiChange:
        {
            CUsb *lpUsb = CUsb::GetInstance();
            if( lpUsb->IsUsbConnected() != 1)
            {

                UINT32 uSubType = msg.iParam2;
                DebugPrintf(DLC_LIUJT, "DispatchMessage handle KMessageWifiChange after OnWifiMessage(), param1=%d, param2=%d, param3=%d, addr=%x", msg.iParam1, msg.iParam2, msg.iParam3, &msg);
                if(uSubType == UPDATE_WIFI_STATUS)
                {
                    UINT32 uWifiStatus = msg.iParam3;
                    if(uWifiStatus == WIFIAVAILABLE)
                    {
                        DebugPrintf(DLC_LIUJT, "DispatchMessage receive Wifi Available message!");
                        WiFiQueue::GetInstance()->StartHandleQueueTask();
                    }
                    else if(uWifiStatus == WIFIUNAVAILABLE)
                    {
						IDownloader* downloadManager = IDownloader::GetInstance();
                        DebugPrintf(DLC_LIUJT, "DispatchMessage receive Wifi UnAvailable message!");
                        if(downloadManager)
                        {
                            downloadManager->PauseAllTasks();
                        }

                        MailService::GetInstance()->StopCheckThread();

                        CAccountManager* pManager = CAccountManager::GetInstance();
                         if(pManager)
                         {
                            pManager->SetNotLoggedIn();
                        }
                    }
                }
//#ifdef KINDLE_FOR_TOUCH
//                UITitleBar* pTitleBar = UITitleBar::GetInstance();
//                if (pTitleBar)
//                {
//                    DebugPrintf(DLC_LIUJT, "DispatchMessage will call %s 's OnWifiMessage()", pTitleBar->GetClassName());
//                    pTitleBar->OnWifiMessage(msg);
//                }
//#else
                UIPage* pCur = CDisplay::GetDisplay()->GetCurrentPage();
                if(pCur)
                {
                    DebugPrintf(DLC_LIUJT, "DispatchMessage will call %s 's OnWifiMessage()", pCur->GetClassName());
                    pCur->OnWifiMessage(msg);
                }
//#endif
            }
        }
        break;
    case KMessageNewSerializedUpdateCompleted:
        {
            CUsb *lpUsb = CUsb::GetInstance();
            if( lpUsb->IsUsbConnected() != 1)
            {
                CDisplay::GetDisplay()->GetCurrentPage()->OnSerializedBookUpdate(msg);
            }
        }
        break;
    case KMessageNewReleaseDownloadNotify:
        {
         CUsb *lpUsb = CUsb::GetInstance();
            if( lpUsb->IsUsbConnected() != 1)
            {
                DebugPrintf(DLC_LIUJT, "CMessageHandler::DispatchMessage receive release Available message! param1=%x, param2=%d", msg.iParam1, msg.iParam2);
                UpgradeModelImpl* upgradeModleImpl = UpgradeModelImpl::GetInstance();
                ReleaseInfo* LatestRelease= (ReleaseInfo*)msg.iParam1;
                UINT32 detection = msg.iParam2;
                if(upgradeModleImpl)
                {
                    if(LatestRelease)
                    {
                        BOOL releaseDownloaded = upgradeModleImpl->IsReleaseDownloaded(LatestRelease);
                        if(releaseDownloaded)//If file has been downloaded before!
                        {
                            upgradeModleImpl->HandleDownloadedRelease(LatestRelease, detection);
                        }
                        else
                        {
                            upgradeModleImpl->HandleUndownloadedRelease(LatestRelease, detection);
                        }
                    }

                    if(detection == ManualDetection)
                    {
                        upgradeModleImpl->SetAutoDetect(TRUE);
                    }
                }
                DebugPrintf(DLC_LIUJT, "CMessageHandler::DispatchMessage receive release Available message end!");
            }
        }
        break;
    case KMessageNewReleaseUpgradeNotify:
        {
         CUsb *lpUsb = CUsb::GetInstance();
            if( lpUsb->IsUsbConnected() != 1)
            {
                    UpgradeModelImpl* upgradeModleImpl = UpgradeModelImpl::GetInstance();
                    BOOL releaseDownloaded = upgradeModleImpl->IsReleaseDownloaded();
                    if(releaseDownloaded)
                    {
                        DebugPrintf(DLC_LIUJT, "CMessageHandler::DispatchMessage receive Download complete message, begin to install!");
                        upgradeModleImpl->InstallRelease();
                    }
                    else
                    {
                        DebugPrintf(DLC_LIUJT, "CMessageHandler::DispatchMessage file is not downloade successfully in fact!!!");
                    }
            }
        }
        break;
    case KMessagePaint:
        {
            if (!fDisablePaint)
            {
                paintflag_t paintFlag = PaintFlagFromPaintMessage(msg);
                DkRect rect = DkRectFromPaintMessage(msg);
                DebugPrintf(DLC_PENGF, "CMessageHandler::DispatchMessage() KMessagePaint, rect: (%d, %d)-(%d,%d)",
                        rect.Left(), rect.Top(), rect.Width(), rect.Height());

                CDisplay::GetDisplay()->ForceDraw(rect, paintFlag);
            }
        }
        break;
	case KMessageUpdateWithoutRepaint:
		{
			CDisplay* pDisplay = CDisplay::GetDisplay();
			pDisplay->StopFullRepaintOnce();
		    if (pDisplay)
		    {
		        pDisplay->Repaint(PAINT_FLAG_REPAINT);
		    }
		}
		break;
    case KMessageUpdateUI:
        {
            if (!fDisablePaint)
            {
                SNativeMessage repaintMsg;
                CDisplay* pDisplay = CDisplay::GetDisplay();
                if (pDisplay)
                {
                    if (MakePaintMessage(0, 0, pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight(), PAINT_FLAG_FULL, &repaintMsg))
                        INativeMessageQueue::GetMessageQueue()->Send(repaintMsg);
                }
            }
        }

        break;
    case KMessagePointer:
        break;

    case KMessageSynchronousCall:
        {
            TRACE(CString(TEXT("Begin Handle message : KMessageSynchronousCall")));
            IRunnable* runnable = (IRunnable*)msg.iParam1;
            if (runnable != NULL)
                runnable->Run();
            TRACE(CString(TEXT("End Handle message : KMessageSynchronousCall")));
        }
        break;

    case KMessageSynchronizedCall:
        {
            TRACE(CString(TEXT("Begin Handle message : KMessageSynchronizedCall")));
            INT32 id = msg.iParam1;
            SPtr<IRunnable> spRunnable = CRunnableDepot::GetRunnableAndDelete(id);
            if (spRunnable)
                spRunnable->Run();
            TRACE(CString(TEXT("End Handle message : KMessageSynchronizedCall")));
        }
        break;
    case KMessageUsbSingle:
        {
            EUsbSingleType eUsbSingle = (EUsbSingleType)msg.iParam1;
			DebugPrintf(DLC_MESSAGE, "KMessageUsbSingle %d", eUsbSingle);
            switch(eUsbSingle)
            {
            case usbUnknow:
                {
                }
                break;
            case usbConfigured:
                {
                    DebugPrintf(DLC_MESSAGE, "KMessageUsbSingle:usbConfigured Begin");
                    CUsb *lpUsb = CUsb::GetInstance();
                    if(lpUsb->IsUsbConnected() == 1)
                    {
                        lpUsb->ShowConfigurePic();
                    }

                    // 各个页面卸载影响挂盘的资源
                    CPageNavigator::OnUsbPlugin();

                    // 卸载字库，否则会影响挂盘
                    FontManager::GetInstance()->UnLoadFont();

                    // 销毁格式解析内核
                    DebugPrintf(DLC_MESSAGE, "KMessageUsbSingle:usbConfigured DestroyKernel");
                    UnRegisterKernelFonts();
                    DestroyKernel();

                    //停止刷新时间、电池电量、wifi信号
                    TitleBarSignalManager::GetTitleBarSignalManager()->Stop();

                    if(lpUsb->IsUsbConnected() == 1)
                    {
                        DebugPrintf(DLC_MESSAGE, "KMessageUsbSingle:usbConfigured SwitchToUsb");
                        lpUsb->SwitchToUsb();
                    }

                    fDisablePaint = TRUE;
                }
                break;
            case usbUnconfigured:
                {
                    fDisablePaint = FALSE;
                    CUsb *lpUsb = CUsb::GetInstance();
                    DebugPrintf(DLC_MESSAGE, "KMessageUsbSingle:usbUnconfigured SwitchToFlash");
                    lpUsb->SwitchToFlash();

                    DebugPrintf(DLC_MESSAGE, "SetScreenSaverStatus(0)");
					// 清除屏保状态
                    GUIHelper::SetScreenSaverStatus(0);

                    DebugPrintf(DLC_MESSAGE, "LoadFont");
                    //重新加载书籍
                    // 加载字库
                    FontManager::GetInstance()->LoadFont();

                    DebugPrintf(DLC_MESSAGE, "InitializeKernel");
                    // 初始化格式解析内核
                    InitializeKernel();

                    DebugPrintf(DLC_MESSAGE, "RefreshAllFontsInfo");
                    // 更新字体管理器.
                    // 如字体文件变更，再次注册 Kernel 字体
                    if (g_pFontManager->IsFontsDirUpdated ())
                    {
                        g_pFontManager->RefreshAllFontsInfo ();
                    }

                    DebugPrintf(DLC_MESSAGE, "RegisterKernelFonts");
					// 内核被重新初始化，需要再次设置内核字体
                    RegisterKernelFonts ();
                    DebugPrintf(DLC_MESSAGE, "SetKernelDefaultFonts");
                    SetKernelDefaultFonts ();

                    DebugPrintf(DLC_MESSAGE, "UpdateTitleBar");

                    //更新时间、电池电量、wifi信号
                    TitleBarSignalManager::GetTitleBarSignalManager()->Start();

                    DebugPrintf(DLC_MESSAGE, "ShowUnfonfigPicture");
                    lpUsb->ShowUnConfigurePic();
                    CDKFileManager* fileManager = CDKFileManager::GetFileManager();
                    fileManager->ReLoadFileToFileManger(SystemSettingInfo::GetInstance()->GetMTDPathLP());
                    // 再次刷新字体，可能开始时字体没有完全载入
                    // 更新字体管理器.
                    // 如字体文件变更，再次注册 Kernel 字体
                    if (g_pFontManager->IsFontsDirUpdated ())
                    {
                        g_pFontManager->RefreshAllFontsInfo ();
                    }
                    SystemSettingInfo::GetInstance()->ResetFontSize();
                    fileManager->FireFileListChangedEvent();
                   // 各个页面加载需要的资源
                //重新加载字典资源
                    DebugPrintf(DLC_MESSAGE, "LoadDict");
                    Dictionary::LoadDict();

                    DebugPrintf(DLC_MESSAGE, "OnUsbPullout");
                    CPageNavigator::OnUsbPullout();
                    DebugPrintf(DLC_MESSAGE, "OnUsbPullout end");

					// KP 在弹出usb时，恢复亮度 （避免原系统干扰）
					if (DeviceInfo::IsKPW())
					{
						int delay_ms = 1000;
						FrontLightResetGracefully(delay_ms);
					}
                }
                break;
            case usbPlugOut:
                {
#ifdef _X86_LINUX
                    CUsb::GetInstance()->SetUsbConnected(false);
#endif
                }
                break;
            case usbPlugIn:
                {
#ifdef _X86_LINUX
                    CUsb::GetInstance()->SetUsbConnected(true);
#endif
                    AudioPlayer* pAudioPlayer = AudioPlayer::GetInstance();
                    if (NULL != pAudioPlayer)
                    {
                        pAudioPlayer->Stop();
                    }
                }
                break;
            default:
                break;

            }
        }
        break;
    case KMessageUpdateTitleBar:
        {
            CDisplay* pDisplay = CDisplay::GetDisplay();
            if (!fDisablePaint && pDisplay)
            {
                UIPage* pUIPage = pDisplay->GetCurrentPage();
                if (pUIPage)
                {
                    UITitleBar *pTitleBar = pUIPage->GetTitleBar();
                    if(pTitleBar && pTitleBar->IsVisible())
                    {
                        pTitleBar->UpdateSignal();
                    }
                }
            }
        }
        break;
    case KMessageExit:
        {
            DebugLog(DLC_MESSAGE, "Handle message : KMessageExit");
        }
        return EXIT_APP;
        break;
	case KMessageScreenSaverDownloaded:
		{
			ScreenSaverUpdateInfo *pUpdateInfo = ScreenSaverUpdateInfo::GetInstance();
			if(pUpdateInfo)
			{
				string strUrl = "";
				string strFileName = "";
				const char *pUrl = (const char*)msg.iParam1;
				if(pUrl)
				{
					strUrl = pUrl;
				}
				const char *pFileName = (const char*)msg.iParam2;
				if(pFileName)
				{
					strFileName = pFileName;
				}
				pUpdateInfo->AddDownloadedToCurrentList(strUrl,strFileName);
			}
		}
		break;

    case KMessageScreenSaverIn:
        {
            DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverIn, %d", GUIHelper::GetScreenSaverStatus());
            if(GUIHelper::GetScreenSaverStatus() == 0)
            {

                DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverIn:DestroyDialog Begin");
				UIPage* pCur = CDisplay::GetDisplay()->GetCurrentPage();
				if(pCur && pCur->GetDialog())
                {
					pCur->DestroyDialog();
                }
                DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverIn:DestroyDialog End");

                GUIHelper::SetScreenSaverStatus(1);

				// 如果当前电量过低， 则保持“电量不足” 的警告画面
				// 如果收到的是强制白屏消息， 则强制白屏
				curPage  = CPageNavigator::GetCurrentPage();
                if (msg.iParam1 == Suspended)
				{
					CDisplay* pDisplay = CDisplay::GetDisplay();
					if (pDisplay)
					{
						pDisplay->DrawFullScreenPicture(SCREEN_WHITE_IMAGE);
					}
				}
				else if ( curPage && curPage->GetClassName() != UIDKLowPowerAlarmPage::UIDKLOWPOWERALARMPAGE)
				{
                      string ScrSavrDir;
					  ScrSavrDir = DK_SCREENSAVER_DIR;

					  vector<string> Scrfiles = vector<string>();
					  int filenum = GetImagesFilesInDir(ScrSavrDir, Scrfiles);
					  DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverIn:Screen file num: %d", filenum);					  
					  bool bPicOK = false;

					  if (filenum >0 )
					  {
						  string fullpath = ScrSavrDir + Scrfiles[Rand(Scrfiles.size())];
						  const char* pImgPath = fullpath.c_str();
						  DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverIn:Screen file path: %s", pImgPath);
						  bPicOK = CDisplay::GetDisplay()->DrawFullScreenPicture(pImgPath);
					  }
					  // 如果绘制屏保图片失败，则绘制屏保提示文件
					  if (!bPicOK)
					  {
						  string fullpath = NO_SCREENSAVER_IMAGE;
						  const char* pImgPath = fullpath.c_str();
						  DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverIn:Screen file path: %s", pImgPath);
						  CDisplay::GetDisplay()->DrawFullScreenPicture(pImgPath);
					  }
				}


                // 推迟50秒关闭WiFi， 以等待上传数据完成
                fWifiPowerOn = WifiManager::GetInstance()->IsPowerOn();
                DebugPrintf(DLC_DIAGNOSTIC, "screen saver In, wifi power status: %d\n", fWifiPowerOn);
                long delayms = 50 * 1000;
                WiFiPowerOffGracefully(delayms);

                // KPW 在进入屏保时，强制灭灯
                if (DeviceInfo::IsKPW())
                {
                    FrontLight::SetLevel(0);
                }

				AudioPlayer* pAudioPlayer = AudioPlayer::GetInstance();
                if (NULL != pAudioPlayer && pAudioPlayer->GetPlayStatus() != PLAYER_STATUS_STOP)
                {
                    DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverIn:StopAudioPlayer Begin");
                    pAudioPlayer->Stop();
                    DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverIn:StopAudioPlayer End");
                }

                DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverIn:OnUsbPlugin");
                // 各个页面卸载影响挂盘的资源
                CPageNavigator::OnUsbPlugin();

                DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverIn:UnLoadFont");
                // 卸载字库，否则会影响挂盘
                FontManager::GetInstance()->UnLoadFont();

                DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverIn:StopTitleBarSignalManager");
                TitleBarSignalManager::GetTitleBarSignalManager()->Stop();
            }

            fDisablePaint = TRUE;
        }
        break;
    case KMessageSuspending:
        {
            DEBUG_POS;
            break;
        }
    case KMessageScreenSaverOut:
        {
			// 如果当前电量过低， 则保持“电量不足” 的警告画面
			curPage = CPageNavigator::GetCurrentPage();
			if( curPage && curPage->GetClassName() == UIDKLowPowerAlarmPage::UIDKLOWPOWERALARMPAGE)
			{
                if(PowerManagement::DKPowerManagerImpl::GetInstance()->GetPowerLevel() > 10) 
                {
                    CPageNavigator::BackToPrePage();
                }
                break;
			}

            DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverOut, %d", GUIHelper::GetScreenSaverStatus());
            fDisablePaint = FALSE;
            if(GUIHelper::GetScreenSaverStatus() == 1)
            {
                // KPW 在退出屏保时，恢复亮度
                if (DeviceInfo::IsKPW())
                {
                    int delay_ms = 1000;
                    FrontLightResetGracefully(delay_ms);
                }
                
                CDisplay::GetDisplay()->SetFullRepaint(true);
                CUsb *lpUsb = CUsb::GetInstance();
                if( lpUsb->IsUsbConnected() == 1)
                {
                    DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverOut SetScreenSaverStatus 0");
                    GUIHelper::SetScreenSaverStatus(0);
                    break;
                }
                                
                //重新加载书籍
                // 加载字库
                DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverOut LoadFont");
                FontManager::GetInstance()->LoadFont();

                // 更新字体管理器.
                // 如字体文件变更，再次注册 Kernel 字体
                if (g_pFontManager->IsFontsDirUpdated ())
                {
                    g_pFontManager->RefreshAllFontsInfo ();
                    RegisterKernelFonts ();
                    SetKernelDefaultFonts ();
                }

                // 各个页面加载需要的资源
                DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverOut OnUsbPullout");
                CPageNavigator::OnUsbPullout();
                GUIHelper::SetScreenSaverStatus(0);

                if(SystemSettingInfo::GetInstance()->IsScreenSaverUnlock())
                {
                    UIUnlockDlg *lpUnlockDlg = UIUnlockDlg::GetInstance();
                    UIContainer *pCurContainer = UIUtility::GetCurDisplay();
                    if(lpUnlockDlg && pCurContainer && pCurContainer != lpUnlockDlg)
                    {
                        UIScreenSaverUnlockPage *pPage = new UIScreenSaverUnlockPage();
                        if(pPage)
                        {
                            pPage->MoveWindow(150,300,300,120);
                            CPageNavigator::Goto(pPage);
                            lpUnlockDlg->SetParent(pPage);
                            lpUnlockDlg->SetUnlockMode();
                            const char* pPasswd = SystemSettingInfo::GetInstance()->GetScreenSaverPassword();
                            if(pPasswd)
                            {
                                lpUnlockDlg->SetRightPassword(pPasswd);
                                lpUnlockDlg->MoveWindow(0,0,300,120);
                                int dlgOK = 0;
                                while(!dlgOK)
                                {
                                    DebugPrintf(DLC_LIUJT, "KMessageScreenSaverOut Message waiting DoModal");
                                    dlgOK = lpUnlockDlg->DoModal();
                                }
                            }
                            CPageNavigator::BackToPrePage();
                        }
                    }
                }

                DebugLog(DLC_DIAGNOSTIC, "Handle message : KMessageScreenSaverOut");
                DebugPrintf(DLC_DIAGNOSTIC, "KMessageScreenSaverOut TitleBarSignalManager Start");
                TitleBarSignalManager::GetTitleBarSignalManager()->Start();
                CDisplay::GetDisplay()->Repaint(PAINT_FLAG_FULL);

                DebugPrintf(DLC_DIAGNOSTIC, "screen saver out, wifi power status: %d\n", fWifiPowerOn);
                if (fWifiPowerOn)
                {
					WiFiPowerOnAndJoinGracefully(10*1000);
                }
            }

        }

         break;
     case KMessageDownloadStatusUpdate:
        {
            CUsb *lpUsb = CUsb::GetInstance();
            if( lpUsb && lpUsb->IsUsbConnected() != 1 && GUIHelper::GetScreenSaverStatus()==0)
            {
                CPageNavigator::GetCurrentPage()->UpdateSectionUI();
            }
        }
        break;
     case KMessageDownloadAddTask:
     case KMessageDownloadDeleteTask:
        {
            CUsb *lpUsb = CUsb::GetInstance();
            if( lpUsb && lpUsb->IsUsbConnected() != 1 && GUIHelper::GetScreenSaverStatus()==0)
            {
                CPageNavigator::GetCurrentPage()->UpdateSectionUI(true);
            }
        }
        break;
     case KMessageFailedToConnectNetwork:
        {
            CUsb *lpUsb = CUsb::GetInstance();
            if( lpUsb && lpUsb->IsUsbConnected() != 1 && GUIHelper::GetScreenSaverStatus()==0)
            {
                CPageNavigator::GetCurrentPage()->OnFailedToConnectServer(msg);
                DebugLog(DLC_MESSAGE, "KMessageFailedToConnectNetwork");
            }
        }
        break;
     case KMessageEndCachePaint:
        {
            CDisplay::GetDisplay()->EndCache();
        }
        break;
     case KMessageBatteryUpdate:
        {
            UIContainer* pCur = UIUtility::GetCurDisplay();
            AudioPlayer* pAudioPlayer = AudioPlayer::GetInstance();
            if (NULL != pAudioPlayer && pAudioPlayer->GetPlayStatus() != PLAYER_STATUS_STOP &&
               PowerManagement::DKPowerManagerImpl::GetInstance()->GetPowerLevel() < POWERLIMIT_MPLAYER)
            {
                DebugPrintf(DLC_LIUJT, "KMessageBatteryUpdate: Has AudoiPlayer");
                if (GUIHelper::GetScreenSaverStatus() == 1 || NULL == pCur)
                {
                    DebugPrintf(DLC_LIUJT, "KMessageBatteryUpdate: Stop AudioPlayer 1");
                    pAudioPlayer->Stop();
                }
                else
                {
                    UIMessageBox messagebox(pCur, StringManager::GetStringById(ACTION_OK), MB_OK | MB_CANCEL);
                    messagebox.SetText(StringManager::GetStringById(LOW_POWER_NO_AUDIOPLAYER));
#ifndef KINDLE_FOR_TOUCH
                    messagebox.SetFocusButton(MB_CANCEL);
#endif
                    int iRes = messagebox.DoModal();
                    if(MB_OK == iRes || MB_YES == iRes)
                    {
                        DebugPrintf(DLC_LIUJT, "KMessageBatteryUpdate: Stop AudioPlayer 2");
                        pAudioPlayer->Stop();
                    }
                }
            }
            else
            {
                DebugPrintf(DLC_LIUJT, "KMessageBatteryUpdate: No AudoiPlayer");
            }

            CTextToSpeech* pTTS = CTextToSpeech::GetInstance();
            if(NULL != pTTS && pTTS->TTS_IsPlay() &&
               PowerManagement::DKPowerManagerImpl::GetInstance()->GetPowerLevel() < POWERLIMIT_TTS)
            {
                DebugPrintf(DLC_LIUJT, "KMessageBatteryUpdate: Has TTS");
                if (NULL == pCur)
                {
                    DebugPrintf(DLC_LIUJT, "KMessageBatteryUpdate: Stop TTS 1");
                    pTTS->TTS_Stop();
                    pTTS->TTS_UnInit();
                }
                else
                {
                    UIMessageBox messagebox(pCur, StringManager::GetStringById(ACTION_OK), MB_OK | MB_CANCEL);
                    messagebox.SetText(StringManager::GetStringById(LOW_POWER_NO_TTS));
#ifndef KINDLE_FOR_TOUCH
                    messagebox.SetFocusButton(MB_CANCEL);
#endif
                    int iRes = messagebox.DoModal();
                    if(MB_OK == iRes || MB_YES == iRes)
                    {
                        DebugPrintf(DLC_LIUJT, "KMessageBatteryUpdate: Stop TTS 2");
                        pTTS->TTS_Stop();
                        pTTS->TTS_UnInit();
                    }
                }

            }
            else
            {
                DebugPrintf(DLC_LIUJT, "KMessageBatteryUpdate: No TTS");
            }
        }
        break;
	 case KMessageBatteryWarning:
		 {
             AudioPlayer* pAudioPlayer = AudioPlayer::GetInstance();
             if (NULL != pAudioPlayer && pAudioPlayer->GetPlayStatus() != PLAYER_STATUS_STOP)
             {
                 pAudioPlayer->Stop();
             }

             CTextToSpeech* pTTS = CTextToSpeech::GetInstance();
             if(NULL != pTTS && pTTS->TTS_IsPlay())
             {
                 pTTS->TTS_Stop();
                 pTTS->TTS_UnInit();
             }

			 UIMessageBox *pMessageBox = UIUtility::CreateMessageBox(MB_TIPMSG);
			 if (pMessageBox)
			 {
				 pMessageBox->SetText(StringManager::GetPCSTRById(LOW_POWER_PLEASE_CHARGE));
				 pMessageBox->DoModal();
				 delete pMessageBox;
				 pMessageBox = NULL;
			 }
		 }
         break;
	 case KMessageBatteryLowAlarm:
		 curPage = CPageNavigator::GetCurrentPage();
         if( curPage && curPage->GetClassName() != UIDKLowPowerAlarmPage::UIDKLOWPOWERALARMPAGE)
         {
             AudioPlayer* pAudioPlayer = AudioPlayer::GetInstance();
             if (NULL != pAudioPlayer && pAudioPlayer->GetPlayStatus() != PLAYER_STATUS_STOP)
             {
                 pAudioPlayer->Stop();
             }

             CTextToSpeech* pTTS = CTextToSpeech::GetInstance();
             if(NULL != pTTS && pTTS->TTS_IsPlay())
             {
                 pTTS->TTS_Stop();
                 pTTS->TTS_UnInit();
             }
             UIDKLowPowerAlarmPage *pPage = new UIDKLowPowerAlarmPage();
             if (pPage)
             {
                 pPage->MoveWindow(0, 0, CDisplay::GetDisplay()->GetScreenWidth() , CDisplay::GetDisplay()->GetScreenHeight());
                 CPageNavigator::Goto(pPage);
             }

			 // 关闭 WiFi
			 WifiManager::GetInstance()->PowerOff();
         }
         break;
	 case KMessageBatteryLowDisalarm:
		 curPage = CPageNavigator::GetCurrentPage();
         if( curPage && curPage->GetClassName() == UIDKLowPowerAlarmPage::UIDKLOWPOWERALARMPAGE)
         {
             CPageNavigator::BackToPrePage();
         }
         break;
     case KMessageMailSyncUp:
        {
            string src(SYNC_PATH);
            string des(DK_NEWS_PATH);
            vector<string> movedFiles;

            if (MoveFilesInDir( src,  des, movedFiles) >0 )
            {
                SQM::GetInstance()->IncCounter(SQM_ACTION_BOOKREADER_EMAILPUSH);
                CDKFileManager* fileManager = CDKFileManager::GetFileManager();
                struct stat buf;
                for (size_t i = 0; i < movedFiles.size(); ++i)
                {
                    if (stat(movedFiles[i].c_str(), &buf) < 0)
                    {
                        continue;
                    }
                    CDKBook* pBookFileInfo = DK_FileFactory::CreateBookFileInfoFromPush(movedFiles[i].c_str(), (int)buf.st_size);
                    fileManager->AddFile(pBookFileInfo);
                }
                fileManager->FireFileListChangedEvent();
                MailService::GetInstance()->SetMailCheckText("RecvMailGotResult\n");
            }
            else
            {
                MailService::GetInstance()->SetMailCheckText("RecvMailNoResult\n");
            }
        }
        break;
     case KMessageOpenBook:
        {
			if(!BookOpenManager::GetInstance()->OpenBook())
			{
				//UIMessageBox messagebox(this, StringManager::GetStringById(CANNOT_OPEN_BOOK), UIMessageBox::ICON_WARNING, MB_TIPMSG);
				//messagebox.DoModal();
			}
        }
        break;
     case KMessageEvent:
		{
			GlobalEventListener* globalEventListener = GlobalEventListener::GetInstance();
			if (globalEventListener->OnMessageEvent(*(EventArgs*)msg.iParam1))
			{
				break;
			}
			if (CanResponseCommonMessage())
			{
				EventListener* eventListener = GUISystem::GetInstance()->GetEventListenerByHandle(msg.hWnd);
				if (NULL != eventListener)
				{
					eventListener->OnMessageEvent(*(EventArgs*)msg.iParam1);
				}
			}
			delete (EventArgs*)msg.iParam1;
		}
		break;
     case KMessageMoveEvent:
        {
            MoveEvent* moveEvent = (MoveEvent*)msg.iParam1;
            DebugPrintf(DLC_GESTURE, "MoveEvent %d, downTime: %d, eventTime: %d",
                    moveEvent->GetActionMasked(),
                    (int)moveEvent->GetDownTime(),
                    (int)moveEvent->GetEventTime());
            //StringManager::SetLanguage("English");
            if (CanResponseCommonMessage())
            {
                if (CNativeThread::GetKeyHomePressStatus())
                {
                    if (moveEvent->GetActionMasked() == MoveEvent::ACTION_DOWN)
                    {
                        struct timeval sTouchStartTime;
                        struct timeval sKeyHomePressedTime = CNativeThread::GetKeyHomePressedTime();
                        gettimeofday(&sTouchStartTime, NULL);
                        long intervalTimes = (sTouchStartTime.tv_sec - sKeyHomePressedTime.tv_sec)*1e6 +
                            (sTouchStartTime.tv_usec - sKeyHomePressedTime.tv_usec);
                        if (intervalTimes >= 1e6)//1-2s
                        {
                            CDisplay::GetDisplay()->PrintSrceen();
                            CNativeThread::SetScreenShotted(true);
                        }
                    }
                    CNativeThread::SetKeyHomeDealedAlready(true);
                }
                else
                {
                    if (!GlobalEventListener::GetInstance()->HookTouch(moveEvent))
                    {
                        UIPage* currentPage = CPageNavigator::GetCurrentPage();
                        if (NULL != currentPage)
                        {
                            currentPage->DispatchTouchEvent(moveEvent);
                        }
                    }
                    else
                    {
                        UIPage* currentPage = CPageNavigator::GetCurrentPage();
                        if (NULL != currentPage)
                        {
                            moveEvent->SetAction(MoveEvent::ACTION_CANCEL);
                            currentPage->DispatchTouchEvent(moveEvent);
                        }
                    }
                }
            }

            delete moveEvent;
        }
        break;
     case KMessageTimerEvent:
        {
            TimerFunc func = (TimerFunc)msg.iParam1;
            void* data = (void*)msg.iParam2;
            func(data);
        }
        break;
     case KMessagePrintScreen:
        if (CanResponseCommonMessage())
        {
            UIContainer* pCur = GUISystem::GetInstance()->GetTopFullScreenContainer();
            if (pCur)
            {
                CDisplay::GetDisplay()->PrintSrceen();
                UIMessageBox messagebox(pCur,StringManager::GetPCSTRById(TOUCH_SCREENSHOTTIPS), MB_OK);
                messagebox.DoModal();
            }
        }
        break;
    case KMessageWebBrowserStart:
        {
            fDisablePaint = true;
            fWebBrowserExec = true;
    
            // suspend display
            CDisplay::GetDisplay()->Suspend();

            // suspend Touch device, do noting now
            TouchDevice::Suspend();

            TitleBarSignalManager::GetTitleBarSignalManager()->Stop();
            if(GUIHelper::GetScreenSaverStatus() == 0)
            {

                UIPage* pCur = CDisplay::GetDisplay()->GetCurrentPage();
	            if(pCur && pCur->GetDialog())
                {
		            pCur->DestroyDialog();
                }
                GUIHelper::SetScreenSaverStatus(1);
            }

            // preparation for launching web browser is done. Launch it
            WebBrowserLauncher::GetInstance()->Launch();
        }
        break;
     case KMessageWebBrowserStop:
         {
            fDisablePaint = false;
            fWebBrowserExec = false;

            // resume display
            CDisplay::GetDisplay()->Resume();
            //if(GUIHelper::GetScreenSaverStatus() == 1)
            {    
                GUIHelper::SetScreenSaverStatus(0);
            }
            CDisplay::GetDisplay()->SetFullRepaint(true);
            TitleBarSignalManager::GetTitleBarSignalManager()->Start();
            CDisplay::GetDisplay()->Repaint(PAINT_FLAG_FULL);

            // Resume touch device
            TouchDevice::Resume();

            // KindleApp has resumed, notify stop event
            WebBrowserLauncher::GetInstance()->NotifyStopEvent();
         }
         break;
     case KMessageThirdPartyAppFinished:
         {
             ThirdPartyTypes type = (ThirdPartyTypes)msg.iParam1;
             switch (type)
             {
             case dk::thirdparties::EVERNOTE_TYPE:
                 EvernoteManager::GetInstance()->OnEvernoteExplorerQuited();
                 break;
             default:
                 break;
             }
         }
         break;
     case KMessageDuokanLoginFailed:
         {
             // Handle the error code
             // Ask Migration Manager to show options
             BookStoreErrno errorNo = (BookStoreErrno)msg.iParam1;
             switch (errorNo)
             {
             case BSE_DUOKAN_ACCOUNT_LOGIN_CLOSED:
                 MiMigrationManager::GetInstance()->ShowOptionsForDuokanLoginClosed();
                 break;
             case BSE_DUOKAN_ACCOUNT_REGISTER_CLOSED:
                 MiMigrationManager::GetInstance()->ShowOptionsForDuokanRegisterClosed();
                 break;
             default:
                 break;
             }
         }
         break;
     default:
         if (CanResponseCommonMessage())
         {
             EventListener* eventListener = GUISystem::GetInstance()->GetEventListenerByHandle(msg.hWnd);
             if (NULL != eventListener)
             {
                 eventListener->OnMessage(msg);
             }
         }

        break;
    }

    DebugPrintf(DLC_MESSAGE, "End process msg: %d: %s end", msg.iType, GetNativeMessageName(msg.iType));
    return CONTINUE_EXECUTION;
}

void CMessageHandler::SetLastDispatchMessage(SNativeMessage &msg, time_t &lastTime)
{
	m_lastMsg.hWnd = msg.hWnd;
	m_lastMsg.iType = msg.iType;
	m_lastMsg.iParam1 = msg.iParam1;
	m_lastMsg.iParam1 = msg.iParam2;
	m_lastMsg.iParam1 = msg.iParam3;

	m_lastMsgTime = lastTime;
}

void CMessageHandler::GetLastDispatchMessage(SNativeMessage &msg, time_t &lastTime)
{
	msg.hWnd = m_lastMsg.hWnd;
	msg.iType = m_lastMsg.iType;
	msg.iParam1 = m_lastMsg.iParam1;
	msg.iParam2 = m_lastMsg.iParam2;
	msg.iParam3 = m_lastMsg.iParam3;
	lastTime = m_lastMsgTime;
}

