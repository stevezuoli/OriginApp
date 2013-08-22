////////////////////////////////////////////////////////////////////////
// IUISwitchingAnimation.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __IUISWITCHINGANIMATION_H__
#define __IUISWITCHINGANIMATION_H__

#include "GUI/UIWindow.h"

#include "Public/Base/TPComBase.h"

class IUISwitchingAnimation
{
    ////////////////////Constructor Section/////////////////////////
public:
    virtual ~IUISwitchingAnimation()
    {
        // empty
    }
    ////////////////////Constructor Section/////////////////////////

    ////////////////////Method Section/////////////////////////
public:
    virtual void Step() = 0;
    virtual void Go(UIWindow* pWindow) = 0;
    virtual int GetInterval() = 0;
    virtual void Init(UIWindow* pWindow) = 0;
    virtual void Draw(ITpGraphics* pGraphics) = 0;
    virtual BOOL Finished() = 0;
    ////////////////////Method Section/////////////////////////
};

#endif
