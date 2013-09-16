////////////////////////////////////////////////////////////////////////
// UIBottomBar.h
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////
#ifndef __UIBOTTOMBAR_H__
#define __UIBOTTOMBAR_H__

#include "CommonUI/UIButtonGroupWithFocusedLine.h"
#include "GUI/UITouchComplexButton.h"
#include "Common/Defs.h"
#include "singleton.h"

class UIBottomBar : public UIButtonGroupWithFocusedLine 
{
    SINGLETON_H(UIBottomBar)
public:
    UIBottomBar();
    ~UIBottomBar();
    
    virtual LPCSTR GetClassName() const
    {
        return "UIBottomBar";
    }
    
    //HRESULT Draw(DK_IMAGE drawingImg);
    
    //virtual void MoveWindow(INT32 _iLeft, INT32 _iTop, INT32 _iWidth, INT32 _iHeight);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    //virtual void SetSelectedCmdId(DWORD _dwCmdId);
    //DWORD GetSelectedCmdId() const { return m_dwSelectedCmdId; }

private:
    void InitUI();

private:
    UITouchComplexButton m_btns[PAT_Count];
    //UITouchComplexButton m_btnBookShelf;
    //UITouchComplexButton m_btnBookStore;
    //UITouchComplexButton m_btnPersonal;
    //UITouchComplexButton m_btnSystemSetting;
    //UIButtonGroup m_btnGroup;
    //DWORD    m_dwSelectedCmdId;
};
#endif //__UIBOTTOMBAR_H__
