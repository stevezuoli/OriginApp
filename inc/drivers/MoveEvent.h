#ifndef __DRIVERS_MOVEEVENT_H__
#define __DRIVERS_MOVEEVENT_H__

#include "Common/Defs.h"

class MoveEvent
{

public:
    MoveEvent()
        : m_action(0)
        , m_actionIndex(0)
        , m_pointerCount(0)
        , m_downTime(0)
        , m_eventTime(0)
    {
        for (int i = 0; i < MAX_TOUCH_POINT; ++i)
        {
            m_x[i] = -1;
            m_y[i] = -1;
            m_pointerId[i] = -1;
        }

    }
    /**
     * An invalid pointer id.
     *
     * This value (-1) can be used as a placeholder to indicate that a pointer id
     * has not been assigned or is not available.  It cannot appear as
     * a pointer id inside a {@link MotionEvent}.
     */
    static const int INVALID_POINTER_ID = -1;

    static const int MAX_TOUCH_POINT = 2;

    /**
     * Bit mask of the parts of the action code that are the action itself.
     */
    static const int ACTION_MASK             = 0xff;
    
    /**
     * Constant for {@link #getActionMasked}: A pressed gesture has started, the
     * motion contains the initial starting location.
     * <p>
     * This is also a good time to check the button state to distinguish
     * secondary and tertiary button clicks and handle them appropriately.
     * Use {@link #getButtonState} to retrieve the button state.
     * </p>
     */
    static const int ACTION_DOWN             = 0;
    
    /**
     * Constant for {@link #getActionMasked}: A pressed gesture has finished, the
     * motion contains the const release location as well as any intermediate
     * points since the last down or move event.
     */
    static const int ACTION_UP               = 1;
    
    /**
     * Constant for {@link #getActionMasked}: A change has happened during a
     * press gesture (between {@link #ACTION_DOWN} and {@link #ACTION_UP}).
     * The motion contains the most recent point, as well as any intermediate
     * points since the last down or move event.
     */
    static const int ACTION_MOVE             = 2;
    
    /**
     * Constant for {@link #getActionMasked}: The current gesture has been aborted.
     * You will not receive any more points in it.  You should treat this as
     * an up event, but not perform any action that you normally would.
     */
    static const int ACTION_CANCEL           = 3;
    
    /**
     * Constant for {@link #getActionMasked}: A movement has happened outside of the
     * normal bounds of the UI element.  This does not provide a full gesture,
     * but only the initial location of the movement/touch.
     */
    static const int ACTION_OUTSIDE          = 4;

    /**
     * Constant for {@link #getActionMasked}: A non-primary pointer has gone down.
     * <p>
     * Use {@link #getActionIndex} to retrieve the index of the pointer that changed.
     * </p><p>
     * The index is encoded in the {@link #ACTION_POINTER_INDEX_MASK} bits of the
     * unmasked action returned by {@link #getAction}.
     * </p>
     */
    static const int ACTION_POINTER_DOWN     = 5;
    
    /**
     * Constant for {@link #getActionMasked}: A non-primary pointer has gone up.
     * <p>
     * Use {@link #getActionIndex} to retrieve the index of the pointer that changed.
     * </p><p>
     * The index is encoded in the {@link #ACTION_POINTER_INDEX_MASK} bits of the
     * unmasked action returned by {@link #getAction}.
     * </p>
     */
    static const int ACTION_POINTER_UP       = 6;
    static const int ACTION_LONG_TAP       = 7;

    MoveEvent* Clone() const
    {
        return new MoveEvent(*this);
    }

    int GetAction() const
    {
        return m_action;
    }

    int GetActionMasked() const
    {
        return GetAction() & ACTION_MASK;
    }

    int GetActionIndex() const
    {
        return m_actionIndex;
    }

    void SetAction(int action)
    {
        m_action = action;
    }
    
    int GetPointerCount() const
    {
        return m_pointerCount;
    }

    // in logical screen coordinate(like 600x800, not 1500x2000)
    int GetX() const
    {
        return GetX(0);
    }
    int GetY() const
    {
        return GetY(0);
    }
    int GetX(int index) const
    {
        return m_x[index];
    }
    int GetY(int index) const
    {
        return m_y[index];
    }
    void OffsetLocation(int offsetX, int offsetY)
    {
        m_x[0] += offsetX;
        m_y[0] += offsetY;
    }
    int GetPointerId(int index) const
    {
        return m_pointerId[index];
    }
    int GetPointerIdBits() const
    {
        return 0;
    }
    dkUpTime GetDownTime() const
    {
        return m_downTime;
    }
    dkUpTime GetEventTime() const
    {
        return m_eventTime;
    }
    int FindPointerIndex(int pointerId) const
    {
        for (int i = 0; i < m_pointerCount; ++i)
        {
            if (pointerId == m_pointerId[i])
            {
                return i;
            }
        }
        return -1;
    }

private:
    friend class TouchDevice;
    friend class UIGestureRecognizer;
    int m_x[MAX_TOUCH_POINT];
    int m_y[MAX_TOUCH_POINT];
    int m_pointerId[MAX_TOUCH_POINT];
    int m_action;
    int m_actionIndex;
    int m_pointerCount;
    dkUpTime m_downTime;
    dkUpTime m_eventTime;
};
#endif
