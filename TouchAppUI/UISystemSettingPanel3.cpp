#include "TouchAppUI/UISystemSettingPanel3.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "Common/SystemSetting_DK.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UIMessageBox.h"
#include "Common/SystemOperation.h"
#include "SQM.h"

using namespace WindowsMetrics;

UISystemSettingPanel3::UISystemSettingPanel3()
{
    Init();
}

UISystemSettingPanel3::~UISystemSettingPanel3()
{
}

void UISystemSettingPanel3::Init()
{
    AddSectionTitle(StringManager::GetPCSTRById(ABOUT_DUOKAN)); m_titleUpSpacing -= m_itemVertSpacing;
    AddLabel(StringManager::GetPCSTRById(SYSTEM_VERSION),
            GetVersion().c_str());
    AddPushButton(StringManager::GetPCSTRById(SYSTEM_UNINSTALL),
            SBI_Uninstall,
            StringManager::GetPCSTRById(SYS_UNINSTALL));
    AddLabel(StringManager::GetPCSTRById(STORAGE_CAPACITY),
            GetDiskSpace().c_str());
    AddPushButton(StringManager::GetPCSTRById(COPYRIGHT),
            SBI_CopyRight,
            StringManager::GetPCSTRById(SYS_CHECK));
    AddSectionTitle(StringManager::GetPCSTRById(ABOUT_KINDLE));
    AddLabel(StringManager::GetPCSTRById(SYSTEM_VERSION),
            GetKindleVersion().c_str());
    AddLabel(StringManager::GetPCSTRById(SN),
            GetKindleSn().c_str());
    AddLabel(StringManager::GetPCSTRById(MAC),
            GetKindleMac().c_str());
}

bool UISystemSettingPanel3::OnChildClick(UIWindow* child)
{
    switch (child->GetId())
    {
        case SBI_Uninstall:
            OnUninstallClick();
            break;
        case SBI_CopyRight:
            OnCopyrightClick();
            break;
        default:
            break;
    }
    return true;
}

void UISystemSettingPanel3::OnUninstallClick()
{
    UIMessageBox messagebox(this, StringManager::GetStringById(SURE_UNINSTALL), MB_OK | MB_CANCEL);
    if(MB_OK == messagebox.DoModal())
    {
        SystemOperation::Uninstall();
        SQM::GetInstance()->IncCounter(SQM_ACTION_SETTINGPAGE_UNINSTALL);
    }
}

void UISystemSettingPanel3::OnCopyrightClick()
{
    UIMessageBox messagebox(this, StringManager::GetStringById(DUOKAN_READER_COPYRIGHT), MB_OK);
    //messagebox.ShowIcon(false);
    messagebox.DoModal();
    SQM::GetInstance()->IncCounter(SQM_ACTION_SETTINGPAGE_COPYRIGHT);
}
