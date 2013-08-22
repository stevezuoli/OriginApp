#include <tr1/functional>
#include "BookStoreUI/UIBookStoreListBoxPage.h"
#include "BookStoreUI/UIBookStoreListBoxPanel.h"
#include "BookStore/DataUpdateArgs.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIBookStoreListBoxPage::UIBookStoreListBoxPage(const std::string& title, bool hasBottomBar)
    : UIBookStoreNavigation(title, hasBottomBar)
    , m_pListBoxPanel(NULL)
{
}

UIBookStoreListBoxPage::~UIBookStoreListBoxPage()
{
    SafeDeletePointer(m_pListBoxPanel);
}

bool UIBookStoreListBoxPage::CreateListBoxPanel()
{
    return false;
}

UISizer* UIBookStoreListBoxPage::CreateMidSizer()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (CreateListBoxPanel() && (NULL == m_pMidSizer))
    {
        m_pMidSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pMidSizer)
        {
            AppendChild(m_pListBoxPanel);
            m_pMidSizer->Add(m_pListBoxPanel, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).ReserveSpaceEvenIfHidden());
        }
    }
    return m_pMidSizer;
}

void UIBookStoreListBoxPage::SetFetchStatus(FETCH_STATUS status)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s m_fetchStatus = %d, status = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, m_fetchStatus, status);
    if (m_pListBoxPanel && (m_fetchStatus != status))
    {
        m_pListBoxPanel->SetVisible(status == FETCH_SUCCESS);
    }
    UIBookStoreNavigation::SetFetchStatus(status);
}

bool UIBookStoreListBoxPage::OnMessageListBoxUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_pListBoxPanel)
    {
        bool result = m_pListBoxPanel->OnMessageListBoxUpdate(args);
        SetFetchStatus(result ? (m_pListBoxPanel->GetItemCount() > 0 ? FETCH_SUCCESS : FETCH_NOELEM) : FETCH_FAIL);
        return result;
    }
    return false;
}

BOOL UIBookStoreListBoxPage::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_mnuMain.IsVisible())
    {
        return UIBookStorePage::OnKeyPress(iKeyCode);
    }

    if (!SendKeyToChildren(iKeyCode))
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s if (!SendKeyToChildren(iKeyCode))", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return FALSE;
    }
    switch (iKeyCode)
    {
    case KEY_UP:
    case KEY_DOWN:
        if (m_pListBoxPanel)
        {
            if (m_btnRefresh.IsFocus())
            {
                m_pListBoxPanel->SetSelectedItemIndex(KEY_UP == iKeyCode ? m_pListBoxPanel->GetCurrentMaxItemIndex() : 0);
                m_pListBoxPanel->SetFocus(true);
                m_btnRefresh.SetFocus(false);
                return FALSE;
            }
            else if (m_pListBoxPanel->IsFocus())
            {
                int selectedItemIndex = m_pListBoxPanel->GetSelectedItemIndex();
                if ((iKeyCode == KEY_DOWN && selectedItemIndex == m_pListBoxPanel->GetCurrentMaxItemIndex())
                 || (iKeyCode == KEY_UP && selectedItemIndex == 0))
                {
                    m_btnRefresh.SetFocus(true);
                    m_pListBoxPanel->SetFocus(false);
                    return FALSE;
                }
                else 
                {
                    return m_pListBoxPanel->OnKeyPress(iKeyCode);
                }
            }
        }
        break;
    case KEY_LPAGEDOWN:
    case KEY_RPAGEDOWN:
    case KEY_SPACE:
    case KEY_LPAGEUP:
    case KEY_RPAGEUP:
        if (m_pListBoxPanel)
        {
            return m_pListBoxPanel->OnKeyPress(iKeyCode);
        }
        break;
    default:
        if(!SendHotKeyToChildren(iKeyCode))
        {
            return false;
        }
        break;
    }
#endif
    return UIBookStoreNavigation::OnKeyPress(iKeyCode);
}

void UIBookStoreListBoxPage::UpdateSectionUI(bool bIsFull)
{
    if (m_pListBoxPanel)
    {
        m_pListBoxPanel->UpdateSectionUI(bIsFull);
    }
    return;
}


