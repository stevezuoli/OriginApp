#include "GUI/Event.h"
#include "GUI/EventArgs.h"

Event::Event(const char* name)
    :m_name(name)
{
    pthread_mutex_init(&m_slotsLock, 0);
}

Event::~Event()
{

    {
        AutoLock lock(&m_slotsLock);
        for (DK_AUTO(pos, m_slots.begin()); pos != m_slots.end(); ++pos)
        {
            pos->second->m_event = 0;
            pos->second->m_subscriber->Cleanup();
        }
        m_slots.clear();
    }
    pthread_mutex_destroy(&m_slotsLock);
}


Event::Connection Event::Subscribe(const Subscriber& slot)
{
    return Subscribe(-1, slot);
}

Event::Connection Event::Subscribe(Event::Group group, const Event::Subscriber& slot)
{
    AutoLock lock(&m_slotsLock);
    Event::Connection connection(new BoundSlot(group, slot, *this));
    m_slots.insert(std::pair<Group, Connection>(group, connection));
    return connection;
}

void Event::operator()(const EventArgs& args)
{
    AutoLock lock(&m_slotsLock);
    for (DK_AUTO(pos, m_slots.begin()); pos != m_slots.end(); ++pos)
    {
        if ((*(pos->second->m_subscriber))(args))
        {
            ++args.handled;
        }
    }
}

void Event::Unscribe(const BoundSlot& slot)
{
    AutoLock lock(&m_slotsLock);
    for (DK_AUTO(pos, m_slots.begin()); pos != m_slots.end(); ++pos)
    {
        if ((*(pos->second)) == slot)
        {
            m_slots.erase(pos);
            break;
        }
    }
}
