#ifndef __BOOKSTORE_TOPICINFO_H__
#define __BOOKSTORE_TOPICINFO_H__

#include "BookStore/BasicObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{

class TopicInfo;
typedef std::tr1::shared_ptr<TopicInfo> TopicInfoSPtr;
typedef std::vector<TopicInfoSPtr> TopicInfoList;

class TopicInfo: public BasicObject
{
public:
    ObjectType GetObjectType() const
    {
        return OT_TOPIC;
    }

    int GetTopicId() const
    {
        return m_listId;
    }

    std::string GetLabel() const
    {
        return m_label;
    }

    std::string GetDescription() const
    {
        return m_description;
    }

    std::string GetCoverUrl() const
    {
        return m_coverUrl; 
    }

    std::string GetUpdateTime() const
    {
        return m_updateTime;
    }
    static TopicInfo* CreateTopicInfo(const JsonObject* jsonObj);
    static TopicInfoList FromBasicObjectList(const BasicObjectList& basicObjectList);

    virtual BasicObject* Clone() const
    {
        return new TopicInfo(*this);
    }

    int GetWeight() const
    {
        return m_weight;
    }
private:
    bool Init(const JsonObject* jsonObj);
    int m_listId;
    std::string m_label;
    std::string m_description;
    std::string m_updateTime;
    std::string m_coverUrl;
    int m_weight;
};


} // namespace model
} // namespace bookstore
} // namespace dk
#endif

