#include <tr1/functional>
#include "PersonalUI/PushedMessagesManager.h"
#include "BookStore/BookStoreInfoManager.h"
#include "interface.h"
#include "Utility/TimerThread.h"
#include "Framework/CNativeThread.h"
#include "Utility/ThreadHelper.h"
#include "Utility/SystemUtil.h"
#include "Utility/PathManager.h"
#include "BookStore/ConstStrings.h"
#include "Common/CAccountManager.h"
#include "Utility/StringUtil.h" 

using namespace std;
using namespace dk::common;
using namespace dk::bookstore;
using namespace dk::utility;
using namespace dk::account;
using dk::utility::StringUtil;

enum {
    BOXMESSAGES_BOOKONLY = 0,
    BOXMESSAGES_BOOK_AND_SERIAL = 1
};

PushedMessagesManager PushedMessagesManager::s_instance;
std::string PushedMessagesManager::m_boxMessageType;

const char* PushedMessagesManager::EventPushedMessageMgrUpdate = "EventPushedMessageMgrUpdate";
const char* PushedMessagesManager::EventMessageUnreadCountUpdate = "EventMessageUnreadCountUpdate";

PushedMessagesManager::PushedMessagesManager(void)
    : m_isRuning(false)
    , m_jsonPageTables(JsonObjectSPtr())
    , m_boxMessageUnreadCount(0)
    , m_postUnreadCountResult(-1)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventBoxMessageUnreadCountUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&PushedMessagesManager::OnBoxMessageUnreadCountUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    SubscribeMessageEvent(BookStoreInfoManager::EventPostBoxMessageUnreadCountUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&PushedMessagesManager::OnPostBoxMessageUnreadCountUpdate),
        this,
        std::tr1::placeholders::_1)
        );
}

PushedMessagesManager::~PushedMessagesManager(void)
{
    TimerThread::GetInstance()->CancelTimer(PushedMessagesManager::FetchBoxMessages, NULL);
    Clear();
}

void PushedMessagesManager::StartGetMessages()
{
    char type[8] = {0};
    snprintf(type, DK_DIM(type), "%d,%d,%d", model::BOXMESSAGES_COMMENT_REPLY, model::BOXMESSAGES_COMMENT_REREPLY, model::BOXMESSAGES_SYSTEM);
    SetBoxMessagesType(type);
    PushedMessagesManager::FetchBoxMessages(NULL);
}

void PushedMessagesManager::FetchBoxMessages(void*)
{
    BookStoreInfoManager* bookstoreManager = BookStoreInfoManager::GetInstance();
    if (bookstoreManager)
    {
        bookstoreManager->FetchBoxMessageUnreadCount(m_boxMessageType.c_str(), BOXMESSAGES_BOOK_AND_SERIAL);
    }
}

void PushedMessagesManager::StopGetMessages(bool clearData)
{
    //DebugLog(DLC_DIAGNOSTIC, "PushedMessagesManager::StopGetMessages");
    TimerThread::GetInstance()->CancelTimer(PushedMessagesManager::FetchBoxMessages, NULL);
    if (clearData)
    {
        Clear();
        DataUpdateArgs msgArgs;
        msgArgs.succeeded = true;
        FireEvent(EventMessageUnreadCountUpdate, msgArgs);
    }
}

void PushedMessagesManager::Clear()
{
    m_currentUserID.clear();
    m_msgList.clear();
    m_boxMessageUnreadCount = 0;
}

PushedMessagesManager* PushedMessagesManager::GetInstance()
{
    return &s_instance;
}


model::PushedMessageInfoList PushedMessagesManager::GetPushedMessageList()
{
    return m_msgList;
}

void PushedMessagesManager::DeleteReadedMessage(const std::string& messageID)
{
    bool changed = false;
    for (size_t j = 0; j < m_msgList.size();)
    {
        LockScope scope(m_lock);
        const model::PushedMessageInfoSPtr msg = m_msgList[j];
        if (msg && (messageID == msg->GetMessageId()))
        {
            m_msgList.erase(m_msgList.begin() + j);
            changed = true;
        }
        else
        {
            ++j;
        }
    }
    if (changed)
    {
        DataUpdateArgs msgArgs;
        msgArgs.succeeded = true;
        FireEvent(EventMessageUnreadCountUpdate, msgArgs);
    }
}

bool PushedMessagesManager::OnMessagesUpdate(const EventArgs& args)
{
    DataUpdateArgs msgArgs = (const DataUpdateArgs&)args;
    model::PushedMessageInfoList list = model::PushedMessageInfo::FromBasicObjectList(msgArgs.dataList);
    for (std::vector<model::PushedMessageInfoSPtr>::iterator itr = list.begin(); itr != list.end(); itr++)
    {
        if ((*itr) && ((*itr)->GetObjectType() == model::OT_BOXMESSAGES))
        {
            m_msgList.push_back(*itr);
        }
    }
    FireEvent(EventPushedMessageMgrUpdate, args);
    return msgArgs.succeeded;
}

