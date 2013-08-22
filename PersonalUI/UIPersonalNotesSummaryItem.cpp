#include <tr1/functional>
#include "PersonalUI/UIPersonalNotesSummaryItem.h"
#include "PersonalUI/UIPersonalBookNotesPage.h"
#include "BookStoreUI/UIBookDetailedInfoPage.h"
#include "CommonUI/CPageNavigator.h"
#include "drivers/DeviceInfo.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include "../Common/FileManager_DK.h"
#include "Framework/CNativeThread.h"
#include "GUI/UIMessageBox.h"
#ifdef KINDLE_FOR_TOUCH
#include "TouchAppUI/BookOpenManager.h"
#include "GUI/GUISystem.h"
#else
#include "AppUI/BookOpenManager.h"
#endif

using namespace WindowsMetrics;
using namespace dk::bookstore;

UIPersonalNotesSummaryItem::UIPersonalNotesSummaryItem()
{
#ifdef KINDLE_FOR_TOUCH
    SubscribeEvent(UIWindow::EventClick,
            m_bookCover,
            std::tr1::bind(
                std::tr1::mem_fn(&UIPersonalNotesSummaryItem::OnMessageBookCoverTouch),
                this,
                std::tr1::placeholders::_1)
            );
#endif
    Init();
}

UIPersonalNotesSummaryItem::~UIPersonalNotesSummaryItem()
{
}

void UIPersonalNotesSummaryItem::Init()
{
    m_labelBookTitle.SetFontSize(GetWindowFontSize(UIListItemBaseTitleFontSizeIndex));
    m_labelBookAuthor.SetFontSize(GetWindowFontSize(UIListItemBaseMidFontSizeIndex));
    m_labelLastUpdateTime.SetFontSize(GetWindowFontSize(UIListItemBaseSmallFontSizeIndex));
    m_labelNotesCount.SetFontSize(GetWindowFontSize(UIListItemBaseSmallFontSizeIndex));
    m_bookCover.Initialize(CUSTOMIZED, IMAGE_ICON_COVER_BACKGROUND, "");
    AppendChild(&m_bookCover);
    AppendChild(&m_labelBookTitle);
    AppendChild(&m_labelBookAuthor);
    AppendChild(&m_labelLastUpdateTime);
    AppendChild(&m_labelNotesCount);
#ifndef KINDLE_FOR_TOUCH
    m_bookCover.SetEnable(false);
#endif

    InitUI();
}

void UIPersonalNotesSummaryItem::InitUI()
{
    UISizer* mainSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* middleSizer = new UIBoxSizer(dkVERTICAL);
    const int elementBorder = GetWindowMetrics(UIPersonalNoteSummaryItemBorderIndex);
    if (mainSizer)
    {
        mainSizer->Add(&m_bookCover, UISizerFlags().Center());
        mainSizer->AddSpacer(GetWindowMetrics(UIPixelValue15Index));
        if (middleSizer)
        {
            middleSizer->Add(&m_labelBookTitle, UISizerFlags().Border(dkBOTTOM, elementBorder));
            middleSizer->Add(&m_labelBookAuthor, UISizerFlags().Border(dkBOTTOM, elementBorder));
            middleSizer->Add(&m_labelLastUpdateTime);
            mainSizer->Add(middleSizer, UISizerFlags(1).Center());
        }
        mainSizer->Add(&m_labelNotesCount, UISizerFlags().Center());

        SetSizer(mainSizer);
    }
}

