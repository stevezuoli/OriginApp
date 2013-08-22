#ifndef __TOUCH_DEVICE_H__
#define __TOUCH_DEVICE_H__

#include <sys/time.h>
#include <stdint.h>
#include "Mutex.h"

typedef enum{
    KINDLE_KEY_NONE,
    KINDLE_KEY_DOWN,
    KINDLE_KEY_UP,
} KINDLE_KEY_EVENT;


typedef enum{
    TOUCH_NONE,
    TOUCH_START,
    TOUCH_LONG_TAP,
    TOUCH_MOVE,
    TOUCH_END,
} TOUCH_EVENT;

typedef enum{
    DEVICE_TOUCH_NONE,
    DEVICE_TOUCH_START,
    DEVICE_TOUCH_END,
    DEVICE_TOUCH_UPDATE,
    DEVICE_TOUCH_POINTER_DOWN,
    DEVICE_TOUCH_POINTER_UP,
} TOUCH_DEVICE_STATUS;

typedef enum{
    GESTURE_NONE,  //0
    GESTURE_START, //1
    GESTURE_LONG_TAP, //2
    GESTURE_MOVE_UP,  //3
    GESTURE_MOVE_DOWN, //4
    GESTURE_MOVE_LEFT, //5
    GESTURE_MOVE_RIGHT, //6
    GESTURE_END, //7
} GESTURE_EVENT;

class MoveEvent;

class TouchDevice
{
public:
    static void InitDevice(void);
    static void CloseDevice(void);
    
    static void Suspend();
    static void Resume();

    static void* EventLoop(void*);

private:
    static bool ReadEvents(int _touch_device, int _key_device);

    static void ReadTouchEvents(int _touch_device);
    static MoveEvent* ReadSingleTouchEvent(int touchDevice);

    static void ReadKeyEvents(int _key_device);
    static KINDLE_KEY_EVENT ReadSingleKeyEvent(int _key_device);

    static int OpenDevice(const char * pszDev);

    static bool CaptureInput();
    static bool ReleaseInput();

private:
    static int s_touch_device;
    static int s_key_device;
    static int s_curPointerCount;
    static int s_curSlotId;
    static bool s_suspend;
};

#endif //__TOUCH_DEVICE_H__
