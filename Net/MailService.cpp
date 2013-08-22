#include "Framework/CNativeThread.h"
#include <tr1/functional>
#include "Common/CAccountManager.h"
#include "Utility/ThreadHelper.h"
#include "Utility/TimerThread.h"
#include "MailService.h"
#include <errno.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include <sys/stat.h>
#include "interface.h"
#include "Utility/SystemUtil.h"
#include "I18n/StringManager.h"

using dk::utility::SystemUtil;
using dk::utility::TimerThread;
using dk::account::CAccountManager;
using dk::account::AccountEventArgs;


const char* MailService::EventMailRecv = "MailService::EventMailRecv";
SINGLETON_CPP(MailService)

MailService::MailService(): m_host(""), m_username(""), m_password(""), m_localpath(""),m_pMailCheckThread(0)
{
    SubscribeMessageEvent(CAccountManager::EventAccount, *CAccountManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&MailService::OnAccountLogInOut),
        this,
        std::tr1::placeholders::_1));
};

bool MailService::Config(std::string host, std::string username, std::string password, std::string localpath)
{
    DebugPrintf(DLC_DIAGNOSTIC, "MailService::Config # %s, %s, %s, %s", host.c_str(), username.c_str(), password.c_str(), localpath.c_str());
    m_host = host;
    m_username = username;
    m_password = password;
    m_localpath = localpath;
    //m_pMailCheckThread =0;
    return true;
}

void MailService::Send(std::string des_address, std::string att_file)
{
    std::string cmd(PYTHON_PATH);

    cmd += SMTP_SCRIPT;
    cmd += m_host;
    cmd += " ";
    cmd += m_username;
    cmd += " ";
    cmd += m_password;
    cmd += " ";
    cmd += m_username;
    cmd += " ";
    cmd += des_address;
    cmd += " ";
    cmd += att_file;
    cmd += " ";

    SystemUtil::ExecuteCommand(cmd.c_str());
}

void MailService::CheckSync()
{
    std::string cmd(PYTHON_PATH);

    cmd += POP3_SCRIPT;
    cmd += m_host;
    cmd += " ";
    cmd += "110";
    cmd += " ";
    cmd += m_username;
    cmd += " ";
    cmd += m_password;
    cmd += " ";
    cmd += SYNC_PATH;
    cmd += " ";

    //DebugPrintf(DLC_DIAGNOSTIC, "MailService::CheckSync #");
    FILE* fp = popen(cmd.c_str(), "r");
    //DebugPrintf(DLC_DIAGNOSTIC, "MailService::CheckSync popen#");
    if (!fp)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "MailService::GetMailRecvInfoThread file = NULL, return");
        return;
    }

    while (1)
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "in While 1 MailService: %d, %x", __LINE__, fp);
        char text[100] = {0};
        char* p = fgets(text, 100, fp);
        if (NULL == p)
        {
            //当python脚本异常时，在未IsCheckOver()检测前，会读取为null，此时退出，提示python错误
            DebugPrintf(DLC_DIAGNOSTIC, "fgets return NULL, return");
            pclose(fp);
            SetMailCheckText("python error!");
            return;
        }
        if (strlen(text) != 0)
        {
            //DebugPrintf(DLC_DIAGNOSTIC, "%%%%%% %s", text);
            SetMailCheckText(text);
            if (IsCheckOver(text))
            {
                //DebugPrintf(DLC_DIAGNOSTIC, "read: %s", text);
                break;
            }
        }
        sleep(1);
    }

    pclose(fp);
    MailSyncHandle();
}

void MailService::MailSyncHandle()
{
    SNativeMessage msg;
    msg.iType = KMessageMailSyncUp;
    CNativeThread::Send(msg);
}

MailService::~MailService()
{
}

bool MailService::OnAccountLogInOut(const EventArgs& args)
{
    const AccountEventArgs& accountEventArgs = (const AccountEventArgs&)args;
    if (accountEventArgs.IsLoggedIn())
    {
        //DebugPrintf(DLC_DIAGNOSTIC,
        //        "login start check thread %d",
        //        m_pMailCheckThread);
        if (!m_pMailCheckThread)
        {
            CAccountManager* pManager = CAccountManager::GetInstance();
            MailService::GetInstance()->Config("mail.iduokan.com",
                    pManager->GetDuoKanMailFromFile(),
                    pManager->GetDuoKanMailPasswordFromFile(),
                    SYNC_PATH);
            StartCheckThread();
        }
    }
    else
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "logout stop check thread %d", m_pMailCheckThread);
        StopCheckThread();
    }
    return true;
}

