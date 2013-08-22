////////////////////////////////////////////////////////////////////////
// IUIListBox.h
// Contact: xzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef IUILISTBOX_H_
#define IUILISTBOX_H_

#include "Public/GUI/IUIWindow.h"
#include "Utility/CString.h"

class IUIListBox : public virtual IUIWindow
{
public:
    virtual INT32 GetContentHeight() = 0;

    virtual void SetFont(INT32 iFace, INT32 iStyle, INT32 iSize) = 0;

    virtual const CString& GetItem(INT32 iItem) = 0;    
    virtual INT32 GetItemCount() = 0;
    virtual void AddItem(const CString& rstrItem) = 0;
    virtual INT32 GetSelectedItem() = 0;
    virtual void InsertItemAt(const CString& rstrItem, INT32 iPos) = 0;
    virtual void RemoveItem(INT32 iItem) = 0;
    virtual BOOL SetSelectedItem(INT32 iItem) = 0;
    virtual void SetItemStyle(INT32 iItemStyle) = 0;
    virtual INT32 GetItemY(INT32 iItem) = 0;
    virtual BOOL GetItemCheck(INT32 iItem) = 0;
    virtual void SetItemCheck(INT32 iItem, BOOL bCheck) = 0;
    virtual void RemoveAllItem() = 0;
    virtual void SetShowItemSeparator(BOOL bShowItemSeparator) = 0;
    virtual BOOL IsShowItemSeparator() = 0;
    virtual INT32 GetItemHeight(INT32 iItem) = 0;

    virtual const CString& GetSelectedText() = 0;
    virtual ~IUIListBox () {}
};

#endif /*IUILISTBOX_H_*/
