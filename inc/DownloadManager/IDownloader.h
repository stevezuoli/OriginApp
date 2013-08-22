#ifndef __IDOWNLOADER_H__
#define __IDOWNLOADER_H__

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <pthread.h>
#include "singleton.h"
#include "IDownloadTask.h"
#include "GUI/EventSet.h"
#include "GUI/EventArgs.h"


#define DOWNLOADPATH "/mnt/us/DK_Download/"
#define DOWNLOADHISTORY "/mnt/us/DK_Download/DLHistroy.dat"

typedef std::map<int, int> DownloadTaskNums;
typedef std::map<int, DownloadTaskNums> AllDownloadTaskNums;

int GetDownloadTaskNums(DownloadTaskNums& tasks, int state);

class DownloadUpdateEventArgs: public EventArgs
{
public:
    DownloadUpdateEventArgs();
    virtual ~DownloadUpdateEventArgs() {}
    virtual EventArgs* Clone() const
    {
        return new DownloadUpdateEventArgs(*this);
    }

public:
    std::string taskId;
    int percentage;
    IDownloadTask::DLState state;
};

typedef std::vector<DownloadUpdateEventArgs> DownloadUpdateEventArgsList;

class IDownloader: public EventSet
{
public:
    static const char* EventDownloadProgressUpdate;
    static IDownloader* GetInstance();

    virtual ~IDownloader(){};

    virtual void StartDownload() = 0;
    virtual void StopDownload(int stopState = IDownloadTask::PAUSED) = 0;

    virtual bool  IsDownloading(std::string _url) = 0;

    virtual void AddTask(IDownloadTask * _pIDownloadTask) = 0;
    virtual void CancelTask(const char* urlId) = 0;
    void CancelTask(const std::string& urlId) { CancelTask(urlId.c_str()); }
    virtual void PauseTask(std::string _url) = 0;
    virtual void PauseAllTasks(int stopState = IDownloadTask::PAUSED) = 0;
    virtual void ResumeTask(std::string _url) = 0;
    virtual void ResumeAllPauses() = 0;

    //virtual std::string  GetDownloadingTaskUrlID() = 0;
    //virtual int  GetDownloadingTaskPercentage() = 0;
    //virtual int  GetDownloadTaskNumberByType(int& percentage,
    //                                         const int state = IDownloadTask::WAITING | IDownloadTask::WORKING,
    //                                         const int type = IDownloadTask::BOOK) = 0;

    virtual void DeleteTask(const char* urlId) = 0;
    void DeleteTask(const std::string& urlId){ DeleteTask(urlId.c_str()); }
    virtual void DeleteTasks(const int state = IDownloadTask::WAITING | IDownloadTask::PAUSED | IDownloadTask::CANCELED | IDownloadTask::FAILED,
                             const int type = IDownloadTask::BOOK) = 0;

    //TODO:暂时用不到。
    virtual void ClearNotExisted() = 0;
    virtual void ClearDone() = 0;
    virtual void ClearCanceled() = 0;
    virtual void ClearTask(const int state = IDownloadTask::WAITING | IDownloadTask::PAUSED | IDownloadTask::CANCELED | IDownloadTask::FAILED,
                           const int type = IDownloadTask::BOOK) = 0;

    virtual void SetMaxDownloadTaskNum(int _nm) = 0;
    virtual int  GetMaxDownloadTaskNum() = 0;
    virtual int  GetDownloadTaskNum() = 0;

    // progress from 0 to 100
    //virtual int GetPercentage(const char* urlId) = 0;
    virtual IDownloadTask * GetTaskInfoByIndex(int _index) = 0;
    virtual IDownloadTask * GetTaskInfoByUrlId(const char* urlId) = 0;
    virtual int GetTaskPercentageByUrlId(std::string _urlID) = 0;
    virtual IDownloadTask::DLState GetTaskStateByUrlId(std::string _urlID) = 0;
    virtual const std::string GetTaskFileNameByUrlId(std::string _urlID) = 0;
    virtual bool CheckSomeTaskIsExistByUrlId(std::string _urlID) = 0;
    virtual void SaveHistroy() = 0;

    // Status Update
    void FireDownloadProgressUpdateEvent(IDownloadTask::DLState state, int percentage, std::string urlID = std::string());

    virtual AllDownloadTaskNums UpdateAllDownloadTaskNums() = 0;
    virtual DownloadTaskNums UpdateDownloadTaskNums(int type) = 0;

protected:
    dk::common::LockObject m_lock;
};

#endif //__protected:
