#include "TouchAppUI/UISystemSettingPanel2.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "Common/SystemSetting_DK.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITouchComplexButton.h"
#include "TouchAppUI/UIButtonDlg.h"
#include "TouchAppUI/UIDateSettingDlg.h"
#include "CommonUI/CPageNavigator.h"

using namespace WindowsMetrics;

UISystemSettingPanel2::UISystemSettingPanel2()
{
    Init();
}

UISystemSettingPanel2::~UISystemSettingPanel2()
{
}

void UISystemSettingPanel2::Init()
{
    //SystemSettingInfo* systemSettingInfo = SystemSettingInfo::GetInstance();
    AddSectionTitle(StringManager::GetPCSTRById(LANGUAGE_INPUT));
    m_titleUpSpacing -= m_itemVertSpacing;
    InitLanguages();
    AddArrowButton(StringManager::GetPCSTRById(LANGUAGE_OPTION),
            SBI_LanguageSetting,
            m_languageNames[m_selectedLanguage].c_str());
    m_languageButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();
#if 0
    m_languageButton->SetEnable(false);
    m_languageButton->SetFontColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
#endif
    InitIME();
    AddArrowButton(StringManager::GetPCSTRById(INPUT_METHOD),
            SBI_InputMethodSetting,
            m_imeNames[m_selectedIMEIndex].c_str());
    m_inputButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();

    AddSectionTitle(StringManager::GetPCSTRById(DATE_AND_TIME));
    UIBoxSizer* sizer = AddPushButton(StringManager::GetPCSTRById(TOUCH_SET_DATE),
            SBI_YearSetting,
            "");
    m_yearButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();
    const int dateTimeSpacing = GetWindowMetrics(UISettingPanelDateTimeHorzSpacingIndex);
    sizer->AddSpacer(dateTimeSpacing);
    sizer->Add(CreateTextSingleLine("-", true), UISizerFlags().Center());
    sizer->AddSpacer(dateTimeSpacing);
    sizer->Add(CreateButton(SBI_MonthSetting, "", true), UISizerFlags().Center());
    m_monthButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();
    sizer->AddSpacer(dateTimeSpacing);
    sizer->Add(CreateTextSingleLine("-", true), UISizerFlags().Center());
    sizer->AddSpacer(dateTimeSpacing);
    sizer->Add(CreateButton(SBI_DaySetting, "", true), UISizerFlags().Center());
    m_dayButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();


    sizer = AddPushButton(StringManager::GetPCSTRById(TOUCH_SET_TIME),
            SBI_HourSetting,
            "");
    m_hourButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();
    sizer->AddSpacer(dateTimeSpacing);
    sizer->Add(CreateTextSingleLine("-", true), UISizerFlags().Center());
    sizer->AddSpacer(dateTimeSpacing);
    sizer->Add(CreateButton(SBI_MinuteSetting, "", true), UISizerFlags().Center());
    m_minuteButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();
    sizer->AddSpacer(dateTimeSpacing*2+m_itemHorzSpacing);
    sizer->Add(CreateButton(SBI_AMPMSetting, "", true), UISizerFlags().Center());
    m_ampmButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();

    int timeFormatIds[] = {SBI_TimeFormat12, SBI_TimeFormat24};
    const char* timeFormatTexts[] = {StringManager::GetPCSTRById(TWELVE_HOUR), StringManager::GetPCSTRById(TWENTYFOUR_HOUR)};
    UISizer* timeFormatSizer = AddRadioButtonGroups(StringManager::GetPCSTRById(TIME_FORMAT), 0, 2, timeFormatIds, timeFormatTexts, GetTimeSystem());
    m_timeFormatGroup = (UIRadioButtonGroup*)timeFormatSizer->GetChildren().front()->GetWindow();
    
    sizer = AddPushButton(StringManager::GetPCSTRById(TIME_ZONE),
        SBI_TimeZoneSetting,
        "");
    m_zoneEastButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();
    sizer->AddSpacer(dateTimeSpacing);
    sizer->Add(CreateTextSingleLine("-", true), UISizerFlags().Center());
    sizer->AddSpacer(dateTimeSpacing);
    sizer->Add(CreateButton(SBI_TimeZoneSetting, "", true), UISizerFlags().Center());
    m_zoneFullButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();
    sizer->AddSpacer(dateTimeSpacing);
    sizer->Add(CreateTextSingleLine("-", true), UISizerFlags().Center());
    sizer->AddSpacer(dateTimeSpacing);
    sizer->Add(CreateButton(SBI_TimeZoneSetting, "", true), UISizerFlags().Center());
    m_zoneHalfButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();

    UpdateDateAndTime();
}

