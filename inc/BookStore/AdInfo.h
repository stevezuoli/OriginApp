#ifndef __BOOKSTORE_ADINFO_H__
#define __BOOKSTORE_ADINFO_H__

#include "BookStore/BasicObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{
class AdInfo: public BasicObject
{
public:
    AdInfo()
    {
    }
    virtual ~AdInfo()
    {
    }
    virtual BasicObject* Clone() const
    {
        return new AdInfo(*this);
    }

    static AdInfo* CreateAdInfo(const JsonObject* jsonObject, ObjectType objectType);

    virtual ObjectType GetObjectType() const
    {
        return m_objectType;
    }
    std::string GetName() const
    {
        return m_name;
    }
    std::string GetReferenceId() const
    {
        return m_referenceId;
    }

    ObjectType GetReferenceType()
    {
        return m_referenceType;
    }

    std::string GetPictureUrl() const
    {
        return m_pictureUrl;
    }
private:
    bool Init(const JsonObject* jsonObj, ObjectType objectType);
    std::string m_name;
    std::string m_referenceId;
    std::string m_pictureUrl;
    ObjectType m_referenceType;
    ObjectType m_objectType;


}; // class AdInfo
typedef std::tr1::shared_ptr<AdInfo> AdInfoSPtr;
typedef std::vector<AdInfoSPtr> AdInfoList;

} // namespace model
} // namespace bookstore
} // namespace dk

#endif
