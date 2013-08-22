#ifndef __GUI_UIRADIONBUTTONGROUP_H__
#define __GUI_UIRADIONBUTTONGROUP_H__

#include "GUI/UIContainer.h"
#include "GUI/UITouchComplexButton.h"

class UIRadioButtonGroup: public UIContainer
{
public:
    UIRadioButtonGroup();
    virtual ~UIRadioButtonGroup();
    virtual LPCSTR GetClassName() const
    {
        return "UIRadioButtonGroup";
    }

    void AddButton(UITouchComplexButton* button);
    void AddButton(int cmdId, const char* pText, int fontsize, bool isSelected = false);
    virtual void MoveWindow(int left, int top, int width, int height);
    virtual dkSize GetMinSize() const;
    virtual void SetFocus(BOOL bIsFocus);
    void SelectChild(size_t index);

private:
    std::vector<UITouchComplexButton*> m_buttons;
    std::vector<UIWindow*> m_dynamicCreatedWindows;
};
#endif
