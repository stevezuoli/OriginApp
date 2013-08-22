////////////////////////////////////////////////////////////////////////
// UIBasicListBox.h
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIBASICLISTBOX_H__
#define __UIBASICLISTBOX_H__

#include <string>
#include "GUI/UICompoundListBox.h"
#include <vector>

class UIBasicListBox : public UICompoundListBox
{
public:
    UIBasicListBox();
	UIBasicListBox(int curPage, int TotalPage);
    UIBasicListBox(UIContainer* pParent, const DWORD dwId);
    virtual ~UIBasicListBox();
    virtual LPCSTR GetClassName() const
    {
        return "UIBasicListBox";
    }

	void AddItem (PCCH pcchItemName, UINT uHighLightStartPos = 0, UINT uHighLightEndPos = 0, INT iLocation = -1);  // iLocation 为元素对应书籍进度百分比
	/**
	 * @brief AddTocItem add toc item to toc-list
	 *
	 * @param pcchItemName chapter name showing
	 * @param enable whether the item is enabled or not, clicking on this item will do nothing if false.
	 */
	void AddTocItem (PCCH pcchItemName, BOOL enable, int expandStatus, long pageIndex = -1);
    void CustomizeItem (INT32 iItemFontSize, INT32 iItemHeight);
    void SetItemFocus (INT32 iItemIndex, BOOL fIsFocus);
    void SetMarkedItemIndex (INT32 iItemIndex);
    INT32 GetSelectedItemIndex();
    void ClearItems ();
	void SetIndentMode(BOOL bIndentMode);
    virtual void SetFocus(BOOL bIsFocus);

    virtual void UpdateListUI();

    virtual BOOL SetVisible(BOOL bVisible);
    void SetKeyword(PCCH pcchKeyword);
    virtual bool OnItemClick(INT32 iSelectedItem);


    void SetTotalPageCount(int total) { m_iTotalPage = total; }
    void SetCurPageIndex(int cur) { m_iCurPage = cur; }
    int GetTotalPageCount() const { return m_iTotalPage; }
    int GetCurPageIndex() const { return m_iCurPage; }

    void SetFamiliarToTraditional(bool toTraditional){m_familiarToTraditional = toTraditional;}
    void SetDisplayExpandButton(bool displayExpandButton);
    virtual void OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam);
protected:
    virtual void InitListItem();
    virtual bool DoHandleListTurnPage(bool fKeyUp);

protected:
    bool IsDisplayExpandButton() const;
    BOOL m_bIsDisposed;
	BOOL m_bIndentMode;
    std::vector<std::string> m_ItemList;
    std::vector<UINT> m_vHighLightStartPos;
    std::vector<UINT> m_vHighLightEndPos;
    std::vector<INT> m_vLocation;  // 如果列表中元素带有进度含义，则标明其进度
    std::vector<BOOL> m_vEnabled;
    std::vector<int> m_vExpandStatus;
    std::vector<int> m_vPageIndex;
    INT32 m_iMarkedIndex;
    INT32 m_iItemCount;
    INT32 m_iItemFontSize;
    CHAR *m_pKeyword;

	int		m_iCurPage;
	int		m_iTotalPage;
    bool m_familiarToTraditional;
    bool m_displayExpandButton;
};


#endif  // __UIBASICLISTBOX_H__
