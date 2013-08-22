#include "TouchAppUI/UIPersonalSettingPanel2.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/SystemSetting_DK.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITouchComplexButton.h"
#include "Thirdparties/EvernoteManager.h"
#include "CommonUI/UIUtility.h"
#include <tr1/functional>

using namespace WindowsMetrics;
using namespace dk::thirdparties;

UIPersonalSettingPanel2::UIPersonalSettingPanel2()
    : m_bindSizer(NULL)
    , m_userNameSizer(NULL)
    , m_unbindSizer(NULL)
{
	SubscribeMessageEvent(EvernoteManager::EventEvernoteAuthFinished,
        *EvernoteManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalSettingPanel2::OnEvernoteAuthFinished),
        this,
        std::tr1::placeholders::_1));

    SubscribeMessageEvent(EvernoteManager::EventEvernoteGetUserFinished,
        *EvernoteManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalSettingPanel2::OnEvernoteGetUserFinished),
        this,
        std::tr1::placeholders::_1));
    Init();
}

UIPersonalSettingPanel2::~UIPersonalSettingPanel2()
{
}

void UIPersonalSettingPanel2::Init()
{
    AddSectionTitle(StringManager::GetPCSTRById(EXPORT_ACCOUNT_SETTING));
    m_itemVertSpacing = m_itemTipVertSpacing;
    UISizer* rightSizer = AddPushButton(
            StringManager::GetPCSTRById(EVERNOTE),
            SBI_UnbindEvernote,
            StringManager::GetPCSTRById(UNBIND), &m_bindSizer);
    //rightSizer->AddSpacer(m_itemHorzSpacing);
    std::tr1::shared_ptr<EvernoteUser> user = EvernoteManager::GetInstance()->GetCurrentUser();
    if (user != 0)
        m_userNameSizer = AddItemTips(user->userName().c_str());
    else
        m_userNameSizer = AddItemTips("");
    m_userName = (UITextSingleLine*)m_dynamicCreatedWindows.back();

    AddPushButton(
            StringManager::GetPCSTRById(EVERNOTE),
            SBI_BindEvernote,
            StringManager::GetPCSTRById(BIND), &m_unbindSizer);
    UpdateData();
}

bool UIPersonalSettingPanel2::OnChildClick(UIWindow* child)
{
    switch (child->GetId())
    {
        case SBI_BindEvernote:
            OnBindEvernoteClick();
            break;
        case SBI_UnbindEvernote:
            OnUnbindEvernoteClick();
            break;
        default:
            break;
    }
    return true;
}


void UIPersonalSettingPanel2::OnBindEvernoteClick()
{
    if(!UIUtility::CheckAndReConnectWifi())
    {
        return;
    }

    EvernoteManager::GetInstance()->Auth();
}

void UIPersonalSettingPanel2::OnUnbindEvernoteClick()
{
    EvernoteManager::GetInstance()->Unbind();
    UpdateData();
}

bool UIPersonalSettingPanel2::OnEvernoteAuthFinished(const EventArgs& args)
{
    UpdateData();
    if (!IsDisplay())
    {
        // Ignore the auth finished event if this page is invisible
        return false;
    }
    const EvernoteTokenArgs& tokenArgs = (const EvernoteTokenArgs&)args;
    if (tokenArgs.succeeded)
    {
        UIUtility::SetScreenTips(StringManager::GetStringById(EVERNOTE_BIND_SUCCEED));
    	m_userName->SetText(StringManager::GetPCSTRById(EXPORT_FETCHING_USER_INFO));
        EvernoteManager::GetInstance()->FetchUser();
    }
    else
    {
        UIUtility::SetScreenTips(StringManager::GetStringById(EVERNOTE_BIND_FAILED));
    }
    return true;
}

bool UIPersonalSettingPanel2::OnEvernoteGetUserFinished(const EventArgs& args)
{
    const EvernoteGetUserArgs& evernoteArgs = (const EvernoteGetUserArgs&)args;
    if (!evernoteArgs.succeeded)
    {
        // display error message
        m_userName->SetText(StringManager::GetPCSTRById(EXPORT_FETCHING_USER_INFO_FAILED));
    }
    UpdateData();
    return true;
}

void UIPersonalSettingPanel2::InitStatus()
{
}

void UIPersonalSettingPanel2::UpdateData()
{
    EvernoteTokenPtr token = EvernoteManager::GetInstance()->GetCurrentToken();
    if (token == 0 || token->token().empty())
    {
        m_bindSizer->Show(false);
        m_userNameSizer->Show(false);
        m_unbindSizer->Show(true);
    }
    else
    {
        EvernoteUserPtr user = EvernoteManager::GetInstance()->GetCurrentUser();
        if (user != 0 && !user->userName().empty())
            m_userName->SetText(user->userName().c_str());
        m_bindSizer->Show(true);
        m_userNameSizer->Show(true);
        m_unbindSizer->Show(false);
    }
    Layout();
    Repaint();
}

