///////////////////////////////////////////////////////////////////////
// UIAccountLoginCloudDialog.h
// Contact: juguanhui
// Copyright (c) Duokan Corporation. All rights reserved.
//
///////////////////////////////////////////////////////////////////////

#ifndef __COMMONUI_UIACCOUNTLOGINCLOUDDIALOG_H__
#define __COMMONUI_UIACCOUNTLOGINCLOUDDIALOG_H__

#include "GUI/UIModalDialog.h"
#include "GUI/UIButtonGroup.h"
#include "Utility/ImageManager.h"
#include "GUI/UIText.h"
#include "Utility/ColorManager.h"

//using namespace DkFormat;

class UIAccountLoginCloudDialog : public UIModalDlg
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIAccountLoginCloudDialog";
    }

    UIAccountLoginCloudDialog(UIContainer* pParent);
    virtual ~UIAccountLoginCloudDialog();

    virtual bool OnChildClick(UIWindow* child);
    bool OnHookTouch(MoveEvent* moveEvent);

private:
    void Init();
    bool OnLoginClick();
    bool OnMigrateClick();

    UITouchComplexButton    m_btnLogin;
    UITouchComplexButton    m_btnMigrate;
    UIText m_info;
};

#endif //__COMMONUI_UIACCOUNTLOGINCLOUDDIALOG_H__
