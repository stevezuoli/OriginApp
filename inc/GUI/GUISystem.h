#ifndef __GUISYSTEM_H__
#define __GUISYSTEM_H__

#include "GUI/EventListener.h"
#include <pthread.h>
#include <map>

class UIContainer;

class GUISystem
{
public:
    static GUISystem* GetInstance();
    int GetNextHandle();
    void RegisterEventListener(EventListener* eventListener);
    void UnregisterEventListener(EventListener* eventListener);
    EventListener* GetEventListenerByHandle(int handle);
    UIContainer* GetTopFullScreenContainer();
private:
    GUISystem();
    ~GUISystem();
private:
    int m_nextHandle;
    pthread_mutex_t m_lock;
    std::map<int, EventListener*> m_eventListeners;


};
#endif
