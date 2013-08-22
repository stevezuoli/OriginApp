#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "Framework/CDisplay.h"
#include "GUI/UIProgressBar.h"
#include "TouchAppUI/StartProgress.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ThreadHelper.h"
#include "drivers/DeviceInfo.h"

#define     PROGRESS_X      120
#define     PROGRESS_Y      610
#define     PROGRESS_W      360
#define     PROGRESS_H      4

#define     START_PIC_KT       "./res/start2013.jpg"
#define     START_PIC_KP       "./res/start2013@kp.jpg"


CStartProgress::CStartProgress(CDisplay* display)
    : m_tid(0)
    , m_doExit(false)
{
	x = PROGRESS_X * display->GetScreenWidth() / 600;
	y = PROGRESS_Y * display->GetScreenHeight() / 800;
	w = PROGRESS_W * display->GetScreenWidth() / 600;
	h = PROGRESS_H * display->GetScreenHeight() / 800;
    m_display = display;
    m_bar.SetMaximum(100);
    m_bar.SetDrawBold(false);
    m_bar.SetBarHeight(w);
    m_bar.MoveWindow(x, y, w, h);
}

CStartProgress::~CStartProgress()
{
    ProgressExit();
}

void CStartProgress::ShowStartPicture()
{
	const char *pFile =  DeviceInfo::IsKPW() ?  START_PIC_KP : START_PIC_KT;
    CDisplay::GetDisplay()->DrawFullScreenPicture(pFile);
}

int CStartProgress::ProgressExit()
{
    if (0 == m_tid)
    {
        return 0;
    }
    SetProgressData(-1, -1, -1);
    m_doExit = true;
    pthread_join(m_tid, NULL);
    m_tid = 0;
    return 0;
}

void  CStartProgress::SetProgress(int nCur)
{
    if(m_display)
    {
        m_bar.SetProgress(nCur);
        m_bar.Draw(m_display->GetMemDC());
        m_display->ScreenRepaint(x, y, w, h);
    }
}

void CStartProgress::Run()
{
    pthread_create(&m_tid, NULL, ProgressThread, (void*)this);
}

void *CStartProgress::ProgressThread(void *lparam)
{
    CStartProgress *pThis = (CStartProgress*)lparam;
    if(NULL == pThis)
    {
        return NULL;
    }
    ProgressData progressData;
    while(pThis->m_progressData.Receive(&progressData))
    {
        int start = progressData.start;
        int end = progressData.end;
        int nStep = progressData.step;
        if (start < 0)
        {
            return NULL;
        }
        for(int i = start; i <= end; i += nStep)
        {
            pThis->SetProgress(i);
            if (pThis->m_doExit)
            {
                return NULL;
            }
            usleep(1000*500);
        }
    }
    return NULL;
}
