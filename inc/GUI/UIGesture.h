////////////////////////////////////////////////////////////////////////
// UIEvent.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIGESTURE_H__
#define __UIGESTURE_H__

#include "dkBaseType.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include "Framework/CDisplay.h"
#include "Utility/ThreadHelper.h"


#include "drivers/TouchDrivers.h"


using namespace std;
class MoveEvent;

//#ifdef _X86_LINUX
//#define TOUCH_SCREEN_WIDTH     600
//#define TOUCH_SCREEN_HEIGHT    800
//
//#else
#define TOUCH_SCREEN_WIDTH     4150
#define TOUCH_SCREEN_HEIGHT    4150
//#endif

class TouchScreenMapper
{
public:
	static void TouchScreenMapperInit();

public:
	static int GetScreenHorizonPostion(int _x)
	{
#ifdef _X86_LINUX
        return _x;
#endif
        int _ret = CDisplay::GetDisplay()->GetScreenWidth() * _x / m_touchScreenWidth;

		return _ret < CDisplay::GetDisplay()->GetScreenWidth()? _ret : CDisplay::GetDisplay()->GetScreenWidth(); 
	};
	static int GetScreenVerticalPostion(int _y)
	{
#ifdef _X86_LINUX
        return _y;
#endif
        int _ret = CDisplay::GetDisplay()->GetScreenHeight() * _y / m_touchScreenHeight;

		return _ret < CDisplay::GetDisplay()->GetScreenHeight()? _ret : CDisplay::GetDisplay()->GetScreenHeight();
	};

private:
	static int m_touchScreenWidth;
	static int m_touchScreenHeight;
};

class UIGestureRecognizer
{

SINGLETON_H(UIGestureRecognizer);

public:
    static void GetTime(struct timeval& _t);

    static void* longTapTriggerThread(void * _pUseless);
	
    void Recognize(const MoveEvent* moveEvent);

    static pthread_t m_LongTapThreadID;
private:

    struct timeval m_stStart_time;
    struct timeval m_stNow_time;
    struct timeval m_stEnd_time;

    static int     m_iStart_x; 
    static int     m_iStart_y;
    static int     m_iEnd_x;
    static int     m_iEnd_y;
    static int     m_iNow_x;
    static int     m_iNow_y;

    static long    m_longTapNo;

    static const int  m_minLongTapDistance = -5;
    static const int  m_maxLongTapDistance = 5;
    static bool  m_longTapOpen;
};

#endif //__UIGESTURE_H__
