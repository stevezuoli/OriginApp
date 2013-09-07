#include <tr1/functional>
#include "CommandID.h"
#include "Utility.h"
#include "Common/FileManager_DK.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "TouchAppUI/UITocDlg.h"
#include "TouchAppUI/UIReaderSettingDlg.h"
#include "Framework/CDisplay.h"
#include "Framework/CHourglass.h"
#include "I18n/StringManager.h"
#include "Common/WindowsMetrics.h"
#include "Utility/StringUtil.h"
#include "Utility/ReaderUtil.h"
#include "Thirdparties/EvernoteManager.h"
#include "CommonUI/UIUtility.h"

using namespace dk::utility;
using namespace dk::thirdparties;
using namespace WindowsMetrics;

UITocDlg::UITocDlg(UIContainer* parent,
        int bookID,
        DKXBlock* dkxBlock,
        IBookReader* bookReader,
        const std::string& strBookName) :
    UIDialog(parent) ,
    m_iBookID(bookID) ,
    m_pBookReader(bookReader) ,
    m_lstBookMark(bookID, NULL, dkxBlock, bookReader, UIBookmarkListItem::UIBOOKMARKLISTTYPE_BOOKMARK) ,
    m_lstBookComment(bookID, NULL, dkxBlock, bookReader, UIBookmarkListItem::UIBOOKMARKLISTTYPE_DIGEST | UIBookmarkListItem::UIBOOKMARKLISTTYPE_COMMENT) ,
    m_btnBack(this, ID_BTN_TOUCH_TOC_BACK) ,
    m_pvAllTOC(NULL),
    m_strBookName(strBookName) ,
    m_pCurTOC(NULL),
    m_iChaptersPerPage(8) ,
    m_eShowFlag(SHOWERROR) ,
    m_bSearchTocFinished(false) ,
    m_endFlag(INVALID),
    m_familiarToTraditional(false),
    m_topSizer(NULL),
    m_bookTocFailureSizer(NULL),
    m_bookMarkFailureSizer(NULL),
    m_bookCommentFailureSizer(NULL),
    isWaitingToExport(false)
{
    SubscribeMessageEvent(
        EvernoteManager::EventEvernoteAuthFinished,
        *EvernoteManager::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&UITocDlg::OnEvernoteAuthFinished),
            this,
            std::tr1::placeholders::_1));
    SubscribeMessageEvent(
        EvernoteManager::EventEvernoteGetUserFinished,
        *EvernoteManager::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&UITocDlg::OnEvernoteGetUserFinished),
            this,
            std::tr1::placeholders::_1));
    SubscribeMessageEvent(
        EvernoteManager::EventEvernoteExportFinished,
        *EvernoteManager::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&UITocDlg::OnEvernoteExportFinished),
            this,
            std::tr1::placeholders::_1));

    InitAttr();
    InitUI();
}

UITocDlg::~UITocDlg()
{
    m_vNormalWins.clear();
	m_pCurTOC = NULL;
}

void UITocDlg::InitAttr()
{
    m_lstBookToc.SetIndentMode(true);
    if (NULL == m_pBookReader)
    {
        m_btnShowBookToc.SetEnable(false);
        m_btnShowBookComment.SetEnable(false);
    }
}

