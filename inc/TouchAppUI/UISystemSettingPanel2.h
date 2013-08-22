#ifndef __TOUCHAPPUI_UISYSTEMSETTINGPANEL2_H__
#define __TOUCHAPPUI_UISYSTEMSETTINGPANEL2_H__

#include "TouchAppUI/UISettingPanel.h"
#include "GUI/UIRadioButtonGroup.h"

class UIBoxSizer;
class UITouchComplexButton;

class UISystemSettingPanel2: public UISettingPanel
{
public:
    UISystemSettingPanel2();
    virtual ~UISystemSettingPanel2();
    virtual bool OnChildClick(UIWindow* child);
private:
    void Init();
    void OnInputMethodClick();
    void OnDateClick();
    void OnTimeClick();
    void OnTimeZoneClick();
    void OnLanguageClick();
    void UpdateDateAndTime();
    void OnTimeFormatClick(int mode);

    UITouchComplexButton* m_languageButton;
    UITouchComplexButton* m_inputButton;
    UITouchComplexButton* m_yearButton;
    UITouchComplexButton* m_monthButton;
    UITouchComplexButton* m_dayButton;
    UITouchComplexButton* m_hourButton;
    UITouchComplexButton* m_minuteButton;
    UITouchComplexButton* m_ampmButton;
    UITouchComplexButton* m_zoneEastButton;
    UITouchComplexButton* m_zoneFullButton;
    UITouchComplexButton* m_zoneHalfButton;
    UIRadioButtonGroup* m_timeFormatGroup;
};
#endif