bool PushedMessagesManager::ResetCurrentUserID()
{
    CAccountManager* accountManager = CAccountManager::GetInstance();
    if (!accountManager->IsLoggedIn())
    {
        return false;
    }
    string userName = StringUtil::ToLower(accountManager->GetEmailFromFile().c_str());
    if (m_currentUserID.compare(userName.c_str()) != 0)
    {
        StopGetMessages();
        m_currentUserID = userName;
    }
    return true;
}

bool PushedMessagesManager::compareMessageTime(model::PushedMessageInfoSPtr messageInfo1, model::PushedMessageInfoSPtr messageInfo2)
{
    if (messageInfo1 && messageInfo2)
    {
        string publishTime1 = messageInfo1->GetStartTime();
        string publishTime2 = messageInfo2->GetStartTime();
        return publishTime1.compare(publishTime2) >= 0;
    }
    return (NULL != messageInfo1);
}

//BoxMessages start
int PushedMessagesManager::GetBoxMessagesUnreadCount() const
{
    return m_boxMessageUnreadCount;
}

bool PushedMessagesManager::OnBoxMessageUnreadCountUpdate(const EventArgs& args)
{
    //DebugLog(DLC_DIAGNOSTIC, "PushedMessagesManager::OnBoxMessageUnreadCountUpdate");
    BoxMessagesUnreadCountUpdateArgs msgArgs = (const BoxMessagesUnreadCountUpdateArgs&)args;
    const int intervalBox = msgArgs.interval * 1000;
    TimerThread::GetInstance()->AddTimer(PushedMessagesManager::FetchBoxMessages, NULL, SystemUtil::GetUpdateTimeInMs() + intervalBox, false, false, 0);
    if (msgArgs.succeeded)
    {
        m_boxMessageUnreadCount = msgArgs.count;
        m_deletionThreshold = msgArgs.deletionThreshold;
    }

    DataUpdateArgs unreadCountArgs;
    unreadCountArgs.succeeded = msgArgs.succeeded;
    unreadCountArgs.total = msgArgs.count;
    FireEvent(EventMessageUnreadCountUpdate, unreadCountArgs);
    return true;
}

FetchDataResult PushedMessagesManager::FetchBoxMessageLst(int startIndex, int length)
{
    BookStoreInfoManager* pBookStoreMgr = BookStoreInfoManager::GetInstance();
    if (pBookStoreMgr)
    {
        if (length < FETCH_NUMBERS_ONCE)
            length = FETCH_NUMBERS_ONCE;
        return pBookStoreMgr->FetchBoxMessageList(m_boxMessageType.c_str(), startIndex, length);
    }
    return FDR_PENDING;
}

bool PushedMessagesManager::OnBoxMessagesUpdate(const EventArgs& args)
{
    //DebugLog(DLC_DIAGNOSTIC, "PushedMessagesManager::OnBoxMessagesUpdate");
    DataUpdateArgs msgArgs = (const DataUpdateArgs&)args;
    if (msgArgs.succeeded)
    {
        m_boxMessageUnreadCount = 0;
        UpdateBoxMessageUnreadCount();
        //return OnMessagesUpdate(args);
        return true;
    }
    return false;
}

FetchDataResult PushedMessagesManager::UpdateBoxMessageUnreadCount()
{
    //DebugLog(DLC_DIAGNOSTIC, "PushedMessagesManager::UpdateBoxMessageUnreadCount");
    BookStoreInfoManager* pBookStoreMgr = BookStoreInfoManager::GetInstance();
    if (pBookStoreMgr && !m_deletionThreshold.empty())
    {
        return pBookStoreMgr->UpdateBoxMessageUnreadCount(m_boxMessageType.c_str(), m_deletionThreshold.c_str());
    }
    return FDR_PENDING;
}

bool PushedMessagesManager::OnPostBoxMessageUnreadCountUpdate(const EventArgs& args)
{
    const PostBoxMessagesUnreadCountUpdateArgs& msgArgs = (const PostBoxMessagesUnreadCountUpdateArgs&)args;
    m_postUnreadCountResult = msgArgs.result;
    //DebugPrintf(DLC_DIAGNOSTIC, "OnPostBoxMessageUnreadCountUpdate succeeded = %d, result = %d", msgArgs.succeeded, m_postUnreadCountResult);
    if (msgArgs.succeeded)
    {
    }
    return true;
}

void PushedMessagesManager::SetBoxMessagesType(const std::string& type)
{
    m_boxMessageType = type;
}
//BoxMessages end

