#ifndef __TOUCHAPPUI_UITOUCHMENU_H__
#define __TOUCHAPPUI_UITOUCHMENU_H__

#include "GUI/UIDialog.h"

class UITouchComplexButton;
class UISeperator;

class UITouchMenu: public UIDialog
{
public:
    UITouchMenu(UIContainer* parent);
    virtual ~UITouchMenu();
    const char* GetClassName() const
    {
        return "UITouchMenu";
    }
    bool AppendButton(DWORD id, const char* text, int fontSize);
    size_t GetButtonCount() const;
    void SetCheckedButton(int checked);
    virtual bool OnHookTouch(MoveEvent* moveEvent);
    int CalculateTop(const UIButton* button);
    virtual bool OnChildClick(UIWindow* child);
    dkSize GetMenuSize();
    HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual void OnCommand(DWORD cmdId, UIWindow* sender, DWORD param);
    int GetCommandId() const;
private:
    UITouchComplexButton* GetButton(int index);
    void InitUI();
    DISALLOW_COPY_AND_ASSIGN(UITouchMenu);
    std::vector<UITouchComplexButton*> m_btns;
    std::vector<UISeperator*> m_seps;
    SPtr<ITpImage> m_checkedImage;
    int m_checkedIndex;
    UISizer* m_buttonSizer;
    int m_commandId;
};
#endif
