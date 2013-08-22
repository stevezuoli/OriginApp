#ifndef CNATIVEMESSAGEQUEUE_H_
#define CNATIVEMESSAGEQUEUE_H_

#include <pthread.h>
#include "Framework/INativeMessageQueue.h"
#include "dkTime.h"

class CNativeThread;

class CNativeMessageQueue : public INativeMessageQueue
{
    friend class CNativeThread;
public:
    CNativeMessageQueue(UINT32 size);
    
    virtual ~CNativeMessageQueue();
    
    virtual void Receive(SNativeMessage &rMsg);

    virtual BOOL Send(const SNativeMessage &rMsg);

private:
    bool ShouldEndCachePaint() const;
    bool ShouldCachePaint() const;
    bool IsCaching() const;
    void DoCachePaint(SNativeMessage* msg);
    void EndCachePaint();

    bool HasKeyMsgsInQueue() const;
    bool HasPaintMsgsInQueue() const;

    SNativeMessage *m_pMessageQueue;
    
    UINT32 m_nQueueSize;
    UINT32 m_nReadPos;
    UINT32 m_nWritePos;
    pthread_cond_t m_QueueCond;
    pthread_mutex_t m_QueueMutex;

    UINT32 m_keyMsgsInQueue;
    UINT32 m_paintMsgsInQueue;

    DkFormat::DkTime m_firstCachePaintTime;
};

#endif /*CNATIVEMESSAGEQUEUE_H_*/
