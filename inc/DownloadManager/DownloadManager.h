#ifndef __BOOKSTORE_DOWNLOADMANAGER_H__
#define __BOOKSTORE_DOWNLOADMANAGER_H__

#include "DownloadTask.h"

namespace dk
{
class DownloadManager
{
private:
    DownloadManager();
public:
    ~DownloadManager();
    static DownloadManager* GetInstance();
    
    void AddTask(DownloadTaskSPtr downloadTask);
    bool Start();
    static void RunTask(DownloadTaskSPtr task);
private:
    void Run();
    static void* StartThreadFunc(void*);
    DownloadTaskSPtr GetTask();
    typedef std::deque<DownloadTaskSPtr> DownloadTaskContainer;
    DownloadTaskContainer m_downloadTaskContainer;
    pthread_mutex_t m_downloadTaskLock;
    pthread_cond_t m_downloadTaskCond;
};
} // namespace dk
#endif
