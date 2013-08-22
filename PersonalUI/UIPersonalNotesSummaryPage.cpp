#include <tr1/functional>
#include "Common/WindowsMetrics.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/BookInfo.h"
#include "BookStore/BookNoteSummary.h"
#include "PersonalUI/UIPersonalNotesSummaryPage.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "I18n/StringManager.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIPersonalNotesSummaryPage::UIPersonalNotesSummaryPage()
    : UIBookStoreListBoxPage(StringManager::GetPCSTRById(PERSONAL_NOTES), false)
    , m_dkBookNotesReady(false)
    , m_localBookNotesReady(false)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventLocalNotesSummaryUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalNotesSummaryPage::OnLocalNotesSummaryUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    SubscribeMessageEvent(BookStoreInfoManager::EventDuoKanNotesSummaryUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalNotesSummaryPage::OnDuoKanNotesSummaryUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    m_fetchInfo.CustomizeNoElementInfo(StringManager::GetPCSTRById(NO_NOTE_TIPS), "");
    pthread_mutex_init(&m_lock, NULL);
}

UIPersonalNotesSummaryPage::~UIPersonalNotesSummaryPage()
{
    pthread_mutex_destroy(&m_lock);
}

bool UIPersonalNotesSummaryPage::OnLocalNotesSummaryUpdate(const EventArgs& args)
{
    if (m_localBookNotesReady && m_pListBoxPanel)
    {
        m_pListBoxPanel->ClearObjectList();
    }
    m_localBookNotesReady = true;
    return OnNotesSummaryUpdate(args);
}

bool UIPersonalNotesSummaryPage::OnDuoKanNotesSummaryUpdate(const EventArgs& args)
{
    if (m_dkBookNotesReady && m_pListBoxPanel)
    {
        m_pListBoxPanel->ClearObjectList();
    }
    m_dkBookNotesReady = true;
    return OnNotesSummaryUpdate(args);
}

bool UIPersonalNotesSummaryPage::OnNotesSummaryUpdate(const EventArgs& args)
{
    pthread_mutex_lock(&m_lock);
    if (m_pListBoxPanel)
    {
        bool result = m_pListBoxPanel->UpdateBookNoteSummary(args);
        if (m_localBookNotesReady && m_dkBookNotesReady)
        {
            if (result)
            {
                m_pListBoxPanel->SortObjectList(model::BookNoteSummary::BookNoteCompare);
                m_pListBoxPanel->UpdateListBox();
                dk::bookstore::model::BasicObjectList objectList = m_pListBoxPanel->GetObjectList();
                int totalNotesCount = 0;
                for (unsigned int i = 0; i < objectList.size(); ++i)
                {
                    dk::bookstore::model::BookNoteSummary* bookNoteSummary = (dk::bookstore::model::BookNoteSummary*)(objectList[i].get());
                    if (bookNoteSummary)
                    {
                        totalNotesCount += bookNoteSummary->GetCommentCount();
                    }
                }
                char title[128];
                snprintf(title, DK_DIM(title), "%s(%s%d%s)", StringManager::GetPCSTRById(PERSONAL_NOTES), 
                        StringManager::GetPCSTRById(BOOK_TOTAL), totalNotesCount, StringManager::GetPCSTRById(BOOK_TIAO));
                m_title.SetText(title);
                SetFetchStatus(objectList.empty() ? FETCH_NOELEM : FETCH_SUCCESS);
            }
            else
            {
                SetFetchStatus(FETCH_FAIL);
            }
        }
        pthread_mutex_unlock(&m_lock);
        return result;
    }
    pthread_mutex_unlock(&m_lock);
    return true;
}

bool UIPersonalNotesSummaryPage::CreateListBoxPanel()
{
    if (NULL == m_pListBoxPanel)
    {
        m_pListBoxPanel = new UIBookStoreListBoxWithBtn(GetWindowMetrics(UIBookStoreListBoxPageItemCountIndex), 
            GetWindowMetrics(UIPersonalNoteSummaryItemHeightIndex));
        if (m_pListBoxPanel)
        {
            m_pListBoxPanel->SetOnceAllItems(true);
        }
    }
    return (NULL != m_pListBoxPanel);
}

FetchDataResult UIPersonalNotesSummaryPage::FetchInfo()
{
    m_dkBookNotesReady = false;
    m_localBookNotesReady = false;
    if (m_pListBoxPanel)
    {
        m_pListBoxPanel->ClearObjectList();
    }
    SetFetchStatus(FETCHING);
    FetchDataResult fdr = FDR_FAILED;
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (manager)
    {
        fdr = manager->FetchAllBookNotesSummary();
    }
    else
    {
        SetFetchStatus(FETCH_FAIL);
    }
    return fdr;
}

