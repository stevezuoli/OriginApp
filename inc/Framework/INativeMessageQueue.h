////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/porting/inc/Framework/INativeMessageQueue.h $
// $Revision: 10 $
// Contact: yuzhan
// Latest submission: $Date: 2008/04/23 18:12:14 $ by $Author: fareast\\wzh $
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef INATIVEMESSAGEQUEUE_H_
#define INATIVEMESSAGEQUEUE_H_
#include "dkRect.h"

#include "Utility/BasicType.h"

enum ENativeMessageType
{
    KMessageNothing,
    KMessageStartApp,
    KMessagePauseApp,
    KMessageDestroyApp,
    KMessageKeyboard,
    KMessagePaint,
    KMessagePointer,
    KMessageSynchronousCall,
    KMessageSynchronizedCall,
    KMessageUsbSingle,
    KMessageUpdateUI,   //used by other thread to draw UI
    KMessageUpdateTitleBar,
    KMessageUpdateWithoutRepaint,
    // FIXME added by genchw
    KMessageExit,
    KMessageScreenSaverIn,
    KMessageScreenSaverOut,
    KMessageSuspending,
    KMessageDownloadStatusUpdate,
    KMessageDownloadAddTask,
    KMessageDownloadDeleteTask,
    KMessageNewReleaseDownloadNotify,
    KMessageNewReleaseUpgradeNotify,
    KMessageNewSerializedSectionDownloaded,
    KMessageFailedToConnectNetwork,
    KMessageNewSerializedACTIONAFTERAFTERPOST,
    KMessageNewSerializedUpdateCompleted,
    KMessageGoHome,
    KMessageGoPageStackEntry,
    KMessageGoPrePage,
    KMessageWifiChange,
    KMessageEndCachePaint,
    KMessageQiuShiBaiKe,
    KMessageBatteryUpdate,
    KMessageBatteryWarning,
    KMessageBatteryLowAlarm,
    KMessageBatteryLowDisalarm,
    KMessageTTSChange,
    KMessageMailSyncUp,
    KMessageDuoKanIDRegister,
    KMessageDuoKanIDLogin,
    KMessageDuoKanIDLogout,
    KMessageScreenSaverDownloaded,
    KMessageTouchEvent,
    KMessageOpenBook,
    KMessageRecommendListUpdate,
    KMessageEvent,
    KMessageTimerEvent,
    KMessageMoveEvent,
    KMessageItemClick,
    KMessagePrintScreen,
    KMessageWebBrowserStart,
    KMessageWebBrowserStop,
    KMessageThirdPartyAppFinished,
    KMessageDuokanLoginFailed,
};

const char* GetNativeMessageName(ENativeMessageType message);


enum EEventActionType
{
    KGotoFrame,
    KEndActivity,
    KClosePresentation,
};

typedef int HWND;
const HWND INVALID_WINDOW_ID = -1;

struct SNativeMessage
{
    SNativeMessage() :
        iType(KMessageNothing),iParam1(0), iParam2(0), iParam3(0)
    {
    }
    ENativeMessageType iType;
    HWND   hWnd;
    int iParam1;
    int iParam2;
    int iParam3;
};

#include "PaintFlag.h"
inline void MakeEndCachePaintMessage(SNativeMessage* msg)
{
    if (NULL == msg)
    {
        return;
    }
    msg->iType = KMessageEndCachePaint;
}

inline bool MakePaintMessage(int x, int y, int width, int height, paintflag_t flag, SNativeMessage* msg)
{
    if (NULL == msg)
    {
        return false;
    }
    if ((x | y | width | height) & 0xFFFF0000)
    {
        msg->iType = KMessageNothing;
        return false;
    }

    msg->iType = KMessagePaint;
    msg->iParam1 = (y << 16) | x;
    msg->iParam2 = (height << 16) | width;
    msg->iParam3 = flag;
    return true;
}
inline bool MakePaintMessage(const DkFormat::DkRect& rect, paintflag_t flag, SNativeMessage* msg)
{
    return MakePaintMessage(rect.Left(), rect.Top(), rect.Width(), rect.Height(), flag, msg);
}
inline DkFormat::DkRect DkRectFromPaintMessage(const SNativeMessage& msg)
{
    return DkFormat::DkRect::FromLeftTopWidthHeight(
            msg.iParam1 & 0xFFFF,
            msg.iParam1 >> 16,
            msg.iParam2 & 0xFFFF,
            msg.iParam2 >> 16);
}
inline paintflag_t PaintFlagFromPaintMessage(const SNativeMessage& msg)
{
    return (paintflag_t)msg.iParam3;
}
// Since message loop is doomed to be in platform independent layer (to support
// modal dialog), a message queue interface is here for message loop to use.
//
// The reason for a platform dependent message queue is just that Symbian provides
// a good enough thread-safe message queue and we don't wanna write our own
//
class INativeMessageQueue
{
public:
    virtual ~INativeMessageQueue()
    {
    }

    static INativeMessageQueue* GetMessageQueue();

    virtual void Receive(SNativeMessage &rMsg)=0;

    virtual BOOL Send(const SNativeMessage &rMsg)=0;
};

#endif /*INATIVEMESSAGEQUEUE_H_*/
