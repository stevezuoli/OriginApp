#ifndef __BOOKSTORE_BASIC_OBJECT_H__
#define __BOOKSTORE_BASIC_OBJECT_H__

#include <string>
#include <tr1/memory>
#include <vector>

class JsonObject;

namespace dk
{

namespace bookstore
{

namespace model
{

enum ObjectType
{
    OT_BOOK,
    OT_BOOK_IN_CART,
    OT_TOPIC,
    OT_CATEGORY,
    OT_CATEGORYTREE,
    OT_CREATE_ORDER_RESULT,
    OT_AD,
    OT_UNKNOWN,
    OT_BOOK_SCORE,
    OT_COMMENT,
    OT_MYCOMMENTS,
    OT_COMMENT_REPLY,
    OT_CHANGELOG,
    OT_CATALOGUE,
    OT_PUBLISH,
    OT_MESSAGES,
    OT_PUSHEDMESSAGES,
    OT_BOXMESSAGES,
    OT_BOXMESSAGES_COMMENT_REPLY,
    OT_BOXMESSAGES_COMMENT_REREPLY,
    OT_BOOKNOTESUMMARY,
    OT_SPECIAL_EVENTS,
    OT_XIAOMI_USER_CARD,
};

class BasicObject
{
public:
    virtual ~BasicObject()
    {
    }
    virtual BasicObject* Clone() const = 0;

    virtual ObjectType GetObjectType() const = 0;
    static BasicObject* CreateBasicObject(const JsonObject* jsonObject, ObjectType type = OT_UNKNOWN);
};

typedef std::tr1::shared_ptr<BasicObject> BasicObjectSPtr;
typedef std::vector<BasicObjectSPtr> BasicObjectList;
typedef bool (*BasicObjectCompare)(BasicObjectSPtr, BasicObjectSPtr);

} // namespace model
} // namespace bookstore
} // namespace dk

#endif
