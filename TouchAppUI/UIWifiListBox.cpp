////////////////////////////////////////////////////////////////////////
// UIBookListBox.cpp
// Contact: zhangxb
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UISizer.h"
#include "TouchAppUI/UIWifiListBox.h"
#include "TouchAppUI/UIWifiDialog.h"
#include <string>

using namespace std;

UIWifiListBox::UIWifiListBox()
    : UICompoundListBox(NULL,IDSTATIC)
    , m_bIsDisposed(FALSE)
{
}

UIWifiListBox::UIWifiListBox(UIContainer* pParent, const DWORD rstrId)
    : UICompoundListBox(pParent, rstrId)
    , m_bIsDisposed(FALSE)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }
}

UIWifiListBox::~UIWifiListBox()
{
    ClearAllChild(true);
}

void UIWifiListBox::ClearConnectingStatus()
{
    string emptyStr;
    for(size_t i = 0; i < GetChildrenCount(); i++)
    {
        UIWifiListItem * pItem = (UIWifiListItem*)GetChildByIndex(i);
        if(NULL == pItem || FALSE == pItem->IsVisible() || TRUE == pItem->GetIsConn())
        {
            continue;
        }

		pItem->ChangeWIFIStatus(UIWifiListItem::NoConnect);
        pItem->SetStateMess(emptyStr);
        pItem->UpdateWindow();
    }
}
HRESULT UIWifiListBox::SetWifiList(IWifiItemModel **ppList, INT32 iNum)
{
    DebugPrintf(DLC_UIWIFIDIALOG, "UIWifiListBox::SetWifiList   iNum=%d, m_iSelectedItem=%d", iNum, m_iSelectedItem);
    if (NULL == ppList || iNum < 0)
    {
        return E_FAIL;
    }

    //m_iSelectedItem = 0;
    if(m_iSelectedItem >= iNum)
    {
        m_iSelectedItem = iNum -1;
    }
    if(m_iSelectedItem <=0)
    {
        m_iSelectedItem = 0;
    }
    m_iVisibleItemNum = iNum;
    //////////////////////////////////////////////////////////////////////////
    UIWifiListItem *pItem = NULL;

    size_t i = 0;
    for (; i < (size_t)iNum; i++)
    {
        if(i >= GetChildrenCount())
        {
            pItem = new UIWifiListItem(this, IDSTATIC);
            if(NULL == pItem)
            {
                return E_OUTOFMEMORY;
            }
            pItem->SetMinHeight(GetItemHeight());
            if (m_windowSizer)
            {
                m_windowSizer->Add(pItem, UISizerFlags().Expand());
            }
        }
        else
        {
            pItem = (UIWifiListItem *)GetChildByIndex(i);
            pItem->SetVisible(TRUE);
        }

        string s = ppList[i]->GetSSID();
        pItem->SetSSID(s);
        pItem->SetIsProtected(ppList[i]->GetIsProtected());
        pItem->SetIsConn(ppList[i]->GetIsConn());
        pItem->SetQualityLv(ppList[i]->GetQualityLv());
        pItem->SetFocus(m_bIsFocus && m_iSelectedItem == (int)i);
		pItem->ChangeWIFIStatus(UIWifiListItem::NoConnect);
        s.clear();
        pItem->SetStateMess(s);
    }

    size_t iUIItemNum = GetChildrenCount();
    for (; i < iUIItemNum; i++)
    {
        pItem = (UIWifiListItem *)GetChildByIndex(i);
        pItem->SetVisible(FALSE);
    }

    Layout();
    DebugPrintf(DLC_UIWIFIDIALOG, "UIWifiListBox::SetWifiList   m_iVisibleItemNum=%d, m_iSelectedItem=%d", m_iVisibleItemNum, m_iSelectedItem);

    return S_OK;
}

