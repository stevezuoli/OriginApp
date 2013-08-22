#include <tr1/functional>
#include "Framework/CNativeThread.h"
#include "Utility/ThreadHelper.h"
#include "Thirdparties/EvernoteManager.h"
#include "CommonUI/UIUtility.h"
#include "common/WindowsMetrics.h"
#include "Utility/WebBrowserUtil.h"
#include "Utility/FileSystem.h"
#include "Utility/PathManager.h"
#include "Utility/XmlUtil.h"
#include "Framework/CDisplay.h"
#include "TouchAppUI/UIBookMenuDlg.h"
#include "I18n/StringManager.h"

using namespace std;
using namespace dk::utility;
using namespace std::tr1;

namespace dk
{
namespace thirdparties
{

pthread_t EvernoteManager::m_pThread = 0;
static const char* EvernoteExplorerBin = "evernote_explorer";
static const char* ExecEnv = "source /DuoKan/setup";
static const char* MakeRootRW = "mntroot rw";
static const char* EVERNOTE_AUTH_URL = "www.evernote.com";
static const char* YINXIANG_AUTH_URL = "app.yinxiang.com";
static const char* SANDBOX_AUTH_URL = "sandbox.evernote.com";

const char* EvernoteManager::EventEvernoteAuthFinished = "EventWebBrowserLaunched";
const char* EvernoteManager::EventEvernoteGetUserFinished = "EventWebBrowserClosed";
const char* EvernoteManager::EventEvernoteExportFinished = "EventEvernoteExportFinished";
const char* EvernoteManager::EventEvernoteExplorerQuit = "EventEvernoteExplorerQuit";


static CommandList GetAuthCommand(EvernoteManager::AuthType type)
{
    CommandList commands;
    switch (type)
    {
    case EvernoteManager::EVERNOTE_AUTH:
        commands.push_back(EVERNOTE_AUTH_URL);
        break;
    case EvernoteManager::YINXIANG_AUTH:
        commands.push_back(YINXIANG_AUTH_URL);
        break;
    case EvernoteManager::SANDBOX_AUTH:
        commands.push_back(SANDBOX_AUTH_URL);
        break;
    default:
        break;
    }
    return commands;
}

static bool IsEvernoteAuthCommand(const string& command)
{
    return (command.compare(EVERNOTE_AUTH_URL) == 0 ||
            command.compare(YINXIANG_AUTH_URL) == 0 ||
            command.compare(SANDBOX_AUTH_URL) == 0);
}

static DK_ReturnCode ReadAtomNodeValue(XMLDomNode &_DomNode, std::string &_strValue)
{
    XMLDomNode TmpNode;
    DK_ReturnCode rc = _DomNode.GetLastChild(&TmpNode);
    if(rc != DKR_OK)
    {
        return rc;
    }
    if(TmpNode.GetNodeType() == DK_XML_NODE_TEXT)
    {
        if(DK_NULL != TmpNode.GetNodeValue())
        {
            _strValue = TmpNode.GetNodeValue();
            return DKR_OK;
        }
    }
    return DKR_FAILED;
}

static bool LoadFromDocument(const std::string& path, std::map<std::string, std::string>& infoMap)
{
    XMLDomDocument* pDocument = XMLDomDocument::CreateDocument();
    if (0 == pDocument)
    {
        return false;
    }

    if (!pDocument->LoadXmlFromFile(path.c_str(), false))
    {
        XMLDomDocument::ReleaseDocument(pDocument);
        return false;
    }

    XMLDomNode* pRootNode = pDocument->RootElement();
    if (0 == pRootNode)
    {
        XMLDomDocument::ReleaseDocument(pDocument);
        return false;
    }

    DK_INT count = 0;
    infoMap.clear();
    pRootNode->GetChildrenCount(&count);
    for (DK_INT i = 0; i < count; ++i)
    {
        XMLDomNode TmpNode;
        pRootNode->GetChildByIndex(i, &TmpNode);
        std::string strTempValue = "";
        if(DK_NULL == TmpNode.GetNodeValue())
        {
            continue;
        }
        
        if(DKR_OK == ReadAtomNodeValue(TmpNode, strTempValue) && !strTempValue.empty())
        {
            infoMap[TmpNode.GetNodeValue()] = strTempValue;
        }
    }
    XMLDomDocument::ReleaseDocument(pDocument);
    return true;
}

bool EvernoteUser::Load(const std::string& path)
{
    m_infoMap.clear();
    return LoadFromDocument(path, m_infoMap);
}

string EvernoteUser::userName()
{
    return m_infoMap["username"];
}

string EvernoteUser::email()
{
    return m_infoMap["email"];
}

string EvernoteUser::name()
{
    return m_infoMap["name"];
}

string EvernoteUser::timeZone()
{
    return m_infoMap["timezone"];
}

bool EvernoteToken::Load(const std::string& path)
{
    m_infoMap.clear();
    return LoadFromDocument(path, m_infoMap);
}

string EvernoteToken::token()
{
    return m_infoMap["token"];
}

string EvernoteToken::host()
{
    return m_infoMap["host"];
}

EvernoteManager::EvernoteManager()
{
    SubscribeMessageEvent(WebBrowserLauncher::EventWebBrowserClosed,
        *WebBrowserLauncher::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&EvernoteManager::OnAuthFinished),
        this,
        std::tr1::placeholders::_1));

