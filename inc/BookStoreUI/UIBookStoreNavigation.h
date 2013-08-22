#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTORENAVIGATION_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTORENAVIGATION_H_

#include "BookStore/BookStoreInfoManager.h"
#include "BookStoreUI/UIBookStoreFetchInfo.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UIPage.h"
#include "GUI/UITouchComplexButton.h"
#else
#include "BookStoreUI/UIBookStorePage.h"
#include "CommonUI/UITitleBar.h"
#include "GUI/UIBackButton.h"
#include "GUI/UIButtonGroup.h"
#endif
#include "GUI/UISizer.h"

/********************************************
* UIBookStoreNavigation
********************************************/
#ifdef KINDLE_FOR_TOUCH
class UIBookStoreNavigation : public UIPage
#else
class UIBookStoreNavigation : public UIBookStorePage
#endif
{
public:
    UIBookStoreNavigation(const std::string& title,
                          bool hasBottomBar = true,
                          bool showFetchInfo = true,
                          bool showRefresh = true);
    virtual ~UIBookStoreNavigation();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreNavigation";
    }

    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual BOOL OnKeyPress(INT32 iKeyCode);

    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

    virtual dk::bookstore::FetchDataResult FetchInfo();
    virtual void SetFetchStatus(FETCH_STATUS status);
    virtual UISizer* CreateTopSizer();
    virtual UISizer* CreateMidSizer();
    virtual UISizer* CreateNavigationLeftSizer();
    virtual UISizer* CreateNavigationRightSizer();
    virtual UISizerFlags GetMiddleSizerFlags(){ return UISizerFlags(1).Expand().ReserveSpaceEvenIfHidden(); }
    virtual UIButtonGroup* GetCustomizedBottomBar() { return 0; }
    virtual void OnEnter();
    virtual void AdjustTitlePos();
    void ShowFetchInfo(bool showFetchInfo);

    void SetTitle(const std::string& title);
private:
    void Init();
    void BackToEntryPage();

protected:
    UITextSingleLine m_title;
    FETCH_STATUS m_fetchStatus;
    UISizer* m_pMidSizer;
    UISizer* m_pTopSizer;
    UISizer* m_pNavigationLeftSizer;
    UISizer* m_pNavigationRightSizer;
    UIBookStoreFetchInfo m_fetchInfo;
    bool    m_bShowFetchInfo;
    bool    m_bShowRefresh;

#ifdef KINDLE_FOR_TOUCH
    UITouchBackButton m_backButton;
    UITouchComplexButton m_backToEntryPageButton;
    UITouchComplexButton m_btnRefresh;
    UIButtonGroup m_topLeftBtns;
#else
    UIBackButton m_backButton;
    UITitleBar m_titleBar;
    UIComplexButton m_btnRefresh;
#endif

};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTORENAVIGATION_H_

