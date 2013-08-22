#ifndef __BOOKSTORE_DOWNLOADTASK_H__
#define __BOOKSTORE_DOWNLOADTASK_H__

#include <tr1/memory>
#include "Mutex.h"
#include <map>
#include <deque>
#include <vector>
#include "GUI/EventSet.h"
#include "GUI/EventArgs.h"
#include "BookStore/BookInfo.h"

namespace dk
{
class DownloadTask;

class DownloadTaskFinishArgs: public EventArgs
{
public:
    virtual EventArgs* Clone() const
    {
        return new DownloadTaskFinishArgs(*this);
    }
    bool succeeded;
    DownloadTask* downloadTask;
};

class DownloadTask: public EventSet
{
public:
    static const char* EventDownloadTaskFinished;
    enum HTTP_METHOD
    {
        GET,
        POST,
        POSTFORM,
        PUT,
        DELETE
    };
    typedef std::map<std::string, std::string> PostData;
    typedef std::map<std::string, std::string> CookieData;
    typedef std::map<std::string, std::string> FormData;


    DownloadTask();
    virtual ~DownloadTask();

    void SetPriority(int priority)
    {
        m_priority = priority;
    }
    int GetPriority() const
    {
        return m_priority;
    }

    void AddCookie(const char* key, const char* value)
    {
        m_cookieData[key] = value;
    }

    void AddPostData(const char* key, const char* value);
    const PostData& GetPostData() const
    {
        return m_postData;
    } 
    std::string GetPostValue(const char* key) const;

    void AddFormData(const char* name, const char* contents);
    const FormData& GetFormData() const
    {
        return m_formData;
    }

    void SetMethod(HTTP_METHOD method)
    {
        m_method = method;
    }
    HTTP_METHOD GetMethod() const
    {
        return m_method;
    }

    void SetUrl(const char* url)
    {
        m_url = url;
    }
    const char* GetUrl() const
    {
        return m_url.c_str();
    }

    void SetTimeoutInSeconds(int timeoutInSeconds)
    {
        m_timeoutInSeconds = timeoutInSeconds;
    }
    int GetTimeoutInSeconds() const
    {
        return m_timeoutInSeconds;
    }

    void SetDestFile(const char* dstFile)
    {
        m_dstFilePath = dstFile;
    }

    void SetPostFile(const char* postFileField, const char* filePath)
    {
        m_postFileField.assign(postFileField);
        m_postFilePath.assign(filePath);
    }
    std::string GetPostFileField() const
    {
        return m_postFileField;
    }
    std::string GetPostFilePath() const
    {
        return m_postFilePath;
    }

    void OnFinished(bool success);
    static long WriteData(void* data, int size, int nmemb, void* userp);
    std::string GetString() const;
    std::string GetCookieString() const;
    virtual std::string GetAllPostDataAsString() const;

    void SetUserData(const std::string& userData) { m_userData = userData; }
    std::string GetUserData() const { return m_userData; }

protected:
    std::string GetTmpFilePath()
    {
        return m_dstFilePath + ".tmp";
    }

    bool IsOutputToFile() const
    {
        return !m_dstFilePath.empty();
    }
    long ReceiveData(void* data, int size, int nmemb);
    long ReceiveDataToFile(void* data, int size, int nmemb);
    long ReceiveDataToBuffer(void* data, int size, int nmemb);
    bool EnsureFileOpen();

    int m_timeoutInSeconds;
    std::string m_url;
    PostData m_postData;
    CookieData m_cookieData;
    FormData m_formData;
    std::vector<unsigned char> m_data;
    std::string m_dstFilePath;
    int m_priority;
    HTTP_METHOD m_method;
    FILE* m_file;
    std::string m_postFileField;
    std::string m_postFilePath;//absolute path
    std::string m_userData;
}; // class DownloadTask

class BookOrderDownloadTask: public DownloadTask
{
public:
    BookOrderDownloadTask();
    virtual ~BookOrderDownloadTask();

    void SetBooks(const bookstore::model::BookInfoList& src);
    const bookstore::model::BookInfoList& GetBooks() const;

    virtual std::string GetAllPostDataAsString() const;
private:
    bookstore::model::BookInfoList m_refBooks;
};

typedef std::tr1::shared_ptr<DownloadTask> DownloadTaskSPtr;
} // namespace dk
#endif
