#include "GUI/UIGesture.h"

#include "drivers/DeviceInfo.h"
#include "drivers/MoveEvent.h"

int TouchScreenMapper::m_touchScreenWidth = 0;
int TouchScreenMapper::m_touchScreenHeight = 0;


SINGLETON_CPP(UIGestureRecognizer)


int  UIGestureRecognizer::m_iStart_x = 0; 
int  UIGestureRecognizer::m_iStart_y = 0;
int  UIGestureRecognizer::m_iEnd_x = 0;
int  UIGestureRecognizer::m_iEnd_y = 0;
int  UIGestureRecognizer::m_iNow_x = 0;
int  UIGestureRecognizer::m_iNow_y = 0;
long  UIGestureRecognizer::m_longTapNo = 0;
bool  UIGestureRecognizer::m_longTapOpen = false;

pthread_t UIGestureRecognizer::m_LongTapThreadID;

void UIGestureRecognizer::GetTime(struct timeval& _t)
{
    DebugPrintf(DLC_CHENM, "%s, %d, %s, Start ", __FILE__,  __LINE__,  __FUNCTION__);
    gettimeofday( &_t, NULL );
    DebugPrintf(DLC_CHENM, "%s, %d, %s, End ", __FILE__,  __LINE__,  __FUNCTION__);
}

void* UIGestureRecognizer::longTapTriggerThread(void * _pUseless)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    long nowThreadID = UIGestureRecognizer::m_longTapNo;
    timeval longTapStarttime, longTapNowTime;
    UIGestureRecognizer::GetTime(longTapStarttime);

    while(true)
    {
        if (UIGestureRecognizer::m_longTapOpen)
        {
            const int iPreX = UIGestureRecognizer::m_iNow_x;
            const int iPreY = UIGestureRecognizer::m_iNow_y;
            nowThreadID = UIGestureRecognizer::m_longTapNo;
            UIGestureRecognizer::GetTime(longTapStarttime);
            DebugPrintf(DLC_TOUCHSTACK_LONGTAP, "longTapTriggerThread before sleep(1) [%d, %d]", iPreX, iPreY);
            sleep(1);
            const int iDiffX = iPreX - UIGestureRecognizer::m_iNow_x;
            const int iDiffY = iPreY - UIGestureRecognizer::m_iNow_y;
            UIGestureRecognizer::GetTime(longTapNowTime);
            DebugPrintf(DLC_TOUCHSTACK_LONGTAP, "longTapTriggerThread after sleep(1) [%d, %d]", iDiffX, iDiffY);
            DebugPrintf(DLC_TOUCHSTACK_LONGTAP, "longTapTriggerThread nowThreadID = %d, m_longTapNo = %d", nowThreadID, m_longTapNo);
            DebugPrintf(DLC_TOUCHSTACK_LONGTAP, "longTapTriggerThread longTapNowTime.tv_sec   = %d", longTapNowTime.tv_sec);
            DebugPrintf(DLC_TOUCHSTACK_LONGTAP, "longTapTriggerThread longTapStarttime.tv_sec = %d", longTapStarttime.tv_sec);

            if ((nowThreadID == UIGestureRecognizer::m_longTapNo) 
                && ((longTapNowTime.tv_sec - longTapStarttime.tv_sec) < 2)
                && (iDiffX >= m_minLongTapDistance && iDiffX <= m_maxLongTapDistance 
                && iDiffY >= m_minLongTapDistance && iDiffY <= m_maxLongTapDistance))
            {
                DebugPrintf(DLC_TOUCHSTACK_LONGTAP, "UIGestureRecognizer::longTapTriggerThread sleep(1) TOUCH_LONG_TAP");
                MoveEvent* event = new MoveEvent();
                event->m_x[0] = UIGestureRecognizer::m_iNow_x;
                event->m_y[0] = UIGestureRecognizer::m_iNow_y;
                event->m_pointerCount = 1;
                event->m_pointerId[0] = 0;
                event->SetAction(MoveEvent::ACTION_LONG_TAP);
                SNativeMessage msg;
                msg.iType   = KMessageTouchEvent;
                msg.iParam1 = (int)event;
                INativeMessageQueue::GetMessageQueue()->Send(msg);
                continue;
            }
        }
        usleep(50000);
    }
    return NULL;
}

