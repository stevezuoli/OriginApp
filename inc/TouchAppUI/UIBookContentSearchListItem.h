#ifndef UIBOOKCONTENTSEARCHLISTITEM_H
#define UIBOOKCONTENTSEARCHLISTITEM_H

#include "TouchAppUI/UIBasicListItem.h"

class UIBookContentSearchListItem : public UIBasicListItem
{

public:
    UIBookContentSearchListItem();
    UIBookContentSearchListItem(UICompoundListBox* pParent,const DWORD dwId);
    ~UIBookContentSearchListItem();

    virtual LPCSTR GetClassName() const
    {
        return "UIBookContentSearchListItem";
    }

	void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);

    void InitItem (PCCH pcchItemName, INT iNameLabelSize=24, INT iLocation=-1);
    void SetItemName (PCCH pcchItemName);
    void SetLocation(INT iLocation = -1);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
};//UIBookContentSearchListItem







#endif//UIBOOKCONTENTSEARCHLISTITEM_H
