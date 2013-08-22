#include "DownloadManager/DownloadTask.h"
#include "curl/curl.h"
#include "Utility/ThreadHelper.h"
#include "KernelDef.h"
#include "DKLogger.h"  
#include "interface.h"
#include "Utility/HttpUtil.h"

using namespace dk::utility;

namespace dk
{
const char* DownloadTask::EventDownloadTaskFinished = "DownloadTaskFinished";

DownloadTask::DownloadTask()
    : m_timeoutInSeconds(30)
    , m_priority(-1)
    , m_method(GET)
    , m_file(NULL)
{
}

DownloadTask::~DownloadTask()
{
    if (NULL != m_file)
    {
        fclose(m_file);
    }
}

long DownloadTask::WriteData(void* data, int size, int nmemb, void* userp)
{
    return ((DownloadTask*)userp)->ReceiveData(data, size, nmemb);
}

long DownloadTask::ReceiveData(void* data, int size, int nmemb)
{
    if (IsOutputToFile())
    {
        return ReceiveDataToFile(data, size, nmemb);
    }
    else
    {
        return ReceiveDataToBuffer(data, size, nmemb);
    }
}

long DownloadTask::ReceiveDataToFile(void* data, int size, int nmemb)
{
    if (!EnsureFileOpen())
    {
        return -1;
    }
    return fwrite(data, size, nmemb, m_file) * size;
}

long DownloadTask::ReceiveDataToBuffer(void* data, int size, int nmemb)
{
    DebugPrintf(DLC_BOOKSTORE, "Receive data, size:%d, nmemb: %d", size, nmemb);
    unsigned char* bytes = (unsigned char*)data;
    unsigned char* bytesEnd = bytes + size * nmemb;
    m_data.insert(m_data.end(), bytes, bytesEnd);
    return bytesEnd - bytes;
}

bool DownloadTask::EnsureFileOpen()
{
    if (NULL != m_file)
    {
        return true;
    }
    if (m_dstFilePath.empty())
    {
        return false;
    }

    m_file = fopen(GetTmpFilePath().c_str(), "wb");
    return NULL != m_file;
}

void DownloadTask::OnFinished(bool succeeded)
{
    DebugPrintf(DLC_BOOKSTORE, "Download finsished: result(%d), data: %s", succeeded, GetString().c_str());
    if (NULL != m_file)
    {
        fclose(m_file);
        m_file = NULL;
        if (succeeded)
        {
            rename(GetTmpFilePath().c_str(), m_dstFilePath.c_str());
        }
    }
    DownloadTaskFinishArgs args;
    args.succeeded = succeeded;
    args.downloadTask = this;
    FireEvent(EventDownloadTaskFinished, args);
}

std::string DownloadTask::GetString() const
{
    if (IsOutputToFile())
    {
        return "";
    }
    std::vector<unsigned char> data(m_data);
    data.push_back(0);
    return std::string((const char*)&data[0]);
}

std::string DownloadTask::GetCookieString() const
{
    std::string result;
    for (DK_AUTO(pos, m_cookieData.begin());
            pos != m_cookieData.end();
            ++pos)
    {
        result += pos->first + "=" + pos->second + ";";
    }
    return result;
}

std::string DownloadTask::GetPostValue(const char* key) const
{
    if (m_method != POST)
    {
        return "";
    }

    DK_AUTO(pos, m_postData.find(key));
    if (pos == m_postData.end())
    {
        return "";
    }
    return pos->second;
}

void DownloadTask::AddPostData(const char* key, const char* value)
{
    m_postData[key] = value;
}

void DownloadTask::AddFormData(const char* name, const char* contents)
{
    m_formData[name] = contents;
}

std::string DownloadTask::GetAllPostDataAsString() const
{
    std::string result;
    for (DK_AUTO(cur, m_postData.begin());
            cur != m_postData.end();
            ++cur)
    {
        result
            .append(cur->first)
            .append("=")
            .append(HttpUtil::UrlEncode(cur->second.c_str()))
            .append("&");
    }
    if (!result.empty())
    {
        return result.substr(0, result.size() - 1);
    }
    else
    {
        return result;
    }
}

BookOrderDownloadTask::BookOrderDownloadTask()
    : DownloadTask()
{
}

BookOrderDownloadTask::~BookOrderDownloadTask()
{
}

const bookstore::model::BookInfoList& BookOrderDownloadTask::GetBooks() const
{
    return m_refBooks;
}

void BookOrderDownloadTask::SetBooks(const bookstore::model::BookInfoList& src)
{
    m_refBooks = src;
}

std::string BookOrderDownloadTask::GetAllPostDataAsString() const
{
    std::string result;
    for (DK_AUTO(cur, m_postData.begin());
            cur != m_postData.end();
            ++cur)
    {
        std::string value = cur->second;
        if (cur->first != "item" || value.find("item") == std::string::npos)
        {
            value = HttpUtil::UrlEncode(cur->second.c_str());
        }

        result
            .append(cur->first)
            .append("=")
            .append(value)
            .append("&");
    }
    if (!result.empty())
    {
        return result.substr(0, result.size() - 1);
    }
    else
    {
        return result;
    }
}

} // namespace dk
