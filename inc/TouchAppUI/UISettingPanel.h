#ifndef __TOUCHAPPUI_UISETTINGPANEL_H__
#define __TOUCHAPPUI_UISETTINGPANEL_H__

#include "GUI/UIContainer.h"
#include <string>
#include <vector>
#include  "KernelType.h"

class UIBoxSizer;
class UITouchComplexButton;
class UITextSingleLine;
class SystemSettingInfo;
class DKFont;

class UISettingPanel: public UIContainer
{
public:
    UISettingPanel();
    virtual ~UISettingPanel();
    //virtual dkSize GetMinSize() const;
    virtual void MoveWindow(int left, int top, int width, int height);
    virtual void OnChildSetFocus(UIWindow* pWindow)
    {
        // override and do nothing
        // otherwise buttons in different groups will affect each other
    }
protected:
    UIBoxSizer* AddSectionTitle(const char* text);
    //functions return the righthalf sizer on purpose.
    //the up-level sizer which contains both righthalf and lefthalf sizer will be stored in wholeSizer
    UIBoxSizer* AddPushButton(const char* itemTitle, int itemBtnId, const char* btnText, UISizer** wholeSizer = NULL, UITextSingleLine** textOnRight = NULL);
    UIBoxSizer* AddLabel(const char* itemTitle, const char* text);
    UIBoxSizer* AddArrowButton(const char* itemTitle, int itemBtnId, const char* btnText);
    UIBoxSizer* AddSwitchButton(int itemTextid, int itemBtnId, int onImageId, int offImageId);
    UIBoxSizer* AddRadioButtonGroups(const char* itemTitle, int groupItemId, int itemButtonCount, int* itemButtonIds, const char** itemButtonTexts,  int initSelected);
    bool EnsureMainSizerExisted();
    UIBoxSizer* CreateItemTitleSizer(const char* text, UITextSingleLine** textSingleLine = NULL);
    UITouchComplexButton* CreateButton(int btnId, const char* btnText, bool addAsChild);
    UITextSingleLine* CreateTextSingleLine(const char* text, bool addAsChild);
    UIBoxSizer* CreateRightHalfItemSizer();
    UIBoxSizer* AddItemTips(const char* text);

protected:
    // NOTICE: Don't exchange relative order of buttons in a radio group
    // like progressBarFull and progressBarMini
    // other logics depends on them
    enum SettingBtnId
    {
        SBI_RefreshInterval,
        SBI_ProgressBarFull,
        SBI_ProgressBarMini,
        SBI_ChineseFont,
        SBI_EnglishFont,
        SBI_TextConvertSimpleChinese,
        SBI_TextConvertTraditionalChinese,
        SBI_FontRenderSharp,
        SBI_FontRenderSmooth,
        SBI_ExportReadingData,
        SBI_EpubMobiMetaData,
        SBI_EpubMobiFileName,
        SBI_FontSize,
        SBI_ReadingLayoutFullScreen,
        SBI_ReadingLayoutShowReaderMessage,
        SBI_Layout,
        SBI_Restart,
        SBI_SwitchToKindle,
        SBI_CheckUpdate,
        SBI_WifiSetting,
        SBI_WifiTransfer,
        SBI_BatteryRatioMode,
        SBI_LanguageSetting,
        SBI_InputMethodSetting,
        SBI_YearSetting,
        SBI_MonthSetting,
        SBI_DaySetting,
        SBI_HourSetting,
        SBI_MinuteSetting,
        SBI_AMPMSetting,
        SBI_Uninstall,
        SBI_CopyRight,
        SBI_Logout,
        SBI_ModifyPersonalInfo,
        SBI_PushMailBox,
        SBI_Login,
        SBI_Register,
        SBI_TimeFormat12,
        SBI_TimeFormat24,
        SBI_TimeZoneSetting,
        SBI_BindWithSinaWeibo,
        SBI_UnbindWithSinaWeibo,
        SBI_SwitchWithSinaWeibo,
        SBI_BindEvernote,
        SBI_UnbindEvernote
    };
    // 磁盘剩余空间：大于1GB按G显示；小于1GB按M显示
    std::string GetDiskSpace() const;
    std::string GetVersion() const;
    std::string GetKindleSn() const;
    std::string GetKindleVersion() const;
    std::string GetKindleMac() const;
    std::vector<UIWindow*> m_dynamicCreatedWindows;
    int GetRepaintPage() const;
    void SetRepaintPage(int page);
    int GetProgressBarMode() const;
    void SetProgressBarMode(int mode);
    int GetTextConvertMode() const;
    void SetTextConvertMode(int mode);
    int GetEpubMobiMetaDataMode() const;
    void SetEpubMobiMetaDataMode(int mode);
    int GetFontRender() const;
    void SetFontRender(int render);
    int GetTimeSystem() const;
    void SetTimeSystem(int mode);
    int GetTimeZoneEast() const;
    void SetTimeZoneEast(int mode);
    int GetTimeZoneFull() const;
    void SetTimeZoneFull(int mode);
    int GetTimeZoneHalf() const;
    void SetTimeZoneHalf(int mode);

    int m_titleFontSize;
    int m_itemFontSize;
    int m_itemButtonFontSize;
    int m_buttonLeftMargin;
    int m_buttonTopMargin;
    int m_titleLeftMargin;
    int m_itemLeftMagin;
    int m_leftHalfWidth;
    int m_rightHalfWidth;
    int m_titleUpSpacing;
    int m_titleDownSpacing;
    int m_itemVertSpacing;
    int m_itemTipVertSpacing;
    int m_itemHorzSpacing;
    int m_fontMenuWidth;
    SystemSettingInfo* m_systemSettingInfo;
    std::vector<std::string> m_chineseFontNames;
    std::vector<std::string> m_englishFontNames;
    void GetFontNamesByCharsets(DK_CHARSET_TYPE* charSets, size_t size, std::vector<std::string>* fontNames);
    void GetFontNamesByCharset(DK_CHARSET_TYPE charSet, std::vector<std::string>* fontNames);
    int FindFontName(const std::vector<std::string>& fontNames, const std::string& curFontName);

    void InitFonts();
    int m_selectedChineseFontIndex;
    int m_selectedEnglishFontIndex;
    std::string m_curChineseFontName;
    std::string m_curEnglishFontName;

    int GetFontSizeIndex() const;
    int GetFontSizeCount() const;
    void SetFontSizeIndex(int index);

	int GetReadingLayout() const;
	void SetReadingLayout(int mode);
	
    int GetLayoutMode() const;
    void SetLayoutMode(int mode);
    bool IsWifiOn() const;
    bool IsWifiConnected() const;
    std::string GetApName() const;

    void InitIME();
    int m_selectedIMEIndex;
    std::vector<std::string> m_imeNames;

    void SetSelectedIME(int ime);

    void InitLanguages();
    std::vector<std::string> m_languageNames;
    int m_selectedLanguage;
    void SelectLanguage(int index);

    void InitLayout();
    std::vector<std::string> m_layoutNames;
};
#endif
