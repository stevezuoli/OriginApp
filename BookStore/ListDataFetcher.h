#ifndef __BOOKSTORE_LISTDATAFETCHER_H__ 
#define __BOOKSTORE_LISTDATAFETCHER_H__ 

#include "DownloadManager/DownloadTask.h" 
#include "DownloadManager/DownloadManager.h" 
#include <cassert> 
#include "GUI/EventListener.h" 
#include "GUI/EventSet.h" 
#include "interface.h" 
#include <tr1/functional> 
#include "BookStore/BasicObject.h" 
#include "BookStore/BookStoreTypes.h" 

class JsonObject;
namespace dk
{
namespace bookstore
{

class ListDataFetcher
    : public EventListener
    , public EventSet
{
public:
    typedef std::vector<model::BasicObjectSPtr> DataList;
    static const char* EventListDataUpdate;

    ListDataFetcher(
            const std::tr1::function<DownloadTaskSPtr (int, int, FetchDataOrder)>& downloadTaskBuilder 
                = std::tr1::function<DownloadTaskSPtr(int, int, FetchDataOrder)>(),
            model::ObjectType objectType = model::OT_UNKNOWN,
            const char* itemsKeyName = "items",
            const char* resultKeyName = "result",
            const char* totalKeyName = "total");
    ~ListDataFetcher();

    FetchDataResult FetchData(int start, int length);

    // for list like category, no more data field,
    // so should set to false manualy

    virtual bool ParseData(const char* data,
            std::vector<model::BasicObjectSPtr>* dataList,
            bool* moreData,
            int* total,
            int* error);

    virtual void SetObjectType(model::ObjectType objectType)
    {
        m_objectType = objectType;
    }

    void SetFetchOrder(FetchDataOrder order)
    {
        AutoLock lock(&m_lock);
        if (order != m_order)
        {
            m_order = order;
            Reset();
        }
    }

protected:
    virtual bool PostParseData(JsonObject* /* jsonObject */)
    {
        return true;
    }

    void Reset()
    {
        m_fetchRemoteDataStartIndex = -1;
        m_currentTask = DownloadTaskSPtr();
    }

    const char* GetItemsKeyName() const
    {
        return m_itemsKeyName.c_str();
    }

    const char* GetResultKeyName() const
    {
        return m_resultKeyName.c_str();
    }

    virtual DownloadTaskSPtr BuildDownloadTask(int start, int length);

    bool IsBusy() const
    {
        return m_currentTask.get() != 0;
    }

    FetchDataResult FetchRemoteData(int start, int length);

    bool OnDownloadFinished(const EventArgs& args);

    pthread_mutex_t m_lock;
    // 获取远程数据列表的起始位置，便于在m_dataList中插入数据
    int m_fetchRemoteDataStartIndex;
    DownloadTaskSPtr m_currentTask;
    std::tr1::function<DownloadTaskSPtr (int, int, FetchDataOrder)> m_downloadTaskBuilder;
    model::ObjectType m_objectType;
    std::string m_itemsKeyName;
    FetchDataOrder m_order;
    std::string m_resultKeyName;
    std::string m_totalKeyName;
};

} // namespace bookstore
} // namespace dk


#endif

