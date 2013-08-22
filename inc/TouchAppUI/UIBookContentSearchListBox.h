#ifndef UIBOOKCONTENTSEARCHLISTBOX_H
#define UIBOOKCONTENTSEARCHLISTBOX_H

#include "TouchAppUI/UIBasicListBox.h"

class UIBookContentSearchListBox : public UIBasicListBox
{

public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookContentSearchListBox";
    }

    virtual bool OnItemClick(INT32 iSelectedItem);
protected:
    virtual void InitListItem();
    virtual bool DoHandleListTurnPage(bool PageUp);

};//UIBookContentSearchListBox
#endif//UIBOOKCONTENTSEARCHLISTBOX_H
