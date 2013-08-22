#include "GUI/MessageEventArgs.h"

bool SendMessageArgsToEventListener(int handle, const EventArgs& args)
{
    SNativeMessage msg;
    msg.iType = KMessageEvent;
    msg.iParam1 = (int)args.Clone();
    msg.hWnd = handle;
    return INativeMessageQueue::GetMessageQueue()->Send(msg);
}
