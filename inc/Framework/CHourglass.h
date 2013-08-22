////////////////////////////////////////////////////////////////////////
// $Source: ./inc/Framework/CDisplay.h $
// $Revision: 17 $
// Contact: JZN
// Copyright (c) Duokan Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////

#ifndef CHOURGLASS_H_
#define CHOURGLASS_H_

//#include "Framework/ISystemMessageListener.h"
//#include "Framework/CMessageHandler.h"
//#include "Framework/INativeDisplay.h"
//#include "GUI/UIPage.h"
//#include "Utility/IRunnable.h"
#include "GUI/UIImage.h"
#include "DkSPtr.h"
//#include "Framework/INativeMessageQueue.h"
//#include "dkImage.h"
//#include "drivers/CFbBitDc.h"

//using namespace DkFormat;

class CHourglass
{
private:
   CHourglass();


    static CHourglass *gpiInstanceInstance;
    pthread_t m_pThread;
    SPtr<ITpImage> m_pImg;
    
    pthread_cond_t m_WaitCond;
    pthread_mutex_t m_WaitMutex;

    pthread_cond_t m_StopCond;
    pthread_mutex_t m_StopMutex;
    
    INT32 m_iTop;
    INT32 m_iLeft;

    INT32 m_iCurrentPage;
    BOOL m_bWork;    
public:
    ~CHourglass();
    
    static CHourglass* GetInstance();
    static void Initialize();
    static void Finalize();
    INT32 Run();
    bool  IsRunning();

    void SetWindow(INT32 iLeft, INT32 iTop);
    void Start(INT32 iLeft, INT32 iTop);
    void Start();
    void Stop();

};


#endif /*CUIMESSAGEDISPATCHER_H_*/


