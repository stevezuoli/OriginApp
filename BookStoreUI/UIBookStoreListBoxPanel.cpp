#include <tr1/functional>
#include "BookStoreUI/UIBookStoreListBoxPanel.h"
#include "BookStoreUI/UIBookStoreBookItem.h"
#include "BookStoreUI/UIBookStoreBookInCartItem.h"
#include "BookStoreUI/UIBookStoreCategoryItem.h"
#include "BookStoreUI/UIBookStoreBookCommentItem.h"
#include "BookStoreUI/UIBookStorePublishingItem.h"
#include "BookStoreUI/UIBookStoreBookCatalogueItem.h"
#include "BookStoreUI/UIBookStoreBookChangeLogItem.h"
#include "PersonalUI/UIPersonalCommentsItem.h"
#include "PersonalUI/UIPersonalSystemMessageItem.h"
#include "PersonalUI/UIBoxMessagesCommentReplyItem.h"
#include "PersonalUI/UIPersonalNotesSummaryItem.h"
#include "PersonalUI/UIPersonalNotesSummaryPage.h"
#include "PersonalUI/PushedMessagesManager.h"
#include "BookStore/BookChangeLog.h"
#include "BookStore/BookNoteSummary.h"
#include "GUI/UIListItemBase.h"
#include "GUI/UISizer.h"
#include "Framework/CDisplay.h"
#include "CommonUI/UIUtility.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include "BookStore/BookStoreOrderManager.h"
#include "BookStore/PushedMessagesInfo.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;

UIBookStoreListBoxPanel::UIBookStoreListBoxPanel(int itemCount, int itemHeight, int itemSpacing)
    : UITablePanel()
    , m_perPageNumber(itemCount)
    , m_listItemHeight(itemHeight)
    , m_currentPage(0)
    , m_totalItem(0)
    , m_elemSpacing(itemSpacing)
    , m_bookInfoType(BS_UNKNOWN)
    , m_hasMoreData(false)
    , m_onceAllItems(false)
    , m_showIndex(false)
    , m_showScore(true)
    , m_enableTurnPage(false)
    , m_enableItemHighlight(false)
#ifdef KINDLE_FOR_TOUCH
    , m_enableItemLongClick(false)
#endif
    , m_objectType(model::OT_UNKNOWN)
    , m_pBottomSizer(NULL)
{
#ifdef KINDLE_FOR_TOUCH
    SubscribeEvent(UITouchCommonListBox::EventListBoxTurnPage, 
        m_listBox,
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreListBoxPanel::OnMessageListBoxTurnPage),
        this,
        std::tr1::placeholders::_1)
        );
#endif
    SubscribeEvent(UITouchCommonListBox::EventListBoxSelectedItemChange, 
        m_listBox,
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreListBoxPanel::OnMessageListBoxSelectedItemChanged),
        this,
        std::tr1::placeholders::_1)
        );
    SubscribeEvent(UITouchCommonListBox::EventListBoxSelectedItemOperation, 
        m_listBox,
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreListBoxPanel::OnMessageListBoxSelectedItemOperation),
        this,
        std::tr1::placeholders::_1)
        );
    
    Init();
}

UIBookStoreListBoxPanel::~UIBookStoreListBoxPanel()
{
    m_listBox.ClearItemList();
}

void UIBookStoreListBoxPanel::Init()
{
    SetTopMargin(GetWindowMetrics(UIPixelValue10Index));
    m_listBox.SetItemHeight(m_listItemHeight);
    AppendChild(&m_listBox);
}

void UIBookStoreListBoxPanel::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
    UIWindow::MoveWindow(iLeft, iTop, iWidth, iHeight);
    if (!m_windowSizer)
    {
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
        m_pBottomSizer = CreateBottomSizer();
        if (pMainSizer && m_pBottomSizer)
        {
            pMainSizer->Add(&m_listBox, UISizerFlags(1).Expand().Border(dkTOP, m_iTopMargin).ReserveSpaceEvenIfHidden());
            m_pBottomSizer->SetMinHeight(m_iHeight - m_iTopMargin - (m_perPageNumber * m_listItemHeight));
            pMainSizer->Add(m_pBottomSizer, UISizerFlags().Expand());
            SetSizer(pMainSizer);
        }
        else
        {
            SafeDeletePointer(pMainSizer);
            SafeDeletePointer(m_pBottomSizer);
        }
    }
}

