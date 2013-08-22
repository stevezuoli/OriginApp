#ifndef _DKREADER_UIBOOKSTORETABLEPAGELISTBOXPANEL_H_
#define _DKREADER_UIBOOKSTORETABLEPAGELISTBOXPANEL_H_

#include "BookStore/BookStoreInfoManager.h"
#include "CommonUI/UITablePanel.h"
#include "GUI/UITouchCommonListBox.h"
#include "BookStore/BasicObject.h"

class UIBookStoreListBoxPanel : public UITablePanel
{
public:
    UIBookStoreListBoxPanel(int itemCount, int itemHeight, int itemSpacing = 0);
    ~UIBookStoreListBoxPanel();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreListBoxPanel";
    }

    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);

    virtual BOOL SetVisible(BOOL bVisible);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void UpdateBottomItem(bool visible = true);
    virtual bool IsLastPage(bool hasMoreData);
    virtual UISizer* CreateBottomSizer();
    virtual bool TurnPageUpOrDown(bool pageDown);
    virtual bool UpdatePushedMessagesList() { return true; }
    void UpdateSectionUI(bool bIsFull = false);

    void SetItemHeight(int itemHeight);
    int  GetItemHeight() const { return m_listItemHeight; }
    void SetPerPageNumber(int perPageNumber);
    void SetOnceAllItems(bool onceAllItems);
    bool GetOnceAllItems() const;
    void SetBookInfoType(int bookInfoType);
    void SetShowIndex(bool showIndex);
    void SetShowScore(bool showScore);
    bool IsFirstPage() const { return (m_currentPage == 0); }

    // Enable item's long click
    void SetItemEnableLongClick(bool enableItemLongClick) { m_enableItemLongClick = enableItemLongClick; }

    // Enable item's drawing highlight
    void SetItemEnableHighlight(bool enableItemHighlight) { m_enableItemHighlight = enableItemHighlight; }

    int GetCurrentPage() const;
    int GetNumberPerPage() const;
    int GetItemCount() const;
    int GetCurrentMaxItemIndex() const;
    int GetSelectedItemIndex() const
    {
        return m_listBox.GetSelectedItem();
    }

    void SetSelectedItemIndex(int index)
    {
        m_listBox.SetSelectedItem(index);
    }
    void ResetListBox();
    dk::bookstore::model::BasicObjectList GetObjectList() const
    {
        return m_objectList;
    }

#ifdef KINDLE_FOR_TOUCH
    bool OnMessageListBoxTurnPage(const EventArgs& args);
#endif
    virtual bool OnMessageListBoxUpdate(const EventArgs& args);
    bool OnMessageListBoxSelectedItemChanged(const EventArgs& args);
    bool OnMessageListBoxSelectedItemOperation(const EventArgs& args);
    bool OnMessageBookCatalogueUpdate(const EventArgs& args);
    bool UpdateBookNoteSummary(const EventArgs& args);
	bool RemoveBookItemFromList(const std::string& bookId);
    void UpdateListBox();
    void UpdateBookNoteSummaryList();
	bool ReLoadObjectList(dk::bookstore::model::BookInfoList objList);

    void ClearObjectList();
    void SortObjectList(dk::bookstore::model::BasicObjectCompare comp);

protected:
    void ShowTips(const std::string& tips);

private:
    void Init();

protected:
    int m_perPageNumber;
    int m_listItemHeight;
    int m_currentPage;
    int m_totalItem;
    int m_elemSpacing;
    int m_bookInfoType;
    bool m_hasMoreData;
    bool m_onceAllItems;
    bool m_showIndex;
    bool m_showScore;
    bool m_enableTurnPage;
    bool m_enableItemHighlight;
#ifdef KINDLE_FOR_TOUCH
    bool m_enableItemLongClick;
#endif
    UITouchCommonListBox m_listBox;
    dk::bookstore::model::BasicObjectList m_objectList;
    dk::bookstore::model::ObjectType m_objectType;
    UISizer* m_pBottomSizer;
};

#endif//_DKREADER_UIBOOKSTORETABLEPAGELISTBOXPANEL_H_
