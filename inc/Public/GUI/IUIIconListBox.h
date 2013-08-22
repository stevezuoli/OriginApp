////////////////////////////////////////////////////////////////////////
// IUIIconListBox.h
// Contact: xzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef IUIICONLISTBOX_H_
#define IUIICONLISTBOX_H_

#include "Public/GUI/IUIListBox.h"
#include "Utility/CString.h"

class IUIIconListBox : public virtual IUIListBox
{
public:
    virtual void SetItemIcons(INT32 iItemIndex, SPtr<ITpImage> spSmallImg, SPtr<ITpImage> spLargeImg) = 0;
    virtual INT32 GetIconSizeLarge() = 0;
    virtual void SetIconSizeLarge(INT32 iSizeLarge) = 0;
    virtual INT32 GetIconSizeSmall() = 0;
    virtual void SetIconSizeSmall(INT32 iSizeSmall) = 0;
    virtual void AddItem(const CString& rstrStr, SPtr<ITpImage> spSmallImg, SPtr<ITpImage> spLargeImg) = 0;
    virtual void InsertItemAt(const CString& strStr, SPtr<ITpImage> spSmallImg, SPtr<ITpImage> spLargeImg, INT32 iPos) = 0;
    virtual ~IUIIconListBox () {}
};

#endif /*IUIICONLISTBOX_H_*/
