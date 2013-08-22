#include <tr1/functional>
#include "BookStoreUI/UIBookStoreTablePage.h"
#include "BookStoreUI/UIBookStoreListBoxPanel.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "GUI/UISizer.h"
#include "GUI/CTpGraphics.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIBookStoreTablePage::UIBookStoreTablePage(const std::string& title, bool hasBottomBar, bool showFetchInfo, bool showRefresh)
    : UIBookStoreNavigation(title, hasBottomBar, showFetchInfo, showRefresh)
    , m_pCurPanel(NULL)
{
    SetLeftMargin(GetWindowMetrics(UIBookStoreTablePageListBoxLeftMarginIndex));
}

UIBookStoreTablePage::~UIBookStoreTablePage()
{
}

UISizer* UIBookStoreTablePage::CreateMidSizer()
{
    if (NULL == m_pMidSizer)
    {
        m_pMidSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pMidSizer)
        {
            SetTableController();
            m_tableController.SetInitPanelVisibleStatus(false);
            m_tableController.SetLeftMargin(m_iLeftMargin);
            m_tableController.SetBottomLinePixel(GetWindowMetrics(UIBookStorePageTableControllerBottomLinePixelIndex));
            m_tableController.SetFocusedImageID(IMAGE_BOOKSTORE_TABLEBAR_ARROW_TOP);
            m_tableController.LayOutTableBox();
            m_pMidSizer->Add(&m_tableController, UISizerFlags(1).Expand());
        }
    }
    return m_pMidSizer;
}

void UIBookStoreTablePage::SetTableController()
{
}

void UIBookStoreTablePage::SetFetchStatus(FETCH_STATUS status)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s m_fetchStatus = %d, status = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, m_fetchStatus, status);
    if (m_fetchStatus != status)
    {
        m_tableController.ShowPanel(status == FETCH_SUCCESS);
        UIBookStoreNavigation::SetFetchStatus(status);
    }
    else if (m_fetchInfo.IsVisible())
    {
        m_fetchInfo.UpdateWindow();
    }
}

bool UIBookStoreTablePage::OnMessageListBoxUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    bool result = false;
    UIBookStoreListBoxPanel* pShowPanel = (UIBookStoreListBoxPanel*)m_tableController.GetShowPanel();
    if (pShowPanel && m_pCurPanel && (m_pCurPanel == pShowPanel))
    {
        result = pShowPanel->OnMessageListBoxUpdate(args);
        SetFetchStatus(result ? (pShowPanel->GetItemCount() > 0 ? FETCH_SUCCESS : FETCH_NOELEM) : FETCH_FAIL);
    }
    return result;
}

bool UIBookStoreTablePage::OnMessagePanelInfoUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const TablePanelArgs& msgArgs = (const TablePanelArgs&)args;
    if (m_pCurPanel == msgArgs.window)
    {
        bool result = msgArgs.succeeded;
        SetFetchStatus(result ? (msgArgs.total > 0 ? FETCH_SUCCESS : FETCH_NOELEM) : FETCH_FAIL);
        return result;
    }
    return true;
}

void UIBookStoreTablePage::UpdateSectionUI(bool bIsFull)
{
    if (m_pCurPanel)
    {
        m_pCurPanel->UpdateSectionUI(bIsFull);
    }
}

BOOL UIBookStoreTablePage::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    if (m_mnuMain.IsVisible())
    {
        return UIBookStoreNavigation::OnKeyPress(iKeyCode);
    }

    if (UIBookStoreListBoxPanel* pShowPanel = (UIBookStoreListBoxPanel*)m_tableController.GetShowPanel())
    {
        switch (iKeyCode)
        {
            case KEY_UP:
                if (m_tableController.IsFocus() && !pShowPanel->IsFocus())
                {
                    if (m_btnRefresh.IsVisible())
                    {
                        m_btnRefresh.SetFocus(true);
                        return FALSE;
                    }
                    else if (m_pCurPanel)
                    {
                        return m_pCurPanel->OnKeyPress(iKeyCode);
                    }
                }
				else if(m_btnRefresh.IsFocus() && m_pCurPanel)
				{
					return m_pCurPanel->OnKeyPress(iKeyCode);
				}
                break;
            case KEY_DOWN:
                if (m_tableController.IsFocus() && pShowPanel && pShowPanel->IsFocus() && (pShowPanel->GetSelectedItemIndex() == pShowPanel->GetNumberPerPage() - 1))
                {
                    m_btnRefresh.SetFocus(true);
                    return FALSE;
                }
                break;
            default:
                break;
        }
    }

    switch (iKeyCode)
    {
    case KEY_LPAGEDOWN:
    case KEY_RPAGEDOWN:
    case KEY_SPACE:
    case KEY_LPAGEUP:
    case KEY_RPAGEUP:
        if (m_pCurPanel)
        {
            return m_pCurPanel->OnKeyPress(iKeyCode);
        }
        break;
    default:
        break;
    }
#endif
    return UIBookStoreNavigation::OnKeyPress(iKeyCode);
}