void UITocDlg::InitUI()
{
    const int fontsize20 = GetWindowFontSize(FontSize20Index);
    m_btnShowBookToc.Initialize(ID_BTN_TOUCH_TOC_SHOWBOOKTOC, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_TOC_BOOKTOC), fontsize20);
    m_btnShowBookMark.Initialize(ID_BTN_TOUCH_TOC_SHOWBOOKMARK, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_TOC_BOOKMARK), fontsize20);
    m_btnShowBookComment.Initialize(ID_BTN_TOUCH_TOC_SHOWBOOKCOMMENTANDDIGEST, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_TOC_COMMENTANDDIGEST), fontsize20);

    m_btnShowBookToc.SetCornerStyle(false, true, true, false);
    m_btnShowBookMark.SetCornerStyle(true, true, true, true);
    m_btnShowBookComment.SetCornerStyle(true, false, false, true);

    m_textBookName.SetText(m_strBookName.c_str());
    m_textBookName.SetAlign(ALIGN_CENTER);
    m_textBookName.SetFontSize(GetWindowFontSize(UITocDlgFileNameIndex));
    char str[50] = {0};
    snprintf(str, DK_DIM(str), "%s %d %s", StringManager::GetPCSTRById(BOOK_TOTAL), 0, StringManager::GetPCSTRById(BOOK_TIAO));
    m_textTotalElementsNumber.SetText(CString(str));
	m_textTotalElementsNumber.SetEnglishGothic();
    m_textTotalElementsNumber.SetAlign(ALIGN_LEFT);
    m_textTotalElementsNumber.SetFontSize(GetWindowFontSize(FontSize16Index));

	m_pageInfo.SetEnglishGothic();
    m_pageInfo.SetAlign(ALIGN_RIGHT);
    m_pageInfo.SetFontSize(GetWindowFontSize(FontSize16Index));

    m_textGetTocFailure.SetText(StringManager::GetPCSTRById(TOUCH_GET_TOC_FAILURE));
    m_textGetTocFailure.SetFontSize(GetWindowFontSize(UITocDlgFileNameIndex));
    m_textAnalyzingToc.SetText(StringManager::GetPCSTRById(TOC_CONTENT_ANALYZING));
    m_textAnalyzingToc.SetFontSize(GetWindowFontSize(UITocDlgFileNameIndex));

    m_btnContinueReading.Initialize(ID_BTN_TOUCH_TOC_BACK , StringManager::GetPCSTRById(CONTINUE_READ_BOOK), GetWindowFontSize(UITocDlgFileNameIndex));
    m_btnExportNotes.Initialize(ID_BTN_EXPORT_NOTE, StringManager::GetPCSTRById(EXPORT_TO_EVERNOTE), GetWindowFontSize(FontSize18Index));
    m_btnExportNotes.SetEnable(m_lstBookComment.GetCurBookmarkNum() > 0);

    m_textGetBookMarkFailure.SetText(StringManager::GetPCSTRById(TOUCH_GET_BOOKMARK_FAILURE));
    m_textGetBookMarkFailure.SetFontSize(GetWindowFontSize(UITocDlgFileNameIndex));
    m_textGetBookCommentFailure.SetText(StringManager::GetPCSTRById(TOUCH_GET_COMMENT_FAILURE));
    m_textGetBookCommentFailure.SetFontSize(GetWindowFontSize(UITocDlgFileNameIndex));

    m_textAddBookMarkGuide.SetText(StringManager::GetPCSTRById(TOUCH_GET_ADD_BOOKMARK_GUIDE));
    m_textAddBookMarkGuide.SetLineSpace(1);
    m_textAddBookMarkGuide.SetFontSize(fontsize20);

    m_textAddBookCommentGuide.SetText(StringManager::GetPCSTRById(TOUCH_GET_ADD_COMMENT_GUIDE));
    m_textAddBookCommentGuide.SetLineSpace(1);
    m_textAddBookCommentGuide.SetFontSize(fontsize20);
    
    m_imgAddBookMark.SetImage(ImageManager::GetImage(IMAGE_TOUCH_ICON_BOOKMARK));

    m_lstBookToc.CustomizeItem(GetWindowFontSize(UIBasicListItemIndex), GetWindowMetrics(UIBasicListItemHeightIndex));

    AppendChild(&m_titleBar);
    AppendChild(&m_textBookName);
    AppendChild(&m_btnShowBookComment);
    AppendChild(&m_btnShowBookToc);
    AppendChild(&m_btnShowBookMark);
    AppendChild(&m_pageInfo);
    AppendChild(&m_textTotalElementsNumber);
    AppendChild(&m_textGetTocFailure);
    AppendChild(&m_textAnalyzingToc);
    AppendChild(&m_btnContinueReading);
    AppendChild(&m_btnExportNotes);
    AppendChild(&m_textGetBookMarkFailure);
    AppendChild(&m_textGetBookCommentFailure);
    AppendChild(&m_textAddBookMarkGuide);
    AppendChild(&m_textAddBookCommentGuide);
    AppendChild(&m_imgAddBookMark);
    AppendChild(&m_lstBookToc);
    AppendChild(&m_lstBookMark);
    AppendChild(&m_lstBookComment);

    m_vNormalWins.clear();

    m_vNormalWins.push_back(&m_titleBar);
    m_vNormalWins.push_back(&m_textBookName);
    m_vNormalWins.push_back(&m_btnBack);
    m_vNormalWins.push_back(&m_btnShowBookComment);
    m_vNormalWins.push_back(&m_btnShowBookToc);
    m_vNormalWins.push_back(&m_btnShowBookMark);
    m_vNormalWins.push_back(&m_pageInfo);
    m_vNormalWins.push_back(&m_textTotalElementsNumber);

    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);

        m_topSizer = new UIBoxSizer(dkHORIZONTAL);
        m_topSizer->Add(&m_btnBack, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        m_topSizer->Add(&m_textBookName, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        m_topSizer->Add(&m_btnExportNotes, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
		m_btnExportNotes.SetMinHeight(GetWindowMetrics(UITocDlgExportBtnHeightIndex));
        m_topSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));

        UISizer* pageInfoSizer = new UIBoxSizer(dkHORIZONTAL);
        pageInfoSizer->Add(&m_textTotalElementsNumber, 1);
        pageInfoSizer->Add(&m_pageInfo, 1);

        m_btnShowBookComment.SetMinHeight(GetWindowMetrics(UITocDlgBottomButtonHeightIndex));
        m_btnShowBookToc.SetMinHeight(GetWindowMetrics(UITocDlgBottomButtonHeightIndex));
        m_btnShowBookMark.SetMinHeight(GetWindowMetrics(UITocDlgBottomButtonHeightIndex));
        UISizer* bottomSizer = new UIBoxSizer(dkHORIZONTAL);
        bottomSizer->Add(&m_btnShowBookToc, 1);
        bottomSizer->Add(&m_btnShowBookMark, 1);
        bottomSizer->Add(&m_btnShowBookComment, 1);

        m_bookTocFailureSizer = new UIBoxSizer(dkVERTICAL);
        m_btnContinueReading.SetMinSize(dkSize(GetWindowMetrics(UITocDlgContinueReadButtonWidthIndex), GetWindowMetrics(UITocDlgBottomButtonHeightIndex)));
        m_bookTocFailureSizer->AddSpacer(GetWindowMetrics(UITocDlgTitleTopMarginIndex)<<1);
        m_bookTocFailureSizer->Add(&m_textGetTocFailure, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
        m_bookTocFailureSizer->Add(&m_textAnalyzingToc, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
        m_bookTocFailureSizer->AddSpacer(GetWindowMetrics(UITocDlgTitleTopMarginIndex));
        m_bookTocFailureSizer->Add(&m_btnContinueReading, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
        m_bookTocFailureSizer->AddStretchSpacer();

        m_bookMarkFailureSizer = new UIBoxSizer(dkVERTICAL);
        m_bookMarkFailureSizer->AddSpacer(GetWindowMetrics(UITocDlgTitleTopMarginIndex)<<1);
        m_bookMarkFailureSizer->Add(&m_textGetBookMarkFailure);
        m_bookMarkFailureSizer->AddSpacer(GetWindowMetrics(UITocDlgTitleTopMarginIndex));
        m_bookMarkFailureSizer->Add(&m_textAddBookMarkGuide, UISizerFlags().Expand());
        m_bookMarkFailureSizer->AddStretchSpacer();

        m_bookCommentFailureSizer = new UIBoxSizer(dkVERTICAL);
        m_bookCommentFailureSizer->AddSpacer(GetWindowMetrics(UITocDlgTitleTopMarginIndex)<<1);
        m_bookCommentFailureSizer->Add(&m_textGetBookCommentFailure);
        m_bookCommentFailureSizer->AddSpacer(GetWindowMetrics(UITocDlgTitleTopMarginIndex));
        m_bookCommentFailureSizer->Add(&m_textAddBookCommentGuide, UISizerFlags().Expand());
        m_bookCommentFailureSizer->AddStretchSpacer();

        mainSizer->Add(&m_titleBar);
        mainSizer->Add(m_topSizer, UISizerFlags().Expand());
        mainSizer->AddSpacer(2);
        mainSizer->Add(&m_lstBookToc, UISizerFlags(1).Expand());
        mainSizer->Add(&m_lstBookMark, UISizerFlags(1).Expand());
        mainSizer->Add(&m_lstBookComment, UISizerFlags(1).Expand());
        mainSizer->Add(m_bookTocFailureSizer, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UITocDlgHorizonMarginIndex)));
        mainSizer->Add(m_bookMarkFailureSizer, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UITocDlgHorizonMarginIndex)));
        mainSizer->Add(m_bookCommentFailureSizer, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UITocDlgHorizonMarginIndex)));
        
        mainSizer->Add(pageInfoSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UITocDlgHorizonMarginIndex)));
        mainSizer->Add(bottomSizer, UISizerFlags().Border(dkLEFT | dkRIGHT | dkBOTTOM,GetWindowMetrics(UIHorizonMarginIndex))
                                                  .Border(dkTOP, GetWindowMetrics(UIPixelValue15Index)).Expand());
        SetSizer(mainSizer);
    }

    m_lstBookToc.SetDisplayExpandButton(true);
    CONNECT(m_lstBookToc,UICompoundListBox::ListTurnPage, this, UITocDlg::OnListTurnPage)
    CONNECT(m_lstBookMark,UICompoundListBox::ListTurnPage, this, UITocDlg::OnListTurnPage)
    CONNECT(m_lstBookComment,UICompoundListBox::ListTurnPage, this, UITocDlg::OnListTurnPage)
    CONNECT(m_lstBookMark,UIBookmarkListBox::ListItemDelete, this, UITocDlg::OnListItemDelete)
	CONNECT(m_lstBookComment,UIBookmarkListBox::ListItemDelete, this, UITocDlg::OnListItemDelete)
}

