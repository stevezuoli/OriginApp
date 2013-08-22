////////////////////////////////////////////////////////////////////////
// IPage.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __IPage_H__
#define __IPage_H__

#include "Utility/BasicType.h"

class UIPage;

class IPage
{
    ////////////////////Constructor Section/////////////////////////
public:
    virtual ~IPage()
    {
        // empty
    }
    ////////////////////Constructor Section/////////////////////////

    ////////////////////Method Section/////////////////////////
public:
    virtual void OnRepaint() = 0;
    virtual INT32 GetScreenHeight() = 0;
    virtual INT32 GetScreenWidth() = 0;

    virtual void SetCurrentPage(UIPage* pUIPage, BOOL bIsPopup) = 0;
    ////////////////////Method Section/////////////////////////
};
#endif
