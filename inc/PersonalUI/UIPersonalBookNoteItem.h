#ifndef __PERSONALUI_UIPERSONALBOOKNOTEITEM_H_
#define __PERSONALUI_UIPERSONALBOOKNOTEITEM_H_

#include "GUI/UIListItemBase.h"
#include "GUI/UIText.h"
#include "GUI/UITextSingleLine.h"
#include "DKXManager/ReadingInfo/CT_ReadingDataItemImpl.h"

class UIPersonalBookNoteItem : public UIListItemBase
{
public:

    virtual LPCSTR GetClassName() const
    {
        return "UIPersonalBookNoteItem";
    }
    UIPersonalBookNoteItem(const std::string& bookTitle);
    ~UIPersonalBookNoteItem();
    void SetBookNoteInfoSPtr(ICT_ReadingDataItemSPtr readingDataItemImpl);

    virtual bool ResponseTouchTap();

private:
    void Init();
    void ShowNoteDetailPage();

private:
    std::string m_bookTitle;
    UIText m_textContent;
    UITextSingleLine m_labelModifyTime;
    UITextSingleLine m_labelComment;
    CT_ReadingDataItemImplSPtr m_readingDataItemImpl;
};//UIPersonalBookNoteItem
#endif//__PERSONALUI_UIPERSONALBOOKNOTEITEM_H_

