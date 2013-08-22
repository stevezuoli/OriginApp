#include "BookStore/CatalogueInfo.h"
#include "Utility/JsonObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{
CatalogueInfo* CatalogueInfo::CreateCatalogueInfo(const char* catalogue, int level)
{
    CatalogueInfo* catalogueInfo = new CatalogueInfo();
    if (NULL != catalogueInfo && catalogueInfo->Init(catalogue, level))
    {
        return catalogueInfo;
    }
    if (catalogueInfo)
    {
        delete catalogueInfo;
    }
    return NULL;
}

bool CatalogueInfo::Init(const char* catalogue, int level)
{
    if (NULL == catalogue)
    {
        return false;
    }
    m_catalogue = catalogue;
    m_cataLevel = level;
    return true;
}

} // namespace model
} // namespace bookstore
} // namespace dk