void UIPersonalNotesSummaryItem::SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr bookNoteSummary, int index)
{
    m_bookNoteSummary.reset((model::BookNoteSummary*)bookNoteSummary.get()->Clone());
    if (m_bookNoteSummary)
    {
        m_bookCover.SetCoverUrl(m_bookNoteSummary->GetBookCoverUrl());
        if (m_bookNoteSummary->GetBookCoverUrl().empty())
        {
            m_bookCover.SetBookTitle(m_bookNoteSummary->GetBookTitle().c_str());
        }
        m_labelBookTitle.SetText(m_bookNoteSummary->GetBookTitle());
        char buf[128] = {0};
        snprintf(buf, DK_DIM(buf), "%d%s", m_bookNoteSummary->GetCommentCount(), StringManager::GetPCSTRById(BOOK_TIAO));
        m_labelNotesCount.SetText(buf);
        m_labelBookAuthor.SetVisible(!m_bookNoteSummary->GetBookAuthor().empty());
        if (!m_bookNoteSummary->GetBookAuthor().empty())
        {
            snprintf(buf, DK_DIM(buf), "%s %s", StringManager::GetPCSTRById(BOOK_AUTHOR), m_bookNoteSummary->GetBookAuthor().c_str());
            m_labelBookAuthor.SetText(buf);
        }
        string updateTime = m_bookNoteSummary->GetLastUpdateTime();
        size_t startIndex = updateTime.find_first_of('-');
        size_t endIndex = updateTime.find_first_of(' ');
        if (endIndex - startIndex > 1)
        {
            snprintf(buf, DK_DIM(buf), "%s: %s", StringManager::GetPCSTRById(LAST_UPDATE), updateTime.substr(startIndex + 1, endIndex - startIndex - 1).c_str());
            m_labelLastUpdateTime.SetText(buf);
        }
#ifndef KINDLE_FOR_TOUCH
        Layout();
#endif
    }
}

bool UIPersonalNotesSummaryItem::OpenBookFromCover()
{
    PCDKFile fileInfo = CDKFileManager::GetFileManager()->GetFileByBookId(m_bookNoteSummary->GetBookId().c_str());
    if(!fileInfo)
    {
        return false;
    }
    int iFileId = fileInfo->GetFileID();
#ifdef KINDLE_FOR_TOUCH
    BookOpenManager::GetInstance()->Initialize(
            GUISystem::GetInstance()->GetTopFullScreenContainer(),
            iFileId, 
            10, 
            0);

    SNativeMessage msg;
    msg.iType = KMessageOpenBook;
    CNativeThread::Send(msg);
#else
    BookOpenManager bookOpen(GetParent(), iFileId, 10, 10);
    if(!bookOpen.OpenBook())
    {
       UIMessageBox messagebox(GetParent(), StringManager::GetStringById(CANNOT_OPEN_BOOK), MB_OK);
       messagebox.DoModal();
    }
#endif

    return true;
}

bool UIPersonalNotesSummaryItem::ResponseTouchTap()
{
    if (!m_bookNoteSummary)
    {
        return true;
    }

    ShowBookNotePage();
    return true;
}

bool UIPersonalNotesSummaryItem::ResponseOperation()
{
    if (!m_bookNoteSummary)
    {
        return true;
    }

    if (OpenBookFromCover())
    {
        return true;
    }
    ShowBookDetailedInfoPage();
    return true;
}

#ifdef KINDLE_FOR_TOUCH
bool UIPersonalNotesSummaryItem::OnMessageBookCoverTouch(const EventArgs& args)
{
    if (!m_bookNoteSummary)
    {
        return true;
    }

    if (OpenBookFromCover())
    {
        return true;
    }

    ShowBookDetailedInfoPage();
    return true;
}

#else
#endif

void UIPersonalNotesSummaryItem::ShowBookDetailedInfoPage()
{
    if (m_bookNoteSummary)
    {
        if (m_bookNoteSummary->IsDuoKanBook())
        {
            UIPage* pPage = new UIBookDetailedInfoPage(m_bookNoteSummary->GetBookId(), m_bookNoteSummary->GetBookTitle());
            if(pPage)
            {
                pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                CPageNavigator::Goto(pPage);
            }
        }
        else
        {
            ShowBookNotePage();
        }
    }
}

void UIPersonalNotesSummaryItem::ShowBookNotePage()
{
    UIPage* pPage = new UIPersonalBookNotesPage(m_bookNoteSummary->GetBookId(),
                                                m_bookNoteSummary->GetBookTitle(),
                                                m_bookNoteSummary->GetBookAuthor(),
                                                m_bookNoteSummary->GetBookCoverUrl(),
                                                m_bookNoteSummary->GetLastUpdateTime(),
                                                m_bookNoteSummary->GetCommentCount());
    if (pPage)
    {
        pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
        CPageNavigator::Goto(pPage);
    }
}
