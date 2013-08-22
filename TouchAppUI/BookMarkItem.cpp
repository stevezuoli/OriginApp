////////////////////////////////////////////////////////////////////////
// BookMarkItem.cpp
// Contact: wanghua
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/BookMarkItem.h"
#include "I18n/StringManager.h"
#include "Utility/ColorManager.h"

#define PAGEFOOT_HEIGHT (40)

CBookMarkItem::CBookMarkItem(int iBookId, LPCSTR pPassword, BOOL bFromReaderPage)
	:UIContainer()
	,m_iBookId(iBookId)
	,m_iPageNum(0)
	,m_iCurPage(0)
	,m_txtTotal()
	,m_btnPageNo()
	,m_lstBookmark(iBookId, pPassword)
{
	//    Init();
}


CBookMarkItem::~CBookMarkItem()
{

}

void CBookMarkItem::Init()
{
	m_txtTotal.SetFontSize(18);
	m_txtTotal.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
	m_txtTotal.MoveWindow(20, m_iHeight - PAGEFOOT_HEIGHT, 150, 20);
	AppendChild(&m_txtTotal);

	m_btnPageNo.SetFontSize(18);
	m_btnPageNo.SetEnglishGothic();
	m_btnPageNo.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
	m_btnPageNo.SetAlign(ALIGN_RIGHT);
	m_btnPageNo.SetEnable(false);
	m_btnPageNo.MoveWindow(370, m_iHeight - PAGEFOOT_HEIGHT, 180, 25);
	AppendChild(&m_btnPageNo);

	m_lstBookmark.MoveWindow(0,PAGEFOOT_HEIGHT >> 1,m_iWidth,m_iHeight - PAGEFOOT_HEIGHT);
	AppendChild(&m_lstBookmark);

}

HRESULT CBookMarkItem::Draw(DK_IMAGE drawingImg)
{
	char pTmp[64];
	int iTotalPage = m_lstBookmark.GetTotalPage();
	if (iTotalPage < 1)
	{
		iTotalPage = 1;
	}
	sprintf(pTmp, "%d/%d %s", m_lstBookmark.GetCurPageNum(), iTotalPage,StringManager::GetPCSTRById(BOOK_PAGE));
	m_btnPageNo.SetText(pTmp);

	sprintf(pTmp, "%s %d %s",StringManager::GetPCSTRById(BOOK_TOTAL), m_lstBookmark.GetCurBookmarkNum(),StringManager::GetPCSTRById(BOOK_TIAO));
	m_txtTotal.SetText(pTmp);

	return UIContainer::Draw(drawingImg);
}


BOOL CBookMarkItem::OnKeyPress(INT32 iKeyCode)
{

	if (!SendKeyToChildren(iKeyCode))
	{
		return FALSE;
	}

	switch (iKeyCode)
	{
	case KEY_LEFT:
		MoveFocus(DIR_LEFT,true);
		break;
	case KEY_UP:
		if(S_FALSE == MoveFocus(DIR_TOP,true))
		{
			this->SetFocus(false);
			return true;
		}
		break;
	case KEY_RIGHT:
		MoveFocus(DIR_RIGHT,true);
		break;
	case KEY_DOWN:
		if(S_FALSE == MoveFocus(DIR_DOWN,true))
		{
			this->SetFocus(false);
			return true;
		}
		break;
	default:
		if(SendHotKeyToChildren(iKeyCode))
		{
			return UIContainer::OnKeyPress(iKeyCode);
		}
		break;
	}

	return FALSE;
}

void CBookMarkItem::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
	switch(dwCmdId)
	{
	case ID_MNU_DELETE_BOOKMARK:
		m_lstBookmark.OnKeyPress(KEY_DEL);
		break;
	case ID_MNU_VIEW_BOOKMARK:
		m_lstBookmark.OnKeyPress(KEY_OKAY);
		break;
	default:
		break;
	}
}


void CBookMarkItem::SetFocus(BOOL bIsFocus)
{
	UIContainer::SetFocus(bIsFocus);
	return m_lstBookmark.SetFocus(bIsFocus);
}


