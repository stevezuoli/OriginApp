#ifndef __BOOKSTORE_OFFLINEBOOKSTOREMANAGER_H__
#define __BOOKSTORE_BOOKSTOREINFOMANAGER_H__

#include <tr1/memory>
#include <deque>
#include <vector>
#include "Mutex.h"
#include "DKXManager/Serializable/DK_Serializable.h"
#include <map>
#include "BookStore/BookStoreErrno.h"
#include "BookStore/DataUpdateArgs.h"
#include "BookStore/BookInfo.h"
#include "GUI/MessageEventArgs.h"

#include "GUI/EventListener.h"
#include "BookStore/ReadingBookInfo.h"

using namespace std;

namespace dk
{

namespace bookstore
{


class OfflineBookStoreTask : public DK_Serializable
{

public:
    OfflineBookStoreTask();
    virtual ~OfflineBookStoreTask();

public:
    bool SetInfo(const std::string& name,const std::string& value);
    string GetInfo(const std::string& name) const;

    virtual bool WriteToXML(XMLDomNode& xmlNode) const;
    virtual bool ReadFromXml(const XMLDomNode& xmlNode);

private:
    std::map<string,string> m_mInfoMap;
};

typedef std::tr1::shared_ptr<OfflineBookStoreTask> OfflineBookStoreTaskPtr;

class OfflineBookStoreManager : public EventListener
{
private:
    OfflineBookStoreManager();
    OfflineBookStoreManager(const OfflineBookStoreManager& right);
public:
    static OfflineBookStoreManager* GetInstance();
    virtual ~OfflineBookStoreManager();

    bool Save();
    bool Load();
    bool ReportedBookBug(const char* bookId,
                         const char* bookName,
                         const char* bookRevision,
                         const char* position,
                         const char* ref_chapter,
                         const char* reference,
                         const char* suggestion,
                         bool bConvertCht = false);
    bool RetrieveBookBug(std::string& bookId,
                         std::string& bookName,
                         std::string& bookRevision,
                         std::string& position,
                         std::string& ref_chapter,
                         std::string& reference,
                         std::string& suggestion,
                         bool& bConvertCht);

    bool AddBookWaitUpload(const char* bookId,
                           const char* bookName,
                           const char* bookRevision);
    bool DelBookWaitUpload(const char* bookId);
    bool Send();
    bool OnPutReadingDataUpdate(const EventArgs& args);
    // Local Cart
    BookStoreErrno AddFavouriteBooks(const model::BookInfoList& books);
    BookStoreErrno RemoveFavouriteBooks(const model::BookInfoList& books);
    BookStoreErrno MoveFavouriteBooksToCart(const model::BookInfoList& books);
    bool IsBookAddedToFavourite(const char* bookId);
    bool GetLocalFavouriteList(DataUpdateArgs& dst);
    BookStoreErrno UpdateCartByCloud(const DataUpdateArgs& cloudList,
                                     model::BookInfoList& toSendList);

    BookStoreErrno AddBooksToCart(const model::BookInfoList& books);
    BookStoreErrno RemoveBooksFromCart(const model::BookInfoList& books);
    BookStoreErrno MoveCartBooksToFavourite(const model::BookInfoList& books);
    bool IsBookAddedToCart(const char* bookId);
    bool GetLocalBooksInCart(DataUpdateArgs& dst);
    BookStoreErrno UpdateFavouritesByCloud(const DataUpdateArgs& cloudList,
                                           model::BookInfoList& toSendList);

private:
    enum ContainerError
    {
        CONTAINER_OK = 0,
        CONTAINER_EMPTY,
        CONTAINER_FULL,
        CONTAINER_NOT_FOUND
    };
    bool LoadCorrections();
    bool SaveCorrections();
    bool SendCorrections();

    bool LoadBooksWaitUploadNewNote();
    bool SaveBooksWaitUploadNewNote();
    bool SendBooksWaitUploadNewNote();
    bool IsBookInWaitUploadQueue(const char* bookId, const char* bookName);
    bool StartSendBookNotes(const EventArgs& args);
    bool GenerateAndPostReadingBookInfo(const char* bookId, const char* revision);

    // For Cart & Favourites
    bool IsBookAdded(const char* bookId,
                     std::tr1::shared_ptr<DataUpdateArgs>& container);
    bool IsBookAdded(const char* bookId,
                     const DataUpdateArgs& container);
    bool IsBookAdded(const char* bookId,
                     pthread_mutex_t& mutex,
                     std::tr1::shared_ptr<DataUpdateArgs>& container);
    ContainerError AddBooksIntoContainer(const model::BookInfoList& books,
                                         pthread_mutex_t& mutex,
                                         std::tr1::shared_ptr<DataUpdateArgs>& container,
                                         dk::bookstore::model::ObjectType subType = dk::bookstore::model::OT_BOOK);
    ContainerError RemoveBooksFromContainer(const model::BookInfoList& books,
                                            pthread_mutex_t& mutex,
                                            std::tr1::shared_ptr<DataUpdateArgs>& container);
    bool GetCachedBooksFromContainer(DataUpdateArgs& dst,
                                     pthread_mutex_t& mutex,
                                     std::tr1::shared_ptr<DataUpdateArgs>& container);

    ContainerError updateByCloud(const DataUpdateArgs& cloudList,
                                 int sizeLimit,
                                 pthread_mutex_t& mutex,
                                 std::tr1::shared_ptr<DataUpdateArgs>& container,
                                 model::BookInfoList& toSendList);
    void updateLocalCart();
    void updateLocalFavourites();

    bool SaveLocalCart();
    bool LoadLocalCart();
    bool SaveLocalFavourites();
    bool LoadLocalFavourites();
    OfflineBookStoreTaskPtr GetOfflineTaskByBook(model::BookInfo* bookInfo);
    model::BasicObjectSPtr GetBookByOfflineTask(OfflineBookStoreTaskPtr task);
    bool GetTasksFromBookList(const model::BasicObjectList& books,
                              std::vector<OfflineBookStoreTaskPtr>& bookTasks);

    bool GetBookListFromTasks(const std::vector<OfflineBookStoreTaskPtr>& bookTasks,
                              model::BasicObjectList& books);
private:
    static const char* CORRECTION_ITEM;
    static const char* BOOK_ITEM;
    static const char* LOCAL_CART_ITEM;
    static const char* LOCAL_FAVOURITES_ITEM;

private:
    std::string m_strCorrectionPath;
    std::string m_strBooksWaitUploadPath;
    std::string m_strCartPath;
    std::string m_strFavouritesPath;

    pthread_mutex_t m_cacheLock;
    pthread_mutex_t m_bookLock;
    std::deque<OfflineBookStoreTaskPtr> m_offlineCorrectionTasks;

    // offline cart
    pthread_mutex_t m_favouriteBooksLock;
    std::tr1::shared_ptr<DataUpdateArgs> m_offlineFavouriteBooks;

    pthread_mutex_t m_cartBooksLock;
    std::tr1::shared_ptr<DataUpdateArgs> m_offlineCartBooks;

    std::vector<OfflineBookStoreTaskPtr> m_booksWaitUpload;
};

} //namespace bookstore

} //namespace dk

#endif
