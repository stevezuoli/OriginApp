#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <tr1/functional>
#include "Framework/CNativeThread.h"
#include "Framework/CMessageHandler.h"
#include "Utility/ThreadHelper.h"
#include "Utility/WebBrowserUtil.h"
#include "PowerManager/PowerManager.h"
#include "interface.h"
#include "I18n/StringManager.h"
#include "CommonUI/UIUtility.h"
#include "Framework/CDisplay.h"
#include "common/WindowsMetrics.h"
#include "GUI/GUISystem.h"

using namespace WindowsMetrics;
using namespace std;

namespace dk
{
namespace utility
{

pthread_t WebBrowserLauncher::m_pThread = 0;
static const char* WebBrowserBin = "web_browser";
static const char* WebBrowserEnv = "source /DuoKan/setup";
static const char* MakeRootRW = "mntroot rw";
static const char* ClearWebBrowser = "killall -9 web_browser";

const char* WebBrowserLauncher::EventWebBrowserLaunched = "EventWebBrowserLaunched";
const char* WebBrowserLauncher::EventWebBrowserClosed = "EventWebBrowserClosed";

WebBrowserLauncher::WebBrowserLauncher()
	: m_msgBoxTip(NULL)
{
	SubscribeMessageEvent(WebBrowserLauncher::EventWebBrowserClosed,
        *this,
        std::tr1::bind(
        std::tr1::mem_fn(&WebBrowserLauncher::OnWebBrowserClosed),
        this,
        std::tr1::placeholders::_1));
}

WebBrowserLauncher::~WebBrowserLauncher()
{
	if(m_msgBoxTip)
	{
		SafeDeletePointer(m_msgBoxTip);
	}
}

WebBrowserLauncher* WebBrowserLauncher::GetInstance()
{
    static WebBrowserLauncher s_webBrowserLauncher;
    return &s_webBrowserLauncher;
}

bool WebBrowserLauncher::Exec(const CommandList& args)
{
    if (m_pThread != 0)
	{	
		DebugPrintf(DLC_DIAGNOSTIC, "WebBrowserLauncher::WebBrowserThread is already running.");
		return false;
	}
    WebBrowserLauncher::GetInstance()->SetCurrentCommand(args);
    WebBrowserLauncher::GetInstance()->ShowBlockUITip();

    SNativeMessage msg;
    msg.iType = KMessageWebBrowserStart;
    CNativeThread::Send(msg);
    //DebugPrintf(DLC_DIAGNOSTIC, "WebBrowserUtil::WebBrowserThread send KMessageWebBrowserStart");
    return true;
}

int WebBrowserLauncher::Launch()
{
	PowerManagement::DKPowerManagerImpl::GetInstance()->AccquireLock(PM_LOCK_SCREEN, MODULE_WEBBROWSER);
    CommandList args = WebBrowserLauncher::GetInstance()->GetCurrentCommand();
    std::string command_str = WebBrowserBin;
    for (int i = 0; i < args.size(); ++i)
    {
        command_str.append(" ")
                   .append(args[i]);
    }
    
    // qws postfix
    command_str.append(" -qws");

    int err = ThreadHelper::CreateThread(&m_pThread,
                                         WebBrowserThread,
                                         (void*)(command_str.c_str()),
                                         "WebBrowserLauncher @ WebBrowserThread",
                                         true,
                                         51200,
                                         SQM_THREAD_PRIORITY);
    
    //DebugPrintf(DLC_DIAGNOSTIC, "WebBrowserLauncher::WebBrowserThread %d#, err(%d)", m_pThread, err);
    if (err != 0)
    {
        PowerManagement::DKPowerManagerImpl::GetInstance()->ReleaseLock(PM_LOCK_SCREEN, MODULE_WEBBROWSER);
        WebBrowserLauncher::m_pThread = 0;

        // Notify message thread that web browser has stopped.
        SNativeMessage msg;
        msg.iType = KMessageWebBrowserStop;
        CNativeThread::Send(msg);
        DebugPrintf(DLC_DIAGNOSTIC, "Launching web browser failed, send top message");
        return err;
    }
    return 0;
}

void* WebBrowserLauncher::WebBrowserThread(void *para)
{    
    //DebugPrintf(DLC_DIAGNOSTIC, "WebBrowserThread: para=(%s)", (char *)para);
    char cmd[512];
    memset(cmd, 0, 512);
    sprintf(cmd, "%s;%s;%s;%s", ClearWebBrowser, WebBrowserEnv, MakeRootRW, (const char*)para);

    DebugPrintf(DLC_DIAGNOSTIC, "%s", cmd);
    system(cmd);
 
    // Notify message thread that web browser has stopped.
    SNativeMessage msg;
    msg.iType = KMessageWebBrowserStop;
    //CNativeThread::Send(msg);
    INativeMessageQueue::GetMessageQueue()->Send(msg);
    //DebugPrintf(DLC_DIAGNOSTIC, "WebBrowserUtil::WebBrowserThread send KMessageWebBrowserStop");
    return 0;
}

void WebBrowserLauncher::NotifyStopEvent()
{
    PowerManagement::DKPowerManagerImpl::GetInstance()->ReleaseLock(PM_LOCK_SCREEN, MODULE_WEBBROWSER);
    WebBrowserLauncher::m_pThread = 0;
    WebBrowserArgs args;
    args.succeeded = true;
    args.startArguments = m_commands;
    WebBrowserLauncher::GetInstance()->FireEvent(EventWebBrowserClosed, args);
}

void WebBrowserLauncher::ShowBlockUITip()
{
	CDisplay::CacheDisabler forceDraw;
	if(m_msgBoxTip)
	{
		SafeDeletePointer(m_msgBoxTip);
	}
	m_msgBoxTip = new UIMessageBox(GUISystem::GetInstance()->GetTopFullScreenContainer(), StringManager::GetStringById(WEBBROSER_START), MB_WITHOUTBTN);
	m_msgBoxTip->SetTipText(StringManager::GetStringById(WEBBROSER_START_TIP));
	m_msgBoxTip->SetEnableHookTouch(true);
	UIText* displayText = m_msgBoxTip->GetTextDisplay();
	displayText->SetAlign(ALIGN_CENTER);
	displayText->SetFontSize(GetWindowFontSize(FontSize24Index));

	UIText* tipText = m_msgBoxTip->GetTextTip();
	tipText->SetFontSize(GetWindowFontSize(FontSize16Index));
	tipText->SetAlign(ALIGN_CENTER);
    m_msgBoxTip->Popup();

	CDisplay* display = CDisplay::GetDisplay();
    display->ForceDraw(PAINT_FLAG_FULL);
}

bool WebBrowserLauncher::OnWebBrowserClosed(const EventArgs& args)
{
	if(m_msgBoxTip)
	{
        m_msgBoxTip->SetEnableHookTouch(false);
		m_msgBoxTip->EndDialog(0);
		SafeDeletePointer(m_msgBoxTip);
	}
	return true;
}


} // namespace utiltiy
} // namespace dk

