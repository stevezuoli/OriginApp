#include <linux/input.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <linux/kd.h>
#include <pthread.h>
#include <signal.h>
#include "interface.h"	
#include "DownloadManager/DownloadManager.h"

#ifdef KINDLE_FOR_TOUCH	   		
#include "CommonUI/CPageNavigator.h"
#include "TouchAppUI/UIHomePage.h"	
#include "TouchAppUI/UINavigation.h"	  
#include "TouchAppUI/StartProgress.h"
#include "TouchAppUI/LayoutHelper.h"
#include "GUI/UIGesture.h"
#include "Common/WindowsMetrics.h"
#else						
#include "AppUI/UIHomePage.h"	
#include "AppUI/UITagPage.h"
#include "AppUI/UIIM.h"
#include "AppUI/UINavigation.h"	  
#include "AppUI/StartProgress.h"
#include "AppUI/LayoutHelper.h"
#endif

#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/BookStoreOrderManager.h"
#include "BookStore/OfflineBookStoreManager.h"

#include "AudioPlayer/AudioPlayer.h"
#include "Common/ConfigInfo.h"
#include "Common/DebugConsole.h"
#include "Common/FileManager_DK.h"
#include "Common/ReadingHistoryStat.h"
#include "drivers/CFbBitDc.h"
#include "drivers/Usb.h"
#include "Framework/INativeMessageQueue.h"
#include "Framework/CNativeThread.h"
#include "Framework/CDisplay.h"
#include "Framework/KernelGear.h"
#include "GUI/CTpGraphics.h"
#include "GUI/GUIHelper.h"
#include "Common/FileManager_DK.h"
#include "Common/DebugConsole.h"
#include "Common/BookCoverLoader.h"
#include "../../DkReader/Common/DKLogger.h"
#include "Common/ConfigInfo.h"
#include "Utility.h"
#include "../../DkReader/Common/DKLogger.h"
#include "PowerManager/PowerManager.h"
#include "Framework/BackgroundLoader.h"
#include "Framework/CHourglass.h"
#include "Utility/ThreadHelper.h"
#include "SQM.h"
#include "I18n/StringManager.h"
#include "Utility/FileSystem.h"     
#include "Wifi/WifiManager.h"
#include "Utility/Dictionary.h"
#include "Net/MailService.h"
#include "FontManager/DKFont.h"
#include "DKTAPI.h" 
#include "DKDParserKernelAPI.h" 

#include "drivers/TouchDrivers.h"
#include "TouchAppUI/BookOpenManager.h"
#include "Utility/PathManager.h"
#include "GUI/GlobalEventListener.h"
#include "GUI/PrintScreenGestureDetector.h"
#include "drivers/DeviceInfo.h"
#include "KernelVersion.h"
#include "Utility/SystemUtil.h"
#include "Utility/TimerThread.h"

#include "drivers/FrontLight.h"
#include "BookStore/LocalCategoryManager.h"
#include "GUI/ImageFullRepaintCalculation.h"
#include "XiaoMi/XiaoMiServiceFactory.h"
using namespace dk::utility;
using namespace dk::bookstore;

static int kbd_fd = 0;          // fd for main keyboard
static int kbd5_fd = 0;         // fd 五向键设备
static int kbd_volume_fd = 0;   // fd Kindle3音量键

#define SIG_BatteryChange              50
#define SIG_BatteryCharging            53
#define SIG_PowerStateChange        56
#define SIG_MailSync                        70

int InitKeyboard(const char * pszDev)
{
    if(NULL == pszDev)
    {
        return -1;
    }
    int s32Fd = open(pszDev, O_RDONLY|O_NOCTTY);
    if(s32Fd>=0)
    {
        //struct kbd_repeat kbdrep;
        //kbdrep.delay =  20000; /* ms */
        //kbdrep.period = 15000; /* ms */

        //if (!ioctl( s32Fd, KDKBDREP, &kbdrep )) {
        //    DebugPrintf(DLC_INPUT, "icotl Dev(%s) successfully. fd=%d", pszDev, s32Fd);
        //}

        //Remove it since the keyboard is not in control with ioctl.

        DebugPrintf(DLC_INPUT, "Open_Dev(%s) successfully. fd=%d", pszDev, s32Fd);
    }
    else
    {
        DebugPrintf(DLC_ERROR,"!!!!FAILED Open_Dev(%s), errno=%d", pszDev, errno);
    }

    if (s32Fd>=FD_SETSIZE)
    {
        DebugLog(DLC_ERROR, "The fd is not less than FD_SETSIZE!!!");
    }
    return s32Fd;
}