void UITocDlg::UpdateFocusedList(UICompoundListBox* pFocusedList)
{
    if (pFocusedList)
    {
        if (pFocusedList == &m_lstBookToc)
        {
            UpdateTocList();
        }
        else if (pFocusedList == &m_lstBookMark)
        {
            UpdateBookMarkList();
        }
        else if  (pFocusedList == &m_lstBookComment)
        {
            UpdateCommentList();
        }
        m_btnExportNotes.SetVisible(pFocusedList == &m_lstBookComment);
        UpdatePageInfo();
        Layout();
        UpdateWindow();
    }
}

void UITocDlg::SetShowDialog(int showFlag) 
{
    if (showFlag < INVALID || showFlag > Nothing)
    {
        return ;
    }
    SHOWDIALOGFLAG eShowFlag = (SHOWDIALOGFLAG)showFlag;
    if (eShowFlag != m_eShowFlag)
    {
        m_eShowFlag = eShowFlag;
        m_btnShowBookComment.SetFocus((SHOWBOOKCOMMENTANDDIGEST == m_eShowFlag));
        m_btnShowBookToc.SetFocus( (SHOWTOC      == m_eShowFlag));
        m_btnShowBookMark.SetFocus((SHOWBOOKMARK == m_eShowFlag));
        m_bookTocFailureSizer->Show(SHOWTOC == m_eShowFlag);
        m_bookMarkFailureSizer->Show(SHOWBOOKMARK == m_eShowFlag);
        m_bookCommentFailureSizer->Show(SHOWBOOKCOMMENTANDDIGEST == m_eShowFlag);
        m_lstBookToc.SetVisible(SHOWTOC == m_eShowFlag);
        m_lstBookMark.SetVisible(SHOWBOOKMARK == m_eShowFlag);
        m_lstBookComment.SetVisible(SHOWBOOKCOMMENTANDDIGEST == m_eShowFlag);
        m_imgAddBookMark.SetVisible(SHOWBOOKMARK == m_eShowFlag);
        UICompoundListBox* pFocusedList = NULL;
        switch(m_eShowFlag)
        {
        case SHOWTOC:
            pFocusedList = &m_lstBookToc;
            break;
        case SHOWBOOKMARK:
            pFocusedList = &m_lstBookMark;
            break;
        case SHOWBOOKCOMMENTANDDIGEST:
            pFocusedList = &m_lstBookComment;
            break;
        default:
            break;
        }
        UpdateFocusedList(pFocusedList);
    }
    if (SHOWTOC == m_eShowFlag)
    {
        //search toc for the first time
        if (!m_bSearchTocFinished)
        {
            CHourglass::GetInstance()->Start();
            GetTocInfo();
            //after got toc over
            UpdateCurChapter();
            if (m_pvAllTOC && !m_pvAllTOC->empty())
            {
                int visibleTocCount = GetVisibleTocCount();
                int totalPage = (visibleTocCount + m_iChaptersPerPage - 1) / m_iChaptersPerPage;
                m_lstBookToc.SetTotalPageCount(totalPage);
            }
            CHourglass::GetInstance()->Stop();
        }
        UpdateFocusedList(&m_lstBookToc);
    }   
}

