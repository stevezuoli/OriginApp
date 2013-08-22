////////////////////////////////////////////////////////////////////////
// UITimerTask.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UITIMERTASK_H__
#define __UITIMERTASK_H__

#include "Utility/BasicType.h"
#include "Utility/ITimerTask.h"
#include "Utility/IRunnable.h"
#include "Public/GUI/IUITimerListener.h"
#include "DkSPtr.h"

//using namespace DkFormat;
using DkFormat::SPtr;

class UITimerTask : public ITimerTask
{
    ////////////////////Constructor Section/////////////////////////
public:
    UITimerTask(IUITimerListener* pTimerListener, INT32 iId, INT64 liInterval, BOOL bRepeat);
    virtual ~UITimerTask();
    ////////////////////Constructor Section/////////////////////////

    ////////////////////Method Section/////////////////////////
public:
    virtual BOOL IsRepeat();
    virtual BOOL IsCanceled();
    virtual BOOL Cancel();
    virtual INT32 GetTimerId();

    // from ITimerTask
    virtual void TimerExpired();
    ////////////////////Method Section/////////////////////////

    ////////////////////Field Section/////////////////////////
private:
    INT32 m_iTimerId;
    IUITimerListener* m_pTimerListener;
    BOOL m_bRepeat;
    BOOL m_bCanceled;

    SPtr<IRunnable> m_spUIRunnable;
    ////////////////////Field Section/////////////////////////
};

#endif
