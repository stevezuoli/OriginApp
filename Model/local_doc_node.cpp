#include <limits.h>
#include <algorithm>
#include <map>

#include "GUI/UIContainer.h"
#include "GUI/UIMenu.h"
#include "GUI/GUIHelper.h"
#include "GUI/UISizer.h"
#include "GUI/GUISystem.h"

#include "interface.h"
#include <assert.h>
#include <tr1/functional>
#include "drivers/DeviceInfo.h"

#include "Model/local_doc_node.h"
#include "Model/cloud_filesystem_tree.h"
#include "Model/local_bookstore_category_node.h"
#include "Model/local_bookstore_node.h"

#include "BookStore/LocalCategoryManager.h"
#include "Utility/PathManager.h"
#include "Utility/ImageManager.h"
#include "I18n/StringManager.h"

using namespace dk::bookstore;
using namespace dk::utility;
using namespace xiaomi;

namespace dk {

namespace document_model {

FileNode::FileNode(Node * parent, PCDKFile file_info)
        : Node(parent)
        , file_info_(file_info)
        , size_(file_info->GetFileSize())
        , uploading_progress_(-1.0)
        , uploading_state_(IDownloadTask::NONE)
{
    mutableAbsolutePath() = file_info->GetFilePath();
    mutableName() = PathManager::GetFileName(file_info->GetFilePath());
    updateDisplayName();

    mutableType() = file_info->IsDuoKanBook() ? NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK : NODE_TYPE_FILE_LOCAL_DOCUMENT;
    mutableLastRead() = file_info->GetFileLastReadTime();
    mutableCoverPath() = file_info->GetCoverImagePath();
    mutableId() = file_info->GetBookID();
    mutableCreateTime() = file_info->GetFileAddOrder();
    status_ = NODE_LOCAL;
    updateByUploadingTask();
}

FileNode::~FileNode()
{
}

/// Update all information.
void FileNode::update()
{
    if (file_info_ != 0)
    {
        file_info_->SyncFile();
    }
    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    HRESULT ret = file_manager->FileDkpProxy(file_info_, GETFILEINFO);
    if (SUCCEEDED(ret))
    {
        mutableAbsolutePath() = file_info_->GetFilePath();
        mutableName() = PathManager::GetFileName(file_info_->GetFilePath());
        updateDisplayName();

        mutableType() = file_info_->IsDuoKanBook() ?
            NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK : NODE_TYPE_FILE_LOCAL_DOCUMENT;
        mutableLastRead() = file_info_->GetFileLastReadTime();
        mutableCoverPath() = file_info_->GetCoverImagePath();
        mutableId() = file_info_->GetBookID();
        mutableCreateTime() = file_info_->GetFileAddOrder();

        size_ = file_info_->GetFileSize();
        updateByUploadingTask();
    }
}

void FileNode::updateDisplayName()
{
    mutableDisplayName() = file_info_->GetFileName();
    mutableGbkName() = file_info_->GetGbkName();
}

bool FileNode::rename(const string& new_name, string& error_msg)
{
    // TODO. Implement Me
    return false;
}

bool FileNode::remove(bool delete_local_files_if_possible, bool exec_now)
{
    if (status() & NODE_IS_UPLOADING)
    {
        return false;
    }

    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    if(0 == file_manager)
    {
        return false;
    }

    if (!file_manager->DelFile(absolutePath()))
    {
        return false;
    }

    return removeFromCategory();
}

bool FileNode::removeFromCategory()
{
    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    if (type() == NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK)
    {
        // delete file by book id
        if (!LocalCategoryManager::RemoveBookFromCategory(parent()->absolutePath().c_str(),
                                                          PathManager::GetFileNameWithoutExt(name().c_str()).c_str()))
        {
            return false;
        }

    }
    file_manager->FireFileListChangedEvent();
    return true;
}

void FileNode::sync()
{
    if (file_info_ != 0)
    {
        file_info_->SyncFile();
    }
}

SPtr<ITpImage> FileNode::getInitialImage()
{
    SPtr<ITpImage> init_img = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_FOLDER);
    if (file_info_ != 0)
    {
        switch(file_info_->GetFileFormat())
        {
        case DFF_Text:
            {
                init_img = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_TXT);
            }
            break;
        case DFF_ElectronicPublishing:
            {
                if (isDuokanBook())
                {
                    init_img = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_DUOKAN);
                }
                else
                {
                    init_img = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_EPUB);
                }
            }
            break;
        case DFF_MobiPocket:
            {
                init_img = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_MOBI);
            }
            break;
        case DFF_PortableDocumentFormat:
            {
                init_img = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_PDF);
            }
            break;
        case DFF_ZipFile:
        case DFF_RoshalArchive:
            {
                init_img = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_ZIP);
            }
            break;
        default:
            return SPtr<ITpImage>();
        }
    }
    return init_img;
}

