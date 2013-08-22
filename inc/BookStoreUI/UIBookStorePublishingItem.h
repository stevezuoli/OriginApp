#ifndef _DKREADER_UIBOOKSTOREPUBLISHINGITEM_H_
#define _DKREADER_UIBOOKSTOREPUBLISHINGITEM_H_

#include "BookStoreUI/UIBookCoverView.h"
#include "GUI/UIListItemBase.h"
#include "GUI/UITextSingleLine.h"
#include "BookStore/AdInfo.h"

class UIBookStorePublishingItem : public UIListItemBase
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStorePublishingItem";
    }
    UIBookStorePublishingItem();
    ~UIBookStorePublishingItem();

    void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr info, int index = -1);
    bool OnChildClick(UIWindow* child);
    void OnClick();
#ifdef KINDLE_FOR_TOUCH
    void PerformClick(MoveEvent* moveEvent);
#else
    BOOL OnKeyPress(INT32 iKeyCode);
#endif

private:
    void Init();
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

private:
    dk::bookstore::model::AdInfoSPtr m_publishInfo;
    UIBookCoverView m_cover;
};

#endif//_DKREADER_UIBOOKSTOREPUBLISHINGITEM_H_
