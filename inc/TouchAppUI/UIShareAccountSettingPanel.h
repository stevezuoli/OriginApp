/*
 * =====================================================================================
 *       Filename:  UIShareAccountSettingPanel.h
 *    Description:  setting page of account shared
 *
 *        Version:  1.0
 *        Created:  01/09/2013 11:27:34 AM
 * =====================================================================================
 */

#ifndef __TOUCHAPPUI_UISHAREACCOUNTSETTINGPANEL_H__
#define __TOUCHAPPUI_UISHAREACCOUNTSETTINGPANEL_H__

#include "TouchAppUI/UISettingPanel.h"

class UISizer;

class UIShareAccountSettingPanel: public UISettingPanel
{
public:
    UIShareAccountSettingPanel();
    virtual ~UIShareAccountSettingPanel();
    virtual const char* GetClassName() const
    {
        return "UIShareAccountSettingPanel";
    }
    void UpdateData();
private:
    void Init();
    UISizer* m_bindSizer;
    UISizer* m_userNameSizer;
    UISizer* m_unbindSizer;
};
#endif//__TOUCHAPPUI_UISHAREACCOUNTSETTINGPANEL_H__

