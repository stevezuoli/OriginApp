#include "GUI/BoundSlot.h"
#include "GUI/Event.h"

BoundSlot::BoundSlot(int group, const SubscriberSlot& subscriber, Event& event)
    : m_group(group)
    , m_subscriber(new SubscriberSlot(subscriber))
    , m_event(&event)
{

}

BoundSlot::~BoundSlot()
{
    Disconnect();
    delete m_subscriber;
}

bool BoundSlot::Connected() const
{
    return m_subscriber != 0 && m_subscriber->Connected();
}

void BoundSlot::Disconnect()
{
    if (Connected())
    {
        m_subscriber->Cleanup();
    }
    if (m_event)
    {
        m_event->Unscribe(*this);
        m_event = 0;
    }
}

bool BoundSlot::operator==(const BoundSlot& rhs) const
{
    return m_subscriber == rhs.m_subscriber;
}

bool BoundSlot::operator!=(const BoundSlot& rhs) const
{
    return !(*this == rhs);
}
