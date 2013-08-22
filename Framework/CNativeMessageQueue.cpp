////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/src/Framework/CNativeMessageQueue.cpp $
// $Revision: 9 $
// Contact: yuzhan
// Latest submission: $Date: 2008/08/01 13:57:50 $ by $Author: fareast\\xzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include <errno.h>
#include "dkTime.h"
#include "Framework/CNativeMessageQueue.h"
#include "Framework/CNativeThread.h"
#include "interface.h"
#include "StopWatch.h"
#include "Mutex.h"
#include "drivers/MoveEvent.h"

using DkFormat::DkTime;
using DkFormat::DkTimeSpan;

INativeMessageQueue* INativeMessageQueue::GetMessageQueue()
{
    static CNativeMessageQueue s_messageQueue(600);
    return &s_messageQueue;
}

CNativeMessageQueue::CNativeMessageQueue(UINT32 size)
    : m_pMessageQueue(NULL), m_nQueueSize(0), m_nReadPos(0), m_nWritePos(0)
{
    if (size > 0)
    {
        m_pMessageQueue = new SNativeMessage[size];
        if (NULL == m_pMessageQueue)
        {
            return;
        }

        m_nQueueSize = size;

        // TODO: initialize with real parameter.
        pthread_mutex_init(&m_QueueMutex, NULL);
        pthread_cond_init (&m_QueueCond, NULL);
    }
    m_keyMsgsInQueue = 0;
    m_paintMsgsInQueue = 0;
    EndCachePaint();
}

CNativeMessageQueue::~CNativeMessageQueue()
{
    if (m_pMessageQueue != NULL)
    {
        delete[] m_pMessageQueue;
        m_pMessageQueue = NULL;

        pthread_mutex_destroy(&m_QueueMutex);
        pthread_cond_destroy(&m_QueueCond);
    }
}

void CNativeMessageQueue::Receive(SNativeMessage &rMsg)
{
    AutoLock lock(&m_QueueMutex);
    while (m_nReadPos == m_nWritePos)
    {
        if (IsCaching())
        {
            if (ShouldEndCachePaint())
            {
                MakeEndCachePaintMessage(&rMsg);
                EndCachePaint();
                return;
            }
            struct timespec ts = (DkTime::Now() + DkTimeSpan::FromMilliSeconds(5)).ToTimeSpec();
            pthread_cond_timedwait (&m_QueueCond, &m_QueueMutex, &ts);
        }
        else
        {
            pthread_cond_wait (&m_QueueCond, &m_QueueMutex);
        }
    }

    rMsg = m_pMessageQueue[m_nReadPos];
    m_nReadPos++;

    if (KMessagePaint == rMsg.iType && m_paintMsgsInQueue > 0)
    {
        --m_paintMsgsInQueue;

        if (ShouldCachePaint())
        {
            DoCachePaint(&rMsg);
        }
        else if (ShouldEndCachePaint())
        {
            EndCachePaint();
        }
    }
    if (KMessageKeyboard == rMsg.iType && DK_KEYTYPE_UP != rMsg.iParam2 && m_keyMsgsInQueue > 0)
    {
        --m_keyMsgsInQueue;
    }
    if (m_nReadPos >= m_nQueueSize)
    {
        m_nReadPos = 0;
    }
}

BOOL CNativeMessageQueue::Send(const SNativeMessage &rMsg)
{
    DebugPrintf(DLC_MESSAGE,"CNativeMessageQueue::Send Message: Type=%d, param1=%d, param2=%d, param3=%d",
        rMsg.iType, rMsg.iParam1, rMsg.iParam2, rMsg.iParam3);
    //clear the key_up event for keyboard
    if (KMessageKeyboard == rMsg.iType && DK_KEYTYPE_UP == rMsg.iParam2)
    {
        return FALSE;
    }
        
    BOOL fSucceeded = FALSE;
    AutoLock lock(&m_QueueMutex);
    if (KMessageMoveEvent == rMsg.iType)
    {
        MoveEvent* curEvent = (MoveEvent*)rMsg.iParam1;
        if (MoveEvent::ACTION_MOVE == curEvent->GetActionMasked())
        {
            int nWritePos = m_nWritePos;
            while (nWritePos != (int)m_nReadPos)
            {
                --nWritePos;
                if (nWritePos == -1)
                {
                    nWritePos = m_nQueueSize - 1;
                }
                if (m_pMessageQueue[nWritePos].iType == KMessageMoveEvent)
                {
                    MoveEvent* moveEvent = (MoveEvent*)m_pMessageQueue[nWritePos].iParam1;
                    if (MoveEvent::ACTION_MOVE == moveEvent->GetActionMasked())
                    {
                        delete moveEvent;
                        m_pMessageQueue[nWritePos].iParam1 = (int)curEvent;
                        return true;
                    }
                    break;
                }
            }
        }
    }
    if ((m_nWritePos + 1) % m_nQueueSize != m_nReadPos)
    {
        m_pMessageQueue[m_nWritePos] = rMsg;
        m_nWritePos++;

        if (KMessageKeyboard == rMsg.iType && DK_KEYTYPE_UP != rMsg.iParam2)
        {
            ++m_keyMsgsInQueue;
        }
        if (KMessagePaint == rMsg.iType)
        {
            ++m_paintMsgsInQueue;
        }
        if (m_nWritePos >= m_nQueueSize)
        {
            m_nWritePos = 0;
        }

        fSucceeded = TRUE;
        pthread_cond_signal (&m_QueueCond);
    }
    return fSucceeded;
}

bool CNativeMessageQueue::HasKeyMsgsInQueue() const
{
    return m_keyMsgsInQueue > 0;
}
bool CNativeMessageQueue::HasPaintMsgsInQueue() const
{
    return m_paintMsgsInQueue > 0;
}

bool CNativeMessageQueue::ShouldCachePaint() const
{
    if (!HasKeyMsgsInQueue() && !HasPaintMsgsInQueue())
        return false;

    const long MAX_PENDING_INTERVAL = 10;
    DkTime now = DkTime::Now();
    if (!m_firstCachePaintTime.IsValid()
            || (now - m_firstCachePaintTime).TotalMilliSeconds() <= MAX_PENDING_INTERVAL)
    {
        return true;
    }
    return false;
}


void CNativeMessageQueue::EndCachePaint()
{
    if (IsCaching())
    {
        DebugPrintf(DLC_PENGF, "CNativeMessageQueue::EndCachePaint() End pending");
        m_firstCachePaintTime.MakeInvalid();
    }
}

void CNativeMessageQueue::DoCachePaint(SNativeMessage* msg)
{
    if (NULL == msg || KMessagePaint != msg->iType)
    {
        return;
    }
    if (!IsCaching())
    {
        DebugPrintf(DLC_PENGF, "CNativeMessageQueue::DoCachePaint() Start pending");
        m_firstCachePaintTime.MakeNow();
    }
    DkRect rect = DkRectFromPaintMessage(*msg);
    paintflag_t paintFlag = PaintFlagFromPaintMessage(*msg);
    paintFlag |= PAINT_FLAG_CACHABLE;
    MakePaintMessage(rect, paintFlag, msg);
}
bool CNativeMessageQueue::IsCaching() const
{
    return m_firstCachePaintTime.IsValid();
}
bool CNativeMessageQueue::ShouldEndCachePaint() const
{
    return IsCaching() && !ShouldCachePaint();
}
