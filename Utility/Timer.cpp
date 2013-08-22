////////////////////////////////////////////////////////////////////////
// Timer.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "Utility/Timer.h"
#include "Utility/TimerThread.h"
//#include "Utility/CWorkerThread.h"
//#include "Public/Utility/CSmartSemaphore.h"

Timer* Timer::m_spTimer= NULL;

void Timer::Schedule(ITimerTask* pTimerTask, INT64 liDelay)
{
    Schedule(pTimerTask, liDelay, 0);
}

void Timer::Schedule(ITimerTask* pTimerTask, INT64 liDelay, INT64 liPeriod)
{
    GetInstance()->TimerSchedule(pTimerTask, liDelay, liPeriod);
}

BOOL Timer::Cancel(ITimerTask* pTimerTask)
{
    return GetInstance()->TimerCancel(pTimerTask);
}

void Timer::Cancel()
{
    GetInstance()->TimerCancel();
}

Timer* Timer::GetInstance()
{
    if (NULL == m_spTimer)
    {
        m_spTimer = new Timer();
    }

    return m_spTimer;
}

Timer::Timer() 
{
    // empty
}

Timer::~Timer()
{
    m_timerTasks.clear();
    m_timerWorkIds.clear();
}

void Timer::TimerSchedule(ITimerTask* pTimerTask, INT64 liDelay)
{
    TimerSchedule(pTimerTask, liDelay, 0);
}

void Timer::TimerSchedule(ITimerTask* pTimerTask, INT64 liDelay, INT64 liPeriod)
{
    CTimerThread *pTimerThread = CTimerThread::GetInstance();
   if(NULL == pTimerThread)
    {
        return;
    }
    
    BOOL result = pTimerThread->AddTimer(pTimerTask, liDelay, liPeriod);
    if(FALSE == result)
    {
        return;
    }
    
    m_timerTasks.push_back(pTimerTask);
}

BOOL Timer::TimerCancel(ITimerTask* pTimerTask)
{
   CTimerThread *pTimerThread = CTimerThread::GetInstance();
   if(NULL == pTimerThread)
    {
        return FALSE;
    }
   
   BOOL result = pTimerThread->CancelTimer(pTimerTask);
   
   INT32 iSize = m_timerWorkIds.size();
   for (INT32 i = 0; i < iSize; i++)
   {
        if (m_timerTasks[i]== pTimerTask)
        {
            m_timerTasks.erase(m_timerTasks.begin() + i);

            break;
        }
    }

   return result;
   
}

void Timer::TimerCancel()
{
   CTimerThread *pTimerThread = CTimerThread::GetInstance();
   if(NULL == pTimerThread)
    {
        return;
    }

    INT32 iSize = m_timerWorkIds.size();
    for (INT32 i = 0; i < iSize; i++)
    {
        pTimerThread->CancelTimer(m_timerTasks[i]);

        m_timerTasks.erase(m_timerTasks.begin() + i);
        m_timerWorkIds.erase(m_timerWorkIds.begin() + i);
    }
}
