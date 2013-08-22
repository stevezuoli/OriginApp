#ifndef __BOOKSTORE_BOOKCHANGELOG_H__
#define __BOOKSTORE_BOOKCHANGELOG_H__

#include "BookStore/BasicObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{

class BookChangeLog;
typedef std::tr1::shared_ptr<BookChangeLog> BookChangeLogSPtr;
typedef std::vector<BookChangeLogSPtr> BookChangeLogList;

class BookChangeLog: public BasicObject
{
private:
    BookChangeLog();
public:

    static BookChangeLog* CreateBookChangeLog(const JsonObject* jsonObject);
    static BookChangeLogList FromBasicObjectList(const BasicObjectList& basicObjectList);
    virtual ObjectType GetObjectType() const
    {
        return OT_CHANGELOG;
    }
    virtual BasicObject* Clone() const
    {
        return new BookChangeLog(*this);
    }

    virtual ~BookChangeLog(){};

    std::string GetUpdatedTime() const
    {
        return m_updated;
    }

    std::string GetLog() const
    {
        return m_log;
    }

    std::string GetRevision() const
    {
        return m_revision;
    }

private:
    bool Init(const JsonObject* jsonObj);
    std::string m_updated;
    std::string m_log;
    std::string m_revision;
};

} // namespace model
} // namespace bookstore
} // namespace dk
#endif // __BOOKSTORE_BOOKCHANGELOG_H__
