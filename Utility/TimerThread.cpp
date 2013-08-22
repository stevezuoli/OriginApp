#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "Utility/TimerThread.h"
#include "Utility/SystemUtil.h"
#include "Utility/ThreadHelper.h"
#include "interface.h"
#include "Framework/INativeMessageQueue.h"
#include "Framework/CNativeThread.h"
#include "Utility/ThreadHelper.h"

#define MILLISEC 1000

CTimerThread* CTimerThread::m_spTimerThread = NULL;

CTimerThread::CTimerThread()
    : m_TimerThread(-1)
    , m_pOldSignoCall(NULL)
    , m_fTimerCreated(FALSE)
{
    pthread_mutex_init(&m_TimerMutex, NULL);
    pthread_cond_init (&m_TimerCond, NULL);

    memset(&m_stNewVal, 0, sizeof(TimerVal));
    memset(&m_stOldVal, 0, sizeof(TimerVal));
}

CTimerThread::~CTimerThread()
{
    pthread_mutex_destroy(&m_TimerMutex);
    pthread_cond_destroy(&m_TimerCond);
}

BOOL CTimerThread::AddTimer(ITimerTask *pTimerTask, LONG lDelay, LONG lPeriod)
{
    if(NULL == m_spTimerThread || NULL == pTimerTask ||
       lDelay < 0 || lPeriod < 0)
    {
        return FALSE;
    }

    BOOL result =FALSE;
    pthread_mutex_lock(&m_TimerMutex);
    result = this->m_TimerList.AddTimer(pTimerTask, lDelay, lPeriod);
    if(result && this->m_TimerList.GetLength() == 1)
    {
        pthread_cond_signal(&m_TimerCond);
    }

    pthread_mutex_unlock(&m_TimerMutex);

    return result;
}

BOOL CTimerThread::CancelTimer(ITimerTask *pTimerTask)
{
    if(NULL == pTimerTask || NULL == m_spTimerThread)
    {
        return FALSE;
    }

    BOOL result = FALSE;

    pthread_mutex_lock(&m_TimerMutex);

    result = this->m_TimerList.RemoveTimer(pTimerTask);

    pthread_mutex_unlock(&m_TimerMutex);
    return result;
}

pthread_t CTimerThread::GetTimerThreadId()
{
    return this->m_TimerThread;
}

CTimerThread* CTimerThread::GetInstance()
{
    if(NULL == m_spTimerThread)
    {
        BOOL result = Start();
        if(FALSE == result)
        {
            return NULL;
        }    
    }
    
    return m_spTimerThread;
}

