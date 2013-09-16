#include "Model/cloud_file_node.h"
#include "Model/cloud_filesystem_tree.h"

#include "Utility/ImageManager.h"
#include "Utility/StringUtil.h"

#include "XiaoMi/MiCloudService.h"
#include "XiaoMi/XiaoMiServiceFactory.h"
#include "DownloadManager/IDownloadTask.h"
#include "DownloadManager/IDownloader.h"

#include "CommandID.h"
#include "I18n/StringManager.h"

using namespace dk::utility;

namespace dk {

namespace document_model {

CloudFileNode::CloudFileNode(Node * parent, MiCloudFileSPtr file_info)
    : Node(parent)
    , cloud_file_info_(file_info)
    , size_(cloud_file_info_->size)
    , downloading_progress_(-1.0)
    , downloading_state_(IDownloadTask::NONE)
{
    status_ = NODE_CLOUD;
    mutableType() = NODE_TYPE_MICLOUD_BOOK;
    update();
    //CloudFileSystemTree::addNodeToGlobalMap(this);
}

CloudFileNode::~CloudFileNode()
{
    //CloudFileSystemTree::removeNodeFromGlobalMap(id());
    CloudFileSystemTree::removeNodeFromCloudLocalMap(id());
}

void CloudFileNode::update()
{
    int stat = status();
    stat |= NODE_CLOUD;
    stat = updateStatusByDownloadTask(stat);
    mutableId() = cloud_file_info_->id;
    mutableAbsolutePath() = absolutePathFromAncestor();
    mutableName() = cloud_file_info_->name;
    mutableDisplayName() = cloud_file_info_->name;
    mutableGbkName() = EncodeUtil::UTF8ToGBKString(displayName());
    mutableCreateTime() = cloud_file_info_->createTime;
    mutableModifyTime() = cloud_file_info_->modifyTime;

    // check whether it is local file
    if (CloudFileSystemTree::isCloudFileInLocalStorage(id(),
                                                       name(),
                                                       size_,
                                                       local_file_info_))
    {
        mutableName() = PathManager::GetFileName(local_file_info_->GetFilePath());
        mutableDisplayName() = local_file_info_->GetFileName();
        mutableGbkName() = EncodeUtil::UTF8ToGBKString(displayName());
        mutableLastRead() = local_file_info_->GetFileLastReadTime();
        mutableCoverPath() = local_file_info_->GetCoverImagePath();
        stat |= NODE_LOCAL;
        stat &= ~NODE_IS_DOWNLOADING; // removing downloading status
        if (local_file_info_->IsDuoKanBook())
        {
            if (local_file_info_->GetIsTrialBook())
            {
                stat |= NODE_IS_TRIAL;
            }
            else
            {
                stat |= NODE_PURCHASED;
            }
        }
        else
        {
            stat |= NODE_SELF_OWN;
        }
    }
    else
    {
        stat &= ~NODE_LOCAL;
        local_file_info_ = PCDKFile();
    }

    if (local_file_info_ != 0)
    {
        mutableCoverPath() = local_file_info_->GetCoverImagePath();
    }

    // Do not fire status update event here because it may cause dead loop
    //setStatus(stat);
    status_ = stat;
}

const string CloudFileNode::artist() const
{
    if (local_file_info_ != 0)
    {
        return local_file_info_->GetFileArtist();
    }
    static string dummy;
    return dummy;
}

SPtr<ITpImage> CloudFileNode::getInitialImage()
{
    SPtr<ITpImage> init_img = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_FOLDER);
    DkFileFormat file_format = fileFormat();
    switch(file_format)
    {
    case DFF_Text:
        {
            init_img = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_TXT);
        }
        break;
    case DFF_ElectronicPublishing:
        {
            init_img = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_EPUB);
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
    return init_img;
}

DkFileFormat CloudFileNode::fileFormat()
{
    return GetFileFormatbyExtName(name().c_str());
}

bool CloudFileNode::testStatus(MiCloudFileSPtr book_info, int status_filter)
{
    if (status_filter == NODE_NONE)
    {
        return true;
    }

    if (status_filter & NODE_NOT_ON_LOCAL)
    {
        PCDKFile local_file_info;
        if (CloudFileSystemTree::isCloudFileInLocalStorage(book_info->id,
            book_info->name,
            book_info->size,
            local_file_info))
        {
            return false;
        }
    }

    if (!(status_filter & NODE_IS_DOWNLOADING))
    {
        IDownloader * downloader = IDownloader::GetInstance();
        if (downloader)
        {
            IDownloadTask* this_task = downloader->GetTaskInfoByUrlId(book_info->id.c_str());
            if (this_task != 0)
            {
                int downloading_state = this_task->GetState();
                if (downloading_state & (IDownloadTask::PAUSED |
                                      IDownloadTask::WAITING |
                                      IDownloadTask::WAITING_QUEUE |
                                      IDownloadTask::WORKING))
                {
                    return false;
                }
            }
        }
    }

    // TODO. Support More filters
    return true;
}

void CloudFileNode::download(bool exec_now)
{
    Node::download(exec_now);
    if (local_file_info_ != 0)
    {
        // forbid duplicated downloading
        return;
    }
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    if (!cloud_service->RequestDownload(id(), name(), exec_now))
    {
        NodeLoadingFinishedArgs node_loading_finished_args;
        node_loading_finished_args.type = NodeLoadingFinishedArgs::NODE_LOADING_TYPE_DOWNLOAD;
        node_loading_finished_args.current_node_path = absolutePath();
        node_loading_finished_args.succeeded = false;
        node_loading_finished_args.reason = "Requesting download failed";
        FireEvent(EventNodeLoadingFinished, node_loading_finished_args);
        return;
    }
    int stat = status();
    stat |= NODE_IS_DOWNLOADING;
    setStatus(stat);
}

bool CloudFileNode::remove(bool delete_local_files_if_possible, bool exec_now)
{
    if (status() & NODE_IS_DOWNLOADING)
    {
        // cannot delete downloading node
        return false;
    }

    // NOTE: only delete the selected node
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    cloud_service->DeleteFile(id(), exec_now);
    if (delete_local_files_if_possible && local_file_info_ != 0)
    {
        // TODO. Move this code to local file node
        CDKFileManager* file_manager = CDKFileManager::GetFileManager();
        if (!file_manager->DelFile(local_file_info_->GetFilePath()))
        {
            return false;
        }
        // rescan
        file_manager->FireFileListChangedEvent();
    }
    return true;
}

void CloudFileNode::fetchInfo()
{
    // No need to fetch info for cloud node
}

bool CloudFileNode::loadingInfo(int& progress, int& state)
{
    // Return nothing by default
    if (!(status() & NODE_IS_DOWNLOADING))
    {
        return false;
    }
    progress = downloading_progress_;
    state    = downloading_state_;
    return true;
}

void CloudFileNode::onInfoReturned(const EventArgs& args)
{
}

void CloudFileNode::onRequestDownloadFinished(const EventArgs& args)
{
    const RequestDownloadFinishedArgs& request_download_args = dynamic_cast<const RequestDownloadFinishedArgs&>(args);
    if (request_download_args.succeeded)
    {
        // nothing to do if request downloading succeeds
        return;
    }
    int stat = status();
    stat &= ~NODE_IS_DOWNLOADING; // reset downloading flag
    setStatus(stat);

    NodeLoadingFinishedArgs node_loading_finished_args;
    node_loading_finished_args.type = NodeLoadingFinishedArgs::NODE_LOADING_TYPE_DOWNLOAD;
    node_loading_finished_args.current_node_path = absolutePath();
    node_loading_finished_args.succeeded = false;
    node_loading_finished_args.reason = "Requesting download failed";
    FireEvent(EventNodeLoadingFinished, node_loading_finished_args);
}

void CloudFileNode::onDownloadingProgress(const std::string& task_id, int progress, int state)
{
    int current_node_status = status();
    downloading_progress_ = progress;
    downloading_state_    = state;
    downloading_task_id_  = task_id;

    NodeLoadingFinishedArgs node_loading_finished_args;
    node_loading_finished_args.type = NodeLoadingFinishedArgs::NODE_LOADING_TYPE_DOWNLOAD;
    node_loading_finished_args.current_node_path = absolutePath();

    if (state & (IDownloadTask::CANCELED |
                 IDownloadTask::FAILED))
    {
        downloading_progress_ = -1;
        current_node_status &= ~NODE_IS_DOWNLOADING; // reset downloading flag
        node_loading_finished_args.succeeded = false;
        node_loading_finished_args.reason = "Downloading Cancelled or Failed";
        FireEvent(EventNodeLoadingFinished, node_loading_finished_args);
    }
    else if (state & IDownloadTask::DONE)
    {
        downloading_progress_ = 100;
        current_node_status &= ~NODE_IS_DOWNLOADING; // reset downloading flag
        if (createLocalFileIfExist())
        {
            current_node_status |= NODE_LOCAL;
        }
        node_loading_finished_args.succeeded = true;
        FireEvent(EventNodeLoadingFinished, node_loading_finished_args);
    }
    else if (current_node_status == NODE_CLOUD && (state & IDownloadTask::WORKING))
    {
        current_node_status |= NODE_IS_DOWNLOADING;
    }
    setStatus(current_node_status, true);
}

bool CloudFileNode::createLocalFileIfExist()
{
    string local_file_path = PathManager::ConcatPath(MiCloudManager::GetInstance()->GetLocalDownloadDir(), name());
    if (!PathManager::IsFileExisting(local_file_path))
    {
        return false;
    }
    
    CDKFileManager *file_manager = CDKFileManager::GetFileManager();
    CDKBook* new_file_info = DK_FileFactory::CreateBookFileInfoFromDownload(
        local_file_path.c_str(),
        artist().c_str(),
        WEBSERVER,
        description().c_str(),
        "", // empty password
        fileSize());

    PCDKFile new_file_ptr = PCDKFile(dynamic_cast<CDKFile*>(new_file_info));
    file_manager->AddFile(new_file_ptr);
    file_manager->SortFile(DFC_Book);
    file_manager->FireFileListChangedEvent();
    return true;
}

bool CloudFileNode::satisfy(int status_filter)
{
    return CloudFileNode::testStatus(cloud_file_info_, status_filter);
}

bool CloudFileNode::supportedCommands(std::vector<int>& command_ids, std::vector<int>& str_ids)
{
    command_ids.clear();
    str_ids.clear();
    if (local_file_info_ != 0)
    {
        command_ids.push_back(ID_BTN_READ_BOOK);
        str_ids.push_back(READ_BOOK);
    }
    else
    {
        command_ids.push_back(ID_BIN_DOWNLOAD_BOOK);
        str_ids.push_back(TOUCH_DOWNLOAD);
    }
    command_ids.push_back(ID_BTN_DELETE);
    command_ids.push_back(ID_BTN_SINAWEIBO_SHARE);
    command_ids.push_back(ID_INVALID);

    str_ids.push_back(DELETE_BOOK);
    str_ids.push_back(SHARE_BOOK_TO_SINAWEIBO);
    str_ids.push_back(-1);
    return true;
}

int CloudFileNode::updateStatusByDownloadTask(int status)
{
    int ret = status;
    IDownloader * downloader = IDownloader::GetInstance();
    if (downloader)
    {
        IDownloadTask* this_task = downloader->GetTaskInfoByUrlId(id().c_str());
        if (this_task)
        {
            downloading_state_    = this_task->GetState();
            downloading_progress_ = this_task->GetPercentage();
            if (downloading_state_ & (IDownloadTask::PAUSED |
                                  IDownloadTask::WAITING |
                                  IDownloadTask::WAITING_QUEUE |
                                  IDownloadTask::WORKING |
                                  IDownloadTask::CURL_DONE))
            {
                ret |= NODE_IS_DOWNLOADING;
            }
            else
            {
                ret &= ~NODE_IS_DOWNLOADING;
            }
        }
    }
    return ret;
}

}

}
