#ifndef __MESSAGEEVENTARGS__
#define __MESSAGEEVENTARGS__

#include "GUI/EventArgs.h"
#include "Framework/INativeMessageQueue.h"

#if 0
class MessageEventArgs: public EventArgs
{
public:
    MessageEventArgs(
            ENativeMessageType _msgType, 
            UINT32 _param1,
            UINT32 _param2,
            UINT32 _param3)
        : msgType(_msgType)
        , param1(_param1)
        , param2(_param2)
        , param3(_param3)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new MessageEventArgs(*this);
    }

    ENativeMessageType msgType;
    uint32_t             param1; 
    uint32_t             param2; 
    uint32_t             param3; 
};
#endif

bool SendMessageArgsToEventListener(int handle, const EventArgs& args); 

#endif
