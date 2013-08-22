#ifndef __TOUCHAPPUI_UIPERSONALSETTINGPANEL1LOGIN_H__
#define __TOUCHAPPUI_UIPERSONALSETTINGPANEL1LOGIN_H__

#include "TouchAppUI/UISettingPanel.h"

class UIBoxSizer;
class UITouchComplexButton;

class UIPersonalSettingPanel1Login: public UISettingPanel
{
public:
    UIPersonalSettingPanel1Login();
    virtual ~UIPersonalSettingPanel1Login();
    virtual const char* GetClassName() const
    {
        return "UIPersonalSettingPanel1Login";
    }
    void UpdateData();
private:
    void Init();
    UITextSingleLine* m_account;
    UITextSingleLine* m_alias;
    UITextSingleLine* m_pushMailBox;
	UITouchComplexButton* m_btnModifyInfo;
};
#endif
