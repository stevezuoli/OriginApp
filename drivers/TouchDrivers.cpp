#include "../../DkReader/Common/DKLogger.h"
#include "Framework/CNativeThread.h"
#include "Framework/INativeMessageQueue.h"
#include "Utility/SystemUtil.h"
#include "Utility/ThreadHelper.h"
#include "drivers/MoveEvent.h"
#include "drivers/TouchDrivers.h"
#include "drivers/Usb.h"
#include "interface.h"
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <linux/input.h>
#include <linux/kd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "drivers/DeviceInfo.h"

using namespace dk::utility;
#define MESSAGE_TYPE KMessageMoveEvent

#if 0
// from Kindle Touch source
#define ABS_MT_SLOT		0x2f	/* MT slot being modified */
#define ABS_MT_TOUCH_MAJOR	0x30	/* Major axis of touching ellipse */
#define ABS_MT_TOUCH_MINOR	0x31	/* Minor axis (omit if circular) */
#define ABS_MT_WIDTH_MAJOR	0x32	/* Major axis of approaching ellipse */
#define ABS_MT_WIDTH_MINOR	0x33	/* Minor axis (omit if circular) */
#define ABS_MT_ORIENTATION	0x34	/* Ellipse orientation */
#define ABS_MT_POSITION_X	0x35	/* Center X ellipse position */
#define ABS_MT_POSITION_Y	0x36	/* Center Y ellipse position */
#define ABS_MT_TOOL_TYPE	0x37	/* Type of touching device */
#define ABS_MT_BLOB_ID		0x38	/* Group a set of packets as a blob */
#define ABS_MT_TRACKING_ID	0x39	/* Unique ID of initiated contact */
#define ABS_MT_PRESSURE		0x3a	/* Pressure on contact area */
#endif

#ifndef ABS_MT_SLOT
#define ABS_MT_SLOT 0x2F
#endif

#ifndef ABS_MT_TRACKING_ID
#define ABS_MT_TRACKING_ID 0x39
#endif

#ifndef ABS_MT_POSITION_X
#define ABS_MT_POSITION_X 0x35
#endif

#ifndef ABS_MT_POSITION_Y
#define ABS_MT_POSITION_Y 0x36
#endif

using namespace std;

int TouchDevice::s_touch_device = -1;          // fd for touch
int TouchDevice::s_key_device = -1;          // fd for key
int TouchDevice::s_curPointerCount = 0;
int TouchDevice::s_curSlotId = 0;
bool TouchDevice::s_suspend = false;

static int GetScreenHorizonPostion(int x)
{
    return DeviceInfo::GetDisplayScreenWidth() * x / DeviceInfo::GetTouchScreenWidth();
}

static int GetScreenVerticalPostion(int y)
{
    return DeviceInfo::GetDisplayScreenHeight() * y / DeviceInfo::GetTouchScreenHeight();
}

void TouchDevice::Suspend()
{
    // Do not grab input or suspend driver because it may disable driver
    // To refine it later.
    //ReleaseInput();
    //s_suspend = true;
}

void TouchDevice::Resume()
{
    // Do not grab input or suspend driver because it may disable driver
    // To refine it later.
    //CaptureInput();
    //s_suspend = false;
}

bool TouchDevice::CaptureInput()
{
    int grab = 1;
    int ioctl_touch_ret = -1;
    if (s_touch_device >= 0 )
    {
        ioctl_touch_ret = ioctl(s_touch_device, EVIOCGRAB, &grab);
        if (ioctl_touch_ret != 0)
        {
            DebugPrintf(DLC_DIAGNOSTIC, "\nKindleApp capture touch failed:%d\n", ioctl_touch_ret);
        }
        else
        {
            DebugPrintf(DLC_DIAGNOSTIC, "\nKindleApp capture touch succeeded\n");
        }
    }

    int ioctl_key_ret = -1;
    if (s_key_device >= 0 )
    {
        ioctl_key_ret = ioctl(s_key_device, EVIOCGRAB, &grab);
        if (ioctl_key_ret != 0)
        {
            DebugPrintf(DLC_DIAGNOSTIC, "\nKindleApp capture key failed:%d\n", ioctl_key_ret);
        }
        else
        {
            DebugPrintf(DLC_DIAGNOSTIC, "\nKindleApp capture key succeeded\n");
        }
    }
    return ioctl_touch_ret == 0 && ioctl_key_ret == 0;
}

