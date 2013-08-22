#include <tr1/functional>
#include "BookStoreUI/UIBookStoreDuokanRecommendPage.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "I18n/StringManager.h"
#include "GUI/MessageEventArgs.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIBookStoreDuokanRecommendPage::UIBookStoreDuokanRecommendPage()
    : UIBookStoreListBoxPage(StringManager::GetPCSTRById(BOOKSTORE_RECOMMEND))
{
    SubscribeMessageEvent(BookStoreInfoManager::EventRecommendListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&UIBookStoreListBoxPage::OnMessageListBoxUpdate),
            this,
            std::tr1::placeholders::_1)
        );
}

UIBookStoreDuokanRecommendPage::~UIBookStoreDuokanRecommendPage()
{
}

bool UIBookStoreDuokanRecommendPage::CreateListBoxPanel()
{
    if (NULL == m_pListBoxPanel)
    {
        m_pListBoxPanel = new UIBookStoreListBoxWithBtn(GetWindowMetrics(UIBookStoreListBoxPageItemCountIndex), 
            GetWindowMetrics(UIBookStoreListBoxPageItemHeightIndex));
    }
    return (NULL != m_pListBoxPanel);
}

FetchDataResult UIBookStoreDuokanRecommendPage::FetchInfo()
{
    SetFetchStatus(FETCHING);

    FetchDataResult fdr = FDR_FAILED;
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (manager && m_pListBoxPanel)
    {
        const int pageNumber = m_pListBoxPanel->GetNumberPerPage();
        const int startIndex = pageNumber * (m_pListBoxPanel->GetCurrentPage());
        fdr = manager->FetchRecommendList(startIndex, pageNumber);
    }
    else
    {
        SetFetchStatus(FETCH_FAIL);
    }
    return fdr;
}

