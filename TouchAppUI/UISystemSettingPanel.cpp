#include "TouchAppUI/UISystemSettingPanel.h"
#include "GUI/UISizer.h"
#include "I18n/StringManager.h"
#include "Common/WindowsMetrics.h"
using namespace WindowsMetrics;

UISystemSettingPanel::UISystemSettingPanel()
	: UITablePanel()
	, m_curPanelIndex(0)
	, m_curPanel(NULL)
{
	Init();
}

void UISystemSettingPanel::Init()
{
	m_panels.push_back(&m_systemPanel1);
	m_panels.push_back(&m_systemPanel2);
	m_panels.push_back(&m_systemPanel3);
	m_systemPanel2.SetVisible(false);
	m_systemPanel3.SetVisible(false);
	m_curPanel = &m_systemPanel1;

	m_curPageLine.SetFontSize(GetWindowFontSize(UISettingPanelItemButtonIndex));
	m_curPageLine.SetEnglishGothic();
	m_curPageLine.SetAlign(ALIGN_RIGHT);

	AppendChild(&m_systemPanel1);
	AppendChild(&m_systemPanel2);
	AppendChild(&m_systemPanel3);
	AppendChild(&m_curPageLine);
	
	UIBoxSizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    mainSizer->Add(m_curPanel, UISizerFlags(1).Expand());
	mainSizer->Add(&m_curPageLine, UISizerFlags().Expand().Border(dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
    SetSizer(mainSizer);

	UpdatePageNumber();
}

void UISystemSettingPanel::UpdatePanel(int panelIndex)
{
	if (panelIndex >= 0 && panelIndex < (int)m_panels.size())
	{
		UpdatePageNumber();
		UISizer* mainSizer = GetSizer();
		UISettingPanel* newPanel = m_panels[panelIndex];
		mainSizer->Replace(m_curPanel, newPanel);
		m_curPanel->SetVisible(false);
		m_curPanel = newPanel;
		m_curPanel->SetVisible(true);
		Layout();
		Repaint(); 
	}
}

bool UISystemSettingPanel::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{
	switch (direction)
	{
		case FD_UP:
		case FD_LEFT:
			return PageDown();
		case FD_DOWN:
		case FD_RIGHT:
			return PageUp();
		default:
			return false;
	}
}
 
void UISystemSettingPanel::UpdatePageNumber()
{
	char buf[20];
	snprintf(buf, DK_DIM(buf), "%d/%d%s", m_curPanelIndex + 1, m_panels.size(), StringManager::GetPCSTRById(BOOK_PAGE));
	m_curPageLine.SetText(buf);
}

bool UISystemSettingPanel::PageUp()
{
	--m_curPanelIndex;
	int panelSize = m_panels.size();
	m_curPanelIndex = (m_curPanelIndex < 0 ? panelSize - 1 : m_curPanelIndex);
	UpdatePanel(m_curPanelIndex);
	return true;
}

bool UISystemSettingPanel::PageDown()
{
	++m_curPanelIndex;
	int panelSize = m_panels.size();
	m_curPanelIndex = (m_curPanelIndex >= panelSize ? 0 : m_curPanelIndex);
	UpdatePanel(m_curPanelIndex);
	return true;
}

void UISystemSettingPanel::InitStatus()
{
    //note:only the item in panel1 need update until now
    m_systemPanel1.InitStatus();
}