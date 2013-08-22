#include "BookStore/BookStoreOrderManager.h"
#include "Mutex.h"
#include "ListDataFetcher.h"
#include "BookStore/BasicObject.h"
#include "BookStore/SpecialEvent.h"
#include "BookStore/DownloadTaskBuilder.h"
#include "BookStore/OfflineBookStoreManager.h"
#include "GUI/EventListener.h"
#include "GUI/MessageEventArgs.h"
#include "I18n/StringManager.h"
#include "BookStore/DataUpdateArgs.h"
#include "BookStore/ConstStrings.h"
#include "BookStore/BookStoreErrno.h"
#include "Utility/JsonObject.h"
#include "Utility/PathManager.h" 
#include "Utility/ConfigFile.h" 
#include "Utility/StringUtil.h" 
#include "Common/CAccountManager.h"
#include "DownloadManager/DownloaderImpl.h"
#include "DownloadManager/DownloadTaskFactory.h"
#include "PersonalUI/PushedMessagesManager.h"
#include "Common/LockObject.h"
#include "XMLDomDocument.h"
#include "Utility/HttpUtil.h"
#include "Utility/SystemUtil.h"
#include <set>
#include <tr1/memory>

using dk::utility::ConfigFile;
using dk::utility::PathManager;
using dk::utility::StringUtil;
using dk::utility::HttpUtil;
using namespace dk::common;
using namespace dk::account;
using namespace dk::bookstore::model;

