////////////////////////////////////////////////////////////////////////
// IMenuSource.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef IMENUSOURCE_H_
#define IMENUSOURCE_H_

#include "Public/Base/TPComBase.h"

class IEventHook;

class IMenuSource
{
public:
    IMenuSource() {}
    virtual ~IMenuSource() {}

    virtual void SetEventHook(IEventHook* pEventHook) = 0;
    virtual IEventHook* GetEventHook() = 0;
    virtual void AddMenuItem(INT32 iMenuItemId, const TCHAR* pstrLabel) = 0;
    virtual INT32 GetItemCount() = 0;
    virtual const TCHAR* GetItemLabel(INT32 iIndex) = 0;
    virtual INT32 GetItemId(INT32 iIndex) = 0;
    virtual void Clear() = 0;
};

#endif /*IMENUSOURCE_H_*/
