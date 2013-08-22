#ifndef __GUI_UICHECKBUTTON_H__
#define __GUI_UICHECKBUTTON_H__
#include "GUI/UITouchComplexButton.h"

class UICheckButton: public UITouchComplexButton
{
public:
    UICheckButton(UIContainer* parent, DWORD id);
    UICheckButton();
    virtual ~UICheckButton();
    virtual const char* GetClasssName() const
    {
        return "UICheckButton";
    }
    void SetChecked(bool checked);
    bool IsChecked() const;
private:
    void UpdateIcon();
    SPtr<ITpImage> m_checkIcon;
    SPtr<ITpImage> m_uncheckIcon;
    bool m_checked;
};
#endif
