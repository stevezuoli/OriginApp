#ifndef __XIAOMI_LIST_DATA_FETCHER_H__ 
#define __XIAOMI_LIST_DATA_FETCHER_H__ 

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

class XiaomiListDataFetcher
    : public EventListener
    , public EventSet
{
public:
    typedef std::vector<model::BasicObjectSPtr> DataList;
    static const char* EventXiaomiListUpdate;

public:
    XiaomiListDataFetcher(
            const std::tr1::function<DownloadTaskSPtr(const std::vector<std::string>&)>& downloadTaskBuilder 
                = std::tr1::function<DownloadTaskSPtr(const std::vector<std::string>&)>(),
            model::ObjectType objectType = model::OT_UNKNOWN,
            const char* dataKeyName = "data",
            const char* resultKeyName = "result",
            const char* descriptionKeyName = "description");
    ~XiaomiListDataFetcher();

    FetchDataResult FetchData(const std::vector<std::string>& variants);

    // for list like category, no more data field,
    // so should set to false manualy

    virtual bool ParseData(const char* data,
            std::vector<model::BasicObjectSPtr>* dataList,
            int& total,
            std::string& result,
            std::string& description);

    virtual void SetObjectType(model::ObjectType objectType){
        m_objectType = objectType;
    }

protected:
    virtual bool PostParseData(JsonObject* /* jsonObject */){
        return true;
    }

    void Reset() { m_currentTask = DownloadTaskSPtr(); }
    const char* GetDataKeyName() const { return m_dataKeyName.c_str(); }
    const char* GetResultKeyName() const { return m_resultKeyName.c_str(); }
    const char* GetDescriptionKeyName() const { return m_descriptionKeyName.c_str(); }

    virtual DownloadTaskSPtr BuildDownloadTask(const std::vector<std::string>& variants);
    bool IsBusy() const { return m_currentTask.get() != 0; }
    FetchDataResult FetchRemoteData(const std::vector<std::string>& variants);
    bool OnDownloadFinished(const EventArgs& args);

private:
    pthread_mutex_t m_lock;
    
    DownloadTaskSPtr m_currentTask;
    std::tr1::function<DownloadTaskSPtr(const std::vector<std::string>&)> m_downloadTaskBuilder;

    model::ObjectType m_objectType;
    
    std::string m_dataKeyName;
    std::string m_resultKeyName;
    std::string m_descriptionKeyName;
};

} // namespace bookstore
} // namespace dk


#endif

