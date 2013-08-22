////////////////////////////////////////////////////////////////////////
//
// UIHomePage.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIHOMEPAGE_H__
#define __UIHOMEPAGE_H__

#include "GUI/UIImage.h"
#include "GUI/UIButton.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UIPage.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UISeperator.h"
#include "GUI/UIButtonGroup.h"
#include "CommonUI/CPageNavigator.h"
#include "TouchAppUI/UIDirectoryListBox.h"
#include "TouchAppUI/UIComplexDialog.h"
#include "TouchAppUI/UIButtonDlg.h"
#include "../Common/FileSorts_DK.h"
#include <string>

class UISizerItem;

typedef enum PageStyle
{
    NORMALPAGE,
    SEARCHPAGE
}HomePageStyle;

class UIHomePage : public UIPage
{
public:
    UIHomePage(HomePageStyle Style);

    virtual LPCSTR GetClassName() const
    {
        return ("UIHomePage");
    }
    
    virtual ~UIHomePage();
    
    virtual void MoveWindow(INT32 _iLeft, INT32 _iTop, INT32 _iWidth, INT32 _iHeight);
    
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    
    //CString  GetHomePageSelectBookName();

    virtual HRESULT Draw(DK_IMAGE drawingImg);
    
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual void Show(BOOL bIsShow, BOOL bIsRepaint = TRUE);

    virtual void OnLoad();
    virtual void OnUnLoad();
    virtual void OnEnter();
    virtual void OnLeave();
    void SetSearchKeyword(const std::string& keyword);
protected:
    HRESULT Init();
    void Dispose();
    virtual bool OnChildClick(UIWindow* child);

private:
    bool OnFileListChanged(const EventArgs& args);
    std::string GetDirectoryViewTitle(const std::string& path) const;
    CString GetSortString();
    void OnDispose(BOOL bIsDisposed);
    void UpdateNavigationButton();
    void UpdateTopBox();
    HRESULT UpdateBookList();
    void Finalize();

    void RefreshUI();
    bool OnListItemClick(const EventArgs& args);
    bool OnListContentChanged(const EventArgs& args);

    UITouchComplexButton m_btnSort;

    UIDirectoryListBox   m_lstDir;
    UITextSingleLine m_txtTotalBook;

    UITextSingleLine m_txtPageNo;

    UITouchBackButton m_btnUpperFolder;
    UITextSingleLine   m_txtUpperFolder;
    UITouchComplexButton m_btnSearch;
    UITouchComplexButton m_btnViewMode;
    UITouchComplexButton m_btnNewCategory;

    BOOL m_bIsDisposed;
    HomePageStyle m_pageStyle;
    DK_FileSorts  m_FileSorts;
    UIText m_txtHint;

    //上方的工具栏有两种形式，搜索框+排序按钮，返回按钮+标题
    UISizer* m_topButtonSizer;
    UISizer* m_topUpLevelSizer;
    BookListMode m_bookListMode;

    bool OnSortClick();
    bool OnUpperFolderClick();
    bool OnSearchClick();
    bool OnNewCategoryClick();
    bool OnViewModeClick();
    bool OnAddBookToCategoryClick();

    void UpdateViewModeImage();
    UIButtonGroup m_topLeftButtonGroup;
    UISizerItem* m_titleLeftSizer;
    UISizerItem* m_titleRightSizer;
    UISizer* m_titleSizer;
    UITextBox m_searchBox;
    std::string m_searchKeyword;
};

#endif

