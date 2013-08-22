#ifndef __WIFIQUEUE_H_
#define __WIFIQUEUE_H_

#include <semaphore.h>
#include "singleton.h"
#include "dkObjBase.h"

class WiFiQueue
{
    SINGLETON_H(WiFiQueue);
    public:
        WiFiQueue();
        virtual ~WiFiQueue();
        static void* HandleQueue(void*);
        STDMETHOD(StartHandleQueueTask)();
private:
        static pthread_t         m_pThread;
};
#endif
