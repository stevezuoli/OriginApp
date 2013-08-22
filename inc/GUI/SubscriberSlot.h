#ifndef __SUBSCRIBER_SLOT_H__
#define __SUBSCRIBER_SLOT_H__

#include "GUI/FreeFunctionSlot.h"
#include "GUI/FunctorCopySlot.h"
#include <assert.h>

class SubscriberSlot
{
public:
    SubscriberSlot()
        :m_functor_impl(0)
    {
    }
    SubscriberSlot(FreeFunctionSlot::SlotFunction* func)
        :m_functor_impl(new FreeFunctionSlot(func))
    {
    }
    template<typename T>
    SubscriberSlot(const T& functor)
    :m_functor_impl(new FunctorCopySlot<T>(functor))
    {
    }

    ~SubscriberSlot()
    {
        //故意不cleanup,等待disconnect时显式cleanup
    }

    void Cleanup()
    {
        delete m_functor_impl;
        m_functor_impl = 0;
    }

    bool operator()(const EventArgs& args)
    {
        return (*m_functor_impl)(args);
    }

    bool Connected() const
    {
        return m_functor_impl != 0;
    }
private:
    SlotFunctorBase* m_functor_impl;

};
#endif
