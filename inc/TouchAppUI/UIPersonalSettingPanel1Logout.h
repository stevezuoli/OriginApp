#ifndef __TOUCHAPPUI_UIPERSONALSETTINGPANEL1LOGOUT_H__
#define __TOUCHAPPUI_UIPERSONALSETTINGPANEL1LOGOUT_H__

#include "TouchAppUI/UISettingPanel.h"

class UIBoxSizer;
class UITouchComplexButton;

class UIPersonalSettingPanel1Logout: public UISettingPanel
{
public:
    UIPersonalSettingPanel1Logout();
    virtual ~UIPersonalSettingPanel1Logout();
    virtual const char* GetClassName() const
    {
        return "UIPersonalSettingPanel1Logout";
    }
	void UpdateData();
private:
    void Init();

private:
	UITouchComplexButton* m_btnLogin;
	UITouchComplexButton* m_btnRegister;
};
#endif
