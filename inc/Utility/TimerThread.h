#ifndef __TIMERTHREAD_INCLUDE__
#define __TIMERTHREAD_INCLUDE__


#include "Common/Defs.h"
#include "KernelDef.h"
#include "TimerList.h"
#include <list>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>

typedef void (*OldSignoFunc)(INT iSignal);

typedef struct itimerval TimerVal;

class CTimerThread
{
public:
    virtual ~CTimerThread();

    BOOL AddTimer(ITimerTask *pTimerTask, LONG lDelay, LONG lPeriod );

    BOOL CancelTimer(ITimerTask *pTimerTask);

    pthread_t GetTimerThreadId();

    static CTimerThread* GetInstance();

    static BOOL Start();
    static BOOL Stop();

private:
    static void* TimerThreadFunc(void *);
    static void NewSignalHandle(INT iSignal);

private:
    CTimerThread();
    BOOL Initialize();
    BOOL CreateTimer();
    BOOL DestroyTimer();
    void TimerThreadHangUp();
    void HandleAlarmSignal();

private:
    static CTimerThread *m_spTimerThread;

private:
    CTimerList m_TimerList;
    pthread_t m_TimerThread;
    pthread_cond_t m_TimerCond;
    pthread_mutex_t m_TimerMutex;
    TimerVal m_stOldVal;
    TimerVal m_stNewVal;
    OldSignoFunc m_pOldSignoCall;
    BOOL m_fTimerCreated;
};

namespace dk
{
namespace utility
{

typedef void(*TimerFunc)(void*);

class TimerThread
{
    struct TimerNode
    {
        TimerFunc func;
        void* data;
        bool repeat;
        bool handleInMsgThread;
        long intervalInMs;
        dkUpTime dueTimeByUptimeInMs;
    };

    TimerThread();
    ~TimerThread();

public:
    static TimerThread* GetInstance();
    void AddTimer(TimerFunc func, void* data, dkUpTime dueTimeByUptimeInMs, bool repeat, bool handleInMsgThread, long intervalInMs);
    bool CancelTimer(TimerFunc func, void* data);
    bool HasTimer(TimerFunc func, void* data);
    // Always call it very earler
    void Start();
private:
    long GetIntervalToFirstTimerInMs();
    bool GetTimer(TimerNode* node);
    static void* ThreadFunc(void*);
    void Run();
    DISALLOW_COPY_AND_ASSIGN(TimerThread);
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    std::list<TimerNode> m_timerList;
};
} // namespace utlity
} // namespace dk

#endif