void UITocDlg::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
    UIDialog::MoveWindow(iLeft, iTop, iWidth, iHeight);
    SetSize(dkSize(iWidth,iHeight));
    m_textAddBookMarkGuide.SetMinWidth(iWidth - (GetWindowMetrics(UIHorizonMarginIndex)<<1));
    m_textAddBookCommentGuide.SetMinWidth(iWidth - (GetWindowMetrics(UIHorizonMarginIndex)<<1));

    //wins while bookmark is empty
    int top = GetWindowMetrics(UITocDlgAddBookMarkImageTOPIndex);

    m_imgAddBookMark.MoveWindow(GetWindowMetrics(UITocDlgAddBookMarkImageXIndex), top, 
        ImageManager::GetImage(IMAGE_TOUCH_ICON_BOOKMARK).Get()->GetWidth(), ImageManager::GetImage(IMAGE_TOUCH_ICON_BOOKMARK).Get()->GetHeight());

}

void UITocDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId) {
    case ID_BTN_TOUCH_TOC_GOTOTOC:
        {
            int selectedIndex = m_lstBookToc.GetSelectedItem() + m_lstBookToc.GetCurPageIndex()*m_iChaptersPerPage;
            if (selectedIndex >= 0 && selectedIndex < (int)GetVisibleTocCount() && m_pvAllTOC && !m_pvAllTOC->empty())
			{
                DK_TOCINFO* curToc = DK_TOCINFO::GetVisibleItemByIndex(m_pvAllTOC, selectedIndex);
                if (NULL == curToc)
                {
                    return;
                }
	            m_pCurTOC = curToc;
	            m_endFlag = GOTOTOC;
	            EndDialog(0);
	            
            }
 			break;
        }
    case ID_BTN_TOUCH_TOC_GOTOBOOKMARK:
        m_endFlag = GOTOBOOKMARK;
        EndDialog(0);
    case ID_BTN_TOUCH_TOC_SHOWBOOKCOMMENTANDDIGEST:
        SetShowDialog(SHOWBOOKCOMMENTANDDIGEST);
        break;
    case ID_BTN_TOUCH_TOC_SHOWBOOKTOC:
        SetShowDialog(SHOWTOC);
        break;
    case ID_BTN_TOUCH_TOC_SHOWBOOKMARK:
        SetShowDialog(SHOWBOOKMARK);
        break;
    case ID_BTN_TOUCH_TOC_BACK:
        EndDialog(0);
        break;
    case ID_BTN_TOUCH_TOC_EXPAND:
        OnTocExpand(dwParam);
        break;
    case ID_BTN_EXPORT_NOTE:
        ExportToEvernote();
        break;
    default:
        break;
    }
}

