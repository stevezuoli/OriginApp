#ifndef __GUI_PRINTSCREENGESTUREDETECTOR_H__
#define __GUI_PRINTSCREENGESTUREDETECTOR_H__

#include "GUI/EventListener.h"
class MoveEvent;

class PrintScreenGestureDetector: public EventListener
{
    PrintScreenGestureDetector();
public:
    bool OnTouchEvent(MoveEvent* moveEvent);
    static PrintScreenGestureDetector* GetInstance();
    static bool OnTouchEventFunc(MoveEvent* moveEvent, void* data);

private:
    int FindNewActiveIndex(MoveEvent* moveEvent, int otherActiveId, int removedPointerIndex);
    void Reset();
    void SetContext(MoveEvent* moveEvent);
    bool m_gestureInProgress;
    int m_activeId0;
    int m_activeId1;
    bool m_active0MostRecent;
    MoveEvent* m_prevEvent;
    MoveEvent* m_curEvent;
    bool m_invalidGesture;
    int m_x0, m_y0, m_x1, m_y1;
    bool IsPointOK() const;
    bool m_inPrintScreen;
    DECLARE_TIMER_THREAD_FUNC(PrintScreenHolding)
    bool m_handled;
};
#endif