namespace dk
{
namespace bookstore
{

const char* BookStoreOrderManager::EventGetFavouriteUpdate = "EventGetFavouriteUpdate";
const char* BookStoreOrderManager::EventAddFavouriteUpdate = "EventAddFavouriteUpdate";
const char* BookStoreOrderManager::EventRemoveFavouriteUpdate = "EventRemoveFavouriteUpdate";

const char* BookStoreOrderManager::EventGetCartUpdate = "EventGetCartUpdate";
const char* BookStoreOrderManager::EventAddToCartUpdate = "EventAddToCartUpdate";
const char* BookStoreOrderManager::EventRemoveFromCartUpdate = "EventRemoveFromCartUpdate";

const char* BookStoreOrderManager::EventGetSpecialEventUpdate = "EventGetSpecialEventUpdate";

const char* BookStoreOrderManager::EventHideBookFinished = "EventHideBookFinished";
const char* BookStoreOrderManager::EventRevealHideBookFinished = "EventRevealHideBookFinished";

bool BookStoreOrderArgs::Contains(const std::string& bookId) const
{ 
    if (books.empty())
    {
        return false;
    }
    std::vector<std::string>::const_iterator iter = books.begin();
    for(; iter != books.end(); iter++)
    {
        if ((*iter).compare(bookId) == 0)
        {
            return true;
        }
    }
    return false;
}

class BookStoreOrderManagerImpl: public BookStoreOrderManager
                               , public EventListener
{
public:
    BookStoreOrderManagerImpl();
    virtual ~BookStoreOrderManagerImpl();

    virtual std::string GetCurrentUserName()
    {
        return StringUtil::ToLower(CAccountManager::GetInstance()->GetEmailFromFile().c_str());
    }

    virtual std::string GetCurrentToken()
    {
        return CAccountManager::GetInstance()->GetTokenFromFile();
    }

    virtual FetchDataResult FetchFavouriteBooks(int start = 0, int length = 100);
    virtual FetchDataResult AddFavouriteBooks(const model::BookInfoList& books);
    virtual FetchDataResult RemoveFavouriteBooks(const model::BookInfoList& books);
    virtual FetchDataResult MoveFavouriteBooksToCart(const model::BookInfoList& books);
    virtual bool IsBookAddedToFavourite(const char* bookId);
    virtual bool AddFavouritesIntoCache(const model::BookInfoList& books);
    virtual bool RemoveFavouritesFromCache(const model::BookInfoList& books);
    virtual bool GetCachedFavouriteList(DataUpdateArgs& dst);

    virtual FetchDataResult FetchBooksInCart(int start = 0, int length = 100);
    virtual FetchDataResult AddBooksToCart(const model::BookInfoList& books);
    virtual FetchDataResult RemoveBooksFromCart(const model::BookInfoList& books);
    virtual FetchDataResult MoveCartBooksToFavourite(const model::BookInfoList& books);
    virtual bool IsBookAddedToCart(const char* bookId);
    virtual bool AddBooksIntoCartCache(const model::BookInfoList& books);
    virtual bool RemoveBooksFromCartCache(const model::BookInfoList& books);
    virtual bool GetCachedBooksInCart(DataUpdateArgs& dst);

    virtual FetchDataResult FetchSpecialEvents();
    virtual SpecialEventList GetSpecialEventByTime(double refTime);

	virtual bool AddHideBookIntoCache(const model::BookInfoList& books);
    virtual bool RemoveHideBookFromCache(const model::BookInfoList& books);
	virtual FetchDataResult FetchHideBookList(int start = 0, int length = 10000);
	virtual FetchDataResult HideBook(const model::BookInfoList& books);	
	virtual FetchDataResult RevealHideBook(const model::BookInfoList& books);
	virtual bool IsBookInHideList(const char* bookId);
	virtual model::BookInfoSPtr GetBookInfoFromHideList(const char* bookId);
private:
    bool OnAccountLoginEvent(const EventArgs& args);

    // Favourite
    bool OnGetFavouriteUpdate(const EventArgs& args);
    bool OnAddFavouriteUpdate(const EventArgs& args);
    bool OnRemoveFavouriteUpdate(const EventArgs& args);
    bool OnMoveFavouriteToCartUpdate(const EventArgs& args);

    // Cart
    bool OnGetBooksInCartUpdate(const EventArgs& args);
    bool OnAddBooksToCartUpdate(const EventArgs& args);
    bool OnRemoveBooksFromCartUpdate(const EventArgs& args);
    bool OnMoveCartToFavouriteUpdate(const EventArgs& args);
	bool OnHideBookListUpdate(const EventArgs& args);
	bool OnHideBookFinished(const EventArgs& args);
	bool OnRevealHideBookFinished(const EventArgs& args);

    // Special Events
    bool OnGetSpecialEventsUpdate(const EventArgs& args);

    // Helper functions
    void SubscribeListEvent(ListDataFetcher& listDataFetcher,
                            bool (BookStoreOrderManagerImpl::* memfun)(const EventArgs&));
    void SubscribeDownloadEvent(DownloadTaskSPtr& downloadTask,
                                bool (BookStoreOrderManagerImpl::* memfun)(const EventArgs&));
    bool OnListDataUpdate(const EventArgs& args, const char* eventName);

    bool IsBookAdded(const char* bookId,
                     pthread_mutex_t& mutex,
                     std::tr1::shared_ptr<DataUpdateArgs>& container);
    bool AddBooksIntoContainer(const model::BookInfoList& books,
                               pthread_mutex_t& mutex,
                               std::tr1::shared_ptr<DataUpdateArgs>& container);
    bool RemoveBooksFromContainer(const model::BookInfoList& books,
                                  pthread_mutex_t& mutex,
                                  std::tr1::shared_ptr<DataUpdateArgs>& container);
    bool GetCachedBooksFromContainer(DataUpdateArgs& dst,
                                     pthread_mutex_t& mutex,
                                     std::tr1::shared_ptr<DataUpdateArgs>& container);

private:
    ListDataFetcher m_favouriteBooksFetcher;
    ListDataFetcher m_booksInCartFetcher;
	ListDataFetcher m_hideBookListFetcher;
    ListDataFetcher m_specialEventsFetcher;

    pthread_mutex_t m_favouriteBooksLock;
    std::tr1::shared_ptr<DataUpdateArgs> m_pFavouriteBooks;

    pthread_mutex_t m_cartBooksLock;
    std::tr1::shared_ptr<DataUpdateArgs> m_pCartBooks;

    pthread_mutex_t m_specialEventsLock;
    std::tr1::shared_ptr<DataUpdateArgs> m_pSpecialEvents;
	
	pthread_mutex_t m_hideBooksLock;
    std::tr1::shared_ptr<DataUpdateArgs> m_pHidetBooks;
};

BookStoreOrderManager* BookStoreOrderManager::GetInstance()
{
    static BookStoreOrderManagerImpl manager;
    return &manager;
}

BookStoreOrderManagerImpl::BookStoreOrderManagerImpl()
    : m_favouriteBooksFetcher(DownloadTaskBuilder::BuildGetFavouriteTask, model::OT_BOOK)
    , m_booksInCartFetcher(DownloadTaskBuilder::BuildGetBooksInCartTask, model::OT_BOOK_IN_CART)
    , m_hideBookListFetcher(DownloadTaskBuilder::BuildHideBookListTask, model::OT_BOOK)
    , m_specialEventsFetcher(DownloadTaskBuilder::BuildGetSpecialEventsTask, model::OT_SPECIAL_EVENTS, "events", "result", "total")
    , m_pFavouriteBooks(new DataUpdateArgs)
    , m_pCartBooks(new DataUpdateArgs)
    , m_pSpecialEvents(new DataUpdateArgs)
    , m_pHidetBooks(new DataUpdateArgs)
{
    SubscribeListEvent(m_favouriteBooksFetcher, &BookStoreOrderManagerImpl::OnGetFavouriteUpdate);
    SubscribeListEvent(m_booksInCartFetcher, &BookStoreOrderManagerImpl::OnGetBooksInCartUpdate);
    SubscribeListEvent(m_hideBookListFetcher, &BookStoreOrderManagerImpl::OnHideBookListUpdate);
    SubscribeListEvent(m_specialEventsFetcher, &BookStoreOrderManagerImpl::OnGetSpecialEventsUpdate);

    SubscribeEvent(CAccountManager::EventAccount, 
            *(CAccountManager::GetInstance()),
            std::tr1::bind(
                std::tr1::mem_fn(&BookStoreOrderManagerImpl::OnAccountLoginEvent),
                this,
                std::tr1::placeholders::_1));

    pthread_mutex_init(&m_favouriteBooksLock, 0);
    pthread_mutex_init(&m_cartBooksLock, 0);
	pthread_mutex_init(&m_hideBooksLock, 0);
    pthread_mutex_init(&m_specialEventsLock, 0);
}

void BookStoreOrderManagerImpl::SubscribeListEvent(
        ListDataFetcher& listDataFetcher,
        bool (BookStoreOrderManagerImpl::* memfun)(const EventArgs&))
{
    SubscribeEvent(ListDataFetcher::EventListDataUpdate,
            listDataFetcher,
            std::tr1::bind(
                std::tr1::mem_fn(memfun),
                this,
                std::tr1::placeholders::_1));
}

void BookStoreOrderManagerImpl::SubscribeDownloadEvent(
        DownloadTaskSPtr& downloadTask,
        bool (BookStoreOrderManagerImpl::* memfun)(const EventArgs&))
{
    if (!downloadTask)
    {
        return;
    }
    SubscribeEvent(DownloadTask::EventDownloadTaskFinished,
            *downloadTask.get(),
            std::tr1::bind(
                std::tr1::mem_fn(memfun),
                this,
                std::tr1::placeholders::_1));
}

bool BookStoreOrderManagerImpl::OnListDataUpdate(
        const EventArgs& args,
        const char* eventName)
{
    const DataUpdateArgs& listDataUpdateArgs = (const DataUpdateArgs&)args;
    FireEvent(eventName, listDataUpdateArgs);
    return true;
}

bool BookStoreOrderManagerImpl::OnAccountLoginEvent(const EventArgs& args)
{
    const AccountEventArgs& loginEvent = (const AccountEventArgs&)args;
    if (loginEvent.IsLoggedIn())
    {
        FetchFavouriteBooks();
        FetchBooksInCart();
    }
    return true;
}

BookStoreOrderManagerImpl::~BookStoreOrderManagerImpl()
{
    pthread_mutex_destroy(&m_favouriteBooksLock);
    pthread_mutex_destroy(&m_cartBooksLock);
	pthread_mutex_destroy(&m_hideBooksLock);
    pthread_mutex_destroy(&m_specialEventsLock);
}

FetchDataResult BookStoreOrderManagerImpl::FetchFavouriteBooks(int start, int length)
{
    if (CAccountManager::GetInstance()->IsLoggedIn())
        return m_favouriteBooksFetcher.FetchData(start, length);

    DataUpdateArgs dst;
    if (OfflineBookStoreManager::GetInstance()->GetLocalFavouriteList(dst))
    {
        return OnListDataUpdate(dst, EventGetFavouriteUpdate) ? FDR_SUCCESSED : FDR_FAILED;
    }
    return FDR_FAILED;
}

FetchDataResult BookStoreOrderManagerImpl::FetchBooksInCart(int start, int length)
{
    if (CAccountManager::GetInstance()->IsLoggedIn())
        return m_booksInCartFetcher.FetchData(start, length);

    DataUpdateArgs dst;
    if (OfflineBookStoreManager::GetInstance()->GetLocalBooksInCart(dst))
    {
        return OnListDataUpdate(dst, EventGetCartUpdate) ? FDR_SUCCESSED : FDR_FAILED;
    }
    return FDR_FAILED;
}

static bool GetBookIDs(const model::BookInfoList& books, std::vector<std::string>& bookIDs)
{
    bookIDs.clear();
    for (int i = 0; i < books.size(); i++)
    {
        bookIDs.push_back(books[i]->GetBookId());
    }
    return true;
}

FetchDataResult BookStoreOrderManagerImpl::AddFavouriteBooks(const model::BookInfoList& books)
{
    if (books.empty())
    {
        return FDR_FAILED;
    }   

    if (CAccountManager::GetInstance()->IsLoggedIn())
    {
        DownloadTaskSPtr task = DownloadTaskBuilder::BuildAddFavouriteTask(books);
        if (!task)
        {
            return FDR_FAILED;
        }
        SubscribeDownloadEvent(task, &BookStoreOrderManagerImpl::OnAddFavouriteUpdate);
        DownloadManager::GetInstance()->AddTask(task);
        return FDR_PENDING;
    }
    else
    {
        // Add favourite to local, and notify
        BookStoreErrno err = OfflineBookStoreManager::GetInstance()->AddFavouriteBooks(books);
        BookStoreOrderArgs addFavouriteArgs;
        GetBookIDs(books, addFavouriteArgs.books);
        addFavouriteArgs.succeeded = (err == BSE_OK) ? true : false;
        addFavouriteArgs.resultCode = err;
        FireEvent(EventAddFavouriteUpdate, addFavouriteArgs);
        return (err == BSE_OK) ? FDR_SUCCESSED : FDR_FAILED;
    }
    return FDR_FAILED;
}

FetchDataResult BookStoreOrderManagerImpl::AddBooksToCart(const model::BookInfoList& books)
{
    if (books.empty())
    {
        return FDR_FAILED;
    }   

    if (CAccountManager::GetInstance()->IsLoggedIn())
    {
        DownloadTaskSPtr task = DownloadTaskBuilder::BuildAddBooksInCartTask(books);
        if (!task)
        {
            return FDR_FAILED;
        }
        SubscribeDownloadEvent(task, &BookStoreOrderManagerImpl::OnAddBooksToCartUpdate);
        DownloadManager::GetInstance()->AddTask(task);
        return FDR_PENDING;
    }
    else
    {
        // Add books to local cart
        BookStoreErrno err = OfflineBookStoreManager::GetInstance()->AddBooksToCart(books);
        BookStoreOrderArgs addCartArgs;
        GetBookIDs(books, addCartArgs.books);
        addCartArgs.succeeded = (err == BSE_OK) ? true : false;
        addCartArgs.resultCode = err;
        FireEvent(EventAddToCartUpdate, addCartArgs);
        return (err == BSE_OK) ? FDR_SUCCESSED : FDR_FAILED;
    }
    return FDR_FAILED;
}

FetchDataResult BookStoreOrderManagerImpl::RemoveFavouriteBooks(const model::BookInfoList& books)
{
    if (books.empty())
    {
        return FDR_FAILED;
    }

    if (CAccountManager::GetInstance()->IsLoggedIn())
    {
        DownloadTaskSPtr task = DownloadTaskBuilder::BuildRemoveFavouriteTask(books);
        if (!task)
        {
            return FDR_FAILED;
        }
        SubscribeDownloadEvent(task, &BookStoreOrderManagerImpl::OnRemoveFavouriteUpdate);
        DownloadManager::GetInstance()->AddTask(task);
        return FDR_PENDING;
    }
    else
    {
        // Remove books from local favourites
        BookStoreErrno err = OfflineBookStoreManager::GetInstance()->RemoveFavouriteBooks(books);
        BookStoreOrderArgs removeFavouritesArgs;
        GetBookIDs(books, removeFavouritesArgs.books);
        removeFavouritesArgs.succeeded = (err == BSE_OK) ? true : false;
        removeFavouritesArgs.resultCode = err;
        FireEvent(EventRemoveFavouriteUpdate, removeFavouritesArgs);
        return (err == BSE_OK) ? FDR_SUCCESSED : FDR_FAILED;
    }
    return FDR_FAILED;
}

FetchDataResult BookStoreOrderManagerImpl::MoveFavouriteBooksToCart(const model::BookInfoList& books)
{
    if (books.empty())
    {
        return FDR_FAILED;
    }

    if (CAccountManager::GetInstance()->IsLoggedIn())
    {
        DownloadTaskSPtr task = DownloadTaskBuilder::BuildMoveFavouriteToCart(books);
        if (!task)
        {
            return FDR_FAILED;
        }
        SubscribeDownloadEvent(task, &BookStoreOrderManagerImpl::OnMoveFavouriteToCartUpdate);
        DownloadManager::GetInstance()->AddTask(task);
        return FDR_PENDING;
    }
    else
    {
        // Move favourites to local cart
        BookStoreErrno err = OfflineBookStoreManager::GetInstance()->MoveFavouriteBooksToCart(books);
        BookStoreOrderArgs addCartArgs;
        GetBookIDs(books, addCartArgs.books);
        addCartArgs.succeeded = (err == BSE_OK) ? true : false;
        addCartArgs.resultCode = err;
        FireEvent(EventAddToCartUpdate, addCartArgs);
        return (err == BSE_OK) ? FDR_SUCCESSED : FDR_FAILED;
    }
    return FDR_FAILED;
}

FetchDataResult BookStoreOrderManagerImpl::RemoveBooksFromCart(const model::BookInfoList& books)
{
    if (books.empty())
    {
        return FDR_FAILED;
    }

    if (CAccountManager::GetInstance()->IsLoggedIn())
    {
        DownloadTaskSPtr task = DownloadTaskBuilder::BuildRemoveBooksInCartTask(books);
        if (!task)
        {
            return FDR_FAILED;
        }
        SubscribeDownloadEvent(task, &BookStoreOrderManagerImpl::OnRemoveBooksFromCartUpdate);
        DownloadManager::GetInstance()->AddTask(task);
        return FDR_PENDING;
    }
    else
    {
        // Remove books from local cart
        BookStoreErrno err = OfflineBookStoreManager::GetInstance()->RemoveBooksFromCart(books);
        BookStoreOrderArgs removeCartArgs;
        GetBookIDs(books, removeCartArgs.books);
        removeCartArgs.succeeded = (err == BSE_OK) ? true : false;
        removeCartArgs.resultCode = err;
        FireEvent(EventRemoveFromCartUpdate, removeCartArgs);
        return (err == BSE_OK) ? FDR_SUCCESSED : FDR_FAILED;
    }
    return FDR_FAILED;
}

FetchDataResult BookStoreOrderManagerImpl::MoveCartBooksToFavourite(const model::BookInfoList& books)
{
    if (books.empty())
    {
        return FDR_FAILED;
    }

    if (CAccountManager::GetInstance()->IsLoggedIn())
    {
        DownloadTaskSPtr task = DownloadTaskBuilder::BuildMoveCartToFavourite(books);
        if (!task)
        {
            return FDR_FAILED;
        }
        SubscribeDownloadEvent(task, &BookStoreOrderManagerImpl::OnMoveCartToFavouriteUpdate);
        DownloadManager::GetInstance()->AddTask(task);
        return FDR_PENDING;
    }
    else
    {
        // Move books from local cart to favourites
        BookStoreErrno err = OfflineBookStoreManager::GetInstance()->MoveCartBooksToFavourite(books);

        // Notify adding to favourite
        BookStoreOrderArgs addFavouriteArgs;
        GetBookIDs(books, addFavouriteArgs.books);
        addFavouriteArgs.succeeded = (err == BSE_OK) ? true : false;
        addFavouriteArgs.resultCode = err;
        FireEvent(EventAddFavouriteUpdate, addFavouriteArgs);

        if (err == BSE_OK)
        {
            BookStoreOrderArgs removeCartArgs;
            GetBookIDs(books, removeCartArgs.books);
            removeCartArgs.succeeded = true;
            removeCartArgs.resultCode = err;
            FireEvent(EventRemoveFromCartUpdate, removeCartArgs);
            return FDR_SUCCESSED;
        }
    }
    return FDR_FAILED;
}

bool BookStoreOrderManagerImpl::OnGetFavouriteUpdate(const EventArgs& args)
{
    // TODO. cache? sort?
    //DebugPrintf(DLC_DIAGNOSTIC, "\n BookStoreOrderManagerImpl::OnGetFavouriteUpdate \n");

    // Cache the books
    const DataUpdateArgs& dataUpdateArgs = (const DataUpdateArgs&)args;
    if (dataUpdateArgs.succeeded)
    {
        {
            AutoLock lock(&m_favouriteBooksLock);
            // Update cache
            m_pFavouriteBooks.reset(new DataUpdateArgs(dataUpdateArgs));
        }

        // merge offline favourites
        model::BookInfoList mergeList;
        if ((OfflineBookStoreManager::GetInstance()->UpdateFavouritesByCloud(dataUpdateArgs, mergeList) != BSE_UNKNOWN) &&
            !mergeList.empty())
        {
            AddFavouriteBooks(mergeList);
        }
    }
    return OnListDataUpdate(args, EventGetFavouriteUpdate);
}

bool BookStoreOrderManagerImpl::OnGetBooksInCartUpdate(const EventArgs& args)
{
    // TODO. cache? sort?
    //DebugPrintf(DLC_DIAGNOSTIC, "\n BookStoreOrderManagerImpl::OnGetBooksInCartUpdate \n");

    // Cache the books
    const DataUpdateArgs& dataUpdateArgs = (const DataUpdateArgs&)args;
    if (dataUpdateArgs.succeeded)
    {
        {
            AutoLock lock(&m_cartBooksLock);
            // Update cache
            m_pCartBooks.reset(new DataUpdateArgs(dataUpdateArgs));
        }

        // merge offline cart
        model::BookInfoList mergeList;
        if ((OfflineBookStoreManager::GetInstance()->UpdateCartByCloud(dataUpdateArgs, mergeList) != BSE_UNKNOWN) &&
            !mergeList.empty())
        {
            AddBooksToCart(mergeList);
        }
    }
    return OnListDataUpdate(args, EventGetCartUpdate);
}

static bool IsStringInList(const std::string& dst, const std::vector<std::string>& list)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        if (list[i].compare(dst) == 0)
        {
            return true;
        }
    }
    return false;
}

