#include <stdio.h>
#include "BookStore/XiaomiUserCard.h"
#include "Utility/JsonObject.h"
#include "Common/LockObject.h"
#include "Utility/StringUtil.h"
#include "Utility/EncodeUtil.h"
#include "KernelDef.h"

using namespace std;
using namespace dk::common;
using namespace dk::utility;

namespace dk
{
namespace bookstore
{
namespace model
{

XiaomiUserCard::XiaomiUserCard()
{
}

XiaomiUserCard::XiaomiUserCard(const XiaomiUserCard& right)
    : m_userId(right.m_userId)
    , m_aliasNick(right.m_aliasNick)
    , m_miliaoNick(right.m_miliaoNick)
    , m_miliaoIconUrl(right.m_miliaoIconUrl)
{
}

bool XiaomiUserCard::Init(const JsonObject* jsonObj)
{
    int userId = -1;
    jsonObj->GetIntValue("userId", &userId);
    char buf[16];
    memset(buf, 0 , 16);
    sprintf(buf, "%d", userId);
    std::string userIdStr(buf);
    m_userId = userIdStr;
    
    jsonObj->GetStringValue("aliasNick", &m_aliasNick);
    jsonObj->GetStringValue("miliaoNick", &m_miliaoNick);
    jsonObj->GetStringValue("miliaoIcon", &m_miliaoIconUrl);
    return true;
}

XiaomiUserCard* XiaomiUserCard::CreateXiaomiUserCard(const JsonObject* jsonObject)
{
    XiaomiUserCard* userCard = new XiaomiUserCard();
    if (NULL != userCard && userCard->Init(jsonObject))
    {
        return userCard;
    }
    if (userCard != 0)
    {
        delete userCard;
    }
    return 0;
}

XiaomiUserCardMap XiaomiUserCard::FromBasicObjectList(const BasicObjectList& basicObjectList)
{
    XiaomiUserCardMap result;
    for (size_t i = 0; i < basicObjectList.size(); ++i)
    {
        if (basicObjectList[i] &&
			(basicObjectList[i]->GetObjectType() == OT_XIAOMI_USER_CARD))
        {
            XiaomiUserCard* origin = (XiaomiUserCard*)(basicObjectList[i].get());
            XiaomiUserCardSPtr copy((XiaomiUserCard*)(origin->Clone()));
            result[copy->GetUserId()] = copy;
        }
    }
    return result;
}


} // namespace model
} // namespace bookstore
} // namespace dk
