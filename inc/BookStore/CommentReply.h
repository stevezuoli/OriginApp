#ifndef __BOOKSTORE_COMMENTREPLY_H__
#define __BOOKSTORE_COMMENTREPLY_H__

#include <tr1/memory>
#include <vector>
#include "BookStore/BasicObject.h"

namespace dk
{
namespace bookstore
{
namespace model
{

class CommentReply;
typedef std::tr1::shared_ptr<CommentReply> CommentReplySPtr;
typedef std::vector<CommentReplySPtr> CommentReplyList;

class CommentReply: public BasicObject
{
private:
    CommentReply();
public:
    static CommentReply* CreateCommentReply(const JsonObject* jsonObject);

    virtual ObjectType GetObjectType() const
    {
        return OT_COMMENT_REPLY;
    }

    virtual BasicObject* Clone() const
    {
        return new CommentReply(*this);
    }

    virtual ~CommentReply(){}
   
    std::string GetReplierNick() const
    {
        return m_commenterNick;
    }

    std::string GetReplyId() const
    {
        return m_replyId;
    }

    std::string GetDeviceType() const
    {
        return m_deviceType;
    }

    std::string GetPublishTime() const
    {
        return m_publishTime;
    }

    std::string GetContent() const
    {
        return m_content;
    }

private:
    bool Init(const JsonObject* jsonObject);
    std::string m_commenterNick;
    std::string m_content;
    std::string m_publishTime;
    std::string m_deviceType;
    std::string m_replyId;
};

} // namespace model
} // namespace bookstore
} // namespace dk
#endif