static bool StringToStringList(const std::string& src, std::vector<std::string>& dst, const std::vector<std::string>& filter)
{
    std::string first;
    std::string second = src;

    unsigned pos = second.find_first_of(";");
    while (pos != std::string::npos)
    {
        first = second.substr(0, pos);
        if (!IsStringInList(first, filter))
        {
            //DebugPrintf(DLC_DIAGNOSTIC, "\n Dump Favourite Book:%s \n", first.c_str());
            dst.push_back(first);
        }
        second = second.substr(pos + 1);
        if (second.empty())
        {
            break;
        }
        pos = second.find_first_of(",");
    }
    if (!second.empty() && !IsStringInList(second, filter))
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "\n Dump Favourite Book:%s \n", second.c_str());
        dst.push_back(second);
    }
    return true;
}

bool BookStoreOrderManagerImpl::OnAddFavouriteUpdate(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;

    BookStoreOrderArgs addFavouriteArgs;
    std::vector<std::string> emptyFilter;
    std::string books_str = task->GetPostValue("item");
    StringToStringList(books_str, addFavouriteArgs.books, emptyFilter);

    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        int resultCode = -1;
        if (jsonObj)
        {
            if (jsonObj->GetIntValue("result", &resultCode) && 0 == resultCode)
            {
                addFavouriteArgs.succeeded = true;
                AddFavouritesIntoCache((dynamic_cast<BookOrderDownloadTask*>(task))->GetBooks());
            }
            addFavouriteArgs.resultCode = resultCode;
        }
    }
    FireEvent(EventAddFavouriteUpdate, addFavouriteArgs);
    return true;
}

