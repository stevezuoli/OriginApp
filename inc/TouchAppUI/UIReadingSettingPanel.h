#ifndef __UIREADINGSETTINGPANEL_H__
#define __UIREADINGSETTINGPANEL_H__

#include "CommonUI/UITablePanel.h"
#include "TouchAppUI/UIReadingSettingPanel1.h"
#include "TouchAppUI/UIReadingSettingPanel2.h"
#include "GUI/GestureDetector.h"

class UIReadingSettingPanel: public UITablePanel
{
public:
	UIReadingSettingPanel();
    virtual LPCSTR GetClassName() const
    {
        return "UIReadingSettingPanel";
    }
	virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);

    virtual void InitStatus();
private:
	bool PageUp();
	bool PageDown();
	
	void Init();
	void UpdatePanel(int panelIndex);
	void UpdatePageNumber();
	
	UITextSingleLine m_curPageLine;
	std::vector<UISettingPanel*> m_panels;
	int m_curPanelIndex;
	UISettingPanel* m_curPanel;
	
	UIReadingSettingPanel1 m_readingPanel1;
	UIReadingSettingPanel2 m_readingPanel2;
};
#endif

