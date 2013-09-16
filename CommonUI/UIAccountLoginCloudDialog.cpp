///////////////////////////////////////////////////////////////////////
// UIAccountLoginCloudDialog.cpp
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "CommonUI/UIAccountLoginCloudDialog.h"
#include "GUI/CTpGraphics.h"
#include "I18n/StringManager.h"
#include "Common/WindowsMetrics.h"
#include "Framework/CDisplay.h"
#include "interface.h"
#include "GUI/UISizer.h"
#include "drivers/DeviceInfo.h"
#include "drivers/MoveEvent.h"
#include <tr1/functional>
#include "CommonUI/UIUtility.h"
#include "Thirdparties/MiMigrationManager.h"
#include "Common/CAccountManager.h"

using namespace dk::utility;
using namespace WindowsMetrics;
using namespace dk::thirdparties;
using namespace dk::account;

UIAccountLoginCloudDialog::UIAccountLoginCloudDialog(UIContainer* pParent)
    : UIModalDlg(pParent, StringManager::GetPCSTRById(CLOUD_LOGININFO_TITLE))
{
    ShowCloseIcon(true);
    HookTouch();
    Init();
}

UIAccountLoginCloudDialog::~UIAccountLoginCloudDialog ()
{
}

void UIAccountLoginCloudDialog::Init()
{
    if (m_windowSizer)
    {
        const int horizonalMargin = GetWindowMetrics(UIModalDialogHorizonMarginIndex);
        const int infoFontSize = GetWindowFontSize(FontSize18Index);
        const int width = GetWindowMetrics(UIModalDialogWidthIndex);
        const int minHeight = GetWindowMetrics(UIPixelValue55Index);
        m_info.SetMaxWidth(width - (horizonalMargin << 1));
        m_info.SetText(StringManager::GetPCSTRById(CLOUD_LOGININFO));
        m_info.SetFontSize(infoFontSize);
        m_info.SetMinHeight(minHeight);
        AppendChild(&m_info);

        const int btnFontSize = GetWindowFontSize(FontSize18Index);
        m_btnLogin.Initialize(IDSTATIC, StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_LOGIN_XIAOMI_ACCOUNT), btnFontSize);
        m_btnMigrate.Initialize(IDSTATIC, StringManager::GetPCSTRById(ACCOUNT_XIAOMI_MIGRATE), btnFontSize);
        m_btnLogin.SetMinHeight(minHeight);
        m_btnMigrate.SetMinHeight(minHeight);
        m_btnGroup.SetBtnsDirection(UIButtonGroup::BGD_Vertical);
        m_btnGroup.AddButton(&m_btnLogin);
        m_btnGroup.AddButton(&m_btnMigrate);
        m_btnGroup.SetMinHeight(dkDefaultCoord);

        m_windowSizer->Add(&m_info, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, horizonalMargin).Border(dkBOTTOM, GetWindowMetrics(UIPixelValue10Index)));
        m_windowSizer->Add(&m_btnGroup, UISizerFlags().Expand());
        const int height = m_windowSizer->GetMinSize().GetHeight();
        MakeCenter(width, height);
        Layout();
    }
}

bool UIAccountLoginCloudDialog::OnChildClick(UIWindow* child)
{
    if (child == &m_btnLogin)
    {
        return OnLoginClick();
    }
    else if (child == &m_btnMigrate)
    {
        return OnMigrateClick();
    }
    return false;
}

bool UIAccountLoginCloudDialog::OnLoginClick()
{
    EndDialog(IDCANCEL); 
    CAccountManager* pAccountMgr = CAccountManager::GetInstance();
    if (pAccountMgr)
    {
        pAccountMgr->LoginXiaomiAccount(false);
    }
    return true;
}

bool UIAccountLoginCloudDialog::OnMigrateClick()
{
    CDisplay::CacheDisabler cache;
    EndDialog(0);
    MiMigrationManager* pMigrationMgr = MiMigrationManager::GetInstance();
    if (pMigrationMgr)
    {
        pMigrationMgr->FireMigrationEvent();
    }
    return true;
}

bool UIAccountLoginCloudDialog::OnHookTouch(MoveEvent* moveEvent)
{
    if (moveEvent->GetActionMasked() != MoveEvent::ACTION_DOWN)
    {
        return false;
    }
    if (!PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
    {
        EndDialog(IDCANCEL);
        return true;
    }
    return false;
}