bool MailService::CheckPythonExist(string& text)
{
    struct stat s;
    string cmd(PYTHON_PATH);
    cmd = cmd.substr(0, cmd.length() - 1);
    int result = stat(cmd.c_str(), &s);
    if (result == -1)
    {
        text = strerror(errno);
        return false;
    }

    return true;
}

void MailService::StartCheckThread()
{
    string result;
    if (!CheckPythonExist(result))
    {
        DebugPrintf(DLC_DIAGNOSTIC, "python: %s", result.c_str());
        SetMailCheckText("python not found");
        return;
    }

    StopCheckThread();
    if (0 == m_pMailCheckThread)
    {
        int err  = ThreadHelper::CreateThread(&m_pMailCheckThread, MailService::CheckSyncThread, NULL,  "MailService @ CheckSyncThread",  true);
        if (!err)
        {
            DebugPrintf(DLC_DIAGNOSTIC, "MailService::StartCheckThread StartCheckThread %d#", m_pMailCheckThread);
        }
    }
}

void MailService::StopCheckThread()
{
    if (m_pMailCheckThread)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "MailService::StopCheckThread StopCheckThread# m_pThreadDaemon");
        ThreadHelper::CancelThread(m_pMailCheckThread);
        ThreadHelper::JoinThread(m_pMailCheckThread, NULL);
        m_pMailCheckThread = 0;
    }
    SetMailCheckText("");
}

void* MailService::CheckSyncThread(void * pUseless)
{
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
    
    sleep(15);

    while(true)
    {
        MailService::GetInstance()->CheckSync();

       // check every 300 seconds
        for(int i = 0; i < 60; i++)
        {
            pthread_testcancel();
            sleep(5);
        }
    }
    return NULL;
}

bool MailService::IsCheckOver(const std::string& text) const
{
    if ((text == "Mail fetch finished!\n"))
        return true;
    
    return false;
}

std::string MailService::GetMailCheckInfo() const
{
    return m_mailCheckText;
}

void MailService::ClearMailCheckText(void* data) 
{
    if (string((char*)data) == StringManager::GetPCSTRById(MAILCHECKNORESULT))
    {
        MailService::GetInstance()->SetMailCheckText("");
    }
}

bool MailService::SetMailCheckText(const std::string& text)
{
    bool textChanged = true;
    if (text.compare("RecvMail Begin\n") == 0)
    {
        m_mailCheckText = StringManager::GetPCSTRById(MAILCHECKSTART);
    }
    else if (text.compare("RecvMailGotResult\n") == 0)
    {
        m_mailCheckText = StringManager::GetPCSTRById(MAILCHECKGOTRESULT);
    }
    else if (text.compare("RecvMailNoResult\n") == 0)
    {
        m_mailCheckText = StringManager::GetPCSTRById(MAILCHECKNORESULT);
        TimerThread::GetInstance()->AddTimer(MailService::ClearMailCheckText, (void*)(m_mailCheckText.c_str()), SystemUtil::GetUpdateTimeInMs() + 5*1000, false, false, 0);
    }
    //else if (text.compare("MailConnected\n") == 0)
    //{
    //    m_mailCheckText = StringManager::GetPCSTRById(MailConnected);
    //}
    else if (text.find("python not found") != string::npos)
    {
        m_mailCheckText = StringManager::GetPCSTRById(MAILPYTHONNOFOUND);
    }
    else if (text.compare("MailDisConnected\n") == 0)
    {
        m_mailCheckText = StringManager::GetPCSTRById(MAILCANTCONNECTED);
    }
    else if (text.compare(0, 9, "RecvMail:") == 0)
    {
        m_mailCheckText = text.substr(9, text.length() - 1 - 9);
        m_mailCheckText += "...";
    }
    else if (text.empty())
    {
        m_mailCheckText.clear();
    }
    else
    {
        textChanged = false;
    }

    if (textChanged)
    {
        EventMailRecvArgs mailRecvArgs;
        FireEvent(EventMailRecv, mailRecvArgs);
    }

    return textChanged;
}
