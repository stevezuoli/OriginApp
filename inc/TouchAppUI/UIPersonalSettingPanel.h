#ifndef __UIRERSONALSETTINGPANEL_H__
#define __UIRERSONALSETTINGPANEL_H__

#include "CommonUI/UITablePanel.h"
#include "TouchAppUI/UIPersonalSettingPanel1.h"
#include "TouchAppUI/UIPersonalSettingPanel2.h"

#include "GUI/GestureDetector.h"
#include "GUI/UITextSingleLine.h"

class UIPersonalSettingPanel: public UITablePanel
{
public:
	UIPersonalSettingPanel();
    virtual LPCSTR GetClassName() const
    {
        return "UIPersonalSettingPanel";
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
	
	UIPersonalSettingPanel1 m_personalPanel1;
	UIPersonalSettingPanel2 m_personalPanel2;
};
#endif

