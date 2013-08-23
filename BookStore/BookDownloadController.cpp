#include <tr1/functional>
#include "BookStore/BookDownloadController.h"
#include "BookStore/BookStoreInfoManager.h"
#include "Utility/SystemUtil.h"
#include "CommonUI/UIUtility.h"
#include "I18n/StringManager.h"
#include "Wifi/WifiManager.h"
#include "Common/CAccountManager.h"

using namespace dk::utility;
using namespace dk::account;
namespace dk
{

namespace bookstore
{
static const char* HOME_FOLDER = "/mnt/us/";
static const unsigned long SAFE_SPACE = 2000; // 2M
const char* BookDownloadController::EventDownloadBookUpdated = "EventDownloadBookUpdated";
const char* BookDownloadController::EventDRMUpdated = "EventDRMUpdated";

BookDownloadController::BookDownloadController()
    : current_book_for_queue_downloading_(BookInfoSPtr())
    , downloading_progress_(-1)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventDrmCertUpdate,
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&BookDownloadController::OnDRMCertUpdated),
            this,
            std::tr1::placeholders::_1));

    SubscribeMessageEvent(
        IDownloader::EventDownloadProgressUpdate,
        *IDownloader::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&BookDownloadController::OnDownloadStatusUpdated),
            this,
            std::tr1::placeholders::_1));
}

BookDownloadController::~BookDownloadController()
{
}

FetchDataResult BookDownloadController::BatchUpdateBooks(int request_state)
{
    if (!AllowDownload())
    {
        UIUtility::SetScreenTips(StringManager::GetStringById(DOWNLOAD_NOT_ENOUGH_SPACE));
        return FDR_FAILED;
    }

    model::BookInfoList purchased_books = BookStoreInfoManager::GetInstance()->GetCachedPurchasedBookList();
    if (purchased_books.empty())
    {
        return FDR_EMPTY;
    }
    FetchDataResult ret = FDR_SUCCESSED;
    int updates_num = 0;
    int failing_num = 0;
    for (size_t i = 0; i < purchased_books.size(); ++i)
    {
        model::BookInfoSPtr book_info = purchased_books[i];
        int status = BookStoreInfoManager::GetInstance()->GetBookStatus(book_info);
        if (status & request_state)
        {
            FetchDataResult single_ret = FDR_SUCCESSED;
            updates_num++;
            if (!book_info->IsDrm())
            {
                BookStoreInfoManager::GetInstance()->WriteInfoFile(book_info, book_info->GetOrderId().c_str());
                single_ret = BookStoreInfoManager::GetInstance()->FetchBook(book_info);
            }
            else
            {
                single_ret = BookStoreInfoManager::GetInstance()->FetchDrmCert(book_info->GetBookId().c_str(),
                                 book_info->GetRevision().c_str(),
                                 book_info->GetOrderId().c_str());
            }
            if (single_ret == FDR_PENDING)
            {
                ret = FDR_PENDING;
            }
            if (single_ret == FDR_FAILED)
            {
                failing_num++;
            }
        }
    }
    if (updates_num == 0)
    {
        return FDR_EMPTY;
    }
    if (updates_num == failing_num)
    {
        return FDR_FAILED;
    }
    return ret;
}

FetchDataResult BookDownloadController::ExecuteDownload(BookInfoSPtr book)
{
    if (!AllowDownload())
    {
        UIUtility::SetScreenTips(StringManager::GetStringById(DOWNLOAD_NOT_ENOUGH_SPACE));
        return FDR_FAILED;
    }

    FetchDataResult ret = FDR_FAILED;
    if (!book->IsDrm())
    {
        BookStoreInfoManager::GetInstance()->WriteInfoFile(book, book->GetOrderId().c_str());
        ret = BookStoreInfoManager::GetInstance()->FetchBook(book);
    }
    else
    {
        ret = BookStoreInfoManager::GetInstance()->FetchDrmCert(book->GetBookId().c_str(),
            book->GetRevision().c_str(),
            book->GetOrderId().c_str());
    }

    if (ret == FDR_FAILED)
    {
        NotifyBookDownloadStatus(book, IDownloadTask::FAILED);
    }
    else if (ret == FDR_SUCCESSED && !current_book_for_queue_downloading_->IsDrm())
    {
        NotifyBookDownloadStatus(book, IDownloadTask::DONE);
    }
    else
    {
        NotifyBookDownloadStatus(book, book->IsDrm() ?
            IDownloadTask::FETCHING_DRM : IDownloadTask::WORKING);
    }
    return ret;
}

