#include <tr1/functional>
#include "BookStoreUI/UIBookStoreBookChangeLogPage.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "BookStore/DataUpdateArgs.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIBookStoreBookChangeLogPage::UIBookStoreBookChangeLogPage(const std::string& bookId, const std::string& bookName)
    : UIBookStoreListBoxPage(bookName)
    , m_bookId(bookId)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventBookChangeLogListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreListBoxPage::OnMessageListBoxUpdate),
        this,
        std::tr1::placeholders::_1)
        );
}

UIBookStoreBookChangeLogPage::~UIBookStoreBookChangeLogPage()
{
}

bool UIBookStoreBookChangeLogPage::CreateListBoxPanel()
{
    if (NULL == m_pListBoxPanel)
    {
        m_pListBoxPanel = new UIBookStoreListBoxWithBtn(GetWindowMetrics(UIBookStoreListBoxPageItemCountIndex), 
            GetWindowMetrics(UIBookStoreListBoxPageItemHeightIndex));
        if (m_pListBoxPanel)
        {
            m_pListBoxPanel->SetShowScore(false);
        }
    }
    return (NULL != m_pListBoxPanel);
}

FetchDataResult UIBookStoreBookChangeLogPage::FetchInfo()
{
    SetFetchStatus(FETCHING);
    FetchDataResult fdr = FDR_FAILED;
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (manager && m_pListBoxPanel)
    {
        const int pageNumber = m_pListBoxPanel->GetNumberPerPage();
        const int startIndex = pageNumber * (m_pListBoxPanel->GetCurrentPage());
        fdr = manager->FetchBookChangeLogList(m_bookId.c_str(), startIndex, pageNumber);
    }
    else
    {
        SetFetchStatus(FETCH_FAIL);
    }
    return fdr;
}