void UITocDlg::OnTocExpand(int tocIndexInPage)
{
    if (NULL == m_pvAllTOC)
    {
        return;
    }
    int index = m_lstBookToc.GetCurPageIndex() * m_iChaptersPerPage + tocIndexInPage;
    DK_TOCINFO* curToc = DK_TOCINFO::GetVisibleItemByIndex(m_pvAllTOC, index);
    if (NULL == curToc)
    {
        return;
    }
    curToc->ShiftExpandStatus();
    UpdateFocusedList(&m_lstBookToc);
}

void UITocDlg::Popup()
{
    m_endFlag = INVALID;
    for (size_t i=0; i<m_vNormalWins.size(); ++i)
        m_vNormalWins[i]->SetVisible(true);
    m_lstBookComment.UpdateList();
    m_lstBookMark.UpdateList();

    UpdateCurChapter();
    UpdateTocList();
    Layout();

    UIDialog::Popup();

}

void UITocDlg::EndDialog(INT32 iEndCode,BOOL fRepaintIt)
{
    m_eShowFlag = SHOWERROR; 
    UIDialog::EndDialog(iEndCode, fRepaintIt);
    for (size_t i=0; i< GetChildrenCount(); ++i)
    {
        GetChildByIndex(i)->SetVisible(FALSE);
    }
}

