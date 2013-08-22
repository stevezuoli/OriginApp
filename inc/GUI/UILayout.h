////////////////////////////////////////////////////////////////////////
// UILayout.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UILAYOUT_H__
#define __UILAYOUT_H__

#include "GUI/UIWindow.h"
#include <vector>

struct LayoutItem
{
    LayoutItem()
    {
        // empty
    }

    LayoutItem(UIWindow* pWindow, INT32 iFlag)
    {
        m_pWindow = pWindow;
        m_iFlag = iFlag;
    }

    UIWindow* m_pWindow;
    INT32 m_iFlag;
    INT32 m_iRightMargin;
    INT32 m_iBottomMargin;
};

class UILayout
{

    ////////////////////Constructor Section/////////////////////////
public:
    UILayout();

    ~UILayout();
    ////////////////////Constructor Section/////////////////////////

    ////////////////////Method Section/////////////////////////
public:
    void OnResize(INT32 iWidth, INT32 iHeight);
    BOOL IsInited();
    void AddControl(UIWindow* pWindow, INT32 iFlag);
    void RemoveControl(UIWindow* pWindow);
    void ClearControls();
    void Init(INT32 iWidth, INT32 iHeight);
    void Dispose();
    ////////////////////Method Section/////////////////////////

    ////////////////////Field Section/////////////////////////
private:
    BOOL m_bInited;
    std::vector<LayoutItem> m_controls;
    ////////////////////Field Section/////////////////////////
};

#endif
