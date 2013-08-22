#include "Framework/CNativeThread.h"
#include "GUI/GUISystem.h"
#include "GUI/PrintScreenGestureDetector.h"
#include "Utility/Misc.h"
#include "drivers/DeviceInfo.h"
#include "drivers/MoveEvent.h"
#include "interface.h"

PrintScreenGestureDetector::PrintScreenGestureDetector()
{
    m_gestureInProgress = false;
    m_activeId0 = m_activeId1 = -1;
    m_active0MostRecent = false;
    m_prevEvent = NULL;
    m_curEvent = NULL;
    m_invalidGesture = false;
    Reset();
}

void PrintScreenGestureDetector::Reset()
{
    if (NULL != m_prevEvent)
    {
        delete m_prevEvent;
        m_prevEvent = NULL;
    }
    if (NULL != m_curEvent)
    {
        delete m_curEvent;
        m_curEvent = NULL;
    }
    m_gestureInProgress = false;
    m_activeId0 = m_activeId1 = -1;
    m_active0MostRecent = false;
    m_invalidGesture = false;
    m_x0 = -1;
    m_y0 = -1;
    m_x1 = -1;
    m_y1 = -1;
    m_inPrintScreen = false;
    m_handled = false;
}

bool PrintScreenGestureDetector::IsPointOK() const
{
    int dist = 100;
    return IsPointNeighbour(m_x0, m_y0, 0, 0, dist) && 
            IsPointNeighbour(m_x1, m_y1,
                    DeviceInfo::GetDisplayScreenWidth(),
                    DeviceInfo::GetDisplayScreenHeight(),
                    dist);
}

bool PrintScreenGestureDetector::OnTouchEvent(MoveEvent* moveEvent)
{
    int action = moveEvent->GetActionMasked();
    if (m_invalidGesture)
    {
        return false;
    }
    if (!m_gestureInProgress)
    {
        switch (action)
        {
            case MoveEvent::ACTION_DOWN:
                m_activeId0 = moveEvent->GetPointerId(0);
                m_active0MostRecent = true;
                break;
            case MoveEvent::ACTION_UP:
                Reset();
                break;
            case MoveEvent::ACTION_POINTER_DOWN:
                {
                    if (NULL != m_prevEvent)
                    {
                        delete m_prevEvent;
                    }
                    m_prevEvent = moveEvent->Clone();
                    int index1 = moveEvent->GetActionIndex();
                    int index0 = moveEvent->FindPointerIndex(m_activeId0);
                    m_activeId1 = moveEvent->GetPointerId(index1);
                    if (index0 < 0 || index0 == index1)
                    {
                        index0 = FindNewActiveIndex(moveEvent, m_activeId1, -1);
                        m_activeId0 = moveEvent->GetPointerId(index0);       
                    }
                    m_active0MostRecent = false;
                    SetContext(moveEvent);
                    DebugPrintf(DLC_GESTURE, "OnScaleBegin");
                    m_gestureInProgress = true;
                    if (IsPointOK())
                    {
                        m_inPrintScreen = true;
                        StartPrintScreenHolding(false, moveEvent->GetEventTime() + 500, 0);
                    }
                }
                break;
            default:
                break;

        }
    }
    else
    {
        switch (action)
        {
            case MoveEvent::ACTION_POINTER_UP:
                {
                    SetContext(moveEvent);
                    int actionIndex = moveEvent->GetActionIndex();
                    int actionId = moveEvent->GetPointerId(actionIndex);
                    SetContext(moveEvent);
                    int activeId = actionId == m_activeId0 ? m_activeId1 : m_activeId0;
                    moveEvent->FindPointerIndex(activeId);
                    Reset();
                    m_activeId0 = activeId;
                    m_active0MostRecent = true;
                }
                break;
            case MoveEvent::ACTION_CANCEL:
                Reset();
                break;
            case MoveEvent::ACTION_UP:
                Reset();
                break;
            case MoveEvent::ACTION_MOVE:
                SetContext(moveEvent);
                delete m_prevEvent;
                m_prevEvent = moveEvent->Clone();
                break;
            default:
                break;
        }
    }
    return m_handled;
}

int PrintScreenGestureDetector::FindNewActiveIndex(MoveEvent* moveEvent, int otherActiveId, int removedPointerIndex) {
    int otherActiveIndex = moveEvent->FindPointerIndex(otherActiveId);
    for (int i = 0; i< moveEvent->GetPointerCount(); ++i)
    {
        if (i != removedPointerIndex && i != otherActiveIndex)
        {
            return i;
        }
    }
    return -1;
}

void PrintScreenGestureDetector::SetContext(MoveEvent* moveEvent)
{
    if (NULL != m_curEvent)
    {
        delete m_curEvent;
    }

    m_curEvent = moveEvent->Clone();
    MoveEvent* prev = m_prevEvent;
    MoveEvent* curr = m_curEvent;
    int prevIndex0 = prev->FindPointerIndex(m_activeId0);
    int prevIndex1 = prev->FindPointerIndex(m_activeId1);
    int currIndex0 = curr->FindPointerIndex(m_activeId0);
    int currIndex1 = curr->FindPointerIndex(m_activeId1);
    if (prevIndex0 < 0 || prevIndex1 < 0|| currIndex0 < 0 || currIndex1 < 0)
    {
        m_invalidGesture = true;
        DebugPrintf(DLC_GESTURE, "Inavlid MoveEvent stream detected.");
        return;
    }
    int cx0 = curr->GetX(currIndex0);
    int cy0 = curr->GetY(currIndex0);
    int cx1 = curr->GetX(currIndex1);
    int cy1 = curr->GetY(currIndex1);
    if(cx0 < cx1)
    {
        m_x0 = cx0;
        m_y0 = cy0;
        m_x1 = cx1;
        m_y1 = cy1;
    }
    else
    {
        m_x0 = cx1;
        m_y0 = cy1;
        m_x1 = cx0;
        m_y1 = cy0;
    }
    
    DebugPrintf(DLC_GESTURE, "PrintScreenGestureDetector::SetContext((%d,%d)~(%d,%d))",
            m_x0, m_y0, m_x1, m_y1);
}

IMPLEMENT_TIMER_THREAD_FUNC_WITHSTART(PrintScreenGestureDetector,
        PrintScreenHolding)

void PrintScreenGestureDetector::DoPrintScreenHolding()
{
    if (m_inPrintScreen)
    {
        DebugPrintf(DLC_GESTURE, "PrintScreenGestureDetector::DoOnPrintScreenHolding");
        SNativeMessage msg;
        msg.iType = KMessagePrintScreen;
        CNativeThread::Send(msg);

        m_handled = true;
    }
}


PrintScreenGestureDetector* PrintScreenGestureDetector::GetInstance()
{
    static PrintScreenGestureDetector s_detector;
    return &s_detector;
}

bool PrintScreenGestureDetector::OnTouchEventFunc(
        MoveEvent* moveEvent,
        void* data)
{
    PrintScreenGestureDetector* pThis = (PrintScreenGestureDetector*)data;
    return pThis->OnTouchEvent(moveEvent);
}
