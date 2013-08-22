////////////////////////////////////////////////////////////////////////
// UITimerTask.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UITimerTask.h"
#include "Utility/Timer.h"
#include "Framework/CDisplay.h"

class UITimerRunnable : public IRunnable
{
public:
    UITimerRunnable(IUITimerListener* pTimerListener, INT32 iTimerId) :
        m_iTimerId(iTimerId), m_pTimerListener(pTimerListener), m_bCanceled(FALSE)
    {
        TP_ASSERT(pTimerListener != NULL);
    }

    ~UITimerRunnable()
    {
        // empty
    }

public:
    void Run()
    {
        if (m_bCanceled)
        {
            return;
        }

        m_pTimerListener->OnTimer(m_iTimerId);
    }

    void Cancel()
    {
        m_bCanceled = TRUE;
    }

private:
    INT32 m_iTimerId;
    IUITimerListener* m_pTimerListener;
    BOOL m_bCanceled;
};

UITimerTask::UITimerTask(IUITimerListener* pTimerListener, INT32 iId, INT64 liInterval, BOOL bRepeat) :
    m_iTimerId(iId), m_pTimerListener(pTimerListener), m_bRepeat(bRepeat), m_bCanceled(FALSE),
            m_spUIRunnable(new UITimerRunnable(pTimerListener, iId))
{
    TP_ASSERT(pTimerListener != NULL);
    TP_ASSERT(liInterval > 0);

    if (bRepeat)
    {
        Timer::Schedule(this, liInterval, liInterval);
    }
    else
    {
        Timer::Schedule(this, liInterval);
    }
}

UITimerTask::~UITimerTask()
{
    // it should be released by the one who creates it
    m_pTimerListener = NULL;

    if (IsRepeat())
    {
        Cancel();
    }
}

BOOL UITimerTask::IsRepeat()
{
    return m_bRepeat && !m_bCanceled;
}

BOOL UITimerTask::IsCanceled()
{
    return m_bCanceled;
}

INT32 UITimerTask::GetTimerId()
{
    return m_iTimerId;
}

void UITimerTask::TimerExpired()
{
    SPtr<IRunnable> spUIRunnable = m_spUIRunnable;
    if (!m_bCanceled)
    {
        CDisplay::GetDisplay()->SynchronizedCall(spUIRunnable);
    }
}

BOOL UITimerTask::Cancel()
{
    m_bCanceled = TRUE;

    UITimerRunnable* pUITimerRunnable = dynamic_cast<UITimerRunnable*>(m_spUIRunnable.Get());
    TP_ASSERT(pUITimerRunnable != NULL);
    pUITimerRunnable->Cancel();

    return Timer::Cancel(this);
}
