#include <tr1/functional>
#include "BookStoreUI/UIBookStoreTagListPage.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "BookStore/DataUpdateArgs.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIBookStoreTagListPage::UIBookStoreTagListPage(const char* tagName)
    : UIBookStoreListBoxPage(tagName)
    , m_tagName(tagName)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventTagListMessagesUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreListBoxPage::OnMessageListBoxUpdate),
        this,
        std::tr1::placeholders::_1)
        );
}

UIBookStoreTagListPage::~UIBookStoreTagListPage()
{
}

bool UIBookStoreTagListPage::CreateListBoxPanel()
{
    if (NULL == m_pListBoxPanel)
    {
        m_pListBoxPanel = new UIBookStoreListBoxWithBtn(GetWindowMetrics(UIBookStoreListBoxPageItemCountIndex), 
            GetWindowMetrics(UIBookStoreListBoxPageItemHeightIndex));
    }
    return (NULL != m_pListBoxPanel);
}

FetchDataResult UIBookStoreTagListPage::FetchInfo()
{
    SetFetchStatus(FETCHING);
    FetchDataResult fdr = FDR_FAILED;
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (manager && m_pListBoxPanel)
    {
        const int pageNumber = m_pListBoxPanel->GetNumberPerPage();
        const int startIndex = pageNumber * (m_pListBoxPanel->GetCurrentPage());
        fdr = manager->FetchTagList(m_tagName.c_str(), startIndex, pageNumber);
    }
    else
    {
        SetFetchStatus(FETCH_FAIL);
    }
    return fdr;
}

