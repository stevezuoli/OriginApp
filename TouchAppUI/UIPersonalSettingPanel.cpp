#include "TouchAppUI/UIPersonalSettingPanel.h"
#include "GUI/UISizer.h"
#include "I18n/StringManager.h"
#include "Common/WindowsMetrics.h"
using namespace WindowsMetrics;

UIPersonalSettingPanel::UIPersonalSettingPanel()
	: UITablePanel()
	, m_curPanelIndex(0)
	, m_curPanel(NULL)
{
	Init();
}

void UIPersonalSettingPanel::Init()
{
	m_panels.push_back(&m_personalPanel1);
	m_panels.push_back(&m_personalPanel2);
	m_personalPanel2.SetVisible(false);
	m_curPanel = &m_personalPanel1;

	m_curPageLine.SetFontSize(GetWindowFontSize(UISettingPanelItemButtonIndex));
	m_curPageLine.SetEnglishGothic();
	m_curPageLine.SetAlign(ALIGN_RIGHT);

	AppendChild(&m_personalPanel1);
	AppendChild(&m_personalPanel2);
	AppendChild(&m_curPageLine);
	
	UIBoxSizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    mainSizer->Add(m_curPanel, UISizerFlags(1).Expand());
	mainSizer->Add(&m_curPageLine, UISizerFlags().Expand().Border(dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
    SetSizer(mainSizer);

	UpdatePageNumber();
}

void UIPersonalSettingPanel::InitStatus()
{
    m_personalPanel1.InitStatus();
	m_personalPanel2.InitStatus();
}

void UIPersonalSettingPanel::UpdatePanel(int panelIndex)
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

bool UIPersonalSettingPanel::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
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
 
void UIPersonalSettingPanel::UpdatePageNumber()
{
	char buf[20];
	snprintf(buf, DK_DIM(buf), "%d/%d%s", m_curPanelIndex + 1, m_panels.size(), StringManager::GetPCSTRById(BOOK_PAGE));
	m_curPageLine.SetText(buf);
}

bool UIPersonalSettingPanel::PageUp()
{
	--m_curPanelIndex;
	int panelSize = m_panels.size();
	m_curPanelIndex = (m_curPanelIndex < 0 ? panelSize - 1 : m_curPanelIndex);
	UpdatePanel(m_curPanelIndex);
	return true;
}

bool UIPersonalSettingPanel::PageDown()
{
	++m_curPanelIndex;
	int panelSize = m_panels.size();
	m_curPanelIndex = (m_curPanelIndex >= panelSize ? 0 : m_curPanelIndex);
	UpdatePanel(m_curPanelIndex);
	return true;
}

