#ifndef __TOUCHAPPUI_UISYSTEMSETTINGPANEL1_H__
#define __TOUCHAPPUI_UISYSTEMSETTINGPANEL1_H__

#include "TouchAppUI/UISettingPanel.h"

class UIBoxSizer;
class UITouchComplexButton;

class UISystemSettingPanel1: public UISettingPanel
{
public:
    UISystemSettingPanel1();
    virtual ~UISystemSettingPanel1();
    virtual bool OnChildClick(UIWindow* child);
    virtual void InitStatus();
private:
    void OnRestartClick();
    void OnCheckUpdateClick();
    void OnSwitchToKindleClick();
    void OnWifiSettingClick();
    void OnWifiTransferClick();
    void OnBatteryRatioModeClick();
    void Init();
    void UpdateRatioButtonIcon();
    std::string GetWifiItemText() const;
    UITouchComplexButton* m_batteryRatioButton;
    UITextSingleLine* m_wifiStatusLabel;
};
#endif
