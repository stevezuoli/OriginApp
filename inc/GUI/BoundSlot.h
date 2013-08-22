#ifndef __BOUNDSLOT_H__
#define __BOUNDSLOT_H__

#include "GUI/SubscriberSlot.h"
#include "KernelDef.h"
#include <stdint.h>

class Event;

class BoundSlot
{
public:
    BoundSlot(int group, const SubscriberSlot& subscriber, Event& event);
    ~BoundSlot();
    bool Connected() const;
    void Disconnect();
    bool operator==(const BoundSlot& rhs) const;
    bool operator!=(const BoundSlot& rhs) const;
private:
    friend class Event;
    // no assignment
    DISALLOW_COPY_AND_ASSIGN(BoundSlot);
    //BoundSlot& operator=(const BoundSlot& rhs);
    int m_group;
    SubscriberSlot* m_subscriber;
    Event* m_event;

};
#endif