void UITocDlg::UpdateTocList()
{
    if (SHOWTOC != m_eShowFlag || !m_pBookReader)
    {
        return;
    }

    //hide/show the relational windows according to the chapters
    bool hasChapter = (m_pvAllTOC && !m_pvAllTOC->empty());
    m_bookTocFailureSizer->Show(!hasChapter);
    m_textGetTocFailure.SetVisible(!hasChapter && m_bSearchTocFinished);
    m_textAnalyzingToc.SetVisible(!hasChapter && !m_bSearchTocFinished);
    m_btnContinueReading.SetVisible(!hasChapter && m_bSearchTocFinished);
    m_lstBookToc.SetVisible(hasChapter);

    if (hasChapter)
    {
        m_lstBookToc.ClearItems();
        m_lstBookToc.SetFamiliarToTraditional(m_familiarToTraditional);
        int iCurStartIndex = m_lstBookToc.GetCurPageIndex() * m_iChaptersPerPage; 
        int itemsToDisplay = ((iCurStartIndex + m_iChaptersPerPage) < GetVisibleTocCount()) ? m_iChaptersPerPage : (GetVisibleTocCount() - iCurStartIndex);
        int iCurEndIndex = iCurStartIndex + itemsToDisplay;
        int visibleItemIndex = -1;
        int itemIndexInCurPage = -1;
        for (size_t i = 0; i < m_pvAllTOC->size(); ++i)
        {
            DK_TOCINFO* curToc = m_pvAllTOC->at(i);
            if (curToc->IsVisible())
            {
                ++visibleItemIndex;
            }
            else
            {
                //assert(false);
            }
            if (iCurStartIndex <= visibleItemIndex && 
                    visibleItemIndex < iCurEndIndex && curToc->IsVisible())
            {
                string strChapterTitle;
                char digit[20] = {0};
                snprintf(digit, DK_DIM(digit), "%d", curToc->GetDepth());
                strChapterTitle += digit;
                strChapterTitle += StringUtil::ReplaceString(curToc->GetChapter(), '\n', " ");
                m_lstBookToc.AddTocItem(strChapterTitle.c_str(), curToc->IsValid(), curToc->GetExpandStatus(), m_pBookReader->GetPageIndex(curToc->GetBeginRefPos()));
                ++itemIndexInCurPage;
                if (curToc == m_pCurTOC)
                {
                    m_lstBookToc.SetMarkedItemIndex(itemIndexInCurPage);
                    m_lstBookToc.SetItemFocus(itemIndexInCurPage, true);
                }
            }
            if (visibleItemIndex >= iCurEndIndex)
            {
                break;
            }
        }
    }

    Layout();
}

void UITocDlg::UpdatePageInfo()
{
    CHAR str[50] = {0};
    int curPage = 0;
    int totalPage = 0;
    int totalElements = 0;
    if (SHOWTOC == m_eShowFlag) 
    {
        curPage = m_lstBookToc.GetCurPageIndex() + 1;
        int visibleTocCount = GetVisibleTocCount();
        totalPage = (visibleTocCount + m_iChaptersPerPage - 1) / m_iChaptersPerPage;
        m_lstBookToc.SetTotalPageCount(totalPage);
        totalElements = visibleTocCount;
    } 
    else if (SHOWBOOKMARK == m_eShowFlag)
    {
        totalPage = m_lstBookMark.GetTotalPage();
        totalPage = totalPage<1 ? 1 : totalPage;
        curPage = m_lstBookMark.GetCurPageNum();
        totalElements = m_lstBookMark.GetCurBookmarkNum();
    }
    else if (SHOWBOOKCOMMENTANDDIGEST == m_eShowFlag)
    {
        totalPage = m_lstBookComment.GetTotalPage();
        totalPage = totalPage<1 ? 1 : totalPage;
        curPage = m_lstBookComment.GetCurPageNum();
        totalElements = m_lstBookComment.GetCurBookmarkNum();
    }

    bool hasElements = totalElements > 0;
    if(hasElements)
    {
        sprintf(str, "%d/%d %s", curPage, totalPage, StringManager::GetPCSTRById(BOOK_PAGE));
    }
    else
    {
        sprintf(str, "%s", StringManager::GetPCSTRById(BOOK_00_PAGE));
    }

    m_pageInfo.SetText(str);
    //int width = m_btnPageInfo.GetTextWidth();
    //int height = m_btnPageInfo.GetTextHeight();
    //int left = m_iWidth - 40 - width;
    //int bottom = m_iHeight - 30 - 60 - 28 - height;
    //m_btnPageInfo.MoveWindow(left, bottom, width, height);

    sprintf(str, "%s %d %s", StringManager::GetPCSTRById(BOOK_TOTAL), totalElements, StringManager::GetPCSTRById(BOOK_TIAO));
    m_textTotalElementsNumber.SetText(CString(str));
    m_pageInfo.SetVisible(hasElements);
    m_textTotalElementsNumber.SetVisible(hasElements);
}

