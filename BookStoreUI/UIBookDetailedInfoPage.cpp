#include <tr1/functional>
#include "BookStoreUI/UIBookDetailedInfoPage.h"
#include "BookStoreUI/UIBookStoreBookInfo.h"
#include "BookStoreUI/UIBookStoreListBoxWithPageNum.h"
#include "BookStoreUI/UIBookStoreBookComment.h"
#include "BookStoreUI/UIBookStoreBookRightsInfo.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

static const int LISTBOX_ITEMCOUNT = 12;

UIBookDetailedInfoPage::UIBookDetailedInfoPage(const std::string& bookId,const std::string& bookName, const std::string& duokanKey)
    : UIBookStoreTablePage(bookName, false)
    , m_bookId(bookId)
    , m_bookName(bookName)
    , m_duokanKey(duokanKey)
    , m_pBookinfo(NULL)
    , m_pBookRights(NULL)
    , m_pBookCatalogue(NULL)
    , m_pBookComment(NULL)
    , m_resultInfo(false)
    , m_resultCatalogue(false)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventBookInfoUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookDetailedInfoPage::OnMessageFetchInfoUpdate),
        this,
        std::tr1::placeholders::_1)
        );
}

UIBookDetailedInfoPage::~UIBookDetailedInfoPage()
{
    m_pBookinfo = NULL;
    m_pBookCatalogue = NULL;
    m_pBookComment = NULL;
}

void UIBookDetailedInfoPage::SetTableController()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    m_pBookinfo = new UIBookStoreBookInfo(this, m_bookId, m_bookName, m_duokanKey);
    if (m_pBookinfo)
    {
        m_tableController.AppendTableBox(ID_TABLEBOX_BOOKINFO, 
            StringManager::GetPCSTRById(BOOK_INFO),
            m_pBookinfo,
            true
#ifndef KINDLE_FOR_TOUCH
            , 'A'
#endif
            );
    }

    m_pBookRights = new UIBookStoreBookRightsInfo(m_bookId);
    if (m_pBookRights)
    {
        m_tableController.AppendTableBox(ID_TABLEBOX_BOOKRIGHTS, 
            StringManager::GetPCSTRById(BOOKSTORE_BOOKCOPYRIGHTS),
            m_pBookRights,
            false
#ifndef KINDLE_FOR_TOUCH
            , 'B'
#endif
            );
    }

    m_pBookCatalogue = new UIBookStoreListBoxWithPageNum(LISTBOX_ITEMCOUNT,  GetWindowMetrics(UIBookStoreBookCatalogueItemHeightIndex));
    if (m_pBookCatalogue)
    {
        m_pBookCatalogue->SetOnceAllItems(true);
        m_tableController.AppendTableBox(ID_TABLEBOX_BOOKCATALOGUE, 
            StringManager::GetPCSTRById(TOUCH_BOOKSETTING_TOC),
            m_pBookCatalogue
#ifndef KINDLE_FOR_TOUCH
            , false, 'C'
#endif
            );
    }
    
    m_pBookComment = new UIBookStoreBookComment(m_bookId, m_bookName);
    if (m_pBookComment)
    {
        m_tableController.AppendTableBox(ID_TABLEBOX_BOOKREVIEW, 
            StringManager::GetPCSTRById(BOOKSTORE_BOOKREVIEW),
            m_pBookComment
#ifndef KINDLE_FOR_TOUCH
            , false, 'D'
#endif
            );

        SubscribeEvent(UITablePanel::EventTablePanelChange, 
            *m_pBookComment,
            std::tr1::bind(
            std::tr1::mem_fn(&UIBookDetailedInfoPage::OnMessagePanelInfoUpdate),
            this,
            std::tr1::placeholders::_1)
            ); 
    }
    AppendChild(&m_tableController);
}

