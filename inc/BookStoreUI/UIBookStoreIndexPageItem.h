#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREINDEXPAGEITEM_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREINDEXPAGEITEM_

#include<vector>
#include<string>
#include "GUI/UIListItemBase.h"
#include "GUI/UIPopNum.h"
#include "GUI/UIImage.h"
#include "GUI/UITextSingleLine.h"

class UIBookStoreIndexPageItem : public UIListItemBase
{
public:
    UIBookStoreIndexPageItem();
    ~UIBookStoreIndexPageItem();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreIndexPageItem";
    }

    void InitItem(const DWORD dwId, const std::string& title, const std::string& abstruct);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    void SetUnreadCount(int count);
private:
    void Init();
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
#ifdef KINDLE_FOR_TOUCH
    virtual void PerformClick(MoveEvent* moveEvent);
#endif
private:
    UITextSingleLine m_titleTextLine;
    UITextSingleLine m_abstructLine;
    UIPopNum m_popNum;
    UIImage m_arrow;
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREINDEXPAGEITEM_