void InitAllKeys(void)
{
    kbd_fd = InitKeyboard("/dev/input/event0");
    kbd5_fd = InitKeyboard("/dev/input/event1");
    kbd_volume_fd = InitKeyboard("/dev/input/event2");
}

void CloseAllKeys(void)
{
    if (kbd_fd >= 0 )
        close(kbd_fd);
    if (kbd5_fd >= 0)
        close(kbd5_fd);
    if (kbd_volume_fd >= 0)
        close(kbd_volume_fd);
}

void  MailSyncHandle(int nSig)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "MailSyncHandle");
    MailService::GetInstance()->MailSyncHandle();
}

char g_szAppPath[PATH_MAX]={0};

void InitFontInternal(DK_CHARSET_TYPE charset)
{
    SystemSettingInfo* systemSettingInfo = SystemSettingInfo::GetInstance();
    std::wstring wFontName = EncodeUtil::ToWString(systemSettingInfo->GetFontByCharset(charset));

    // 如果配置文件中的字体不存在，则使用默认字体 strDefaultFont
    DKFont* pFont = g_pFontManager->GetFontForFaceName(wFontName);
    if (!pFont)
    {
        wFontName = EncodeUtil::ToWString(systemSettingInfo->GetDefaultFontByCharset(charset));
        systemSettingInfo->UseDefaultFont(charset);
    }
    else if (DK_CHARSET_ANSI != charset)
    {
        bool bSupportPRC (false);
        vector<unsigned short> lstCharSet;
        vector<unsigned short>::iterator it;
        pFont->GetSupportedCharSet (lstCharSet);
        for (it = lstCharSet.begin(); it < lstCharSet.end(); ++it)
        {
            if (DK_CHARSET_GB == *it || DK_CHARSET_BIG5 == *it)
            {
                bSupportPRC = true;
                break;
            }
        }

        // 针对该字体是否支持中文字符集做不同的默认字体设置
        // 当要设置的字体支持中文时，将所有字符集的默认字体均设置为该字体
        // 当要设置的字体不支持中文时，将中文字符集的默认字体设置为系统默认字体
        if (bSupportPRC)
        {
            g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_GB, wFontName);
            g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_SHITJIS, wFontName);
            g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_JOHAB, wFontName);
            g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_BIG5, wFontName);
        }
        else
        {
            g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_SHITJIS, wFontName);
            g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_JOHAB, wFontName);

            wstring strSysDefaultFont (L"");
            g_pFontManager->GetSystemDefaultFontFaceName(&strSysDefaultFont);
            g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_GB, strSysDefaultFont);
            g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_BIG5, strSysDefaultFont);
        }
        return ;
    }

    // 设置指定 eCharset 的默认字体
    g_pFontManager->SetDefaultFontFaceNameForCharSet(charset, wFontName.c_str());
}

void InitFontManager()
{
    DebugPrintf (DLC_LIUHJ, "Global InitFontManager Start");

    string sFontSysDir(SYSTEM_FONT_PATH);
    g_pFontManager->AddFontsDir(sFontSysDir);  
    string sFontUsrDir(USER_FONT_PATH);
    g_pFontManager->AddFontsDir(sFontUsrDir);     

    // 设置默认中文字体
    InitFontInternal(DK_CHARSET_GB);
    
    // 设置默认英文字体
    InitFontInternal(DK_CHARSET_ANSI);

    // 设置系统字体, 系统字体不应被用户修改
    wstring wFontName = EncodeUtil::ToWString(SystemSettingInfo::GetInstance()->GetSysDefaultFont());
    g_pFontManager->SetSystemDefaultFontFaceName(wFontName);
}

void sig_handle(int sig)
{
    switch (sig)
    {
        case SIG_MailSync:
            MailSyncHandle(sig);
            break;
        default:
            break;
    }
}

