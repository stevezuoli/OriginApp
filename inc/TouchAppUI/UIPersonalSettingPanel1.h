#ifndef __TOUCHAPPUI_UIPERSONALSETTINGPANEL1_H__
#define __TOUCHAPPUI_UIPERSONALSETTINGPANEL1_H__

#include "TouchAppUI/UISettingPanel.h"
#include "TouchAppUI/UIPersonalSettingPanel1Login.h"
#include "TouchAppUI/UIPersonalSettingPanel1Logout.h"
#include "TouchAppUI/UIShareAccountSettingPanel.h"

class UIBoxSizer;
class UITouchComplexButton;

class UIPersonalSettingPanel1: public UISettingPanel
{
public:
    UIPersonalSettingPanel1();
    virtual ~UIPersonalSettingPanel1();
    virtual const char* GetClassName() const
    {
        return "UIPersonalSettingPanel1";
    }
    virtual bool OnChildClick(UIWindow* child);

    void InitStatus();
    bool OnAccountLogInOut(const EventArgs& args);
	bool OnXiaomiUserCardReceived(const EventArgs& args);

private:
    void UpdateData();
    void Init();
    void OnLogoutClick();
    void OnLoginClick();
    void OnModifyPersonalInfoClick();
    void OnPushMailBoxClick();
    void OnRegisterClick();

    // Weibo
    void OnBindWithSinaWeiboClick();
    void OnUnbindWithSinaWeiboClick();
    void OnSwitchWithSinaWeiboClick();

    // Xiaomi Account Migration
	void OnAccountMigrateFeedBackClick();

private:
    UIPersonalSettingPanel1Login m_loginPanel;
    UIPersonalSettingPanel1Logout m_logoutPanel;
    UIShareAccountSettingPanel m_shareAccountPanel;
    UISettingPanel* m_curPanel;
};
#endif
