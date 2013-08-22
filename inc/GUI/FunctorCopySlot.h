#ifndef __FUNCTORCOPYSLOT_H__
#define __FUNCTORCOPYSLOT_H__

#include "GUI/SlotFunctorBase.h"
template <typename T>
class FunctorCopySlot: public SlotFunctorBase
{
public:
    FunctorCopySlot(const T& functor)
        :m_functor(functor)
    {
    }
    virtual bool operator()(const EventArgs& args)
    {
        return m_functor(args);
    }
private:
    T m_functor;
};
#endif

