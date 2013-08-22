#include "TouchAppUI/UIDateSettingDlg.h"
#include "CommonUI/UIUtility.h"
#include "GUI/CTpGraphics.h"
#include "I18n/StringManager.h"
#include "interface.h"
#include "Framework/CDisplay.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;

UIDateOrTimeSettingDlg::UIDateOrTimeSettingDlg(UIContainer* _pParent, int settingType)
    : UIDialog(_pParent)
    , m_settingType(settingType)
{
    Init();
    const int width = GetWindowMetrics(UIDateSettingDlgWidthIndex);
    const int height = GetWindowMetrics(UIDateSettingDlgHeightIndex);
    MakeCenter(width, height);
    //MoveWindow(0, 0, 0, 0);
}

UIDateOrTimeSettingDlg::~UIDateOrTimeSettingDlg()
{
}

void UIDateOrTimeSettingDlg::Init()
{
    UpdateContent();

    const int btnHeight = GetWindowMetrics(UINormalBtnHeightIndex);
    const int fontsize20 = GetWindowFontSize(FontSize20Index);
    const int fontsize24 = GetWindowFontSize(FontSize24Index);

    m_btnSave.Initialize(IDOK, StringManager::GetPCSTRById(TOUCH_ACTION_SAVE), fontsize20);
    m_btnCancel.Initialize(IDCANCEL, StringManager::GetPCSTRById(TOUCH_ACTION_CANCEL), fontsize20);
    m_btnSave.SetMinHeight(btnHeight);
    m_btnCancel.SetMinHeight(btnHeight);

    m_txtTitle.SetFontSize(fontsize24);
    m_txtTitle.SetBackColor(ColorManager::GetColor(COLOR_TITLE_BACKGROUND));

    AppendChild(&m_btnSave);
    AppendChild(&m_btnCancel);
    AppendChild(&m_txtTitle);
}

void UIDateOrTimeSettingDlg::UpdateContent()
{
    int cmdUpID[INDEX_END] = {0};
    int cmdDownID[INDEX_END] = {0};
    if (UIDATESETTING == m_settingType)
    {
        cmdUpID[INDEX_LEFT] = ID_BTN_TOUCH_YEAR_UP;
        cmdUpID[INDEX_MIDDLE] = ID_BTN_TOUCH_MONTH_UP;
        cmdUpID[INDEX_RIGHT] = ID_BTN_TOUCH_DAY_UP;
        cmdDownID[INDEX_LEFT] = ID_BTN_TOUCH_YEAR_DOWN;
        cmdDownID[INDEX_MIDDLE] = ID_BTN_TOUCH_MONTH_DOWN;
        cmdDownID[INDEX_RIGHT] = ID_BTN_TOUCH_DAY_DOWN;
        m_txtTitle.SetText(StringManager::GetPCSTRById(TOUCH_DATESETTING));
        GetDate();
    }
    else if (UITIMESETTING == m_settingType)
    {
        cmdUpID[INDEX_LEFT] = ID_BTN_TOUCH_HOUR_UP;
        cmdUpID[INDEX_MIDDLE] = ID_BTN_TOUCH_MINUTE_UP;
        cmdUpID[INDEX_RIGHT] = ID_BTN_TOUCH_AMORPM_UP;
        cmdDownID[INDEX_LEFT] = ID_BTN_TOUCH_HOUR_DOWN;
        cmdDownID[INDEX_MIDDLE] = ID_BTN_TOUCH_MINUTE_DOWN;
        cmdDownID[INDEX_RIGHT] = ID_BTN_TOUCH_AMORPM_DOWN;
        m_txtTitle.SetText(StringManager::GetPCSTRById(TOUCH_TIMESETTING));
        GetTime();
    }
    else
    {
        cmdUpID[INDEX_LEFT] = ID_BTN_TIMEZONE_HEMISPHERE_UP;
        cmdUpID[INDEX_MIDDLE] = ID_BTN_TIMEZONE_HOUR_UP;
        cmdUpID[INDEX_RIGHT] = ID_BTN_TIMEZONE_MINUTE_UP;
        cmdDownID[INDEX_LEFT] = ID_BTN_TIMEZONE_HEMISPHERE_DOWN;
        cmdDownID[INDEX_MIDDLE] = ID_BTN_TIMEZONE_HOUR_DOWN;
        cmdDownID[INDEX_RIGHT] = ID_BTN_TIMEZONE_MINUTE_DOWN;
        m_txtTitle.SetText(StringManager::GetPCSTRById(TIME_ZONE));
        GetTimeZone();
    }

    const int btnHeight = GetWindowMetrics(UINormalBtnHeightIndex);
    const int fontsize20 = GetWindowFontSize(FontSize20Index);
    int indexArray[INDEX_END] = {INDEX_LEFT, INDEX_MIDDLE, INDEX_RIGHT};
    for (int i = 0; i < INDEX_END; ++i)
    {
        const int& index = indexArray[i];

        UITouchComplexButton& btnUp = m_btnUp[index];
        btnUp.SetId(cmdUpID[index]);
        btnUp.SetIcon(ImageManager::GetImage(IMAGE_TOP_ARROW), UIButton::ICON_TOP);
        btnUp.ShowBorder(false);
        btnUp.SetMinHeight(btnHeight);
        AppendChild(&btnUp);

        UITouchComplexButton& btnDown = m_btnDown[index];
        btnDown.SetId(cmdDownID[index]);
        btnDown.SetIcon(ImageManager::GetImage(IMAGE_BOTTOM_ARROW), UIButton::ICON_TOP);
        btnDown.ShowBorder(false);
        btnDown.SetMinHeight(btnHeight);
        AppendChild(&btnDown);

        UITextSingleLine& txtContent = m_txtContent[index];
        txtContent.SetMinWidth(btnHeight << 1);
        txtContent.SetFontSize(fontsize20);
        txtContent.SetAlign(ALIGN_CENTER);
        AppendChild(&txtContent);
    }
}

