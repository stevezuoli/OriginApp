////////////////////////////////////////////////////////////////////////
// UIShortcutDlg.h
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UISHORTCUTBAR_H__
#define __UISHORTCUTBAR_H__

#include "GUI/UIContainer.h"
#include "GUI/UITouchComplexButton.h"
#include "singleton.h"
#include "CommonUI/UITitleBar.h"
#include "GUI/UIButtonGroup.h"

// #define UISHORTCUTBAR_TOUCH_MAX_Y   100
// #define UISHORTCUT_HEIGHT           108

class UIShortcutBar : public UIButtonGroup
{
    SINGLETON_H(UIShortcutBar)
public:
    UIShortcutBar();
    ~UIShortcutBar();

    virtual LPCSTR GetClassName() const
    {
        return ("UIShortcutBar");
    }
    virtual bool OnShadowTouch(GESTURE_EVENT _ge, int _x, int _y);
    virtual void OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam);
	void UpdateButtonStatus();

private:
	void OnUserLogClicked();
    void UserLogin();
	void UserLogOut();
	void OnWifiPowerClicked();
    bool OnWifiMessageEvent(const EventArgs& args);
    bool OnAccountMessageEvent(const EventArgs& args);
    bool OnWebBrowserClicked();

protected:
    UITouchComplexButton m_btnWLAN;
    UITouchComplexButton m_btnWifiTransfer;
    UITouchComplexButton m_btnUserLogin;
    UITouchComplexButton m_btnWebBrowser;
};

#endif //__UISHORTCUTBAR_H__
