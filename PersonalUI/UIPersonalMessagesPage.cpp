#include <tr1/functional>
#include "PersonalUI/PushedMessagesManager.h"
#include "PersonalUI/UIPersonalMessagesPage.h"
#include "PersonalUI/UIBookStoreListBoxOfMessages.h"
#include "BookStore/DataUpdateArgs.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/CPageNavigator.h"
#include "Common/CAccountManager.h"
#include "Framework/CDisplay.h"
#include "drivers/DeviceInfo.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;
using namespace dk::account;

UIPersonalMessagesPage::UIPersonalMessagesPage()
    : UIBookStoreListBoxPage(StringManager::GetPCSTRById(PERSONAL_MESSAGE))
{
    SubscribeMessageEvent(BookStoreInfoManager::EventMessagesListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalMessagesPage::OnMessageListBoxUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    SetLeftMargin(0);
}

UIPersonalMessagesPage::~UIPersonalMessagesPage()
{
}

bool UIPersonalMessagesPage::CreateListBoxPanel()
{
    if (NULL == m_pListBoxPanel)
    {
        const int itemCount = GetWindowMetrics(UIPersonalMessagesPageItemCountIndex);
        const int itemHeight = GetWindowMetrics(UIPersonalMessagesPageItemHeightIndex);
        m_pListBoxPanel = new UIBookStoreListBoxOfMessages(itemCount, itemHeight);
        if (m_pListBoxPanel)
        {
#ifdef KINDLE_FOR_TOUCH
            m_pListBoxPanel->SetItemEnableLongClick(true);
#endif
            //m_pListBoxPanel->SetOnceAllItems(true);
        }

    }
    return (NULL != m_pListBoxPanel);
}

FetchDataResult UIPersonalMessagesPage::FetchInfo()
{
    SetFetchStatus(FETCHING);
    FetchDataResult fdr = FDR_FAILED;
    PushedMessagesManager* pPushedMgr = PushedMessagesManager::GetInstance();
    if (pPushedMgr && m_pListBoxPanel)
    {
        const int pageNumber = m_pListBoxPanel->GetNumberPerPage();
        const int startIndex = pageNumber * (m_pListBoxPanel->GetCurrentPage());
        fdr = pPushedMgr->FetchBoxMessageLst(startIndex, pageNumber);
    }
    else
    {
        SetFetchStatus(FETCH_FAIL);
    }
    return fdr;
}

bool UIPersonalMessagesPage::OnMessageListBoxUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_pListBoxPanel)
    {
        bool result = m_pListBoxPanel->OnMessageListBoxUpdate(args);
        SetFetchStatus(result ? (m_pListBoxPanel->GetItemCount() > 0 ? FETCH_SUCCESS : FETCH_NOELEM) : FETCH_FAIL);
        PushedMessagesManager::GetInstance()->OnBoxMessagesUpdate(args);
        return result;
    }
    return false;
}

void UIPersonalMessagesPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
    case ID_BTN_REFRESH:
        if (m_pListBoxPanel)
        {
            m_pListBoxPanel->ResetListBox();
        }
        break;
    default:
        break;
    }
    UIBookStoreListBoxPage::OnCommand(dwCmdId, pSender, dwParam);
}

