#include "Thirdparties/MiMigrationManager.h"
#include "BookStore/DownloadTaskBuilder.h"
#include "BookStore/BookStoreErrno.h"
#include "DownloadManager/DownloadManager.h"
#include "Utility/JsonObject.h"
#include "Common/CAccountManager.h"
#include "GUI/GUISystem.h"
#include "Utility/WebBrowserUtil.h"
#include "Utility/PathManager.h"
#include "CommonUI/UIUtility.h"
#include "Common/DuoKanServer.h"
#include "Utility/SystemUtil.h"
#include <tr1/functional>
#include "Framework/CNativeThread.h"
#include "Utility.h"

using namespace std;
using namespace std::tr1;
using namespace dk::bookstore;
using namespace dk::common;
using namespace dk::account;
using namespace dk::utility;

namespace dk
{

namespace thirdparties
{

const char* MiMigrationManager::EventAskForMigratingToXiaomi = "EventAskForMigratingToXiaomi";
const char* MiMigrationManager::EventShowMigrationDialog = "EventShowMigrationDialog";

static const std::string XIAOMI_MIGRATION_URI = "/dk_id/migrate/kindle/start";
static const int OVERDUE_DAYS = 7;

class ShowMigrationDlgArgs: public EventArgs
{
public:
    virtual EventArgs* Clone() const
    {
        return new ShowMigrationDlgArgs(*this);
    }
    bool enforce;
};


MiMigrationManager::MiMigrationManager()
    : m_lastPromptTime(0)
{
    CAccountManager* accountManager = CAccountManager::GetInstance();
    SubscribeMessageEvent(CAccountManager::EventAccount, *accountManager,
        std::tr1::bind(
        std::tr1::mem_fn(&MiMigrationManager::OnLoginEvent),
        this,
        std::tr1::placeholders::_1));
    SubscribeMessageEvent(MiMigrationManager::EventShowMigrationDialog, *this,
        std::tr1::bind(
        std::tr1::mem_fn(&MiMigrationManager::OnShowMigrationDialog),
        this,
        std::tr1::placeholders::_1));
    SubscribeMessageEvent(WebBrowserLauncher::EventWebBrowserClosed,
        *WebBrowserLauncher::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&MiMigrationManager::OnWebBrowserClosed),
        this,
        std::tr1::placeholders::_1));
    LoadPromptTime();
}

MiMigrationManager::~MiMigrationManager()
{
}

string MiMigrationManager::GetMigrateStage(const std::string& accountID, const std::string& type)
{
    std::string account = accountID.empty() ? "global" : accountID;
    if (m_migrateStages.find(account) != m_migrateStages.end())
    {
        if (m_migrateStages[account].find(type) != m_migrateStages[account].end())
        {
            return m_migrateStages[account][type];
        }
    }
    return "open";
}

void MiMigrationManager::FetchMigrateStage(const std::string& accountID, const std::string& userData)
{
    DownloadTaskSPtr downloadTask = DownloadTaskBuilder::BuildGetMigrateStageTask(accountID, userData);
    if (downloadTask)
    {
    	SubscribeEvent(DownloadTask::EventDownloadTaskFinished,
                *downloadTask.get(),
                std::tr1::bind(
                    std::tr1::mem_fn(&MiMigrationManager::OnAskForMigratingToXiaomi),
                    this,
                    std::tr1::placeholders::_1));
        DownloadManager::GetInstance()->AddTask(downloadTask);
    }
}

