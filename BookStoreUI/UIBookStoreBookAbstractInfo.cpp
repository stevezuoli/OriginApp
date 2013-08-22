#include "BookStoreUI/UIBookStoreBookAbstractInfo.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;

UIBookStoreBookAbstractInfo::UIBookStoreBookAbstractInfo(const std::string& abstractInfo)
    : UITablePanel()
    , m_totalPage(1)
    , m_curPage(0)
{
#ifdef KINDLE_FOR_TOUCH
    m_gestureListener.SetTablePanel(this);
    m_gestureDetector.SetListener(&m_gestureListener);
#endif
    m_abstractText.SetText(abstractInfo.c_str());
    Init();
}

UIBookStoreBookAbstractInfo::~UIBookStoreBookAbstractInfo()
{
}

void UIBookStoreBookAbstractInfo::Init()
{
    const int abstractFontsize = GetWindowFontSize(FontSize20Index);
    const int pageNumFontsize = GetWindowFontSize(FontSize16Index);

    m_abstractText.SetAlign(ALIGN_JUSTIFY);
    m_abstractText.SetFontSize(abstractFontsize);
    m_abstractText.SetLineSpace(1.2);
    m_abstractText.SetVAlign(VALIGN_TOP);
    AppendChild(&m_abstractText);

    m_pageNumber.SetFontSize(pageNumFontsize);
    m_pageNumber.SetEnglishGothic();
    m_pageNumber.SetAlign(ALIGN_RIGHT);
    m_pageNumber.SetVAlign(VALIGN_BOTTOM);
    AppendChild(&m_pageNumber);
}

void UIBookStoreBookAbstractInfo::MoveWindow(int left, int top, int width, int height)
{
    UITablePanel::MoveWindow(left, top, width, height);

    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    if (mainSizer)
    {
        const int margin = GetWindowMetrics(UIPixelValue30Index);
        m_abstractText.SetMaxWidth(m_iWidth);
        mainSizer->Add(&m_abstractText, UISizerFlags(1).Expand().Border(dkTOP, margin));
        m_pageNumber.SetMinHeight(margin);
        mainSizer->Add(&m_pageNumber, UISizerFlags().Expand().Border(dkBOTTOM, GetWindowMetrics(UIPixelValue15Index)));
        SetSizer(mainSizer);
        Layout();
    }
    m_totalPage = m_abstractText.GetPageCount();
    UpdatePageNumber();
}

BOOL UIBookStoreBookAbstractInfo::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    switch (iKeyCode)
    {
    case KEY_LPAGEDOWN:
    case KEY_RPAGEDOWN:
    case KEY_SPACE:
        TurnPage(TRUE);
        break;
    case KEY_LPAGEUP:
    case KEY_RPAGEUP:
        TurnPage(FALSE);
        break;
    default:
        break;
    }
#endif
    return TRUE;
}

bool UIBookStoreBookAbstractInfo::TurnPage(bool downPage)
{
    if (downPage)
    {
        if (m_curPage >= m_totalPage - 1)
        {
            return false;
        }
        m_curPage++;
    }
    else
    {
        if (m_curPage <= 0)
        {
            return false;
        }
        m_curPage--;
    }
    UpdatePageNumber();
    return true;
}

void UIBookStoreBookAbstractInfo::UpdatePageNumber()
{
    char szPageNumber[256] = {0};
    sprintf(szPageNumber, "%d / %d %s", m_curPage + 1, m_totalPage, StringManager::GetPCSTRById(BOOK_PAGE));
    m_pageNumber.SetText(szPageNumber);
    m_abstractText.SetDrawPageNo(m_curPage);
    UpdateWindow();
}

#ifdef KINDLE_FOR_TOUCH
bool UIBookStoreBookAbstractInfo::OnTouchEvent(MoveEvent* moveEvent)
{
    m_gestureDetector.OnTouchEvent(moveEvent);
    return true;
}

bool UIBookStoreBookAbstractInfo::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{
    switch (direction)
    {
    case FD_UP:
    case FD_LEFT:
        TurnPage(true);
        break;
    case FD_DOWN:
    case FD_RIGHT:
        TurnPage(false);
        break;
    default:
        break;
    }
    return true;
}

bool UIBookStoreBookAbstractInfo::OnSingleTapUp(MoveEvent* moveEvent)
{
    double widthPercent  = 0.2;
    int x = moveEvent->GetX();

    if (x>=0 && x<= m_iWidth*widthPercent)
    {
        TurnPage(false);
    }
    else
    {
        TurnPage(true);
    }
    return true;
}
#endif
