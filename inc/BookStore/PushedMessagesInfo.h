#ifndef __BOOKSTORE_PUSHEDMESSAGESINFO_H__
#define __BOOKSTORE_PUSHEDMESSAGESINFO_H__

#include "BookStore/BasicObject.h"
#include "BookStore/BoxMessagesCommentReply.h"

namespace dk
{
namespace bookstore
{
namespace model
{
enum {
    BOXMESSAGES_POPUP = 0,
    BOXMESSAGES_BOOK_DETAIL = 1,
    BOXMESSAGES_BOOK_TOPIC = 2,
    BOXMESSAGES_FICTION_DETAIL = 3,
    BOXMESSAGES_FICTION_TOPIC = 4,
    BOXMESSAGES_COMMENT_REPLY = 5,
    BOXMESSAGES_COMMENT_LIKE = 6,
    BOXMESSAGES_COMMENT_REREPLY = 10,
    BOXMESSAGES_SYSTEM = 13,
    //......
};

class PushedMessageInfo;
typedef std::tr1::shared_ptr<PushedMessageInfo> PushedMessageInfoSPtr;
typedef std::vector<PushedMessageInfoSPtr> PushedMessageInfoList;

class PushedMessageInfo: public BasicObject
{
public:
    PushedMessageInfo();
    virtual ObjectType GetObjectType() const
    {
        return m_objectType;
    }

    void SetObjectType(ObjectType type)
    {
        m_objectType = type;
    }

    static PushedMessageInfo* CreatePushedMessageInfo(const JsonObject* jsonObject, ObjectType objectType);
    static PushedMessageInfoList FromBasicObjectList(const BasicObjectList& basicObjectList);

    virtual BasicObject* Clone() const
    {
        return new PushedMessageInfo(*this);
    }

    virtual ~PushedMessageInfo(){};

    void SetMessageId(const std::string& messageID) { m_messageId = messageID; }
    std::string GetMessageId() const   { return m_messageId; }

    void SetTitle(const std::string& title) { m_title = title; }
    std::string GetTitle() const { return m_title; }

    void SetMessage(const std::string& message) { m_message = message; }
    std::string GetMessage() const { return m_message; }

    void SetStartTime(const std::string& startTime) { m_startTime = startTime; }
    std::string GetStartTime() const { return m_startTime; }

    void SetMessageType(int messageType) { m_messageType = messageType; }
    int GetMessageType() const { return m_messageType; }

    // only used in pushedmessages
    void SetLink(const std::string& link) { m_link = link; }
    std::string GetLink() const { return m_link; }

    // only used in boxmessages
    void SetCreateTime(const std::string& createTime) { m_createTime = createTime; }
    std::string GetCreateTime() const { return m_createTime; }
    
    void SetMessageInfo(BasicObjectSPtr messageInfo) {  m_pushMessageInfo = messageInfo; }
    BasicObjectSPtr GetMessageInfo() const { return m_pushMessageInfo; }

    bool Init(const JsonObject* jsonObj, ObjectType objectType, bool fromLocal = false);

    std::string GetStartTimeKey(ObjectType objectType);

private:
    ObjectType m_objectType;
    std::string m_messageId;
    std::string m_message;
    std::string m_title;
    std::string m_startTime;
    int m_messageType;
    // only used in pushedmessages
    std::string m_link;
    // only used in boxmessages
    std::string m_createTime;
    BasicObjectSPtr m_pushMessageInfo;

}; // class PushedMessageInfo


} // namespace model
} // namespace bookstore
} // namespace dk
#endif

