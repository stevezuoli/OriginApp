////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/src/Framework/CNativeThread.cpp $
// $Revision: 24 $
// Contact: yuzhan
// Latest submission: $Date: 2008/08/25 20:50:27 $ by $Author: fareast\\xzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "interface.h"

#include "Framework/CNativeThread.h"
#include "Framework/CMessageHandler.h"
#include "GUI/FontManager.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ThreadHelper.h"
#include "drivers/DeviceInfo.h"

static void* ThreadFunc (void*)
{
    pthread_t ppid = pthread_self();
    pthread_detach(ppid);

    while(TRUE)
    {
        CMessageHandler::EHandlerReturnType ret = CMessageHandler::HandleMessage();

        if (CMessageHandler::EXIT_APP == ret)
        {
            break;
        }
    }
    return NULL;
}

bool CNativeThread::s_isKeyHomePressed = false;
bool CNativeThread::s_isKeyHomeDealedAlready = false;
bool CNativeThread::s_isScreenShotted = false;
struct timeval CNativeThread::s_keyHomePressedTime = timeval();

struct timeval CNativeThread::GetKeyHomePressedTime()
{
    return s_keyHomePressedTime;
}

bool CNativeThread::GetKeyHomePressStatus()
{
    return s_isKeyHomePressed;
}

void CNativeThread::SetKeyHomeDealedAlready(bool dealed)
{
    s_isKeyHomeDealedAlready = dealed;
}

bool CNativeThread::GetKeyHomeDealedAlready()
{
    return s_isKeyHomeDealedAlready;
}

bool CNativeThread::SetScreenShotted(bool screenShoted)
{
    s_isScreenShotted = screenShoted;
    return true;
}

bool CNativeThread::GetScreenShotted()
{
    return s_isScreenShotted;
}

void CNativeThread::Initialize()
{
    static bool inited = false;
    if (!inited)
    {
        inited = true;
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init (&attr);
        pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&tid, &attr, ThreadFunc, NULL);
        pthread_attr_destroy(&attr);
        s_isKeyHomeDealedAlready = false;
        s_isKeyHomePressed = false;
        s_isScreenShotted = false;
        gettimeofday(&s_keyHomePressedTime, NULL);
    }
}

void CNativeThread::Send(SNativeMessage &msg)
{
    INativeMessageQueue::GetMessageQueue()->Send(msg);
}

void CNativeThread::SendKeyMessage(int aKeyEvent, int aType)
{
    static int KeyStatus = 0;

    int tpKeyCode = aKeyEvent;
    int tpKeyEventType = GetTPKeyboardEventType(aType);

    if (tpKeyCode == KEY_LEFTALT)
    {
        if(tpKeyEventType == DK_KEYTYPE_DOWN)
        {
            KeyStatus |= ALT_DOWN;
        }
        else if(tpKeyEventType == DK_KEYTYPE_UP)
        {
            KeyStatus &= (~ALT_DOWN);
        }
    }
    else if(tpKeyCode == KEY_LEFTSHIFT)
    {
        if(tpKeyEventType == DK_KEYTYPE_DOWN)
        {
            KeyStatus |= SHIFT_DOWN;
        }
        else if(tpKeyEventType == DK_KEYTYPE_UP)
        {
            KeyStatus &= (~SHIFT_DOWN);
        }
    }
    else if( (tpKeyCode == KEY_SYM) && (DeviceInfo::IsK4NT()) )
    {
        if(tpKeyEventType == DK_KEYTYPE_DOWN)
        {
            KeyStatus |= KEY_SYM_DOWN_K4NT;
        }
        else if(tpKeyEventType == DK_KEYTYPE_UP)
        {
            KeyStatus &= (~KEY_SYM_DOWN_K4NT);
        }
    }
    else
    {
        tpKeyCode |= KeyStatus;
    }
    bool isAltPressed = tpKeyCode & ALT_DOWN, isShiftPressed = tpKeyCode & SHIFT_DOWN;
    DebugPrintf(DLC_MESSAGE, "CNativeThread::SendKeyMessage() altPressed = %s  shiftPressed =%s", isAltPressed? "DOWN":"UP",  isShiftPressed? "DOWN":"UP");
    DebugPrintf(DLC_MESSAGE, "CNativeThread::SendKeyMessage() inputKey = %d  translatedKey =%d, real key=%c", aKeyEvent, tpKeyCode, VirtualKeyToChar(tpKeyCode));
    DebugPrintf(DLC_MESSAGE, "CNativeThread::SendKeyMessage() tpKeyCode = %d  tpKeyEventType =%d", tpKeyCode, tpKeyEventType);


    //using the repeating_key + key_up to handle the long press 
    static bool key_repeat =false;


    // illegal (unhandled) keyboard event is eaten here
    if (tpKeyCode == KEY_HOME)
    {
        if (tpKeyEventType == DK_KEYTYPE_DOWN)
        {
            s_isKeyHomePressed = false;
        }
        else if (tpKeyEventType == DK_KEYTYPE_UP)
        {
            gettimeofday(&s_keyHomePressedTime, NULL);
            s_isKeyHomePressed = true;
            s_isKeyHomeDealedAlready = false;
            s_isScreenShotted = false;
            return;
        }
    }

    if (tpKeyCode >= 0 && tpKeyEventType >= 0 && false == key_repeat)
    {
        SNativeMessage msg;
        msg.iType = KMessageKeyboard;
        msg.iParam1 = tpKeyCode;
        msg.iParam2 = tpKeyEventType;
        INativeMessageQueue::GetMessageQueue()->Send(msg);
    }
}

void CNativeThread::AsyncCall(void(*func)(void*), void* data)
{
    SNativeMessage msg;
    msg.iType = KMessageTimerEvent;
    msg.iParam1 = (uint32_t)func;
    msg.iParam2 = (uint32_t)data;
    Send(msg);
}