FetchDataResult UIBookDetailedInfoPage::FetchInfo()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    SetFetchStatus(FETCHING);
    UpdateWindow();

    UITablePanel* pShowPanel = m_tableController.GetShowPanel();
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    FetchDataResult fdr = FDR_FAILED;
    if (pShowPanel && manager)
    {
        UITablePanel* pPrePanel = m_pCurPanel;
        m_pCurPanel = pShowPanel;
        if(pShowPanel == m_pBookComment)
        {
            fdr = m_pBookComment->FetchInfo();
        }
        else
        {
            if (pPrePanel == m_pBookComment)
            {
                fdr = manager->FetchBookInfo(m_bookId.c_str());
            }
            else if (pShowPanel == m_pBookinfo)
            {
                if (m_resultInfo)
                {
                    SetFetchStatus(FETCH_SUCCESS);
                }
                else if (NULL == pPrePanel)
                {
                    fdr = manager->FetchBookInfo(m_bookId.c_str());
                }
            }
            else if (pShowPanel == m_pBookCatalogue)
            {
                if (m_resultCatalogue)
                {
                    SetFetchStatus((m_pBookCatalogue->GetItemCount() > 0) ? FETCH_SUCCESS : FETCH_NOELEM);
                }
                else if (NULL == pPrePanel)
                {
                    fdr = manager->FetchBookInfo(m_bookId.c_str());
                }
            }
            else if (pShowPanel == m_pBookRights)
            {
                if (m_resultRights)
                {
                    SetFetchStatus(FETCH_SUCCESS);
                }
                else if (NULL == pPrePanel)
                {
                    fdr = manager->FetchBookInfo(m_bookId.c_str());
                }
            }
            else
            {
                SetFetchStatus(FETCH_FAIL);
            }
        }
    }
    else
    {
        SetFetchStatus(FETCH_FAIL);
    }
    return fdr;
}

bool UIBookDetailedInfoPage::OnMessageFetchInfoUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UITablePanel* pShowPanel = (UITablePanel*)m_tableController.GetShowPanel();
    bool result = false;
    if((pShowPanel == m_pBookinfo) || (pShowPanel == m_pBookCatalogue) || (pShowPanel == m_pBookRights))
    {
        m_resultInfo = m_pBookinfo->OnMessageBookDetailedInfoUpdate(args);
        if (m_resultInfo)
        {
            m_bookName = m_pBookinfo->GetBookTitle();
            m_title.SetText(m_bookName.c_str());
        }
        result = m_resultInfo;

        m_resultCatalogue = m_pBookCatalogue->OnMessageBookCatalogueUpdate(args);
        if (pShowPanel == m_pBookinfo)
        {
            SetFetchStatus(m_resultInfo ? FETCH_SUCCESS : FETCH_FAIL);
        }
        else if (pShowPanel == m_pBookRights)
        {
            SetFetchStatus(m_resultRights ? FETCH_SUCCESS : FETCH_FAIL);
            result = m_resultRights;
        }
        else
        {
            SetFetchStatus(m_resultCatalogue ? (m_pBookCatalogue->GetItemCount() > 0 ? FETCH_SUCCESS : FETCH_NOELEM) : FETCH_FAIL);
            result = m_resultCatalogue;
        }

        m_resultRights = m_pBookRights->OnMessageBookDetailedInfoUpdate(args);
        if (pShowPanel == m_pBookinfo)
        {
            SetFetchStatus(m_resultInfo ? FETCH_SUCCESS : FETCH_FAIL);
        }
        else if (pShowPanel == m_pBookCatalogue)
        {
            SetFetchStatus(m_resultCatalogue ? FETCH_SUCCESS : FETCH_FAIL);
            result = m_resultCatalogue;
        }
        else
        {
            SetFetchStatus(m_resultRights ? FETCH_SUCCESS : FETCH_FAIL);
            result = m_resultRights;
        }
    }
    return result;
}

void UIBookDetailedInfoPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch(dwCmdId)
    {
    case ID_BTN_REFRESH:
        if(UIUtility::CheckAndReConnectWifi())
        {
            m_pCurPanel = NULL;
            m_resultInfo = false;
            FetchInfo();
        }
        break;
    case ID_TABLEBOX_BOOKINFO:
    case ID_TABLEBOX_BOOKCATALOGUE:
    case ID_TABLEBOX_BOOKREVIEW:
    case ID_TABLEBOX_BOOKRIGHTS:
        if(UIUtility::CheckAndReConnectWifi())
        {
            FetchInfo();
        }
        break;
    default:
        UIBookStoreTablePage::OnCommand(dwCmdId, pSender, dwParam);
        break;
    }
}