UISizer* UIBookStoreListBoxPanel::CreateBottomSizer()
{
    return NULL;
}

void UIBookStoreListBoxPanel::SetItemHeight(int itemHeight)
{
    m_listItemHeight = itemHeight;
}

void UIBookStoreListBoxPanel::SetPerPageNumber(int perPageNumber)
{
    m_perPageNumber = perPageNumber;
}

void UIBookStoreListBoxPanel::SetOnceAllItems(bool onceAllItems)
{
    m_onceAllItems = onceAllItems;
}

bool UIBookStoreListBoxPanel::IsLastPage(bool hasMoreData)
{
    if (m_onceAllItems)
    {
        const int totalPage = (m_totalItem + m_perPageNumber - 1) / m_perPageNumber;
        if (totalPage > 0)
        {
            if (m_currentPage >= totalPage)
            {
                m_currentPage = totalPage - 1;
            }
            return (m_currentPage < (totalPage - 1));
        }
    }
    return hasMoreData;
}

void UIBookStoreListBoxPanel::SetBookInfoType(int bookInfoType) 
{ 
    m_bookInfoType = bookInfoType;
}

void UIBookStoreListBoxPanel::SetShowIndex(bool showIndex)
{
    m_showIndex = showIndex;
}

void UIBookStoreListBoxPanel::SetShowScore(bool showScore)
{
    m_showScore = showScore;
}

int UIBookStoreListBoxPanel::GetCurrentPage() const 
{ 
    return m_currentPage; 
}

bool UIBookStoreListBoxPanel::GetOnceAllItems() const 
{
    return m_onceAllItems; 
}

int UIBookStoreListBoxPanel::GetNumberPerPage() const 
{
    return m_perPageNumber; 
}

int UIBookStoreListBoxPanel::GetItemCount() const 
{
    return m_objectList.size(); 
}

void UIBookStoreListBoxPanel::ResetListBox()
{
    m_currentPage = 0;
}

BOOL UIBookStoreListBoxPanel::SetVisible(BOOL bVisible)
{
    if (m_bIsVisible != bVisible)
    {
        m_bIsVisible = bVisible;
        m_listBox.SetVisible(bVisible);
        UpdateBottomItem(TRUE == bVisible);
#ifndef KINDLE_FOR_TOUCH
        SetFocus(bVisible);
        if (bVisible && m_listBox.GetVisibleItemNum() > 0)
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            m_listBox.SetFocus(false);
            m_listBox.SetSelectedItem(0);
            m_listBox.SetFocus(true);
        }
#endif
    }
    return TRUE;
}

#ifdef KINDLE_FOR_TOUCH
bool UIBookStoreListBoxPanel::OnMessageListBoxTurnPage(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const TouchCommonListBoxTurnPageEventArgs& msgArgs = 
        dynamic_cast<const TouchCommonListBoxTurnPageEventArgs&>(args);
    TurnPageUpOrDown(msgArgs.isDownPage);
    return true;
}
#endif

bool UIBookStoreListBoxPanel::OnMessageListBoxSelectedItemChanged(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const TouchCommonListBoxSelectedChangedEventArgs& msgArgs = 
        dynamic_cast<const TouchCommonListBoxSelectedChangedEventArgs&>(args);
    int selectedItem = msgArgs.m_selectedItem;
    bool bRet = false;
    if(selectedItem >= 0 && selectedItem < (int)m_listBox.GetChildrenCount())
    {
        UIListItemBase *pItem = (UIListItemBase *)m_listBox.GetChildByIndex(selectedItem);
        if (pItem && pItem->IsEnable() && pItem->IsVisible())
        {
            pItem->SetHighLight(true);
            bRet = pItem->ResponseTouchTap();
            pItem->SetHighLight(false);
        }
    }
    return bRet;
}

