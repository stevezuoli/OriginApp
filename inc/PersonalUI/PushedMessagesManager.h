#ifndef __PUSHEDMESSAGEMANAGER_H__
#define __PUSHEDMESSAGEMANAGER_H__

#include <pthread.h>
#include "Common/LockObject.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/PushedMessagesInfo.h"
#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"
#include "GUI/EventListener.h"


class PushedMessagesManager : public EventSet, EventListener
{
    PushedMessagesManager();
public:
    static const char* EventPushedMessageMgrUpdate;
    static const char* EventMessageUnreadCountUpdate;
    static const int FETCH_NUMBERS_ONCE = 4;
    static PushedMessagesManager* GetInstance();
    virtual ~PushedMessagesManager();

    void StartGetMessages();
    void StopGetMessages(bool clearData = true);

    dk::bookstore::model::PushedMessageInfoList GetPushedMessageList();
    void DeleteReadedMessage(const std::string& messageID);
    bool ResetCurrentUserID();
    std::string GetCurrentUserID() const;
    int GetBoxMessagesUnreadCount() const;

    dk::bookstore::FetchDataResult FetchBoxMessageLst(int startIndex, int length);
    dk::bookstore::FetchDataResult UpdateBoxMessageUnreadCount();
    bool OnBoxMessageUnreadCountUpdate(const EventArgs& args);
    bool OnBoxMessagesUpdate(const EventArgs& args);
    bool OnPostBoxMessageUnreadCountUpdate(const EventArgs& args);
    void SetBoxMessagesType(const std::string& type);
private:
    static void FetchBoxMessages(void*);
    static bool compareMessageTime(dk::bookstore::model::PushedMessageInfoSPtr messageInfo1,
                                   dk::bookstore::model::PushedMessageInfoSPtr messageInfo2);
    bool OnMessagesUpdate(const EventArgs& args);
    void Clear();

    static PushedMessagesManager s_instance;
    static std::string m_boxMessageType;

    dk::common::LockObject m_lock;
    bool m_isRuning;
    dk::bookstore::model::PushedMessageInfoList m_msgList;
    JsonObjectSPtr m_jsonPageTables;
    std::string m_currentUserID;
    std::string m_deletionThreshold;
    int m_boxMessageUnreadCount;
    int m_postUnreadCountResult;
};

#endif//__PUSHEDMESSAGEMANAGER_H__

