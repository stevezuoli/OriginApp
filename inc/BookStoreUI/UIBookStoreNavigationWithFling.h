#ifndef _DKREADER_BOOKSTOREUI_UIBOOKSTORENAVIGATIONWITHFLING_H_
#define _DKREADER_BOOKSTOREUI_UIBOOKSTORENAVIGATIONWITHFLING_H_

#include "BookStoreUI/UIBookStoreNavigation.h"

/********************************************
* UIBookStoreNavigationWithFling
********************************************/
class UIBookStoreNavigationWithFling : public UIBookStoreNavigation
{
public:
    UIBookStoreNavigationWithFling(const std::string& title,
        bool hasBottomBar = true,
        bool showFetchInfo = true,
        bool showRefresh = true);
    virtual ~UIBookStoreNavigationWithFling();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreNavigationWithFling";
    }
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void UpdatePageNumber();
    virtual bool TurnPage(bool downPage);

protected:
    int m_totalPage;
    int m_curPage;

private:
#ifdef KINDLE_FOR_TOUCH
    bool OnTouchEvent(MoveEvent* moveEvent);
    bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);
    bool OnSingleTapUp(MoveEvent* moveEvent);
    class BookAbstractPageGestureListener: public SimpleGestureListener
    {
    public:
        void SetNavigationPage(UIBookStoreNavigationWithFling* navigationPage)
        {
            m_pPage = navigationPage;
        }
        virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
        {
            return m_pPage->OnFling(moveEvent1, moveEvent2, direction, vx, vy);
        }
        virtual bool OnSingleTapUp(MoveEvent* moveEvent)
        {
            return m_pPage->OnSingleTapUp(moveEvent);
        }
    private:
        UIBookStoreNavigationWithFling* m_pPage;
    };

    GestureDetector m_gestureDetector;
    BookAbstractPageGestureListener m_gestureListener;
#endif
};

#endif//_DKREADER_BOOKSTOREUI_UIBOOKSTORENAVIGATIONWITHFLING_H_

