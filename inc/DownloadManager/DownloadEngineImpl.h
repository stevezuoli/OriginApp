#ifndef __DOWNLOADENGINE_H_
#define __DOWNLOADENGINE_H_

#include "IDownloadEngine.h"
#include "IDownloadTask.h"
#include <pthread.h>
#include <string>

class SingleThreadDownloadEngineImpl : public IDownloadEngine
{
public:

    SingleThreadDownloadEngineImpl(IDownloadTask* _de)
    {
        m_pdownloadtask = _de;
        m_taskworking = false;
        m_taskthread = 0;
    }

    ~SingleThreadDownloadEngineImpl()
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
	static std::string GetExtensionNameAndScreenWebName(std::string _strFileName);
    static void* TaskThread(void* pV);
    IDownloadTask *m_pdownloadtask;
    pthread_t   m_taskthread;
    bool        m_taskworking;
};

#endif//__DOWNLOADENGINE_H_