void UIDateOrTimeSettingDlg::MoveWindow(int left, int top, int width, int height)
{
    if (!m_windowSizer)
    {
        UIWindow::MoveWindow(left, top, width, height);

        const int elementSpacing = GetWindowMetrics(UIElementLargeSpacingIndex);
        const int margin = GetWindowMetrics(UIHorizonMarginIndex);
        const int dateSettingRowNumber = 3;
        const int dateSettingLineNumber = 3;
        const int btnRowNumber = 2;
        const int btnLineNumber = 1;
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pDateSettingSizer = new UIGridSizer(dateSettingLineNumber, dateSettingRowNumber, elementSpacing, elementSpacing);
        UISizer* pBtnLstSizer = new UIGridSizer(btnLineNumber, btnRowNumber, elementSpacing, elementSpacing);
        if (pMainSizer && pDateSettingSizer && pBtnLstSizer)
        {
            for (int i = 0; i < INDEX_END; ++i)
            {
                UITouchComplexButton& btnUp = m_btnUp[i];
                pDateSettingSizer->Add(&btnUp, UISizerFlags().Expand());
            }

            for (int i = 0; i < INDEX_END; ++i)
            {
                UITextSingleLine& txtContent = m_txtContent[i];
                pDateSettingSizer->Add(&txtContent, UISizerFlags().Align(dkALIGN_CENTRE));
            }

            for (int i = 0; i < INDEX_END; ++i)
            {
                UITouchComplexButton& btnDown = m_btnDown[i];
                pDateSettingSizer->Add(&btnDown, UISizerFlags().Expand());
            }

            pBtnLstSizer->Add(&m_btnCancel, UISizerFlags().Expand());
            pBtnLstSizer->Add(&m_btnSave, UISizerFlags().Expand());

            pMainSizer->Add(&m_txtTitle, UISizerFlags().Expand().Border(dkALL, elementSpacing));
            pMainSizer->AddStretchSpacer();
            pMainSizer->Add(pDateSettingSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, margin));
            pMainSizer->Add(pBtnLstSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT | dkBOTTOM, margin));

            SetSizer(pMainSizer);
            Layout();
        }
        else
        {
            SafeDeletePointer(pMainSizer);
            SafeDeletePointer(pDateSettingSizer);
            SafeDeletePointer(pBtnLstSizer);
        }
    }
}

void UIDateOrTimeSettingDlg::LimitNumber(unsigned int& number, unsigned int min, unsigned int max)
{
    number = (number > max) ? min : ((number < min) ? max : number);
}

void UIDateOrTimeSettingDlg::UpdateYear(bool add)
{
    unsigned int& year = m_content[INDEX_LEFT];
    UITextSingleLine& txtYear = m_txtContent[INDEX_LEFT];
    year += add ? 1 : -1;
    char strTemp[128] = {0};
    sprintf(strTemp, "%04d", year);
    txtYear.SetText(strTemp);
    txtYear.Repaint();
}

