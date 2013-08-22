#ifndef __BOOKSTORE_LISTDATAUPDATEARGS_H__
#define __BOOKSTORE_LISTDATAUPDATEARGS_H__

#include "BookStore/BasicObject.h"
#include "BookStore/BookInfo.h"

namespace dk
{
namespace bookstore
{

class DataUpdateArgs: public EventArgs
{
public:
    DataUpdateArgs()
        : succeeded(false)
        , startIndex(0)
        , moreData(false)
        , total(-1)
        , errorCode(-1)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new DataUpdateArgs(*this);
    }

    bool succeeded;
    // 允许EventHandler对list重新排序，如个人下载列表
    mutable model::BasicObjectList dataList;
    int startIndex;
    bool moreData;
    int total;
    int errorCode;
};

class XiaomiDataUpdateArgs: public EventArgs
{
public:
    XiaomiDataUpdateArgs()
        : succeeded(false)
        , total(0)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new XiaomiDataUpdateArgs(*this);
    }

    bool succeeded;
    mutable model::BasicObjectList dataList;
    int total;
    std::string result;
    std::string description;
};

} // namespace bookstore
} // namespace dk
#endif
