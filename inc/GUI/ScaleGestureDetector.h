#ifndef __GUI_SCALEGESTUREDETECTOR_H__
#define __GUI_SCALEGESTUREDETECTOR_H__

#include "GUI/EventListener.h"

class MoveEvent;
class ScaleGestureDetector;

class ScaleGestureListener
{
public:
    virtual ~ScaleGestureListener(){};
    virtual bool OnScale(ScaleGestureDetector* detector) = 0;
    virtual bool OnScaleBegin(ScaleGestureDetector* detector) = 0;
    virtual bool OnScaleEnd(ScaleGestureDetector* detector) = 0;
   
};

class ScaleGestureDetector: public EventListener
{
public:
    ScaleGestureDetector();
    void SetListener(ScaleGestureListener* listener)
    {
        m_listener = listener;
    }

    bool OnTouchEvent(MoveEvent* moveEvent);

    int GetFocusX() const
    {
        return m_focusX;
    }
 
    int GetFocusY() const
    {
        return m_focusY;
    }

    int GetCurrentSpan();

    int GetCurrentSpanX() const
    {
        return m_currFingerDiffX;
    }

    int GetCurrentSpanY() const
    {
        return m_currFingerDiffY;
    }
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
    ScaleGestureListener* m_listener;
    bool m_invalidGesture;
    int m_prevFingerDiffX;
    int m_prevFingerDiffY;
    int m_currFingerDiffX;
    int m_currFingerDiffY;
    int m_focusX;
    int m_focusY;
    int m_currLen;
    int m_prevLen;
};

#endif
