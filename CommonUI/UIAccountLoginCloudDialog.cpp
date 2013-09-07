///////////////////////////////////////////////////////////////////////
// UIAccountLoginCloudDialog.cpp
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "CommonUI/UIAccountLoginCloudDialog.h"
#include "GUI/CTpGraphics.h"
#include "I18n/StringManager.h"
#include "Common/CAccountManager.h"
#include "Common/WindowsMetrics.h"
#include "Framework/CDisplay.h"
#include "Framework/CMessageHandler.h"
#include "Framework/CNativeThread.h"
#include "interface.h"
#include "GUI/UISizer.h"
#include "Utility/StringUtil.h"
#include "drivers/DeviceInfo.h"
#include <tr1/functional>
#include "CommonUI/UIIMEManager.h"
#include "CommonUI/UIUtility.h"
#include "BookStore/BookStoreErrno.h"

using namespace dk::account;
using namespace dk::utility;
using namespace WindowsMetrics;
using namespace dk::bookstore;

UIAccountLoginCloudDialog::UIAccountLoginCloudDialog(UIContainer* pParent)
    : UIModalDlg(pParent, StringManager::GetPCSTRById(CLOUD_LOGININFO_TITLE))
{
}

UIAccountLoginCloudDialog::~UIAccountLoginCloudDialog ()
{
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
    return false;
}

bool UIAccountLoginCloudDialog::OnMigrateClick()
{
    return false;
}

