#include "BookStore/BasicObject.h"
#include "BookStore/BookInfo.h"
#include "BookStore/TopicInfo.h"
#include "BookStore/CategoryInfo.h"
#include "BookStore/AdInfo.h"
#include "BookStore/BookComment.h"
#include "BookStore/BookChangeLog.h"
#include "BookStore/PushedMessagesInfo.h"
#include "BookStore/SpecialEvent.h"
#include "BookStore/XiaomiUserCard.h"
#include "Utility/JsonObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{

static ObjectType GetJsonObjectType(const JsonObject* jsonObject)
{
    std::string vs;
    int vi;
    if (jsonObject->GetStringValue("book_id", &vs))
    {
        if (jsonObject->GetSubObject("comment_id"))
        {
            if (jsonObject->GetSubObject("reply"))
            {
                return OT_COMMENT_REPLY;
            }
            return OT_COMMENT;
        }
        return OT_BOOK;
    }
    else if (jsonObject->GetIntValue("list_id", &vi))
    {
        return OT_TOPIC;
    }
    else if (jsonObject->GetStringValue("category_id", &vs))
    {
        return OT_CATEGORY;
    }
    else if (jsonObject->GetSubObject("ad_type"))
    {
        return OT_AD;
    }
    else if (jsonObject->GetSubObject("log"))
    {
        return OT_CHANGELOG;
    }
    else if (jsonObject->GetSubObject("message"))
    {
        return OT_PUSHEDMESSAGES;
    }
    else if (jsonObject->GetSubObject("messages"))
    {
        return OT_BOXMESSAGES;
    }
    else
    {
        return OT_UNKNOWN;
    }
}

BasicObject* BasicObject::CreateBasicObject(const JsonObject* jsonObject, ObjectType type)
{
    if (type == OT_UNKNOWN)
    {
        type = GetJsonObjectType(jsonObject);
    }
    switch (type)
    {
        case OT_BOOK_IN_CART:
            return BookInfo::CreateBookInfo(jsonObject, OT_BOOK_IN_CART);
            break;
        case OT_BOOK:
            return BookInfo::CreateBookInfo(jsonObject, OT_BOOK);
            break;
        case OT_TOPIC:
            return TopicInfo::CreateTopicInfo(jsonObject);
        case OT_CATEGORY:
            return CategoryInfo::CreateCategoryInfo(jsonObject);
        case OT_AD:
        case OT_PUBLISH:
            return AdInfo::CreateAdInfo(jsonObject, type);
        case OT_COMMENT:
        case OT_MYCOMMENTS:
            return BookComment::CreateBookComment(jsonObject, type);
        case OT_COMMENT_REPLY:
            return CommentReply::CreateCommentReply(jsonObject);
        case OT_CHANGELOG:
            return BookChangeLog::CreateBookChangeLog(jsonObject);
        case OT_PUSHEDMESSAGES:
        case OT_BOXMESSAGES:
            return PushedMessageInfo::CreatePushedMessageInfo(jsonObject, type);
        case OT_SPECIAL_EVENTS:
            return SpecialEvent::CreateSpecialEvent(jsonObject);
        case OT_XIAOMI_USER_CARD:
            return XiaomiUserCard::CreateXiaomiUserCard(jsonObject);
        default:
            return NULL;
            break;
    }
};

} // namespace model
} // namespace bookstore
} // namespace dk