bool UIBookStoreListBoxPanel::OnMessageListBoxSelectedItemOperation(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const TouchCommonListBoxSelectedChangedEventArgs& msgArgs = 
        dynamic_cast<const TouchCommonListBoxSelectedChangedEventArgs&>(args);
    int selectedItem = msgArgs.m_selectedItem;
    bool bRet = false;
    if(selectedItem >= 0 && selectedItem < (int)m_listBox.GetChildrenCount())
    {
        UIListItemBase *pItem = (UIListItemBase *)m_listBox.GetChildByIndex(selectedItem);
        if (pItem)
        {
            pItem->SetHighLight(true);
            bRet = pItem->ResponseOperation();
            pItem->SetHighLight(false);
        }
    }
    return bRet;
}

bool UIBookStoreListBoxPanel::OnMessageListBoxUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const DataUpdateArgs& msgArgs = (const DataUpdateArgs&)args;
    if (msgArgs.succeeded)
    {
        m_objectList.clear();
        const int startIndex = m_onceAllItems ? 0 : (m_perPageNumber * m_currentPage);
        if ((startIndex == msgArgs.startIndex) && (msgArgs.dataList.size() > 0))
        {
            m_totalItem = msgArgs.total;
            m_objectType = msgArgs.dataList[0]->GetObjectType();
            switch(m_objectType)
            {
            case model::OT_BOOK_IN_CART:
            case model::OT_BOOK:
                {
                    model::BookInfoList list = model::BookInfo::FromBasicObjectList(msgArgs.dataList);
                    for (std::vector<model::BookInfoSPtr>::iterator itr = list.begin(); itr != list.end(); itr++)
                    {
                        if ((*itr))
                        {
                            if (m_bookInfoType > 0)
                            {
                                int status = BookStoreInfoManager::GetInstance()->GetBookStatus(*itr);
                                if (m_bookInfoType & status)
                                {
                                    m_objectList.push_back(model::BasicObjectSPtr((*itr)->Clone()));
                                }

                            }
                            else
                            {
                                m_objectList.push_back(model::BasicObjectSPtr((*itr)->Clone()));
                            }
                        }
                    }
                    if (m_onceAllItems)
                    {
                        m_totalItem = m_objectList.size();
                    }
                }
                break;
            case model::OT_CATEGORY:
                {
                    model::CategoryInfoList list = model::CategoryInfo::FromBasicObjectList(msgArgs.dataList);
                    for (std::vector<model::CategoryInfoSPtr>::iterator itr = list.begin(); itr != list.end(); itr++)
                    {
                        m_objectList.push_back(model::BasicObjectSPtr((*itr)->Clone()));
                    }
                    m_totalItem = m_objectList.size();
                }
                break;
            case model::OT_COMMENT:
            case model::OT_MYCOMMENTS:
                {
                    const CommentListUpdateArgs& commentListArgs = (const CommentListUpdateArgs&)args;
                    model::BookCommentList list = model::BookComment::FromBasicObjectList(msgArgs.dataList);
                    for (std::vector<model::BookCommentSPtr>::iterator itr = list.begin(); itr != list.end(); itr++)
                    {
                        if (m_objectType == model::OT_COMMENT)
                        {
                            // 查看我的书评时，每个item对应一个bookId，但是书籍评论列表里面是所有的评论对应一个bookid
                            (*itr)->SetBookId(commentListArgs.bookId.c_str());
                        }
                        m_objectList.push_back(model::BasicObjectSPtr((*itr)->Clone()));
                    }
                }
                break;
            case model::OT_PUBLISH:
                {
                    for (std::vector<model::BasicObjectSPtr>::iterator itr = msgArgs.dataList.begin(); itr != msgArgs.dataList.end(); itr++)
                    {
                        if ((*itr) && ((*itr)->GetObjectType() == model::OT_PUBLISH))
                        {
                            m_objectList.push_back(*itr);
                        }
                    }
                    // TODO:
                    m_totalItem = m_objectList.size();
                }
                break;
            case model::OT_CHANGELOG:
                {
                    model::BookChangeLogList list = model::BookChangeLog::FromBasicObjectList(msgArgs.dataList);
                    for (std::vector<model::BookChangeLogSPtr>::iterator itr = list.begin(); itr != list.end(); itr++)
                    {
                        m_objectList.push_back(model::BasicObjectSPtr((*itr)->Clone()));
                    }
                }
                break;
            default:
                break;
            }
            m_hasMoreData = IsLastPage(msgArgs.moreData);
            UpdateListBox();
        }
    }
    else
    {
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_UPDATE_LIST_PANEL_FAILED));
    }
    return msgArgs.succeeded;
}

bool UIBookStoreListBoxPanel::OnMessageBookCatalogueUpdate(const EventArgs& args)
{
    const BookInfoUpdateArgs& msgArgs = (const BookInfoUpdateArgs&) args;
    if (msgArgs.succeeded)
    {
        m_objectList.clear();
        m_objectType = model::OT_CATALOGUE;
        model::BookInfoSPtr bookInfo = msgArgs.bookInfo;
        if (bookInfo)
        {
            model::CatalogueInfoList list = bookInfo->GetCatalogueList();
            for (std::vector<model::CatalogueInfoSPtr>::iterator itr = list.begin(); itr != list.end(); itr++)
            {
                m_objectList.push_back(model::BasicObjectSPtr((*itr)->Clone()));
            }
        }
        m_totalItem = m_objectList.size();
        m_hasMoreData = IsLastPage(false);
        UpdateListBox();
    }
    return msgArgs.succeeded;
}

bool UIBookStoreListBoxPanel::TurnPageUpOrDown(bool pageDown)
{
    if (!m_enableTurnPage)
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    if (pageDown)
    {
        if (m_hasMoreData)
        {
            m_currentPage++;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (m_currentPage > 0)
        {
            m_currentPage--;
        }
        else
        {
            return false;
        }
    }
    m_enableTurnPage = false;
    if (m_onceAllItems)
    {
        m_hasMoreData = IsLastPage(m_hasMoreData);
        UpdateListBox();
#ifndef KINDLE_FOR_TOUCH
        if (m_bIsVisible && m_listBox.GetVisibleItemNum() > 0)
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            m_listBox.SetFocus(false);
            m_listBox.SetSelectedItem(0);
            m_listBox.SetFocus(true);
        }
#endif
    }
    else if (m_pParent)
    {
        m_pParent->OnCommand(ID_LISTBOX_TURNPAGE, this, 0);
    }
    return true;
}

void UIBookStoreListBoxPanel::UpdateBottomItem(bool visible)
{

}

