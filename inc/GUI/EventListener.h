#ifndef __GUI_EVENTLISTENER_H__
#define __GUI_EVENTLISTENER_H__

#include "GUI/EventSet.h"
#include <vector>
#include "Common/Defs.h"

#define DECLARE_TIMER_THREAD_FUNC(name) void Do##name();\
    void Start##name(bool repeated, dkUpTime dueTimeInMs, long intervalImMs);\
    void Stop##name();\
    static void name##Callback(void* data);

#define IMPLEMENT_TIMER_THREAD_FUNC(className, funcName) \
    void className::funcName##Callback(void* data) \
    {\
        className* pThis = (className*)GUISystem::GetInstance()->GetEventListenerByHandle((int)data);\
        if (NULL != pThis) \
        {\
            pThis->Do##funcName();\
        }\
    }\
    void className::Stop##funcName()\
    {\
        RemoveCallback(&className::funcName##Callback);\
    }\


#define IMPLEMENT_TIMER_THREAD_FUNC_WITHSTART(className, funcName) \
    IMPLEMENT_TIMER_THREAD_FUNC(className, funcName) \
    void className::Start##funcName(bool repeated, dkUpTime dueTime, long intervalInMs)\
    {\
        PostDelayed(&className::funcName##Callback, repeated, dueTime, intervalInMs); \
    }

class EventListener
{
public:
    EventListener();
    virtual ~EventListener();
    virtual const char* GetClassName() const
    {
        return "EventListener";
    }

    void SubscribeEvent(const char* name, EventSet& eventSet, const Event::Subscriber& subscriber);
    int GetHandle() const
    {
        return m_handle;
    }

    bool OnMessageEvent(const EventArgs& args);
    virtual void OnMessage(const SNativeMessage& /* msg */){}
protected:
    void PostDelayed(void (*func)(void*), bool repeated, dkUpTime dueTime, long intervalInMs);
    void RemoveCallback(void (*func)(void*));

    void SubscribeMessageEvent(const char* name, EventSet& eventSet, const Event::Subscriber& subscriber);
    std::map<std::string, Event::Subscriber> m_messageEventConnections;
    typedef std::vector<Event::Connection> ConnectionTracker;
    ConnectionTracker m_connections;
    int m_handle;
    std::vector<void (*)(void*)> m_timerCallbacks;
};
#endif