bool UISystemSettingPanel2::OnChildClick(UIWindow* child)
{
    switch (child->GetId())
    {
        case SBI_LanguageSetting:
            OnLanguageClick();
            break;
        case SBI_InputMethodSetting:
            OnInputMethodClick();
            break;
        case SBI_YearSetting:
        case SBI_MonthSetting:
        case SBI_DaySetting:
            OnDateClick();
            break;
        case SBI_HourSetting:
        case SBI_MinuteSetting:
        case SBI_AMPMSetting:
            OnTimeClick();
            break;
        case SBI_TimeFormat12:
        case SBI_TimeFormat24:
            OnTimeFormatClick(child->GetId() - SBI_TimeFormat12);
            break;
        case SBI_TimeZoneSetting:
            OnTimeZoneClick();
            break;
        default:
            break;
    }
    return true;
}

void UISystemSettingPanel2::UpdateDateAndTime()
{
    time_t t;
    struct tm lt;
    time(&t);
    localtime_r(&t, &lt);
    char buf[20];
    snprintf(buf, DK_DIM(buf), "%d", lt.tm_year + 1900);
    m_yearButton->SetText(buf);
    snprintf(buf, DK_DIM(buf), "%02d", lt.tm_mon + 1);
    m_monthButton->SetText(buf);
    snprintf(buf, DK_DIM(buf), "%02d", lt.tm_mday);
    m_dayButton->SetText(buf);
    int hour = lt.tm_hour;
    char amOrPm[] = "AM";
    if (hour >= 12)
    {
        hour -= 12;
        amOrPm[0] = 'P';
    }
    if (hour == 0)
    {
        hour = 12;
    }
    snprintf(buf, DK_DIM(buf), "%02d", hour);
    m_hourButton->SetText(buf);
    snprintf(buf, DK_DIM(buf), "%02d", lt.tm_min);
    m_minuteButton->SetText(buf);
    m_ampmButton->SetText(amOrPm);

    m_zoneEastButton->SetText(StringManager::GetPCSTRById((GetTimeZoneEast() > 0) ? TIME_ZONE_EAST : TIME_ZONE_WEST));
    snprintf(buf, DK_DIM(buf), "%02d", GetTimeZoneFull());
    m_zoneFullButton->SetText(buf);
    snprintf(buf, DK_DIM(buf), "%02d", GetTimeZoneHalf());
    m_zoneHalfButton->SetText(buf);
}

void UISystemSettingPanel2::OnInputMethodClick()
{
    UIButtonDlg dlg(this);
    dlg.SetData(m_imeNames, m_selectedIMEIndex);
    int top = dlg.CalculateTop(m_inputButton);
    dlg.MoveWindow(m_inputButton->GetX(), top, GetWindowMetrics(InputMethodMenuWidthIndex), dlg.GetTotalHeight());
    dlg.DoModal();
    int newSelect = dlg.GetSelectedButtonIndex();
    if (newSelect >= 0 && newSelect < (int)m_imeNames.size())
    {
        m_inputButton->SetText(m_imeNames[newSelect].c_str());
        SetSelectedIME(newSelect);
    }
}

void UISystemSettingPanel2::OnDateClick()
{
    UIDateOrTimeSettingDlg dlgDateSetting(this);
    dlgDateSetting.DoModal();
    UpdateDateAndTime();
}

void UISystemSettingPanel2::OnLanguageClick()
{
    UIButtonDlg dlg(this);
    dlg.SetData(m_languageNames, m_selectedLanguage);
    int top = dlg.CalculateTop(m_languageButton);
    dlg.MoveWindow(m_languageButton->GetX(), top, m_fontMenuWidth, dlg.GetTotalHeight());
    dlg.DoModal();
    int newSelect = dlg.GetSelectedButtonIndex();
    if (newSelect >= 0 && newSelect < (int)m_languageNames.size())
    {
        m_languageButton->SetText(m_languageNames[newSelect].c_str());
        SelectLanguage(newSelect);
        CPageNavigator::BackToHome();
    }
}

void UISystemSettingPanel2::OnTimeClick()
{
    UIDateOrTimeSettingDlg dlgTimeSetting(this, UIDateOrTimeSettingDlg::UITIMESETTING);
    dlgTimeSetting.DoModal();
    UpdateDateAndTime();
}

void UISystemSettingPanel2::OnTimeZoneClick()
{
    UIDateOrTimeSettingDlg dlgTimeSetting(this, UIDateOrTimeSettingDlg::UITIMEZONESETTING);
    dlgTimeSetting.DoModal();
    UpdateDateAndTime();
}

void UISystemSettingPanel2::OnTimeFormatClick(int mode)
{
    m_timeFormatGroup->SelectChild(mode);
    SetTimeSystem(mode);
    UpdateDateAndTime();
    //TODO(jugh):如果不加Repaint(), titleBar的时间不会发生变化
    Repaint();
}
