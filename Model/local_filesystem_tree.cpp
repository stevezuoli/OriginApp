#include "Utility/StringUtil.h"
#include "Model/local_filesystem_tree.h"
#include "Model/local_folder_node.h"
#include "Model/cloud_filesystem_tree.h"
#include <tr1/functional>
#include "Common/FileManager_DK.h"
#include "DownloadManager/IDownloader.h"

using namespace dk::utility;

namespace dk
{

namespace document_model
{

LocalFileSystemTree::LocalFileSystemTree()
: root_node_(0)
{
    initialize();
}

LocalFileSystemTree::~LocalFileSystemTree()
{
}

/// Initialize data model. It will initialize the data model.
/// By default it will insert all nodes that at top and second level
/// into the node tree.
/// If preference data is available, it will also initialize according
/// to the preference data.
void LocalFileSystemTree::initialize()
{
    // by_ = static_cast<Field>(explorerOption().value(SORT_BY_TAG, NAME).toInt());
    // order_ = static_cast<SortOrder>(explorerOption().value(SORT_ORDER_TAG, ASCENDING).toInt());
    setCurrentNode(&root_node_);

    SubscribeMessageEvent(
            CDKFileManager::EventFileListChanged,
            *CDKFileManager::GetFileManager(),
            std::tr1::bind(
                std::tr1::mem_fn(&LocalFileSystemTree::onFileListChanged),
                this,
                std::tr1::placeholders::_1));
    
    SubscribeMessageEvent(BookCoverLoader::EventBookCoverLoaded,
            *BookCoverLoader::GetInstance(),
            std::tr1::bind(
            std::tr1::mem_fn(&LocalFileSystemTree::onCoverLoaded),
            this,
            std::tr1::placeholders::_1));

    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    SubscribeMessageEvent(
        MiCloudService::EventFileCreated,
            *cloud_service,
            std::tr1::bind(
            std::tr1::mem_fn(&LocalFileSystemTree::onCreateFileFinished),
                this,
                std::tr1::placeholders::_1));

    SubscribeMessageEvent(
        MiCloudService::EventInfoGot,
            *cloud_service,
            std::tr1::bind(
            std::tr1::mem_fn(&LocalFileSystemTree::onGetInfoReturned),
                this,
                std::tr1::placeholders::_1));

    SubscribeMessageEvent(
        MiCloudService::EventDirectoryCreated,
            *cloud_service,
            std::tr1::bind(
            std::tr1::mem_fn(&LocalFileSystemTree::onDirectoryCreated),
                this,
                std::tr1::placeholders::_1));

    // connect download manager
    SubscribeMessageEvent(
        IDownloader::EventDownloadProgressUpdate,
        *IDownloader::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&LocalFileSystemTree::onUploadingProgress),
            this,
            std::tr1::placeholders::_1));

}

ContainerNode * LocalFileSystemTree::setCurrentNode(ContainerNode *node)
{
    current_node_ = node;
    mapFilePathToModelPath(current_node_->absolutePath(), current_path_);
    return current_node_;
}

bool LocalFileSystemTree::onCoverLoaded(const EventArgs& args)
{
    const BookCoverLoadedArgs& cover_loader_args = (const BookCoverLoadedArgs&)args;
    if (cover_loader_args.coverPath.empty())
    {
        return true;
    }

    string book_path = cover_loader_args.bookPath;
    ModelPath model_path;
    if (mapFilePathToModelPath(book_path, model_path))
    {
        Node* node = root_node_.getNodeByModelPath(model_path);
        if (node != 0)
        {
            node->mutableCoverPath() = cover_loader_args.coverPath;
        }
        fireCoverLoadedEvent(book_path, cover_loader_args.coverPath);
    }
    return true;
}

ContainerNode * LocalFileSystemTree::root()
{
    return &root_node_;
}

/// Retrieve current node for current active type.
ContainerNode * LocalFileSystemTree::currentNode()
{
    return current_node_;
}

NodeType LocalFileSystemTree::currentType()
{
    return current_node_->type();
}

/// Return the top level container node according to the type.
ContainerNode * LocalFileSystemTree::containerNodeWithinTopNode(NodeType type)
{
    ContainerNode * node = dynamic_cast<ContainerNode *>(root_node_.node(type).get());
    return node;
}

