#include <tr1/functional>
#include "BookStoreUI/UIBookStoreBookInfoRelatedList.h"
#include "BookStoreUI/UIBookDetailedInfoPage.h"
#include "CommonUI/CPageNavigator.h"
#include "drivers/DeviceInfo.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;


UIBookStoreBookInfoRelatedList::UIBookStoreBookInfoRelatedList(int rows, int columns, const CString& label)
    : m_startIndex(0)
    , m_rows(rows)
    , m_columns(columns)
{ 
#ifdef KINDLE_FOR_TOUCH
    const int minWidth = GetWindowMetrics(UIPixelValue30Index);
    m_prevWnd.SetIcon(ImageManager::GetImage(IMAGE_ARROW_STRONG_LEFT), UIButton::ICON_CENTER);
    m_prevWnd.SetId(ID_KEYBOARD_WORDS_PAGEUP);
    m_prevWnd.ShowBorder(false);
    m_nextWnd.SetIcon(ImageManager::GetImage(IMAGE_ARROW_STRONG_RIGHT), UIButton::ICON_CENTER);
    m_nextWnd.SetId(ID_KEYBOARD_WORDS_PAGEDOWN);
    m_nextWnd.ShowBorder(false);
    m_prevWnd.SetMinWidth(minWidth);
    m_nextWnd.SetMinWidth(minWidth);
#else
    m_prevWnd.SetImage(ImageManager::GetImage(IMAGE_ARROW_STRONG_LEFT));
    m_prevWnd.SetVAlign(VALIGN_CENTER);
    m_nextWnd.SetImage(ImageManager::GetImage(IMAGE_ARROW_STRONG_RIGHT));
    m_nextWnd.SetVAlign(VALIGN_CENTER);
#endif
    AppendChild(&m_prevWnd);
    AppendChild(&m_nextWnd);

    m_relatedBookLabel.SetFontSize(GetWindowFontSize(FontSize20Index));
    m_relatedBookLabel.SetText(label);
    AppendChild(&m_relatedBookLabel);

    for (int i = 0; i < RELATED_BOOK_COUNT; i++)
    {
#ifdef KINDLE_FOR_TOUCH
        SubscribeEvent(UIWindow::EventClick,
#else
        SubscribeEvent(UIBookCoverView::EventBookCoverViewTouched,
#endif
            m_relatedBookCoverList[i],
            std::tr1::bind(
            std::tr1::mem_fn(&UIBookStoreBookInfoRelatedList::OnMessageRelatedBookCoverTouch),
            this,
            std::tr1::placeholders::_1)
            );
    }
}

UIBookStoreBookInfoRelatedList::~UIBookStoreBookInfoRelatedList()
{

}

void UIBookStoreBookInfoRelatedList::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
    case ID_KEYBOARD_WORDS_PAGEDOWN:
        TurnPageUpOrDown(false);
        break;
    case ID_KEYBOARD_WORDS_PAGEUP:
        TurnPageUpOrDown(true);
        break;
    default:
        break;
    }
}

bool UIBookStoreBookInfoRelatedList::TurnPageUpOrDown(bool pageUp)
{
    int startIndex = m_startIndex;
    int itemsPerPage = m_rows * m_columns;
    if (pageUp)
    {
        startIndex -= itemsPerPage;
        if (startIndex < 0)
        {
            startIndex = 0;
        }
    }
    else
    {
        const int totalItem = m_relatedList.size();
        startIndex += itemsPerPage;
        if ((startIndex + itemsPerPage) > totalItem)
        {
            startIndex = totalItem - itemsPerPage;
        }
    }
    if (startIndex != m_startIndex)
    {
        m_startIndex = startIndex;
        UpdateRelatedInfo();
        return true;
    }
    return false;
}

void ReverseBookList(dk::bookstore::model::BookInfoList& list)
{
    dk::bookstore::model::BookInfoList copy;
    int size = list.size();
    for (int i = 0; i < size; i++)
    {
        copy.push_back(list.at(size - (i + 1)));
    }
    list = copy;
}

