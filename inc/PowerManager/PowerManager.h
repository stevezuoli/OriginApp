#ifndef __DKPowerManager_h__
#define __DKPowerManager_h__

#include "PowerManager/IPowerManager.h"
#include "../Common/Mutex.h"
#include "dkBaseType.h"

#include <pthread.h>
#include <vector>
#include "singleton.h"

#define  LOCK_MAX 1024

enum EPowerState
{
	Undefine,
	Unknown,
    Active,
	ScreenSaver,
	ReadytoSuspend,
	Suspended,
	BatteryLow
};

namespace PowerManagement{

    /* need to handle the thread safty */

    struct PM_LOCK_SRC_PAIR
    {
        PM_LOCK lock;
        PMSOURCE src;
        PM_LOCK_SRC_PAIR():lock((PM_LOCK)0), src((PMSOURCE)0)
        {
        }
    };

    class DKPowerManagerImpl : public IPowerManager ,public MutexBase
    {
        SINGLETON_H(DKPowerManagerImpl)

    public:
        DKPowerManagerImpl();
        ~DKPowerManagerImpl();

        virtual void AccquireLock(PM_LOCK lock, PMSOURCE src);
        virtual void ReleaseLock(PM_LOCK lock, PMSOURCE src);
        virtual int  GetPowerLevel(); 
        virtual bool IsCharging();

        virtual void StartWaitEvent();
		virtual void StartPowerdMonitor();
    private:
        static void* WaitEventFunc(void*);
		static void *PowerdMonitorFunc(void*);
        void PowerEventHandling(const char* event);
        void UpdateChargingStatus();
        void UpdatePowerLevel();
        void PreventScreenSaver(bool prevent);
        bool ReadPowerLevel();		
        bool ReadBatteryVoltage();
        bool ReadBatteryCapacity();
		bool ReadBatterymAH();
        void UpdateData();
        std::string GetKindleEvent();
        static int RestartPowerd();

        PM_LOCK_SRC_PAIR m_LockList[LOCK_MAX];
        UINT m_lockNum;
        pthread_t m_pmtid;
        int  m_powerlevel;
		int  m_voltage;
		int  m_mAH;
        bool m_chargestatus;
        time_t m_lastUpdateTime;
        time_t m_lastWaitTime;
    };
};

#endif //__DKPowerManager_h__
