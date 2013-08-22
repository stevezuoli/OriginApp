#ifndef __TOUCHAPPUI_UIREADINGSETTINGPANEL2_H__
#define __TOUCHAPPUI_UIREADINGSETTINGPANEL2_H__

#include "GUI/UIContainer.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UIRadioButtonGroup.h"
#include "TouchAppUI/UISettingPanel.h"

class UIReadingSettingPanel2: public UISettingPanel
{
public:
    UIReadingSettingPanel2();
    virtual ~UIReadingSettingPanel2();

    void InitStatus();
private:
    void Init();
    bool OnChildClick(UIWindow* child);
	void OnReadingLayoutClick(int mode);
    void OnLayoutMode();
    void OnFontSizeClick();
    std::string FontSizeIndexToString(int index);
    UITouchComplexButton* m_layoutButton;
    UITouchComplexButton* m_fontSizeButton;
	UIRadioButtonGroup* m_readerLayoutGroup;
};
#endif
