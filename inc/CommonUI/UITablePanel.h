////////////////////////////////////////////////////////////////////////
// UITablePanel.h
// Contact:
////////////////////////////////////////////////////////////////////////

#ifndef _UITABLEPANEL_H_
#define _UITABLEPANEL_H_

#include "GUI/UIContainer.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/GestureDetector.h"
#endif

class TablePanelArgs: public EventArgs
{
public:
    TablePanelArgs()
        : succeeded(false)
        , total(-1)
        , window(NULL)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new TablePanelArgs(*this);
    }

    bool succeeded;
    int total;
    UIWindow* window;
};

class UITablePanel : public UIContainer
{
public:
    static const char* EventTablePanelChange;
    UITablePanel();
    ~UITablePanel();
    virtual LPCSTR GetClassName() const
    {
        return "UITablePanel";
    }

    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual void InitStatus() {};

#ifdef KINDLE_FOR_TOUCH
    virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);
#endif
};

#endif //_UITABLEPANEL_H_
