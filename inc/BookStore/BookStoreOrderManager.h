#ifndef __BOOKSTORE_ORDER_MANAGER_H__
#define __BOOKSTORE_ORDER_MANAGER_H__

#include <vector>
#include "BookStore/BasicObject.h"
#include "BookStore/BookInfo.h"
#include "BookStore/TopicInfo.h"
#include "BookStore/CategoryInfo.h"
#include "BookStore/CreateOrderResult.h"
#include "GUI/EventSet.h"
#include "GUI/EventArgs.h"
#include "BookStore/BookStoreTypes.h"
#include "BookStore/DataUpdateArgs.h"
#include "BookStore/ReadingBookInfo.h"
#include "BookStore/BookComment.h"
#include "BookStore/BookScore.h"
#include "BookStore/BookNoteSummary.h"
#include "BookStore/SpecialEvent.h"
#include "Utility/JsonObject.h"

using namespace dk::bookstore::sync;
using namespace dk::bookstore::model;

namespace dk
{
namespace bookstore
{

class BookStoreOrderArgs : public EventArgs
{
public:
    BookStoreOrderArgs()
        : succeeded(false)
        , resultCode(-1)
    {};

    virtual EventArgs* Clone() const
    {
        return new BookStoreOrderArgs(*this);
    }

    bool Contains(const std::string& bookId) const;
    bool succeeded;
	int resultCode;
    std::vector<std::string> books;
};

class BookStoreAddCartArgs : public BookStoreOrderArgs
{
public:
    BookStoreAddCartArgs()
        : BookStoreOrderArgs()
    {};

    virtual EventArgs* Clone() const
    {
        return new BookStoreAddCartArgs(*this);
    }

    std::vector<std::string> paid_books;
};

class BookStoreOrderManager : public EventSet
{
public:
    // Favourite books
    static const char* EventGetFavouriteUpdate;
    static const char* EventAddFavouriteUpdate;
    static const char* EventRemoveFavouriteUpdate;

    // Cart
    static const char* EventGetCartUpdate;
    static const char* EventAddToCartUpdate;
    static const char* EventRemoveFromCartUpdate;

    // Special Event
    static const char* EventGetSpecialEventUpdate;

	static const char* EventHideBookFinished;
	static const char* EventRevealHideBookFinished;

public:
    virtual ~BookStoreOrderManager() {};
    static BookStoreOrderManager* GetInstance();

    // Favourite
    virtual FetchDataResult FetchFavouriteBooks(int start = 0, int length = 100) = 0;
    virtual FetchDataResult AddFavouriteBooks(const model::BookInfoList& books) = 0;
    virtual FetchDataResult RemoveFavouriteBooks(const model::BookInfoList& books) = 0;
    virtual FetchDataResult MoveFavouriteBooksToCart(const model::BookInfoList& books) = 0;
    virtual bool RemoveFavouritesFromCache(const model::BookInfoList& books) = 0;
    virtual bool GetCachedFavouriteList(DataUpdateArgs& dst) = 0;
    virtual bool IsBookAddedToFavourite(const char* bookId) = 0;

    // Cart
    virtual FetchDataResult FetchBooksInCart(int start = 0, int length = 100) = 0;
    virtual FetchDataResult AddBooksToCart(const model::BookInfoList& books) = 0;
    virtual FetchDataResult RemoveBooksFromCart(const model::BookInfoList& books) = 0;
    virtual FetchDataResult MoveCartBooksToFavourite(const model::BookInfoList& books) = 0;
    virtual bool RemoveBooksFromCartCache(const model::BookInfoList& books) = 0;
    virtual bool GetCachedBooksInCart(DataUpdateArgs& dst) = 0;
    virtual bool IsBookAddedToCart(const char* bookId) = 0;

    // Special Events
    virtual FetchDataResult FetchSpecialEvents() = 0;
    virtual SpecialEventList GetSpecialEventByTime(double refTime) = 0;

    // Hide
	virtual bool AddHideBookIntoCache(const model::BookInfoList& books) = 0;
    virtual bool RemoveHideBookFromCache(const model::BookInfoList& books) = 0;
	virtual FetchDataResult FetchHideBookList(int start = 0, int length = 10000) = 0;
	virtual FetchDataResult HideBook(const model::BookInfoList& books) = 0;
	virtual FetchDataResult RevealHideBook(const model::BookInfoList& books) = 0;
	virtual bool IsBookInHideList(const char* bookId) = 0;
	virtual model::BookInfoSPtr GetBookInfoFromHideList(const char* bookId) = 0;
};


}
}
#endif
