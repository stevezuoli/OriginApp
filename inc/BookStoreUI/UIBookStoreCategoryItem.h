#ifndef _DKREADER_BOOKSTORECATEGORYITEM_H_
#define _DKREADER_BOOKSTORECATEGORYITEM_H_

#include "GUI/UIImage.h"
#include "GUI/UITextSingleLine.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "BookStore/BookStoreInfoManager.h"
#include "GUI/UIListItemBase.h"


class UIBookStoreCategoryItem : public UIListItemBase
{
public:
    static const int s_maxBookCountsFor2ndCategory = 100;
    UIBookStoreCategoryItem();
    ~UIBookStoreCategoryItem();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreCategoryItem";
    }

    void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr categoryInfo, int index = -1);
    bool ResponseTouchTap();
private:
    void Init();
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    void SetBookCountLine(int bookCount);

private:
    UIBookCoverView m_categoryCover;
    UITextSingleLine m_categoryTitel;
    UITextSingleLine m_internalSorting;
    UITextSingleLine m_bookCount;
    UIImage m_passIntoImage;
    dk::bookstore::model::CategoryInfoSPtr m_categoryInfo;
};

#endif//_DKREADER_BOOKSTORECATEGORYITEM_H_
