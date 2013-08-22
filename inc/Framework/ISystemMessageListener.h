////////////////////////////////////////////////////////////////////////
// ISystemMessageListener.h
// Contact: yuzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef IMESSAGECONSUMER_H_
#define IMESSAGECONSUMER_H_

#include "Public/Base/TPDef.h"
#include "GUI/dkImage.h"

//using namespace DkFormat;

using DkFormat::DK_IMAGE;


class ISystemMessageListener
{
public:
    virtual ~ISystemMessageListener()
    {}

    virtual void OnKeyEvent(INT32 keyCode, EKeyboardEventType eventType)=0;

    virtual void OnPointerPressed(int x, int y)=0;

    virtual void OnPointerDragged(int x, int y)=0;

    virtual void OnPointerReleased(int x, int y)=0;

    virtual void OnPaint(DK_IMAGE g)=0;
};

#endif /*IMESSAGECONSUMER_H_*/

