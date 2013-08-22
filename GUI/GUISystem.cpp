#include "GUI/GUISystem.h"
#include <map>
#include "Mutex.h"
#include <cassert>
#include "KernelDef.h"
#include "CommonUI/CPageNavigator.h"

GUISystem::GUISystem()
{
    m_nextHandle = 0;
    pthread_mutex_init(&m_lock, 0);
}

GUISystem::~GUISystem()
{
    pthread_mutex_destroy(&m_lock);
}

GUISystem* GUISystem::GetInstance()
{
    static GUISystem system;
    return &system;
}

HWND GUISystem::GetNextHandle()
{
    AutoLock lock(&m_lock);
    return m_nextHandle++;
}

void GUISystem::RegisterEventListener(EventListener* eventListener)
{
    assert(eventListener);
    AutoLock lock(&m_lock);
#ifdef _DEBUG
    assert(m_eventListeners.find(eventListener->GetHandle()) == m_eventListeners.end());
#endif
    m_eventListeners[eventListener->GetHandle()] = eventListener;
}

void GUISystem::UnregisterEventListener(EventListener* eventListener)
{
    assert(eventListener);
    AutoLock lock(&m_lock);
#ifdef _DEBUG
    assert(m_eventListeners.find(eventListener->GetHandle()) != m_eventListeners.end());
#endif
    m_eventListeners.erase(eventListener->GetHandle());
}

EventListener* GUISystem::GetEventListenerByHandle(int handle)
{
    AutoLock lock(&m_lock);
    DK_AUTO(pos, m_eventListeners.find(handle));
    if (pos != m_eventListeners.end())
    {
        return pos->second;
    }
    else
    {
        return NULL;
    }
}

UIContainer* GUISystem::GetTopFullScreenContainer()
{
    UIPage* curPage = CPageNavigator::GetCurrentPage();
    if (NULL == curPage)
    {
        return NULL;
    }
    return curPage->GetTopFullScreenContainer();
}
