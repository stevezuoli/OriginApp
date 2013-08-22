#ifndef __GUI_GESTUREDETECTOR_H__
#define __GUI_GESTUREDETECTOR_H__

#include "GUI/EventListener.h"
#include "KernelDef.h"

class MoveEvent;

enum FlingDirection
{
    FD_UNKNOWN,
    FD_LEFT,
    FD_UP,
    FD_RIGHT,
    FD_DOWN
};

inline FlingDirection TransformFlingDirection(FlingDirection direction, int degree)
{
    degree /= 90;
    int d = direction;
    d -= FD_LEFT;
    d += degree;
    d %= 4;
    d += FD_LEFT;
    return (FlingDirection)d;
}

class GestureListener
{
public:
    virtual ~GestureListener(){};
    virtual bool OnDown(MoveEvent* moveEvent) = 0;
    virtual bool OnSingleTapUp(MoveEvent* moveEvent) = 0;
    virtual bool OnScroll(MoveEvent* moveEvent1, MoveEvent* moveEvent2, int distanceX, int distanceY) = 0;
    virtual void OnLongPress(MoveEvent* moveEvent) = 0;
    virtual void OnHolding(int x, int y) = 0;
    // currently we don't calculate vx and vy, don't use it
    virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy) = 0;
};

class SimpleGestureListener: public GestureListener
{
public:
    virtual bool OnDown(MoveEvent* moveEvent)
    {
        return false;
    }

    virtual bool OnSingleTapUp(MoveEvent* moveEvent)
    {
        return false;
    }

    virtual bool OnScroll(MoveEvent* moveEvent1, MoveEvent* moveEvent2, int distanceX, int distanceY)
    {
        return false;
    }

    virtual void OnLongPress(MoveEvent* moveEvent)
    {
    }

    // currently we don't calculate vx and vy, don't use it
    virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
    {
        return false;
    }

    virtual void OnHolding(int x, int y)
    {
    }
};

class DoubleTapListener
{
public:
    virtual ~DoubleTapListener(){};
    virtual void OnSingleTapConfirmed(MoveEvent* moveEvent) = 0;
    virtual bool OnDoubleTap(MoveEvent* moveEvent) = 0;
    virtual bool OnDoubleTapEvent(MoveEvent* moveEvent) = 0;
};

class GestureDetector: public EventListener
{
public:
    GestureDetector();
    virtual ~GestureDetector();
    bool OnTouchEvent(MoveEvent* moveEvent);
    void SetListener(GestureListener* listener)
    {
        m_listener = listener;
    }
    void SetDoubleTapListener(DoubleTapListener* doubleTapListener)
    {
        m_doubleTapListener = doubleTapListener;
    }
    void EnableLongPress(bool enable)
    {
        m_isLongPressEnabled = enable;
    }

    void EnableHolding(bool enable)
    {
        m_isHoldingEnabled = enable;
    }

    void SetHoldingTimeOut(int millisecond)
    {
        m_holdingTimeout = millisecond;
    }

    void ResetHoldingTimeOut();
private:
    FlingDirection CalcFlingDirection(const MoveEvent* moveEvent1, const MoveEvent* moveEvent2);
    bool IsConsideredDoubleTap(MoveEvent* firstDown, MoveEvent* firstUp, MoveEvent* secondDown);
    void ResetMoveEvents();
    void Cancel();
    DISALLOW_COPY_AND_ASSIGN(GestureDetector);

    DECLARE_TIMER_THREAD_FUNC(OnSingleTapConfirmed);
    DECLARE_TIMER_THREAD_FUNC(OnLongPress);
    DECLARE_TIMER_THREAD_FUNC(OnHolding);
    int m_touchSlotSquare;
    int m_doubleTapTouchSlotSquare; 
    int m_doubleTapSlotSquare;
    int m_minFlingVelocity;
    int m_maxFlingVelocity;
    int m_longPressTimeout;
    int m_tapTimeout;
    int m_doubleTapTimeout;
    bool m_stillDown;
    bool m_inLongPress;
    bool m_alwaysInTapRegion;
    bool m_alwaysInBiggerTapRegion;
    MoveEvent* m_currentDownEvent;
    MoveEvent* m_prevUpEvent;
    bool m_isDoubleTapping;
    int m_lastX;
    int m_lastY;
    bool m_isLongPressEnabled;
    bool m_ignoreMultiTouch;
    GestureListener* m_listener;
    DoubleTapListener* m_doubleTapListener;
    int m_minFlingDistanceX;
    int m_minFlingDistanceY;
    bool m_isHoldingEnabled;
    int m_holdingX;
    int m_holdingY;
    dkUpTime m_holdingStartTime;
    int m_holdingTimeout;


};
#endif
