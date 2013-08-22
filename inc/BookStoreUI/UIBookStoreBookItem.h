#ifndef _DKREADER_UIBOOKSTOREREBOOKITEM_H_
#define _DKREADER_UIBOOKSTOREREBOOKITEM_H_

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


// Events of book item
class BookItemEventArgs: public EventArgs
{
public:
    BookItemEventArgs()
    {}
    virtual EventArgs* Clone() const
    {
        return new BookItemEventArgs(*this);
    }

    dk::bookstore::model::BookInfoSPtr bookInfo;
};

class UIBookStoreBookItemStatusListener: public EventSet
{
public:
    static const char* EventBookItemLongTapped;
    static UIBookStoreBookItemStatusListener* GetInstance();

    void FireBookItemLongTappedEvent(dk::bookstore::model::BookInfoSPtr info);
};

class UIBookStoreBookItem : public UIListItemBase
{
public:
    UIBookStoreBookItem(bool showScore = true);
    ~UIBookStoreBookItem();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookItem";
    }
    void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr info, int index = 0);
    bool ResponseTouchTap();
    void UpdateSectionUI(bool bIsFull = false);

    virtual bool ResponseOperation();
private:
    void Init();
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    bool OnMessagePriceButtonUpdate(const EventArgs& args);
private:
    bool m_showScore;
    UIBookCoverView m_bookCover;
    UITextSingleLine m_bookTitle;
    UITextSingleLine m_bookAuthor;
    UIStars m_scoring;
    UITextSingleLine m_summary;
	UITextSingleLine m_labelPurchasedTime;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_responseWnd;
#else
    UITextSingleLine m_responseWnd;
#endif
    dk::bookstore::model::BookInfoSPtr    m_bookInfo;
    UIDiscountTextSingleLine m_originPriceLine;
    std::string m_orderId;
    UIBookStorePriceButtonController m_buttonController;
};
#endif//_DKREADER_UIBOOKSTOREREBOOKITEM_H_
