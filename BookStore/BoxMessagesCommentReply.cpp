#include "BookStore/BoxMessagesCommentReply.h"
#include <string.h>
#include "Utility/JsonObject.h"
#include "BookStore/ConstStrings.h"
#include "BookStore/PushedMessagesInfo.h"
#include "CommonUI/UIUtility.h"

namespace dk
{
namespace bookstore
{
namespace model
{
static const char* BOOK_NAME = "book_name";
static const char* COMMENT = "comment";
static const char* REPLY_ID = "reply_id";
static const char* REPLY = "reply";
static const char* REPLY_USER_ID = "reply_user_id";
static const char* REPLY_ALIAS = "reply_alias";
static const char* REPLY_TIME = "reply_time";

static const char* RE_REPLY_ID = "re_reply_id";
static const char* RE_REPLY = "re_reply";
static const char* RE_REPLY_USER_ID = "re_reply_user_id";
static const char* RE_REPLY_ALIAS = "re_reply_alias";
static const char* RE_REPLY_TIME = "re_reply_time";


BoxMessagesCommentReply::BoxMessagesCommentReply()
    : m_replyUserID(0)
{
}

BoxMessagesCommentReply* BoxMessagesCommentReply::CreateBoxMessagesCommentReply(const JsonObject* jsonObject, int messageType, bool fromLocal)
{
    BoxMessagesCommentReply* commentReply = new BoxMessagesCommentReply();
    if (NULL != commentReply && commentReply->Init(jsonObject, messageType, fromLocal))
    {
        return commentReply;
    }
    else
    {
        delete commentReply;
        return NULL;
    }
}

bool BoxMessagesCommentReply::Init(const JsonObject* jsonObject, int messageType, bool fromLocal)
{
    jsonObject->GetStringValue(ConstStrings::BOOK_ID, &m_bookId);
    jsonObject->GetStringValue(BOOK_NAME, &m_bookName);
    jsonObject->GetStringValue(ConstStrings::COMMENT_ID, &m_commentId);
    jsonObject->GetStringValue(COMMENT, &m_comment);
    jsonObject->GetIntValue(ConstStrings::USER_ID, &m_userID);
    jsonObject->GetIntValue(REPLY_USER_ID, &m_replyUserID);
    jsonObject->GetStringValue(REPLY_ALIAS, &m_replyAlias);
    if (fromLocal)
    {
        jsonObject->GetStringValue(REPLY_TIME, &m_replyTime);
    }
    else
    {
        int replyTime = 0;
        jsonObject->GetIntValue(REPLY_TIME, &replyTime);
        m_replyTime  = UIUtility::TransferIntToString(replyTime);
    }
    if (messageType == model::BOXMESSAGES_COMMENT_REREPLY || messageType == model::BOXMESSAGES_COMMENT_REPLY)
    {
        jsonObject->GetStringValue(REPLY_ID, &m_replyID);
        jsonObject->GetStringValue(REPLY, &m_reply);
        if (messageType == model::BOXMESSAGES_COMMENT_REREPLY)
        {
            jsonObject->GetStringValue(RE_REPLY_ID, &m_rereplyID);
            jsonObject->GetStringValue(RE_REPLY, &m_rereply);
            jsonObject->GetIntValue(RE_REPLY_USER_ID, &m_rereplyUserID);
            jsonObject->GetStringValue(RE_REPLY_ALIAS, &m_rereplyAlias);
            if (fromLocal)
            {
                jsonObject->GetStringValue(RE_REPLY_TIME, &m_rereplyTime);
            }
            else
            {
                int replyTime = 0;
                jsonObject->GetIntValue(RE_REPLY_TIME, &replyTime);
                m_rereplyTime  = UIUtility::TransferIntToString(replyTime);
            }
        }
    }
    return true;
}

} // namespace model
} // namespace bookstore
} // namespace dk
