#include "BookStore/CommentReply.h"
#include <string.h>
#include "Utility/JsonObject.h"


namespace dk
{
namespace bookstore
{
namespace model
{

CommentReply::CommentReply()
{
    
}


CommentReply* CommentReply::CreateCommentReply(const JsonObject* jsonObject)
{
    CommentReply* commentReply = new CommentReply();
    if (NULL != commentReply && commentReply->Init(jsonObject))
    {
        return commentReply;
    }
    else
    {
        delete commentReply;
        return NULL;
    }
}

bool CommentReply::Init(const JsonObject* jsonObject)
{
    jsonObject->GetStringValue("publish_time", &m_publishTime);
    jsonObject->GetStringValue("nick_name", &m_commenterNick);
    jsonObject->GetStringValue("content", &m_content);
    jsonObject->GetStringValue("device_type", &m_deviceType);
    jsonObject->GetStringValue("reply_id", &m_replyId);
    return true;
}


} // namespace model
} // namespace bookstore
} // namespace dk
