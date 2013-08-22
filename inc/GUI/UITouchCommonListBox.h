#ifndef _DKREADER_TOUCHAPPUI_UITOUCHCOMMONLISTBOX_H_
#define _DKREADER_TOUCHAPPUI_UITOUCHCOMMONLISTBOX_H_

#include <vector>
#include "GUI/UIWindow.h"
#include "GUI/ITpGraphics.h"
#include "GUI/UIImage.h"
#include "GUI/UICompoundListBox.h"
#include "GUI/EventArgs.h"

class TouchCommonListBoxSelectedChangedEventArgs : public EventArgs
{
public:
	virtual EventArgs* Clone() const
	{
		return new TouchCommonListBoxSelectedChangedEventArgs(*this);
	}
	int m_selectedItem;
};


class TouchCommonListBoxTurnPageEventArgs : public EventArgs
{
public:
    virtual EventArgs* Clone() const
    {
        return new TouchCommonListBoxTurnPageEventArgs(*this);
    }
    bool isDownPage;
};

class UITouchCommonListBox : public UICompoundListBox
{
public:
	static const char* EventListBoxSelectedItemChange;
    static const char* EventListBoxTurnPage;
    static const char* EventListBoxSelectedItemOperation;
	UITouchCommonListBox();
	UITouchCommonListBox(UIContainer* pParent, const DWORD dwId);
	~UITouchCommonListBox();

	virtual LPCSTR GetClassName() const
	{
		return "UITouchCommonListBox";
	}

	void LayoutCommentListBox();

	void SetFocus(BOOL bIsFocus);
	void ClearItemList();
	bool AddItem(UIContainer* item);
    UIContainer* GetItem(int index);
    virtual bool OnItemClick(INT32 iSelectedItem);
    void ResetVisibleItemNum();
    int GetVisibleItemNum();
    virtual bool DoHandleListTurnPage(bool fKeyUp);
#ifdef KINDLE_FOR_TOUCH
    virtual BOOL HandleLongTap(INT32 selectedItem);
#endif
private:
    virtual void InitListItem();
private:
    int m_iItemCount;
    std::vector <UIContainer*> m_itemList;
};
#endif//_DKREADER_TOUCHAPPUI_UITOUCHCOMMONLISTBOX_H_