void UIBookStoreBookInfoRelatedList::SetBookInfo(model::BookInfoSPtr bookInfo, bool reverse)
{
    if (bookInfo)
    {
        int itemsPerPage = m_rows * m_columns;
        m_relatedList = bookInfo->GetRelatedBooks();
        if (reverse)
            ReverseBookList(m_relatedList);
        m_startIndex = 0;
        if ((size_t)itemsPerPage > m_relatedList.size())
        {
            itemsPerPage = m_relatedList.size();
        }
        const int fontSize14 = GetWindowFontSize(FontSize14Index);
        const int relatedBookTitleHeight = GetWindowMetrics(UIPixelValue48Index);
        const int relatedBookTitleWidth  = GetWindowMetrics(UIPixelValue100Index);
        const int relatedBooksVerticalGap = GetWindowMetrics(UIPixelValue40Index);
        if (itemsPerPage > 0)
        {
            UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
            UISizer* pListSizer = new UIBoxSizer(dkHORIZONTAL);
            UISizer* pMidSizer = new UIBoxSizer(dkVERTICAL);
            UISizer* pCoverSizer = new UIGridSizer(m_rows, m_columns, relatedBooksVerticalGap, 0);
            if (pMainSizer && pListSizer && pMidSizer && pCoverSizer)
            {
                for (int i = 0; i < itemsPerPage; i++)
                {
                    m_relatedBookCoverList[i].Initialize(SMALL, true);
                    AppendChild(&(m_relatedBookCoverList[i]));

                    m_relatedBookTitleList[i].SetFontSize(fontSize14);
                    m_relatedBookTitleList[i].SetMode(UIAbstractText::TextMultiLine);
                    m_relatedBookTitleList[i].SetAlign(ALIGN_CENTER);
                    AppendChild(&(m_relatedBookTitleList[i]));

                    UISizer* pItemSizer = new UIBoxSizer(dkVERTICAL);
                    m_relatedBookTitleList[i].SetMinSize(relatedBookTitleWidth,relatedBookTitleHeight);
                    pItemSizer->Add(&m_relatedBookCoverList[i], UISizerFlags().Align(dkALIGN_CENTER).ReserveSpaceEvenIfHidden());
                    pItemSizer->Add(&m_relatedBookTitleList[i], UISizerFlags().Align(dkALIGN_CENTER).ReserveSpaceEvenIfHidden());
                    pCoverSizer->Add(pItemSizer, UISizerFlags().Align(dkALIGN_CENTER).ReserveSpaceEvenIfHidden());
                }
                pMidSizer->Add(pCoverSizer, UISizerFlags(1).Expand());

#ifdef KINDLE_FOR_TOUCH
                pListSizer->Add(&m_prevWnd, UISizerFlags().Expand().ReserveSpaceEvenIfHidden());
#else
                //TODO(JUGH):K3/K4ÉÏUIImage ExpandÎÞÐ§
                pListSizer->Add(&m_prevWnd, UISizerFlags().ReserveSpaceEvenIfHidden().Align(dkALIGN_CENTER_VERTICAL).Border(dkBOTTOM, relatedBookTitleHeight));
#endif
                pListSizer->Add(pMidSizer, UISizerFlags(1).Expand());
#ifdef KINDLE_FOR_TOUCH
                pListSizer->Add(&m_nextWnd, UISizerFlags().Expand().ReserveSpaceEvenIfHidden());
#else
                pListSizer->Add(&m_nextWnd, UISizerFlags().ReserveSpaceEvenIfHidden().Align(dkALIGN_CENTER_VERTICAL).Border(dkBOTTOM, relatedBookTitleHeight));
#endif

                pMainSizer->Add(&m_relatedBookLabel, UISizerFlags().Expand().Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIPixelValue22Index)));
                pMainSizer->Add(pListSizer, UISizerFlags(1).Expand());

                SetSizer(pMainSizer);
                Layout();
                UpdateRelatedInfo();
            }
            else
            {
                SafeDeletePointer(pMainSizer);
                SafeDeletePointer(pListSizer);
                SafeDeletePointer(pMidSizer);
                SafeDeletePointer(pCoverSizer);
            }
        }
    }
}

