/*
 * =====================================================================================
 *       Filename:  UIExportAccountSettingPanel.h
 *    Description:  setting page of account for exporting
 *
 *        Version:  1.0
 *        Created:  06/30/2013
 * =====================================================================================
 */

#ifndef __TOUCHAPPUI_UIEXPORTACCOUNTSETTINGPANEL_H__
#define __TOUCHAPPUI_UIEXPORTACCOUNTSETTINGPANEL_H__

#include "TouchAppUI/UISettingPanel.h"

class UISizer;

class UIPersonalSettingPanel2: public UISettingPanel
{
public:
    UIPersonalSettingPanel2();
    virtual ~UIPersonalSettingPanel2();
    virtual const char* GetClassName() const
    {
        return "UIPersonalSettingPanel2";
    }
	virtual bool OnChildClick(UIWindow* child);
	void InitStatus();
    void UpdateData();

	void OnBindEvernoteClick();
	void OnUnbindEvernoteClick();
	bool OnEvernoteAuthFinished(const EventArgs& args);
	bool OnEvernoteGetUserFinished(const EventArgs& args);

private:
    void Init();
    UISizer* m_bindSizer;
    UISizer* m_userNameSizer;
    UISizer* m_unbindSizer;
    UITextSingleLine* m_userName;
};
#endif//__TOUCHAPPUI_UIEXPORTACCOUNTSETTINGPANEL_H__

