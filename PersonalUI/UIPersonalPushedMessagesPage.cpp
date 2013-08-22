#include <tr1/functional>
#include "PersonalUI/PushedMessagesManager.h"
#include "PersonalUI/UIPersonalPushedMessagesPage.h"
#include "PersonalUI/UIBookStoreListBoxOfMessages.h"
#include "BookStore/DataUpdateArgs.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/CPageNavigator.h"
#include "Common/CAccountManager.h"
#include "Framework/CDisplay.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;
using namespace dk::account;

UIPersonalPushedMessagesPage::UIPersonalPushedMessagesPage()
    : UIBookStoreListBoxPage(StringManager::GetPCSTRById(PERSONAL_MESSAGE))
{
    SubscribeMessageEvent(PushedMessagesManager::EventPushedMessageMgrUpdate, 
        *PushedMessagesManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalPushedMessagesPage::OnMessagePushedMessagesUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    SubscribeMessageEvent(CAccountManager::EventAccount,
        *CAccountManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalPushedMessagesPage::LogDataChanged),
        this,
        std::tr1::placeholders::_1));

    SubscribeMessageEvent(PushedMessagesManager::EventMessageUnreadCountUpdate,
        *PushedMessagesManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalPushedMessagesPage::OnMessageUnreadCountChanged),
        this,
        std::tr1::placeholders::_1));
    SetLeftMargin(0);
}

UIPersonalPushedMessagesPage::~UIPersonalPushedMessagesPage()
{
}

bool UIPersonalPushedMessagesPage::CreateListBoxPanel()
{
    if (NULL == m_pListBoxPanel)
    {
        m_pListBoxPanel = new UIBookStoreListBoxOfMessages(GetWindowMetrics(UIPersonalMessagesPageItemCountIndex), 
            GetWindowMetrics(UIPersonalMessagesPageItemHeightIndex));
        if (m_pListBoxPanel)
        {
#ifdef KINDLE_FOR_TOUCH
            m_pListBoxPanel->SetItemEnableLongClick(true);
#endif
            m_pListBoxPanel->SetOnceAllItems(true);
        }

    }
    return (NULL != m_pListBoxPanel);
}

UISizer* UIPersonalPushedMessagesPage::CreateMidSizer()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (CreateListBoxPanel() && (NULL == m_pMidSizer))
    {
        m_pMidSizer = new UIBoxSizer(dkVERTICAL);
        if (m_pMidSizer)
        {
            AppendChild(m_pListBoxPanel);

            m_loading.SetFontSize(GetWindowFontSize(FontSize16Index));
            m_loading.SetText(StringManager::GetPCSTRById(BOXMESSAGES_MORE));
            m_loading.SetAlign(ALIGN_CENTER);
            m_loading.SetMinHeight(GetWindowMetrics(UIPixelValue30Index));
            m_loading.SetVisible(false);
            AppendChild(&m_loading);

            m_pMidSizer->Add(&m_loading, UISizerFlags().Expand().Align(dkALIGN_CENTER));
            m_pMidSizer->Add(m_pListBoxPanel, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).ReserveSpaceEvenIfHidden());
        }
    }
    return m_pMidSizer;
}

void UIPersonalPushedMessagesPage::SetFetchStatus(FETCH_STATUS status)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s m_fetchStatus = %d, status = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, m_fetchStatus, status);
    if (m_pListBoxPanel && (m_fetchStatus != status))
    {
        m_fetchStatus = (status == NOFETCH) ? FETCH_SUCCESS : status;
        PushedMessagesManager* messageMgr = PushedMessagesManager::GetInstance();
        bool isLoading = messageMgr && (messageMgr->GetBoxMessagesUnreadCount() > 0);
        m_loading.SetVisible(isLoading);

        int itemHeight = GetWindowMetrics(UIPersonalMessagesPageItemHeightIndex);
        itemHeight -= isLoading ? GetWindowMetrics(UIPixelValue8Index) : 0;
        m_pListBoxPanel->SetItemHeight(itemHeight);
        m_pListBoxPanel->UpdatePushedMessagesList();
        m_fetchInfo.SetFetchStatus((isLoading || m_pListBoxPanel->GetItemCount() > 0) ? FETCH_SUCCESS : FETCH_NOELEM);
        Layout();
        CDisplay::CacheDisabler cache;
        Repaint();
    }
}

