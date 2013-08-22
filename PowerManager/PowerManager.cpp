#include "PowerManager/PowerManager.h"
#include "Framework/CNativeThread.h"
#include "Framework/CMessageHandler.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <time.h>
#include "dkWinError.h"
#include "Utility/ThreadHelper.h"
#include "Utility/SystemUtil.h"
#include "Common/SystemOperation.h"
#include "interface.h"
#include "KernelDef.h"
#include "drivers/DeviceInfo.h"
#include "Utility/StringUtil.h"
#include "StopWatch.h"
#include "GUI/GUIHelper.h"

using namespace std;
using namespace PowerManagement;
using namespace dk::utility;

#define SCREENMESSAGE_IN 0
#define SCREENMESSAGE_OUT 1

//#define PM_MODULE_NAME_STRING(a) ".\\debug\\"#a"\\",
#define POWERDATA "/tmp/powerdata"
SINGLETON_CPP(DKPowerManagerImpl)

DKPowerManagerImpl::DKPowerManagerImpl()
    : m_lockNum(0)
    , m_powerlevel(0)
	, m_voltage(0)
    , m_chargestatus(false)
    , m_lastUpdateTime(0)
{
    PreventScreenSaver(false);
    ReadPowerLevel();
    DebugPrintf(
        DLC_DIAGNOSTIC,
        "DKPowerManagerImpl::UpdatePowerLevel init power=%d, voltage=%d, mAH=%d",
        m_powerlevel, m_voltage, m_mAH);

    m_lastWaitTime = time(NULL);
}

bool DKPowerManagerImpl::ReadBatteryVoltage()
{
#ifdef _X86_LINUX
	m_voltage = 4350;
	return true;
#else
    const char K3_VOLTAGE_FILE[] = "/sys/devices/system/luigi_battery/luigi_battery0/battery_voltage";
	const char K4_VOLTAGE_FILE[] = "/sys/devices/system/yoshi_battery/yoshi_battery0/battery_voltage";
    const char* voltageFile = NULL;

    if (DeviceInfo::IsK3())
    {
        voltageFile = K3_VOLTAGE_FILE;
    }
    else
    {
        voltageFile = K4_VOLTAGE_FILE;
    }

	int mV = 0;
    if (SystemUtil::ReadIntFromFile(voltageFile, &mV, false))
    {
        m_voltage = mV;
        return true;
    }
    else
    {
        return false;

    } 
#endif
}

bool DKPowerManagerImpl::ReadBatteryCapacity()
{
#ifdef _X86_LINUX
    m_powerlevel = 100;
    return true;
#else
    const char K3_POWER_FILE[] = "/sys/devices/system/luigi_battery/luigi_battery0/battery_capacity";
	const char K4_POWER_FILE[] = "/sys/devices/system/yoshi_battery/yoshi_battery0/battery_capacity";
    const char* powerFile = NULL;

    if (DeviceInfo::IsK3())
    {
        powerFile = K3_POWER_FILE;
    }
    else
    {
        powerFile = K4_POWER_FILE;
    }

    int powerLevel = 0;
    if (SystemUtil::ReadIntFromFile(powerFile, &powerLevel, false))
    {
        m_powerlevel = powerLevel;
        return true;
    }
    else
    {
        return false;
    }
#endif
}

bool DKPowerManagerImpl::ReadBatterymAH()
{
#ifdef _X86_LINUX
    m_mAH = 9999;
    return true;
#else
    const char K3_POWER_FILE[] = "/sys/devices/system/luigi_battery/luigi_battery0/battery_mAH";
	const char K4_POWER_FILE[] = "/sys/devices/system/yoshi_battery/yoshi_battery0/battery_mAH";
    const char* powerFile = NULL;

    if (DeviceInfo::IsK3())
    {
        powerFile = K3_POWER_FILE;
    }
    else
    {
        powerFile = K4_POWER_FILE;
    }

    int mAH = 0;
    if (SystemUtil::ReadIntFromFile(powerFile, &mAH, false))
    {
        m_mAH = mAH;
        return true;
    }
    else
    {
        return false;
    }
#endif
}

bool DKPowerManagerImpl::ReadPowerLevel()
{
	return ReadBatteryVoltage() && ReadBatteryCapacity() && ReadBatterymAH();
}



