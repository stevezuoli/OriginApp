#ifndef _DKREADER_UIBOOKSTORETABLEPAGELISTBOXWITHPAGENUM_H_
#define _DKREADER_UIBOOKSTORETABLEPAGELISTBOXWITHPAGENUM_H_

#include "BookStoreUI/UIBookStoreListBoxPanel.h"
#include "GUI/UITextSingleLine.h"

class UIBookStoreListBoxWithPageNum : public UIBookStoreListBoxPanel
{
public:
    UIBookStoreListBoxWithPageNum(int itemCount, int itemHeight, int itemSpacing = 0);
    ~UIBookStoreListBoxWithPageNum();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreListBoxWithPageNum";
    }
    virtual void UpdateBottomItem(bool visible = true);
    virtual bool IsLastPage(bool hasMoreData);
    virtual UISizer* CreateBottomSizer();

private:
    void Init();

private:
    UITextSingleLine m_pageNum;
    UITextSingleLine m_itemNum;
};

#endif//_DKREADER_UIBOOKSTORETABLEPAGELISTBOXWITHPAGENUM_H_
