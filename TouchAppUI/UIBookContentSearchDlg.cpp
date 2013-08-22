///////////////////////////////////////////////////////////////////////
// UIBookContentSearchDlg.cpp
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIBookContentSearchDlg.h"
#include "TouchAppUI/UIBookReaderContainer.h"
#include "CommonUI/UIIMEManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIMessageBox.h"
#include "GUI/UISizer.h"
#include "Framework/CDisplay.h"
#include "Framework/CHourglass.h"
#include "I18n/StringManager.h"
#include "Utility/ImageManager.h"
#include "Utility/ColorManager.h"
#include "drivers/DeviceInfo.h"
#include "Common/WindowsMetrics.h"
#include <tr1/functional>
using namespace WindowsMetrics;

using DkFormat::IBookmark;

UIBookContentSearchDlg::UIBookContentSearchDlg (UIContainer* pParent, std::string strBookName, bool needWaitingInfo)
	: UIDialog (pParent),
    m_btnBack(this, ID_BTN_BOOK_CONTENT_DIALOG_BACK),
	m_strBookName(strBookName) ,
	m_iSearchResultCountAll (0),
	m_iBookmarkNum (0),
	m_iBookmarkNumPerPage (0),
	m_iSelectedIndex (-1),
	m_iNextPageIndex (0),
	m_iCurPage (0),
	m_iTotalPage (0),
	m_fUIInitialized(FALSE),
    m_needWaitingInfo(false),
    m_topSizer(NULL),
    m_searchSizer(NULL)
{
	memset (m_chKeyword, 0, MAX_SEARCH_KEYWORD_LENGTH * sizeof (CHAR));
	memset (m_searchResult, 0, MAX_SEARCH_RESULT_COUNT * sizeof (IBookmark*));
	memset (m_HighLightenStartPos, 0, MAX_SEARCH_RESULT_COUNT * sizeof (UINT));
	memset (m_HighLightenEndPos, 0, MAX_SEARCH_RESULT_COUNT * sizeof (UINT));
    memset (m_locationsInBook, -1, MAX_SEARCH_RESULT_COUNT * sizeof (INT));
	Initialize(needWaitingInfo);
}

UIBookContentSearchDlg::~UIBookContentSearchDlg ()
{
    ClearSearchResult();
}

BOOL UIBookContentSearchDlg::Initialize (bool needWaitingInfo)
{
    m_needWaitingInfo = needWaitingInfo;
	return InitUI();
}