void UITocDlg::SetVecVisible(vector<UIWindow*> *wins, bool visible)
{
    for (size_t i=0; i<(*wins).size(); ++i)
        (*wins)[i]->SetVisible(visible);
}

void UITocDlg::UpdateBookMarkList()
{
    if (SHOWBOOKMARK != m_eShowFlag)
        return;

    //沿用之前的书签类
    //更新在UIBookMarkListBox完成
    //TODO:统一书签与目录的操作

    BOOL bHasBookMark = (m_lstBookMark.GetCurBookmarkNum() > 0);
    m_lstBookMark.SetVisible(bHasBookMark);
    m_bookMarkFailureSizer->Show(!bHasBookMark);
    //m_textGetBookMarkFailure.SetVisible(!bHasBookMark);
    //m_textAddBookMarkGuide.SetVisible(!bHasBookMark);
    m_imgAddBookMark.SetVisible(!bHasBookMark);
}

void UITocDlg::UpdateCommentList()
{
    if (SHOWBOOKCOMMENTANDDIGEST != m_eShowFlag)
        return;

    //沿用之前的书签类
    //更新在UIBookMarkListBox完成
    //TODO:统一书签与目录的操作
    BOOL bHasBookMark = (m_lstBookComment.GetCurBookmarkNum() > 0);
    m_lstBookComment.SetVisible(bHasBookMark);
    m_bookCommentFailureSizer->Show(!bHasBookMark);
    //m_textGetBookCommentFailure.SetVisible(!bHasBookMark);
    //m_textAddBookCommentGuide.SetVisible(!bHasBookMark);
}

void UITocDlg::GetTocInfo()
{
	if(m_pvAllTOC)
	{
        return;
	}
    if (m_bSearchTocFinished || NULL == m_pBookReader)
    {
        return;
    }
	m_pvAllTOC = m_pBookReader->GetTOC();
	m_bSearchTocFinished = true;
}

void UITocDlg::UpdateCurChapter()
{
    if (NULL == m_pBookReader)
    {
        return;
    }

    if (m_pvAllTOC && !m_pvAllTOC->empty())
	{
		ATOM curPageAtom, endAtom;
		m_pBookReader->GetCurrentPageATOM(curPageAtom, endAtom);
		const DK_TOCINFO* curChapterInfo = m_pBookReader->GetChapterInfo(curPageAtom.ToFlowPosition(), CURRENT_CHAPTER);
		for(size_t i = 0; i < m_pvAllTOC->size(); i++)
		{
			if(m_pvAllTOC->at(i) == curChapterInfo)
			{
				m_pCurTOC = m_pvAllTOC->at(i);
                m_pCurTOC->SetAsCurrentToc();
				int curPageIndex = DK_TOCINFO::GetVisibleIndexOfItem(m_pvAllTOC, m_pCurTOC) / m_iChaptersPerPage;
	            m_lstBookToc.SetCurPageIndex(curPageIndex);
				return;
			}
        }
    }
}

std::string UITocDlg::GetCurChapterTitle()
{
    UpdateCurChapter();
    if(m_pCurTOC && m_bSearchTocFinished)
    {
        return m_pCurTOC->GetChapter();
    }
    return "";
}

DK_TOCINFO* UITocDlg::GetCurTocInfo()
{
    return m_pCurTOC;
}

HRESULT UITocDlg::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
    grf.EraserBackGround(ColorManager::knWhite);

    RTN_HR_IF_FAILED(grf.DrawLine(GetWindowMetrics(UIHorizonMarginIndex), 
        m_iTop + m_titleBar.GetHeight() + GetWindowMetrics(UIBackButtonHeightIndex), 
        m_iWidth - (GetWindowMetrics(UIHorizonMarginIndex)<<1), 2, SOLID_STROKE));

    return hr;
}

