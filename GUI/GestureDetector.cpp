#include "GUI/GUIConfiguration.h"
#include "GUI/GUISystem.h"
#include "GUI/GestureDetector.h"
#include "Utility/TimerThread.h"
#include "Utility/SystemUtil.h"
#include "interface.h"
#include "drivers/MoveEvent.h"
#include "Utility/Misc.h"

using namespace dk::utility;

GestureDetector::GestureDetector()
{
    const GUIConfiguration* guiConfiguration = GUIConfiguration::GetInstance();
    m_touchSlotSquare = guiConfiguration->GetTapSlotSquare();
    m_doubleTapSlotSquare = guiConfiguration->GetDoubleTapSlotSquare();
    m_doubleTapTouchSlotSquare = m_doubleTapSlotSquare;
    m_minFlingVelocity = guiConfiguration->GetMinFlingVelocity();
    m_maxFlingVelocity = guiConfiguration->GetMaxFlingVelocity();
    m_longPressTimeout = guiConfiguration->GetLongPressTimeout();
    m_tapTimeout = guiConfiguration->GetTapTimeout();
    m_doubleTapTimeout = guiConfiguration->GetDoubleTapTimeout();
    m_stillDown = false;
    m_inLongPress = false;
    m_alwaysInTapRegion = false;
    m_alwaysInBiggerTapRegion = false;
    m_isDoubleTapping = false;
    m_lastX = -1;
    m_lastY = -1;
    m_isLongPressEnabled = false;
    m_ignoreMultiTouch = true;
    m_listener = NULL;
    m_doubleTapListener = NULL;
    m_currentDownEvent = NULL;
    m_prevUpEvent = NULL;
    m_minFlingDistanceX = guiConfiguration->GetMinFlingDistanceX();
    m_minFlingDistanceY = guiConfiguration->GetMinFlingDistanceY();
    m_holdingX = -1;
    m_holdingY = -1;
    m_isHoldingEnabled = false;
    m_holdingTimeout = guiConfiguration->GetHoldingTimeout();
}

GestureDetector::~GestureDetector()
{
    ResetMoveEvents();
}

void GestureDetector::ResetMoveEvents()
{
    delete m_currentDownEvent;
    m_currentDownEvent = NULL;
    delete m_prevUpEvent;
    m_prevUpEvent = NULL;
}

bool GestureDetector::IsConsideredDoubleTap(MoveEvent* firstDown, MoveEvent* firstUp, MoveEvent* secondDown)
{
    if (!m_alwaysInBiggerTapRegion)
    {
        DebugPrintf(DLC_GESTURE, "GestureDetector::IsConsideredDoubleTap, not always in tap region!");
        return false;
    }
    if (secondDown->GetEventTime() - firstUp->GetEventTime() > m_doubleTapTimeout)
    {
        DebugPrintf(DLC_GESTURE, "GestureDetector::IsConsideredDoubleTap, double tap time out: %d, real time out: %d",
                m_doubleTapTimeout, secondDown->GetEventTime() - firstUp->GetEventTime());
        return false;
    }
    int dx = firstDown->GetX() - secondDown->GetX();
    int dy = firstDown->GetY() - secondDown->GetY();
    DebugPrintf(DLC_GESTURE, "GestureDetector::IsConsideredDoubleTap, dx*dx+dy*dy: %d, m_doubleTapTouchSlotSquare: %d",
            dx*dx+dy*dy, m_doubleTapTouchSlotSquare);
    return dx * dx + dy * dy < m_doubleTapTouchSlotSquare;
}

void GestureDetector::ResetHoldingTimeOut()
{
    const GUIConfiguration* guiConfiguration = GUIConfiguration::GetInstance();
    if (guiConfiguration)
    {
        m_holdingTimeout = guiConfiguration->GetHoldingTimeout();
    }
    else
    {
        m_holdingTimeout = 1000;
    }
}

FlingDirection GestureDetector::CalcFlingDirection(const MoveEvent* moveEvent1, const MoveEvent* moveEvent2)
{
    int dx = moveEvent2->GetX() - moveEvent1->GetX();
    int dy = moveEvent2->GetY() - moveEvent1->GetY();
    if (abs(dx) < m_minFlingDistanceX && abs(dy) < m_minFlingDistanceY)
    {
        return FD_UNKNOWN;
    }
    if (dx == 0)
    {
        return dy > 0 ? FD_DOWN : FD_UP;
    }
    if (dy == 0)
    {
        return dx > 0 ? FD_RIGHT : FD_LEFT;
    }
    double tan30degree = 0.5773;
    if (abs(dx) * 1.0 / abs(dy) < tan30degree)
    {
        return dy > 0 ? FD_DOWN : FD_UP;
    }
    else if (abs(dx) > abs(dy))
    {
        return dx > 0 ? FD_RIGHT : FD_LEFT;
    }

    return FD_UNKNOWN;
}

