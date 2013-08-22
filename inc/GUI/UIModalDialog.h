////////////////////////////////////////////////////////////////////////
// UIModalDialog.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIMODALDIALOG_H__
#define __UIMODALDIALOG_H__

#include "GUI/UISizer.h"
#include "GUI/UIDialog.h"
#include "GUI/UIButtonGroup.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITouchComplexButton.h"

class UISizer;
class UIModalDlg : public UIDialog
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIModalDlg";
    }

    UIModalDlg(UIContainer* pParent, LPCSTR szTitle);
    ~UIModalDlg();

    void ShowCloseIcon(bool isVisible);

protected:
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

protected:
    UITextSingleLine m_txtTitle;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnCancelIcon;
#endif
    UIButtonGroup m_btnGroup;
    UISizer* m_pTopSizer;
};
#endif // __UIMODALDIALOG_H__
