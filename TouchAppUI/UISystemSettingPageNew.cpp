#include "TouchAppUI/UISystemSettingPageNew.h"
#include "TouchAppUI/UIReadingSettingPanel.h"
#include "TouchAppUI/UISystemSettingPanel.h"
#include "TouchAppUI/UIPersonalSettingPanel.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"

UISystemSettingPageNew::UISystemSettingPageNew()
    : m_readingPanel(NULL)
    , m_systemPanel(NULL)
    , m_accountPanel(NULL)
{
    Init();
    m_gestureListener.SetPage(this);
    m_gestureDetector.SetListener(&m_gestureListener);
}

void UISystemSettingPageNew::Init()
{
    if(!m_readingPanel)
    {
        m_readingPanel = new UIReadingSettingPanel();
        m_tabController.AppendTableBox(ID_TAB_SYSTEMSETTING_READING, StringManager::GetPCSTRById(SETTING_READING), m_readingPanel, false);
    }
    if(!m_systemPanel)
    {
        m_systemPanel = new UISystemSettingPanel();
        m_tabController.AppendTableBox(ID_TAB_SYSTEMSETTING_SYSTEM, StringManager::GetPCSTRById(SETTING_SYSTEM), m_systemPanel, true);
    }
    if(!m_accountPanel)
    {
        m_accountPanel = new UIPersonalSettingPanel();
        m_tabController.AppendTableBox(ID_TAB_SYSTEMSETTING_ACCOUNT, StringManager::GetPCSTRById(SETTING_ACCOUNT), m_accountPanel, false);
    }
    m_tabController.SetLeftMargin(m_iLeftMargin);
    m_tabController.SetBottomLinePixel(GetWindowMetrics(UISystemSettingPageTableControllerBottomLinePixelIndex));
    m_tabController.SetFocusedImageID(IMAGE_SYSTEMSETTING_TABLEBAR_ARROW_TOP);
    m_tabController.LayOutTableBox();

    AppendChild(&m_titleBar);
    AppendChild(&m_tabController);
    AppendChild(m_pBottomBar);

    UIBoxSizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    mainSizer->Add(&m_titleBar);
    mainSizer->Add(&m_tabController, UISizerFlags(1).Expand());
    mainSizer->Add(m_pBottomBar);
    SetSizer(mainSizer);
}

void UISystemSettingPageNew::MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height)
{
    UIPage::MoveWindow(left, top, width, height);
    Layout();
}

bool UISystemSettingPageNew::OnTouchEvent(MoveEvent* moveEvent)
{
    return m_gestureDetector.OnTouchEvent(moveEvent);
}

bool UISystemSettingPageNew::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{ 
    UITablePanel* showPanel = m_tabController.GetShowPanel();
    if(showPanel)
    {
        return showPanel->OnFling(moveEvent1, moveEvent2, direction, vx, vy);
    }
    return false;
}

void UISystemSettingPageNew::OnEnter()
{
    if(m_readingPanel)
    {
        m_readingPanel->InitStatus();
    }
    if(m_systemPanel)
    {
        m_systemPanel->InitStatus();
    }
    if(m_accountPanel)
    {
        m_accountPanel->InitStatus();
    }
}
