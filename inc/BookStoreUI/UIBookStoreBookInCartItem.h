#ifndef _DKREADER_UI_BOOKSTORE_BOOKINCART_ITEM_H_
#define _DKREADER_UI_BOOKSTORE_BOOKINCART_ITEM_H_

#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#endif
#include "GUI/UIImage.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIContainer.h"
#include "GUI/CTpGraphics.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "BookStoreUI/UIDiscountTextSingleLine.h"
#include "BookStore/BookStoreInfoManager.h"
#include "interface.h"
#include "BookStoreUI/UIBookStorePriceButtonController.h"
#include "GUI/UIListItemBase.h"
#include "GUI/UIStars.h"

// Events of book in cart item
class BookInCartItemEventArgs: public EventArgs
{
public:
    BookInCartItemEventArgs()
    {}
    virtual EventArgs* Clone() const
    {
        return new BookInCartItemEventArgs(*this);
    }

    dk::bookstore::model::BookInfoSPtr bookInfo;
};

class UIBookStoreBookInCartItemStatusListener: public EventSet
{
public:
    static const char* EventBookInCartItemLongTapped;
    static const char* EventBookInCartDelete;
    static UIBookStoreBookInCartItemStatusListener* GetInstance();

    void FireBookInCartItemLongTappedEvent(dk::bookstore::model::BookInfoSPtr info);
    void FireBookInCartItemDeleteEvent(dk::bookstore::model::BookInfoSPtr info);
};


class UIBookStoreBookInCartItem : public UIListItemBase
{
public:
    UIBookStoreBookInCartItem(bool showScore = true);
    ~UIBookStoreBookInCartItem();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookInCartItem";
    }
    void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr info, int index = 0);
    bool ResponseTouchTap();
    virtual bool ResponseOperation();
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

private:
    void Init();
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
private:
    UIBookCoverView  m_bookCover;
    UITextSingleLine m_bookTitle;
    UITextSingleLine m_bookAuthor;
    UITextSingleLine m_bookPrice;
    UIDiscountTextSingleLine m_bookOriginPrice;
    //UIDiscountTextSingleLine m_bookPaperPrice;
    //UITextSingleLine m_disCountFromPaper;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_responseWnd;
#else
    UITextSingleLine m_responseWnd;
#endif
    dk::bookstore::model::BookInfoSPtr m_bookInfo;
};

#endif //_DKREADER_UI_BOOKSTORE_BOOKINCART_ITEM_H_

