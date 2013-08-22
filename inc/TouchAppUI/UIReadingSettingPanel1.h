#ifndef __TOUCHAPPUI_UIREADINGSETTINGPANEL1_H__
#define __TOUCHAPPUI_UIREADINGSETTINGPANEL1_H__

#include "GUI/UIContainer.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UIRadioButtonGroup.h"
#include "TouchAppUI/UISettingPanel.h"

class UIReadingSettingPanel1: public UISettingPanel
{
public:
    UIReadingSettingPanel1();
    virtual ~UIReadingSettingPanel1();
    virtual bool OnChildClick(UIWindow* child);

    void InitStatus();
private:
    void OnRefreshIntervalClick();
    void OnProgressBarModeClick(int mode);
    void OnTextConvertModeClick(int mode);
    void OnFontRenderClick(int mode);
    void OnEpubMobiMetaDataMode(int mode);
    void Init();
    void OnExportReadingData();
    void ShowFontDialog(UITouchComplexButton* button, const std::vector<std::string>& fontNames, int selectedIndex);
    UITextSingleLine* m_refreshIntervalPage;
    UITouchComplexButton* m_refreshIntervalPageButton;
    UIRadioButtonGroup* m_progressBarModeGroup;
    UIRadioButtonGroup* m_textConvertGroup;
    UIRadioButtonGroup* m_fontRenderGroup;
    UIRadioButtonGroup* m_epubMobiGroup;
    UITouchComplexButton* m_chineseFontButton;
    UITouchComplexButton* m_englishFontButton;
};

#endif
