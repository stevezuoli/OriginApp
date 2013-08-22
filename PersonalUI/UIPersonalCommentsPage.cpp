#include <tr1/functional>
#include "PersonalUI/UIPersonalCommentsPage.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "I18n/StringManager.h"
#include "GUI/MessageEventArgs.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIPersonalCommentsPage::UIPersonalCommentsPage()
    : UIBookStoreTablePage(StringManager::GetPCSTRById(PERSONAL_COMMENTSSQUARE), false)
    , m_pHotCommentsPanel(NULL)
    , m_pOwnCommentsPanel(NULL)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventHotCommentListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalCommentsPage::OnMessageHotCommentsListUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreInfoManager::EventOwnCommentListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalCommentsPage::OnMessageOwnCommentsListUpdate),
        this,
        std::tr1::placeholders::_1)
        );

}

UIPersonalCommentsPage::~UIPersonalCommentsPage()
{
}

FetchDataResult UIPersonalCommentsPage::FetchInfo()
{
    if (FDR_SUCCESSED != UIBookStoreTablePage::FetchInfo())
    {
        UIBookStoreListBoxPanel* pShowPanel = (UIBookStoreListBoxPanel*)m_tableController.GetShowPanel();
        BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
        FetchDataResult fdr = FDR_FAILED;
        if (pShowPanel && manager)
        {
            m_pCurPanel = pShowPanel;
            const int pageNumber = pShowPanel->GetNumberPerPage();
            const int startIndex = pageNumber * (pShowPanel->GetCurrentPage());
            if (pShowPanel == m_pHotCommentsPanel)
            {
                fdr = manager->FetchHotCommentsList(startIndex, pageNumber);
            }
            else if (pShowPanel == m_pOwnCommentsPanel)
            {
                fdr = manager->FetchOwnCommentsList(startIndex, pageNumber);
            }
        }
        else
        {
            SetFetchStatus(FETCH_FAIL);
        }

        return fdr;
    }

    return FDR_SUCCESSED;
}

void UIPersonalCommentsPage::SetTableController()
{
    const int itemCount = GetWindowMetrics(UIPersonalCommentsPageItemCountIndex);
    const int itemHeight = GetWindowMetrics(UIPersonalCommentsPageItemHeightIndex);
    m_pHotCommentsPanel = new UIBookStoreListBoxWithBtn(itemCount, itemHeight);
    if (m_pHotCommentsPanel)
    {
        m_tableController.AppendTableBox(ID_BTN_PERSONAL_HOT_COMMENT,
                StringManager::GetPCSTRById(PERSONAL_HOTCOMMENTS),
                m_pHotCommentsPanel,
                true
#ifndef KINDLE_FOR_TOUCH
                , 'A'
#endif
                );
    }

    m_pOwnCommentsPanel = new UIBookStoreListBoxWithBtn(itemCount, itemHeight);
    if (m_pOwnCommentsPanel)
    {
        m_tableController.AppendTableBox(ID_BTN_PERSONAL_OWN_COMMENT,
                StringManager::GetPCSTRById(PERSONAL_OWNCOMMENTS),
                m_pOwnCommentsPanel
#ifndef KINDLE_FOR_TOUCH
                , false, 'B'
#endif
                );
    }

    AppendChild(&m_tableController);
}

void UIPersonalCommentsPage::OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
        case ID_BTN_PERSONAL_HOT_COMMENT:
        case ID_BTN_PERSONAL_OWN_COMMENT:
            FetchInfo();
            break;
        default:
            UIBookStoreTablePage::OnCommand(dwCmdId, pSender, dwParam);
    }
}

bool UIPersonalCommentsPage::OnMessageHotCommentsListUpdate(const EventArgs& args)
{
    if (m_pCurPanel != m_pHotCommentsPanel)
    {
        return false;
    }

    return UIBookStoreTablePage::OnMessageListBoxUpdate(args);
}

bool UIPersonalCommentsPage::OnMessageOwnCommentsListUpdate(const EventArgs& args)
{
    if (m_pCurPanel != m_pOwnCommentsPanel)
    {
        return false;
    }

    return UIBookStoreTablePage::OnMessageListBoxUpdate(args);
}

#ifndef KINDLE_FOR_TOUCH
BOOL UIPersonalCommentsPage::OnKeyPress(INT32 iKeyCode)
{
    return UIBookStoreTablePage::OnKeyPress(iKeyCode);
}
#endif