void UIBookStoreBookInfoRelatedList::UpdateRelatedInfo()
{
    int itemsPerPage = m_rows * m_columns;
    for (int i = 0; i < itemsPerPage; i++)
    {
        const size_t index = i + m_startIndex;
        if (index >= 0 && index < m_relatedList.size())
        {
            model::BookInfoSPtr bookInfo = m_relatedList.at(i + m_startIndex);
            if (bookInfo)
            {
                m_relatedBookCoverList[i].SetCoverUrl(bookInfo->GetCoverUrl());
                m_relatedBookTitleList[i].SetText(bookInfo->GetTitle().c_str());
            }
        }
    }
    m_prevWnd.SetVisible(m_startIndex > 0);
    m_nextWnd.SetVisible((m_startIndex + itemsPerPage) < m_relatedList.size());
    Repaint();
}

bool UIBookStoreBookInfoRelatedList::OnMessageRelatedBookCoverTouch(const EventArgs& args)
{
#ifdef KINDLE_FOR_TOUCH
    const ClickArgs& clickArgs = (const ClickArgs&)(args);
    UIBookCoverView* coverView = (UIBookCoverView*)clickArgs.window;
    if (NULL == coverView)
    {
        return false;
    }
    const string coverUrl = coverView->GetCoverUrl();
#else
    const BookCoverViewTouchedEventArgs& msgArgs = dynamic_cast<const BookCoverViewTouchedEventArgs&>(args);
    const string coverUrl = msgArgs.m_coverUrl;
#endif
    int itemsPerPage = m_rows * m_columns;
    for (int i = 0;i < itemsPerPage; i++)
    {
        model::BookInfoSPtr bookInfo = m_relatedList[i + m_startIndex];
        if((m_relatedBookCoverList[i].GetCoverUrl() == coverUrl) && bookInfo)
        {

            UIBookDetailedInfoPage* pPage = new UIBookDetailedInfoPage(bookInfo->GetBookId(), bookInfo->GetTitle());
            if (pPage)
            {
                pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                CPageNavigator::Goto(pPage);
                return true;
            }
            return false;
        }
    }
    return false;
}

BOOL UIBookStoreBookInfoRelatedList::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    int itemsPerPage = m_rows * m_columns;
    switch (iKeyCode)
    {
    case KEY_LPAGEDOWN:
    case KEY_RPAGEDOWN:
    case KEY_SPACE:
        TurnPageUpOrDown(false);
        break;
    case KEY_LPAGEUP:
    case KEY_RPAGEUP:
        TurnPageUpOrDown(true);
        break;
    case KEY_LEFT:
        if (itemsPerPage > 0)
        {
            if (m_relatedBookCoverList[0].IsFocus())
            {
                if (TurnPageUpOrDown(true))
                {
                    m_relatedBookCoverList[itemsPerPage - 1].SetFocus(true);
                }
                else
                {
                    return FALSE;
                }
            }
            else
            {
                MoveFocus(DIR_LEFT);
            }
        }
        break;
    case KEY_RIGHT:
        if (itemsPerPage > 0)
        {
            if (m_relatedBookCoverList[itemsPerPage - 1].IsFocus())
            {
                if (TurnPageUpOrDown(false))
                {
                    m_relatedBookCoverList[0].SetFocus(true);
                }
                else
                {
                    return FALSE;
                }
            }
            else
            {
                MoveFocus(DIR_RIGHT);
            }
        }
        break;
    case KEY_UP:
    case KEY_DOWN:
        return TRUE;
    default:
        if(SendHotKeyToChildren(iKeyCode))
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s default", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return UIContainer::OnKeyPress(iKeyCode);
        }
        break;
    }
#endif
    return FALSE;
}

void UIBookStoreBookInfoRelatedList::SetFocus(BOOL bIsFocus)
{
    if (m_bIsFocus != bIsFocus)
    {
        UIContainer::SetFocus(bIsFocus);
        m_bIsFocus = bIsFocus;
        if ((m_iFocusedChild >= 0) && ((size_t)m_iFocusedChild < GetChildrenCount()))
        {
            UIWindow *pItem = GetChildByIndex(m_iFocusedChild);
            if (pItem)
            {
                pItem->SetFocus(m_bIsFocus);
            }
        }
    }
}

HRESULT UIBookStoreBookInfoRelatedList::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    int itemsPerPage = m_rows * m_columns;
    if (itemsPerPage > 0)
    {
        CTpGraphics grf(drawingImg);
        const int lineHeight = 1;
        grf.DrawLine(0, 0, m_iWidth, lineHeight, SOLID_STROKE);
    }
    return hr;
}
