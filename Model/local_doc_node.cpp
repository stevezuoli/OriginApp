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
#include "BookStore/LocalCategoryManager.h"
#include "Utility/PathManager.h"
#include "Utility/ImageManager.h"

using namespace dk::bookstore;
using namespace dk::utility;
using namespace xiaomi;

namespace dk {

namespace document_model {

FileNode::FileNode(Node * parent, PCDKFile file_info)
        : Node(parent)
        , data_state_(MD_TOSCAN)
        , file_info_(file_info)
        , size_(file_info->GetFileSize())
        , uploading_progress_(-1.0)
        , uploading_state_(IDownloadTask::NONE)
{
    mutableAbsolutePath() = file_info->GetFilePath();
    mutableName() = PathManager::GetFileName(file_info->GetFilePath());
    mutableDisplayName() = file_info->GetFileName(); // TODO.
    mutableType() = file_info->IsDuoKanBook() ? NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK : NODE_TYPE_FILE_LOCAL_DOCUMENT;
    mutableLastRead() = file_info->GetFileLastReadTime();
    mutableCoverPath() = file_info->GetCoverImagePath();
    mutableId() = file_info->GetBookID();
    status_ = NODE_LOCAL;
}

FileNode::~FileNode()
{
}

/// Update all information.
void FileNode::update()
{
    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    HRESULT ret = file_manager->FileDkpProxy(file_info_, GETFILEINFO);
    if (SUCCEEDED(ret))
    {
        mutableAbsolutePath() = file_info_->GetFilePath();
        mutableName() = PathManager::GetFileName(file_info_->GetFilePath());
        mutableDisplayName() = file_info_->GetFileName(); // TODO.

        mutableType() = file_info_->IsDuoKanBook() ?
            NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK : NODE_TYPE_FILE_LOCAL_DOCUMENT;
        mutableLastRead() = file_info_->GetFileLastReadTime();
        mutableCoverPath() = file_info_->GetCoverImagePath();
        mutableId() = file_info_->GetBookID();

        size_ = file_info_->GetFileSize();
        data_state_ = MD_SCANNED;        
    }
}

bool FileNode::rename(const string& new_name, string& error_msg)
{
    // TODO. Implement Me
    return false;
}

bool FileNode::remove(bool delete_local_files_if_possible)
{
    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    if(0 == file_manager)
    {
        return false;
    }

    if (!file_manager->DelFile(absolutePath()))
    {
        return false;
    }

    if (type() == NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK)
    {
        if (!LocalCategoryManager::RemoveBookFromCategory(parent()->absolutePath().c_str(),
            PathManager::GetFileNameWithoutExt(name().c_str()).c_str()))// delete file by book id
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

bool FileNode::testStatus(const string& path, int status_filter)
{
    if (status_filter == NODE_NONE)
    {
        return true;
    }

    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    PCDKFile file = file_manager->GetFileByPath(path);
    // file must not be null
    if (file == 0)
    {
        return false;
    }

    string name = PathManager::GetFileName(file->GetFilePath());
    if (status_filter & NODE_CLOUD)
    {
        // make sure this function is called after cloud filesystem is scanned
        if (!isOnCloud(name, file->GetFileSize()))
        {
            return false;
        }
    }

    if (status_filter & NODE_NOT_ON_CLOUD)
    {
        if (isOnCloud(name, file->GetFileSize()))
        {
            return false;
        }
    }

    if (status_filter & NODE_PURCHASED)
    {
        if (!file->IsDuoKanBook() || file->GetIsTrialBook())
        {
            return false;
        }
    }

    if (status_filter & NODE_IS_TRIAL)
    {
        if (!file->IsDuoKanBook() || !file->GetIsTrialBook())
        {
            return false;
        }
    }

    if (status_filter & NODE_SELF_OWN)
    {
        if (file->IsDuoKanBook())
        {
            return false;
        }
    }
    return true;
}

void FileNode::upload()
{
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
        XiaoMiServiceFactory::GetMiCloudService()->CreateFile(parent_id, absolutePath());
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

}  // namespace document_model

}  // namespace dk