bool BookStoreOrderManagerImpl::OnAddBooksToCartUpdate(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    BookStoreAddCartArgs addCartArgs;

    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        int resultCode = -1;
        if (jsonObj)
        {
            // result of adding
            if (jsonObj->GetIntValue("result", &resultCode) && 0 == resultCode)
            {
                addCartArgs.succeeded = true;
            }

            // paid list
            JsonObjectSPtr items = jsonObj->GetSubObject("paid");
            if (items != 0)
            {
                int count = items->GetArrayLength();
                for (int i = 0; i < count; ++i)
                {
                    JsonObjectSPtr item = items->GetElementByIndex(i);
                    if (item == 0)
                    {
                        continue;
                    }
                    std::string item_str;
                    if (item->GetStringValue(&item_str))
                    {
                        addCartArgs.paid_books.push_back(item_str);
                    }
                }
            }

            // filter books in cart
            std::string books_str = task->GetPostValue("item");
            StringToStringList(books_str, addCartArgs.books, addCartArgs.paid_books);

            // add filtered books into cache. Here is a bug, should use filtered books
            if (addCartArgs.succeeded)
                AddBooksIntoCartCache((dynamic_cast<BookOrderDownloadTask*>(task))->GetBooks());
            addCartArgs.resultCode = resultCode;
        }
    }
    FireEvent(EventAddToCartUpdate, addCartArgs);
    return true;
}