void DKPowerManagerImpl::PreventScreenSaver(bool prevent)
{
#ifdef _X86_LINUX
#else
    char buf[256] = {0};
    snprintf(buf, DK_DIM(buf), "lipc-set-prop com.lab126.powerd preventScreenSaver %d", prevent ? 1: 0);
    SystemUtil::ExecuteCommand(buf, false);
#endif
}

DKPowerManagerImpl::~DKPowerManagerImpl()
{
#ifdef _X86_LINUX
#else
    PreventScreenSaver(false);
#endif
}

/* void AccquireWifi (in long sourceID); */
void DKPowerManagerImpl::AccquireLock(PM_LOCK lock, PMSOURCE src)
{
    Mutex _lock(this);

//  debug sreeensaver
    //DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::AccquireLock, lock(%d),src=%d,m_lockNum=%d",  lock, src, m_lockNum);

    if(lock == PM_LOCK_SCREEN && src == MODULE_DOWNLOAD)
    {
        DebugPrintf(DLC_LIUJT, "DKPowerManagerImpl::AccquireLock for Download task");
    }

    if (m_lockNum < LOCK_MAX)
    {
        switch (lock)
        {
        case PM_LOCK_SCREEN:
            if (m_lockNum == 0) //the lock from 0 ==> 1, stop screensaver
            {
                //DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::AccquireLock:preventScreenSaver 1");
                PreventScreenSaver(true);
            }
            break;
        default:
            break;
        }

        m_LockList[m_lockNum].lock = lock;
        m_LockList[m_lockNum].src = src;        
        m_lockNum++;
    }

    return;
}

/* void ReleaseWifi (in long sourceID); */
void DKPowerManagerImpl::ReleaseLock(PM_LOCK lock, PMSOURCE src)
{
    Mutex _lock(this);

    //DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::ReleaseLock, lock(%d),src=%d,m_lockNum=%d",  lock, src, m_lockNum);

    for(UINT i=0; i < m_lockNum; i++)
    {
        if (lock == m_LockList[i].lock && src == m_LockList[i].src)
        {
            if (m_lockNum > 1 )
            {
                m_LockList[i].lock = m_LockList[m_lockNum-1].lock;
                m_LockList[i].src = m_LockList[m_lockNum-1].src;
            }

            m_lockNum--;
            break;
        }
    }

    if (m_lockNum == 0 )
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::ReleaseLock, preventScreenSaver 0");
        PreventScreenSaver(false);
    }

    return;
}

