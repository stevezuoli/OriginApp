#ifndef __GUI_GUICONFIGURATION_H__
#define __GUI_GUICONFIGURATION_H__

#include "KernelDef.h"
#include "drivers/DeviceInfo.h"

class GUIConfiguration
{
    GUIConfiguration();
public:
    static const GUIConfiguration* GetInstance();

    int GetTouchSlot() const
    {
        return m_touchSlot;
    }

    int GetClickSlot() const
    {
        if (DeviceInfo::IsKPW())
        {
            return 30;
        }
        else
        {
            return 20;
        }
    }

    int GetClickSlotSquare() const
    {
        return GetClickSlot() * GetClickSlot();
    }

    int GetLongClickSlot() const
    {
        if (DeviceInfo::IsKPW())
        {
            return 30;
        }
        else
        {
            return 20;
        }
    }

    int GetTapSlot() const
    {
        return GetClickSlot();
    }

    int GetTapSlotSquare() const
    {
        return GetTapSlot() * GetTapSlot();
    }

    int GetDoubleTapSlot() const
    {
        return GetClickSlot();
    }

    int GetDoubleTapSlotSquare() const
    {
        return GetDoubleTapSlot() * GetDoubleTapSlot();
    }

    int GetMinFlingVelocity() const
    {
        return 1;
    }

    int GetMaxFlingVelocity() const
    {
        return 1000;
    }

    int GetLongPressTimeout() const
    {
        return 500;
    }

    int GetHoldingTimeout() const
    {
        return 750;
    }
    int GetTapTimeout() const
    {
        return 180;
    }

    int GetDoubleTapTimeout() const
    {
        return 300;
    }

    int GetMinFlingDistanceX() const
    {
        if (DeviceInfo::IsKPW())
        {
            return 64;
        }
        else
        {
            return 50;
        }
    }

    int GetMinFlingDistanceY() const
    {
        if (DeviceInfo::IsKPW())
        {
            return 64;
        }
        else
        {
            return 50;
        }
    }
private:
    DISALLOW_COPY_AND_ASSIGN(GUIConfiguration);
    int m_touchSlot;
};
#endif