void* SigHandleThreadFunc(void*)
{
    sigset_t waitset;
    int sig;
    int rc;

    pthread_t ppid = pthread_self();
    pthread_detach(ppid);
    sigemptyset(&waitset);
    sigaddset(&waitset, SIG_MailSync);
    sigaddset(&waitset, SIGRTMIN);
    //DebugPrintf(DLC_DIAGNOSTIC, "sigwaitinfo() begin wait sig");
    while (1)
    {
        rc = sigwait(&waitset, &sig);
        if (rc != -1)
        {
            sig_handle(sig);
        }
        else
        {
            DebugPrintf(DLC_DIAGNOSTIC, "sigwaitinfo() returned err: %d\n", rc);
        }

    }
    return NULL;
}

void InstallSignalHandlers()
{
    sigset_t bset, oset;
    sigemptyset(&bset);
    sigaddset(&bset, SIGPIPE);
    sigaddset(&bset, SIGRTMIN);
    sigaddset(&bset, SIG_BatteryChange);
    sigaddset(&bset, SIG_BatteryCharging);
    sigaddset(&bset, SIG_PowerStateChange);
    sigaddset(&bset, SIG_MailSync);
    sigaddset(&bset, USB_CONFIGURED);
    sigaddset(&bset, USB_UNCONFIGURED);
    sigaddset(&bset, USB_PLUGOUT);
    sigaddset(&bset, USB_PLUGIN);
    if (pthread_sigmask(SIG_BLOCK, &bset, &oset) != 0)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "!!! Set pthread mask failed");
    }
    pthread_t ppid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&ppid, &attr, SigHandleThreadFunc, NULL);
    pthread_attr_destroy(&attr);
    return;
}

void InitBookStoreManagers()
{
    dk::bookstore::BookStoreInfoManager::GetInstance();
    dk::bookstore::BookStoreOrderManager::GetInstance();
    dk::bookstore::OfflineBookStoreManager::GetInstance();
}

void InitXiaoMiService()
{
    xiaomi::XiaoMiServiceFactory::GetMiCloudService();
}

void ScreenSaverFunc(void* data)
{
    static bool inSaver = false;
    SNativeMessage msg;
    if (!inSaver)
    {
        msg.iType = KMessageScreenSaverIn; 
        inSaver = true;
    }
    else
    {
        msg.iType = KMessageScreenSaverOut;
        inSaver = false;
    }
    CNativeThread::Send(msg);
}