bool BookStoreOrderManagerImpl::OnRemoveBooksFromCartUpdate(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;

    BookStoreOrderArgs removeBooksFromCartArgs;
    std::vector<std::string> emptyFilter;
    std::string books_str = task->GetPostValue("item");
    StringToStringList(books_str, removeBooksFromCartArgs.books, emptyFilter);

    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        //DebugPrintf(DLC_DIAGNOSTIC, "Remove Data:%s From Cart", task->GetAllPostDataAsString().c_str());
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        int resultCode = -1;
        if (jsonObj)
        {
            if (jsonObj->GetIntValue("result", &resultCode) && 0 == resultCode)
            {
                removeBooksFromCartArgs.succeeded = true;
                RemoveBooksFromCartCache((dynamic_cast<BookOrderDownloadTask*>(task))->GetBooks());
            }
            removeBooksFromCartArgs.resultCode = resultCode;
        }
    }
    FireEvent(EventRemoveFromCartUpdate, removeBooksFromCartArgs);
    return true;
}

bool BookStoreOrderManagerImpl::OnRemoveFavouriteUpdate(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;

    BookStoreOrderArgs removeFavouriteArgs;
    std::vector<std::string> emptyFilter;
    std::string books_str = task->GetPostValue("item");
    StringToStringList(books_str, removeFavouriteArgs.books, emptyFilter);
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        //DebugPrintf(DLC_DIAGNOSTIC, "Remove Data:%s", task->GetAllPostDataAsString().c_str());
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        int resultCode = -1;
        if (jsonObj)
        {
            if (jsonObj->GetIntValue("result", &resultCode) && 0 == resultCode)
            {
                removeFavouriteArgs.succeeded = true;
                RemoveFavouritesFromCache((dynamic_cast<BookOrderDownloadTask*>(task))->GetBooks());
            }
            removeFavouriteArgs.resultCode = resultCode;
        }
    }
    FireEvent(EventRemoveFavouriteUpdate, removeFavouriteArgs);
    return true;
}

