#include "DownloadManager/DownloadManager.h"
#include "curl/curl.h"
#include "Utility/ThreadHelper.h"
#include "Utility/StringUtil.h"
#include "KernelDef.h"
#include "interface.h"

namespace dk
{

int DownloadManager::download_manager_instance_num_ = 0;

DownloadManager::DownloadManager()
{
    // make sure curl_global_init is called once
    if (download_manager_instance_num_ == 0)
    {
        curl_global_init(CURL_GLOBAL_ALL);
    }
    download_manager_instance_num_++;

    pthread_mutex_init(&m_downloadTaskLock, 0);
    pthread_cond_init(&m_downloadTaskCond, 0);
}

DownloadManager::~DownloadManager()
{
    pthread_cond_destroy(&m_downloadTaskCond);
    pthread_mutex_destroy(&m_downloadTaskLock);

    download_manager_instance_num_--;
    if (download_manager_instance_num_ == 0)
    {
        curl_global_cleanup();
    }
}

DownloadManager* DownloadManager::GetInstance()
{
    static DownloadManager s_downloadManager;
    return &s_downloadManager;
}

DownloadManager* DownloadManager::GetXiaoMiRequestInstance()
{
    static DownloadManager s_downloadManager;
    return &s_downloadManager;
}

DownloadTaskSPtr DownloadManager::GetTask()
{
    AutoLock lock(&m_downloadTaskLock);
    if (m_downloadTaskContainer.empty())
    {
        pthread_cond_wait(&m_downloadTaskCond, &m_downloadTaskLock);
    }
    DownloadTaskSPtr result = m_downloadTaskContainer.back();
    // LIFO
    m_downloadTaskContainer.pop_back();
    return result;
}

void DownloadManager::RunTask(DownloadTaskSPtr task)
{
    if (!task || !task->PrepareData())
    {
        return;
    }

    CURL* curl = curl_easy_init();
    if (curl)
    {
        curl_slist* headers = NULL;
        struct curl_httppost* post_start = NULL;
        struct curl_httppost* post_end = NULL;

        curl_easy_setopt(curl, CURLOPT_URL, task->GetUrl());
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        if (utility::StringUtil::StartWith(task->GetUrl(), "https"))
        {
#ifndef _X86_LINUX
            curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ssl/certs/ca-certificates.crt");
            //curl_easy_setopt(curl, CURLOPT_CAINFO, "/DuoKan/cacert.pem");
#endif
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
        }
        std::string postData = task->GetAllPostDataAsString();
        if (DownloadTask::POST == task->GetMethod())
        {
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        }
        else if (DownloadTask::PUT == task->GetMethod())
        {
            curl_easy_setopt(curl, CURLOPT_PUT, 1L);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, 0);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE, 0);
        }
        else if (DownloadTask::DELETE == task->GetMethod())
        {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,"DELETE");
        }
        else if (DownloadTask::POSTFORM == task->GetMethod())
        {
            headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            
            for (DK_AUTO(cur, task->GetFormData().begin()); cur != task->GetFormData().end(); ++cur)
            {
                curl_formadd(&post_start, &post_end,
                    CURLFORM_COPYNAME, cur->first.c_str(),
                    CURLFORM_COPYCONTENTS, cur->second.c_str(),
                    CURLFORM_END);
            }
            std::string fileField = task->GetPostFileField();
            std::string filePath = task->GetPostFilePath();
            if (!fileField.empty() && !filePath.empty())
            {
                curl_formadd(&post_start, &post_end,
                    CURLFORM_COPYNAME, fileField.c_str(),
                    CURLFORM_FILE, filePath.c_str(),
                    CURLFORM_END);
            }

            curl_easy_setopt(curl, CURLOPT_HTTPPOST, post_start);
        }
        curl_easy_setopt(curl, CURLOPT_COOKIE, task->GetCookieString().c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, task->GetTimeoutInSeconds());
        curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
#ifdef _DEBUG
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
#endif
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, task.get());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DownloadTask::WriteData);

       	CURLcode res = curl_easy_perform(curl);
		long retcode = 0;
		if(CURLE_OK == res)
		{
      		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		}

        curl_easy_cleanup(curl);
        curl_formfree(post_start);
        curl_slist_free_all(headers);
        
        task->SetRetCode(retcode);
        task->OnFinished((res == CURLE_OK) && retcode == 200);
    }
} 

void DownloadManager::AddTask(DownloadTaskSPtr task)
{
    AutoLock lock(&m_downloadTaskLock);
    m_downloadTaskContainer.push_back(task);
    pthread_cond_signal(&m_downloadTaskCond);
}

void DownloadManager::Run()
{
    for(;;)
    {
        RunTask(GetTask());
        usleep(20000);
    }
}

void* DownloadManager::StartThreadFunc(void* arg)
{
    DownloadManager* downloadManager = (DownloadManager*)arg;
    downloadManager->Run();
    return NULL;
}

bool DownloadManager::Start()
{
    pthread_t tid = 0;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    bool result = (0 == pthread_create(&tid, &attr, StartThreadFunc, this));
    pthread_attr_destroy(&attr);
    return result;
}

void DownloadManager::Clear()
{
    AutoLock lock(&m_downloadTaskLock);
    m_downloadTaskContainer.clear();
}

} // namespace dk