const string FileNode::artist() const
{
    return file_info_->GetFileArtist();
}

bool FileNode::isTrialBook() const
{
    return file_info_->GetIsTrialBook();
}

DkFileFormat FileNode::fileFormat()
{
    return file_info_->GetFileFormat();
}

bool FileNode::isDuokanBook() const
{
    return file_info_->IsDuoKanBook();
}

bool FileNode::satisfy(int status_filter)
{
    if (status_filter == NODE_NONE)
    {
        return true;
    }

    // file must not be null
    if (file_info_ == 0)
    {
        return false;
    }

    string name = PathManager::GetFileName(file_info_->GetFilePath());
    if (status_filter & NODE_CLOUD)
    {
        // make sure this function is called after cloud filesystem is scanned
        if (!isOnCloud(name, file_info_->GetFileSize()))
        {
            return false;
        }
    }

    if (status_filter & NODE_NOT_ON_CLOUD)
    {
        if (isOnCloud(name, file_info_->GetFileSize()))
        {
            return false;
        }
    }

    if (status_filter & NODE_PURCHASED)
    {
        if (!file_info_->IsDuoKanBook() || file_info_->GetIsTrialBook())
        {
            return false;
        }
    }

    if (status_filter & NODE_IS_TRIAL)
    {
        if (!file_info_->IsDuoKanBook() || !file_info_->GetIsTrialBook())
        {
            return false;
        }
    }

    if (status_filter & NODE_SELF_OWN)
    {
        if (file_info_->IsDuoKanBook())
        {
            return false;
        }
    }

    if (status_filter & NODE_DUOKAN_BOOK_NOT_CLASSIFIED)
    {
        Node* parent_node = mutableParent();
        LocalBookStoreNode* bookstore_root = dynamic_cast<LocalBookStoreNode*>(parent_node);
        if (bookstore_root == 0)
        {
            return false;
        }
    }

    if (!(status_filter & NODE_IS_UPLOADING))
    {
        IDownloader * downloader = IDownloader::GetInstance();
        if (downloader)
        {
            IDownloadTask* this_task = downloader->GetTaskInfoByUrlId(absolutePath().c_str());
            if (this_task != 0)
            {
                int uploading_state = this_task->GetState();
                if (uploading_state & (IDownloadTask::PAUSED |
                                      IDownloadTask::WAITING |
                                      IDownloadTask::WAITING_QUEUE |
                                      IDownloadTask::WORKING))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void FileNode::upload(bool exec_now)
{
    Node::upload(exec_now);

    // Only upload the selected file
    if (cloud_file_info_ != 0)
    {
        // TODO. Notify callers
        return;
    }

    // TODO. support create file in given directory in the future.
    // make sure the parent id is valid
    string parent_id = mutableRoot()->id();
    if (!parent_id.empty())
    {
        int stat = status();
        stat |= NODE_IS_UPLOADING;
        setStatus(stat);
        XiaoMiServiceFactory::GetMiCloudService()->CreateFile(parent_id, absolutePath(), displayName(), exec_now);
        return;
    }
}

void FileNode::onCreateFileFinished(const EventArgs& args)
{
    const FileCreatedArgs& file_create_args = dynamic_cast<const FileCreatedArgs&>(args);
    int stat = status();
    NodeLoadingFinishedArgs node_loading_finished_args;
    node_loading_finished_args.type = NodeLoadingFinishedArgs::NODE_LOADING_TYPE_UPLOAD;
    node_loading_finished_args.current_node_path = absolutePath();
    if (file_create_args.succeeded && file_create_args.result != 0)
    {
        MiCloudFileSPtr cloud_file_info = file_create_args.result->GetFileInfo();
        if (cloud_file_info != 0)
        {
            // upload finished
            stat &= ~NODE_IS_UPLOADING; // reset uploading flag
            cloud_file_info_ = cloud_file_info;

            stat |= NODE_CLOUD;
            setStatus(stat, true);

            node_loading_finished_args.succeeded = true;
        }
        else
        {
            // waiting for uploading to KSS, do nothing but return
            return;
        }
    }
    else
    {
        // uploading failed
        stat &= ~NODE_IS_UPLOADING; // reset uploading flag
        setStatus(stat, true);
        node_loading_finished_args.succeeded = false;
        if (file_create_args.result != 0)
        {
            node_loading_finished_args.reason = file_create_args.result->GetErrorReason();
        }
    }
    FireEvent(EventNodeLoadingFinished, node_loading_finished_args);
}

void FileNode::onFileCommitted(const EventArgs& args)
{
    const FileCreatedArgs& file_create_args = dynamic_cast<const FileCreatedArgs&>(args);
    int stat = status();
    NodeLoadingFinishedArgs node_loading_finished_args;
    node_loading_finished_args.type = NodeLoadingFinishedArgs::NODE_LOADING_TYPE_UPLOAD;
    node_loading_finished_args.current_node_path = absolutePath();

    stat &= ~NODE_IS_UPLOADING; // reset uploading flag
    if (file_create_args.succeeded && file_create_args.result != 0)
    {
        MiCloudFileSPtr cloud_file_info = file_create_args.result->GetFileInfo();
        if (cloud_file_info != 0)
        {
            // upload finished
            cloud_file_info_ = cloud_file_info;
            stat |= NODE_CLOUD;
            node_loading_finished_args.succeeded = true;
        }
    }
    setStatus(stat, true);
}

void FileNode::fetchInfo()
{
    string cloud_folder_path = cloudFilePath();
    XiaoMiServiceFactory::GetMiCloudService()->GetInfo(cloud_folder_path);
}

void FileNode::onInfoReturned(const EventArgs& args)
{
    const GetInfoArgs& get_info_args = dynamic_cast<const GetInfoArgs&>(args);
    if (get_info_args.succeeded && get_info_args.result != 0)
    {
        MiCloudFileSPtr returned_cloud_file = get_info_args.result->GetFileInfo();
        int stat = status();

        // match the file name and size
        if (returned_cloud_file != 0 &&
            returned_cloud_file->size == static_cast<int64_t>(fileSize()))
        {
            cloud_file_info_ = returned_cloud_file;
            stat |= NODE_CLOUD;
        }
        else
        {
            stat |= NODE_NOT_ON_CLOUD;
        }
        setStatus(stat);
    }
    // TODO. Error handling
}

bool FileNode::isOnCloud(const string& name, int64_t size)
{
    MiCloudFileSPtr cloud_file_info;
    return CloudFileSystemTree::isLocalFileInCloud(name, size, cloud_file_info);
}

void FileNode::onUploadingProgress(int progress, int state)
{
    int current_node_status = status();
    uploading_progress_    = progress;
    uploading_state_       = state;

    NodeLoadingFinishedArgs node_loading_finished_args;
    node_loading_finished_args.type = NodeLoadingFinishedArgs::NODE_LOADING_TYPE_UPLOAD;
    node_loading_finished_args.current_node_path = absolutePath();

    if (state & (IDownloadTask::CANCELED |
                 IDownloadTask::FAILED))
    {
        uploading_progress_ = -1;
        current_node_status &= ~NODE_IS_UPLOADING; // remove uploading flag
        node_loading_finished_args.succeeded = false;
        node_loading_finished_args.reason = "Upload Cancelled";
        FireEvent(EventNodeLoadingFinished, node_loading_finished_args);
    }
    else if (state & IDownloadTask::DONE)
    {
        /* Do NOT handle uploading done here but in onCommitted().
        uploading_progress_ = 100;
        current_node_status &= ~NODE_IS_UPLOADING; // remove uploading flag

        // Notify cloud model that a new node has been uploaded
        // TODO. support bookself in the future.
        MiCloudManager* cloud_manager = MiCloudManager::GetInstance();
        cloud_manager->FetchFileListsInBookRootDir();
        current_node_status |= NODE_CLOUD;
        */
    }
    else if (current_node_status == NODE_LOCAL && (state & IDownloadTask::WORKING))
    {
        // the task could be resumed by IDownloader, update the status
        current_node_status |= NODE_IS_UPLOADING;
    }
    setStatus(current_node_status, true);
}

bool FileNode::loadingInfo(int& progress, int& state)
{
    // Return nothing by default
    if (!(status() & NODE_IS_UPLOADING))
    {
        return false;
    }
    progress = uploading_progress_;
    state    = uploading_state_;
    return true;
}

void FileNode::updateByUploadingTask()
{
    IDownloader * downloader = IDownloader::GetInstance();
    if (downloader)
    {
        IDownloadTask* this_task = downloader->GetTaskInfoByUrlId(absolutePath().c_str());
        if (this_task)
        {
            int node_state = status();
            uploading_state_    = this_task->GetState();
            uploading_progress_ = this_task->GetPercentage();
            if (uploading_state_ & (IDownloadTask::PAUSED |
                                  IDownloadTask::WAITING |
                                  IDownloadTask::WAITING_QUEUE |
                                  IDownloadTask::WORKING |
                                  IDownloadTask::CURL_DONE |
                                  IDownloadTask::DONE))
            {
                node_state |= NODE_IS_UPLOADING;
            }
            else
            {
                node_state &= ~NODE_IS_UPLOADING; // reset uploading flag
            }
            status_ = node_state;
        }
    }
}

void FileNode::stopLoading()
{
}

bool FileNode::supportedCommands(std::vector<int>& command_ids,
                                 std::vector<int>& str_ids)
{
    command_ids.clear();
    str_ids.clear();
    if (type() == NODE_TYPE_FILE_LOCAL_DOCUMENT)
    {
        string name = PathManager::GetFileName(absolutePath());
        bool is_on_cloud = isOnCloud(name, fileSize());
        command_ids.push_back(ID_BTN_READ_BOOK);
        command_ids.push_back(ID_BTN_DELETE);
        command_ids.push_back(ID_BTN_SINAWEIBO_SHARE);
        if (!is_on_cloud)
        {
            command_ids.push_back(ID_BIN_UPLOAD_BOOK);
        }
        command_ids.push_back(ID_INVALID);

        str_ids.push_back(READ_BOOK);
        str_ids.push_back(DELETE_BOOK);
        str_ids.push_back(SHARE_BOOK_TO_SINAWEIBO);
        if (!is_on_cloud)
        {
            str_ids.push_back(TOUCH_UPLOAD);
        }
        str_ids.push_back(-1);
    }
    else
    {
        const Node* parent_node = parent();
        command_ids.push_back(ID_BTN_READ_BOOK);
        command_ids.push_back(ID_BTN_DELETE);
        if (parent_node->type() != NODE_TYPE_CATEGORY_LOCAL_BOOK_STORE)
        {
            command_ids.push_back(ID_BTN_DELETE_FROM_CATEGORY);
        }
        command_ids.push_back(ID_BTN_SINAWEIBO_SHARE);
        command_ids.push_back(ID_INVALID);

        str_ids.push_back(READ_BOOK);
        str_ids.push_back(DELETE_BOOK);
        if (parent_node->type() != NODE_TYPE_CATEGORY_LOCAL_BOOK_STORE)
        {
            str_ids.push_back(CATEGORY_DELETE_FILE_FROM_CATEGORY);
        }
        str_ids.push_back(SHARE_BOOK_TO_SINAWEIBO);
        str_ids.push_back(-1);
    }
    return true;
}

}  // namespace document_model

}  // namespace dk
