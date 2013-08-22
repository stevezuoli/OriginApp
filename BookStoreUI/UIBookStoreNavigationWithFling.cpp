#include "BookStoreUI/UIBookStoreNavigationWithFling.h"

UIBookStoreNavigationWithFling::UIBookStoreNavigationWithFling(const std::string& title, bool hasBottomBar, bool showFetchInfo, bool showRefresh)
    : UIBookStoreNavigation(title, hasBottomBar, showFetchInfo, showRefresh)
    , m_totalPage(1)
    , m_curPage(0)
{
#ifdef KINDLE_FOR_TOUCH
    m_gestureListener.SetNavigationPage(this);
    m_gestureDetector.SetListener(&m_gestureListener);
#endif
}

UIBookStoreNavigationWithFling::~UIBookStoreNavigationWithFling()
{
}

BOOL UIBookStoreNavigationWithFling::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    if (m_mnuMain.IsVisible())
    {
        return UIBookStoreNavigation::OnKeyPress(iKeyCode);
    }
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
    return UIBookStoreNavigation::OnKeyPress(iKeyCode);
}

bool UIBookStoreNavigationWithFling::TurnPage(bool downPage)
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

void UIBookStoreNavigationWithFling::UpdatePageNumber()
{
}

#ifdef KINDLE_FOR_TOUCH
bool UIBookStoreNavigationWithFling::OnTouchEvent(MoveEvent* moveEvent)
{
    m_gestureDetector.OnTouchEvent(moveEvent);
    return true;
}

bool UIBookStoreNavigationWithFling::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
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

bool UIBookStoreNavigationWithFling::OnSingleTapUp(MoveEvent* moveEvent)
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
