////////////////////////////////////////////////////////////////////////
// UIBasicListBox.cpp
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIBasicListBox.h"
#include "TouchAppUI/UIBasicListItem.h"
#include "Framework/CDisplay.h"
#include "Common/WindowsMetrics.h"
#include "GUI/UISizer.h"
#include "BookReader/TocInfo.h"

using namespace std;
using namespace WindowsMetrics;

UIBasicListBox::UIBasicListBox()
	: UICompoundListBox(NULL,IDSTATIC)
	, m_bIsDisposed(FALSE)
	, m_iMarkedIndex(-1)
	, m_iItemCount(0)
	, m_iItemFontSize(24)
	, m_pKeyword(NULL)
	, m_iCurPage(0)
	, m_iTotalPage(0)
    , m_familiarToTraditional(FALSE)
    , m_displayExpandButton(false)
{
    SetItemHeight(60);
}

UIBasicListBox::UIBasicListBox(int curPage, int TotalPage)
	: UICompoundListBox(NULL,IDSTATIC)
	, m_bIsDisposed(FALSE)
	, m_iMarkedIndex(-1)
	, m_iItemCount(0)
	, m_iItemFontSize(24)
	, m_pKeyword(NULL)
	, m_iCurPage(curPage)
	, m_iTotalPage(TotalPage)
    , m_familiarToTraditional(FALSE)
    , m_displayExpandButton(false)
{
    SetItemHeight(60);
}

UIBasicListBox::UIBasicListBox(UIContainer* pParent, const DWORD dwId)
	: UICompoundListBox(pParent, dwId)
	, m_bIsDisposed(FALSE)
	, m_iMarkedIndex(-1)
	, m_iItemCount(0)
	, m_iItemFontSize(24)
	, m_pKeyword(NULL)
    , m_familiarToTraditional(FALSE)
    , m_displayExpandButton(false)
{
	if (pParent != NULL)
	{
		pParent->AppendChild(this);
    }
    SetItemHeight(60);
}

UIBasicListBox::~UIBasicListBox()
{
    ClearAllChild(true);

	if (m_pKeyword != NULL)
	{
		delete[] m_pKeyword;
		m_pKeyword = NULL;
	}
}

void UIBasicListBox::AddTocItem (PCCH pcchItemName, BOOL enable, int expandStatus, long pageIndex)
{
	m_ItemList.push_back(pcchItemName);
	m_vEnabled.push_back(enable);
    m_vExpandStatus.push_back(expandStatus);
    m_vPageIndex.push_back(pageIndex);
	m_iItemCount++;
	m_iVisibleItemNum++;

	//TODO,以下在目录item里没有用到，与k3代码保持一致。
	m_vHighLightStartPos.push_back(0);
	m_vHighLightEndPos.push_back(0);
    m_vLocation.push_back(-1);
}

#if 1
void UIBasicListBox::AddItem (PCCH pcchItemName, UINT uHighLightStartPos, UINT uHighLightEndPos, INT iLocation)
{
	m_ItemList.push_back(pcchItemName);
	m_vHighLightStartPos.push_back(uHighLightStartPos);
	m_vHighLightEndPos.push_back(uHighLightEndPos);
    m_vLocation.push_back(iLocation);
	m_vEnabled.push_back(true);
    m_vExpandStatus.push_back(DK_TOCINFO::ES_NONE);
    m_vPageIndex.push_back(-1);
	m_iItemCount++;
	m_iVisibleItemNum++;
}
#endif

void UIBasicListBox::SetIndentMode(BOOL bIndentMode)
{
	this->m_bIndentMode = bIndentMode;
}

void UIBasicListBox::CustomizeItem (INT32 iItemFontSize, INT32 iItemHeight)
{
	if (0 < iItemFontSize)
	{
		this->m_iItemFontSize = iItemFontSize;
	}
	if (0 < iItemHeight)
	{
		this->m_iItemHeight = iItemHeight;
	}
	DebugPrintf(DLC_LIUJT, "UIBasicListBox::CustomizeItem iItemFontSize=%d, m_iItemHeight=%d", iItemFontSize, m_iItemHeight);
}

