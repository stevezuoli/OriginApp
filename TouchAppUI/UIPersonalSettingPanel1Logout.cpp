#include "TouchAppUI/UIPersonalSettingPanel1Logout.h"
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

UIPersonalSettingPanel1Logout::UIPersonalSettingPanel1Logout()
{
    Init();
}

UIPersonalSettingPanel1Logout::~UIPersonalSettingPanel1Logout()
{
}

void UIPersonalSettingPanel1Logout::Init()
{
    AddSectionTitle(StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_ACCOUNTINFO));
    m_titleUpSpacing -= m_itemVertSpacing;
    UIBoxSizer* sizer = AddPushButton(
            StringManager::GetPCSTRById(ACCOUNT_NOTLOGGEDIN),
            SBI_Login,
            StringManager::GetPCSTRById(ACCOUNT_LOGIN));
	m_btnLogin = (UITouchComplexButton*)m_dynamicCreatedWindows.back();
    sizer->AddSpacer(m_itemHorzSpacing);
    sizer->Add(
            CreateButton(
                SBI_Register,
                StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_ACCOUNT_REGISTER),
                true),
            UISizerFlags().Center());
	m_btnRegister = (UITouchComplexButton*)m_dynamicCreatedWindows.back();
}

void UIPersonalSettingPanel1Logout::UpdateData()
{
	CAccountManager* accountManager = CAccountManager::GetInstance();
	switch(accountManager->GetLoginStatus())
	{
		case CAccountManager::Disconnected:
		case CAccountManager::NotLoggedIn:
		case CAccountManager::LoggedIn_XiaoMi:
		case CAccountManager::LoggedIn_DuoKan:
			{
				m_btnRegister->SetEnable(true);
				m_btnLogin->SetEnable(true);
			}
			break;
		case CAccountManager::Logging:
		case CAccountManager::Registering:
			{
				m_btnRegister->SetEnable(false);
				m_btnLogin->SetEnable(false);
			}
			break;
		default:
			break;
	}
}


