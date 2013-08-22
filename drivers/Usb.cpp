#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h> 
#include <sys/mount.h>
#include "drivers/Usb.h"
#include "Framework/INativeMessageQueue.h"
#include "GUI/CTpImage.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "Utility/SystemUtil.h"
#include "Utility/PathManager.h"

using dk::utility::SystemUtil;
using dk::utility::PathManager;

static const char* usbEvents[] = {"usbConfigured", "usbUnconfigured", "usbPlugOut", "usbPlugIn", NULL};
static int usbSignals[] = {usbConfigured, usbUnconfigured, usbPlugOut, usbPlugIn};
static int g_send_msg = 0;
static int g_usb_stat = -1;

static void SendUsbMessage(int eType)
{
    SNativeMessage usbMsg;
    usbMsg.iType = KMessageUsbSingle;
    usbMsg.iParam1 = eType;
    INativeMessageQueue::GetMessageQueue()->Send(usbMsg);
}

static int SigFromEventName(const char* eventName)
{
    for (int i = 0;  usbEvents[i]; ++i)
    {
        if (strcmp(eventName, usbEvents[i]) == 0)
        {
            return usbSignals[i];
        }
    }
    return -1;
}

static void HandleEvent(const char *eventName)
{
    int sig = SigFromEventName(eventName);
    if (sig != -1)
    {
        DebugPrintf(DLC_PENGF, "HandleUsbEvent: %s, sig: %d", eventName, sig);
        if (usbConfigured == sig)
        {
            g_usb_stat = 1;
        } 
        else if (usbUnconfigured == sig || usbPlugOut == sig)
        {
            g_usb_stat = 0;
        }
        SendUsbMessage(sig);
    }
}

static void* ThreadUsbSignal(void *lparam)
{
#ifdef _X86_LINUX
    return NULL;
#else
    char szCmd[128] = {0};
    sprintf(szCmd, "lipc-wait-event -s 0 com.lab126.hal  %s", (char*)lparam);
    while(1)
    {

        int res = 0;
        if((res = system(szCmd)) == 0)
        {
            DebugPrintf(DLC_PENGF, "wait %s return res %d", (const char*)lparam, res);
            if (g_send_msg);
            {
                HandleEvent((const char*)lparam);
            }
        }
    }
    return NULL;
#endif
}

static void CreateUsbThread(const char* usbEvent)
{
    pthread_attr_t threadAttr;
    pthread_t threadid;
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&threadAttr, PTHREAD_STACK_MIN);
    pthread_create(&threadid, &threadAttr, ThreadUsbSignal, (void*)usbEvent);
    pthread_attr_destroy(&threadAttr);
}

void StartWaitThread()
{
    for (size_t i = 0; usbEvents[i]; ++i)
    {
        CreateUsbThread(usbEvents[i]);
    }
}

extern char g_szAppPath[];
CUsb *CUsb::m_spInstance = 0;

CUsb::CUsb()
    : m_szUsbStatPath()
#ifdef _X86_LINUX
    , m_connected(false)
#endif

{
    memset(m_szUsbStatPath, 0, 512);
}

CUsb::~CUsb()
{

}

#ifdef _X86_LINUX
void CUsb::SetUsbConnected(bool connected)
{
    m_connected = connected;
}
#endif

CUsb *CUsb::GetInstance()
{
    if (m_spInstance == NULL)
    {
        m_spInstance = new CUsb();
    }

    return m_spInstance;
}

void CUsb::Release()
{
    if(m_spInstance != NULL)
    {
        delete m_spInstance;
        m_spInstance = NULL;
    }
}

char* CUsb::GetUsbStatPath()
{
#ifdef _X86_LINUX
    m_szUsbStatPath[0] = 0;
    return m_szUsbStatPath;
#else
    int  nReadLen = 0;
    FILE *fipath = NULL;
    if(m_szUsbStatPath[0] != 0)
    {
        return m_szUsbStatPath;
    }

    fipath = popen("kdb get system/driver/usb/gadget/SYS_GADGET_ONLINE", "r");
    if (NULL == fipath)
    {
        DebugPrintf(DLC_ERROR, "popen error\n");
        return NULL;
    }

    nReadLen = fread(m_szUsbStatPath, 1, sizeof(m_szUsbStatPath)-1, fipath);
    pclose(fipath);
    if(nReadLen <= 0)
    {
        return NULL;
    }
    m_szUsbStatPath[nReadLen-1] = '\0';
    return m_szUsbStatPath;
#endif
}
int CUsb::IsUsbConnected()
{
#ifdef _X86_LINUX
    return m_connected;
#else
    if(g_usb_stat != -1)
    {
        return g_usb_stat;
    }
    int  nStatus = -1;
    int  nReadLen = -1;
    FILE *fipath = NULL;
    char szStat[32] = {0};

    char *lpStatPath = GetUsbStatPath();
    if (NULL == lpStatPath)
    {
        return nStatus;
    }
    fipath = fopen(lpStatPath, "r");
    if (NULL == fipath)
    {
        return nStatus;
    }
    nReadLen = fread(szStat, 1, sizeof(szStat)-1, fipath); //»ñµÃÂ·¾¶
    if (nReadLen > 0)
    {
        szStat[nReadLen] = '\0';
        nStatus = atoi(szStat);
    }
    fclose(fipath);
    return nStatus;
#endif
}