void UIGestureRecognizer::Recognize(const MoveEvent* moveEvent)
{
    DebugPrintf(DLC_DIAGNOSTIC, "Recognize:(%d, %d)", moveEvent->GetX(), moveEvent->GetY());
    GESTURE_EVENT _ge = GESTURE_NONE;

    int touchX = moveEvent->GetX();
    int touchY = moveEvent->GetY();
    switch (moveEvent->GetAction() & MoveEvent::ACTION_MASK)
    {
        case MoveEvent::ACTION_DOWN:
            m_iStart_x = touchX;
            m_iStart_y = touchY;

            m_iNow_x = m_iStart_x;
            m_iNow_y = m_iStart_y;

            _ge = GESTURE_START;
            m_longTapOpen = true;
            //ThreadHelper::CreateThread(&UIGestureRecognizer::m_LongTapThreadID, UIGestureRecognizer::longTapTriggerThread, NULL, "LongTap thread");

            GetTime(m_stStart_time);
            break;
        case MoveEvent::ACTION_MOVE:
            {
                int _tempx = touchX;
                int _tempy = touchY;

                bool _bMoved =false;
                const int  iMoveDistance = 30;
                if ( _tempx - m_iNow_x > iMoveDistance || m_iNow_x - _tempx > iMoveDistance)
                {
                    m_iNow_x = _tempx;
                    m_iNow_y = _tempy;
                    _bMoved = true;
                }
                else if ( _tempy - m_iNow_y > iMoveDistance || m_iNow_y - _tempy > iMoveDistance)
                {
                    m_iNow_x = _tempx;
                    m_iNow_y = _tempy;
                    _bMoved = true;
                }

                if (_bMoved)
                {
                    int _diffx = m_iNow_x - m_iStart_x;
                    int _diffy = m_iNow_y - m_iStart_y;

                    if (_diffx ==0)
                    {
                        _ge = _diffy >0 ? GESTURE_MOVE_DOWN : GESTURE_MOVE_UP;
                    }
                    else if (_diffy ==0)
                    {
                        _ge = _diffx >0 ? GESTURE_MOVE_RIGHT : GESTURE_MOVE_LEFT;
                    }
                    else
                    {
                        double tan30degree = 0.5773;
                        int p = _diffy / _diffx; 
                        int absDiffY = abs(_diffy);
                        int absDiffX = abs(_diffx);
                        if (absDiffX * 1.0 / absDiffY < tan30degree)
                        {
                            _ge = _diffy >0 ? GESTURE_MOVE_DOWN : GESTURE_MOVE_UP;
                        }
                        else if (absDiffX > absDiffY)
                        {
                            _ge = _diffx >0 ? GESTURE_MOVE_RIGHT : GESTURE_MOVE_LEFT;
                        }
                    }

                    GetTime(m_stNow_time);
                }
            }
            break;
        case MoveEvent::ACTION_UP:
            m_iEnd_x = touchX;
            m_iEnd_y = touchY;
            m_iNow_x = m_iEnd_x;
            m_iNow_y = m_iEnd_y;

            GetTime(m_stEnd_time);

            //ThreadHelper::CancelThread(UIGestureRecognizer::m_LongTapThreadID);
            m_longTapNo++;
            m_longTapOpen = false;
            _ge = GESTURE_END;
            break;
        case MoveEvent::ACTION_LONG_TAP:
            m_iNow_x = touchX;
            m_iNow_y = touchY;
            _ge = GESTURE_LONG_TAP;
            break;
    }
    if (_ge != GESTURE_NONE)

    DebugPrintf(DLC_CHENM, "%s, %d, %s, End ", __FILE__,  __LINE__,  __FUNCTION__);
}


void TouchScreenMapper::TouchScreenMapperInit()
{
	if ( (m_touchScreenWidth != 0) && (m_touchScreenHeight != 0))
	{
		return;	
	}

	if (DeviceInfo::IsKPW())
	{
		m_touchScreenWidth = 758;
		m_touchScreenHeight = 1024;

	}else 
	{
		m_touchScreenWidth = TOUCH_SCREEN_WIDTH;
		m_touchScreenHeight = TOUCH_SCREEN_HEIGHT;
	}
}
