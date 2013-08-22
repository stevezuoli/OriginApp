#include "GUI/EventSet.h"
#include "Mutex.h"
#include "KernelDef.h"
#include "GUI/EventArgs.h"

using namespace dk::common;

EventSet::EventSet()
{
}

EventSet::~EventSet()
{
    RemoveAllEvents();
}

void EventSet::AddEvent(const char* name)
{
    LockScope lock(m_eventsLock);
    if (IsEventPresent(name))
    {
        return;
    }
    m_events[name] = new Event(name);
}

void EventSet::RemoveEvent(const char* name)
{
    LockScope lock(m_eventsLock);
    DK_AUTO(pos, m_events.find(name));
    if (pos != m_events.end())
    {
        delete pos->second;
        m_events.erase(pos);
    }
}

void EventSet::RemoveAllEvents()
{
    LockScope lock(m_eventsLock);
    for (DK_AUTO(pos, m_events.begin()); pos != m_events.end(); ++pos)
    {
        delete pos->second;
    }
    m_events.clear();
}

bool EventSet::IsEventPresent(const char* name)
{
    return m_events.find(name) != m_events.end();
}

Event::Connection EventSet::AddEventListener(const char* name, const Event::Subscriber& subscriber)
{
    LockScope lock(m_eventsLock);
    return GetEventObject(name, true)->Subscribe(subscriber);
}

Event::Connection EventSet::AddEventListener(const char* name, Event::Group group, const Event::Subscriber& subscriber)
{
    LockScope lock(m_eventsLock);
    return GetEventObject(name, true)->Subscribe(group, subscriber);
}

Event* EventSet::GetEventObject(const char* name, bool autoAdd)
{
    DK_AUTO(pos, m_events.find(name));
    if (pos != m_events.end())
    {
        return pos->second;
    }
    if (autoAdd)
    {
        m_events[name] = new Event(name);
        return m_events.find(name)->second;
    }
    return 0;
}

void EventSet::FireEvent(const char* name, const  EventArgs& args)
{
    FireEventImpl(name, args);
}

void EventSet::FireEventImpl(const char* name, const EventArgs& args)
{
    LockScope lock(m_eventsLock);
    Event* event = GetEventObject(name, false);
    if (event)
    {
        args.SetName(name);
        (*event)(args);
    }
}