BOOL UIBookContentSearchDlg::InitUI ()
{
	if (m_fUIInitialized)
	{
        m_editSearchBox.SetFocus(true);
		return TRUE;
	}

	m_textBookName.SetAlign(ALIGN_CENTER);
	if (!m_strBookName.empty())
	{
		m_textBookName.SetText(m_strBookName.c_str());
		m_textBookName.SetFontSize(GetWindowFontSize(UIBookContentSearchDlgFileNameIndex));
	}

    m_editSearchBox.SetImage(ImageManager::GetImage(IMAGE_ICON_SEARCH));
	m_editSearchBox.SetFontSize(GetWindowFontSize(UIBookContentSearchDlgIndex));
	m_editSearchBox.SetTextUTF8("");
	m_editSearchBox.SetTipUTF8(StringManager::GetInstance()->GetStringById(BOOK_SEARCH_KEYWORD));
    m_editSearchBox.SetId(ID_BTN_BOOK_CONTENT_SEARCH);

	m_btnSearch.Initialize(ID_BTN_BOOK_CONTENT_SEARCH, StringManager::GetPCSTRById(BOOK_CONTENT_SEARCH), GetWindowFontSize(UIBookContentSearchDlgIndex));

	AppendChild(&m_editSearchBox);
	AppendChild(&m_btnSearch);

	m_txtMessage.SetFontSize(GetWindowFontSize(UIBookContentSearchDlgIndex));
	m_txtMessage.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
	m_txtMessage.SetVisible(FALSE);
	m_lstChapterListBox.CustomizeItem(GetWindowFontSize(UIBookContentSearchListItemIndex), GetWindowMetrics(UIPixelValue135Index));

	char str[50] = {0};
	sprintf(str, "%s %d %s", StringManager::GetPCSTRById(BOOK_TOTAL), 0, StringManager::GetPCSTRById(BOOK_TIAO));
	m_txtTotalTag.SetText(CString(str));
	m_txtTotalTag.SetEnglishGothic();
	m_txtTotalTag.SetAlign(ALIGN_LEFT);
    m_txtTotalTag.SetFontSize(GetWindowFontSize(UIBookContentSearchDlgIndex));

	m_pageNum.SetText(StringManager::GetPCSTRById(BOOK_00_PAGE));
	m_pageNum.SetEnglishGothic();
	m_pageNum.SetFontSize(GetWindowFontSize(UIBookContentSearchDlgIndex));
	m_pageNum.SetAlign(ALIGN_RIGHT);

	AppendChild(&m_btnBack);
	AppendChild(&m_textBookName);
	AppendChild(&m_editSearchBox);
	AppendChild(&m_btnSearch);
	AppendChild(&m_txtMessage);
	AppendChild(&m_lstChapterListBox);
	AppendChild(&m_txtTotalTag);
	AppendChild(&m_pageNum);
	AppendChild(&m_titleBar);

	m_fUIInitialized = TRUE;

    EventListener::SubscribeEvent(
            UIAbstractTextEdit::EventTextEditChange,
            m_editSearchBox, 
            std::tr1::bind(
                std::tr1::mem_fn(&UIBookContentSearchDlg::OnInputChange),
                this,
                std::tr1::placeholders::_1));
    if (!m_windowSizer)
    {
        const int backBtnWidth = GetWindowMetrics(UIBookContentSearchDlgBackButtonWidthIndex);
        const int backBtnHeight = GetWindowMetrics(UIBackBtnHeightIndex);
        m_btnSearch.SetMinSize(dkSize(backBtnWidth, backBtnHeight));
        m_editSearchBox.SetMinHeight(backBtnHeight);

        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);

        m_topSizer = new UIBoxSizer(dkHORIZONTAL);
        m_topSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));
        m_topSizer->Add(&m_btnBack, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        m_topSizer->Add(&m_textBookName, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        m_topSizer->AddSpacer(backBtnWidth);

        m_searchSizer = new UIBoxSizer(dkHORIZONTAL);
        m_searchSizer->Add(&m_editSearchBox, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        m_searchSizer->AddSpacer(GetWindowMetrics(UIBookContentSearchDlgButtonMarginIndex));
        m_searchSizer->Add(&m_btnSearch, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));

        UISizer* bottomSizer = new UIBoxSizer(dkHORIZONTAL);
        bottomSizer->Add(&m_txtTotalTag, 1);
        bottomSizer->Add(&m_pageNum, 1);

        mainSizer->Add(&m_titleBar);
        mainSizer->Add(m_topSizer, UISizerFlags().Expand().Border(dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
        mainSizer->AddSpacer(GetWindowMetrics(UIBookContentSearchDlgBarMarginIndex));
        mainSizer->Add(m_searchSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT,GetWindowMetrics(UIHorizonMarginIndex)));
        mainSizer->AddSpacer(GetWindowMetrics(UIBookContentSearchDlgBarMarginIndex)<<1);
        mainSizer->Add(&m_lstChapterListBox, UISizerFlags(1).Expand());
        mainSizer->Add(bottomSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT,GetWindowMetrics(UIHorizonMarginIndex)));
        mainSizer->AddSpacer(GetWindowMetrics(UIBookContentSearchDlgBottomMarginIndex));
        SetSizer(mainSizer);
    }
    UIIME* pIME = UIIMEManager::GetIME(IUIIME_CHINESE_LOWER, &m_editSearchBox);
    if (pIME)
    {
        pIME->SetShowDelay(true);
    }
    return TRUE;
}