    EvernoteToken token;
    if (token.Load(PathManager::GetEvernoteTokenPath()))
    {
        m_currentToken.reset(new EvernoteToken());
        *m_currentToken = token;
    }

    EvernoteUser user;
    if (user.Load(PathManager::GetEvernoteUserPath()))
    {
        m_currentUser.reset(new EvernoteUser());
        *m_currentUser = user;
    }
}

EvernoteManager::~EvernoteManager()
{
}

bool EvernoteManager::Auth(AuthType type)
{
    std::vector<int> btnIDs;
    btnIDs.reserve(8);
    btnIDs.push_back(ID_BTN_ACCOUNT_YINXIANG);
    btnIDs.push_back(ID_BTN_ACCOUNT_EVERNOTE);
    //btnIDs.push_back(ID_BTN_ACCOUNT_SANDBOX);
    btnIDs.push_back(ID_INVALID);

    std::vector<int> strIDs;
    strIDs.reserve(8);
    strIDs.push_back(YINXIANG);
    strIDs.push_back(EVERNOTE_INTERNATIONAL);
    //strIDs.push_back(SANDBOX);
    strIDs.push_back(-1);

    CDisplay* pDisplay = CDisplay::GetDisplay();
    UIBookMenuDlg dlgMenu(0, btnIDs, strIDs); 
	int dlgWidth = dlgMenu.GetWidth();
	int dlgHeight = dlgMenu.GetHeight();
	dlgMenu.MoveWindow((pDisplay->GetScreenWidth() - dlgWidth)/2, (pDisplay->GetScreenHeight() - dlgHeight)/2, dlgWidth, dlgHeight);
    if (dlgMenu.DoModal() == IDOK)
    {
        AuthType this_type = type;
        switch (dlgMenu.GetCommandId())
        {
        case ID_BTN_ACCOUNT_YINXIANG:
            this_type = YINXIANG_AUTH;
            break;
        case ID_BTN_ACCOUNT_EVERNOTE:
            this_type = EVERNOTE_AUTH;
            break;
        case ID_BTN_ACCOUNT_SANDBOX:
            this_type = SANDBOX_AUTH;
            break;
        default:
            break;
        }
        // Launch web browser to authorize.
        // Web Browser exists when authorization finishes.
        // Event EventEvernoteAuthFinished will be fired.
        return WebBrowserLauncher::GetInstance()->Exec(GetAuthCommand(this_type));
    }
    return false;
}

bool EvernoteManager::Unbind()
{
    FileSystem::DeleteFile(PathManager::GetEvernoteTokenPath());
    FileSystem::DeleteFile(PathManager::GetEvernoteUserPath());
    m_currentUser = std::tr1::shared_ptr<EvernoteUser>();
    m_currentToken = std::tr1::shared_ptr<EvernoteToken>();
    return true;
}

