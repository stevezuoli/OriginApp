#ifndef _TOUCHAPPUI_UISYSTEMSETTINGPAGENEW_H_
#define _TOUCHAPPUI_UISYSTEMSETTINGPAGENEW_H_

#include "GUI/UIPage.h"
#include "CommonUI/UITablePanel.h"
#include "CommonUI/UITableBarController.h"

class UISystemSettingPageNew: public UIPage
{
public:
    UISystemSettingPageNew();
    virtual void MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height);
    virtual LPCSTR GetClassName() const
    {
        return "UISystemSettingPageNew";
    }

    virtual void OnEnter();
private:
    void Init();
    class UISystemSettingPageNewGestureListener: public SimpleGestureListener
    {
    public:
        virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
        {
            return m_page->OnFling(moveEvent1, moveEvent2, direction, vx, vy);
        }
        void SetPage(UISystemSettingPageNew* page)
        {
            m_page = page;
        }
    private:
        UISystemSettingPageNew* m_page;
    };
    virtual bool OnTouchEvent(MoveEvent* moveEvent);
    bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);
    GestureDetector m_gestureDetector;
    UISystemSettingPageNewGestureListener m_gestureListener;
    UITableController m_tabController;
    UITablePanel* m_readingPanel;
    UITablePanel* m_systemPanel;
    UITablePanel* m_accountPanel;
};
#endif
