#ifndef __DownloaderImpl_H__
#define __DownloaderImpl_H__

#include <semaphore.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <list>
#include "../Common/Mutex.h"
#include "DownloadManager/IDownloader.h"
#include "DownloadManager/IDownloadEngine.h"
#include "DownloadManager/IDownloadTask.h"
#include "singleton.h"
#include "interface.h"
#include "Common/LockObject.h"

class DownloaderImpl : public IDownloader
{
    DownloaderImpl(void);
public:
    static IDownloader* GetInstance();
    virtual ~DownloaderImpl(void);

    virtual void StartDownload();
    virtual void StopDownload(int stopState = IDownloadTask::PAUSED);

    virtual bool IsDownloading(std::string _url);

    virtual void AddTask(IDownloadTask *_pDownloadTask);
    virtual void DeleteTask(const char* urlId);
    virtual void DeleteTasks(const int state = IDownloadTask::WAITING | IDownloadTask::PAUSED | IDownloadTask::CANCELED | IDownloadTask::FAILED,
                             const int type = IDownloadTask::BOOK);
    virtual void CancelTask(const char* urlId) ;
    virtual void PauseTask(std::string _url);
    virtual void PauseAllTasks(int stopState = IDownloadTask::PAUSED);
    virtual void ResumeTask(std::string _url) ;
    virtual void ResumeAllPauses();

    //virtual std::string  GetDownloadingTaskUrlID();
    //virtual int  GetDownloadingTaskPercentage();
    //virtual int  GetDownloadTaskNumberByType(int& percentage,
    //                                         const int state = IDownloadTask::WAITING | IDownloadTask::WORKING,
    //                                         const int type = IDownloadTask::BOOK);

    virtual void ClearTask(const int state = IDownloadTask::WAITING | IDownloadTask::PAUSED | IDownloadTask::CANCELED | IDownloadTask::FAILED,
                           const int type = IDownloadTask::BOOK);
    virtual void ClearNotExisted();
    virtual void ClearDone();
    virtual void ClearCanceled();

    virtual void SetMaxDownloadTaskNum(int _num);
    virtual int  GetMaxDownloadTaskNum();

    virtual int  GetDownloadTaskNum();
    virtual IDownloadTask * GetTaskInfoByIndex(int _index);
    virtual IDownloadTask * GetTaskInfoByUrlId(const char* urlId);
    
    virtual IDownloadTask::DLState GetTaskStateByUrlId(std::string _urlID);
    virtual const std::string GetTaskFileNameByUrlId(std::string _urlID);
    virtual bool CheckSomeTaskIsExistByUrlId(std::string _urlID);
    virtual bool CanTaskResume(std::string _urlID);
    virtual bool IsUploadTask(std::string _urlID);
    virtual void SaveHistroy();

    //virtual int GetPercentage(const char* urlId);
    virtual int GetTaskPercentageByUrlId(std::string _urlID);

    virtual AllDownloadTaskNums UpdateAllDownloadTaskNums();
    virtual DownloadTaskNums UpdateDownloadTaskNums(int type);


private:
    struct thread_args
    {
        DownloaderImpl* m_this ;
        // other args as required; in this case
        void* actual_arg ;
        thread_args( DownloaderImpl* t, void* p )
        : m_this(t), actual_arg(p) {};
    };

private:
    void LoadHistroy();
    int GetWorkingTaskNumber();
    std::string ReplaceHiddenCharactersWithSpace(std::string _filename);
    bool IsPthreadWorking(){ return m_threadWorking; };
    static void* TaskQueueThread(void* pV);
    void* RunTask();
    void DoAddTask(IDownloadTask* downloadTask);
    void GetDownloadUpdateArgs(IDownloadTask::DLState state, std::string urlID, DownloadUpdateEventArgs& args);

private:
    std::list<IDownloadTask*> m_tasks;
    pthread_t   m_taskThread;
    int         m_maxtasknum;
    sem_t       m_sem;
    volatile bool m_threadWorking;
    AllDownloadTaskNums m_downloadTaskNums;
};

#endif //__DownloaderImpl_H__
