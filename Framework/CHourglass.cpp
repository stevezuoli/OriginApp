////////////////////////////////////////////////////////////////////////
// $Source:  $
// $Revision: 24 $
// Contact: jzn
// Latest submission: $Date: 2008/08/25 20:50:27 $ by $Author:  $
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////
#include <pthread.h>
#include <errno.h>
#include "interface.h"
#include   <unistd.h>

#include "Utility/ImageManager.h"
#include "Framework/CNativeThread.h"
#include "Framework/CMessageHandler.h"
#include "GUI/FontManager.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ImageManager.h"
#include "Utility/ColorManager.h"
#include "Utility/ThreadHelper.h"
#include "Framework/CDisplay.h"
#include "Framework/CHourglass.h"

//#include "Utility/ImageManager.h"

CHourglass *CHourglass::gpiInstanceInstance = NULL;

static void* ThreadFuncCHourglass (void* pVoid)
{
    CHourglass *pCHourglass = (CHourglass *)pVoid;
    pCHourglass->Run();
    ThreadHelper::ThreadExit(NULL);
    return NULL;
}

CHourglass::CHourglass()
{
    m_pImg = ImageManager::GetImage(IMAGE_WAIT_HOURGLASS);
    pthread_mutex_init(&m_WaitMutex, NULL);
    pthread_cond_init (&m_WaitCond, NULL);
    pthread_mutex_init(&m_StopMutex, NULL);
    pthread_cond_init (&m_StopCond, NULL);
    m_iLeft = 0;
    m_iTop = 0;
    m_iCurrentPage = 0;
    m_bWork = FALSE;
}

CHourglass::~CHourglass()
{
    ThreadHelper::CancelThread(m_pThread);
    ThreadHelper::JoinThread(m_pThread, NULL);
    m_pThread = 0;
    pthread_mutex_destroy(&m_WaitMutex);
    pthread_cond_destroy(&m_WaitCond);
    pthread_mutex_destroy(&m_StopMutex);
    pthread_cond_destroy(&m_StopCond);
}

CHourglass* CHourglass::GetInstance()
{
    return gpiInstanceInstance;
}

void CHourglass::Initialize()
{
    if (NULL == gpiInstanceInstance)
    {
        gpiInstanceInstance = new CHourglass();


        // FIXME: joinable, or detached?
        ThreadHelper::CreateThread(&(gpiInstanceInstance->m_pThread), ThreadFuncCHourglass, gpiInstanceInstance, "CHourglass @ ThreadFuncCHourglass", true);
    }
}

void CHourglass::Finalize()
{
    if (gpiInstanceInstance != NULL)
    {
        delete gpiInstanceInstance;
        gpiInstanceInstance = NULL;
    }
}

INT32 CHourglass::Run()
{
    INT32 iCount = m_pImg.Get()->GetHeight() / m_pImg.Get()->GetWidth();
    unsigned long timeout = 300;
    pthread_mutex_lock(&m_WaitMutex);
        CDisplay* pDisplay = CDisplay::GetDisplay();
    while(TRUE)
    {
        pthread_cond_wait (&m_WaitCond, &m_WaitMutex);
        while(m_bWork)
        {
            pDisplay->ScreenShowImageRepaint(m_iLeft, m_iTop, m_pImg.Get()->GetWidth(), m_pImg.Get()->GetWidth(), m_pImg.Get(), 0, m_pImg.Get()->GetWidth() * m_iCurrentPage);
            m_iCurrentPage++;
            if(m_iCurrentPage >= iCount)
                m_iCurrentPage = 0;
            usleep(timeout * 1000);
        }
        pDisplay->ScreenShowImageRepaint(m_iLeft, m_iTop, m_pImg.Get()->GetWidth(), m_pImg.Get()->GetWidth(), NULL, 0, 0);
        pthread_mutex_lock(&m_StopMutex);
        pthread_cond_signal (&m_StopCond);
        pthread_mutex_unlock(&m_StopMutex);

    }
    pthread_mutex_unlock(&m_WaitMutex);
    return 0;
}

void CHourglass::SetWindow(INT32 iLeft, INT32 iTop)
{
    INT32 iImageWidth = 0;
    INT32 iImageHeight = 0;
    CDisplay* pDisplay = CDisplay::GetDisplay();
    if(m_pImg)
    {
        iImageWidth = m_pImg.Get()->GetWidth();
        iImageHeight = m_pImg.Get()->GetHeight();
    }

    if(iLeft < 0 || iTop < 0 || iLeft > pDisplay->GetScreenWidth() - iImageWidth || iTop > pDisplay->GetScreenHeight() - iImageHeight)
    {
        iLeft = 0;
        iTop = 0;
    }
    m_iLeft = iLeft;
    m_iTop = iTop;
}

void CHourglass::Start(INT32 iLeft, INT32 iTop)
{
    if(m_bWork)//If Newer start, we must make sure the Stop() was called!!! Otherwise, it will cause deadlock!!!
    {
        Stop();
    }
    SetWindow(iLeft, iTop);
    pthread_mutex_lock(&m_WaitMutex);
    m_bWork = TRUE;
    m_iCurrentPage = 0;
    pthread_cond_signal (&m_WaitCond);
    pthread_mutex_unlock(&m_WaitMutex);

}

void CHourglass::Start()
{

   pthread_mutex_lock(&m_WaitMutex);
    m_bWork = TRUE;
   m_iCurrentPage = 0;
   pthread_cond_signal (&m_WaitCond);
   pthread_mutex_unlock(&m_WaitMutex);
}

void CHourglass::Stop()
{
    if(!m_bWork)
    {
        return;
    }
    unsigned long timeoutSec = 3;
    struct timeval  abstv = {1, 0};
    struct timespec absts = {1, 0};
    int result = 0;
    pthread_mutex_lock(&m_StopMutex);
    gettimeofday(&abstv, NULL); // grrr
    absts.tv_sec = abstv.tv_sec + timeoutSec;
    m_bWork = FALSE;
    result = pthread_cond_timedwait(&m_StopCond, &m_StopMutex, &absts);
    if(result == ETIMEDOUT)
    {
        DebugPrintf(DLC_ERROR, "pthread_cond_timedwait == ETIMEDOUT");
    }

    pthread_mutex_unlock(&m_StopMutex);
}

bool CHourglass::IsRunning()
{
    DebugPrintf(DLC_JUGH, "CHourglass::IsRunning : (%d, %d)", m_bWork, m_iCurrentPage);    
    return (m_bWork == TRUE);
}