bool GestureDetector::OnTouchEvent(MoveEvent* moveEvent)
{
    int actionMask = moveEvent->GetActionMasked();
    DebugPrintf(DLC_GESTURE, "GestureDetector::OnTouchEvent(), action: %d -----------------time = %d, event time: %d", 
            actionMask,
            (int)SystemUtil::GetUpdateTimeInMs(),
            (int)moveEvent->GetEventTime());
    int x = moveEvent->GetX();
    int y = moveEvent->GetY();
    DebugPrintf(DLC_GESTURE, "GestureDetector::OnTouchEvent(), action: %d -----------------time = %d, event time: %d, (%d, %d)", 
            actionMask,
            (int)SystemUtil::GetUpdateTimeInMs(),
            (int)moveEvent->GetEventTime(),
            x,y);

    bool handled = false;
    switch (actionMask)
    {
    case MoveEvent::ACTION_DOWN:
        if (m_doubleTapListener != NULL)
        {
            DebugPrintf(DLC_GESTURE, "DoubleTapListener cancel single tap timer");
            DebugPrintf(DLC_GESTURE, "DoubleTapListener cancel single tap timer");
            bool hasTapMessage = TimerThread::GetInstance()->CancelTimer(
                    OnSingleTapConfirmedCallback,
                    (void*)GetHandle());
            DebugPrintf(DLC_GESTURE, "DoubleTapDetector: curDownEvent: %08X, prevUpEvent: %08X, hasTapMessage: %d",
                    m_currentDownEvent, m_prevUpEvent, hasTapMessage);
            DebugPrintf(DLC_GESTURE, "DoubleTapDetector: curDownEvent: %08X, prevUpEvent: %08X, hasTapMessage: %d",
                    m_currentDownEvent, m_prevUpEvent, hasTapMessage);
            if (m_currentDownEvent != NULL 
                    && m_prevUpEvent != NULL 
                    && hasTapMessage 
                    && IsConsideredDoubleTap(m_currentDownEvent, m_prevUpEvent, moveEvent))
            {
                m_isDoubleTapping = true;
                handled |= m_doubleTapListener->OnDoubleTap(moveEvent);
                handled |= m_doubleTapListener->OnDoubleTapEvent(moveEvent);
            }
            else
            {
                DebugPrintf(DLC_GESTURE, "StartOnSingleTapConfirmed");
                StartOnSingleTapConfirmed(false, moveEvent->GetDownTime() + m_doubleTapTimeout, 0);
            }
        }
        m_stillDown = true;
        m_inLongPress = false;
        m_lastX = x;
        m_lastY = y;
        if (NULL != m_currentDownEvent)
        {
            delete m_currentDownEvent;
        }
        m_currentDownEvent = moveEvent->Clone();
        m_alwaysInTapRegion = true;
        m_alwaysInBiggerTapRegion = true;

        if (m_isLongPressEnabled)
        {
            StopOnLongPress();
            StopOnHolding();
            DebugPrintf(DLC_GESTURE, "StartOnLongPress");
            StartOnLongPress(false,
                    m_currentDownEvent->GetEventTime() + m_longPressTimeout + m_tapTimeout,
                    0);
        }
        handled |= m_listener->OnDown(moveEvent);
        break;
    case MoveEvent::ACTION_MOVE:
        {
            if (NULL == m_currentDownEvent)
            {
                break;
            }
            int scrollX = x - m_lastX;
            int scrollY = y - m_lastY;
            if (m_isDoubleTapping)
            {
                handled |= m_doubleTapListener->OnDoubleTapEvent(moveEvent);
            }
            else if (m_alwaysInTapRegion)
            {
                int dx = x - m_currentDownEvent->GetX();
                int dy = y - m_currentDownEvent->GetY();
                int distance = dx * dx + dy * dy;
                if (distance > m_touchSlotSquare)
                {
                    handled |= m_listener->OnScroll(m_currentDownEvent, moveEvent, scrollX, scrollY); 
                    m_lastX = x;
                    m_lastY = y;
                    m_alwaysInTapRegion = false;
                    DebugPrintf(DLC_GESTURE, "Move StopOnSingleTapConfirmed");
                    StopOnSingleTapConfirmed();
                    StopOnLongPress();
                }
                if (distance > m_doubleTapSlotSquare)
                {
                    m_alwaysInBiggerTapRegion = false;
                }
            }
            else if (scrollX != 0 || scrollY != 0)
            {
                handled = m_listener->OnScroll(m_currentDownEvent, moveEvent, scrollX, scrollY);
                m_lastX = x;
                m_lastY = y;
            }
            if (m_isHoldingEnabled)
            {
                if (CalcDistanceSquare(x, y, m_holdingX, m_holdingY) > m_touchSlotSquare)
                {
                    StopOnHolding();
                    m_holdingX = x;
                    m_holdingY = y;
                    m_holdingStartTime = moveEvent->GetEventTime();
                    StartOnHolding(false, m_holdingStartTime + m_holdingTimeout, 0);
                }
            }
        }
        break;
    case MoveEvent::ACTION_UP:
        m_stillDown = false;
        if (m_isDoubleTapping)
        {
            handled |= m_doubleTapListener->OnDoubleTapEvent(moveEvent);
        }
        else if (m_inLongPress)
        {
            DebugPrintf(DLC_GESTURE, "InLongPress up StopOnSingleTapConfirmed()");
            StopOnSingleTapConfirmed();
            StopOnHolding();
            m_inLongPress = false;
        }
        else if (m_alwaysInTapRegion)
        {
            handled |= m_listener->OnSingleTapUp(moveEvent);
            if (m_doubleTapListener != NULL)
            {
                bool hasTapMessage = TimerThread::GetInstance()->HasTimer(
                        OnSingleTapConfirmedCallback,
                        (void*)GetHandle());
                if (!hasTapMessage)
                {
                    m_doubleTapListener->OnSingleTapConfirmed(moveEvent);
                }
            }
        }
        else
        {
            if (NULL != m_currentDownEvent)
            {
                FlingDirection direction = CalcFlingDirection(m_currentDownEvent, moveEvent);
                handled |= m_listener->OnFling(m_currentDownEvent, moveEvent, direction, 0, 0);
            }
        }
        if (NULL != m_prevUpEvent)
        {
            delete m_prevUpEvent;
        }
        m_prevUpEvent = moveEvent->Clone();
        m_isDoubleTapping = false;
        StopOnLongPress();
        DebugPrintf(DLC_GESTURE, "GestureDetector::OnMoveEvent(Up), StopOnHolding");
        StopOnHolding();
        break;
    case MoveEvent::ACTION_CANCEL:
        Cancel();
        break;
    }
    return handled;
}

