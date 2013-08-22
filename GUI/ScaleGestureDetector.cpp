#include "GUI/ScaleGestureDetector.h"
#include "interface.h"
#include "drivers/MoveEvent.h"

ScaleGestureDetector::ScaleGestureDetector()
{
    m_gestureInProgress = false;
    m_activeId0 = m_activeId1 = -1;
    m_active0MostRecent = false;
    m_prevEvent = NULL;
    m_curEvent = NULL;
    m_listener = NULL;
    m_invalidGesture = false;
    Reset();
}

void ScaleGestureDetector::Reset()
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
}

bool ScaleGestureDetector::OnTouchEvent(MoveEvent* moveEvent)
{
    int action = moveEvent->GetActionMasked();
    bool handled = true;
    if (m_invalidGesture)
    {
        handled = false;
    }
    else if (!m_gestureInProgress)
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
                    m_gestureInProgress = m_listener->OnScaleBegin(this);
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
                    //SetContext(moveEvent);
                    int activeId = actionId == m_activeId0 ? m_activeId1 : m_activeId0;
                    int index = moveEvent->FindPointerIndex(activeId);
                    //m_focusX = moveEvent->GetX(index);
                    //m_focusY = moveEvent->GetY(index);
                    m_listener->OnScaleEnd(this);
                    Reset();
                    m_activeId0 = activeId;
                    m_active0MostRecent = true;
                }
                break;
            case MoveEvent::ACTION_CANCEL:
                m_listener->OnScaleEnd(this);
                Reset();
                break;
            case MoveEvent::ACTION_UP:
                Reset();
                break;
            case MoveEvent::ACTION_MOVE:
                SetContext(moveEvent);
                if (m_listener->OnScale(this))
                {
                    delete m_prevEvent;
                    m_prevEvent = moveEvent->Clone();
                }
                break;
            default:
                break;
        }
    }
    return handled;
}

int ScaleGestureDetector::FindNewActiveIndex(MoveEvent* moveEvent, int otherActiveId, int removedPointerIndex) {
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

void ScaleGestureDetector::SetContext(MoveEvent* moveEvent)
{
    if (NULL != m_curEvent)
    {
        delete m_curEvent;
    }

    m_currLen = -1;
    m_prevLen = -1;
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
        if (m_gestureInProgress)
        {
            m_listener->OnScaleEnd(this);
        }
        return;
    }
    int px0 = prev->GetX(prevIndex0);
    int py0 = prev->GetY(prevIndex0);
    int px1 = prev->GetX(prevIndex1);
    int py1 = prev->GetY(prevIndex1);
    int cx0 = curr->GetX(currIndex0);
    int cy0 = curr->GetY(currIndex0);
    int cx1 = curr->GetX(currIndex1);
    int cy1 = curr->GetY(currIndex1);
    
    m_prevFingerDiffX = px1 - px0;
    m_prevFingerDiffY = py1 - py0;
    m_currFingerDiffX = cx1 - cx0;
    m_currFingerDiffY = cy1 - cy0;
    m_focusX = (cx0 + cx1) / 2;
    m_focusY = (cy0 + cy1) / 2;
}

int ScaleGestureDetector::GetCurrentSpan()
{
    if (m_currLen == -1)
    {
        m_currLen = sqrt(m_currFingerDiffX * m_currFingerDiffX + m_currFingerDiffY * m_currFingerDiffY);
    }
    return m_currLen;
}
