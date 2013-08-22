////////////////////////////////////////////////////////////////////////
// IUIWindow.h
// Contact: xzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef IUIWINDOW_H_
#define IUIWINDOW_H_

#include "Public/Base/TPComBase.h"

struct IEventHook;
class IMenuSource;

class IUIWindow
{
public:
    virtual ~IUIWindow() {}

    virtual BOOL IsEnable() const = 0; // IsEnable
    virtual void SetEnable(BOOL bEnable) = 0; // SetEnable

    virtual BOOL IsFocus() const = 0; // IsFocus
    virtual void SetFocus(BOOL bIsFocus) = 0; // SetFocus

    virtual BOOL IsVisible() const = 0; // IsVisible
    virtual void Show(BOOL bIsShow, BOOL bIsRepaint = TRUE) = 0; // Show

    virtual INT32 GetHeight() = 0; // GetHeight
    virtual INT32 GetWidth() = 0; // GetWidth
    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight) = 0;

    virtual INT32 GetTabIndex() const = 0;
    virtual void SetTabIndex(const int iTabIndex) = 0;

    virtual void SetEventHook(IEventHook* pHook) = 0; // SetEventHook
};

#endif /*IUIWINDOW_H_*/