void DKPowerManagerImpl::PowerEventHandling(const char* event)
{
    if (NULL == event || 0 == event)
    {
        return;
    }
    //DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::PowerEventHandling kindle event=%s", event);

    if (StringUtil::StartWith(event, "userShutdown"))
    {
        // SystemOperation::CleanupEnv();
        SNativeMessage msg;
        msg.iType = KMessageScreenSaverIn;
		msg.iParam1 = Suspended;  //强制白屏
        CNativeThread::Send(msg);
        //DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::PowerEventHandling send out (userShutdown) KMessageScreenSaverIn");
    }
    
    if (StringUtil::StartWith(event, "outOfShutdown"))
    {
        // keep power for 7 seconds, system will goes to usershutdown.
        // slash the power will get outOfShutdown, can cancel the shutdown action
        SNativeMessage msg;
        msg.iType = KMessageScreenSaverOut;
        CNativeThread::Send(msg);
        //notify the app to repaint
    }

    if (StringUtil::StartWith(event, "charging") || StringUtil::StartWith(event, "notCharging"))
    {
        UpdateChargingStatus();
        
        SNativeMessage msg;
        msg.iType = KMessageUpdateTitleBar;
        INativeMessageQueue::GetMessageQueue()->Send(msg);
    }

#if 0
    if (_kindleEvent.find("suspending") != string::npos)
    {
        //Check lock, and delay the suspending

        if (m_lockNum > 0)
        {
            DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::PowerEventHandling suspending deferSuspend 300");
            SystemUtil::ExecuteCommand("lipc-set-prop -i com.lab126.powerd deferSuspend 300", false);
        }

        SNativeMessage msg;
        msg.iType = KMessageSuspending;
        CNativeThread::Send(msg);
        DebugPrintf(DLC_LIUJT, "DKPowerManagerImpl::PowerEventHandling send out KMessageSuspending");
    }
     if (_kindleEvent.find("resuming") != string::npos)
     {
         //do nothing, outOfScreenSaver is coming with "resuming"
         SNativeMessage msg;
         msg.iType = KMessageScreenSaverOut;
         CNativeThread::Send(msg);
         DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::PowerEventHandling resuming, send out KMessageScreenSaverOut with true parameter");
     }
#endif


    if (StringUtil::StartWith(event, "goingToScreenSaver"))
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::PowerEventHandling goingToScreenSaver");
        //TODO
        //user may trigger it. or by Kindle PM
        //check the screen lock
        //if true, defer the screensave
        //else do nothing
        //preventScreenSaver
           
        Mutex _lock(this);
		int index = 0;

        BOOL bHasScreenLock = FALSE;
        for(UINT i=0; i < m_lockNum; i++)
        {
            if ( PM_LOCK_SCREEN == m_LockList[i].lock )
            {
                bHasScreenLock = TRUE;
				index = i;
                break;
            }
        }

        if (!bHasScreenLock)
        {
            SNativeMessage msg;
            msg.iType = KMessageScreenSaverIn;
            CNativeThread::Send(msg);
            //DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::PowerEventHandling send out KMessageScreenSaverIn");
        }
        else
        {
            DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::PowerEventHandling has screen lock ,don't send out KMessageScreenSaverIn, src: %d", m_LockList[index].src);
        }
     }

     if (StringUtil::StartWith(event, "t1TimerReset") || StringUtil::StartWith(event, "outOfScreenSaver"))
     {
        //TODO user trigger it
        //notify the app to repaint
        SNativeMessage msg;
        msg.iType = KMessageScreenSaverOut;
        CNativeThread::Send(msg);
        DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::PowerEventHandling outOfScreenSaver, send out outOfScreenSaver with parameter false");        
     }

     if (StringUtil::StartWith(event, "battLevelChanged"))
     {
		 UpdatePowerLevel();   
     }

	// 针对有可能收不到屏保消息的情况，主动读取，系统powerd已经进入屏保状态，不能让屏幕处于活动状态
	if (StringUtil::StartWith(event, "screenSaver"))
    {
        SNativeMessage msg;
        msg.iType = KMessageScreenSaverIn;
		msg.iParam1 = ScreenSaver;
        CNativeThread::Send(msg);
        //DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::PowerEventHandling send out KMessageScreenSaverIn (screenSaver)");
    }

	if (StringUtil::StartWith(event, "active"))
    {
        SNativeMessage msg;
        msg.iType = KMessageScreenSaverOut;
		msg.iParam1 = Active;
        CNativeThread::Send(msg);
        //DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::PowerEventHandling send out KMessageScreenSaverOut (active)");
	}

}

int DKPowerManagerImpl::GetPowerLevel()
{
    UpdateData();
#ifdef _X86_LINUX
    return 100;
#else
    return m_powerlevel;
#endif
}


void DKPowerManagerImpl::UpdatePowerLevel()
{
    int oldPower = m_powerlevel;
	int oldVoltage = m_voltage;
	int oldmAH = m_mAH;
	
	// 采用新方法后， 比lipc-get-prop (socket)读取电量快速准确，不用担心阻塞
	ReadPowerLevel();

	if (oldPower == m_powerlevel)
	{
		return;
	}

	//DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::UpdatePowerLevel(%d to %d), Voltage(%d to %d), mAH(%d to %d), lsmod(ar600x):%s", \
	//	oldPower, m_powerlevel, oldVoltage, m_voltage, oldmAH, m_mAH , \
	//	SystemUtil::ReadStringFromShellOutput("lsmod | grep ar600").c_str(), false);
	
	SNativeMessage msg;
	msg.iType = KMessageBatteryUpdate;
	CNativeThread::Send(msg);
	SNativeMessage msg2;
	msg2.iType = KMessageUpdateTitleBar;
	INativeMessageQueue::GetMessageQueue()->Send(msg2);

	if (m_powerlevel <= 10 && oldPower > 10)
	{
		SNativeMessage msg;
		msg.iType = KMessageBatteryWarning;
        CNativeThread::Send(msg);
		return;
	}

	if (m_powerlevel <= 7)
	{
		SNativeMessage msg;
		msg.iType = KMessageBatteryLowAlarm;
        CNativeThread::Send(msg);
		return;
	}
	
	if ( m_powerlevel > 7)
	{
		SNativeMessage msg;
		msg.iType = KMessageBatteryLowDisalarm;
        CNativeThread::Send(msg);
		return;


	}
}

