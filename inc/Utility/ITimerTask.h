////////////////////////////////////////////////////////////////////////
// ITimerTask.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __ITIMERTASK_H__
#define __ITIMERTASK_H__

class ITimerTask
{
public:
    virtual ~ITimerTask()
    {
        // empty;
    }

public:
    /// <summary>
    /// This method is to be executed when the requied time is expired.
    /// </summary>
    virtual void TimerExpired() = 0;
    virtual BOOL IsCanceled() = 0;
    virtual BOOL IsRepeat() = 0;
    virtual INT32 GetTimerId() = 0;
};

#endif
