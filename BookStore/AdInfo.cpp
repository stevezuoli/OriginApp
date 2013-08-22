#include "BookStore/AdInfo.h"
#include "Utility/JsonObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{
bool AdInfo::Init(const JsonObject* jsonObject, ObjectType objectType)
{
    m_objectType = objectType;
    jsonObject->GetStringValue("ad_name", &m_name);
    jsonObject->GetStringValue("ad_pic_url", &m_pictureUrl);
    int type = 0;
    jsonObject->GetIntValueFromIntOrString("ad_type", &type);
    switch (type)
    {
        case 1:
            m_referenceType = OT_BOOK;
            break;
        case 2:
            m_referenceType = OT_TOPIC;
            break;
        case 4:
            m_referenceType = OT_PUBLISH;
            break;
        default:
            m_referenceType = OT_UNKNOWN;
            break;
    }
    jsonObject->GetStringValue("reference_id", &m_referenceId);
    return true;
}

AdInfo* AdInfo::CreateAdInfo(const JsonObject* jsonObject, ObjectType objectType)
{
    AdInfo* adInfo = new AdInfo();
    if (NULL != adInfo && adInfo->Init(jsonObject, objectType))
    {
        return adInfo;
    }
    if (adInfo)
    {
        delete adInfo;
    }
    return NULL;

}


} // namespace model
} // namespace bookstore
} // namespace dk
