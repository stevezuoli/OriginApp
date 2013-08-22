////////////////////////////////////////////////////////////////////////
// CNativeThread.h
// Contact: yuzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////
#ifndef __CNATIVETHREAD_H__
#define __CNATIVETHREAD_H__

#include "Framework/CNativeMessageQueue.h"
#include "GUI/ITpGraphics.h"
#include "Utility/IRunnable.h"
#include "Public/Base/TPDef.h"
#include "sys/time.h"

class CNativeThread
{
public:
    static void Initialize();

    static void SendKeyMessage(int aKeyEvent, int aType);
    static void Send(SNativeMessage &msg);

    static struct timeval GetKeyHomePressedTime();
    static bool GetKeyHomePressStatus();
    static void SetKeyHomeDealedAlready(bool);
    static bool GetKeyHomeDealedAlready();
    static bool GetScreenShotted();
    static bool SetScreenShotted(bool);
    static void AsyncCall(void(*func)(void*), void* data);

private:
    CNativeThread();

private:
    static struct timeval s_keyHomePressedTime;
    static bool s_isKeyHomePressed;
    static bool s_isKeyHomeDealedAlready;
    static bool s_isScreenShotted;
};

#endif // HELLOTHREAD_H