bool UITocDlg::OnListTurnPage(const EventArgs& args)
{
    const ListTurnPageEvent& listTurnPageEventArgs = (const ListTurnPageEvent&)(args);
    UpdateFocusedList(listTurnPageEventArgs.GetListBoxSender());
    return true;
}

bool UITocDlg::OnListItemDelete(const EventArgs& args)
{
    const ListItemDeleteEvent& listItemDeleteEventArgs = (const ListItemDeleteEvent&)(args);
    UpdateFocusedList(listItemDeleteEventArgs.GetListBoxSender());
    return true;
}

size_t UITocDlg::GetVisibleTocCount() const
{
    return DK_TOCINFO::GetVisibleItemCount(m_pvAllTOC); 
}

void UITocDlg::ExportToEvernote()
{
    if(!UIUtility::CheckAndReConnectWifi())
    {
        return;
    }

    if (EvernoteManager::GetInstance()->IsBusy())
    {
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_WAIT_FOR_LAST_TASK));
        return;
    }

    // Get token and user to see whether we can export directly
    m_btnExportNotes.SetEnable(FALSE);
    isWaitingToExport = true;
    EvernoteTokenPtr token = EvernoteManager::GetInstance()->GetCurrentToken();
    if (token == 0 || token->token().empty())
    {
        if (EvernoteManager::GetInstance()->Auth())
        {
            // Display "Authorizing"
            UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_AUTHORIZING));
            
        }
        else
        {
            UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_AUTHORIZING_FAILED));
            isWaitingToExport = false;
            m_btnExportNotes.SetEnable(TRUE);
        }
        return;
    }
    ImplementExportToEvernote();
}

bool UITocDlg::OnEvernoteAuthFinished(const EventArgs& args)
{
    const EvernoteTokenArgs& tokenArgs = (const EvernoteTokenArgs&)args;
    if (tokenArgs.succeeded && isWaitingToExport)
    {
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_FETCHING_USER_INFO));
        return EvernoteManager::GetInstance()->FetchUser();
    }

    // Display Authorization failed
    if (isWaitingToExport)
    {
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_AUTHORIZING_FAILED));
        isWaitingToExport = false;
        m_btnExportNotes.SetEnable(TRUE);
    }
    return true;
}

bool UITocDlg::OnEvernoteGetUserFinished(const EventArgs& args)
{
    const EvernoteGetUserArgs& userArgs = (const EvernoteGetUserArgs&)args;
    if (isWaitingToExport && userArgs.succeeded)
    {
        return ImplementExportToEvernote();
    }

    // Display Authorization failed
    if (isWaitingToExport)
    {
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_FETCHING_USER_INFO_FAILED));
        isWaitingToExport = false;
        m_btnExportNotes.SetEnable(TRUE);
    }
    return true;
}

bool UITocDlg::ImplementExportToEvernote()
{
    if (EvernoteManager::GetInstance()->ExportLocalNote(m_iBookID))
    {
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_IS_EXPORTING), 100000);
        return true;
    }

    if (isWaitingToExport)
    {
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_EXPORTING_FAILED));
        isWaitingToExport = false;
        m_btnExportNotes.SetEnable(TRUE);
    }
    return false;
}

bool UITocDlg::OnEvernoteExportFinished(const EventArgs& args)
{
    CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
    PCDKFile pDkFile = pFileManager->GetFileById(m_iBookID);
    if(0 == pDkFile)
    {
        return false;
    }

    const EvernoteExportArgs& exportArgs = (const EvernoteExportArgs&)args;
    string bookName = pDkFile->GetFileName();
    string author = pDkFile->GetFileArtist();
    DebugPrintf(DLC_DIAGNOSTIC, "UITocDlg::OnEvernoteExportFinished. bookName:%s\n author:%s\n exportArgs.book_name:%s\n exportArgs.author:%s",
        bookName.c_str(), author.c_str(), exportArgs.book_name.c_str(), exportArgs.author.c_str());
    if (exportArgs.book_name == bookName && exportArgs.author == author)
    {
        if (exportArgs.succeeded)
            UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_SUCCEEDED));
        else
            UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_EXPORTING_FAILED));
    }

    if (isWaitingToExport)
    {
        isWaitingToExport = false;
        m_btnExportNotes.SetEnable(TRUE);
    }
    return true;
}