bool UIPersonalPushedMessagesPage::OnMessagePushedMessagesUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_pListBoxPanel)
    {
        SetFetchStatus(FETCH_SUCCESS);
        return true;
    }
    return false;
}

void UIPersonalPushedMessagesPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
#ifdef KINDLE_FOR_TOUCH
    case ID_BTN_TOUCH_BACK:
        {
            CPageNavigator::BackToPrePage();
        }
        break;
#endif
    case ID_BTN_REFRESH:
        SetFetchStatus(FETCHING);
        StopGetMessages();
        StartGetMessages();
        break;
    default:
        UIBookStoreListBoxPage::OnCommand(dwCmdId, pSender, dwParam);
        break;
    }
}

bool UIPersonalPushedMessagesPage::OnMessageUnreadCountChanged(const EventArgs& args)
{
    const DataUpdateArgs& unreadCountArgs = (const DataUpdateArgs&)args;
    if (unreadCountArgs.succeeded && IsDisplay())
    {
        if (unreadCountArgs.total > 0)
        {
            SetFetchStatus(FETCHING);
            PushedMessagesManager* pPushedMgr = PushedMessagesManager::GetInstance();
            if (pPushedMgr)
            {
                pPushedMgr->FetchBoxMessageLst();
            }
        }
        else
        {
            // for updatewindow
            SetFetchStatus(NOFETCH);
        }
    }
    return true;
}

bool UIPersonalPushedMessagesPage::LogDataChanged(const EventArgs& args)
{
    StartGetMessages();
    return true;
}

void UIPersonalPushedMessagesPage::StartGetMessages()
{
    CAccountManager* accountManager = CAccountManager::GetInstance();
    if (accountManager->IsLoggedIn())
    {
        PushedMessagesManager* pPushedMgr = PushedMessagesManager::GetInstance();
        if (pPushedMgr && pPushedMgr->ResetCurrentUserID())
        {
            pPushedMgr->StartGetMessages();
        }
    }
    else
    {
        CAccountManager::GetInstance()->LoginWithTokenAsync();
    }
}

void UIPersonalPushedMessagesPage::StopGetMessages()
{
    PushedMessagesManager* pPushedMgr = PushedMessagesManager::GetInstance();
    if (pPushedMgr)
    {
        pPushedMgr->StopGetMessages(false);
    }
}

void UIPersonalPushedMessagesPage::OnEnter()
{
    UIPage::OnEnter();
    StartGetMessages();
}

void UIPersonalPushedMessagesPage::OnLeave()
{
    UIPage::OnLeave();
    StopGetMessages();
}

void UIPersonalPushedMessagesPage::OnLoad()
{
    UIPage::OnLoad();
    StartGetMessages();
}

void UIPersonalPushedMessagesPage::OnUnLoad()
{
    UIPage::OnUnLoad();
    StopGetMessages();
}

BOOL UIPersonalPushedMessagesPage::OnKeyPress(INT32 iKeyCode)
{
#ifdef KINDLE_FOR_TOUCH
    return UIPage::OnKeyPress(iKeyCode);
#else
    if (iKeyCode == KEY_BACK)
    {
        if (m_mnuMain.IsVisible())
        {
            m_mnuMain.Show(FALSE);
        }
        else
        {
            CPageNavigator::BackToPrePage();
        }
        return FALSE;
    }
    return UIBookStoreListBoxPage::OnKeyPress(iKeyCode);
#endif
}