bool MiMigrationManager::OnAskForMigratingToXiaomi(const EventArgs& args)
{
	const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* downloadTask = downloadTaskFinishedArgs.downloadTask;
    std::string userData = downloadTask->GetUserData();
    if (downloadTaskFinishedArgs.succeeded)
    {
        const std::string& result = downloadTask->GetString();
		if(!result.empty())
		{
            MiMigrationManager::MigrateStages stages;
			JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
			if(jsonObj != 0)
			{
                string magrateStageForLogin;
				if(jsonObj->GetStringValue("login", &magrateStageForLogin))
				{
                    if (magrateStageForLogin != "open" && !userData.empty())
                    {
                        ShowMigrationDlgArgs migrationArgs;
                        migrationArgs.enforce = (magrateStageForLogin == "close");
                        FireEvent(EventShowMigrationDialog, migrationArgs);
                    }
                    stages["login"] = magrateStageForLogin;
                    DebugPrintf(DLC_DIAGNOSTIC, "\n\nMigrate Stage Login:%s\n\n", magrateStageForLogin.c_str());
				}

                string migrateStageForRegister;
                if (jsonObj->GetStringValue("reg", &migrateStageForRegister))
                {
                    stages["reg"] = migrateStageForRegister;
                    DebugPrintf(DLC_DIAGNOSTIC, "\n\nMigrate Stage Reg:%s\n\n", migrateStageForRegister.c_str());
                }

                if (!stages.empty())
                {
                    string key = userData.empty() ? "global" : userData;
                    DebugPrintf(DLC_DIAGNOSTIC, "\n\nMigrate Stage Key:%s\n\n", key.c_str());
                    m_migrateStages[key] = stages;
                }
			}
		}
    }

    //if (isLogin)
    //{
    //    MigrateStateForLoginArgs loginArgs;
    //    loginArgs.state = magrateStageForLogin;
    //    loginArgs.userData = userData;
    //    FireEvent(EventMigrationStateForLogin, loginArgs);
    //}

    //if (userData == "register")
    //{
    //    MigrateStateForRegisterArgs regArgs;
    //    regArgs.state = migrateStageForRegister;
    //    FireEvent(EventMigrationStateForRegister, regArgs);
    //}
	return true;
}

bool MiMigrationManager::OnLoginEvent(const EventArgs& args)
{
    CAccountManager* accountManager = CAccountManager::GetInstance();
	if(accountManager->IsLoggedIn())
	{
        CAccountManager::LoginStatus status = accountManager->GetLoginStatus();
		if(CAccountManager::LoggedIn_DuoKan == status)
		{
            FetchMigrateStage(accountManager->GetDuoKanAccount(), accountManager->GetDuoKanAccount());
		}
	}
    else
    {
        //string empty;
        //FetchMigrateStage(empty, "login");
		const AccountEventArgs& accountEventArgs = (const AccountEventArgs&)args;
        if (BSE_DUOKAN_ACCOUNT_LOGIN_CLOSED == accountEventArgs.errorCode ||
            BSE_DUOKAN_ACCOUNT_REGISTER_CLOSED == accountEventArgs.errorCode)
        {
            SNativeMessage msg;
            msg.iType = KMessageDuokanLoginFailed;
            msg.iParam1 = accountEventArgs.errorCode;
            CNativeThread::Send(msg);
        }
    }
    return true;
}

bool MiMigrationManager::OnShowMigrationDialog(const EventArgs& args)
{
    string current_account = CAccountManager::GetInstance()->GetDuoKanAccount();
    //if (m_lastPromptTime == 0 || IsPromptOverdue() || (current_account != m_lastAccount))
    {
        const ShowMigrationDlgArgs& migrateArgs = (const ShowMigrationDlgArgs&)args;
        UIAccountMigrateDlg::PopupMode popupMode = migrateArgs.enforce ? UIAccountMigrateDlg::NOTICE_FORCE : UIAccountMigrateDlg::NOTICE_NORMAL;
        ShowAccountMigrateDlg(popupMode);
    }
    return true;
}


void MiMigrationManager::ShowAccountMigrateDlg(UIAccountMigrateDlg::PopupMode popupMode)
{
	UIAccountMigrateDlg accountDlg(GUISystem::GetInstance()->GetTopFullScreenContainer(), popupMode);
    SubscribeEvent(UIAccountMigrateDlg::EventMigrate,
            accountDlg,
            std::tr1::bind(
                std::tr1::mem_fn(&MiMigrationManager::OnMigrate),
                this,
                std::tr1::placeholders::_1));
    SubscribeEvent(UIAccountMigrateDlg::EventMigrateLater,
            accountDlg,
            std::tr1::bind(
            std::tr1::mem_fn(&MiMigrationManager::OnMigrateLater),
                this,
                std::tr1::placeholders::_1));
    SubscribeEvent(UIAccountMigrateDlg::EventLoginXiaomiAccount,
            accountDlg,
            std::tr1::bind(
            std::tr1::mem_fn(&MiMigrationManager::OnLoginXiaomiAccount),
                this,
                std::tr1::placeholders::_1));
	accountDlg.DoModal();
}

void MiMigrationManager::ShowOptionsForDuokanLoginClosed()
{
    ShowAccountMigrateDlg(UIAccountMigrateDlg::NOTICE_DUOKAN_LOGIN_CLOSED);
}

void MiMigrationManager::ShowOptionsForDuokanRegisterClosed()
{
    UIMessageBox messagebox(GUISystem::GetInstance()->GetTopFullScreenContainer(),
            StringManager::GetStringById(ACCOUNT_DUOKAN_REGISTER_CLOSED_TIP),
            MB_OK | MB_CANCEL);
    if(MB_OK == messagebox.DoModal())
    {
	    if (UIUtility::CheckAndReConnectWifi())
        {
            CAccountManager::GetInstance()->RegisterXiaomiAccount();
        }
    }
}

