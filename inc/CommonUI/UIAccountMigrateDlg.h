
#ifndef __UIACCOUNTMIGRATIONDLG_H__
#define __UIACCOUNTMIGRATIONDLG_H__

#include "GUI/UIDialog.h"
#include "GUI/UIText.h"
#include "GUI/UITextSingleLine.h"
#include "PersonalUI/UISingleLine.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#include "GUI/UIBackButton.h"
#endif

class UIAccountMigrateDlg : public UIDialog
{
public:
    static const char* EventMigrateLater;
    static const char* EventLoginXiaomiAccount;

    enum PopupMode
    {
        NOTICE_NORMAL = 0,
        NOTICE_FORCE,
        NOTICE_DUOKAN_LOGIN_CLOSED,
    };

public:
    UIAccountMigrateDlg(UIContainer* pParent, PopupMode mode);
    ~UIAccountMigrateDlg();
	virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
	virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

    bool OnHookTouch(MoveEvent* moveEvent);
private:
    bool InitUI();
	void OnMigrationBtnClick();
    void OnMigrateLaterBtnClick();
    void OnLoginXiaomiAccountClick();

    CString GetOpitonTextByMode(PopupMode popupMode);
    CString GetTipsByMode(PopupMode popupMode);

private:
    PopupMode m_popupMode;
    UIText m_migrationTip;
	UITextSingleLine m_title;
	UISingleLine m_splitLine1;
	UISingleLine m_splitLine2;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnMigration;
	UITouchComplexButton m_btnOtherOption;
#else
    UIComplexButton m_btnMigration;
	UIComplexButton m_btnOtherOption;
#endif
    
};
#endif//__UIACCOUNTMIGRATIONDLG_H__