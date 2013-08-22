////////////////////////////////////////////////////////////////////////
// UIBookListBox.h
// Contact: zhangxb
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIWifiLISTBOX_H__
#define __UIWifiLISTBOX_H__

#include "GUI/UICompoundListBox.h"
#include "Model/IWifiItemModel.h"
#include "TouchAppUI/UIWifiListItem.h"
#include <string>
using std::string;

class UIWifiListBox : public UICompoundListBox
{
public:
    UIWifiListBox();
    UIWifiListBox(UIContainer* pParent, const DWORD rstrId);

    virtual ~UIWifiListBox();

    virtual LPCSTR GetClassName() const
    {
        return "UIWifiListBox";
    }
    void    ClearConnectingStatus();
    HRESULT    SetWifiList(IWifiItemModel **ppList, INT32 iNum);

    virtual void SetFocus(BOOL bIsFocus);

    UIWifiListItem *GetSelectedItem();
    UIWifiListItem* GetListItemByLabel(string label);
    UIWifiListItem* GetListItemByIndex(INT32 index);

    INT32 GetItemIndexByLabel(string label);

    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void InitListItem();
    
    void ClickListBoxItem(INT32 selectedItem);
#ifdef KINDLE_FOR_TOUCH
    virtual BOOL HandleLongTap(INT32 selectedItem);
#endif

protected:
    virtual bool OnItemClick(INT32 iSelectedItem);
    bool DoHandleListTurnPage(bool PageUp);

private:    
    BOOL m_bIsDisposed;
};

#endif  // __UIWifiLISTBOX_H__

