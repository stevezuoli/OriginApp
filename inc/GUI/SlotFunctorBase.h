#ifndef __SLOTFUNCTORBASE_H__
#define __SLOTFUNCTORBASE_H__
class EventArgs;

class SlotFunctorBase
{
public:
    virtual ~SlotFunctorBase(){};
    virtual bool operator()(const EventArgs& args) = 0;
};

#endif

