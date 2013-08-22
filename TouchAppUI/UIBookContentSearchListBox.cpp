#include "Common/WindowsMetrics.h"
#include "GUI/UISizer.h"
#include "TouchAppUI/UIBookContentSearchDlg.h"
#include "TouchAppUI/UIBookContentSearchListBox.h"
#include "TouchAppUI/UIBookContentSearchListItem.h"
using namespace WindowsMetrics;

void UIBookContentSearchListBox::InitListItem()
{
	DebugPrintf (DLC_ZHY, "UIBookContentSearchListItem::InitListItem entrance");
	UIBookContentSearchListItem *pItem (NULL);
	INT32 iItemNum = m_ItemList.size();
	DebugPrintf(DLC_ZHY, "iItemNum: %d", iItemNum);
	INT32 i (0);
	for (; i < iItemNum; i++)
	{
		if (i == (int)GetChildrenCount())
		{
			pItem = new UIBookContentSearchListItem(this, IDSTATIC);
			if (NULL != pItem)
			{
				//pItem->SetIndentMode(this->m_bIndentMode);
                if (m_windowSizer)
                {
                    m_windowSizer->Add(pItem, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
                }
                pItem->SetMinSize(dkSize(m_iWidth, m_iItemHeight));
				//pItem->MoveWindow(0, i * m_iItemHeight, m_iWidth, m_iItemHeight);
				pItem->InitItem((m_ItemList[i]).c_str(), m_iItemFontSize);
			}
		}
		else
		{
			pItem = (UIBookContentSearchListItem *)GetChildByIndex(i);
			if (NULL != pItem)
			{
				//pItem->SetIndentMode(this->m_bIndentMode);
				pItem->SetItemName((m_ItemList[i]).c_str());
				pItem->SetVisible(TRUE);
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
		}
	}

	INT32 iUIItemNum = GetChildrenCount();
	for (; i < iUIItemNum; i++)
	{
		pItem = (UIBookContentSearchListItem *)GetChildByIndex(i);
		if (NULL != pItem)
		{
			pItem->SetVisible(FALSE);
		}
	}
    Layout();
}

#ifdef KINDLE_FOR_TOUCH
bool UIBookContentSearchListBox::OnItemClick(INT32 iSelectedItem)
{
	DebugPrintf (DLC_UICOMPOUNDLISTBOX, "UIBasicListBox::OnItemClick begin");
    if (iSelectedItem < 0 || iSelectedItem >= m_iVisibleItemNum)
    {
        DebugPrintf(DLC_UICOMPOUNDLISTBOX, "UIBasicListBox::OnItemClick () error --iSelectedItem = %d, m_iVisibleItemNum = %d ",iSelectedItem, m_iVisibleItemNum);
        return TRUE;
    }

    UICompoundListBox::OnItemClick(iSelectedItem);
    if (m_pParent)
    {
        DebugPrintf (DLC_UICOMPOUNDLISTBOX, "UIBasicListBox::OnItemClick m_pParent");
        m_pParent->OnCommand(ID_BTN_LISTITEM_CLICK, this, iSelectedItem);
    }
	DebugPrintf (DLC_UICOMPOUNDLISTBOX, "UIBasicListBox::OnItemClick finish");
	return true;
}

bool UIBookContentSearchListBox::DoHandleListTurnPage(bool PageUp)
{
    if (m_pParent)
    {
        if (PageUp)
        {
            m_pParent->OnCommand(ID_LIST_PAGEUP, this, 0);
        }
        else
        {
            m_pParent->OnCommand(ID_LIST_PAGEDOWN, this, 0);
        }
    }
    return true;
}
#endif
