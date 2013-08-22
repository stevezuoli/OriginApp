////////////////////////////////////////////////////////////////////////
// UIBookmarkListItem.h
// Contact: zhangjd
// Copyright (c) Duokan Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////

#ifndef __UIBOOKMARKLISTITEM_H__
#define __UIBOOKMARKLISTITEM_H__

#include "GUI/UIContainer.h"
#include "Model/IBookmarkItemModel.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIProgressBar.h"
#include "../GUI/UIText.h"
#include "GUI/UIProgressBar.h"
#include "GUI/UIListItemBase.h"

class IBookReader;

class UIBookmarkListItem : public UIListItemBase
{
public:
    static const int UIBOOKMARKLISTTYPE_BOOKMARK = 0x1;
    static const int UIBOOKMARKLISTTYPE_DIGEST   = 0x2;
    static const int UIBOOKMARKLISTTYPE_COMMENT  = 0x4;

    UIBookmarkListItem(UICompoundListBox* pParent, IBookReader* pBookReader, int iType);

    ~UIBookmarkListItem();

    virtual LPCSTR GetClassName() const
    {
        return ("UIBookmarkListItem");
    }

    void SetBookItemModel(IBookmarkItemModel *pItemModel);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
private:
    void InitItem();

private:    
    IBookmarkItemModel *m_pItemModel;
    UITextSingleLine m_percentage;
    UITextSingleLine m_bookmarkTime;
    UIText m_comment;
    UIText m_fileContent;
    int m_itemType;
    IBookReader* m_pBookReader;
};

#endif  // __UIBOOKMARKLISTITEM_H__
