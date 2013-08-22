///////////////////////////////////////////////////////////////////////
// UIBookContentSearchDlg.h
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIBOOKCONTENTSEARCHDLG_H__
#define __UIBOOKCONTENTSEARCHDLG_H__

#include "Common/File_DK.h"
//#include "TouchAppUI/UIBookContentSearchBar.h"
#include "TouchAppUI/UIBookContentSearchListBox.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UIDialog.h"
#include "GUI/UIImage.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITextBox.h"
#include "DkSPtr.h"
#include "Bookmark.h"
#include "DKXManager/ReadingInfo/ICT_ReadingDataItem.h"
#include "CommonUI/UITitleBar.h"

#define MAX_SEARCH_KEYWORD_LENGTH      (128)
#define MAX_SEARCH_RESULT_COUNT        (1000)
#define SEARCH_COUNT_PER_TIME          (12)

class UISizer;

class UIBookContentSearchDlg : public UIDialog
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookContentSearchDlg";
    }

    UIBookContentSearchDlg(UIContainer* pParent, std::string strBookName, bool needWaitingInfo);
    virtual ~UIBookContentSearchDlg();

    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
	virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
	virtual void Popup();
	virtual void EndDialog(INT32 iEndCode,BOOL fRepaintIt = TRUE);

protected:
    virtual HRESULT DrawBackGround (DK_IMAGE drawingImg);
	void OnKeywordChange();
	void BackToRead();

private:
    enum SEARCH_MODE
    {
        SEARCH_FROM_START,
        SEARCH_NEXT
    };

private:
    // 初始化对话框
    // needWaitingInfo 是否需要提示等待，如PDF查找较慢，需要提示等待
    BOOL Initialize(bool needWaitingInfo);

    BOOL InitUI ();
    BOOL InitListBox (ICT_ReadingDataItem * bookmarkList[], INT iListSize, BOOL clearListBox = true);
    void ClearListBox ();
    void ClearSearchResult();
    void UpdateNavigationButton();
    HRESULT UpdateBookmarkList();
    BOOL HandleFormatRenderSearch (SEARCH_MODE mode = SEARCH_FROM_START);
    void HandleFormatRenderJump ();
    void HandlePageUpDown(BOOL fPageDown);
    void Finalize();
	void StartSearchKeyword();
    bool OnInputChange(const EventArgs& args);

private:
	UITouchBackButton m_btnBack;
	UITextSingleLine	 m_textBookName;

	//UIBookContentSearchBar m_BookContentSearchBar;
	UITextBox			m_editSearchBox;
	UITouchComplexButton m_btnSearch;
    UIBookContentSearchListBox m_lstChapterListBox;
    UITextSingleLine m_txtTotalTag;
    UITextSingleLine m_pageNum;
    UITextSingleLine m_txtMessage;

	std::string m_strBookName;
    INT32 m_iSearchResultCountAll;
    INT32 m_iBookmarkNum;
    INT32 m_iBookmarkNumPerPage;
    INT32 m_iSelectedIndex;
    INT32 m_iNextPageIndex;
    INT32 m_iCurPage;
    INT32 m_iTotalPage;
    BOOL m_fUIInitialized;
    CHAR m_chKeyword[MAX_SEARCH_KEYWORD_LENGTH];
    ICT_ReadingDataItem* m_searchResult[MAX_SEARCH_RESULT_COUNT];
	UINT m_HighLightenStartPos[MAX_SEARCH_RESULT_COUNT];
	UINT m_HighLightenEndPos[MAX_SEARCH_RESULT_COUNT];
    INT m_locationsInBook[MAX_SEARCH_RESULT_COUNT];

    bool m_needWaitingInfo;

    UISizer* m_topSizer;
    UISizer* m_searchSizer;
    UITitleBar m_titleBar;
};

#endif

