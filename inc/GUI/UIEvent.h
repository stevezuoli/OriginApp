////////////////////////////////////////////////////////////////////////
// UIEvent.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIEVENT_H__
#define __UIEVENT_H__

#include "Utility/BasicType.h"
#include "Utility/CString.h"
#include "Public/Base/TPDef.h"
#include "GUI/UIWindow.h"
#include "Public/Base/TPComBase.h"

enum DefinedEvents
{
    CLICK_EVENT = 1,
    FOCUS_EVENT,
    BLUR_EVENT,
    SHOW_EVENT,
    HIDE_EVENT,
    DESTORY_EVENT,
    MENU_CLICK_EVENT,
    ITEM_CLICK_EVENT,
    ITEM_SELECTED_EVENT,
    VSCROLL_EVENT,
    HSCROLL_EVENT,
    UNKNOWN_EVENT,
    COMMAND_EVENT,
    LEAVE_BOTTOM_EVENT,
    REACH_BOTTOM_EVENT,
    IME_SHOW,
    IME_HIDE,
    SYM_SHOW,
    SYM_HIDE,
    CLICK_LISTBOX,
    SIZE_EVENT,
};

class UIWindow;

class UIEvent
{
    ////////////////////Constructor Section/////////////////////////
public:
    UIEvent(DefinedEvents iEventId=UNKNOWN_EVENT, UIWindow *pSender=NULL, DWORD dwPara1=0, DWORD dwParam2=0);

    ~UIEvent();
    ////////////////////Constructor Section/////////////////////////

    ////////////////////Method Section/////////////////////////
public:
    DWORD GetParam() const;
    DWORD GetParam2() const;
    UIWindow* GetSender() const;

    DefinedEvents GetEventId() const;
    void SetParam(DWORD param=0, DWORD Param2=0);

    void SetSender(UIWindow* pWindow);

private:
    void UpdateEventNameById();
    ////////////////////Method Section/////////////////////////

    ////////////////////Field Section/////////////////////////
private:
    DefinedEvents m_iEventId;

    UIWindow* m_pSender;

    DWORD m_dwParam1, m_dwParam2;
    ////////////////////Field Section/////////////////////////
};

#endif