void UIDateOrTimeSettingDlg::UpdateMonth(bool add)
{
    unsigned int& year = m_content[INDEX_LEFT];
    unsigned int& month = m_content[INDEX_MIDDLE];
    unsigned int& day = m_content[INDEX_RIGHT];

    UITextSingleLine& txtMonth = m_txtContent[INDEX_MIDDLE];
    UITextSingleLine& txtDay = m_txtContent[INDEX_RIGHT];

    month += add ? 1 : -1;
    LimitNumber(month, 1, 12);

    char strTemp[128] = {0};
    sprintf(strTemp, "%02d", month);
    txtMonth.SetText(strTemp);
    txtMonth.Repaint();

    int iDay = GetDaysOfMonth(year, month);
    if ((iDay > 0) && (day > (unsigned int)iDay))
    {
        day = (unsigned int)iDay;
        char strTemp[128] = {0};
        sprintf(strTemp, "%02d", day);
        txtDay.SetText(strTemp);
        txtDay.Repaint();
    }
}

void UIDateOrTimeSettingDlg::UpdateDay(bool add)
{
    unsigned int& year = m_content[INDEX_LEFT];
    unsigned int& month = m_content[INDEX_MIDDLE];
    unsigned int& day = m_content[INDEX_RIGHT];

    UITextSingleLine& txtDay = m_txtContent[INDEX_RIGHT];
    day += add ? 1 : -1;
    int iDay = GetDaysOfMonth(year, month);
    if (iDay > 0)
    {
        LimitNumber(day, 1, iDay);
        char strTemp[128] = {0};
        sprintf(strTemp, "%02d", day);
        txtDay.SetText(strTemp);
        txtDay.Repaint();
    }
}

void UIDateOrTimeSettingDlg::UpdateHour(bool add)
{
    unsigned int& hour = m_content[INDEX_LEFT];
    UITextSingleLine& txtHour = m_txtContent[INDEX_LEFT];
    hour += add ? 1 : -1;
    LimitNumber(hour, 1, 12);

    char strTemp[128] = {0};
    sprintf(strTemp, "%02d", hour);
    txtHour.SetText(strTemp);
    txtHour.Repaint();
}

void UIDateOrTimeSettingDlg::UpdateMinute(bool add)
{
    unsigned int& minute = m_content[INDEX_MIDDLE];
    UITextSingleLine& txtMinute = m_txtContent[INDEX_MIDDLE];
    minute += add ? 1 : -1;
    LimitNumber(minute, 1, 59);

    char strTemp[128] = {0};
    sprintf(strTemp, "%02d", minute);
    txtMinute.SetText(strTemp);
    txtMinute.Repaint();
}

void UIDateOrTimeSettingDlg::UpdateAMorPM(bool add)
{
    unsigned int& AMOrPM = m_content[INDEX_RIGHT];
    UITextSingleLine& txtAMOrPM = m_txtContent[INDEX_RIGHT];
    AMOrPM++;
    AMOrPM &= 0x1;

    txtAMOrPM.SetText(StringManager::GetPCSTRById(AMOrPM ? TOUCH_SYSTEMSETTING_TIME_AM : TOUCH_SYSTEMSETTING_TIME_PM));
    txtAMOrPM.Repaint();
}

void  UIDateOrTimeSettingDlg::UpdateTimeZoneEast(bool add)
{
    unsigned int& iValue = m_content[INDEX_LEFT];
    UITextSingleLine& txtValue = m_txtContent[INDEX_LEFT];
    iValue = -iValue;
    bool east = (((int)iValue) >= 0);
    txtValue.SetText(StringManager::GetPCSTRById(east ? TIME_ZONE_EAST : TIME_ZONE_WEST));
    txtValue.Repaint();
}

void  UIDateOrTimeSettingDlg::UpdateTimeZoneFull(bool add)
{
    unsigned int& iValue = m_content[INDEX_MIDDLE];
    UITextSingleLine& txtValue = m_txtContent[INDEX_MIDDLE];
    const int incr = add ? 1 : -1;
    iValue =  (iValue + 13 + incr) % 13;

    char strTemp[128] = {0};
    sprintf(strTemp, "%02d", iValue);
    txtValue.SetText(strTemp);
    txtValue.Repaint();
}