void UIBookContentSearchDlg::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
	UIDialog::MoveWindow(iLeft, iTop, iWidth, iHeight);
    SetSize(dkSize(iWidth, iHeight));
    Layout();
	int margin = m_topSizer->GetSize().GetHeight();
	int searchBarHeight = m_searchSizer->GetSize().GetHeight() + (GetWindowMetrics(UIBookContentSearchDlgBarMarginIndex)<<1);
    int top =  m_titleBar.GetHeight() + margin + searchBarHeight + GetWindowMetrics(UIBookContentSearchDlgBarMarginIndex);
    int lineWidth = m_iWidth - (GetWindowMetrics(UIHorizonMarginIndex)<<1);
	m_txtMessage.MoveWindow(GetWindowMetrics(UIHorizonMarginIndex), top, lineWidth, 30);

	m_lstChapterListBox.SetVisible(FALSE);
}

BOOL UIBookContentSearchDlg::InitListBox (ICT_ReadingDataItem * bookmarkList[], INT iListSize, BOOL clearListBox)
{
	if (NULL == bookmarkList || 0 >= iListSize)
	{
		return FALSE;
	}


    if (clearListBox)
    {
	    ClearListBox();
	    m_iCurPage = 0;
	    m_iBookmarkNum = iListSize;
    }
    else
    {
        if (0 == m_iBookmarkNum % m_iBookmarkNumPerPage)
        {
            ++m_iCurPage;
        }
	    m_iBookmarkNum += iListSize;
    }

	m_iBookmarkNumPerPage = m_lstChapterListBox.GetMaxDisplayNum();
	if(0 >= m_iBookmarkNumPerPage)
	{
		return FALSE;
	}
    
	m_iTotalPage = (m_iBookmarkNum + m_iBookmarkNumPerPage - 1) / m_iBookmarkNumPerPage;
	if(!m_iTotalPage)
	{
		m_iTotalPage = 1;
	}


	return TRUE;
}

void UIBookContentSearchDlg::ClearListBox ()
{
	m_iSearchResultCountAll = 0;
	m_iBookmarkNum = 0;
	m_iBookmarkNumPerPage = 0;
	m_iSelectedIndex = -1;
	m_iNextPageIndex = 0;
	m_iCurPage = 0;
	m_iTotalPage = 0;

	m_lstChapterListBox.SetVisible(FALSE);
	m_pageNum.SetVisible(FALSE);
	m_txtTotalTag.SetVisible(FALSE);
}

void UIBookContentSearchDlg::UpdateNavigationButton()
{
	CHAR chTmp[50] = {0};
	if(m_iBookmarkNum)
	{
		sprintf(chTmp, "%d/%d %s", m_iCurPage + 1, m_iTotalPage,StringManager::GetPCSTRById(BOOK_PAGE));
	}
	else
	{
		sprintf(chTmp, "%s", StringManager::GetPCSTRById(BOOK_00_PAGE));
	}

	m_pageNum.SetText(chTmp);
	//int leftMargin = 40;
	//int width  = m_btnPageNo.GetTextWidth();
	//int height = m_btnPageNo.GetTextHeight();
	//int top	   = m_iHeight - 30 - height;
	//m_btnPageNo.MoveWindow(m_iWidth - leftMargin - width, top, width, height);

	sprintf(chTmp, "%s %d %s", StringManager::GetPCSTRById(BOOK_TOTAL), m_iSearchResultCountAll, StringManager::GetPCSTRById(BOOK_TIAO));
	m_txtTotalTag.SetText(CString(chTmp));
	//width  = m_txtTotalTag.GetTextWidth();
	//height = m_txtTotalTag.GetTextHeight();
	//m_txtTotalTag.MoveWindow(leftMargin, top, 200, height);
}

