#ifndef _THREADHELPER_H
#define _THREADHELPER_H

#include <limits.h>
#include <pthread.h>
#include <vector>
#include <string>

using std::string;
using std::vector;

const int THREAD_STACK_SIZE = PTHREAD_STACK_MIN;
const int THREAD_DEFAULT_PRIORITY = 35;
const int SQM_THREAD_PRIORITY = 30;
const int UPGRADE_THREAD_PRIORITY = 25;
const int SERIALIZEDBOOK_THREAD_PRIORITY = 20;

// default stack size without set stack size, from running test program on Kindle
const int THREAD_STACK_SIZE_KINDLE_DEFAULT = 8*1024*1024;

typedef struct
{
    pthread_t   ThreadID;
    string      strThreadName;
} ThreadStruct;


class ThreadHelper
{
public:
    static int CreateThread(
            pthread_t* tidp, 
            void*(*start_rtn)(void*), 
            void* arg, 
            string threadName,
            bool detached = false, 
            size_t stackSize = 0,
            int priority = 0);

    static int JoinThread( pthread_t threadID, void* pUseless);
    static int CancelThread( pthread_t threadID);
    static int ThreadExit(void * value_ptr);

private:
    static vector<ThreadStruct> m_vThreadList;

private:
    ThreadHelper();
};


#endif