static int UmountFlash()
{
    return umount("/mnt/base-us/");
}

static int MountFlash()
{
    //return mount( "/dev/loop/0",  "/mnt/base-us/",   "vfat"  ,  MS_MGC_VAL,  (void*)"defaults,noatime,nodiratime,utf8,noexec,shortname=winnt");
    int ret = SystemUtil::ExecuteCommand("mount -t vfat -o defaults,noatime,nodiratime,utf8,noexec,shortname=mixed /dev/loop/0 /mnt/base-us/");

    DebugPrintf(DLC_PENGF, "mount usb return %d", ret);
    int waitTime = 20;
    while (
            (!PathManager::IsFileExisting("/mnt/us/DK_Documents") 
             || !PathManager::IsFileExisting("/mnt/us/DK_System/xKindle/res"))
            && waitTime)
    {
        if (ret)
        {
            ret = SystemUtil::ExecuteCommand("mount -t vfat -o defaults,noatime,nodiratime,utf8,noexec,shortname=mixed /dev/loop/0 /mnt/base-us/");
            DebugPrintf(DLC_PENGF, "mount usb return %d", ret);
        }
        usleep(500000);
        --waitTime;
    }
    sleep(2); //TODO check if the sleep time setting is good. 
              //Extend 2 seconds to make sure the mount is done and ready for resources.
    DebugPrintf(DLC_PENGF, "mount flash return %d", ret);
    DebugPrintf(DLC_PENGF, "mount flash current mount:\n%s", SystemUtil::ReadStringFromShellOutput("mount -l").c_str());
    DebugPrintf(DLC_PENGF, "mount flash DK_System\\res\\userfonts:\n%s", SystemUtil::ReadStringFromShellOutput("ls -l /mnt/us/DK_System/xKindle/res/userfonts").c_str());
    return ret;
}

int CUsb::SwitchToOtg(const char *lpFlash, int nBufferLength)
{
    FILE *fi = NULL;
    int nWriteLength = -1;
    fi = fopen("/sys/devices/platform/fsl-usb2-udc/gadget/gadget-lun0/file", "w");      
    if(fi != NULL)
    {
        nWriteLength = fwrite(lpFlash, 1, nBufferLength, fi);
        fclose(fi);
    }
    return nWriteLength;
}

void CUsb::StartWaitUsbSingle()
{
    g_send_msg = 1;
    if(m_spInstance->IsUsbConnected() == 1)
    {
        SendUsbMessage(usbConfigured);
    }
    StartWaitThread();
}

int CUsb::SwitchToUsb()
{
    UmountFlash();
    return SwitchToOtg("/dev/mmcblk0p4", strlen("/dev/mmcblk0p4"));
}
int CUsb::SwitchToFlash()
{
    int nRet = SwitchToOtg("  ", 1);
    UmountFlash();
    MountFlash();
    return nRet;
}
void CUsb::ShowUsbPic(const char *lpFilePath)
{
    CTpImage img((char*)lpFilePath);
    CDisplay* display = CDisplay::GetDisplay();
    DK_IMAGE pGraphics = display->GetMemDC();
    CTpGraphics prg(pGraphics); 
    prg.DrawImage(&img, 0, 0);
    display->ForceDraw(PAINT_FLAG_FULL);
}

void CUsb::ShowConfigurePic()
{
    CString str1(g_szAppPath);
    CDisplay::GetDisplay()->DrawFullScreenPicture(str1 + "res/UsbConfigured.png");
}

void CUsb::ShowUnConfigurePic()
{
    CString str1(g_szAppPath);
    CDisplay::GetDisplay()->DrawFullScreenPicture(str1 + "res/UsbUnConfigured.png");
}




