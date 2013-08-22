#ifndef __UISYSTEMSETTINGPANEL_H__
#define __UISYSTEMSETTINGPANEL_H__

#include "CommonUI/UITablePanel.h"
#include "TouchAppUI/UISystemSettingPanel1.h"
#include "TouchAppUI/UISystemSettingPanel2.h"
#include "TouchAppUI/UISystemSettingPanel3.h"
#include "GUI/GestureDetector.h"
#include "GUI/UITextSingleLine.h"

class UISystemSettingPanel: public UITablePanel
{
public:
	UISystemSettingPanel();
    virtual LPCSTR GetClassName() const
    {
        return "UISystemSettingPanel";
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
	
	UISystemSettingPanel1 m_systemPanel1;
	UISystemSettingPanel2 m_systemPanel2;
	UISystemSettingPanel3 m_systemPanel3;
};

#endif

