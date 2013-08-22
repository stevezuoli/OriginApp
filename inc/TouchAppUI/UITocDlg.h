#ifndef UITOCDLG_H
#define UITOCDLG_H

#include <string>
#include <vector>

#include "GUI/UIDialog.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITouchComplexButton.h"

#include "TouchAppUI/UIBookmarkListBox.h"
#include "TouchAppUI/UIBasicListBox.h"

#include "BookReader/IBookReader.h"

class UIReaderSettingDlg;
class DKXBlock;

class UITocDlg : public UIDialog 
{

public:
	virtual LPCSTR GetClassName() const
	{
		return "UITocDlg";
	}

	enum EndDialogFlag {
        INVALID,
		GOTOTOC,
		GOTOBOOKMARK,
		Nothing
	};

    enum SHOWDIALOGFLAG{
        SHOWBOOKCOMMENTANDDIGEST,
        SHOWTOC,
        SHOWBOOKMARK,
        SHOWERROR
    };
    
	UITocDlg(UIContainer* parent,
            int bookID,
            DKXBlock* dkxBlock,
            IBookReader* bookReader,
            const std::string& strBookName);
	~UITocDlg();


    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
	virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
	virtual void Popup();
	virtual void EndDialog(INT32 iEndCode,BOOL fRepaintIt = TRUE);
    void SetShowDialog(int showFlag);
    void SetFamiliarToTraditional(bool familiarToTraditional){m_familiarToTraditional = familiarToTraditional;}
	DK_TOCINFO* GetCurTocInfo();
	void UpdateCurChapter();
	bool IsTocInfoReady() const { return m_bSearchTocFinished; }
	std::string GetCurChapterTitle();
    int GetEndFlag() const { return m_endFlag; }

protected:
	virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

private:
	void InitUI();
	void InitAttr();
	void UpdateTocList();
    void UpdateBookMarkList();
    void UpdateCommentList();
    bool OnListTurnPage(const EventArgs& args);
	bool OnListItemDelete(const EventArgs& args);
    void UpdatePageInfo();
	void SetVecVisible(vector<UIWindow*> *wins, bool visible);
	void GetTocInfo();
	HRESULT FlatTOC(DK_TOCINFO* _pRootToc, int _iDepth, vector<DK_TOCINFO*>& _vTocFlatList);
    void UpdateFocusedList(UICompoundListBox* pFocusedList);

    // exporting handlers
    void ExportToEvernote();
    bool ImplementExportToEvernote();
    bool OnEvernoteAuthFinished(const EventArgs& args);
    bool OnEvernoteGetUserFinished(const EventArgs& args);
    bool OnEvernoteExportFinished(const EventArgs& args);

    size_t GetVisibleTocCount() const;
    void OnTocExpand(int tocIndexInPage);

private:
	int m_iBookID;
	IBookReader* m_pBookReader;

	//UI
	UITouchComplexButton m_btnShowBookToc;
    UITouchComplexButton m_btnShowBookMark;
    UITouchComplexButton m_btnShowBookComment;

	UIBasicListBox		 m_lstBookToc;
    UIBookmarkListBox	 m_lstBookMark;
    UIBookmarkListBox	 m_lstBookComment;

	UITouchBackButton m_btnBack;

	UITextSingleLine	 m_textBookName;
	UITextSingleLine	 m_textTotalElementsNumber;
	UITextSingleLine	 m_pageInfo;

	UITextSingleLine	 m_textGetTocFailure;
	UITextSingleLine	 m_textAnalyzingToc;
	UITouchComplexButton m_btnContinueReading;
    UITouchComplexButton m_btnExportNotes;

    UITextSingleLine	 m_textGetBookMarkFailure;
	UIText				 m_textAddBookMarkGuide;
    UIImage				 m_imgAddBookMark;

    UITextSingleLine	 m_textGetBookCommentFailure;
    UIText				 m_textAddBookCommentGuide;

	//attributes
	vector<DK_TOCINFO*>* m_pvAllTOC;//initialized when fist showing, destroyed int desctructor self
	
	std::string m_strBookName;
	DK_TOCINFO* m_pCurTOC;
	unsigned int m_iChaptersPerPage;

    vector<UIWindow*> m_vNormalWins;

    SHOWDIALOGFLAG m_eShowFlag;

	bool m_bSearchTocFinished;
    int m_endFlag;
    bool m_familiarToTraditional;

    UITitleBar m_titleBar;
	UISizer* m_topSizer;
    UISizer* m_bookTocFailureSizer;
    UISizer* m_bookMarkFailureSizer;
    UISizer* m_bookCommentFailureSizer;

    // flag for exporting to evernote
    bool isWaitingToExport;
};//UITocDlg
#endif//UITOCDLG_H
