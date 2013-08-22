#ifndef _DKREADER_UIBOOKSTORETABLEPAGELISTBOXWITHBTN_H_
#define _DKREADER_UIBOOKSTORETABLEPAGELISTBOXWITHBTN_H_

#include "BookStoreUI/UIBookStoreListBoxPanel.h"
#include "GUI/UITextSingleLine.h"

class UIBookStoreListBoxWithBtn : public UIBookStoreListBoxPanel
{
public:
    UIBookStoreListBoxWithBtn(int itemCount, int itemHeight, int itemSpacing = 0);
    ~UIBookStoreListBoxWithBtn();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreListBoxWithBtn";
    }

    virtual void UpdateBottomItem(bool visible = true);
    virtual UISizer* CreateBottomSizer();

private:
    void Init();

protected:
    UITextSingleLine m_nextWnd;
    UITextSingleLine m_prevWnd;
};

#endif//_DKREADER_UIBOOKSTORETABLEPAGELISTBOXWITHBTN_H_
