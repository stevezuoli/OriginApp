#include <tr1/functional>
#include "BookStoreUI/UIBookStoreCategoryPage.h"
#include "BookStoreUI/UIBookStoreClassifyPage.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "CommonUI/CPageNavigator.h"
#include "Framework/INativeMessageQueue.h"
#include "Framework/CHourglass.h"
#include "Framework/CDisplay.h"
#include "BookStore/BookStoreInfoManager.h"
#include "DownloadManager/DownloadManager.h"
#include "BookStore/DataUpdateArgs.h"
#include "I18n/StringManager.h"
#include "Utility/ImageManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/MessageEventArgs.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace std;
using namespace WindowsMetrics;

UIBookStoreCategoryPage::UIBookStoreCategoryPage()
    : UIBookStoreListBoxPage(StringManager::GetPCSTRById(BOOKSTORE_CATEGORYVIEW))
{
    SubscribeMessageEvent(BookStoreInfoManager::EventCategoryListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreListBoxPage::OnMessageListBoxUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreInfoManager::EventCategoryTreeUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreCategoryPage::OnCategoryTreeUpdate),
        this,
        std::tr1::placeholders::_1)
        );
}

UIBookStoreCategoryPage::~UIBookStoreCategoryPage()
{
}

bool UIBookStoreCategoryPage::OnCategoryTreeUpdate(const EventArgs& args)
{
    if (m_categoryId.empty())
    {
        return false;
    }

    const CategoryTreeArgs categoryTreeArgs = (const CategoryTreeArgs&)(args);
    DataUpdateArgs listDataUpdateArgs;
    listDataUpdateArgs.succeeded = true;
    categoryTreeArgs.GetChildrenCategoryOfId(m_categoryId.c_str(), &listDataUpdateArgs.dataList);

    return OnMessageListBoxUpdate(listDataUpdateArgs);
}

bool UIBookStoreCategoryPage::CreateListBoxPanel()
{
    if (NULL == m_pListBoxPanel)
    {
        m_pListBoxPanel = new UIBookStoreListBoxWithBtn(GetWindowMetrics(UIBookStoreListBoxPageItemCountIndex), 
            GetWindowMetrics(UIBookStoreListBoxPageItemHeightIndex));
        if (m_pListBoxPanel)
        {
            m_pListBoxPanel->SetOnceAllItems(true);
        }
    }
    return (NULL != m_pListBoxPanel);
}

FetchDataResult UIBookStoreCategoryPage::FetchInfo()
{
    SetFetchStatus(FETCHING);
    FetchDataResult fdr = FDR_FAILED;
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (manager)
    {
        if (!m_categoryId.empty())
        {
            fdr = manager->FetchCategoryTree();
        }
        else
        {
            fdr = manager->FetchCategoryList();
        }
    }
    else
    {
        SetFetchStatus(FETCH_FAIL);
    }
    return fdr;
}

