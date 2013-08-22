#ifndef _BOOKSTOREUI_UIBOOKSTOREBOOKABSTRACTINFO_H_
#define _BOOKSTOREUI_UIBOOKSTOREBOOKABSTRACTINFO_H_

#include "CommonUI/UITablePanel.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIText.h"

class UIBookStoreBookAbstractInfo : public UITablePanel
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookAbstractInfo";
    }
    UIBookStoreBookAbstractInfo(const std::string& abstractInfo);
    ~UIBookStoreBookAbstractInfo();

    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void MoveWindow(int left, int top, int width, int height);

private:
    void Init();
    void UpdatePageNumber();
    bool TurnPage(bool downPage);
    UIText m_abstractText;
    UITextSingleLine m_pageNumber;
    int m_totalPage;
    int m_curPage;
#ifdef KINDLE_FOR_TOUCH
    bool OnTouchEvent(MoveEvent* moveEvent);
    bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);
    bool OnSingleTapUp(MoveEvent* moveEvent);
    class BookAbstractPageGestureListener: public SimpleGestureListener
    {
    public:
        void SetTablePanel(UIBookStoreBookAbstractInfo* pTablePanel)
        {
            m_pTablePanel = pTablePanel;
        }
        virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
        {
            return m_pTablePanel->OnFling(moveEvent1, moveEvent2, direction, vx, vy);
        }
        virtual bool OnSingleTapUp(MoveEvent* moveEvent)
        {
            return m_pTablePanel->OnSingleTapUp(moveEvent);
        }
    private:
        UIBookStoreBookAbstractInfo* m_pTablePanel;
    };

    GestureDetector m_gestureDetector;
    BookAbstractPageGestureListener m_gestureListener;
#endif
};

#endif//_BOOKSTOREUI_UIBOOKSTOREBOOKABSTRACTINFO_H_

