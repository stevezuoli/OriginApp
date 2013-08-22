////////////////////////////////////////////////////////////////////////
// IGUI.h
// Contact: xzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __IGUI_H__
#define __IGUI_H__

#define IUIWINDOW_INTERFACE_CONVERTION     virtual void SetWindowEnable(BOOL bEnable)  \
    {  \
        SetEnable(bEnable);  \
    }  \
    virtual void MoveWindowPosition(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)  \
    {  \
        MoveWindow(iLeft, iTop, iWidth, iHeight);   \
    }  \
    virtual BOOL IsWindowFocus()  \
    {  \
        return IsFocus();  \
    }  \
    virtual void SetWindowFocus(BOOL bIsFocus)  \
    {  \
        SetFocus(bIsFocus);  \
    }  \
    virtual BOOL IsWindowEnable()  \
    {  \
        return IsEnable();  \
    }  \
    virtual void SetWindowEventHook(IEventHook* pHook)  \
    {  \
        SetEventHook(pHook);  \
    }  \
    virtual BOOL IsWindowVisible()  \
    {  \
        return IsVisible();  \
    }  \
    virtual void ShowWindow(BOOL bIsShow, BOOL bIsRepaint = TRUE)  \
    {  \
        Show(bIsShow, bIsRepaint);  \
    }  \
    virtual INT32 GetWindowHeight()  \
    {  \
        return GetHeight();  \
    }  \
    virtual INT32 GetWindowWidth()  \
    {  \
        return GetWidth();  \
    }  \
    virtual INT32 GetWindowTabIndex()  \
    {  \
        return GetTabIndex();  \
    }  \
    virtual void SetWindowTabIndex(const int iTabIndex)  \
    {  \
        SetTabIndex(iTabIndex);  \
    }  \
    virtual void SetWindowUserSoftKeyMenu(IMenuSource* pMenuSource, BOOL bIsLeft)  \
    {  \
        SetUserSoftKeyMenu(pMenuSource, bIsLeft);  \
    } 

#endif /*__IGUI_H__*/
