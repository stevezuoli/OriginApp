#include "TouchAppUI/UIShareAccountSettingPanel.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
//#include <tr1/functional>
#include "GUI/UISizer.h"
#include "Common/SystemSetting_DK.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITouchComplexButton.h"
#include "Weibo/WeiboFactory.h"

using namespace WindowsMetrics;
using namespace dk::weibo;

UIShareAccountSettingPanel::UIShareAccountSettingPanel()
    : m_bindSizer(NULL)
    , m_userNameSizer(NULL)
    , m_unbindSizer(NULL)
{
    Init();
}

UIShareAccountSettingPanel::~UIShareAccountSettingPanel()
{
}

void UIShareAccountSettingPanel::Init()
{
    m_titleUpSpacing -= m_itemVertSpacing;
    AddSectionTitle(StringManager::GetPCSTRById(SHARE_ACCOUNT_SETTING));
    m_itemVertSpacing = m_itemTipVertSpacing;
    UISizer* rightSizer = AddPushButton(
            StringManager::GetPCSTRById(SINAWEIBO),
            SBI_UnbindWithSinaWeibo,
            StringManager::GetPCSTRById(UNBIND), &m_bindSizer);
    rightSizer->AddSpacer(m_itemHorzSpacing);
    rightSizer->Add(
            CreateButton(
                SBI_SwitchWithSinaWeibo,
                StringManager::GetPCSTRById(SWITCH),
                true),
            UISizerFlags().Center());
    m_userNameSizer = AddItemTips(WeiboFactory::GetSinaWeibo()->GetUserName().c_str());

    AddPushButton(
            StringManager::GetPCSTRById(SINAWEIBO),
            SBI_BindWithSinaWeibo,
            StringManager::GetPCSTRById(BIND), &m_unbindSizer);
    UpdateData();
}

void UIShareAccountSettingPanel::UpdateData()
{
    if (WeiboFactory::GetSinaWeibo()->GetAccessToken().empty())
    {
        m_bindSizer->Show(false);
        m_userNameSizer->Show(false);
        m_unbindSizer->Show(true);
    }
    else
    {
        m_bindSizer->Show(true);
        m_userNameSizer->Show(true);
        m_unbindSizer->Show(false);
    }
}
