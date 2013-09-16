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
    static DownloadManager* GetXiaoMiRequestInstance();
    
    void AddTask(DownloadTaskSPtr task);
    bool Start();
    static void RunTask(DownloadTaskSPtr task);
    void Clear();
private:
    void Run();
    static void* StartThreadFunc(void*);
    DownloadTaskSPtr GetTask();

private:
    typedef std::deque<DownloadTaskSPtr> DownloadTaskContainer;

private:
    DownloadTaskContainer m_downloadTaskContainer;
    pthread_mutex_t m_downloadTaskLock;
    pthread_cond_t m_downloadTaskCond;

    static int download_manager_instance_num_;
};
} // namespace dk
#endif
