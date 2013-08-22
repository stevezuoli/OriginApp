#include "GUI/UITouchCommonListBox.h"
#include "GUI/UISizer.h"
//#include "KernelDef.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;


const char* UITouchCommonListBox::EventListBoxSelectedItemChange = "ListBoxSelectedItemChange";
const char* UITouchCommonListBox::EventListBoxTurnPage = "EventListBoxTurnPage";
const char* UITouchCommonListBox::EventListBoxSelectedItemOperation = "EventListBoxSelectedItemOperation";

UITouchCommonListBox::UITouchCommonListBox()
	: UICompoundListBox(NULL,IDSTATIC)
	, m_iItemCount(0)
{
    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    SetSizer(mainSizer);
}
UITouchCommonListBox::UITouchCommonListBox(UIContainer* pParent, const DWORD dwId)
	: UICompoundListBox(pParent,dwId)
	, m_iItemCount(0)
{

}
UITouchCommonListBox::~UITouchCommonListBox()
{
	ClearObjectInVector(m_itemList);
}

void UITouchCommonListBox::LayoutCommentListBox()
{
	UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
	for(size_t i=0;i<m_itemList.size();i++)
	{
		mainSizer->Add(m_itemList[i], UISizerFlags().Expand().ReserveSpaceEvenIfHidden());
	}
	SetSizer(mainSizer);
}


void UITouchCommonListBox::SetFocus(BOOL bIsFocus)
{
    if (m_bIsFocus != bIsFocus)
    {
        UICompoundListBox::SetFocus(bIsFocus);
        m_bIsFocus = bIsFocus;
        if (m_iSelectedItem >= 0 && m_iSelectedItem < m_iVisibleItemNum)
        {
            UIWindow *pItem = GetChildByIndex(m_iSelectedItem);
            if (pItem)
            {
                pItem->SetFocus(m_bIsFocus);
            }
        }
        /*for (size_t i = 0; i < GetChildrenCount(); i++)
        {
            UIWindow *pItem = GetChildByIndex(i);
            if (m_bIsFocus && (int)i == m_iSelectedItem)
            {
                pItem->SetFocus(TRUE == m_bIsFocus);
            }
            else
            {
                pItem->SetFocus(false);
            }
        }*/
    }
}

bool UITouchCommonListBox::DoHandleListTurnPage(bool fKeyUp)
{
    TouchCommonListBoxTurnPageEventArgs args;
    args.isDownPage = !fKeyUp;
    FireEvent(EventListBoxTurnPage,args);
    return true;
}

bool UITouchCommonListBox::OnItemClick(INT32 iSelectedItem)
{
	if(iSelectedItem < 0 || iSelectedItem >= (int)m_itemList.size())
	{
		return true;
	}
	if(!m_itemList[iSelectedItem]->IsVisible())
	{
		return true;
	}
	m_iSelectedItem = iSelectedItem;
	TouchCommonListBoxSelectedChangedEventArgs args;
	args.m_selectedItem = m_iSelectedItem;
	FireEvent(EventListBoxSelectedItemChange,args);
	return false;
}

void UITouchCommonListBox::ClearItemList()
{
	ClearAllChild(false);
	ClearObjectInVector(m_itemList);
    if (m_windowSizer)
    {
        m_windowSizer->Clear();
    }
    m_iVisibleItemNum = 0;
}

void UITouchCommonListBox::ResetVisibleItemNum()
{
    m_iVisibleItemNum = 0;
    for (DK_AUTO(vi, m_itemList.begin()); vi != m_itemList.end(); ++vi)
    {
        if ((*vi)->IsVisible())
            m_iVisibleItemNum++;
    }
}

int UITouchCommonListBox::GetVisibleItemNum() 
{
    return (m_itemList.size() <= 0) ? -1 : m_iVisibleItemNum; 
}

bool UITouchCommonListBox::AddItem(UIContainer* item)
{
	if(NULL == item)
	{
		return false;
	}
    m_itemList.push_back(item);
    if (m_windowSizer)
    {
        m_windowSizer->Add(item, UISizerFlags().Expand().ReserveSpaceEvenIfHidden());
    }
	AppendChild(item);
    m_iVisibleItemNum++;
	return true;
}

void UITouchCommonListBox::InitListItem()
{

}
UIContainer* UITouchCommonListBox::GetItem(int index)
{
    if (index < 0 || index >= (int) m_itemList.size())
    {
        return NULL;
    }
    return m_itemList[index];
}

#ifdef KINDLE_FOR_TOUCH
BOOL UITouchCommonListBox::HandleLongTap(INT32 selectedItem)
{
    TouchCommonListBoxSelectedChangedEventArgs args;
    args.m_selectedItem = m_iSelectedItem = selectedItem;
    FireEvent(EventListBoxSelectedItemOperation,args);
    return false;
}
#endif