void  UIDateOrTimeSettingDlg::UpdateTimeZoneHalf(bool add)
{
    unsigned int& iValue = m_content[INDEX_RIGHT];
    UITextSingleLine& txtValue = m_txtContent[INDEX_RIGHT];
    iValue  = (iValue  + 30) %  60;
    // 仅支持整时区和半时区, 无需支持四分之一时区(15 45 时区)
    if(iValue != 0)
        iValue = 30;

    char strTemp[128] = {0};
    sprintf(strTemp, "%02d", iValue);
    txtValue.SetText(strTemp);
    txtValue.Repaint();
}

void UIDateOrTimeSettingDlg::Commit()
{
    SystemSettingInfo* pInfo = SystemSettingInfo::GetInstance();
    if (pInfo)
    {
        unsigned int& left = m_content[INDEX_LEFT];
        const unsigned int& middle = m_content[INDEX_MIDDLE];
        const unsigned int& right = m_content[INDEX_RIGHT];
        if (UIDATESETTING == m_settingType)
        {
            pInfo->m_Time.SetYear(left);
            pInfo->m_Time.SetMonth(middle);
            pInfo->m_Time.SetDay(right);
            pInfo->m_Time.SetSysTime();
        }
        else if (UITIMESETTING == m_settingType)
        {
            if (right && (12 == left))
                left = 0;
            else if (!right && (left < 12))
                left += 12;

            pInfo->m_Time.SetHour(left);
            pInfo->m_Time.SetMinute(middle);
            pInfo->m_Time.SetSysTime();
        }
        else
        {
            pInfo->m_Time.SetTimeZoneEast(left);
            pInfo->m_Time.SetTimeZoneFull(middle);
            pInfo->m_Time.SetTimeZoneHalf(right);
            pInfo->m_Time.SetTimeZone();
        }
        EndDialog(1);
    }
}

void UIDateOrTimeSettingDlg::OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam)
{
    switch(_dwCmdId)
    {
    case ID_BTN_TOUCH_YEAR_UP:
    case ID_BTN_TOUCH_YEAR_DOWN:
        UpdateYear(ID_BTN_TOUCH_YEAR_UP == _dwCmdId);
        break;
    case ID_BTN_TOUCH_HOUR_UP:
    case ID_BTN_TOUCH_HOUR_DOWN:
        UpdateHour(ID_BTN_TOUCH_HOUR_UP == _dwCmdId);
        break;
    case ID_BTN_TOUCH_MONTH_UP:
    case ID_BTN_TOUCH_MONTH_DOWN:
        UpdateMonth(ID_BTN_TOUCH_MONTH_UP == _dwCmdId);
        break;
    case ID_BTN_TOUCH_MINUTE_UP:
    case ID_BTN_TOUCH_MINUTE_DOWN:
        UpdateMinute(ID_BTN_TOUCH_MINUTE_UP == _dwCmdId);
        break;
    case ID_BTN_TOUCH_DAY_UP:
    case ID_BTN_TOUCH_DAY_DOWN:
        UpdateDay(ID_BTN_TOUCH_DAY_UP == _dwCmdId);
        break;
    case ID_BTN_TOUCH_AMORPM_UP:
    case ID_BTN_TOUCH_AMORPM_DOWN:
        UpdateAMorPM(ID_BTN_TOUCH_AMORPM_UP == _dwCmdId);
        break;
    case ID_BTN_TIMEZONE_HEMISPHERE_UP:
    case ID_BTN_TIMEZONE_HEMISPHERE_DOWN:
        UpdateTimeZoneEast(ID_BTN_TIMEZONE_HEMISPHERE_UP == _dwCmdId);
        break;
    case ID_BTN_TIMEZONE_HOUR_UP:
    case ID_BTN_TIMEZONE_HOUR_DOWN:
        UpdateTimeZoneFull(ID_BTN_TIMEZONE_HOUR_UP == _dwCmdId);
        break;
    case ID_BTN_TIMEZONE_MINUTE_UP:
    case ID_BTN_TIMEZONE_MINUTE_DOWN:
        UpdateTimeZoneHalf(ID_BTN_TIMEZONE_MINUTE_UP == _dwCmdId);
        break;
    case IDOK:
        Commit();
        break;
    case IDCANCEL:
        {
            EndDialog(0);
        }
        break;
    default:
        break;
    }
}