void UIWifiListBox::SetFocus(BOOL bIsFocus)
{
    if(m_iSelectedItem<0 ||GetChildrenCount() <=0 ) 
    {
        DebugPrintf(DLC_UIWIFIDIALOG, "UIWifiListBox::SetFocus invalid item %d, with focus: %d, childWindow size=%d", m_iSelectedItem, bIsFocus, GetChildrenCount());
        return;
    }

    if (m_bIsFocus != bIsFocus)
    {
        m_bIsFocus = bIsFocus;     
        if (m_iSelectedItem < (int)GetChildrenCount())
        {
            UIWifiListItem *pItem = (UIWifiListItem *)GetChildByIndex(m_iSelectedItem);
            if(pItem != NULL)
            {
                pItem->SetFocus(bIsFocus);
            }
        }
    }
}

void UIWifiListBox::InitListItem()
{
    return;
}

UIWifiListItem* UIWifiListBox::GetSelectedItem()
{
    if (m_iVisibleItemNum<=0 || m_iSelectedItem < 0 || m_iSelectedItem >= m_iVisibleItemNum||m_iSelectedItem >= (int)GetChildrenCount())
    {
        return NULL;
    }
    return  GetListItemByIndex(m_iSelectedItem);
}

void UIWifiListBox::ClickListBoxItem(INT32 selectedItem)
{
    OnItemClick(selectedItem);
}

bool UIWifiListBox::OnItemClick(INT32 iSelectedItem)
{
    DebugPrintf(DLC_UIWIFIDIALOG, "UIWifiListBox::OnItemClick   visible items=%d, selectedItem =%d ", m_iVisibleItemNum,iSelectedItem);
	
    if (m_iVisibleItemNum <=0 || iSelectedItem < 0 || iSelectedItem >= m_iVisibleItemNum)
    {
        return false;
    }

    INT32 connectedIndex = -1;

    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s for", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    for(size_t index = 0 ; index < GetChildrenCount(); index++)
    {
        UIWifiListItem * pConnectedItem = (UIWifiListItem*)GetChildByIndex(index);
        if(NULL == pConnectedItem) 
        {
            continue;
        }

        if(pConnectedItem->GetIsConn())
        {
            connectedIndex = index;
            if (index == (size_t)iSelectedItem)
            {
                return FALSE;
            }
            break;
        }
    }
    
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s OnItemClick", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
	((UIWifiListItem*)GetChildByIndex(iSelectedItem))->ChangeWIFIStatus(UIWifiListItem::Connecting);
    UICompoundListBox::OnItemClick(iSelectedItem);
    
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s OnEvent", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UIEvent event(CLICK_LISTBOX,this, iSelectedItem, connectedIndex);
    OnEvent(event);
    
    DebugPrintf(DLC_UIWIFIDIALOG, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return false;    
}

BOOL UIWifiListBox::OnKeyPress(INT32 iKeyCode)
{
    if(iKeyCode == KEY_LEFT || iKeyCode == KEY_RIGHT)  //No action when input Left and Right Key
    {
        return FALSE;
    }
    else
    {
        return UICompoundListBox::OnKeyPress(iKeyCode);
    }
}

UIWifiListItem* UIWifiListBox::GetListItemByIndex(INT32 index)
{
    if(index <0 || index >= (int)GetChildrenCount())
    {
        return NULL;
    }

    return (UIWifiListItem*) GetChildByIndex(index);
}


UIWifiListItem* UIWifiListBox::GetListItemByLabel(string label)
{
    INT32 index = GetItemIndexByLabel(label);
    return GetListItemByIndex(index);
}

INT32 UIWifiListBox::GetItemIndexByLabel(string label)
{
    if(label.empty())
    {
        return -1;
    }
    UIWifiListItem* item = NULL;
    for(size_t i = 0; i < GetChildrenCount(); i++)
    {
        item = (UIWifiListItem*) GetChildByIndex(i);
        if(NULL == item)
        {
            continue;
        }

        if( item->GetSSID() == label)
        {
            return i;
        }
    }

    return -1;

}

bool UIWifiListBox::DoHandleListTurnPage(bool PageUp)
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

#ifdef KINDLE_FOR_TOUCH
BOOL UIWifiListBox::HandleLongTap(INT32 selectedItem)
{
    if (selectedItem < 0 || selectedItem >= (int)GetChildrenCount())
        return false;
    if (m_pParent)
    {
        m_pParent->OnCommand(ID_TOUCH_DELETE_USERDEFINEDWIFI, this, selectedItem);
    }
    return true;
}
#endif

