#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREPERSONCENTEREDPAGE_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREPERSONCENTEREDPAGE_H_

#include "BookStoreUI/UIBookStoreTablePage.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "GUI/UITextBox.h"

class UIBookStoreListBoxWithBtn;

class UIBookStorePersonCenteredPage : public UIBookStoreTablePage
{
public:
    UIBookStorePersonCenteredPage();
    ~UIBookStorePersonCenteredPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStorePersonCenteredPage";
    }

    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual void OnEnter();
	virtual void OnLeave();
    virtual dk::bookstore::FetchDataResult FetchInfo();
    virtual void SetTableController();
	virtual bool OnBookItemLongTapped(const EventArgs& args);
	virtual bool OnHideBookFinished(const EventArgs& args);
	virtual UISizer* CreateNavigationRightSizer();
    virtual void AdjustTitlePos();
#ifdef KINDLE_FOR_TOUCH
    virtual bool OnHookTouch(MoveEvent* moveEvent);
#else
	virtual BOOL OnKeyPress(INT32 iKeyCode);
#endif

private:
	void Init();
	void SearchPersonalBook();
	void SwitchSearchMode(bool reloadInfo = true);
	void SetSearchMode(bool isSearchMode, bool showBack = true);
    void DownloadAll();
    void StopAllDownloads();
    int  GetWorkingAndWaitingDownloads();

    bool OnDownloadStateUpdate(const EventArgs& args);
    bool OnPanelVisible(const EventArgs& args);

    bool UpdateDownloadTasks();

private:
	bool m_activated;
    UIBookStoreListBoxWithBtn* m_pAllBooksPanel;
    UIBookStoreListBoxWithBtn* m_pNotDownloadPanel;
	dk::bookstore::model::BookInfoSPtr m_currentBookInfo;
	bool m_searchMode;

#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnDownload;
    UITouchComplexButton m_btnSearch;
    UIButtonGroup m_topRightBtns;
#else
    UIComplexButton m_btnDownload;
	UIComplexButton m_btnSearch;
#endif
	UITextBox m_searchBox;
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREPERSONCENTEREDPAGE_H_
