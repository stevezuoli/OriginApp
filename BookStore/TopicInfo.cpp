#include "BookStore/TopicInfo.h"
#include "Utility/JsonObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{

bool TopicInfo::Init(const JsonObject* jsonObj)
{
    jsonObj->GetIntValue("list_id", &m_listId);
    // 这里label对应title主要是为了兼容CommentInfo接口
    jsonObj->GetStringValue("label", &m_label);
    jsonObj->GetStringValue("description", &m_description);
    jsonObj->GetStringValue("cover", &m_coverUrl);
    jsonObj->GetIntValue("weight", &m_weight);
    jsonObj->GetStringValue("updated", &m_updateTime);
    jsonObj->GetIntValue("book_count", &m_bookCount);
    jsonObj->GetStringValue("banner", &m_banner);
    return true;
}

TopicInfo* TopicInfo::CreateTopicInfo(const JsonObject* jsonObj)
{
    TopicInfo* listInfo = new TopicInfo();
    if (NULL != listInfo && listInfo->Init(jsonObj))
    {
        return listInfo;
    }
    if (NULL != listInfo)
    {
        delete listInfo;
    }
    return NULL;
}

TopicInfoList TopicInfo::FromBasicObjectList(const BasicObjectList& basicObjectList)
{
    TopicInfoList result;
    for (size_t i = 0; i < basicObjectList.size(); ++i)
    {
        if (basicObjectList[i] && basicObjectList[i]->GetObjectType() == OT_TOPIC)
        {
            result.push_back(TopicInfoSPtr((TopicInfo*)(basicObjectList[i]->Clone())));
        }
    }
    return result;
}
} // namespace model
} // namespace bookstore
} // namespace dk
