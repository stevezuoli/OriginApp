#include "BookStore/XiaomiListDataFetcher.h"
#include "BookStore/DataUpdateArgs.h"
#include "Utility/JsonObject.h"
#include "Utility/StringUtil.h"

namespace dk
{
namespace bookstore
{

const char* XiaomiListDataFetcher::EventXiaomiListUpdate = "EventXiaomiListUpdate";

XiaomiListDataFetcher::XiaomiListDataFetcher(
    const std::tr1::function<DownloadTaskSPtr(const std::vector<std::string>&)>& downloadTaskBuilder,
    model::ObjectType objectType,
    const char* dataKeyName,
    const char* resultKeyName,
    const char* descriptionKeyName)
    : m_downloadTaskBuilder(downloadTaskBuilder)
    , m_objectType(objectType)
    , m_dataKeyName(dataKeyName)
    , m_resultKeyName(resultKeyName)
    , m_descriptionKeyName(descriptionKeyName)
{
    pthread_mutex_init(&m_lock, 0);
}

XiaomiListDataFetcher::~XiaomiListDataFetcher()
{
    pthread_mutex_destroy(&m_lock);
}

FetchDataResult XiaomiListDataFetcher::FetchData(const std::vector<std::string>& variants)
{
    if (variants.empty())
    {
        return FDR_FAILED;
    }
    AutoLock lock(&m_lock);
    return FetchRemoteData(variants);
}

FetchDataResult XiaomiListDataFetcher::FetchRemoteData(const std::vector<std::string>& variants)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "XiaomiListDataFetcher", __FUNCTION__);
    if (IsBusy())
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s return FDR_PENDING", __FILE__,  __LINE__, "XiaomiListDataFetcher", __FUNCTION__);
        return FDR_PENDING;
    }

    DownloadTaskSPtr downloadTask = BuildDownloadTask(variants);
    EventListener::SubscribeEvent(DownloadTask::EventDownloadTaskFinished, 
        *downloadTask, 
        std::tr1::bind(std::tr1::mem_fn(&XiaomiListDataFetcher::OnDownloadFinished),
        this, 
        std::tr1::placeholders::_1));

    DownloadManager::GetInstance()->AddTask(downloadTask);
    m_currentTask = downloadTask;
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s END", __FILE__,  __LINE__, "XiaomiListDataFetcher", __FUNCTION__);
    return FDR_PENDING;
}

bool XiaomiListDataFetcher::OnDownloadFinished(const EventArgs& args)
{
    AutoLock lock(&m_lock);
    const DownloadTaskFinishArgs& taskArgs = (const DownloadTaskFinishArgs&)args;
    // 有可能DownloadTask被重设
    if (taskArgs.downloadTask != m_currentTask.get())
    {
        return false;
    }

    XiaomiDataUpdateArgs listDataUpdateArgs;
    if (taskArgs.succeeded)
    {
        std::string fetchedData = m_currentTask->GetString();
        if (ParseData(fetchedData.c_str(),
                      &listDataUpdateArgs.dataList,
                      listDataUpdateArgs.total,
                      listDataUpdateArgs.result,
                      listDataUpdateArgs.description))
        {
            listDataUpdateArgs.succeeded = true;
        }
    }

    FireEvent(EventXiaomiListUpdate, listDataUpdateArgs);
    m_currentTask.reset();
    return true;
}

bool XiaomiListDataFetcher::ParseData(
        const char* data,
        std::vector<model::BasicObjectSPtr>* dataList,
        int& total,
        std::string& result,
        std::string& description)
{
    if (NULL == data)
    {
        return false;
    }
    JsonObjectSPtr rootObj = JsonObject::CreateFromString(data);
    if (rootObj == 0)
    {
        return false;
    }
    
    rootObj->GetStringValue(GetResultKeyName(), &result);
    if (dk::utility::StringUtil::ToLower(result.c_str()) != "ok")
    {
        return false;
    }

    rootObj->GetStringValue(GetDescriptionKeyName(), &description);

    JsonObjectSPtr dataObj = rootObj->GetSubObject(GetDataKeyName());
    if (data == 0)
    {
        return false;
    }

    JsonObjectSPtr items = dataObj->GetSubObject("list");
    if (items == 0)
    {
        return false;
    }
    
    total = items->GetArrayLength();
    for (int i = 0; i < total; ++i)
    {
        JsonObjectSPtr item = items->GetElementByIndex(i);
        if (item == 0)
        {
            continue;
        }
        model::BasicObject* basicObject = model::BasicObject::CreateBasicObject(item.get(), m_objectType);
        if (basicObject)
        {
            dataList->push_back(model::BasicObjectSPtr(basicObject));
        }
    }
    PostParseData(rootObj.get());
    return true;
}

DownloadTaskSPtr XiaomiListDataFetcher::BuildDownloadTask(const std::vector<std::string>& variants)
{
    return m_downloadTaskBuilder(variants);
}

} // namespace bookstore
} // namespace dk
