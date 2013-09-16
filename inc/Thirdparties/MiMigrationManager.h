#ifndef MI_MIGRATION_MANAGER_H_
#define MI_MIGRATION_MANAGER_H_

#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"
#include "GUI/EventListener.h"
#include "Thirdparties/ThirdpartyTypes.h"
#include "Common/UserInfo.h"
#include "CommonUI/UIAccountMigrateDlg.h"

using namespace std::tr1;
using namespace std;

namespace dk
{
namespace account
{
    class CAccountManager;
};
};

namespace dk
{

namespace thirdparties
{

    class MigrationOperationArgs: public EventArgs
    {
    public:
        virtual EventArgs* Clone() const
        {
            return new MigrationOperationArgs(*this);
        }
        int operation_id;
    };

//class MigrateStateForRegisterArgs: public EventArgs
//{
//public:
//    virtual EventArgs* Clone() const
//    {
//        return new MigrateStateForRegisterArgs(*this);
//    }
//    std::string state;
//};
//
//class MigrateStateForLoginArgs: public EventArgs
//{
//public:
//    virtual EventArgs* Clone() const
//    {
//        return new MigrateStateForLoginArgs(*this);
//    }
//    std::string state;
//    std::string userData;
//};

class MiMigrationManager : public EventSet, EventListener
{
    MiMigrationManager();
    MiMigrationManager(const MiMigrationManager& right);
public:
    virtual ~MiMigrationManager();

    static MiMigrationManager* GetInstance() {
        static MiMigrationManager manager;
        return &manager;
    }

    void FetchMigrateStage(const std::string& accountID, const std::string& userData);
    string GetMigrateStage(const std::string& accountID, const std::string& type);
    bool StartMigrate();
    void ShowOptionsForDuokanLoginClosed();
    void ShowOptionsForDuokanRegisterClosed();

    void FireMigrationEvent();

private:
    static const char* EventMigrate;
    static const char* EventAskForMigratingToXiaomi;
    static const char* EventShowMigrationDialog;

private:
    // message handler
    bool OnLoginEvent(const EventArgs& args);
    bool OnAskForMigratingToXiaomi(const EventArgs& args);
    bool OnShowMigrationDialog(const EventArgs& args);
    bool OnWebBrowserClosed(const EventArgs& args);
    bool OnMigrate(const EventArgs& args);
    bool OnMigrateLater(const EventArgs& args);
    bool OnLoginXiaomiAccount(const EventArgs& args);

private:
    void ShowAccountMigrateDlg(UIAccountMigrateDlg::PopupMode popupMode);
    bool IsPromptOverdue();
    void SavePromptTime();
    void LoadPromptTime();

private:
    typedef map<string, string> MigrateStages;
    typedef map<string, MigrateStages> DuokanMigrateStages;

private:
    time_t m_lastPromptTime;
    std::string m_lastAccount;
    dk::account::MigrateResult m_migrateResult;
    DuokanMigrateStages m_migrateStages;
};

};
};
#endif
