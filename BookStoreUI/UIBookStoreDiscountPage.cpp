#include <tr1/functional>
#include "BookStoreUI/UIBookStoreDiscountPage.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "BookStore/DataUpdateArgs.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIBookStoreDiscountPage::UIBookStoreDiscountPage()
    : UIBookStoreTablePage(StringManager::GetPCSTRById(TOUCH_DISCOUNT))
    , m_pDiscountPanel(NULL)
    , m_pFreePanel(NULL)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventDiscountListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreDiscountPage::OnMessageDiscountListUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreInfoManager::EventFreeListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreDiscountPage::OnMessageFreeListUpdate),
        this,
        std::tr1::placeholders::_1)
        );
}

UIBookStoreDiscountPage::~UIBookStoreDiscountPage()
{
}

FetchDataResult UIBookStoreDiscountPage::FetchInfo()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
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
            if(pShowPanel == m_pDiscountPanel)
            {
                fdr = manager->FetchDiscountBookList(startIndex, pageNumber);
            }
            else if(pShowPanel == m_pFreePanel)
            {
                fdr = manager->FetchFreeBookList(startIndex, pageNumber);
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

void UIBookStoreDiscountPage::SetTableController()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const int itemCount = GetWindowMetrics(UIBookStoreTablePageListBoxItemCountIndex);
    const int itemHeight = GetWindowMetrics(UIBookStoreTablePageListBoxItemHeightIndex);
    m_pDiscountPanel = new UIBookStoreListBoxWithBtn(itemCount, itemHeight);
    if (m_pDiscountPanel)
    {
        m_pDiscountPanel->SetShowScore(false);
        m_tableController.AppendTableBox(ID_BIN_BOOKSTORE_DISCOUNT_SORT, 
            StringManager::GetPCSTRById(TOUCH_TODAYDISCOUNT),
            m_pDiscountPanel,
            true
#ifndef KINDLE_FOR_TOUCH
            , 'A'
#endif
            );
    }

    m_pFreePanel = new UIBookStoreListBoxWithBtn(itemCount, itemHeight);
    if (m_pFreePanel)
    {
        m_pFreePanel->SetShowScore(false);
        m_tableController.AppendTableBox(ID_BIN_BOOKSTORE_FREE_SORT, 
            StringManager::GetPCSTRById(TOUCH_FREEZONE),
            m_pFreePanel
#ifndef KINDLE_FOR_TOUCH
            , false, 'B'
#endif
            );
    }
    AppendChild(&m_tableController);
}

void UIBookStoreDiscountPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
    case ID_BIN_BOOKSTORE_DISCOUNT_SORT:
    case ID_BIN_BOOKSTORE_FREE_SORT:
        FetchInfo();
        break;
    default:
        UIBookStoreTablePage::OnCommand(dwCmdId, pSender, dwParam);
        break;
    }
}

bool UIBookStoreDiscountPage::OnMessageDiscountListUpdate(const EventArgs& args)
{
    if (m_pCurPanel != m_pDiscountPanel)
    {
        return false;
    }
    return UIBookStoreTablePage::OnMessageListBoxUpdate(args);
}

bool UIBookStoreDiscountPage::OnMessageFreeListUpdate(const EventArgs& args)
{
    if (m_pCurPanel != m_pFreePanel)
    {
        return false;
    }
    return UIBookStoreTablePage::OnMessageListBoxUpdate(args);
}