bool MiMigrationManager::OnWebBrowserClosed(const EventArgs& args)
{
    const WebBrowserArgs& webArgs = (const WebBrowserArgs&)args;
    CommandList commands = webArgs.startArguments;

    if (commands.empty() ||
        commands[0].find(XIAOMI_MIGRATION_URI) == std::string::npos)
    {
        return false;
    }

    UIMessageBox msgBox(GUISystem::GetInstance()->GetTopFullScreenContainer(), StringManager::GetStringById(ACCOUNT_MIGRATE_FAILED));
	UIText* displayText = msgBox.GetTextDisplay();
	displayText->SetAlign(ALIGN_CENTER);
	if (!m_migrateResult.LoadFromFile(PathManager::GetMigrateResultPath().c_str()))
    {
        msgBox.SetText(StringManager::GetStringById(ACCOUNT_MIGRATE_CANCELED));
    }
    else
    {
        string message = m_migrateResult.GetMsg();
	    if (!message.empty())
	    {
		    if (message == "succeed")
		    {
			    msgBox.SetText(StringManager::GetStringById(ACCOUNT_MIGRATE_SUCCEED));
			    msgBox.DoModal();
                CAccountManager::GetInstance()->OnWebBrowserLoginFinished(args);
			    return true;
		    }
		    else if (message == "canceled")
		    {
			    msgBox.SetText(StringManager::GetStringById(ACCOUNT_MIGRATE_CANCELED));
			    msgBox.DoModal();
			    return true;
		    }
            else if (message == "failed")
            {
			    msgBox.SetText(StringManager::GetStringById(ACCOUNT_MIGRATE_FAILED));
			    msgBox.DoModal();
			    return true;
            }
        }
	}
	msgBox.DoModal();
	return false;
}

bool MiMigrationManager::OnMigrate(const EventArgs& args)
{
	return StartMigrate();
}

bool MiMigrationManager::OnLoginXiaomiAccount(const EventArgs& args)
{
	if (UIUtility::CheckAndReConnectWifi())
    {
        CAccountManager::GetInstance()->LoginXiaomiAccount(false);
    }
    return true;
}

bool MiMigrationManager::StartMigrate()
{
	if (UIUtility::CheckAndReConnectWifi())
    {
    	CommandList commands;
		std::string migrateUrl = DuoKanServer::GetBookHost() + XIAOMI_MIGRATION_URI;
        std::string duokanAccount = CAccountManager::GetInstance()->GetDuoKanAccount();
    	commands.push_back(migrateUrl);
        commands.push_back(duokanAccount);
        WebBrowserLauncher::GetInstance()->Exec(commands);
    }
    return true;
}

bool MiMigrationManager::IsPromptOverdue()
{
    time_t current_time = SystemUtil::GetUpdateTimeInMs() / 1000;
    //DebugPrintf(DLC_DIAGNOSTIC, "Current Time: %lld, Last Prompt Time: %lld, Overdue: %lld",
    //    current_time, m_lastPromptTime, OVERDUE_DAYS * 24 * 3600);
    return ((current_time - m_lastPromptTime) >= OVERDUE_DAYS * 24 * 3600);
}

void MiMigrationManager::SavePromptTime()
{
    m_lastPromptTime = SystemUtil::GetUpdateTimeInMs() / 1000;
    m_lastAccount    = CAccountManager::GetInstance()->GetDuoKanAccount();
    const char* path = PathManager::GetMigratePromptTimePath().c_str();
    FILE* pFile = fopen(path, "wb");
    if (pFile != 0)
    {
        fwrite(&m_lastPromptTime, sizeof(time_t), 1, pFile);
        CUtility::WriteStringToFile(m_lastAccount, pFile);
        fclose(pFile);
    }
}

void MiMigrationManager::LoadPromptTime()
{
    const char* path = PathManager::GetMigratePromptTimePath().c_str();
    FILE* pFile = fopen(path, "rb");
    if (pFile != 0)
    {
        fread(&m_lastPromptTime, sizeof(time_t), 1, pFile);
        CUtility::ReadStringFromFile(m_lastAccount, pFile);
        fclose(pFile);
    }
}

bool MiMigrationManager::OnMigrateLater(const EventArgs& args)
{
    SavePromptTime();
    return true;
}

}

}