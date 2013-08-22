#include "BookStore/CategoryInfo.h"
#include "Utility/JsonObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{

CategoryInfo::CategoryInfo()
    : m_bookCount(0)
    , m_pParentNode(NULL)
{
}

CategoryInfo* CategoryInfo::CreateCategoryInfo(const JsonObject* jsonObject)
{
    CategoryInfo* categoryInfo = new CategoryInfo();
    if (NULL != categoryInfo && categoryInfo->Init(jsonObject))
    {
        return categoryInfo;
    }
    delete categoryInfo;
    return NULL;
}

bool CategoryInfo::Init(const JsonObject* jsonObject)
{
    if (!jsonObject)
    {
        return false;
    }

    jsonObject->GetStringValue("category_id", &m_categoryId);
    jsonObject->GetStringValue("label", &m_label);
    jsonObject->GetStringValue("description", &m_description);
    jsonObject->GetStringValue("titles", &m_titles);
    jsonObject->GetStringValue("cover", &m_coverUrl);
    jsonObject->GetIntValue("book_count", &m_bookCount);

    int count = 0;
    if (jsonObject->GetIntValue("count", &count))
    {
        JsonObjectSPtr items = jsonObject->GetSubObject("items");
        if (items)
        {
            for (int i = 0; i < count; ++i)
            {
                JsonObjectSPtr item = items->GetElementByIndex(i);
                if (!item)
                {
                    continue;
                }
                model::BasicObject* basicObject = model::BasicObject::CreateBasicObject(item.get(), OT_CATEGORY);
                if (basicObject)
                {
                    ((CategoryInfo*)(basicObject))->m_pParentNode = this;
                    m_childrenList.push_back(model::BasicObjectSPtr(basicObject));
                }
            }
        }
    }

    return true;
}

CategoryInfoList CategoryInfo::FromBasicObjectList(const BasicObjectList& basicObjectList)
{
    CategoryInfoList result;
    for (size_t i = 0; i < basicObjectList.size(); ++i)
    {
        if (basicObjectList[i] && basicObjectList[i]->GetObjectType() == OT_CATEGORY)
        {
            result.push_back(CategoryInfoSPtr((CategoryInfo*)(basicObjectList[i]->Clone())));
        }
    }
    return result;
}

bool CategoryInfo::GetChildrenNodeList(std::string categoryId, BasicObjectList& childrenList) const
{
    if (GetCategoryId() == categoryId)
    {
        childrenList = m_childrenList;
        return true;
    }

    for (size_t i = 0; i < m_childrenList.size(); ++i)
    {
        CategoryInfo* categoryInfo = (CategoryInfo*)(m_childrenList[i].get());
        if (categoryInfo && categoryInfo->GetChildrenNodeList(categoryId, childrenList))
        {
            return true;
        }
    }

    return false;
}

} // namespace model
} // namespace bookstore
} // namespace dk
