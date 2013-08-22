#include "TouchAppUI/UIPersonalSettingPanel1Login.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "Common/SystemSetting_DK.h"
#include "Common/CAccountManager.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITouchComplexButton.h"

using namespace WindowsMetrics;
using namespace dk::account;

UIPersonalSettingPanel1Login::UIPersonalSettingPanel1Login()
{
    Init();
}

UIPersonalSettingPanel1Login::~UIPersonalSettingPanel1Login()
{
}

void UIPersonalSettingPanel1Login::Init()
{
    AddSectionTitle(StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_ACCOUNTINFO));
    m_titleUpSpacing -= (m_itemVertSpacing);
    int savedItemVertSpacing = m_itemVertSpacing;
    m_itemVertSpacing = m_itemTipVertSpacing;
    AddPushButton(StringManager::GetPCSTRById(ACCOUNT),
            SBI_Logout,
            StringManager::GetPCSTRById(ACCOUNT_LOGOUT));
    CAccountManager* accountManager = CAccountManager::GetInstance();
    m_itemVertSpacing = savedItemVertSpacing;
    AddItemTips(accountManager->GetEmailFromFile().c_str());
    m_account = (UITextSingleLine*)m_dynamicCreatedWindows.back();

    m_itemVertSpacing = m_itemTipVertSpacing;
    AddPushButton(StringManager::GetPCSTRById(ACCOUNT_USERNAME),
            SBI_ModifyPersonalInfo,
            StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_ACCOUNT_MODIFYUSER));
	m_btnModifyInfo = (UITouchComplexButton*)m_dynamicCreatedWindows.back();
	m_itemVertSpacing = savedItemVertSpacing;
    AddItemTips(accountManager->GetAliasFromFile().c_str());
    m_alias = (UITextSingleLine*)m_dynamicCreatedWindows.back();
	
    AddSectionTitle(StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_PUSHES));
    m_itemVertSpacing = m_itemTipVertSpacing;
    AddPushButton(StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_PUSHBOOK_MAIL),
        SBI_PushMailBox,
        StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_PUSHBOOK_TIPS));
    m_itemVertSpacing = savedItemVertSpacing;
    AddItemTips(accountManager->GetDuoKanMailFromFile().c_str());
    m_pushMailBox = (UITextSingleLine*)m_dynamicCreatedWindows.back();
}

void UIPersonalSettingPanel1Login::UpdateData()
{
    CAccountManager* accountManager = CAccountManager::GetInstance();
    m_account->SetText(accountManager->GetEmailFromFile().c_str());
    m_alias->SetText(accountManager->GetAliasFromFile().c_str());
    m_pushMailBox->SetText(accountManager->GetDuoKanMailFromFile().c_str());
	if(CAccountManager::LoggedIn_XiaoMi == accountManager->GetLoginStatus())
	{
		m_btnModifyInfo->SetVisible(false);
	}
	else
	{
		m_btnModifyInfo->SetVisible(true);
	}
	Layout();
}
