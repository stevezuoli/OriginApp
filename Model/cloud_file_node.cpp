#include "Model/cloud_file_node.h"
#include "Model/cloud_filesystem_tree.h"

#include "Utility/ImageManager.h"
#include "XiaoMi/MiCloudService.h"
#include "XiaoMi/XiaoMiServiceFactory.h"
#include "DownloadManager/IDownloadTask.h"

namespace dk {

namespace document_model {

CloudFileNode::CloudFileNode(Node * parent, MiCloudFileSPtr file_info)
    : Node(parent)
    , data_state_(MD_TOSCAN)
    , cloud_file_info_(file_info)
    , size_(cloud_file_info_->size)
    , downloading_progress_(-1.0)
    , downloading_state_(IDownloadTask::NONE)
{
    mutableType() = NODE_TYPE_MICLOUD_BOOK;
    update();
    CloudFileSystemTree::addNodeToGlobalMap(this);
}

CloudFileNode::~CloudFileNode()
{
    CloudFileSystemTree::removeNodeFromGlobalMap(id());
}

void CloudFileNode::update()
{
    int stat = status();
    mutableId() = cloud_file_info_->id;
    mutableAbsolutePath() = absolutePathFromAncestor();
    mutableName() = cloud_file_info_->name;
    mutableDisplayName() = cloud_file_info_->name;

    // check whether it is local file
    if (CloudFileSystemTree::isCloudFileInLocalStorage(id(),
                                                       name(),
                                                       size_,
                                                       local_file_info_))
    {
        mutableAbsolutePath() = local_file_info_->GetFilePath();
        mutableName() = local_file_info_->GetFileName();
        mutableDisplayName() = local_file_info_->GetFileName(); // TODO.
        mutableLastRead() = local_file_info_->GetFileLastReadTime();
        mutableCoverPath() = local_file_info_->GetCoverImagePath();
        stat |= NODE_LOCAL;
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

    if (local_file_info_ != 0)
    {
        mutableCoverPath() = local_file_info_->GetCoverImagePath();
    }

    data_state_ = MD_SCANNED;

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
    // TODO.
    return true;
}

void CloudFileNode::download()
{
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    if (!cloud_service->RequestDownload(id(), name()))
    {
        // TODO. Handle the downloading failed request
        return;
    }
    int stat = status();
    stat |= NODE_IS_DOWNLOADING;
    setStatus(stat);
}

bool CloudFileNode::remove(bool delete_local_files_if_possible)
{
    // NOTE: only delete the selected node
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    cloud_service->DeleteFile(id());
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

void CloudFileNode::onDownloadingProgress(int progress, int state)
{
    int current_node_status = status();
    downloading_progress_ = progress;
    downloading_state_    = state;

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

}

}