#ifndef __FREEFUNCTION_SLOT_H__
#define __FREEFUNCTION_SLOT_H__

#include "GUI/SlotFunctorBase.h"

class FreeFunctionSlot: public SlotFunctorBase
{
public:
    typedef bool (SlotFunction)(const EventArgs&);
    FreeFunctionSlot(SlotFunction* func)
        :m_function(func)
    {
    }
    virtual bool operator()(const EventArgs& args)
    {
        return m_function(args);
    }
private:
    SlotFunction* m_function;
};
#endif