Field LocalFileSystemTree::sortField() const
{
    //return current_node_->sortField();
    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    DK_FileSorts sort = file_manager->GetBookSort();
    switch (sort)
    {
    case RecentlyAdd:
        return RECENTLY_ADD;
    case RecentlyReadTime:
        return LAST_ACCESS_TIME;
    case Name:
        return NAME;
    case DIRECTORY:
        return BY_DIRECTORY;
    default:
        break;
    }
    return NAME; // sort by name by default
}

void LocalFileSystemTree::setSortField(Field by)
{
    DK_FileSorts sort = RecentlyReadTime;
    switch (by)
    {
    case RECENTLY_ADD:
        sort = RecentlyAdd;
        break;
    case LAST_ACCESS_TIME:
        sort = RecentlyReadTime;
        break;
    case NAME:
        sort = Name;
        break;
    case BY_DIRECTORY:
        sort = DIRECTORY;
        break;
    default:
        break;
    }
    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    file_manager->SetBookSortType(sort);
}

void LocalFileSystemTree::sort()
{
    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    file_manager->SortFile(DFC_Book);
}

void LocalFileSystemTree::search(const string& keyword)
{
    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    file_manager->SearchBook(keyword.c_str()); 
}

SortOrder LocalFileSystemTree::sortOrder() const
{
    return current_node_->sortOrder();
}

ContainerNode * LocalFileSystemTree::cdRootNode()
{
    return setCurrentNode(&root_node_);
}

ContainerNode * LocalFileSystemTree::cdContainerNodeWithinTopNode(const string& name)
{
    NodePtr ptr = root_node_.node(name);
    if (ptr == 0)
    {
        return 0;
    }
    return setCurrentNode(dynamic_cast<ContainerNode *>(ptr.get()));
}

ContainerNode * LocalFileSystemTree::cd(NodePtr p)
{
    return setCurrentNode(dynamic_cast<ContainerNode *>(p.get()));
}

ContainerNode * LocalFileSystemTree::cd(const string& name)
{
    // Get child node.
    NodePtr ptr = current_node_->node(name);
    if (ptr != 0)
    {
        return setCurrentNode(dynamic_cast<ContainerNode *>(ptr.get()));
    }
    return 0;
}