HRESULT UIBookContentSearchDlg::UpdateBookmarkList()
{
	if(0 == m_iBookmarkNum)
	{
		m_lstChapterListBox.SetFocus(FALSE);
		return S_OK;
	}

	m_lstChapterListBox.ClearItems();

	m_iNextPageIndex = (m_iCurPage + 1) == m_iTotalPage ? m_iBookmarkNum : (m_iCurPage + 1) * m_iBookmarkNumPerPage;

	ICT_ReadingDataItem* pBookmark (NULL);
	for (INT32 i = m_iCurPage * m_iBookmarkNumPerPage; i < m_iNextPageIndex; i++)
	{
		pBookmark = m_searchResult[i];
		if (NULL != pBookmark)
		{
			string _strComment = pBookmark->GetUserContent();//优先显示用户创建的批注
			if(_strComment.empty())
			{
				_strComment = pBookmark->GetBookContent();
				if(_strComment.empty())
				{
					continue;
				}
			}

			m_lstChapterListBox.AddItem(_strComment.c_str(), m_HighLightenStartPos[i], m_HighLightenEndPos[i], m_locationsInBook[i]);
		}
	}

	return S_OK;
}

HRESULT UIBookContentSearchDlg::Draw(DK_IMAGE drawingImg)
{
	if (m_lstChapterListBox.IsVisible())
	{
		UpdateNavigationButton();
		UpdateBookmarkList();
	}

	return UIDialog::Draw(drawingImg);
}

HRESULT UIBookContentSearchDlg::DrawBackGround (DK_IMAGE drawingImg)
{
	HRESULT hr (S_OK);
	RTN_HR_IF_FAILED(UIDialog::DrawBackGround(drawingImg));
	DK_IMAGE imgSelf;
	DK_RECT rcSelf={0, 0, m_iWidth, m_iHeight};

	RTN_HR_IF_FAILED(CropImage(
		drawingImg,
		rcSelf,
		&imgSelf));

	CTpGraphics grf(imgSelf);
	int margin = m_topSizer->GetSize().GetHeight();
    int lineWidth = m_iWidth - (GetWindowMetrics(UIHorizonMarginIndex)<<1);
	int searchBarHeight = m_searchSizer->GetSize().GetHeight() + (GetWindowMetrics(UIBookContentSearchDlgBarMarginIndex)<<1);
    grf.DrawLine(GetWindowMetrics(UIHorizonMarginIndex), m_titleBar.GetHeight() + margin, lineWidth, 2, SOLID_STROKE);
    grf.DrawLine(GetWindowMetrics(UIHorizonMarginIndex), m_titleBar.GetHeight() + margin + searchBarHeight, lineWidth, 2, DOTTED_STROKE);

	return S_OK;
}

void UIBookContentSearchDlg::HandlePageUpDown(BOOL fPageDown)
{
    DebugPrintf (DLC_LIUHJ, "UIBookContentSearchDlg::HandlePageUpDown Entrance");
    if (fPageDown)
	{
		m_iCurPage++;
	}
	else
	{
		m_iCurPage--;
	}

    // 当查找结果固定后（向后翻页不会触发递增查找），在首页不响应向前翻页，尾页不响应向后翻页，并弹出提示框
	if (m_iCurPage < 0)
	{
		m_iCurPage = 0;
	}
	else if (m_iCurPage == m_iTotalPage)
	{
		--m_iCurPage;
        if (m_iBookmarkNum >= MAX_SEARCH_RESULT_COUNT)
        {
        }
        else
        {
            // 提示等待
            if (m_needWaitingInfo)
            {
                CHourglass* pIns = CHourglass::GetInstance();
	            if(pIns)
	            {
		            pIns->Start(m_iLeft + m_iWidth / 2, m_iTop + m_iHeight / 2);
	            }
            }

            HandleFormatRenderSearch(SEARCH_NEXT);

            if (m_needWaitingInfo)
            {
                CHourglass* pIns = CHourglass::GetInstance();
	            if(pIns)
	            {
		            pIns->Stop();
	            }
            }
        }
	}
	/*翻页提示
    if (!pageTurned)
    {
        m_messageBoxPopped = true;
	    UIMessageBox messagebox(this, StringManager::GetStringById(fPageDown ? LAST_PAGE_REACHED : FIRST_PAGE_REACHED), UIMessageBox::ICON_INFO, MB_TIPMSG);
        messagebox.DoModal();
    }*/

    UpdateWindow();
    DebugPrintf (DLC_LIUHJ, "UIBookContentSearchDlg::HandlePageUpDown Finish");
}