bool TouchDevice::ReleaseInput()
{
    int ioctl_touch_ret = -1;
    if (s_touch_device >= 0 )
    {
        ioctl_touch_ret = ioctl(s_touch_device, EVIOCGRAB, 0);
        if (ioctl_touch_ret != 0)
        {
            DebugPrintf(DLC_DIAGNOSTIC, "\nKindleApp release touch failed:%d\n", ioctl_touch_ret);
        }
        else
        {
            DebugPrintf(DLC_DIAGNOSTIC, "\nKindleApp release touch succeeded\n");
        }
    }

    int ioctl_key_ret = -1;
    if (s_key_device >= 0 )
    {
        ioctl_key_ret = ioctl(s_key_device, EVIOCGRAB, 0);
        if (ioctl_key_ret != 0)
        {
            DebugPrintf(DLC_DIAGNOSTIC, "\nKindleApp release key failed:%d\n", ioctl_key_ret);
        }
        else
        {
            DebugPrintf(DLC_DIAGNOSTIC, "\nKindleApp release key succeeded\n");
        }
    }
    return ioctl_touch_ret == 0 && ioctl_key_ret == 0;
}

#ifndef _X86_LINUX
void TouchDevice::InitDevice()
{	
	if (DeviceInfo::IsKPW())
	{
		s_touch_device = OpenDevice("/dev/input/by-path/platform-cyttsp.0-event");
		return;
	}

    s_touch_device = OpenDevice("/dev/input/by-path/platform-zforce.0-event");
    s_key_device = OpenDevice("/dev/input/by-path/platform-whitney-button-event");
    //CaptureInput();
}

int TouchDevice::OpenDevice(const char * pszDev)
{
    if(NULL == pszDev)
    {
        return -1;
    }

    int s32Fd = -1;
	s32Fd = open(pszDev, O_RDONLY|O_NOCTTY);

    if(s32Fd>=0)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "Open_Dev(%s) successfully. fd=%d", pszDev, s32Fd);
    }
    else
    {
        DebugPrintf(DLC_DIAGNOSTIC,"!!!!FAILED Open_Dev(%s), errno=%d", pszDev, errno);
    }

    if (s32Fd>=FD_SETSIZE)
    {
        DebugPrintf(DLC_ERROR, "The fd is not less than FD_SETSIZE!!!");
    }
    return s32Fd;
}


void TouchDevice::CloseDevice(void)
{
    //ReleaseInput();

    if (s_touch_device >= 0 )
        close(s_touch_device);

    if (s_key_device >= 0 )
        close(s_key_device);

    s_key_device = -1;
    s_touch_device = -1;
}

void* TouchDevice::EventLoop(void*)
{
    if (s_touch_device < 0)
        return NULL;
    TouchDevice::ReadEvents(s_touch_device, s_key_device);
    return NULL;
}

bool TouchDevice::ReadEvents(int _touch_device, int _key_device)
{
    fd_set readfds;
    while (TRUE)
    {
        FD_ZERO(&readfds);
        FD_SET(_touch_device, &readfds);
        if (_key_device > 0)
        {
            FD_SET(_key_device, &readfds);
        }

        int iMaxfd = _touch_device > _key_device ? _touch_device : _key_device;
        int iRet=select(iMaxfd+1, &readfds, NULL, NULL, NULL);
        if (_key_device > 0 && FD_ISSET(_key_device, &readfds))
        {
            ReadKeyEvents(_key_device);
        }

        if (FD_ISSET(_touch_device, &readfds))
        {
            ReadTouchEvents(_touch_device);
        }
    }
    return true;
}

void TouchDevice::ReadTouchEvents(int _touch_device)
{
    s_curPointerCount = 0;
    s_curSlotId = 0;
    for (;;)
    {
        MoveEvent* moveEvent =  ReadSingleTouchEvent(_touch_device);
        if (NULL == moveEvent)
        {
            continue;
        }

        SNativeMessage msg;
        msg.iType = MESSAGE_TYPE;
        msg.iParam1 = (int)moveEvent;
        bool isUp = (moveEvent->GetActionMasked() == MoveEvent::ACTION_UP);

        INativeMessageQueue::GetMessageQueue()->Send(msg);
        if (isUp)
        {
            break;
        }
    }
}

