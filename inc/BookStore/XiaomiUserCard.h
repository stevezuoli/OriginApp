#ifndef __XIAOMI_SPECIAL_EVENT_H__
#define __XIAOMI_SPECIAL_EVENT_H__

#include "BookStore/BasicObject.h"
#include <time.h>
#include <vector>
#include <map>

namespace dk
{
namespace bookstore
{
namespace model
{

class XiaomiUserCard;

typedef std::tr1::shared_ptr<XiaomiUserCard> XiaomiUserCardSPtr;
typedef std::vector<XiaomiUserCardSPtr> XiaomiUserCardList;
typedef std::map<std::string, XiaomiUserCardSPtr> XiaomiUserCardMap;

class XiaomiUserCard : public BasicObject
{
private:
    XiaomiUserCard();
    XiaomiUserCard(const XiaomiUserCard& right);

public:
    virtual ObjectType GetObjectType() const
    {
        return OT_XIAOMI_USER_CARD;
    }

    static XiaomiUserCard* CreateXiaomiUserCard(const JsonObject* jsonObject);

    virtual BasicObject* Clone() const
    {
        return new XiaomiUserCard(*this);
    }

    virtual ~XiaomiUserCard(){};

    const std::string& GetUserId() const { return m_userId; }
    const std::string& GetAliasNick() const { return m_aliasNick; }
    const std::string& GetMiliaoNick() const { return m_miliaoNick; }
    const std::string& GetMiliaoIconUrl() const { return m_miliaoIconUrl; }
    static XiaomiUserCardMap FromBasicObjectList(const BasicObjectList& basicObjectList);

private:
    bool Init(const JsonObject* jsonObj);

private:
    std::string m_userId;
    std::string m_aliasNick;
    std::string m_miliaoNick;
    std::string m_miliaoIconUrl;
}; // class XiaomiUserCard


} // namespace model
} // namespace bookstore
} // namespace dk
#endif

