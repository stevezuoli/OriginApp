#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTORESEARCHPAGE_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTORESEARCHPAGE_H_

#include "GUI/UIPage.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStoreUI/UIBookStoreFetchInfo.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "GUI/UITextBox.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "CommonUI/UITitleBar.h"
#endif

/********************************************
* UIBookStoreSearchPage
********************************************/
class UIBookStoreSearchPage : public UIPage
{
public:
    UIBookStoreSearchPage(const std::string& searchKey);
    ~UIBookStoreSearchPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreSearchPage";
    }

    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void OnClick();

    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

    virtual dk::bookstore::FetchDataResult FetchInfo();
    virtual void SetFetchStatus(FETCH_STATUS status);
    virtual bool OnMessageListBoxUpdate(const EventArgs& args);
private:
    void Init();

protected:
    std::string m_searchKey;
    FETCH_STATUS m_fetchStatus;
    UITextBox m_searchBox;
    UIText m_nonSearchResult;
    UIBookStoreFetchInfo m_fetchInfo;
    UIBookStoreListBoxWithBtn m_listBoxPanel;
#ifdef KINDLE_FOR_TOUCH
    UITouchBackButton m_backButton;
#else
    UITitleBar m_titleBar;
#endif
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTORESEARCHPAGE_H_

