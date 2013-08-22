#ifndef __BOOKSTORE_CATEGORYINFO_H__
#define __BOOKSTORE_CATEGORYINFO_H__

#include "BookStore/BasicObject.h"
#include <time.h>

namespace dk
{
namespace bookstore
{
namespace model
{

class CategoryInfo;
typedef std::tr1::shared_ptr<CategoryInfo> CategoryInfoSPtr;
typedef std::vector<CategoryInfoSPtr> CategoryInfoList;

class CategoryInfo: public BasicObject
{
private:
    CategoryInfo();

public:
    virtual ObjectType GetObjectType() const
    {
        return OT_CATEGORY;
    }

    static CategoryInfo* CreateCategoryInfo(const JsonObject* jsonObject);
    static CategoryInfoList FromBasicObjectList(const BasicObjectList& basicObjectList);

    virtual BasicObject* Clone() const
    {
        return new CategoryInfo(*this);
    }

    virtual ~CategoryInfo(){};

    std::string GetCategoryId() const
    {
        return m_categoryId;
    }

    std::string GetLabel() const
    {
        return m_label;
    }

    // price in cent
    std::string GetDescription() const 
    {
        return m_description; 
    }

    std::string GetTitles() const
    {
        return m_titles;
    }

    std::string GetCoverUrl() const
    {
        return m_coverUrl;
    }

    int GetBookCount() const
    {
        return m_bookCount;
    }

    bool GetChildrenNodeList(std::string categoryId, BasicObjectList& childrenList) const;
    BasicObjectList GetChildrenNodeList() const
    {
        return m_childrenList;
    }
    
    CategoryInfo* GetParentNode() const
    {
        return m_pParentNode;
    }
private:
    bool Init(const JsonObject* jsonObj);
    std::string m_categoryId;
    std::string m_label;
    std::string m_description;
    std::string m_titles;
    std::string m_coverUrl;
    int         m_bookCount;
    CategoryInfo* m_pParentNode;
    BasicObjectList m_childrenList;
}; // class BookInfo


} // namespace model
} // namespace bookstore
} // namespace dk
#endif

