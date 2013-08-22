/*
 * =====================================================================================
 *       Filename:  UIWeiboBindDlg.h
 *    Description:  新浪微博绑定页面
 *
 *        Version:  1.0
 *        Created:  12/27/2012 03:29:53 PM
 * =====================================================================================
 */

#ifndef UIWEIBOBINDDLG_H
#define UIWEIBOBINDDLG_H

#include "GUI/UIDialog.h"
#include "GUI/UITextBox.h"
#include "GUI/UIImage.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#include "GUI/UIBackButton.h"
#endif
#include "CommonUI/UITitleBar.h"

class UISizer;
class UIWeiboBindDlg : public UIDialog
{
public:
    UIWeiboBindDlg(UIContainer* pParent);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    bool OnWeiboLoginUpdate(const EventArgs& args);
    bool OnInputChanges(const EventArgs& args);
protected:
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

private:
    void LoginWeibo();
    void InitUI();

private:
    UITitleBar m_titleBar;
    UITextBox m_usernameBox;
    UITextBox m_passwordBox;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_loginButton;
    UITouchBackButton m_backButton;
#else
    UIBackButton m_backButton;
    UIComplexButton m_loginButton;
#endif
    UITextSingleLine m_titleBindLabel;
    UIImage m_logoDuoKan;
    UIImage m_logoSina;
    UIImage m_imgConnectd;
    UISizer* m_topSizer;
};//UIWeiboBindDlg
#endif//UIWEIBOBINDDLG_H