FetchDataResult BookDownloadController::StartDownload()
{
    if (current_book_for_queue_downloading_ != 0)
    {
        // it means there is pending task
        return FDR_PENDING;
    }

    if (download_queue_.empty())
    {
        return FDR_EMPTY;
    }

    if (!AllowDownload())
    {
        download_queue_.clear();
        UIUtility::SetScreenTips(StringManager::GetStringById(DOWNLOAD_NOT_ENOUGH_SPACE));
        return FDR_FAILED;
    }

    FetchDataResult ret = FDR_FAILED;
    current_book_for_queue_downloading_ = download_queue_.front();
    download_queue_.pop_front();
    if (!current_book_for_queue_downloading_->IsDrm())
    {
        BookStoreInfoManager::GetInstance()->WriteInfoFile(current_book_for_queue_downloading_, current_book_for_queue_downloading_->GetOrderId().c_str());
        ret = BookStoreInfoManager::GetInstance()->FetchBook(current_book_for_queue_downloading_);
    }
    else
    {
        ret = BookStoreInfoManager::GetInstance()->FetchDrmCert(current_book_for_queue_downloading_->GetBookId().c_str(),
                        current_book_for_queue_downloading_->GetRevision().c_str(),
                        current_book_for_queue_downloading_->GetOrderId().c_str());
    }

    if (ret == FDR_FAILED)
    {
        NotifyBookDownloadStatus(current_book_for_queue_downloading_, IDownloadTask::FAILED);

        // start downloading of next book
        current_book_for_queue_downloading_ = BookInfoSPtr();
        return StartDownload();
    }
    else if (ret == FDR_SUCCESSED && !current_book_for_queue_downloading_->IsDrm())
    {
        NotifyBookDownloadStatus(current_book_for_queue_downloading_, IDownloadTask::DONE);

        // this book has been downloaded, start downloading of next book
        current_book_for_queue_downloading_ = BookInfoSPtr();
        return StartDownload();
    }
    NotifyBookDownloadStatus(current_book_for_queue_downloading_, current_book_for_queue_downloading_->IsDrm() ?
        IDownloadTask::FETCHING_DRM : IDownloadTask::WORKING);
    return ret;
}

void BookDownloadController::NotifyBookDownloadStatus(BookInfoSPtr book,
                                                      int state,
                                                      int progress,
                                                      string task_id)
{
    BookDownloadStateUpdateArgs args;
    if (book != 0)
    {
        args.book_id  = book->GetBookId();
    }
    args.state    = state;
    args.progress = progress;
    args.task_id  = task_id;
    FireEvent(EventDownloadBookUpdated, args);
}

FetchDataResult BookDownloadController::AddBook(BookInfoSPtr book)
{
    if (book == 0)
    {
        return FDR_FAILED;
    }

    BookInfoSPtr same_book = GetBookFromDownloadListByID(book->GetBookId());
    if (same_book == 0)
    {
        model::BookInfoSPtr cloned_book(dynamic_cast<model::BookInfo*>(book->Clone()));
        download_queue_.push_back(book);
    }

    if (current_book_for_queue_downloading_ == 0)
    {
        // there is no working download task, then start new download
        return StartDownload();
    }
    return FDR_PENDING;
}

FetchDataResult BookDownloadController::RemoveBook(BookInfoSPtr book)
{
    // TODO.
    return FDR_FAILED;
}