bool EvernoteManager::FetchUser()
{
    if (IsBusy())
    {
        return false;
    }

    // get host
    if (m_currentToken == 0)
    {
        m_currentToken.reset(new EvernoteToken());
    }
    if (!m_currentToken->Load(PathManager::GetEvernoteTokenPath()))
    {
        DebugPrintf(DLC_DIAGNOSTIC, "EvernoteManager: cannot load invalid token!");
        return false;
    }

    std::string command_str("getuser");
    m_currentCommand = command_str;
    command_str = command_str.append(" ").append(m_currentToken->host());
    //DebugPrintf(DLC_DIAGNOSTIC, "EvernoteManager::FetchUser %s", command_str.c_str());
    int err = ThreadHelper::CreateThread(&m_pThread,
                                         EvernoteExplorerThread,
                                         (void*)(command_str.c_str()),
                                         "EvernoteManager @ EvernoteExplorerThread",
                                         true,
                                         51200,
                                         SQM_THREAD_PRIORITY);
    
    //DebugPrintf(DLC_DIAGNOSTIC, "EvernoteManager::EvernoteExplorerThread %d#, err(%d)", m_pThread, err);
    if (err != 0)
    {
        EvernoteManager::m_pThread = 0;
        SendEvernoteExplorerQuitMessage();
        return false;
    }
    return true;
}

bool EvernoteManager::ExportLocalNote(const int bookId)
{
    if (IsBusy())
    {
        return false;
    }

    // get host
    if (m_currentToken == 0)
    {
        m_currentToken.reset(new EvernoteToken());
    }
    if (!m_currentToken->Load(PathManager::GetEvernoteTokenPath()))
    {
        DebugPrintf(DLC_DIAGNOSTIC, "EvernoteManager: cannot load invalid token!");
        return false;
    }

    if (FileSystem::SaveLocalNoteToEvernoteFile(bookId,
                                       m_currentBookName,
                                       m_currentAuthor))
    {
        std::string command_str = PathManager::GetEvernoteInternalPath();
        m_currentCommand = command_str;
        command_str = command_str.append(" ").append(m_currentToken->host());

        int err = ThreadHelper::CreateThread(&m_pThread,
                                             EvernoteExplorerThread,
                                             (void*)(command_str.c_str()),
                                             "EvernoteManager @ EvernoteExplorerThread",
                                             true,
                                             51200,
                                             SQM_THREAD_PRIORITY);
    
        //DebugPrintf(DLC_DIAGNOSTIC, "EvernoteManager::EvernoteExplorerThread %d#, err(%d)", m_pThread, err);
        if (err != 0)
        {
            EvernoteManager::m_pThread = 0;
            SendEvernoteExplorerQuitMessage();
            return false;
        }
        return true;
    }
    return false;
}

bool EvernoteManager::ExportCloudNote(const string& content,
                                      const string& bookName,
                                      const string& author)
{
    if (IsBusy())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "EvernoteManager is busy, Refuse to export cloud note");
        return false;
    }
    
    // get host
    if (m_currentToken == 0)
    {
        m_currentToken.reset(new EvernoteToken());
    }
    if (!m_currentToken->Load(PathManager::GetEvernoteTokenPath()))
    {
        DebugPrintf(DLC_DIAGNOSTIC, "EvernoteManager: cannot load invalid token!");
        return false;
    }

    m_currentBookName = bookName;
    m_currentAuthor = author;
    if (FileSystem::SaveCloudNoteToEvernoteFile(content,
                                       bookName,
                                       author))
    {
        std::string command_str = PathManager::GetEvernotePath();
        m_currentCommand = command_str;
        command_str = command_str.append(" ").append(m_currentToken->host());

        int err = ThreadHelper::CreateThread(&m_pThread,
                                             EvernoteExplorerThread,
                                             (void*)(command_str.c_str()),
                                             "EvernoteManager @ EvernoteExplorerThread",
                                             true,
                                             51200,
                                             SQM_THREAD_PRIORITY);
    
        //DebugPrintf(DLC_DIAGNOSTIC, "EvernoteManager::EvernoteExplorerThread %d#, err(%d)", m_pThread, err);
        if (err != 0)
        {
            EvernoteManager::m_pThread = 0;
            SendEvernoteExplorerQuitMessage();
            return false;
        }
        return true;
    }
    DebugPrintf(DLC_DIAGNOSTIC, "EvernoteManager cannot save note to file.");
    return false;
}