void UIBookStoreListBoxPanel::UpdateListBox()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const size_t itemCount = m_objectList.size();
    const int startIndex = m_onceAllItems ? (m_currentPage * m_perPageNumber) : 0;
    size_t i = 0;
    int topOffset = 0;
    for (i = 0;i < (size_t)m_perPageNumber; i++)
    {
        const size_t index = startIndex + i;
        if (index >= itemCount)
        {
            break;
        }
        model::BasicObjectSPtr objectItem = m_objectList.at(index);
        if (!objectItem)
        {
            break;
        }

        UIListItemBase *pItem = NULL;
        if(i == m_listBox.GetChildrenCount())
        {
            switch(m_objectType)
            {
            case model::OT_BOOK:
                pItem = new UIBookStoreBookItem(m_showScore);
                break;
            case model::OT_CATEGORY:
                pItem = new UIBookStoreCategoryItem();
                break;
            case model::OT_COMMENT:
                pItem = new UIBookStoreBookCommentItem();
                break;
            case model::OT_MYCOMMENTS:
                pItem = new UIPersonalCommentsItem();
                break;
            case model::OT_PUBLISH:
                pItem = new UIBookStorePublishingItem();
                break;
            case model::OT_CATALOGUE:
                pItem = new UIBookStoreBookCatalogueItem();
                break;
            case model::OT_CHANGELOG:
                pItem = new UIBookStoreBookChangeLogItem();
                break;
            case model::OT_BOXMESSAGES:
				{
					model::PushedMessageInfoSPtr messageInfo(model::PushedMessageInfoSPtr((PushedMessageInfo*)(objectItem->Clone())));
					if (messageInfo->GetMessageType() == model::BOXMESSAGES_SYSTEM)
					{
						pItem = new UIPersonalSystemMessageItem();
					}
					else
					{
						pItem = new UIBoxMessagesCommentReplyItem();
					}
            	}
				break;
            case model::OT_BOOKNOTESUMMARY:
                pItem = new UIPersonalNotesSummaryItem();
				break;
            case model::OT_BOOK_IN_CART:
                pItem = new UIBookStoreBookInCartItem();
				break;
            default:
                break;
            }
            if(pItem)
            {
                pItem->SetMinHeight(m_listItemHeight);
#ifdef KINDLE_FOR_TOUCH
                pItem->SetLongClickable(m_enableItemLongClick);
#endif
                pItem->SetHighLight(m_enableItemHighlight, false);
                m_listBox.AddItem(pItem);
                pItem->MoveWindow(0, topOffset, m_iWidth, m_listItemHeight);
                topOffset += m_listItemHeight;
            }
        }
        else
        {
            pItem = (UIListItemBase *)m_listBox.GetChildByIndex(i);
        }
        if (pItem)
        {
            pItem->SetVisible(TRUE);
            const int itemIndex = m_showIndex ? ((m_currentPage * m_perPageNumber) + i + 1) : -1;
            pItem->SetInfoSPtr(objectItem, itemIndex);
			pItem->Layout();
        }
    }
    for(;i < m_listBox.GetChildrenCount();i++)
    {
        UIWindow* pChild = m_listBox.GetChildByIndex(i);
        if(pChild)
        {
            pChild->SetVisible(FALSE);
        }
    }
    UpdateBottomItem(TRUE == IsVisible());
    //m_listBox.LayoutCommentListBox();
    Layout();
    m_enableTurnPage = true;
}

BOOL UIBookStoreListBoxPanel::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!SendKeyToChildren(iKeyCode))
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s if (!SendKeyToChildren(iKeyCode))", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return FALSE;
    }
    
    switch (iKeyCode)
    {
    case KEY_LPAGEDOWN:
    case KEY_RPAGEDOWN:
    case KEY_SPACE:
        TurnPageUpOrDown(TRUE);
        break;
    case KEY_LPAGEUP:
    case KEY_RPAGEUP:
        TurnPageUpOrDown(FALSE);
        break;
    case KEY_LEFT:
        if (m_listBox.IsFocus())
        {
            UIListItemBase* pItem = (UIListItemBase*)m_listBox.GetItem(m_listBox.GetSelectedItem());
            if (pItem && pItem->IsVisible())
            {
                pItem->ResponseOperation();
                return FALSE;
            }
        }
        break;
    case KEY_UP:
        if (!IsFocus() && (m_listBox.GetVisibleItemNum() > 0))
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            m_listBox.SetSelectedItem(m_listBox.GetVisibleItemNum() - 1);
            SetFocus(true);
            m_listBox.SetFocus(true);
        }
        else
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            m_listBox.SetFocus(false);
            SetFocus(false);
            return TRUE;
        }
        break;
    case KEY_DOWN:
        if (!IsFocus() && (m_listBox.GetVisibleItemNum() > 0))
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            m_listBox.SetSelectedItem(0);
            SetFocus(true);
            m_listBox.SetFocus(true);
        }
        else
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            m_listBox.SetFocus(false);
            SetFocus(false);
            return TRUE;
        }
        break;
    default:
        if(SendHotKeyToChildren(iKeyCode))
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s default", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return UIContainer::OnKeyPress(iKeyCode);
        }
        break;
    }
#endif
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return FALSE;
}

void UIBookStoreListBoxPanel::UpdateSectionUI(bool bIsFull)
{
    for (int i = 0; i < m_listBox.GetVisibleItemNum(); ++i)
    {
        UIListItemBase* pWnd = (UIListItemBase*)m_listBox.GetChildByIndex(i);
        if (pWnd)
        {
            pWnd->UpdateSectionUI(bIsFull);
        }
    }
    return;
}

