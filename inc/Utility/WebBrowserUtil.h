#ifndef __UTILITY_WEBBROWSERUTIL_H__
#define __UTILITY_WEBBROWSERUTIL_H__

#include <string>
#include <vector>
#include <stdint.h>
#include "Common/Defs.h"
#include <semaphore.h>
#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"
#include "GUI/EventListener.h"
#include "GUI/UIMessageBox.h"

namespace dk
{
namespace utility
{
typedef std::vector<std::string> CommandList;

class WebBrowserArgs: public EventArgs
{
public:
    WebBrowserArgs()
        : succeeded(false)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new WebBrowserArgs(*this);
    }

    bool succeeded;
    CommandList startArguments;
};

class WebBrowserLauncher : public EventSet, EventListener
{
public:
    static const char* EventWebBrowserLaunched;
    static const char* EventWebBrowserClosed;

public:
    static bool Exec(const CommandList& args);
    static void* WebBrowserThread(void*);
    static WebBrowserLauncher* GetInstance();

    int Launch();
    void NotifyStopEvent();
    CommandList GetCurrentCommand() { return m_commands; }
    void SetCurrentCommand(const CommandList& args) { m_commands = args; }
	bool OnWebBrowserClosed(const EventArgs& args);
    void ShowBlockUITip();

private:
    WebBrowserLauncher();
    virtual ~WebBrowserLauncher();

private:
    static pthread_t m_pThread;
    CommandList m_commands;
	UIMessageBox* m_msgBoxTip;
};

} // namespace utiltiy
} // namespace dk
#endif
