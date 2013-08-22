#ifndef __BOOKSTORE_CATALOGUEINFO_H__
#define __BOOKSTORE_CATALOGUEINFO_H__

#include "BookStore/BasicObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{
class CatalogueInfo;
typedef std::tr1::shared_ptr<CatalogueInfo> CatalogueInfoSPtr;
typedef std::vector<CatalogueInfoSPtr> CatalogueInfoList;

class CatalogueInfo: public BasicObject
{
public:
    CatalogueInfo()
    {
    }
    virtual ~CatalogueInfo()
    {
    }
    virtual BasicObject* Clone() const
    {
        return new CatalogueInfo(*this);
    }

    static CatalogueInfo* CreateCatalogueInfo(const char* catalogue, int level);

    virtual ObjectType GetObjectType() const
    {
        return OT_CATALOGUE;
    }
    enum{
        FIRST_LEVEL,
        SECOND_LEVEL,
        THIRD_LEVEL,
    };

    std::string GetCatalogue() const
    {
        return m_catalogue;
    }
    std::string GetPreview() const
    {
        return m_preview;
    }
    void SetPreview(const std::string& preview)
    {
        m_preview = preview;
    }

    int GetCatalogueLevel()
    {
        return m_cataLevel;
    }
private:
    bool Init(const char* catalogue, int level);
    std::string m_catalogue;
    std::string m_preview;
    int m_cataLevel;
    
}; // class CatalogueInfo
} // namespace model
} // namespace bookstore
} // namespace dk

#endif // __BOOKSTORE_CATALOGUEINFO_H__