bool DKPowerManagerImpl::IsCharging()
{
    UpdateData();
    return m_chargestatus;
}

void DKPowerManagerImpl::UpdateChargingStatus()
{
    int charge = 0;
	const char *chargeFile;
	
	if (DeviceInfo::IsKPW())
    {
        chargeFile = "/sys/devices/platform/aplite_charger.0/charging";
    }
    else
    {
        chargeFile = "/sys/devices/platform/fsl-usb2-udc/charging";
    }

    m_chargestatus = SystemUtil::ReadIntFromFile(chargeFile, &charge, false) && (charge != 0);

}

void DKPowerManagerImpl::UpdateData()
{
    time_t now = 0;
    time(&now);
    int MIN_UPDATE_INTERVAL_IN_SECOND = 60;
    
    if (now - m_lastUpdateTime < MIN_UPDATE_INTERVAL_IN_SECOND)
    {
        return; 
    }
    UpdatePowerLevel();
    UpdateChargingStatus();

    m_lastUpdateTime = now;
}

void DKPowerManagerImpl::StartWaitEvent()
{
    //DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::StartWaitEvent()");
    pthread_attr_t threadAttr;
    pthread_t threadid;
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&threadAttr, PTHREAD_STACK_MIN);
    pthread_create(&threadid, &threadAttr, WaitEventFunc, this);
    pthread_attr_destroy(&threadAttr);
}

void* DKPowerManagerImpl::WaitEventFunc(void* arg)
{
#ifdef _X86_LINUX
    return NULL;
#else
    DKPowerManagerImpl* pThis = (DKPowerManagerImpl*)arg;
    while (true)
    {
		pThis->m_lastWaitTime = time(NULL);

        StopWatch sw;
        sw.Start();
        std::string res = SystemUtil::ReadStringFromShellOutput(
                "lipc-wait-event  -s 60 com.lab126.powerd outOfScreenSaver,goingToScreenSaver,userShutdown,outOfShutdown,t1TimerReset,charging,notCharging,battLevelChanged 2>>/mnt/us/DK_System/xKindle/powerError.log", false);
        sw.Stop();
        if (!res.empty())
        {
            //DebugPrintf(DLC_DIAGNOSTIC, "WaitEventFunc get %s", res.c_str());
            pThis->PowerEventHandling(res.c_str());
        }
        else
        {
            if (sw.DurationInMs() <= 500)
            {
                // sleep to avoid makding cpu overloaded
                sleep(5);
            }
        }
    }
    return NULL;
#endif
}

void DKPowerManagerImpl::StartPowerdMonitor()
{
    //DebugPrintf(DLC_DIAGNOSTIC, "DKPowerManagerImpl::StartPowerdMonitor()");
    pthread_attr_t threadAttr;
    pthread_t threadid;
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&threadAttr, PTHREAD_STACK_MIN);
    pthread_create(&threadid, &threadAttr, PowerdMonitorFunc, this);
    pthread_attr_destroy(&threadAttr);
}


