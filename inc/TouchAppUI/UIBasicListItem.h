////////////////////////////////////////////////////////////////////////
// UIBasicListItem.h
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIBASICLISTITEM_H__
#define __UIBASICLISTITEM_H__

#include "GUI/UIComplexButton.h"
#include "GUI/UIContainer.h"
#include "GUI/UIImage.h"
#include "GUI/UIText.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIProgressBar.h"
#include "GUI/UIListItemBase.h"
#include "dkBaseType.h"
#include "dkBuffer.h"
#include "GUI/UITouchComplexButton.h"

#define MarkIconWidth 18
#define IndentWidth 30
class UIBasicListItem : public UIListItemBase
{
public:
    UIBasicListItem();

    UIBasicListItem(UICompoundListBox* pParent,const DWORD dwId);

    ~UIBasicListItem();

    virtual LPCSTR GetClassName() const
    {
        return "UIBasicListItem";
    }

    void InitItem (PCCH pcchItemName, INT iNameLabelSize=24, INT iLocation=-1);
    void SetItemName (PCCH pcchItemName);
    void SetMarked (BOOL fIsMarked);
    void SetIndentMode(BOOL fIndentMode);
    void SetLocation(INT iLocation = -1);
    void SetPageIndex(long pageIndex);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void SetFocus(BOOL bIsFocus);
    virtual BOOL SetVisible(BOOL bVisible);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
	HRESULT DrawBackGround(DK_IMAGE);
    void SetKeyword(PCCH pcchKeyword, UINT uHighLightStartPos, UINT uHighLightEndPos);

    void SetTocItemEnable(bool bEnable);
    void SetFamiliarToTraditional(bool toTraditional){m_familiarToTraditional = toTraditional;}
    void SetDisplayExpandButton(bool displayExpandButton);
    void SetExpandStatus(int expandStatus);
    virtual void OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam);
    virtual void PerformClick(MoveEvent* moveEvent);

protected:
    bool IsDisplayExpandButton() const;
    int GetExpandStatus() const;
    BOOL m_fIsMarked;
    BOOL m_fIndentMode;
    BOOL m_bIsEnabled;
    UIComplexButton m_hotKeyBtn;
    UIText m_itemNameLabel;
    UITextSingleLine m_itemLocation;
    UITextSingleLine m_itemPage;
    UIProgressBar m_readingProgressBar;
    bool m_familiarToTraditional;
    UITouchComplexButton m_expandButton;
    bool m_displayExpandButton;
    int m_expandStatus;
    bool m_tocEnable;
    long m_lPageIndex;
};

#endif  // __UIBASICLISTITEM_H__