void UIBasicListBox::ClearItems ()
{
	m_ItemList.clear();
	m_vHighLightStartPos.clear();
	m_vHighLightEndPos.clear();
	m_vEnabled.clear();
    m_vLocation.clear();
    m_vPageIndex.clear();
    m_vExpandStatus.clear();
	m_iVisibleItemNum = 0;
	m_iSelectedItem = 0;
	m_iItemCount = 0;
	m_iMarkedIndex = -1;
}

void UIBasicListBox::SetFocus(BOOL bIsFocus)
{
	if (m_bIsFocus != bIsFocus)
	{
		m_bIsFocus = bIsFocus;
		if (m_iSelectedItem < (int)GetChildrenCount())
		{
			UIBasicListItem *pItem = (UIBasicListItem *)GetChildByIndex(m_iSelectedItem);
			if(pItem)
			{
				pItem->SetFocus(bIsFocus);
			}
		}
	}
}

INT32 UIBasicListBox::GetSelectedItemIndex()
{
	if (0 > m_iSelectedItem ||
		m_iSelectedItem >= m_iVisibleItemNum)
	{
		return -1;
	}

	return m_iSelectedItem;
}

BOOL UIBasicListBox::SetVisible(BOOL bVisible)
{
	size_t iSize = GetChildrenCount();
	size_t i;
	m_bIsVisible = bVisible;
	for (i = 0; i < iSize; i++)
	{
		UIBasicListItem *pItem = (UIBasicListItem *)GetChildByIndex(i);
		pItem->SetVisible(bVisible);
	}

	return TRUE;
}

void UIBasicListBox::InitListItem()
{
	UIBasicListItem *pItem (NULL);
	INT32 iItemNum = m_ItemList.size();
	INT32 i (0);
	for (; i < iItemNum; i++)
	{
		if (i == (int)GetChildrenCount())
		{
			pItem = new UIBasicListItem(this, IDSTATIC);
			if (NULL != pItem)
			{
                pItem->SetDisplayExpandButton(IsDisplayExpandButton());
                pItem->SetExpandStatus(m_vExpandStatus[i]);
				pItem->SetIndentMode(this->m_bIndentMode);
                pItem->SetFamiliarToTraditional(m_familiarToTraditional);
				pItem->MoveWindow(0, i * m_iItemHeight, m_iWidth, m_iItemHeight);
				pItem->InitItem(m_ItemList[i].c_str(), m_iItemFontSize);
			}
		}
		else
		{
			pItem = (UIBasicListItem *)GetChildByIndex(i);
			if (NULL != pItem)
			{
                pItem->SetDisplayExpandButton(IsDisplayExpandButton());
                pItem->SetExpandStatus(m_vExpandStatus[i]);
				pItem->SetIndentMode(this->m_bIndentMode);
                pItem->SetFamiliarToTraditional(m_familiarToTraditional);
				pItem->SetItemName((m_ItemList[i]).c_str());
				pItem->SetVisible(TRUE);
				pItem->SetTocItemEnable(m_vEnabled[i]);
			}
		}

		if (NULL != pItem)
		{
			pItem->SetVisible(FALSE);
			pItem->SetFocus(m_bIsFocus && m_iSelectedItem == i);
			pItem->SetVisible(TRUE);
			pItem->SetMarked(m_iMarkedIndex == i);
			if (NULL != m_pKeyword)
			{
				pItem->SetKeyword(m_pKeyword, m_vHighLightStartPos.at(i), m_vHighLightEndPos.at(i));
			}
            pItem->SetLocation(m_vLocation.at(i));
            pItem->SetPageIndex(m_vPageIndex.at(i));
			pItem->SetTocItemEnable(m_vEnabled[i]);
		}
	}

	INT32 iUIItemNum = GetChildrenCount();
	for (; i < iUIItemNum; i++)
	{
		pItem = (UIBasicListItem *)GetChildByIndex(i);
		if (NULL != pItem)
		{
			pItem->SetVisible(FALSE);
		}
	}
}

