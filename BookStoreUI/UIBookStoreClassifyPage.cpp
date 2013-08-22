#include <tr1/functional>
#include "BookStoreUI/UIBookStoreClassifyPage.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "BookStore/DataUpdateArgs.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIBookStoreClassifyPage::UIBookStoreClassifyPage(const std::string& id, const std::string& name)
    : UIBookStoreTablePage(name)
    , m_pBestPanel(NULL)
    , m_pNewPanel(NULL)
    , m_id(id)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventCategoryBookListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreTablePage::OnMessageListBoxUpdate),
        this,
        std::tr1::placeholders::_1)
        );
}

UIBookStoreClassifyPage::~UIBookStoreClassifyPage()
{
}

FetchDataResult UIBookStoreClassifyPage::FetchInfo()
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
            if(pShowPanel == m_pBestPanel)
            {
                fdr = manager->FetchCategoryBookList(m_id.c_str(), startIndex, pageNumber, FDO_DEFAULT);
            }
            else if(pShowPanel == m_pNewPanel)
            {
                fdr = manager->FetchCategoryBookList(m_id.c_str(), startIndex, pageNumber, FDO_LATEST);
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

void UIBookStoreClassifyPage::SetTableController()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const int itemCount = GetWindowMetrics(UIBookStoreTablePageListBoxItemCountIndex);
    const int itemHeight = GetWindowMetrics(UIBookStoreTablePageListBoxItemHeightIndex);
    m_pBestPanel = new UIBookStoreListBoxWithBtn(itemCount, itemHeight);
    if (m_pBestPanel)
    {
        m_tableController.AppendTableBox(ID_BIN_BOOKSTORE_SELLER, 
            StringManager::GetPCSTRById(BOOKSTORE_SELLER),
            m_pBestPanel,
            true
#ifndef KINDLE_FOR_TOUCH
            , 'A'
#endif
            );
    }

    m_pNewPanel = new UIBookStoreListBoxWithBtn(itemCount, itemHeight);
    if (m_pNewPanel)
    {
        m_tableController.AppendTableBox(ID_BIN_BOOKSTORE_NEWARIVALS, 
            StringManager::GetPCSTRById(BOOKSTORE_NEWARIVALS),
            m_pNewPanel
#ifndef KINDLE_FOR_TOUCH
            , false, 'B'
#endif
            );
    }
    AppendChild(&m_tableController);
}

void UIBookStoreClassifyPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
    case ID_BIN_BOOKSTORE_SELLER:
    case ID_BIN_BOOKSTORE_NEWARIVALS:
        FetchInfo();
        break;
    default:
        UIBookStoreTablePage::OnCommand(dwCmdId, pSender, dwParam);
        break;
    }
}

