#include "ListDataFetcher.h"
#include "BookStore/DataUpdateArgs.h"
#include "Utility/JsonObject.h"

namespace dk
{
namespace bookstore
{


const char* ListDataFetcher::EventListDataUpdate = "EventListUpdate";

ListDataFetcher::ListDataFetcher(
        const std::tr1::function<DownloadTaskSPtr (int, int, FetchDataOrder)>& downloadTaskBuilder,
        model::ObjectType objectType,
        const char* itemsKeyName,
        const char* resultKeyName,
        const char* totalKeyName)
    : m_downloadTaskBuilder(downloadTaskBuilder)
    , m_objectType(objectType)
    , m_itemsKeyName(itemsKeyName)
    , m_order(FDO_DEFAULT) 
    , m_resultKeyName(resultKeyName)
    , m_totalKeyName(totalKeyName)
{
    pthread_mutex_init(&m_lock, 0);
    m_fetchRemoteDataStartIndex = -1;
}

ListDataFetcher::~ListDataFetcher()
{
    pthread_mutex_destroy(&m_lock);
}

FetchDataResult ListDataFetcher::FetchData(int start, int length)
{
    if (start < 0 || length <= 0)
    {
        return FDR_FAILED;
    }
    AutoLock lock(&m_lock);
    return FetchRemoteData(start, length);
}



FetchDataResult ListDataFetcher::FetchRemoteData(int start, int length)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s START", __FILE__,  __LINE__, "ListDataFetcher", __FUNCTION__);
    if (IsBusy())
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s return FDR_PENDING", __FILE__,  __LINE__, "ListDataFetcher", __FUNCTION__);
        return FDR_PENDING;
    }
    DownloadTaskSPtr downloadTask = BuildDownloadTask(start, length);
    EventListener::SubscribeEvent(DownloadTask::EventDownloadTaskFinished, 
        *downloadTask, 
        std::tr1::bind(std::tr1::mem_fn(&ListDataFetcher::OnDownloadFinished),
        this, 
        std::tr1::placeholders::_1));
    DownloadManager::GetInstance()->AddTask(downloadTask);
    m_currentTask = downloadTask;
    m_fetchRemoteDataStartIndex = start;
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s END", __FILE__,  __LINE__, "ListDataFetcher", __FUNCTION__);
    return FDR_PENDING;
}

bool ListDataFetcher::OnDownloadFinished(const EventArgs& args)
{
    AutoLock lock(&m_lock);

    const DownloadTaskFinishArgs& taskArgs = (const DownloadTaskFinishArgs&)args;
    // 有可能DownloadTask被重设
    if (taskArgs.downloadTask != m_currentTask.get())
    {
        return false;
    }
    DataUpdateArgs listDataUpdateArgs;
    listDataUpdateArgs.startIndex = m_fetchRemoteDataStartIndex;
    if (taskArgs.succeeded)
    {
        std::string fetchedData = m_currentTask->GetString();
        if (ParseData(fetchedData.c_str(), &listDataUpdateArgs.dataList, &listDataUpdateArgs.moreData, &listDataUpdateArgs.total, &listDataUpdateArgs.errorCode))
        {
            listDataUpdateArgs.succeeded = true;
        }
    }
    FireEvent(EventListDataUpdate, listDataUpdateArgs);
    m_currentTask.reset();
    return true;
}

bool ListDataFetcher::ParseData(
        const char* data,
        std::vector<model::BasicObjectSPtr>* dataList,
        bool* moreData,
        int* total,
        int* result)
{
    if (NULL == data)
    {
        return false;
    }
    JsonObjectSPtr rootObj = JsonObject::CreateFromString(data);
    if (!rootObj)
    {
        return false;
    }
    
    rootObj->GetIntValue(GetResultKeyName(), result);
    if ((*result) != 0)
    {
        return false;
    }
    rootObj->GetBooleanValue("more", moreData);
    rootObj->GetIntValue(m_totalKeyName.c_str(), total);

    int count = 0;
    JsonObjectSPtr items = rootObj->GetSubObject(GetItemsKeyName());
    if (!rootObj->GetIntValue("count", &count) && items != 0)
    {
        // if count is NULL, get the length of array
        count = items->GetArrayLength();
        if (total <= 0)
            *total = count;
    }
    
    if (items)
    {
        for (int i = 0; i < count; ++i)
        {
            JsonObjectSPtr item = items->GetElementByIndex(i);
            if (!item)
            {
                continue;
            }
            model::BasicObject* basicObject = model::BasicObject::CreateBasicObject(item.get(), m_objectType);
            if (basicObject)
            {
                dataList->push_back(model::BasicObjectSPtr(basicObject));
            }
        }
    }
    PostParseData(rootObj.get());
    return true;
}

DownloadTaskSPtr ListDataFetcher::BuildDownloadTask(int start, int length)
{
    return m_downloadTaskBuilder(start, length, m_order);
}

} // namespace bookstore
} // namespace dk