/// Convert file system path to ModelPath.
bool LocalFileSystemTree::mapFilePathToModelPath(const string & path, ModelPath & model_path)
{
    model_path.clear();
    if (StringUtil::StartWith(path.c_str(), root_node_.absolutePath().c_str())) // "/mnt/us"
    {
        model_path.push_back(root_node_.absolutePath());
        if (path.size() > root_node_.absolutePath().size())
        {
            string relative_path = path.substr(root_node_.absolutePath().size() + 1);
            StringList relative_names = StringUtil::Split(relative_path.c_str(), "/");
            for (size_t i = 0; i < relative_names.size(); ++i)
            {
                model_path.push_back(relative_names[i]);
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool LocalFileSystemTree::cdPath(const string& path)
{
    ModelPath paths;
    if (mapFilePathToModelPath(path, paths))
    {
        return cdPath(paths);
    }
    return false;
}

bool LocalFileSystemTree::cdPath(const ModelPath &path)
{
    if (path.size() <= 0)
    {
        return false;
    }

    string root_path = path.front();
    if (root_path != root_node_.absolutePath())
    {
        return false;
    }
    Node* node = &root_node_;
    for (int i = 1; i < path.size(); i++)
    {
        if (node == 0)
        {
            return false;
        }
        ContainerNode* container = dynamic_cast<ContainerNode*>(node);
        if (container != 0)
        {
            node = container->node(path[i]).get();
        }
        else
        {
            return false;
        }
    }

    ContainerNode* current_node = dynamic_cast<ContainerNode*>(node);
    if (current_node != 0)
    {
        setCurrentNode(current_node);
        return true;
    }
    return false;
}

ContainerNode * LocalFileSystemTree::cdUp()
{
    if (!canGoUp())
    {
        return 0;
    }

    return setCurrentNode(dynamic_cast<ContainerNode *>(current_node_->mutableParent()));
}

bool LocalFileSystemTree::canGoUp()
{
    return (current_node_ != &root_node_);
}

void LocalFileSystemTree::setSortCriteria(Field by, SortOrder order)
{
    by_ = by;
    order_ = order;
}

DKDisplayMode LocalFileSystemTree::displayMode()
{
    return root_node_.currentDisplayMode();
}

void LocalFileSystemTree::setDisplayMode(DKDisplayMode display_mode)
{
    root_node_.mutableDisplayMode() = display_mode;
    root_node_.setDirty(true);
}

bool LocalFileSystemTree::onFileListChanged(const EventArgs& args)
{
    ContainerNode* current_node = currentNode();
    if (current_node != 0)
    {
        // madatorily update children of current node
        current_node->setDirty(true);
    }
    fireFileSystemChangedEvent();
}

// Cloud Events Slots
bool LocalFileSystemTree::onCreateFileFinished(const EventArgs& args)
{
    const FileCreatedArgs& file_create_args = dynamic_cast<const FileCreatedArgs&>(args);
    string node_path = file_create_args.local_file_path;
    ModelPath node_model_path;
    if (mapFilePathToModelPath(node_path, node_model_path))
    {
        Node* target = root_node_.getNodeByModelPath(node_model_path);
        if (target != 0)
        {
            FileNode* file_target = dynamic_cast<FileNode*>(target);
            if (file_target != 0)
            {
                file_target->onCreateFileFinished(args);
            }
        }
    }
    return true;
}

bool LocalFileSystemTree::onDirectoryCreated(const EventArgs& args)
{
    const DirectoryCreatedArgs& dir_create_args = dynamic_cast<const DirectoryCreatedArgs&>(args);
    string node_path = dir_create_args.dir_path;
    ModelPath node_model_path;
    if (mapFilePathToModelPath(node_path, node_model_path))
    {
        Node* target = root_node_.getNodeByModelPath(node_model_path);
        if (target != 0)
        {
            LocalFolderNode* folder_target = dynamic_cast<LocalFolderNode*>(target);
            if (folder_target != 0)
            {
                folder_target->onCreateCloudDirectoryFinished(args);
            }
        }
    }
    return true;
}

bool LocalFileSystemTree::onGetInfoReturned(const EventArgs& args)
{
    const FileCreatedArgs& file_create_args = dynamic_cast<const FileCreatedArgs&>(args);
    string node_path = file_create_args.local_file_path;
    ModelPath node_model_path;
    if (mapFilePathToModelPath(node_path, node_model_path))
    {
        Node* target = root_node_.getNodeByModelPath(node_model_path);
        if (target != 0)
        {
            LocalFolderNode* folder_target = dynamic_cast<LocalFolderNode*>(target);
            if (folder_target != 0)
            {
                folder_target->onInfoReturned(args);
                return true;
            }
            DesktopNode* desktop = dynamic_cast<DesktopNode*>(target);
            if (desktop != 0)
            {
                desktop->onInfoReturned(args);
                return true;
            }
            FileNode* file = dynamic_cast<FileNode*>(target);
            if (file != 0)
            {
                file->onInfoReturned(args);
                return true;
            }
        }
    }
    return true;
}

bool LocalFileSystemTree::onUploadingProgress(const EventArgs& args)
{
    // TODO. Check the type of download task. Only handle the upload task
    const DownloadUpdateEventArgs& upload_args = (const DownloadUpdateEventArgs&)args;
    if (upload_args.type != IDownloadTask::MICLOUDFILE)
    {
        return false;
    }

    string file_path = upload_args.taskId;
    ModelPath node_model_path;
    if (mapFilePathToModelPath(file_path, node_model_path))
    {
        Node* target = root_node_.getNodeByModelPath(node_model_path);
        if (target != 0)
        {
            FileNode* file = dynamic_cast<FileNode*>(target);
            if (file != 0)
            {
                file->onUploadingProgress(upload_args.percentage, upload_args.state);
                return true;
            }
        }
    }

    // For downloading task, if it's done, update the children of DK_Download
    if (upload_args.state == IDownloadTask::DONE)
    {
        string cloud_file_id = upload_args.taskId;
        Node* cloud_node = CloudFileSystemTree::getNodeFromGlobalMap(cloud_file_id);
        if (cloud_node != 0)
        {
            string download_path = PathManager::GetDownloadFolderPath();
            string root_path;
            string download_folder_name;
            PathManager::SplitLastPath(download_path.c_str(), &root_path, &download_folder_name);
            NodePtr download_node = root_node_.node(download_folder_name);
            ContainerNode* download_container = dynamic_cast<ContainerNode*>(download_node.get());
            if (download_container != 0)
            {
                RetrieveChildrenResult ret = RETRIEVE_FAILED;
                download_container->children(ret, true, download_container->statusFilter());
            }
        }
    }
    return true;
}

}  // namespace document_model

}  // namespace dk