int UIDateOrTimeSettingDlg::GetDaysOfMonth(unsigned int _uYear, unsigned int _uMonth)
{
    if (_uMonth < 1 || _uMonth > 12)
        return -1;

    const int DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int iDay = DaysOfMonth[_uMonth - 1];
    if((_uMonth == 2) && ((0 == (_uYear % 4)) || (0 == (_uYear % 100)) || (0 == (_uYear % 400)))) 
    {
        iDay = 29;
    }
    return iDay;
}

void UIDateOrTimeSettingDlg::GetTime()
{
    unsigned int& hour = m_content[INDEX_LEFT];
    unsigned int& minute = m_content[INDEX_MIDDLE];
    unsigned int& am = m_content[INDEX_RIGHT];

    time_t t;
    struct tm lt;
    time(&t);
    localtime_r(&t, &lt);
    hour = lt.tm_hour;
    minute = lt.tm_min;

    am = ((hour >= 0) && (hour < 12));
    if (hour > 12)
    {
        hour-= 12;
    }
    else if (hour == 0)
    {
        hour = 12;
    }

    char buf[20];
    snprintf(buf, DK_DIM(buf), "%02d", hour);
    m_txtContent[INDEX_LEFT].SetText(buf);
    snprintf(buf, DK_DIM(buf), "%02d", minute);
    m_txtContent[INDEX_MIDDLE].SetText(buf);
    m_txtContent[INDEX_RIGHT].SetText(StringManager::GetPCSTRById(am ? TOUCH_SYSTEMSETTING_TIME_AM : TOUCH_SYSTEMSETTING_TIME_PM));
}

void UIDateOrTimeSettingDlg::GetDate()
{
    unsigned int& year = m_content[INDEX_LEFT];
    unsigned int& month = m_content[INDEX_MIDDLE];
    unsigned int& day = m_content[INDEX_RIGHT];

    time_t t;
    struct tm lt;
    time(&t);
    localtime_r(&t, &lt);
    year = lt.tm_year + 1900;
    month = lt.tm_mon + 1;
    day = lt.tm_mday;

    char buf[20];
    snprintf(buf, DK_DIM(buf), "%02d", year);
    m_txtContent[INDEX_LEFT].SetText(buf);
    snprintf(buf, DK_DIM(buf), "%02d", month);
    m_txtContent[INDEX_MIDDLE].SetText(buf);
    snprintf(buf, DK_DIM(buf), "%02d", day);
    m_txtContent[INDEX_RIGHT].SetText(buf);
}

void UIDateOrTimeSettingDlg::GetTimeZone()
{
    SystemSettingInfo *systemInfo = SystemSettingInfo::GetInstance();
    if (systemInfo)
    {
        unsigned int& left = m_content[INDEX_LEFT];
        unsigned int& middle = m_content[INDEX_MIDDLE];
        unsigned int& right = m_content[INDEX_RIGHT];

        left = systemInfo->m_Time.GetTimeZoneEast();
        middle = systemInfo->m_Time.GetTimeZoneFull();
        right = systemInfo->m_Time.GetTimeZoneHalf();

        bool east = (((int)left) >= 0);
        char buf[20];
        m_txtContent[INDEX_LEFT].SetText(StringManager::GetPCSTRById(east ? TIME_ZONE_EAST : TIME_ZONE_WEST));
        snprintf(buf, DK_DIM(buf), "%02d", middle);
        m_txtContent[INDEX_MIDDLE].SetText(buf);
        snprintf(buf, DK_DIM(buf), "%02d", right);
        m_txtContent[INDEX_RIGHT].SetText(buf);
    }
}

void UIDateOrTimeSettingDlg::DrawBackground(DK_IMAGE drawingImg)
{
    UISizerItem* pItem = m_windowSizer->GetItem(&m_txtTitle);
    if (pItem)
    {
        const int height = pItem->GetSize().GetHeight();
        UIUtility::DrawBorder(drawingImg, 0, 0, 0, 0, height);
    }
}

HRESULT UIDateOrTimeSettingDlg::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
    {
        return S_OK;
    }

    if (drawingImg.pbData == NULL)
    {
        return E_POINTER;
    }

    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        drawingImg,
        rcSelf,
        &imgSelf
        ));

    DrawBackground(imgSelf);

    int iSize = GetChildrenCount();
    for (int i = 0; i < iSize; ++i)
    {
        UIWindow* pWnd = GetChildByIndex(i);
        if (pWnd)
        {
            pWnd->Draw(imgSelf);
        }
    }

    return hr;
}

