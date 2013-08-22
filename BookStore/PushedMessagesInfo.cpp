#include "BookStore/PushedMessagesInfo.h"
#include "Utility/JsonObject.h"
#include "CommonUI/UIUtility.h"

namespace dk
{
namespace bookstore
{
namespace model
{
static const char* MESSAGE_ID = "message_id";
static const char* TITLE = "title";
static const char* MESSAGE = "message";

static const char* PUBLISH_TIME = "publish_time";
static const char* LINK = "link";

static const char* ACTION_PARAMS = "action_params";
static const char* START_TIME = "start_time";
static const char* CREATE_TIME = "create_time";
static const char* MESSAGE_TYPE = "message_type";

PushedMessageInfo::PushedMessageInfo()
    : m_messageType(-1)
{
}

PushedMessageInfo* PushedMessageInfo::CreatePushedMessageInfo(const JsonObject* jsonObject, ObjectType objectType)
{
    PushedMessageInfo* messageInfo = new PushedMessageInfo();
    if (NULL != messageInfo && messageInfo->Init(jsonObject, objectType))
    {
        return messageInfo;
    }
    delete messageInfo;
    return NULL;
}

bool PushedMessageInfo::Init(const JsonObject* jsonObject, ObjectType objectType, bool fromLocal)
{
    m_objectType = objectType;
    std::string startTimeKey = GetStartTimeKey(objectType);
    jsonObject->GetStringValue(MESSAGE_ID, &m_messageId);
    jsonObject->GetStringValue(TITLE, &m_title);
    jsonObject->GetStringValue(MESSAGE, &m_message);
    if (fromLocal)
    {
        jsonObject->GetStringValue(startTimeKey.c_str(), &m_startTime);
    }
    else
    {
        int startTime = 0;
        jsonObject->GetIntValue(startTimeKey.c_str(), &startTime);
        m_startTime  = UIUtility::TransferIntToString(startTime);
    }
    if (m_objectType == OT_PUSHEDMESSAGES)
    {
        jsonObject->GetStringValue(LINK, &m_link);
        m_messageType = -1;
    }
    else if (m_objectType == OT_BOXMESSAGES)
    {
        jsonObject->GetIntValue(MESSAGE_TYPE, &m_messageType);
        if (fromLocal)
        {
            jsonObject->GetStringValue(CREATE_TIME, &m_createTime);
        }
        else
        {
            int createTime = 0;
            jsonObject->GetIntValue(CREATE_TIME, &createTime);
            m_createTime  = UIUtility::TransferIntToString(createTime);
        }
		switch(m_messageType)
		{
			case BOXMESSAGES_COMMENT_REREPLY:
			case BOXMESSAGES_COMMENT_REPLY:
				{
					JsonObjectSPtr replyObj = jsonObject->GetSubObject(ACTION_PARAMS);
			        if (replyObj)
			        {
			            m_pushMessageInfo = model::BoxMessagesCommentReplySPtr(model::BoxMessagesCommentReply::CreateBoxMessagesCommentReply(replyObj.get(), m_messageType, fromLocal));
			        }
				}
				break;
			default:
				break;
		}
        
    }
    return true;
}

std::string PushedMessageInfo::GetStartTimeKey(ObjectType objectType)
{
    return (objectType == OT_PUSHEDMESSAGES) ? PUBLISH_TIME : START_TIME;
}

PushedMessageInfoList PushedMessageInfo::FromBasicObjectList(const BasicObjectList& basicObjectList)
{
    PushedMessageInfoList result;
    for (size_t i = 0; i < basicObjectList.size(); ++i)
    {
        if (basicObjectList[i] 
        && (basicObjectList[i]->GetObjectType() == OT_PUSHEDMESSAGES
            || basicObjectList[i]->GetObjectType() == OT_BOXMESSAGES))
        {
            result.push_back(PushedMessageInfoSPtr((PushedMessageInfo*)(basicObjectList[i]->Clone())));
        }
    }
    return result;
}

} // namespace model
} // namespace bookstore
} // namespace dk
