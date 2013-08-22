#ifndef EVERNOTE_MANAGER_H_
#define EVERNOTE_MANAGER_H_

#include <string>
#include <vector>
#include <stdint.h>
#include "Common/Defs.h"
#include <semaphore.h>
#include <tr1/memory>
#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"
#include "GUI/EventListener.h"
#include "BookStore/ReadingBookInfo.h"
#include "Thirdparties/ThirdpartyTypes.h"

using namespace dk::bookstore::sync;
using namespace std::tr1;

namespace dk
{

namespace thirdparties
{

typedef std::vector<std::string> EvernoteCommands;

class EvernoteUser
{
public:
    EvernoteUser() {}
    ~EvernoteUser() {}

    bool Load(const std::string& path);

    string userName();
    string email();
    string name();
    string timeZone();

public:
    std::map<std::string, std::string> m_infoMap;
};

typedef shared_ptr<EvernoteUser> EvernoteUserPtr;

class EvernoteToken
{
public:
    EvernoteToken() {}
    ~EvernoteToken() {}

    bool Load(const std::string& path);
    string token();
    string host();
public:
    std::map<std::string, std::string> m_infoMap;
};

typedef shared_ptr<EvernoteToken> EvernoteTokenPtr;

class EvernoteExportArgs: public EventArgs
{
public:
    EvernoteExportArgs()
        : succeeded(false)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new EvernoteExportArgs(*this);
    }

    bool succeeded;
    bool is_internel;
    string book_name;
    string author;
};

class EvernoteTokenArgs: public EventArgs
{
public:
    EvernoteTokenArgs()
        : succeeded(false)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new EvernoteTokenArgs(*this);
    }

    bool succeeded;
    EvernoteToken token;
};

class EvernoteGetUserArgs: public EventArgs
{
public:
    EvernoteGetUserArgs()
        : succeeded(false)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new EvernoteGetUserArgs(*this);
    }

    bool succeeded;
    EvernoteUser user;
};

class EvernoteExplorerQuitArgs: public EventArgs
{
public:
    EvernoteExplorerQuitArgs() {}
    virtual ~EvernoteExplorerQuitArgs() {}

    virtual EventArgs* Clone() const
    {
        return new EvernoteExplorerQuitArgs(*this);
    }

    string parameter;
};

class EvernoteManager : public EventSet, EventListener
{
public:
    static const char* EventEvernoteAuthFinished;
    static const char* EventEvernoteGetUserFinished;
    static const char* EventEvernoteExportFinished;

    typedef enum AuthType
    {
        EVERNOTE_AUTH = 0,
        YINXIANG_AUTH,
        SANDBOX_AUTH
    };

private:
    static const char* EventEvernoteExplorerQuit;

public:
    static EvernoteManager* GetInstance() {
        static EvernoteManager manager;
        return &manager;
    }

    bool Auth(AuthType type = SANDBOX_AUTH);
    bool FetchUser();
    bool ExportLocalNote(const int bookId);
    bool ExportCloudNote(const string& content,
                         const string& bookName,
                         const string& author);
    bool IsBusy();
    bool Unbind();

    EvernoteUserPtr GetCurrentUser() { return m_currentUser; }
    EvernoteTokenPtr GetCurrentToken() { return m_currentToken; }
    string GetCurrentBookName() { return m_currentBookName; }
    string GetCurrentAuthor() { return m_currentAuthor; }
    bool OnEvernoteExplorerQuited();
private:
    static void* EvernoteExplorerThread(void*);
    static void  SendEvernoteExplorerQuitMessage();

    bool OnAuthFinished(const EventArgs& args); 

private:
    EvernoteManager();
    virtual ~EvernoteManager();

private:
    static pthread_t m_pThread;
    string m_currentCommand;
    string m_currentBookName;
    string m_currentAuthor;
    EvernoteUserPtr m_currentUser;
    EvernoteTokenPtr m_currentToken;
};

} // namespace thirdparties

} // namespace dk
#endif
