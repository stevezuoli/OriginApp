#include "BookStore/BookComment.h"
#include <string.h>
#include "Utility/JsonObject.h"


namespace dk
{
namespace bookstore
{
namespace model
{

BookComment::BookComment()
    : m_usefulCount(0)
    , m_uselessCount(0)
    , m_score(0)
    , m_replyCount(0)
    , m_publishUTCTime(0)
    , m_objectType(OT_COMMENT)
{
}

BookComment* BookComment::CreateBookComment(const JsonObject* jsonObject, ObjectType objectType)
{
    BookComment* bookComment = new BookComment();
    if (NULL != bookComment && bookComment->Init(jsonObject, objectType))
    {
        return bookComment;
    }
    else
    {
        delete bookComment;
        return NULL;
    }
}

BookCommentList BookComment::FromBasicObjectList(const BasicObjectList& basicObjectList)
{
    BookCommentList result;
    for (size_t i = 0; i < basicObjectList.size(); ++i)
    {
        if (basicObjectList[i] 
            && ((basicObjectList[i]->GetObjectType() == OT_COMMENT) 
                || (basicObjectList[i]->GetObjectType() == OT_MYCOMMENTS)))
        {
            result.push_back(BookCommentSPtr((BookComment*)(basicObjectList[i]->Clone())));
        }
    }
    return result;
}

bool BookComment::Init(const JsonObject* jsonObject, ObjectType objectType)
{
    if (objectType == OT_COMMENT || objectType == OT_MYCOMMENTS)
    {
        m_objectType = objectType;
    }
    jsonObject->GetStringValue("publish_time", &m_publishTime);
    jsonObject->GetIntValue("useful", &m_usefulCount);
    jsonObject->GetIntValue("useless", &m_uselessCount);
    jsonObject->GetStringValue("title", &m_title);
    jsonObject->GetStringValue("nick_name", &m_commenterNick);
    jsonObject->GetStringValue("content", &m_content);
    jsonObject->GetIntValue("score", &m_score);
    jsonObject->GetStringValue("device_type", &m_deviceType);
    jsonObject->GetIntValue("reply_count", &m_replyCount);
    jsonObject->GetStringValue("comment_id", &m_commentId);
    jsonObject->GetStringValue("book_id", &m_bookId);
    jsonObject->GetStringValue("book_name", &m_bookName);
    jsonObject->GetStringValue("link_cover", &m_bookCoverUrl);
    jsonObject->GetIntValue("publish_utc_time", &m_publishUTCTime);
    JsonObjectSPtr repliesJsonObj = jsonObject->GetSubObject("reply");
    if (repliesJsonObj)
    {
        int replyCount = repliesJsonObj->GetArrayLength();
        for (int i = 0; i < replyCount; ++i)
        {
            JsonObjectSPtr replyJsonObj = repliesJsonObj->GetElementByIndex(i);
            if (replyJsonObj.get() == NULL)
            {
                continue;
            }
            CommentReply* commentReply = 
                CommentReply::CreateCommentReply(
                        replyJsonObj.get());
            if (NULL == commentReply)
            {
                continue;
            }
            m_replyList.push_back(CommentReplySPtr(commentReply));
        }
    }
    return true;
}

} // namespace model
} // namespace bookstore
} // namespace dk
