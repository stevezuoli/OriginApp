#include <tr1/functional>
#include "BookStoreUI/UIBookStoreChartsListBoxPage.h"
#include "BookStoreUI/UIBookStoreLatestPage.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "BookStore/DataUpdateArgs.h"
#include "BookStore/BookStoreInfoManager.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
using namespace WindowsMetrics;

UIBookStoreChartsListBoxPage::UIBookStoreChartsListBoxPage(const std::string& title, const ChartsRankType& type)
    : UIBookStoreListBoxPage(title)
    , m_rankType(type)
{
    if (m_rankType >= 0 && m_rankType < CRT_COUNTS)
    {
        const char* EventNames[CRT_COUNTS] = 
        {
            BookStoreInfoManager::EventHotPayListUpdate,
            BookStoreInfoManager::EventHotFreeListUpdate,
            BookStoreInfoManager::EventMonthlyPayListUpdate,
            BookStoreInfoManager::EventFavorPayListUpdate,
            BookStoreInfoManager::EventDouBanRankListUpdate,
            BookStoreInfoManager::EventAmazonRankListUpdate,
            BookStoreInfoManager::EventJingDongRankListUpdate,
            BookStoreInfoManager::EventDangDangRankListUpdate
        };
        SubscribeMessageEvent(EventNames[m_rankType], 
                *BookStoreInfoManager::GetInstance(),
                std::tr1::bind(
                    std::tr1::mem_fn(&UIBookStoreListBoxPage::OnMessageListBoxUpdate),
                    this,
                    std::tr1::placeholders::_1)
                );
    }
}

UIBookStoreChartsListBoxPage::~UIBookStoreChartsListBoxPage()
{
}

bool UIBookStoreChartsListBoxPage::CreateListBoxPanel()
{
    if (NULL == m_pListBoxPanel)
    {
        m_pListBoxPanel = new UIBookStoreListBoxWithBtn(GetWindowMetrics(UIBookStoreListBoxPageItemCountIndex), 
            GetWindowMetrics(UIBookStoreListBoxPageItemHeightIndex));
        if (m_pListBoxPanel)
        {
            m_pListBoxPanel->SetShowIndex(true);
        }
    }
    return (NULL != m_pListBoxPanel);
}

FetchDataResult UIBookStoreChartsListBoxPage::FetchInfo()
{
    SetFetchStatus(FETCHING);
    FetchDataResult fdr = FDR_FAILED;
    if (m_rankType >= 0 && m_rankType < CRT_COUNTS)
    {
        FetchDataResult (BookStoreInfoManager::*fetchFuncs[CRT_COUNTS])(int start, int length) = 
        {
            &BookStoreInfoManager::FetchHotPayBookList,
            &BookStoreInfoManager::FetchHotFreeBookList,
            &BookStoreInfoManager::FetchHotMonthlyBookList,
            &BookStoreInfoManager::FetchHotFavorBookList,
            &BookStoreInfoManager::FetchDouBanRankBookList,
            &BookStoreInfoManager::FetchAmazonRankBookList,
            &BookStoreInfoManager::FetchJingDongRankBookList,
            &BookStoreInfoManager::FetchDangDangRankBookList,
        };
        BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
        if (manager && m_pListBoxPanel)
        {
            const int pageNumber = m_pListBoxPanel->GetNumberPerPage();
            const int startIndex = pageNumber * (m_pListBoxPanel->GetCurrentPage());
            fdr = (manager->*fetchFuncs[m_rankType])(startIndex, pageNumber);
        }
        else
        {
            SetFetchStatus(FETCH_FAIL);
        }
    }
    return fdr;
}