void UIBasicListBox::SetItemFocus (INT32 iItemIndex, BOOL fIsFocus)
{
	if (0 > iItemIndex)
	{
		iItemIndex = 0;
	}
	else if (iItemIndex >= m_iVisibleItemNum)
	{
		iItemIndex = m_iVisibleItemNum - 1;
	}

	SetChildWindowFocus (iItemIndex, !fIsFocus);
	this->SetSelectedItem(iItemIndex);
}

bool UIBasicListBox::DoHandleListTurnPage(bool fKeyUp)
{
    m_iCurPage = fKeyUp ? (m_iCurPage <= 0 ? m_iTotalPage-1 : m_iCurPage - 1) : (m_iCurPage >= m_iTotalPage-1 ? 0 : m_iCurPage + 1);
    return true;
}

void UIBasicListBox::SetMarkedItemIndex (INT32 iItemIndex)
{
	this->m_iMarkedIndex = iItemIndex;
}

void UIBasicListBox::SetKeyword(PCCH pcchKeyword)
{
	if (pcchKeyword != NULL)
	{
		INT iStrLen = strlen(pcchKeyword);
		if (iStrLen >= 0)
		{
			if (m_pKeyword != NULL)
			{
				delete[] m_pKeyword;
				m_pKeyword = NULL;
			}

			m_pKeyword = new CHAR[iStrLen + 1];
			if (m_pKeyword != NULL)
			{
				memset(m_pKeyword, 0, iStrLen + 1);
				if (iStrLen > 0)
				{
					memcpy(m_pKeyword, pcchKeyword, iStrLen);
				}
			}
		}
	}
}
#ifdef KINDLE_FOR_TOUCH
void UIBasicListBox::UpdateListUI()
{
	DebugPrintf (DLC_UICOMPOUNDLISTBOX, "UIBasicListBox::UpdateListUI begin");
	InitListItem();
	DebugPrintf (DLC_UICOMPOUNDLISTBOX, "UIBasicListBox::UpdateListUI finish");
}

bool UIBasicListBox::OnItemClick(INT32 iSelectedItem)
{
	DebugPrintf (DLC_UICOMPOUNDLISTBOX, "UIBasicListBox::OnItemClick begin");
    if (iSelectedItem < 0 || iSelectedItem >= m_iVisibleItemNum)
    {
        DebugPrintf(DLC_UICOMPOUNDLISTBOX, "UIBasicListBox::OnItemClick () error --iSelectedItem = %d, m_iVisibleItemNum = %d ",iSelectedItem, m_iVisibleItemNum);
        return TRUE;
    }
    UIBasicListItem * pWin = (UIBasicListItem *)GetChildByIndex(iSelectedItem);
	if  (!pWin->IsEnable())
	{
        DebugPrintf(DLC_UICOMPOUNDLISTBOX, "UIBasicListBox::OnItemClick () --iSelectedItem = %d is not enabled, m_iVisibleItemNum = %d ",iSelectedItem, m_iVisibleItemNum);
		return TRUE;
	}

    UICompoundListBox::OnItemClick(iSelectedItem);
    if (m_pParent)
    {
        DebugPrintf (DLC_UICOMPOUNDLISTBOX, "UIBasicListBox::OnItemClick m_pParent");
        m_pParent->OnCommand(ID_BTN_TOUCH_TOC_GOTOTOC, this, iSelectedItem);
    }
	DebugPrintf (DLC_UICOMPOUNDLISTBOX, "UIBasicListBox::OnItemClick finish");
	return true;
}
#endif

void UIBasicListBox::SetDisplayExpandButton(bool displayExpandbutton)
{
    m_displayExpandButton = displayExpandbutton;
}

bool UIBasicListBox::IsDisplayExpandButton() const
{
    return m_displayExpandButton;
}

void UIBasicListBox::OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam)
{
    UIWindow* parent = GetParent();
    if (NULL == parent)
    {
        return;
    }
    if (dwCmdId == ID_BTN_TOUCH_TOC_EXPAND)
    {
        parent->OnCommand(dwCmdId, this, GetChildIndex(pSender));
    }
}