bool BookStoreOrderManagerImpl::OnMoveFavouriteToCartUpdate(const EventArgs& args)
{
    if (OnAddBooksToCartUpdate(args))
    {
        // Update local favourite books
        FetchFavouriteBooks();
    }
	return true;
}

bool BookStoreOrderManagerImpl::OnMoveCartToFavouriteUpdate(const EventArgs& args)
{
    if (OnAddFavouriteUpdate(args))
    {
        // Update local cart books
        FetchBooksInCart();
    }
	return true;
}

bool BookStoreOrderManagerImpl::IsBookAddedToFavourite(const char* bookId)
{
    if (!CAccountManager::GetInstance()->IsLoggedIn())
    {
        return OfflineBookStoreManager::GetInstance()->IsBookAddedToFavourite(bookId);
    }
    return IsBookAdded(bookId, m_favouriteBooksLock, m_pFavouriteBooks);
}

bool BookStoreOrderManagerImpl::AddFavouritesIntoCache(const model::BookInfoList& books)
{
    if (AddBooksIntoContainer(books, m_favouriteBooksLock, m_pFavouriteBooks))
    {
        DataUpdateArgs favouritesCache;
        if (GetCachedFavouriteList(favouritesCache))
        {
            return OnListDataUpdate(favouritesCache, EventGetFavouriteUpdate);
        }
    }
    return false;
}

bool BookStoreOrderManagerImpl::RemoveFavouritesFromCache(const model::BookInfoList& books)
{
    if (RemoveBooksFromContainer(books, m_favouriteBooksLock, m_pFavouriteBooks))
    {
        DataUpdateArgs favouritesCache;
        if (GetCachedFavouriteList(favouritesCache))
        {
            return OnListDataUpdate(favouritesCache, EventGetFavouriteUpdate);
        }
    }
    return false;
}

bool BookStoreOrderManagerImpl::GetCachedFavouriteList(DataUpdateArgs& dst)
{
    if (!GetCachedBooksFromContainer(dst, m_favouriteBooksLock, m_pFavouriteBooks))
    {
        return OfflineBookStoreManager::GetInstance()->GetLocalFavouriteList(dst);
    }
    return true;
}

bool BookStoreOrderManagerImpl::IsBookAddedToCart(const char* bookId)
{
    if (!CAccountManager::GetInstance()->IsLoggedIn())
    {
        return OfflineBookStoreManager::GetInstance()->IsBookAddedToCart(bookId);
    }
    return IsBookAdded(bookId, m_cartBooksLock, m_pCartBooks);
}

bool BookStoreOrderManagerImpl::AddBooksIntoCartCache(const model::BookInfoList& books)
{
    if (AddBooksIntoContainer(books, m_cartBooksLock, m_pCartBooks))
    {
        DataUpdateArgs cachedCart;
        if (GetCachedBooksInCart(cachedCart))
        {
            return OnListDataUpdate(cachedCart, EventGetCartUpdate);
        }
    }
    return false;
}

