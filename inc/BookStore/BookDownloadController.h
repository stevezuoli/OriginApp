#ifndef __BOOKSTORE_BOOK_DOWNLOAD_CONTROLLER_H__
#define __BOOKSTORE_BOOK_DOWNLOAD_CONTROLLER_H__

#include <deque>
#include "GUI/EventSet.h"
#include "GUI/EventArgs.h"
#include "GUI/EventListener.h"
#include "BookStore/BookInfo.h"
#include "BookStore/BookStoreTypes.h"
#include "BookStore/BookStoreInfoManager.h"
#include "DownloadManager/IDownloader.h"

using namespace std;
using namespace dk::bookstore::model;

namespace dk
{

namespace bookstore
{

class BookDownloadStateUpdateArgs: public EventArgs
{
public:
    BookDownloadStateUpdateArgs()
        : state(IDownloadTask::NONE)
        , progress(-1)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new BookDownloadStateUpdateArgs(*this);
    }

public:
    string book_id;
    string task_id;
    int    state;
    int    progress;
};

class BookDownloadDRMUpdateArgs: public DrmCertUpdateArgs
{
public:
    BookDownloadDRMUpdateArgs()
        : DrmCertUpdateArgs()
        , drm_handled(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new BookDownloadDRMUpdateArgs(*this);
    }

public:
    bool drm_handled;
};

class BookDownloadController: public EventSet
                            , public EventListener
{
    BookDownloadController();
    BookDownloadController(const BookDownloadController& right);

public:
    static const char* EventDownloadBookUpdated;
    static const char* EventDRMUpdated;

public:
    static BookDownloadController* GetInstance() {
        static BookDownloadController instance;
        return &instance;
    }
    virtual ~BookDownloadController();

    FetchDataResult BatchUpdateBooks(int request_state);
    FetchDataResult AddBook(BookInfoSPtr book);
    FetchDataResult ExecuteDownload(BookInfoSPtr book);
    FetchDataResult RemoveBook(BookInfoSPtr book);
    bool StopAll();

    int GetBookDownloadStatus(BookInfoSPtr book);
    int GetWorkingAndWaitingDownloads();
    int GetWaitingDownloads();
    int GetAllActiveDownloads();
    int GetCurrentDownloadingProgress() const { return downloading_progress_; }
    // Event handlers

private:
    bool OnDownloadStatusUpdated(const EventArgs& args);
    bool OnDRMCertUpdated(const EventArgs& args);

private:
    BookInfoSPtr GetBookFromDownloadListByID(const string& id);
    BookInfoSPtr GetBookFromDownloadListByURL(const string& url);
    FetchDataResult StartDownload();

    void NotifyBookDownloadStatus(BookInfoSPtr book, int state, int progress = -1, string task_id = string());
    int GetActiveDownloadsNumByType(int type);
    bool AllowDownload();

private:
    std::deque<BookInfoSPtr> download_queue_;
    BookInfoSPtr             current_book_for_queue_downloading_;
    AllDownloadTaskNums      download_tasks_overview_;
    std::string              current_downloading_id_;
    int                      downloading_progress_;
};

};

};

#endif