// 修改监视频率， 避免耗电过大
// 正常运行时，监视频率每10秒1次，进入屏保后，2秒1次，以便进入屏保后，下次响应能快速退出屏保
// 系统进入(readyToSuspend)，然后转为(suspended)休眠后，程序会自动停止运行，不用担心
void* DKPowerManagerImpl::PowerdMonitorFunc(void* arg)
{
	
#ifdef _X86_LINUX
    return NULL;
#else
    DKPowerManagerImpl* pThis = (DKPowerManagerImpl*)arg;
	int interval = 20;
	int err = 0;	
	
	SNativeMessage msg;
	time_t lastMsgTime = 0;
	time_t curTime, lastTime;

	lastTime = time(NULL);

    while (true)
    {		
		sleep(interval);

		// 每隔一段时间，检测一次msg状态， 比如大约60秒
		curTime = time(NULL);
		if(curTime - lastTime > 60)
		{
			lastTime = curTime;
			CMessageHandler::GetLastDispatchMessage(msg, lastMsgTime);

			// 当前活跃的msg信息不打印（10秒内的消息不打印）
			//if(curTime - lastMsgTime > 10)
			//{
			//	DebugPrintf(DLC_DIAGNOSTIC, "GetLastDispatchMessage():msg time(%08X),type(%d),param(%d,%d,%d),name(%s)", \
			//		lastMsgTime, msg.iType, msg.iParam1, msg.iParam2, msg.iParam3, GetNativeMessageName(msg.iType));
			//}
		}

        std::string res = SystemUtil::ReadStringFromShellOutput("echo -n `lipc-get-prop com.lab126.powerd state`", false);
		 // DebugPrintf(DLC_DIAGNOSTIC, "PowerdMonitorFunc get (%s)", res.c_str());

		// 当前powerd状态读取失败，则重新启动powerd
		// com.lab126.powerd failed to access property state (0x3 lipcErrNoSuchSource)
		if (res.length() < 3 || res.find("com.lab126.powerd failed") != string::npos)
		{
			// 连续错误4次以上，才重启powerd， 以便过滤掉偶尔的错误
			if (++err > 4) 
			{
				err = 0;

				if (RestartPowerd() != 0)
				{
					sleep(30);
				}
			}
			interval = 5;
			continue;
		}
				
		err = 0;

		// lipc-wait-event 在正常情况下超时(2-3分钟)， 则kill之（排除睡眠状态）
		long delta = time(NULL) - pThis->m_lastWaitTime;
		if ( delta > 120 && delta < 180)
		{
			if (res.find("active") != string::npos) 
			{
				SystemUtil::ExecuteCommand("ps ax | grep lipc-wait-event | grep com.lab126.powerd | awk '{print $1}' | xargs kill");
			}
		}

		// 当前屏保状态与电源状态不一致，则发送消息
		interval = 20;
		switch(GUIHelper::GetScreenSaverStatus())
		{
		case 0:
			if (res.find("screenSaver") != string::npos) 
			{
				pThis->PowerEventHandling(res.c_str());
				interval = 5;
			}
			break;
		case 1:			
			if (res.find("active") != string::npos) 
			{
				pThis->PowerEventHandling(res.c_str());
				interval = 5;
			}
			break;
		default:
			break;
		}
	}	
    return NULL;
#endif
}

int DKPowerManagerImpl::RestartPowerd()
{

#ifdef _X86_LINUX
    return 0;
#else

	int err = 0;

	DebugPrintf(DLC_DIAGNOSTIC, "RestartPowerd()");	

	if (DeviceInfo::IsKPW())
	{
        if (DeviceInfo::GetKPVersion() >= 531) 
		{
			err = SystemUtil::ExecuteCommand("/DuoKan/powerd534");  // 5.3.1, 5.3.3, 5.3.4, the same powerd
        }
		else if (DeviceInfo::GetKPVersion() >= 530) 
		{
			err = SystemUtil::ExecuteCommand("/DuoKan/powerd530");
		}else
		{
			err = SystemUtil::ExecuteCommand("/DuoKan/powerd520");
		}
	}
	else if (DeviceInfo::IsK4Touch())
	{
		if (DeviceInfo::GetTouchVersion() >= 532) 
		{
			err = SystemUtil::ExecuteCommand("/DuoKan/powerd532");
		}
		else if (DeviceInfo::GetTouchVersion() >= 511) 
		{
			err = SystemUtil::ExecuteCommand("/DuoKan/powerd511");
		}
		else if(DeviceInfo::GetTouchVersion() >= 510) 
		{
			err = SystemUtil::ExecuteCommand("/DuoKan/powerd510");
		}
		else
		{
			err = SystemUtil::ExecuteCommand("start powerd");
		}
	}
	else
	{
		err = SystemUtil::ExecuteCommand("start powerd");
	}

	return err;

#endif
}
