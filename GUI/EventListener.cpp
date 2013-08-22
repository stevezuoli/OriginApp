#include "GUI/EventListener.h"
#include "GUI/GUISystem.h"
#include "GUI/MessageEventArgs.h"
#include "Utility/SystemUtil.h"
#include "Utility/TimerThread.h"
#include "interface.h"
#include <tr1/functional>

using namespace dk::utility;

EventListener::EventListener()
{
    m_handle = GUISystem::GetInstance()->GetNextHandle();
    GUISystem::GetInstance()->RegisterEventListener(this);
}

EventListener::~EventListener()
{
    for (DK_AUTO(pos, m_connections.begin()); pos != m_connections.end(); ++pos)
    {
        (*pos)->Disconnect();
    }
    GUISystem::GetInstance()->UnregisterEventListener(this);
    for (DK_AUTO(cur, m_timerCallbacks.begin());
            cur != m_timerCallbacks.end();
            ++cur)
    {
        TimerThread::GetInstance()->CancelTimer(*cur, (void*)GetHandle());
    }
    for (DK_AUTO(cur, m_messageEventConnections.begin());
            cur != m_messageEventConnections.end();
            ++cur)
    {
        cur->second.Cleanup();
    }
}

void EventListener::SubscribeEvent(const char* name, EventSet& eventSet, const Event::Subscriber& subscriber)
{
    m_connections.push_back(eventSet.AddEventListener(name, subscriber));
}

void EventListener::SubscribeMessageEvent(
        const char* name,
        EventSet& eventSet,
        const Event::Subscriber& subscriber)
{
    SubscribeEvent(name,
            eventSet,
            std::tr1::bind(SendMessageArgsToEventListener, GetHandle(), std::tr1::placeholders::_1));
    m_messageEventConnections[name] = subscriber;
}

bool EventListener::OnMessageEvent(const EventArgs& args)
{
    DK_AUTO(pos, m_messageEventConnections.find(args.GetName()));
    if (pos != m_messageEventConnections.end()) 
    {
        return pos->second(args);
    }
    return false;
}

void EventListener::PostDelayed(void (*func)(void*), bool repeated, dkUpTime dueTime, long intervalInMs)
{
    if (std::find(m_timerCallbacks.begin(), m_timerCallbacks.end(), func) != m_timerCallbacks.end())
    {
        //DebugPrintf(DLC_GESTURE, "WARNING: %s::PostDelayed add one func more than once!!!", GetClassName());
        //return;
    }
    TimerThread::GetInstance()->AddTimer(func, (void*)GetHandle(), dueTime, repeated, true, intervalInMs);
    m_timerCallbacks.push_back(func);
}

void EventListener::RemoveCallback(void (*func)(void*))
{
    if (std::find(m_timerCallbacks.begin(), m_timerCallbacks.end(), func) == m_timerCallbacks.end())
    {
        return;
    }
    TimerThread::GetInstance()->CancelTimer(func, (void*)GetHandle());
    m_timerCallbacks.erase(
            std::remove(m_timerCallbacks.begin(), m_timerCallbacks.end(), func),
            m_timerCallbacks.end());
}

