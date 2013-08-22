#ifndef __GUI_UISEPERATOR_H__
#define __GUI_UISEPERATOR_H__

#include "GUI/UIWindow.h"

class UISeperator: public UIWindow
{
public:
    UISeperator();
    enum SeperatorDirection
    {
        SD_VERT,
        SD_HORZ
    };
    virtual const char* GetClassName() const
    {
        return "UISeperator";
    }
    virtual HRESULT Draw(DK_IMAGE drawingImg);   
    void SetDirection(SeperatorDirection direction)
    {
        m_direction = direction;
    }
    void SetThickness(int thickness)
    {
        if (thickness > 0)
        {
            m_thickness = thickness;
        }
    }
    virtual dkSize GetMinSize() const;
private:
    SeperatorDirection m_direction;
    int m_thickness;
};
#endif