bool BookDownloadController::StopAll()
{
    if (current_book_for_queue_downloading_ != 0)
    {
        NotifyBookDownloadStatus(current_book_for_queue_downloading_, IDownloadTask::CANCELED);
        current_book_for_queue_downloading_ = BookInfoSPtr();
    }

    IDownloader * downloader = IDownloader::GetInstance();
    downloader->StopDownload(IDownloadTask::CANCELED);
    downloader->DeleteTasks();
    download_tasks_overview_ = downloader->UpdateAllDownloadTaskNums();
    downloading_progress_= -1;
    current_downloading_id_.clear();

    if (!download_queue_.empty())
    {
        std::deque<BookInfoSPtr>::iterator iter = download_queue_.begin();
        for(; iter != download_queue_.end(); iter++)
        {
            NotifyBookDownloadStatus(*iter, IDownloadTask::CANCELED);
        }
        download_queue_.clear();
    }
    else
    {
        NotifyBookDownloadStatus(BookInfoSPtr(), IDownloadTask::CANCELED);
    }
    return true;
}

bool BookDownloadController::OnDownloadStatusUpdated(const EventArgs& args)
{
    const DownloadUpdateEventArgs& download_args = (const DownloadUpdateEventArgs&)args;
    string url_id = download_args.taskId;
    int current_progress = download_args.percentage;
    int state = download_args.state;
    if (state & IDownloadTask::FAILED && !AllowDownload())
    {
        UIUtility::SetScreenTips(StringManager::GetStringById(DOWNLOAD_NOT_ENOUGH_SPACE));
        StopAll();
        return true;
    }

    IDownloader * downloader = IDownloader::GetInstance();
    download_tasks_overview_ = downloader->UpdateAllDownloadTaskNums();

    if (current_progress >= 0 && (state & IDownloadTask::WORKING))
    {
        downloading_progress_ = current_progress;
        current_downloading_id_ = url_id;
    }
    if (current_downloading_id_ == url_id &&
        (state & (IDownloadTask::CANCELED |
                  IDownloadTask::PAUSED |
                  IDownloadTask::FAILED |
                  IDownloadTask::WAITING_QUEUE)))
    {
        downloading_progress_ = -1;
    }

    BookInfoSPtr empty;
    NotifyBookDownloadStatus(empty, state, current_progress, url_id);
    return true;
}

bool BookDownloadController::OnDRMCertUpdated(const EventArgs& args)
{
    const DrmCertUpdateArgs& drm_args = (const DrmCertUpdateArgs&)args;
    if (drm_args.succeeded)
    {
        model::BookInfoSPtr drm_book = BookStoreInfoManager::GetInstance()->GetPurchasedBookInfo(drm_args.bookId.c_str());
        // Download book in purchase list
        // If it's not in purchase list, the downloading will be handled in book price button controller.
        if (drm_book != 0 && !drm_book->GetFileUrl().empty())
        {
            BookStoreInfoManager::GetInstance()->FetchBook(drm_book);
            return true;
        }
    }
    else
    {
        if (!AllowDownload())
        {
            UIUtility::SetScreenTips(StringManager::GetStringById(DOWNLOAD_NOT_ENOUGH_SPACE));
            StopAll();
        }
        else
        {
            UIUtility::SetScreenTips(StringManager::GetStringById(DOWNLOAD_DRM_ERROR));
        }
        return true;
    }

    BookDownloadDRMUpdateArgs forward_args;
    forward_args.drm_handled = false;
    forward_args.succeeded = drm_args.succeeded;
    forward_args.bookId = drm_args.bookId;
    forward_args.bookRevision = drm_args.bookRevision;
    forward_args.bookTitle = drm_args.bookTitle;
    forward_args.cert = drm_args.cert;
    forward_args.magicId = drm_args.magicId;
    forward_args.userId = drm_args.userId;
    forward_args.orderId = drm_args.orderId;
    FireEvent(EventDRMUpdated, forward_args);
    return true;
}