//maybe just update one of x/y
MoveEvent* TouchDevice::ReadSingleTouchEvent(int _touch_device)
{
    struct input_event iebuf;
    MoveEvent moveEvent;
    int moveAction = MoveEvent::ACTION_MASK;
    int curActionIndex = -1;
    int curIndex = -1;
    static int s_oldX[2] = {-1, -1};
    static int s_oldY[2] = {-1, -1};
    static int index2Slots[2] ={-1, -1};
    static int64_t downTime = -1;
    int readCount = 0;
    for (;;)
    {
        if (read(_touch_device, &iebuf, sizeof(iebuf)) >0 )
        {
            if (EV_ABS == iebuf.type)
            {
                ++readCount;
            }
            DebugPrintf(DLC_GESTURE, "type: %d, code: %d, value: %d", iebuf.type, iebuf.code, iebuf.value);
            
            if (iebuf.type == EV_ABS && iebuf.code == ABS_MT_SLOT)
            {
                s_curSlotId = iebuf.value;
                continue;
            }
            if (iebuf.type == EV_ABS && iebuf.code == ABS_MT_TRACKING_ID && iebuf.value >=0)
            {
                ++curIndex;
                index2Slots[curIndex] = s_curSlotId;
                ++s_curPointerCount;
                if (1 == s_curPointerCount)
                {
                    moveAction = MoveEvent::ACTION_DOWN;
                    downTime = SystemUtil::GetUpdateTimeInMs();
                }
                else if (2 == s_curPointerCount)
                {
                    moveAction = MoveEvent::ACTION_POINTER_DOWN;
                }
                curActionIndex = curIndex;
            }

            if (iebuf.type == EV_ABS && iebuf.code == ABS_MT_TRACKING_ID && iebuf.value ==-1)
            {
                ++curIndex;
                index2Slots[curIndex] = s_curSlotId;
                --s_curPointerCount;
                curActionIndex = curIndex;
                if (1 == s_curPointerCount)
                {
                    moveAction = MoveEvent::ACTION_POINTER_UP;
                    // sometimes we get two ABS_MT_TRACKING_ID in one loop
                    // and must break it in two
                    break;
                }
                else if (0 == s_curPointerCount)
                {
                    moveAction = MoveEvent::ACTION_UP;
                }
            }

            if (iebuf.type == EV_ABS && iebuf.code == ABS_MT_POSITION_X)
            {
                if (-1 == curIndex || s_curSlotId != index2Slots[curIndex])
                {
                    ++curIndex;
                    index2Slots[curIndex] = s_curSlotId;
                }
                if (-1 == curActionIndex)
                {
                    curActionIndex = curIndex;
                }
                s_oldX[s_curSlotId] = GetScreenHorizonPostion(iebuf.value);
                if (MoveEvent::ACTION_MASK == moveAction)
                {
                    moveAction = MoveEvent::ACTION_MOVE;
                }
            }

            if (iebuf.type == EV_ABS && iebuf.code == ABS_MT_POSITION_Y)
            {
                if (-1 == curIndex || s_curSlotId != index2Slots[curIndex])
                {
                    ++curIndex;
                    index2Slots[curIndex] = s_curSlotId;
                }
                if (-1 == curActionIndex)
                {
                    curActionIndex = curIndex;
                }
                s_oldY[s_curSlotId] = GetScreenVerticalPostion(iebuf.value);

                if (MoveEvent::ACTION_MASK == moveAction)
                {
                    moveAction = MoveEvent::ACTION_MOVE;
                }
                if (MoveEvent::ACTION_DOWN == moveAction)
                {
                    // 有可能action_down后还有一个action_pointer_down
                    // 需要拆成两个move event
                    break;
                }
            }

            if (iebuf.type == EV_SYN && iebuf.code == SYN_REPORT && iebuf.value ==0)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    // previous read bread before read (0, 0, 0)
    // currently only read (0, 0, 0)
    if (0 == readCount)
    {
        return NULL;
    }
    moveEvent.m_pointerCount = curIndex + 1;
    moveEvent.m_action = moveAction;
    moveEvent.m_downTime = downTime;
    for (int i = 0; i < moveEvent.m_pointerCount; ++i)
    {
        moveEvent.m_x[i] = s_oldX[index2Slots[i]];
        moveEvent.m_y[i] = s_oldY[index2Slots[i]];
        moveEvent.m_pointerId[i] = index2Slots[i];
    }
    // 1. currently there're two fingers on touch screen
    //  but only one finger position information is reported in this loop
    // 2. previously there're two fingers on toush screen 
    //  but only one finger up information is reported in this loop
    // adding another's information
    if (2 == s_curPointerCount && 0 == curIndex
            || (MoveEvent::ACTION_POINTER_UP == moveAction && 0 == curIndex))
    {
        int anotherSlot = 1 - index2Slots[0];
        moveEvent.m_x[1] = s_oldX[anotherSlot];
        moveEvent.m_y[1] = s_oldY[anotherSlot];
        moveEvent.m_pointerId[1] = anotherSlot;
        moveEvent.m_pointerCount = 2;
    }
    moveEvent.m_actionIndex = curActionIndex;
    moveEvent.m_eventTime = SystemUtil::GetUpdateTimeInMs();

    for (int i = 0; i < 2; ++i)
    {
        DebugPrintf(DLC_GESTURE, "Slot %d: (%d, %d)", i, s_oldX[i], s_oldY[i]);
    }
    DebugPrintf(DLC_GESTURE, "Read move event %d: (%d, %d), pointer count: %d, action index: %d",
            moveEvent.GetActionMasked(),
            moveEvent.GetX(),
            moveEvent.GetY(),
            moveEvent.GetPointerCount(),
            moveEvent.GetActionIndex());
    return moveEvent.Clone();
}


void TouchDevice::ReadKeyEvents(int _key_device)
{
    bool data_finish = false;

    while (data_finish == false)
    {
        KINDLE_KEY_EVENT  _ke_ = ReadSingleKeyEvent(_key_device);

        if (_ke_ == KINDLE_KEY_DOWN)
        {
            CNativeThread::SendKeyMessage(102, 1);
            data_finish = true;
        }
        else if (_ke_ == KINDLE_KEY_UP)
        {
            CNativeThread::SendKeyMessage(102, 0);
            data_finish = true;
        }
    }
}

KINDLE_KEY_EVENT TouchDevice::ReadSingleKeyEvent(int _key_device)
{
    struct input_event iebuf;
    bool data_finished = false;

    KINDLE_KEY_EVENT _ke = KINDLE_KEY_NONE;

    while (data_finished == false)
    {
        if (read(_key_device, &iebuf, sizeof(iebuf)) >0 )
        {
            if (iebuf.type ==0 && iebuf.code ==0 && iebuf.value ==0)
            {
                data_finished = true; //finished the data reading for one section
            }

            if (iebuf.type ==1 && iebuf.code ==102 && iebuf.value ==0)
            {
                _ke = (KINDLE_KEY_EVENT)KINDLE_KEY_DOWN;
            }

            if (iebuf.type ==1 && iebuf.code ==102 && iebuf.value ==1)
            {
                _ke = (KINDLE_KEY_EVENT)KINDLE_KEY_UP;
            }
        }
        else
        {
            data_finished = true;
        }
    }
    return _ke;
}
#else

const char EMULATOR_KEY_FIFO[] = "/DuoKan/key_fifo";
const char EMULATOR_TOUCH_FIFO[] = "/DuoKan/touch_fifo";
void TouchDevice::InitDevice()
{
    // umask(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    umask(0);
    if (0 != mkfifo(EMULATOR_KEY_FIFO, O_RDWR | O_CREAT | O_TRUNC) && errno != EEXIST)
    {
        return;
    }
    s_key_device = open(EMULATOR_KEY_FIFO, O_RDONLY);
    int flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;

    struct stat statbuf;
    stat(EMULATOR_KEY_FIFO, &statbuf);
    chmod(EMULATOR_KEY_FIFO, statbuf.st_mode | flags);
    if (0 != mkfifo(EMULATOR_TOUCH_FIFO, O_RDWR | O_CREAT | O_TRUNC) && errno != EEXIST)
    {
        return;
    }
    s_touch_device = open(EMULATOR_TOUCH_FIFO, O_RDONLY);
    stat(EMULATOR_TOUCH_FIFO, &statbuf);
    chmod(EMULATOR_TOUCH_FIFO, statbuf.st_mode | flags);
}

void TouchDevice::CloseDevice()
{
    if (-1 != s_key_device)
    {
        close(s_key_device);
        s_key_device = -1;
    }

    if (-1 != s_touch_device)
    {
        close(s_touch_device);
        s_touch_device = -1;
    }
}

void* TouchDevice::EventLoop(void*)
{
    if (s_touch_device < 0 || s_key_device < 0 )
        return NULL;

    fd_set readfds;

    while (TRUE)
    {
        FD_ZERO(&readfds);

        FD_SET(s_touch_device, &readfds);
        FD_SET(s_key_device, &readfds);

        int iMaxfd = s_touch_device > s_key_device ? s_touch_device : s_key_device;
        select(iMaxfd+1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(s_key_device, &readfds))
        {
            ReadKeyEvents(s_key_device);
        }

        if (FD_ISSET(s_touch_device, &readfds))
        {
            ReadTouchEvents(s_touch_device);
        }
    }
}

void TouchDevice::ReadKeyEvents(int key_device)
{
    const int SCREENSAVER_IN_KEY = 0;
    const int SCREENSAVER_OUT_KEY = 1;
    const int USB_PLUGIN_KEY = 2;
    const int USB_PLUGOUT_KEY = 3;

    unsigned char c;
    read(key_device, &c, sizeof(c));
    SNativeMessage msg;
    switch (c)
    {
        case SCREENSAVER_IN_KEY:
            msg.iType = KMessageScreenSaverIn;
            CNativeThread::Send(msg);
            break;
        case SCREENSAVER_OUT_KEY:
            msg.iType = KMessageScreenSaverOut;
            CNativeThread::Send(msg);
            break;
        case USB_PLUGIN_KEY:
            msg.iType = KMessageUsbSingle;
            msg.iParam1 = usbPlugIn;
            CNativeThread::Send(msg);
            msg.iParam1 = usbConfigured;
            CNativeThread::Send(msg);
            break;
        case USB_PLUGOUT_KEY:
            msg.iType = KMessageUsbSingle;
            msg.iParam1 = usbPlugOut;
            CNativeThread::Send(msg);
            msg.iParam1 = usbUnconfigured;
            CNativeThread::Send(msg);
            break;
        default:
            CNativeThread::SendKeyMessage(c, 1);
            break;

    }
}

void TouchDevice::ReadTouchEvents(int touch_device)
{
      s_curPointerCount = 0;
    s_curSlotId = 0;
    int oldX = -1, oldY = -1;
    int64_t downTime = 0;
    for (;;)
    {
        MoveEvent* moveEvent = ReadSingleTouchEvent(touch_device);
        if (NULL == moveEvent)
        {
            continue;
        }
        if (MoveEvent::ACTION_DOWN == moveEvent->GetActionMasked())
        {
            oldX = moveEvent->GetX();
            oldY = moveEvent->GetY();
            downTime = moveEvent->m_eventTime;
            moveEvent->m_downTime = downTime; 

            SNativeMessage msg;
            msg.iType = MESSAGE_TYPE;

            msg.iParam1 = (uint32_t)moveEvent;
            msg.iParam2 = 0;
            msg.iParam3 = 0;
            INativeMessageQueue::GetMessageQueue()->Send(msg);
        }
        else if (MoveEvent::ACTION_UP == moveEvent->GetActionMasked())
        {
	        SNativeMessage msg;
            moveEvent->m_x[0] = oldX;
            moveEvent->m_y[0] = oldY;
            moveEvent->m_downTime = downTime;
            
            msg.iType = MESSAGE_TYPE;
            msg.iParam1 = (uint32_t)moveEvent;
            msg.iParam2 = 0;
            msg.iParam3 = 0;
            INativeMessageQueue::GetMessageQueue()->Send(msg);
            break;
        }
        else if (MoveEvent::ACTION_MOVE == moveEvent->GetActionMasked())
        {
            oldX = moveEvent->GetX();
            oldY = moveEvent->GetY();
            moveEvent->m_downTime = downTime;
            //send message
            SNativeMessage msg;
            msg.iType = MESSAGE_TYPE;
            msg.iParam1 = (uint32_t)moveEvent;
            msg.iParam2 = 0;
            msg.iParam3 = 0;

            INativeMessageQueue::GetMessageQueue()->Send(msg);
        }
    }
}

MoveEvent* TouchDevice::ReadSingleTouchEvent(int touch_device)
{
    MoveEvent moveEvent;
    TOUCH_DEVICE_STATUS tds = DEVICE_TOUCH_NONE;;
    int x, y;

    read(touch_device, &tds, sizeof(tds));
    read(touch_device, &x, sizeof(x));
    read(touch_device, &y, sizeof(y));
    switch (tds)
    {
        case DEVICE_TOUCH_START:
            moveEvent.m_action = MoveEvent::ACTION_DOWN;
            break;
        case DEVICE_TOUCH_END:
            moveEvent.m_action = MoveEvent::ACTION_UP;
            break;
        case DEVICE_TOUCH_UPDATE:
            moveEvent.m_action = MoveEvent::ACTION_MOVE;
            break;
        case DEVICE_TOUCH_NONE:
        case DEVICE_TOUCH_POINTER_DOWN:
        case DEVICE_TOUCH_POINTER_UP:
        default:
            break;
    }
    x = GetScreenHorizonPostion(x);
    y = GetScreenVerticalPostion(y);
    moveEvent.m_x[0] = x;
    moveEvent.m_y[0] = y;
    moveEvent.m_pointerId[0] = 0;
    moveEvent.m_pointerCount = 1;
    moveEvent.m_eventTime = SystemUtil::GetUpdateTimeInMs();
    return moveEvent.Clone();
}

#endif
