////////////////////////////////////////////////////////////////////////
// UICompoundListBox.h
// Contact: zhangxb
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UICOMPOUNDLISTBOX_H__
#define __UICOMPOUNDLISTBOX_H__

#include "GUI/UIContainer.h"
#include "GUI/EventArgs.h"
#include "GUI/GestureDetector.h"

class UICompoundListBox;

class ListTurnPageEvent : public EventArgs
{
public:
    ListTurnPageEvent(UICompoundListBox* sender = NULL)
        : listBox(sender)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new ListTurnPageEvent(*this);
    }

    UICompoundListBox* GetListBoxSender() const { return listBox; } 

private:
    UICompoundListBox* listBox;
};//ListContentChanged

class ListItemClickEvent : public EventArgs
{
public:
    virtual EventArgs* Clone() const
    {
        return new ListItemClickEvent(*this);
    }
    enum ListItemType
    {
        Normal,
        Directory
        //add other types
    };

    ListItemClickEvent(UICompoundListBox* sender = NULL, ListItemType listItemType = Normal)
        : listBox(sender)
        , m_eListItemType(listItemType)
    {}

    UICompoundListBox* GetListBoxSender() const { return listBox; } 
    ListItemType GetListItemType() const { return m_eListItemType; }

    bool IsDirectoryItem() const {return m_eListItemType == Directory; }
private:
    UICompoundListBox* listBox;
    ListItemType m_eListItemType;
};//ListItemClickEvent

class ListItemDeleteEvent : public EventArgs
{
public:
    ListItemDeleteEvent(UICompoundListBox* sender = NULL)
        : listBox(sender)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new ListItemDeleteEvent(*this);
    }

    UICompoundListBox* GetListBoxSender() const { return listBox; } 

private:
    UICompoundListBox* listBox;
};

class UICompoundListBox
    : public UIContainer
{
public:
    virtual LPCSTR GetClassName() const
    {
        return ("UICompoundListBox");
    }
    static const char* ListTurnPage;
    static const char* ListItemClick;
	static const char* ListItemDelete;

    virtual ~UICompoundListBox();

    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual BOOL OnKeyRelease(INT32 iKeyCode);

    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual HRESULT Draw(DK_IMAGE drawingImg);

    virtual INT32 GetMaxDisplayNum();
    virtual BOOL  SetItemHeight(INT32 _iItemHeight);
    virtual BOOL  SetItemWidth(INT32 _iItemWidth);
    virtual INT32 GetItemHeight() const { return m_iItemHeight; }
    virtual INT32 GetItemWidth()  const { return m_iItemWidth; }
    virtual BOOL  SetItemSpacing(INT32 _iItemSpacing);
    virtual INT32 GetItemSpacing() const { return m_iItemSpacing; }
     void SetSelectedItem(INT32 iSelected)
     {
         if(iSelected < 0)
         {
             m_iSelectedItem = 0;
         }
         else if (iSelected >= m_iVisibleItemNum)
         {
             m_iSelectedItem = m_iVisibleItemNum -1;
         }
         else
         {
             m_iSelectedItem = iSelected;
         }
     }
    INT32 GetSelectedItem() const
    {
        return m_iSelectedItem;
    }

    virtual void InitListItem()=0;
    virtual void UpdateListUI();
    virtual bool OnChildClick(UIWindow* child);
    virtual bool OnChildLongPress(UIWindow* child);
#ifdef KINDLE_FOR_TOUCH
    virtual bool OnInterceptionTouchEvent(MoveEvent* moveEvent);
    virtual bool OnTouchEvent(MoveEvent* moveEvent);
#endif
    virtual dkSize GetMinSize() const;
    virtual void OnMessage(const SNativeMessage& msg);

protected:
    // UICompoundListBox acts as a base control, which we don't want it be instantiated.
    UICompoundListBox();
    UICompoundListBox(UIContainer* pParent, const DWORD dwId);

    bool HandleListTurnPage(bool pageUp);
    virtual bool DoHandleListTurnPage(bool PageUp);
    virtual BOOL HandleListUpDown(BOOL fKeyUp);
#ifdef KINDLE_FOR_TOUCH
    virtual BOOL HandleLongTap(INT32 selectedItem);
#endif
    virtual void UpdatePartialRegion(INT32 iOldSelectedItem, INT32 iNewSelectedItem);
    virtual bool OnItemClick(INT32 iSelectedItem);

    INT32 m_iSelectedItem;
    INT32 m_iVisibleItemNum;
    INT32 m_iItemHeight;
    INT32 m_iItemWidth;
    int   m_iItemSpacing;

private:
    BOOL m_bIsDisposed;
    bool m_inFlingCheck;
    bool InFlingCheck() const
    {
        return m_inFlingCheck;
    }
    virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);

    class FlingGestureListener : public SimpleGestureListener
    {
    public:
        void SetListBox(UICompoundListBox* listBox)
        {
            m_listBox = listBox;
        }
        virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
        {
            printf("FlingGestureListener::OnFling()\n");
            if (m_listBox->InFlingCheck())
            {
                return m_listBox->OnFling(moveEvent1, moveEvent2, direction, vx, vy);
            }
            return false;
        }
    private:
        UICompoundListBox* m_listBox;
    };
    GestureDetector m_gestureDetector;
    FlingGestureListener m_flingGestureListener;
};

#endif  // __UICOMPOUNDLISTBOX_H__

