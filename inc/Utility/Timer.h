////////////////////////////////////////////////////////////////////////
// Timer.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __TIMER_H__
#define __TIMER_H__

#include "Utility/BasicType.h"
#include <vector>

class ITimerTask;

/// <summary>
/// This class is supposed to implement a timer in a platform-independent way.
/// This class is made as static class because we only require one timer to schedule
/// our tasks. It is expensive to have multiple timers in a handset device.
/// </summary>
/// <remark>
/// Generally a new thread should be created for a timer to execute the scheduled timer tasks.
/// </remark>
/// <remark>
/// Note, the singleton instance must be deleted when this application exists.
/// </remark>
class Timer
{
public:
    static void Schedule(ITimerTask* pTimerTask, INT64 liDelay);
    static void Schedule(ITimerTask* pTimerTask, INT64 liDelay, INT64 liPeriod);
    static BOOL Cancel(ITimerTask* pTimerTask);
    static void Cancel();

    static Timer* GetInstance();

public:
    ~Timer();

public:
    void TimerSchedule(ITimerTask* pTimerTask, INT64 liDelay);
    void TimerSchedule(ITimerTask* pTimerTask, INT64 liDelay, INT64 liPeriod);
    BOOL TimerCancel(ITimerTask* pTimerTask);
    void TimerCancel();

private:
    Timer();

private:
    static Timer* m_spTimer;

    std::vector<ITimerTask*> m_timerTasks;
    std::vector<INT32> m_timerWorkIds;
    
};

#endif