void* EvernoteManager::EvernoteExplorerThread(void* para)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "EvernoteExplorer: para=(%s)", (char *)para);
    char cmd[512];
    memset(cmd, 0, 512);
    sprintf(cmd, "%s;%s;%s %s", ExecEnv, MakeRootRW, EvernoteExplorerBin, (const char*)para);

    DebugPrintf(DLC_DIAGNOSTIC, "%s", cmd);
    system(cmd);
 
    SendEvernoteExplorerQuitMessage();
    return 0;
}

void EvernoteManager::SendEvernoteExplorerQuitMessage()
{
    SNativeMessage msg;
    msg.iType = KMessageThirdPartyAppFinished;
    msg.iParam1 = EVERNOTE_TYPE;
    CNativeThread::Send(msg);
}

bool EvernoteManager::OnEvernoteExplorerQuited()
{
    m_pThread = 0;
    //DebugPrintf(DLC_DIAGNOSTIC, "Evernote Explorer Param:%s", m_currentCommand.c_str());
    if (m_currentCommand.compare("getuser") == 0)
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "Evernote: GetUser Done");
        EvernoteGetUserArgs userArgs;
        userArgs.succeeded = userArgs.user.Load(PathManager::GetEvernoteUserPath());
        if (userArgs.succeeded)
        {
            if (m_currentUser == 0)
            {
                m_currentUser.reset(new EvernoteUser());
            }
            *m_currentUser = userArgs.user;
            //DebugPrintf(DLC_DIAGNOSTIC, "EventEvernoteGetUser Succeeded"); 
        }
        else
        {
            DebugPrintf(DLC_DIAGNOSTIC, "EventEvernoteGetUser Failed"); 
        }
        FireEvent(EventEvernoteGetUserFinished, userArgs);
    }
    else if (m_currentCommand.compare(PathManager::GetEvernoteInternalPath()) == 0)
    {
        EvernoteExportArgs exportArgs;
        exportArgs.book_name = m_currentBookName;
        exportArgs.author    = m_currentAuthor;
        exportArgs.succeeded = !FileSystem::IsEvernoteFileExist(m_currentBookName, m_currentAuthor, true);
        exportArgs.is_internel = true;
        FireEvent(EventEvernoteExportFinished, exportArgs);
    }
    else if (m_currentCommand.compare(PathManager::GetEvernotePath()) == 0)
    {
        EvernoteExportArgs exportArgs;
        exportArgs.book_name = m_currentBookName;
        exportArgs.author    = m_currentAuthor;
        exportArgs.succeeded = !FileSystem::IsEvernoteFileExist(m_currentBookName, m_currentAuthor, false);
        exportArgs.is_internel = false;
        FireEvent(EventEvernoteExportFinished, exportArgs);
    }
    return true;
}

bool EvernoteManager::IsBusy()
{
    return m_pThread != 0;
}
   
bool EvernoteManager::OnAuthFinished(const EventArgs& args)
{
    const WebBrowserArgs& webArgs = (const WebBrowserArgs&)args;
    if (!webArgs.startArguments.empty() && IsEvernoteAuthCommand(webArgs.startArguments[0]))
    {
        EvernoteTokenArgs tokenArgs;
        tokenArgs.succeeded = tokenArgs.token.Load(PathManager::GetEvernoteTokenPath());
        if (tokenArgs.succeeded)
        {
            if (m_currentToken == 0)
            {
                m_currentToken.reset(new EvernoteToken());
            }
            *m_currentToken = tokenArgs.token;
        }
        FireEvent(EventEvernoteAuthFinished, tokenArgs);
        return true;
    }
    return false;
}

} // namespace utiltiy
} // namespace dk

