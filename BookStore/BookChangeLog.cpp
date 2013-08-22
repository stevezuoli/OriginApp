#include "BookStore/BookChangeLog.h"
#include <string.h>
#include "Utility/JsonObject.h"


namespace dk
{
namespace bookstore
{
namespace model
{

BookChangeLog::BookChangeLog()
{
}

BookChangeLog* BookChangeLog::CreateBookChangeLog(const JsonObject* jsonObject)
{
    BookChangeLog* bookChangeLog = new BookChangeLog();
    if (NULL != bookChangeLog && bookChangeLog->Init(jsonObject))
    {
        return bookChangeLog;
    }
    else
    {
        delete bookChangeLog;
        return NULL;
    }
}

BookChangeLogList BookChangeLog::FromBasicObjectList(const BasicObjectList& basicObjectList)
{
    BookChangeLogList result;
    for (size_t i = 0; i < basicObjectList.size(); ++i)
    {
        if (basicObjectList[i] && basicObjectList[i]->GetObjectType() == OT_CHANGELOG)
        {
            result.push_back(BookChangeLogSPtr((BookChangeLog*)(basicObjectList[i]->Clone())));
        }
    }
    return result;
}

bool BookChangeLog::Init(const JsonObject* jsonObject)
{
    jsonObject->GetStringValue("updated", &m_updated);
    jsonObject->GetStringValue("log", &m_log);
    jsonObject->GetStringValue("revision", &m_revision);
    return true;
}

} // namespace model
} // namespace bookstore
} // namespace dk
