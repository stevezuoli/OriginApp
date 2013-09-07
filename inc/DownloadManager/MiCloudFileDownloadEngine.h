#ifndef __MICLOUD_FILE_DOWNLOAD_ENGINE_H__
#define __MICLOUD_FILE_DOWNLOAD_ENGINE_H__

#include "IDownloadEngine.h"
#include "IDownloadTask.h"
#include <pthread.h>
#include <string>

class MiCloudFileDownloadEngine : public IDownloadEngine
{
public:

    MiCloudFileDownloadEngine(IDownloadTask* _de)
    {
        m_pdownloadtask = _de;
        m_taskworking = false;
        m_taskthread = 0;
    }

    ~MiCloudFileDownloadEngine()
    {
    }

    void Start();
    void Stop();
    int GetFileInfoFromServer(std::string _url,void* _pinfo);
    bool &TaskIsWorking()
    {
        return m_taskworking;
    }
private:
    void SetTaskWorking(bool _state)
    {
        m_taskworking = _state;
    }
    static void* TaskThread(void* pV);
    IDownloadTask *m_pdownloadtask;
    pthread_t   m_taskthread;
    bool        m_taskworking;
};

#endif//__DOWNLOADENGINE_H_