int main(int argc, char *argv[])
{  
#ifdef _X86_LINUX
    int ch;
    while ((ch = getopt(argc, argv, "ud:")) != -1)
    {
        switch (ch)
        {
        case 'u':
        case '?':
            printf("Usage: \n\
                -d: SetDeviceType, eg.-d k3/k4/kt/kp");
            return 0;
        case 'd':
            if (strcmp(optarg, "k3") == 0)
            {
                printf("%s\n",optarg);
                DeviceInfo::SetDeviceType(DeviceInfo::KINDLE_3WIFI);
            }
            else if (strcmp(optarg, "k4") == 0)
            {
                printf("%s\n",optarg);
                DeviceInfo::SetDeviceType(DeviceInfo::KINDLE_4NT);
            }
            else if (strcmp(optarg, "kt") == 0)
            {
                printf("%s\n",optarg);
                DeviceInfo::SetDeviceType(DeviceInfo::KINDLE_4TOUCH);
            }
            else if (strcmp(optarg, "kp") == 0)
            {
                printf("%s\n",optarg);
                DeviceInfo::SetDeviceType(DeviceInfo::KINDLE_PW);
            }
            else
            {
                printf("Unknown device type");
                return -1;
            }
            break;
        }
    }
#endif
    SQM::GetInstance()->IncCounter(SQM_ACTION_SYSTEM_BOOT);            

    DKLogger::InitLogger();

    const char diagnostic[] = "DIAGNOSTIC";
    DKLogger::getInstance()->addModuleCfg(diagnostic, true, false, "/mnt/us/DK_System/xKindle/DIAGNOSTIC.log");
    EnableDebugDevice(DLC_DIAGNOSTIC, true);
#ifndef _RELEASE_VERSION
    //DKLogger::getInstance()->addModuleCfg("GESTURE", true, false, "/mnt/us/DK_System/xKindle/DIAGNOSTIC.log");
    //EnableDebugDevice(DLC_GESTURE, true);
#endif

	LoadConfigInfo();   //加载配置文件

	DebugPrintf(DLC_DIAGNOSTIC, "Diagnostic, kindle version(%s), device type: (%d), kernel version:(%s)", 
            SystemSettingInfo::GetInstance()->GetVersion().GetBuffer(),
            DeviceInfo::GetDeviceType(),
            DK_GetKernelVersion());
    DebugPrintf(DLC_DIAGNOSTIC, "free space:\n%s", SystemUtil::ReadStringFromShellOutput("free").c_str());
	DebugPrintf(DLC_DIAGNOSTIC, "\n%s\n", SystemUtil::ReadStringFromShellOutput("top -b -n1").c_str(), false);

    if (DeviceInfo::IsKPW())
    {
        WindowsMetrics::SetDeviceIndex(WindowsMetrics::DI_TOUCH_KPW);
    }
    
    PowerManagement::DKPowerManagerImpl::GetInstance()->StartPowerdMonitor();
    PowerManagement::DKPowerManagerImpl::GetInstance()->StartWaitEvent();
    CNativeThread::Initialize();
    //DebugLog(DLC_DIAGNOSTIC, "After CNativeThread::Initialize()");
    InstallSignalHandlers();

    SystemUtil::ExecuteCommand("lipc-set-prop com.lab126.powerd preventScreenSaver 0");
    SystemUtil::ExecuteCommand("killall -9 mplayer");

    // overcommit_memory = 0 ， 分配内存会检查内存不足(此时并未commit), fork()不成功，导致system(), popen()等失败
    // overcommit_memory = 1 ,  则直接放行
    int ret = SystemUtil::ExecuteCommand("sysctl vm.overcommit_memory=1");
    if (ret != 0)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "sysctl error(%s)", strerror(errno));
    }

    // Initialize BookStore Managers
    InitBookStoreManagers();
    InitXiaoMiService();

    // TODO: ignoring OUTOFMEMORY errors
    PathManager::Init();
    LocalCategoryManager::Init();
    TimerThread::GetInstance()->Start();
    BookCoverLoader::GetInstance()->Start();
    //TimerThread::GetInstance()->AddTimer(ScreenSaverFunc, NULL, SystemUtil::GetUpdateTimeInMs() + 60000, true, false, 30 * 1000);

    CFbBitDc screenDc;
    CDisplay* display = CDisplay::GetDisplay();
    display->SetScreenDC(&screenDc);

    //DebugPrintf(DLC_DIAGNOSTIC, "Start progress");
    CStartProgress progress(display);
    progress.ShowStartPicture();
    progress.SetProgressData(0, 80, 1);
    progress.Run();

    DebugPrintf(DLC_DIAGNOSTIC, "Check KPW front light");
	// IsKPW 点灯测试
	if (DeviceInfo::IsKPW())
	{
		FrontLight::Reset();
	}

    //DebugPrintf(DLC_DIAGNOSTIC, "Start bookstore download manager");
	dk::DownloadManager::GetInstance()->Start();
	dk::DownloadManager::GetXiaoMiRequestInstance()->Start();


    realpath(program_invocation_name, g_szAppPath);
    // TODO: Change to use dirname(
    LPSTR pstr=strrchr(g_szAppPath, '/');
    if (pstr)
    {
        *(pstr+1)=0;
    }

    
    
    CHourglass::Initialize();
    
    DebugPrintf(DLC_DIAGNOSTIC, "InitAllKeys");
    InitAllKeys();

    //CFCFontManager::InitFCAtSysStart();
    CBackgroundLoader::Start();
    
    DebugPrintf(DLC_DIAGNOSTIC, "InitFontManager");
    InitFontManager();

    // 初始化 Kernel 并注册、设置字体
    DebugPrintf(DLC_DIAGNOSTIC, "InitKernel");
    InitializeKernel ();
    DebugPrintf(DLC_DIAGNOSTIC, "RegisterKernelFonts");
    RegisterKernelFonts ();
    SetKernelDefaultFonts ();

    InitLanguage();
    
    Dictionary::LoadDict();
    
    DebugLog(DLC_COMM, "After LoadConfigInfo()");

    ImageFullRepaintCalculation* imageFullRepaint = ImageFullRepaintCalculation::GetInstance();
    if (imageFullRepaint)
    {
        const int horizontalPointNum = 3;
        const int verticalPointNum = 4;
        const int threshold = 4;
        imageFullRepaint->InitEnvironment(
            DeviceInfo::GetDisplayScreenWidth(),
            DeviceInfo::GetDisplayScreenHeight(),
            horizontalPointNum,
            verticalPointNum,
            threshold);
    }

    WifiManager* wifiBase = WifiManager::GetInstance();
    wifiBase->PowerOff();

    GUIHelper::SetScreenSaverStatus(0);


	DebugPrintf(DLC_DIAGNOSTIC, "CreateFileTagManger");
    CDKFileManager* pFileManager = CDKFileManager::GetFileManager();

    // TDOO:路径改成宏或从配置文件读取
	time_t start = time(NULL);
    
    // Do NOT load all files at beginning. Tree model will handle this
	DebugPrintf(DLC_DIAGNOSTIC, "Begin LoadFileToFileManger");
    pFileManager->LoadFileToFileManger("/mnt/us/", true);
	time_t end = time(NULL);
	long lTime = difftime(end, start);
	DebugPrintf(DLC_DIAGNOSTIC, "Main cost %ld seconds to scan all %d files", lTime, pFileManager->GetFileNum());

    progress.SetProgressData(100, 100, 1);
    progress.ProgressExit();

    //DebugPrintf(DLC_DIAGNOSTIC, "Start title bar sigal manager");
    TitleBarSignalManager::GetTitleBarSignalManager()->Start();
    // TODO: 如果最后Join了，其实可以考虑直接在主线程里起消息循环。
    pthread_t thrEventLoop;

    DebugPrintf(DLC_DIAGNOSTIC, "Begin Init Device");
    TouchDevice::InitDevice();
    GlobalEventListener::GetInstance()->RegisterTouchHook(
            PrintScreenGestureDetector::OnTouchEventFunc,
            PrintScreenGestureDetector::GetInstance());;
	ReadingHistoryStat::GetInstance();

    //pthread_create(&thrEventLoop, NULL, EventLoop, NULL);
    DebugPrintf(DLC_DIAGNOSTIC, "Begin Create main thread");
    ThreadHelper::CreateThread(&thrEventLoop, TouchDevice::EventLoop, NULL, "Main @ EventLoop");

    DebugPrintf(DLC_DIAGNOSTIC, "Start wait usb");
    CUsb::GetInstance()->StartWaitUsbSingle();
    //set the page to homepage
    DebugPrintf(DLC_DIAGNOSTIC, "Back to home");
    CPageNavigator::BackToHome();

#if 0
    if (SystemSettingInfo::GetInstance()->GetIsAppFirstLoading())
    {
        SystemSettingInfo::GetInstance()->SetConfigValue(CKI_AppFirstLoading, 0);
        int bookId = pFileManager->GetFileIdByPath("/mnt/us/DK_Documents/DuoKan2012ForKindleTouch.epub");
        if (bookId >= 0)
        {
            BookOpenManager::GetInstance()->Initialize(CPageNavigator::GetCurrentPage(), bookId, 0, 0);
            SNativeMessage msg;
            msg.iType = KMessageOpenBook;
            CNativeThread::GetInstance()->Send(msg);
        }
    }
#endif


	DebugPrintf(DLC_DIAGNOSTIC, "Start create guesture recognizer OK");

    ThreadHelper::JoinThread(thrEventLoop, NULL);

	DebugPrintf(DLC_DIAGNOSTIC, "JoinThread thrEventLoop OK");

    TouchDevice::CloseDevice();

	DebugPrintf(DLC_DIAGNOSTIC, "CloseDevice OK");


    SQM::GetInstance()->Flush();

	DebugPrintf(DLC_DIAGNOSTIC, "Flush OK");

    UnRegisterKernelFonts();
    DestroyKernel(); 

	DebugPrintf(DLC_DIAGNOSTIC, "DestroyKernel OK");

    return 0;
}