BOOL CTimerThread::Start()
{
    if(NULL == m_spTimerThread)
    {
        m_spTimerThread = new CTimerThread();
        if(NULL == m_spTimerThread)
        {
            return FALSE;
        }

        BOOL result = m_spTimerThread->Initialize();
        if(!result)
        {
            delete m_spTimerThread;
            m_spTimerThread = NULL;
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CTimerThread::Stop()
{
    if(NULL == m_spTimerThread)
    {
        return FALSE;
    }

    pthread_t tThreadId = m_spTimerThread->GetTimerThreadId();
    if(tThreadId < 0)
    {
        return FALSE;
    }

    ThreadHelper::CancelThread(tThreadId);
    ThreadHelper::JoinThread(tThreadId, NULL);
    m_spTimerThread->DestroyTimer();

    delete m_spTimerThread;
    m_spTimerThread = NULL;
    return TRUE;
}

BOOL CTimerThread::Initialize()
{
    m_stNewVal.it_value.tv_sec = 1;
    m_stNewVal.it_value.tv_usec = 0; //MILLISEC;
    m_stNewVal.it_interval.tv_sec = 1;
    m_stNewVal.it_interval.tv_usec = 0;//MILLISEC;

    INT iRet = ThreadHelper::CreateThread(&(this->m_TimerThread),
                                                CTimerThread::TimerThreadFunc,
                                                NULL,
                                                "CTimerThread @ TimerThreadFunc");
    if(iRet == 0)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CTimerThread::CreateTimer()
{
    if(FALSE == this->m_fTimerCreated)
    {
        m_pOldSignoCall = signal(SIGALRM, CTimerThread::NewSignalHandle);
        if(m_pOldSignoCall == SIG_ERR)
        {
            return FALSE;
        }

        INT iRet = setitimer(ITIMER_REAL, &(m_stNewVal), &(m_stOldVal));
        if(iRet < 0)
        {
            return FALSE;
        }

        this->m_fTimerCreated = TRUE;
    }

    return TRUE;
}

BOOL CTimerThread::DestroyTimer()
{
    DebugPrintf(DLC_ZHANGJF, "CTimerThread::DestroyTimer entering");
    if(TRUE == this->m_fTimerCreated)
    {
        DebugPrintf(DLC_ZHANGJF, "CTimerThread::DestroyTimer destroy begain");
        DebugPrintf(DLC_ZHANGJF, "CTimerThread::DestroyTimer destroy m_pOldSignoCall %x", m_pOldSignoCall);

        signal(SIGALRM, m_pOldSignoCall);

        INT iRet = setitimer(ITIMER_REAL, &(m_stOldVal), &(m_stNewVal));
        DebugPrintf(DLC_ZHANGJF, "CTimerThread::DestroyTimer destroy iRet %d", iRet);
        if(iRet < 0)
        {
            return FALSE;
        }

        this->m_fTimerCreated = FALSE;
        DebugPrintf(DLC_ZHANGJF, "CTimerThread::DestroyTimer destroy out");
    }

    return TRUE;
}

void CTimerThread::TimerThreadHangUp()
{
    if(NULL == m_spTimerThread)
    {
        return;
    }

    pthread_mutex_lock(&m_TimerMutex);

    while(0 == m_spTimerThread->m_TimerList.GetLength())
    {
        m_spTimerThread->DestroyTimer();
        pthread_cond_wait(&m_TimerCond, &m_TimerMutex);
    }
    DebugPrintf(DLC_ZHANGJF, "CTimerThread::TimerThreadHangUp ing...");
    pthread_mutex_unlock(&m_TimerMutex);

    this->m_fTimerCreated ? sleep(1) : m_spTimerThread->CreateTimer();
}

void CTimerThread::HandleAlarmSignal()
{
    if(NULL == m_spTimerThread)
    {
        return;
    }

    pthread_mutex_lock(&m_TimerMutex);

    INT iListLen = m_spTimerThread->m_TimerList.GetLength();
    PTimerListItem pItem = NULL;
    
    for(INT iIndex = 0; iIndex < iListLen; iIndex++)
    {
        //DebugPrintf(DLC_ZHANGJF, "CTimerThread::NewSignoHandle iIndex %d", iIndex);
        pItem = m_spTimerThread->m_TimerList.Get(iIndex);
        if(NULL == pItem || NULL == pItem->m_pTimerTask)
        {
            continue;
        }

        if(pItem->m_pTimerTask->IsCanceled())
        {
            continue;
        }
        
        pItem->m_iElapse++;
        //DebugPrintf(DLC_ZHANGJF, "CTimerThread::NewSignoHandle pItem->m_iElapse %d", pItem->m_iElapse);
        
        if((LONG)pItem->m_iElapse == pItem->m_lNewInterval)
        {
            pItem->m_pTimerTask->TimerExpired();
            if(pItem->m_pTimerTask->IsRepeat())
            {
                pItem->m_iElapse = 0;
            }
        }
    }
    
    pthread_mutex_unlock(&m_TimerMutex);
}

void* CTimerThread::TimerThreadFunc(void *)
{
    if(NULL == m_spTimerThread)
    {
        return NULL;
    }

    while(TRUE)
    {
        m_spTimerThread->TimerThreadHangUp();  
        
    }

    ThreadHelper::ThreadExit(NULL);
}

void CTimerThread::NewSignalHandle(INT iSignal)
{
    DebugPrintf(DLC_ZHANGJF, "CTimerThread::NewSignoHandle entering");
    if(NULL == m_spTimerThread || iSignal != SIGALRM)
    {
        return;
    }

    m_spTimerThread->HandleAlarmSignal();
}


namespace dk
{
namespace utility
{
TimerThread::TimerThread()
{
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init (&m_cond, NULL);
}

TimerThread::~TimerThread()
{
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}

TimerThread* TimerThread::GetInstance()
{
    static TimerThread s_timerThread;
    return &s_timerThread;
}

void TimerThread::AddTimer(TimerFunc func, void* data, int64_t dueTimeByUptimeInMs, bool repeat, bool handleInMsgThread, long intervalInMs)
{
    DebugPrintf(DLC_GESTURE, "TimerThread::AddTimer(0x%08X, 0x%08X, %d)", func, data, (int)dueTimeByUptimeInMs);
    pthread_mutex_lock(&m_mutex);
    TimerNode node;
    node.func = func;
    node.data = data;
    node.repeat = repeat;
    node.handleInMsgThread = handleInMsgThread;
    node.intervalInMs = intervalInMs;
    node.dueTimeByUptimeInMs = dueTimeByUptimeInMs;
    DK_AUTO(cur, m_timerList.begin());
    for (; cur != m_timerList.end(); ++cur)
    {
        if (cur->dueTimeByUptimeInMs > node.dueTimeByUptimeInMs)
        {
            break;
        }
    }
    m_timerList.insert(cur, node);
    DebugPrintf(DLC_GESTURE, "TimerThread::AddTimer insert(0x%08X, 0x%08X, %d), size: %d", func, data, (int)dueTimeByUptimeInMs, m_timerList.size());
    pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_mutex);
}

bool TimerThread::GetTimer(TimerNode* node)
{
    pthread_mutex_lock(&m_mutex);
    if (m_timerList.empty())
    {
        pthread_cond_wait(&m_cond, &m_mutex);
    }
    if (m_timerList.empty())
    {
        pthread_mutex_unlock(&m_mutex);
        return false;
    }
    *node = m_timerList.front();
    if (node->dueTimeByUptimeInMs <= SystemUtil::GetUpdateTimeInMs())
    {
        m_timerList.pop_front();
        DebugPrintf(DLC_GESTURE, "TimerThread::GetTimer, func time: %d, cur time: %d, size: %d",
                (int)node->dueTimeByUptimeInMs,
                (int)SystemUtil::GetUpdateTimeInMs(),
                m_timerList.size());
        pthread_mutex_unlock(&m_mutex);
        return true;
    }
    pthread_mutex_unlock(&m_mutex);
    return false;
}

bool TimerThread::CancelTimer(TimerFunc func, void* data)
{
    DebugPrintf(DLC_GESTURE, "Cancel Timer(0x%08X, 0x%08X", func, data);
    bool canceled = false;
    pthread_mutex_lock(&m_mutex);
    for (DK_AUTO(cur, m_timerList.begin()); cur != m_timerList.end();)
    {
        DebugPrintf(DLC_GESTURE, "func, data:(0x%08X, 0x%08X)", cur->func, cur->data);
        if (cur->func == func && cur->data == data)
        {
            cur = m_timerList.erase(cur);
            DebugPrintf(DLC_GESTURE, "TimerThread::DoErase, func, data:(0x%08X, 0x%08X), size: %d", cur->func, cur->data, m_timerList.size());
            canceled = true;
        }
        else
        {
            ++cur;
        }
    }
    pthread_mutex_unlock(&m_mutex);
    DebugPrintf(DLC_GESTURE, "CancelTimer return %d", canceled);
    return canceled;
}

bool TimerThread::HasTimer(TimerFunc func, void* data)
{
    DebugPrintf(DLC_GESTURE, "HasTimer(0x%08X, 0x%08X", func, data);
    bool result = false;
    pthread_mutex_lock(&m_mutex);
    for (DK_AUTO(cur, m_timerList.begin()); cur != m_timerList.end();)
    {
        DebugPrintf(DLC_GESTURE, "func, data:(0x%08X, 0x%08X)", cur->func, cur->data);
        if (cur->func == func && cur->data == data)
        {
            result = true;
            break;
        }
        else
        {
            ++cur;
        }
    }
    pthread_mutex_unlock(&m_mutex);
    DebugPrintf(DLC_GESTURE, "HasTimer return %d", result);
    return result;
}

void* TimerThread::ThreadFunc(void* data)
{
    TimerThread* pThis = (TimerThread*)data;
    pThis->Run();
    return NULL;
}

void TimerThread::Start()
{
    // Always call it very earler
    // don't need to add lock
    static bool s_init = false;
    if (s_init)
    {
        return;
    }
    s_init = true;
    pthread_t tid;
    ThreadHelper::CreateThread(&tid, &TimerThread::ThreadFunc, this, "TimerThread", true);
}

long TimerThread::GetIntervalToFirstTimerInMs()
{
    const int DEFAULT_INTERVAL_IN_MS = 50;
    long result = DEFAULT_INTERVAL_IN_MS;
    int64_t curTime = SystemUtil::GetUpdateTimeInMs();

    pthread_mutex_lock(&m_mutex);
    if (!m_timerList.empty())
    {
        result = m_timerList.front().dueTimeByUptimeInMs - curTime;
        if (result < 0)
        {
            result = 0;
        }
        if (result > DEFAULT_INTERVAL_IN_MS)
        {
            result = DEFAULT_INTERVAL_IN_MS;
        }
    }
    pthread_mutex_unlock(&m_mutex);
    return result;
}


void TimerThread::Run()
{
    for (;;)
    {
        TimerNode node;
        while (GetTimer(&node))
        {
            DebugPrintf(DLC_GESTURE, "TimerThread::Run, Begin run (0x%08X, 0x%08X)", node.func, node.data);
            if (node.handleInMsgThread)
            {
                SNativeMessage msg;
                msg.iType = KMessageTimerEvent;
                msg.iParam1 = (uint32_t)node.func;
                msg.iParam2 = (uint32_t)node.data;
                CNativeThread::Send(msg);
            }
            else
            {
                node.func(node.data);
            }
            if (node.repeat)
            {
                AddTimer(node.func, node.data, node.dueTimeByUptimeInMs + node.intervalInMs, true, node.handleInMsgThread, node.intervalInMs);
            }
        }
        long sleepInterval = GetIntervalToFirstTimerInMs();
        if (sleepInterval > 0)
        {
            usleep(sleepInterval);
        }
    }
}
} // namespace utlity
} // namespace dk