BOOL UIBookContentSearchDlg::OnKeyPress (INT32 iKeyCode)
{
	return true;
}

BOOL UIBookContentSearchDlg::HandleFormatRenderSearch (SEARCH_MODE mode/* = SEARCH_FROM_START*/)
{
    DebugPrintf (DLC_LIUHJ, "UIBookContentSearchDlg::HandleFormatRenderSearch Start");

	BOOL fNeedRepaint (FALSE);
	UIBookReaderContainer *pTmpReader = dynamic_cast<UIBookReaderContainer *>(m_pParent);
	if (NULL != pTmpReader)
	{
		strncpy(m_chKeyword, m_editSearchBox.GetTextUTF8().c_str(), MAX_SEARCH_KEYWORD_LENGTH - 1);
		m_chKeyword[MAX_SEARCH_KEYWORD_LENGTH - 1] = '\0';
        int iLen = (int)m_editSearchBox.GetTextUTF8().size();
		if (0 !=  iLen)
        {
            DK_FLOWPOSITION loadPos(0, 0, 0);
			INT iSearchResultTableUsed (0);
			INT iSearchResultCountAll (0);
            INT iSearchResultExisted(m_iBookmarkNum);

            if (SEARCH_FROM_START == mode)
            {
                ClearSearchResult();
                iSearchResultExisted = 0;
            }

            if (iSearchResultExisted)
            {
                CT_RefPos tempPos;
                tempPos = m_searchResult[iSearchResultExisted - 1]->GetBeginPos();
                loadPos = DK_FLOWPOSITION(tempPos.GetChapterIndex(), tempPos.GetParaIndex(), tempPos.GetAtomIndex() + 1);
            }

            DebugPrintf (DLC_LIUHJ, "UIBookContentSearchDlg::HandleFormatRenderSearch : Search mode %d, Existed result %d, Search max count at this time %d",
                                                        mode, iSearchResultExisted, dk_min(MAX_SEARCH_RESULT_COUNT - iSearchResultExisted, SEARCH_COUNT_PER_TIME));

            if (SUCCEEDED(pTmpReader->HandleBookContentSearch(m_searchResult + iSearchResultExisted, m_HighLightenStartPos + iSearchResultExisted, 
                                                        m_HighLightenEndPos + iSearchResultExisted, m_locationsInBook + iSearchResultExisted, dk_min(MAX_SEARCH_RESULT_COUNT - iSearchResultExisted, SEARCH_COUNT_PER_TIME), 
                                                        &iSearchResultTableUsed, &iSearchResultCountAll, loadPos, m_chKeyword, iLen)))
			{
				if (InitListBox(m_searchResult + iSearchResultExisted, iSearchResultTableUsed, SEARCH_FROM_START == mode))
				{
					m_iSearchResultCountAll = iSearchResultExisted + iSearchResultCountAll;
					m_lstChapterListBox.SetVisible(TRUE);
					m_lstChapterListBox.SetKeyword(m_chKeyword);
					m_pageNum.SetVisible(TRUE);
					m_txtTotalTag.SetVisible(TRUE);
					fNeedRepaint = TRUE;
				}
			}
		}
	}

	return fNeedRepaint;
}

