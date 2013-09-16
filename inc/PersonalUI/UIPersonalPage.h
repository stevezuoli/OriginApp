#ifndef __PERSONALUI_UIPERSONALPAGE_H__
#define __PERSONALUI_UIPERSONALPAGE_H__

#include "GUI/UIPage.h"
#include "GUI/UITextBox.h"
#include "PersonalUI/UISingleLine.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "BookStoreUI/UIBookStoreIndexPageItem.h"

#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#endif

class UIPersonalPage : public UIPage
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStorePresonalPage";
    }

    static const int s_itemCount = 7;
    static const int s_itemCol = 2;

    UIPersonalPage();
    ~UIPersonalPage();

    virtual void MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height);
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual void OnEnter();
    virtual void OnLeave();
    virtual void OnLoad();
    virtual void OnUnLoad();

private:
    void InitUI();
    void OnRegisterClick();
    void OnLoginClick();
    void UpdateData();
	bool LoginWithTip();
    void OnPersonalBookClick();
    void OnCommentSquareClick();
    void OnPersonalExperienceClick();
    void OnPersonalMessageClick();
    void OnPersonalKeyClick();
    void OnPersonalNotesClick();
    void OnPersonalFavouriteClick();
    void OnCloudShelfClick();

    // Account Manager
    bool LogDataChanged(const EventArgs& args);
    bool OnXiaomiUserCardReceived(const EventArgs& args);

    bool OnMessageUnreadCountChanged(const EventArgs& args);
    UIBookStoreIndexPageItem* GetItemByID(DWORD id);
    void UpdateMessagesUnreadCount();
    void StartGetMessages();
    void StopGetMessages();
private:
    UITextSingleLine m_loginTip;
    UITextSingleLine m_loginName;
    UITextSingleLine m_loginEmail;

    UISingleLine m_buttonsplitLine1;
    UISingleLine m_buttonsplitLine2;
    UISingleLine m_buttonsplitLine3;

#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnRegister;
    UITouchComplexButton m_btnLogin;
    UITouchComplexButton m_btnLogout;
#else
    UIComplexButton m_btnRegister;
    UIComplexButton m_btnLogin;
    UITitleBar m_titleBar;
#endif

    UIBookStoreIndexPageItem m_itemList[s_itemCount];
    UISizer* m_loginSizer;
    UISizer* m_logoutSizer;
};
#endif