bool BookStoreOrderManagerImpl::RemoveBooksFromCartCache(const model::BookInfoList& books)
{
    if (RemoveBooksFromContainer(books, m_cartBooksLock, m_pCartBooks))
    {
        DataUpdateArgs cachedCart;
        if (GetCachedBooksInCart(cachedCart))
        {
            return OnListDataUpdate(cachedCart, EventGetCartUpdate);
        }
    }
    return false;
}

bool BookStoreOrderManagerImpl::GetCachedBooksInCart(DataUpdateArgs& dst)
{
    if (!GetCachedBooksFromContainer(dst, m_cartBooksLock, m_pCartBooks))
    {
        return OfflineBookStoreManager::GetInstance()->GetLocalBooksInCart(dst);
    }
    return true;
}

bool BookStoreOrderManagerImpl::IsBookAdded(const char* bookId,
                                            pthread_mutex_t& mutex,
                                            std::tr1::shared_ptr<DataUpdateArgs>& container)
{
    AutoLock lock(&mutex);
    // Clear favourite list when logging out
    if (!CAccountManager::GetInstance()->IsLoggedIn())
    {
        container.reset();
        return false;
    }

    if (container == 0 || container->dataList.empty())
    {
        return false;
    }

    for (size_t i = 0; i < container->dataList.size(); ++i)
    {
        if (container->dataList[i] != 0 &&
            dynamic_cast<model::BookInfo*>(container->dataList[i].get())->GetBookId().compare(bookId) == 0)
        {
            return true;
        }
    }
    return false;
}

bool BookStoreOrderManagerImpl::AddBooksIntoContainer(const model::BookInfoList& books,
                                                      pthread_mutex_t& mutex,
                                                      std::tr1::shared_ptr<DataUpdateArgs>& container)
{
    AutoLock lock(&mutex);
    if (container == 0)
    {
        container.reset(new DataUpdateArgs());
    }

    model::BookInfoList::const_iterator iter = books.begin();
    for (; iter != books.end(); iter++)
    {
        bool found = false;
        for (size_t i = 0; i < container->dataList.size(); ++i)
        {
            if (container->dataList[i] != 0 &&
                dynamic_cast<model::BookInfo*>(container->dataList[i].get())->GetBookId().compare((*iter)->GetBookId()) == 0)
            {
                found = true;
                container->dataList[i] = *iter;
                break;
            }
        }
        if (!found)
        {
            container->dataList.push_back(*iter);
        }
    }
    return true;
}

bool BookStoreOrderManagerImpl::RemoveBooksFromContainer(const model::BookInfoList& books,
                                                         pthread_mutex_t& mutex,
                                                         std::tr1::shared_ptr<DataUpdateArgs>& container)
{
    AutoLock lock(&mutex);
    if (container == 0)
    {
        return false;
    }

    bool found = false;
    model::BookInfoList::const_iterator iter = books.begin();
    for (; iter != books.end(); iter++)
    {
        model::BasicObjectList::iterator ti = container->dataList.begin();
        for (; ti != container->dataList.end(); ++ti)
        {
            if ((*ti) != 0 &&
                dynamic_cast<model::BookInfo*>((*ti).get())->GetBookId().compare((*iter)->GetBookId()) == 0)
            {
                found = true;
                container->dataList.erase(ti);
                break;
            }
        }
    }
    return found;
}

bool BookStoreOrderManagerImpl::GetCachedBooksFromContainer(DataUpdateArgs& dst,
                                                            pthread_mutex_t& mutex,
                                                            std::tr1::shared_ptr<DataUpdateArgs>& container)
{
    AutoLock lock(&mutex);
    if (!CAccountManager::GetInstance()->IsLoggedIn())
    {
        container.reset();
        return false;
    }

    if (container == 0 || container->dataList.empty())
    {
        return false;
    }
    dst.dataList = container->dataList;
    dst.errorCode = container->errorCode;
    dst.handled = container->handled;
    dst.moreData = container->moreData;
    dst.startIndex = container->startIndex;
    dst.succeeded = container->succeeded;
    dst.total = container->total;
    return true;
}

FetchDataResult BookStoreOrderManagerImpl::FetchHideBookList(int start, int length)
{
    return m_hideBookListFetcher.FetchData(start, length);
}

bool BookStoreOrderManagerImpl::AddHideBookIntoCache(const model::BookInfoList& books)
{
    return AddBooksIntoContainer(books, m_hideBooksLock, m_pHidetBooks);
}

bool BookStoreOrderManagerImpl::RemoveHideBookFromCache(const model::BookInfoList& books)
{
    return RemoveBooksFromContainer(books, m_hideBooksLock, m_pHidetBooks);
}

bool BookStoreOrderManagerImpl::IsBookInHideList(const char* bookId)
{
    return IsBookAdded(bookId, m_hideBooksLock, m_pHidetBooks);
}

model::BookInfoSPtr BookStoreOrderManagerImpl::GetBookInfoFromHideList(const char* bookId)
{
	AutoLock lock(&m_hideBooksLock);
	if(m_pHidetBooks.get())
	{
		const model::BasicObjectList bookList = m_pHidetBooks->dataList;
		if (!bookList.empty())
	    {
	        for (size_t i = 0; i < bookList.size(); ++i)
		    {
		    	if(bookList[i].get())
	    		{
	    			const model::BookInfoSPtr bookInfo(dynamic_cast<model::BookInfo*>(bookList[i]->Clone()));
			        if (bookInfo.get() && bookInfo->GetBookId().compare(bookId) == 0)
			        {
			        	return bookInfo;
			        }
	    		}
		    }
	    }
	}
	return model::BookInfoSPtr();
    
}