void UIBookContentSearchDlg::HandleFormatRenderJump ()
{
	UIBookReaderContainer *pTmpReader = dynamic_cast<UIBookReaderContainer *>(m_pParent);
	if (NULL != pTmpReader)
	{
		if (0 != m_iBookmarkNum)
		{
			INT iIndex = m_lstChapterListBox.GetSelectedItemIndex() + m_iCurPage * m_iBookmarkNumPerPage;
			if (0 <= iIndex && MAX_SEARCH_RESULT_COUNT > iIndex)
			{
				pTmpReader->GotoBookmarkAndHighlightKeyword(m_searchResult[iIndex]);
			}
		}
		EndDialog(m_iSelectedIndex);
	}
}

void UIBookContentSearchDlg::ClearSearchResult()
{    
    for (unsigned int i(0); i < DK_DIM(m_searchResult); ++i)
    {
        SafeDeletePointer(m_searchResult[i]);
    }
}

void UIBookContentSearchDlg::StartSearchKeyword()
{
    if (m_editSearchBox.IsEmpty())
	{
		m_txtMessage.SetText(StringManager::GetInstance()->GetStringById(NO_KEYWORD_FOR_TEXT_SEARCHING));
		m_txtMessage.SetVisible(TRUE);
	}
	else
	{
		// 提示等待
		if (m_needWaitingInfo)
		{
			CHourglass* pIns = CHourglass::GetInstance();
			if(pIns)
			{
				pIns->Start(m_iLeft + m_iWidth / 2, m_iTop + m_iHeight / 2);
			}
		}

		// 调用 BookReader 进行查找操作
		m_txtMessage.SetVisible(FALSE);
		if (TRUE == HandleFormatRenderSearch (SEARCH_FROM_START))
		{
			m_txtMessage.SetVisible(FALSE);
			m_lstChapterListBox.SetItemFocus(0, TRUE);
            m_editSearchBox.SelectAll();
        }
		else
		{
			ClearListBox ();
			m_lstChapterListBox.ClearItems();
			m_txtMessage.SetText(StringManager::GetInstance()->GetStringById(NO_TEXT_SEARCH_RESULT));
			m_txtMessage.SetVisible(TRUE);
		}

		if (m_needWaitingInfo)
		{
			CHourglass* pIns = CHourglass::GetInstance();
			if(pIns)
			{
				pIns->Stop();
			}
		}
	}

	UpdateWindow();
}

void UIBookContentSearchDlg::BackToRead()
{
	EndDialog(0);
}

void UIBookContentSearchDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
	switch(dwCmdId)
	{
	case ID_BTN_BOOK_CONTENT_SEARCH:
		StartSearchKeyword();
		break;
	case ID_BTN_LISTITEM_CLICK:
		HandleFormatRenderJump();
		break;
	case ID_LIST_PAGEUP:
		HandlePageUpDown(FALSE);
		break;
	case ID_LIST_PAGEDOWN:
		HandlePageUpDown(TRUE);
		break;
	case ID_BTN_BOOK_CONTENT_DIALOG_BACK:
		BackToRead();
		break;
	default:
		break;
	}
}

void UIBookContentSearchDlg::OnKeywordChange()
{
	if (m_lstChapterListBox.IsVisible())
	{
		ClearListBox();
	}

	UpdateWindow();
}

void UIBookContentSearchDlg::Popup()
{
	for (UINT32 i = 0; i < GetChildrenCount(); i++)
	{
		UIWindow* pWin = GetChildByIndex(i);
		if (pWin) 
        {
			pWin->SetVisible(true);
		}
    }
	m_txtMessage.SetVisible(false);
    m_editSearchBox.SelectAll();
    UIDialog::Popup();
}

void UIBookContentSearchDlg::EndDialog(INT32 iEndCode,BOOL fRepaintIt)
{
	UINT32 iSize = GetChildrenCount();
	for (UINT32 i = 0; i < iSize; i++)
	{
		UIWindow* pWin = GetChildByIndex(i);
		if (pWin) {
			pWin->SetVisible(false);
		}
    }
    m_titleBar.SetVisible(true);
    UIDialog::EndDialog(iEndCode, fRepaintIt);
}

bool UIBookContentSearchDlg::OnInputChange(const EventArgs& args)
{
	OnKeywordChange();
    return true;
}

