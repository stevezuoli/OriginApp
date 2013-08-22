#ifndef __EVENT_SET_H__
#define __EVENT_SET_H__

#include <map>
#include <string>
#include "Framework/INativeMessageQueue.h"
#include "GUI/Event.h"
#include "Common/LockObject.h"

class EventSet
{
public:
    EventSet();
    virtual ~EventSet();
    void AddEvent(const char* name);
    void RemoveEvent(const char* name);
    void RemoveAllEvents();
    virtual Event::Connection AddEventListener(const char* name, const Event::Subscriber& subscriber);
    virtual Event::Connection AddEventListener(const char* name, Event::Group group, const Event::Subscriber& subscriber);
    virtual void FireEvent(const char* name, const EventArgs& args);

    void SendMessage(ENativeMessageType type, UINT32 iParam1 = 0, UINT32 iParam2 = 0, UINT32 iParam3 = 0);
private:
    bool IsEventPresent(const char* name);
    Event* GetEventObject(const char* name, bool autoAdd = false);
    void FireEventImpl(const char* name, const EventArgs& args);
    typedef std::map<std::string, Event*> EventMap;
    EventMap m_events;
    dk::common::LockObject m_eventsLock;
};

#endif
