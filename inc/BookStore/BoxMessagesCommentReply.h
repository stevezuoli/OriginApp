#ifndef __BOOKSTORE_BOXMESSAGESCOMMENTREPLY_H__
#define __BOOKSTORE_BOXMESSAGESCOMMENTREPLY_H__

#include "BookStore/BasicObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{

class BoxMessagesCommentReply;
typedef std::tr1::shared_ptr<BoxMessagesCommentReply> BoxMessagesCommentReplySPtr;
typedef std::vector<BoxMessagesCommentReplySPtr> BoxMessagesCommentReplyList;

class BoxMessagesCommentReply: public BasicObject
{
private:
    BoxMessagesCommentReply();
public:
    static BoxMessagesCommentReply* CreateBoxMessagesCommentReply(const JsonObject* jsonObject, int messageType, bool fromLocal);
    virtual ObjectType GetObjectType() const
    {
        return OT_BOXMESSAGES_COMMENT_REPLY;
    }
    virtual BasicObject* Clone() const
    {
        return new BoxMessagesCommentReply(*this);
    }

    virtual ~BoxMessagesCommentReply(){};

    std::string GetBookId() const { return m_bookId; }
    std::string GetBookName() const { return m_bookName; }
    std::string GetCommentId() const { return m_commentId; }
    std::string GetComment() const { return m_comment; }
    int GetUserId() const { return m_userID; }
    std::string GetReplyId() const { return m_replyID; }
    std::string GetReply() const { return m_reply; }
    int GetReplyUserID() const { return m_replyUserID; }
    std::string GetReplyAlias() const { return m_replyAlias; }
    std::string GetReplyTime() const { return m_replyTime; }

    std::string GetRereplyId() const { return m_rereplyID; }
    std::string GetRereply() const { return m_rereply; }
    int GetRereplyUserID() const { return m_rereplyUserID; }
    std::string GetRereplyAlias() const { return m_rereplyAlias; }
    std::string GetRereplyTime() const { return m_rereplyTime; }

private:
    bool Init(const JsonObject* jsonObj, int messageType, bool fromLocal);
    std::string m_bookId;
    std::string m_bookName;
    std::string m_commentId;
    std::string m_comment;

    std::string m_replyID; // only used in messageType = 5
    std::string m_reply; // only used in messageType = 5
    std::string m_replyAlias;
    std::string m_replyTime;

    std::string m_rereplyID; // only used in messageType = 10
    std::string m_rereply; // only used in messageType = 10
    std::string m_rereplyAlias; // only used in messageType = 10
    std::string m_rereplyTime; // only used in messageType = 10
    int m_rereplyUserID; // only used in messageType = 10

    int m_replyUserID;
    int m_userID;
};

} // namespace model
} // namespace bookstore
} // namespace dk
#endif
