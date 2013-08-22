#ifndef __TOUCHAPPUI_UISYSTEMSETTINGPANEL3_H__
#define __TOUCHAPPUI_UISYSTEMSETTINGPANEL3_H__

#include "TouchAppUI/UISettingPanel.h"

class UIBoxSizer;
class UITouchComplexButton;

class UISystemSettingPanel3: public UISettingPanel
{
public:
    UISystemSettingPanel3();
    virtual ~UISystemSettingPanel3();
    virtual bool OnChildClick(UIWindow* child);
private:
    void Init();
    void OnUninstallClick();
    void OnCopyrightClick();
};
#endif