void GestureDetector::Cancel()
{
    DebugPrintf(DLC_GESTURE, "Cancel StopOnSingleTapConfirmed");
    StopOnLongPress();
    StopOnSingleTapConfirmed();
    StopOnHolding();
    m_isDoubleTapping = false;
    m_stillDown = false;
    m_alwaysInTapRegion = false;
    m_alwaysInBiggerTapRegion = false;
    m_inLongPress = false;
}

IMPLEMENT_TIMER_THREAD_FUNC_WITHSTART(GestureDetector, OnSingleTapConfirmed)

void GestureDetector::DoOnSingleTapConfirmed()
{
    if (m_doubleTapListener != NULL && !m_stillDown)
    {
        DebugPrintf(DLC_GESTURE, "GestureDetector::DoOnSingleTapConfirmed() at up time %d",
            SystemUtil::GetUpdateTimeInMs());
        m_doubleTapListener->OnSingleTapConfirmed(m_currentDownEvent);
    }
}

IMPLEMENT_TIMER_THREAD_FUNC_WITHSTART(GestureDetector, OnLongPress)
void GestureDetector::DoOnLongPress()
{
    if (!m_stillDown)
    {
        return;
    }
    DebugPrintf(DLC_GESTURE, "GestureDetector::DoOnLongPress");
    StopOnSingleTapConfirmed();
    m_inLongPress = true;
    m_listener->OnLongPress(m_currentDownEvent);
    StopOnSingleTapConfirmed();
    if (m_isHoldingEnabled)
    {
        m_holdingX = m_lastX;
        m_holdingY = m_lastY;
        m_holdingStartTime = SystemUtil::GetUpdateTimeInMs();
        DebugPrintf(DLC_GESTURE, "StartOnHolding");
        StartOnHolding(false, m_holdingStartTime + m_holdingTimeout, 0);
    }
}

IMPLEMENT_TIMER_THREAD_FUNC_WITHSTART(GestureDetector, OnHolding)
void GestureDetector::DoOnHolding()
{
    if (!m_stillDown)
    {
        return;
    }
    DebugPrintf(DLC_GESTURE, "holdingStartTime: %d", m_holdingStartTime);
    if (SystemUtil::GetUpdateTimeInMs() - m_holdingStartTime >= m_longPressTimeout)
    {
        m_listener->OnHolding(m_holdingX, m_holdingY);
        m_holdingStartTime = SystemUtil::GetUpdateTimeInMs();
        StartOnHolding(false, m_holdingStartTime + m_holdingTimeout, 0);
    }
}
