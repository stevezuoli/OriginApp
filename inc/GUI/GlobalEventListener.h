#ifndef __GUI_GLOGALEVENTLISTENER_H__
#define __GUI_GLOGALEVENTLISTENER_H__
#include "GUI/EventListener.h"
#include <list>
class MoveEvent;
class UIBookReaderContainer;
typedef bool(*TouchHookFunc)(MoveEvent* moveEvent, void* data);
class GlobalEventListener: public EventListener
{
    struct TouchHookNode
    {
        TouchHookFunc func;
        void* data;
    };
public:
    static GlobalEventListener* GetInstance();
    bool HookTouch(MoveEvent* moveEvent);
    void RegisterTouchHook(TouchHookFunc func, void* data);
    void UnRegisterTouchHook(TouchHookFunc func, void* data);
	void SetBookReaderContainer(UIBookReaderContainer* bookreader);
private:
    bool OnPutReadingDataUpdate(const EventArgs& args);
    GlobalEventListener();
    GlobalEventListener(const GlobalEventListener&);
    GlobalEventListener& operator=(const GlobalEventListener&);
    std::list<TouchHookNode> m_touchHooks;
	UIBookReaderContainer* m_bookreaderContainer;
};
#endif