bool BookStoreOrderManagerImpl::OnHideBookFinished(const EventArgs& args)
{
	const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    BookStoreOrderArgs hideBookArgs;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        int resultCode = -1;
        if (jsonObj)
        {
            if (jsonObj->GetIntValue("result", &resultCode) && 0 == resultCode)
            {
            	const model::BookInfoList& books = (dynamic_cast<BookOrderDownloadTask*>(task))->GetBooks();
                hideBookArgs.succeeded = true;
				for(std::vector<model::BookInfoSPtr>::const_iterator itr = books.begin(); itr != books.end(); itr++)
				{
					hideBookArgs.books.push_back((*itr)->GetBookId());
				}
				AddHideBookIntoCache((dynamic_cast<BookOrderDownloadTask*>(task))->GetBooks());
            }
            hideBookArgs.resultCode = resultCode;
        }
    }
    FireEvent(EventHideBookFinished, hideBookArgs);
    return true;
}

bool BookStoreOrderManagerImpl::OnRevealHideBookFinished(const EventArgs& args)
{
	const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    BookStoreOrderArgs revealHideBookArgs;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result = task->GetString();
        JsonObjectSPtr jsonObj = JsonObject::CreateFromString(result.c_str());
        int resultCode = -1;
        if (jsonObj)
        {
            if (jsonObj->GetIntValue("result", &resultCode) && 0 == resultCode)
            {
            	const model::BookInfoList& books = (dynamic_cast<BookOrderDownloadTask*>(task))->GetBooks();
                revealHideBookArgs.succeeded = true;
				for(std::vector<model::BookInfoSPtr>::const_iterator itr = books.begin(); itr != books.end(); itr++)
				{
					revealHideBookArgs.books.push_back((*itr)->GetBookId());
				}
				RemoveHideBookFromCache(books);
            }
            revealHideBookArgs.resultCode = resultCode;
        }
    }
	FireEvent(EventRevealHideBookFinished, revealHideBookArgs);
    return true;
}

FetchDataResult BookStoreOrderManagerImpl::HideBook(const model::BookInfoList& books)
{
	if (books.empty())
	{
		return FDR_FAILED;
	}
	DownloadTaskSPtr task = DownloadTaskBuilder::BuildHideBookTask(books);
	if (!task)
	{
		return FDR_FAILED;
	}
	SubscribeDownloadEvent(task, &BookStoreOrderManagerImpl::OnHideBookFinished);
	DownloadManager::GetInstance()->AddTask(task);
	return FDR_PENDING;
}

FetchDataResult BookStoreOrderManagerImpl::RevealHideBook(const model::BookInfoList& books)
{
	if (books.empty())
	{
		return FDR_FAILED;
	}
	DownloadTaskSPtr task = DownloadTaskBuilder::BuildRevealHideBookTask(books);
	if (!task)
	{
		return FDR_FAILED;
	}
	SubscribeDownloadEvent(task, &BookStoreOrderManagerImpl::OnRevealHideBookFinished);
	DownloadManager::GetInstance()->AddTask(task);
	return FDR_PENDING;
}

bool BookStoreOrderManagerImpl::OnHideBookListUpdate(const EventArgs& args)
{
    const DataUpdateArgs& dataUpdateArgs = (const DataUpdateArgs&)args;
    if (dataUpdateArgs.succeeded)
    {
        AutoLock lock(&m_hideBooksLock);
        // Update cache
        m_pHidetBooks.reset(new DataUpdateArgs(dataUpdateArgs));
    }
	return true;
}

FetchDataResult BookStoreOrderManagerImpl::FetchSpecialEvents()
{
    return m_specialEventsFetcher.FetchData(0, 100);
}

bool BookStoreOrderManagerImpl::OnGetSpecialEventsUpdate(const EventArgs& args)
{
    const DataUpdateArgs& dataUpdateArgs = (const DataUpdateArgs&)args;
    if (dataUpdateArgs.succeeded)
    {
        AutoLock lock(&m_specialEventsLock);
        m_pSpecialEvents.reset(new DataUpdateArgs(dataUpdateArgs));
    }
	return OnListDataUpdate(args, EventGetSpecialEventUpdate);;
}

SpecialEventList BookStoreOrderManagerImpl::GetSpecialEventByTime(double refTime)
{
    SpecialEventList result;
    AutoLock lock(&m_specialEventsLock);
    if (m_pSpecialEvents != 0 && !m_pSpecialEvents->dataList.empty())
    {
        for (size_t i = 0; i < m_pSpecialEvents->dataList.size(); ++i)
        {
            if (m_pSpecialEvents->dataList[i] &&
			    (m_pSpecialEvents->dataList[i]->GetObjectType() == OT_SPECIAL_EVENTS))
            {
                SpecialEvent* specialEvent = (SpecialEvent*)(m_pSpecialEvents->dataList[i].get());
                if (specialEvent->GetStartTime() <= refTime && specialEvent->GetEndTime() >= refTime)
                {
                    result.push_back(SpecialEventSPtr((SpecialEvent*)(m_pSpecialEvents->dataList[i]->Clone())));
                }
            }
        }
    }
    return result;
}

}
}