bool UIBookStoreListBoxPanel::UpdateBookNoteSummary(const EventArgs& args)
{
    const BookNoteSummaryArgs bookNoteSummaryArgs = (const BookNoteSummaryArgs&)args;
    if (!bookNoteSummaryArgs.succeeded)
    {
        return false;
    }
    const int startIndex = m_onceAllItems ? 0 : (m_perPageNumber * m_currentPage);
    if ((startIndex == bookNoteSummaryArgs.startIndex) && (bookNoteSummaryArgs.dataList.size() > 0))
    {
        //m_totalItem += bookNoteSummaryArgs.total;
        m_objectType = bookNoteSummaryArgs.dataList[0]->GetObjectType();

        if (m_objectType == model::OT_BOOKNOTESUMMARY)
        {
            model::BookNoteSummarySPtrList list = model::BookNoteSummary::FromBasicObjectList(bookNoteSummaryArgs.dataList);
            for (std::vector<model::BookNoteSummarySPtr>::iterator itr = list.begin(); itr != list.end(); itr++)
            {
                if ((*itr)->GetCommentCount() > 0)
                {
                    m_objectList.push_back(model::BasicObjectSPtr((*itr)->Clone()));
                }
            }
        }
        m_totalItem = m_objectList.size();
        m_hasMoreData = IsLastPage(bookNoteSummaryArgs.moreData);
    }
    return true;
}

void UIBookStoreListBoxPanel::ClearObjectList()
{
    m_objectList.clear();
    m_totalItem = 0;
}

void UIBookStoreListBoxPanel::SortObjectList(model::BasicObjectCompare comp)
{
    std::sort(m_objectList.begin(), m_objectList.end(), comp);
}

bool UIBookStoreListBoxPanel::RemoveBookItemFromList(const std::string& bookId)
{
	for(std::vector<model::BasicObjectSPtr>::iterator itr = m_objectList.begin(); itr != m_objectList.end(); itr++)
	{
		if((*itr)->GetObjectType() != model::OT_BOOK)
		{
			return false;
		}
		model::BookInfo* bookInfo = (model::BookInfo*)((*itr).get());
		if(bookInfo && bookInfo->GetBookId() == bookId)
		{
			m_objectList.erase(itr);
			m_totalItem--;
			m_hasMoreData = IsLastPage(true);
			m_enableTurnPage = m_hasMoreData;
			UpdateListBox();
			return true;
		}
	}
	return false;
}

bool UIBookStoreListBoxPanel::ReLoadObjectList(model::BookInfoList objList)
{
	ClearObjectList();
	for(std::vector<model::BookInfoSPtr>::iterator itr = objList.begin(); itr != objList.end(); itr++)
	{
		int status = BookStoreInfoManager::GetInstance()->GetBookStatus(*itr);
		if(BS_UNKNOWN == m_bookInfoType)
		{
			m_objectList.push_back(model::BasicObjectSPtr((model::BasicObject*)(itr->get()->Clone())));
		}
        else if(m_bookInfoType & status)
    	{
    		m_objectList.push_back(model::BasicObjectSPtr((model::BasicObject*)(itr->get()->Clone())));
    	}
	}
	m_totalItem = m_objectList.size();
	m_hasMoreData = IsLastPage(true);
	m_enableTurnPage = m_hasMoreData;
	UpdateListBox();
	return true;
}

void UIBookStoreListBoxPanel::ShowTips(const std::string& tips)
{
    if (IsDisplay())
    {
        CDisplay::CacheDisabler forceDraw;
        //UIUtility::ShowTip(m_pTips, true);
        UpdateWindow();
        usleep(500000);
        UIUtility::SetScreenTips(tips.c_str());
    }
}

int UIBookStoreListBoxPanel::GetCurrentMaxItemIndex() const
{
    if (m_currentPage < 0)
    {
        return -1;
    }
    if (m_objectList.size()/m_perPageNumber == (unsigned int)m_currentPage)
    {
        return m_objectList.size() - m_currentPage * m_perPageNumber - 1;
    }

    return m_perPageNumber - 1;
}
