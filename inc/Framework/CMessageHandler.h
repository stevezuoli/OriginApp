////////////////////////////////////////////////////////////////////////
// CMessageHandler.h
// Contact: yuzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __CMESSAGEHANDLER_H__
#define __CMESSAGEHANDLER_H__

#include "Utility/BasicType.h"
#include "Framework/INativeMessageQueue.h"

class CMessageHandler
{
    static bool CanResponseCommonMessage();
public:
    enum EHandlerReturnType{
        CONTINUE_EXECUTION,
        EXIT_APP,
    };
    
    static EHandlerReturnType HandleMessage();

    static void GetMessage(SNativeMessage &msg);

    static EHandlerReturnType DispatchMessage(SNativeMessage &msg);
	
	static void SetLastDispatchMessage(SNativeMessage &msg, time_t &lastTime);
	static void GetLastDispatchMessage(SNativeMessage &msg, time_t &lastTime);
private:
	static SNativeMessage m_lastMsg;
	static time_t m_lastMsgTime;
};

#endif /*__CMESSAGEHANDLER_H__*/

