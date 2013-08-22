#include "Utility/ThreadHelper.h"
#include "pthread.h"
#include "interface.h"



vector<ThreadStruct>     ThreadHelper::m_vThreadList;

int ThreadHelper::CreateThread(pthread_t* tidp, void*(*start_rtn)(void*), void* arg, string threadName, bool detached, size_t stackSize, int priority)
{
    ThreadStruct _pstr;

    pthread_attr_t attr;
    int err = pthread_attr_init(&attr);
    if (err != 0)
        goto labErr0;
    if (detached)
    {
        err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if (err != 0)
        {
            goto labErr1;
        }
    }
    if(priority <= 0)
    {//默认创建的线程都比较高优先级. 在声明优先级的线程中，sqm>check新版本>连载书更新
        priority = THREAD_DEFAULT_PRIORITY;
    }
    
    struct sched_param param;
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    param.sched_priority = priority;
    pthread_attr_setschedparam(&attr, &param);

    if (0 == stackSize)
    {
        stackSize = THREAD_STACK_SIZE;
    }
    if (stackSize > 0)
    {
        err = pthread_attr_setstacksize(&attr, stackSize);
        size_t stackSizeAfterSet = 0;
        pthread_attr_getstacksize(&attr, &stackSizeAfterSet);
        DebugPrintf(DLC_THREAD, "ThreadHelper::CreateThread(): set stack size to %d", stackSizeAfterSet);
        if (err != 0)
        {
            goto labErr1;
        }

    }

    err = pthread_create(tidp, &attr, start_rtn, arg);


    _pstr.ThreadID = *tidp;
    _pstr.strThreadName = threadName;
    m_vThreadList.push_back(_pstr);

    // pass through
labErr1:
    pthread_attr_destroy(&attr);
labErr0:
    return err;
}

int ThreadHelper::JoinThread( pthread_t threadID, void* pUseless)
{
    pthread_join(threadID, NULL);

    for(vector<ThreadStruct>::iterator itr = ThreadHelper::m_vThreadList.begin() ; 
                                   itr != ThreadHelper::m_vThreadList.end(); 
                                   itr++)
    if (threadID == itr->ThreadID)
        {
            ThreadHelper::m_vThreadList.erase(itr);
            break;
        }

    return 0;
}

int ThreadHelper::CancelThread( pthread_t threadID)
{
    pthread_cancel(threadID);
    return 0;
}

int ThreadHelper::ThreadExit(void * value_ptr)
{
    pthread_t threadID = pthread_self();
    for(vector<ThreadStruct>::iterator itr = ThreadHelper::m_vThreadList.begin() ; 
                                   itr != ThreadHelper::m_vThreadList.end(); 
                                   itr++)
    if (threadID == itr->ThreadID)
    {
        ThreadHelper::m_vThreadList.erase(itr);
        break;
    }
	
    // pthread_exit(value_ptr);
	return 0;
}