BookInfoSPtr BookDownloadController::GetBookFromDownloadListByID(const string& id)
{
    std::deque<BookInfoSPtr>::iterator iter = download_queue_.begin();
    for(; iter != download_queue_.end(); iter++)
    {
        if ((*iter) != 0 && (*iter)->GetBookId().compare(id) == 0)
        {
            return *iter;
        }
    }
    return BookInfoSPtr();
}

BookInfoSPtr BookDownloadController::GetBookFromDownloadListByURL(const string& url)
{
    std::deque<BookInfoSPtr>::iterator iter = download_queue_.begin();
    for(; iter != download_queue_.end(); iter++)
    {
        if (*iter != 0)
        {
            std::string book_url_id = BookStoreInfoManager::BuildUrlIdForBook((*iter)->GetBookId().c_str());
            std::string book_trail_id = BookStoreInfoManager::BuildUrlIdForTrialVersion((*iter)->GetBookId().c_str());
            if (book_url_id.compare(url) == 0 || book_trail_id.compare(url) == 0)
            {
                return *iter;
            }
        }
    }
    return BookInfoSPtr();
}

int BookDownloadController::GetWorkingAndWaitingDownloads()
{
    if (!download_tasks_overview_.empty() &&
        download_tasks_overview_.find(IDownloadTask::BOOK) != download_tasks_overview_.end())
    {
        return GetDownloadTaskNums(download_tasks_overview_[IDownloadTask::BOOK], IDownloadTask::WORKING) +
               GetDownloadTaskNums(download_tasks_overview_[IDownloadTask::BOOK], IDownloadTask::WAITING) +
               download_queue_.size();
               
    }
    return download_queue_.size();
}

int BookDownloadController::GetWaitingDownloads()
{
    if (!download_tasks_overview_.empty() &&
        download_tasks_overview_.find(IDownloadTask::BOOK) != download_tasks_overview_.end())
    {
        return GetDownloadTaskNums(download_tasks_overview_[IDownloadTask::BOOK], IDownloadTask::WAITING) +
               download_queue_.size();
               
    }
    return download_queue_.size();
}

int BookDownloadController::GetBookDownloadStatus(BookInfoSPtr book)
{
    if (book == 0)
    {
        return BS_UNKNOWN;
    }
    if (current_book_for_queue_downloading_ != 0 &&
        current_book_for_queue_downloading_->GetBookId().compare(book->GetBookId()) == 0)
    {
        return BS_DOWNLOADING;
    }
    if (GetBookFromDownloadListByID(book->GetBookId()) != 0)
    {
        return BS_WAITING;
    }
    return BS_UNKNOWN;
}

int BookDownloadController::GetAllActiveDownloads()
{
    return GetActiveDownloadsNumByType(IDownloadTask::BOOK) +
           GetActiveDownloadsNumByType(IDownloadTask::UPGRADEPACKAGE);
}

int BookDownloadController::GetActiveDownloadsNumByType(int type)
{
    if (download_tasks_overview_.empty())
    {
        return 0;
    }

    if (download_tasks_overview_.find(type) != download_tasks_overview_.end())
    {
        return GetDownloadTaskNums(download_tasks_overview_[type], IDownloadTask::WAITING) +
               GetDownloadTaskNums(download_tasks_overview_[type], IDownloadTask::WORKING) +
               GetDownloadTaskNums(download_tasks_overview_[type], IDownloadTask::PAUSED) +
               GetDownloadTaskNums(download_tasks_overview_[type], IDownloadTask::UPDATED) +
               GetDownloadTaskNums(download_tasks_overview_[type], IDownloadTask::CURL_DONE);
    }
    return 0;
}

bool BookDownloadController::AllowDownload()
{
    unsigned long left_space = SystemUtil::FreeSpace(HOME_FOLDER) / 1024;
	if(left_space < SAFE_SPACE)
    {
        return false;
    }

    bool wifi_connected = WifiManager::GetInstance()->IsConnected();
    if(!wifi_connected)
    {
        return false;
    }

    if (!CAccountManager::GetInstance()->IsLoggedIn())
    {
        return false;
    }
    return true;
}

}

}
