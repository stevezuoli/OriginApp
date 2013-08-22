#ifndef __EVENT_H__
#define __EVENT_H__

#include <tr1/memory>
#include <string>
#include "KernelDef.h"
#include <map>
#include "GUI/BoundSlot.h"
#include "Mutex.h"

class BoundSlot;
class SubscriberSlot;
class EventArgs;

class Event
{
public:
    typedef std::tr1::shared_ptr<BoundSlot> Connection;
    typedef SubscriberSlot Subscriber;
    typedef int Group;

    Event(const char* name);
    ~Event();
    const char* GetName(void) const
    {
        return m_name.c_str();
    }
    Connection Subscribe(const Subscriber& slot);
    Connection Subscribe(Group group, const Subscriber& slot);
    void operator()(const EventArgs& args);

private:
    friend void BoundSlot::Disconnect();
    void Unscribe(const BoundSlot& slot);
    DISALLOW_COPY_AND_ASSIGN(Event);
    typedef std::multimap<Group, Connection> SlotContainer;
    pthread_mutex_t m_slotsLock;
    SlotContainer m_slots;
    std::string m_name;

};
#endif

