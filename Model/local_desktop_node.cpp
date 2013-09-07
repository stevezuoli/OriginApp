#include <algorithm>
#include <map>

#include "Model/local_desktop_node.h"
#include "Model/local_folder_node.h"
#include "Model/local_bookstore_node.h"
#include "Model/model_tree.h"
#include "Model/cloud_filesystem_tree.h"

#include "Utility/PathManager.h"
#include "Utility/FileSystem.h"
#include "Common/FileManager_DK.h"

#include <tr1/functional>
#include "XiaoMi/MiCloudService.h"
#include "XiaoMi/XiaoMiServiceFactory.h"
#include "XiaoMi/MiCloudManager.h"

using namespace dk::utility;
using namespace xiaomi;

namespace dk {

namespace document_model {

static const char* CLOUD_DESKTOP_NAME = "BOOKS";

/// Put all initial directories here.
/// Get Last access location.
static string getLibraryRoot()
{
    return PathManager::GetRootPath();
}

DesktopNode::DesktopNode(Node * p)
    : ContainerNode(p)
    , current_display_mode_(BY_FOLDER)
    , pending_scan_(false)
{
    mutableType() = NODE_TYPE_LOCAL_DESKTOP;
    mutableAbsolutePath() = getLibraryRoot();
    //updateChildren();

    MiCloudManager* cloud_manager = MiCloudManager::GetInstance();
    SubscribeMessageEvent(
        MiCloudManager::BookRootDirCreated,
            *cloud_manager,
            std::tr1::bind(
            std::tr1::mem_fn(&DesktopNode::onCloudRootCreated),
                this,
                std::tr1::placeholders::_1));

    ContainerNode* cloud_root_node = ModelTree::getModelTree(MODEL_MICLOUD)->root();
    SubscribeMessageEvent(Node::EventChildrenIsReady,
        *cloud_root_node,
        std::tr1::bind(
            std::tr1::mem_fn(&DesktopNode::onCloudRootScanned),
            this,
            std::tr1::placeholders::_1));
}

DesktopNode::~DesktopNode()
{
    clearChildren();
}

NodePtr DesktopNode::createBookStoreNode(const string& path, int status_filter)
{
    if (LocalBookStoreNode::testStatus(path, status_filter))
    {
        NodePtr bookstore(new LocalBookStoreNode(this, path));
        return bookstore;
    }
    return NodePtr();
}

Node* DesktopNode::getNodeByModelPath(const ModelPath &path)
{
    if (path.size() <= 0)
    {
        return 0;
    }

    string root_path = path.front();
    if (root_path != absolutePath())
    {
        return 0;
    }
    Node* node = this;
    for (int i = 1; i < path.size(); i++)
    {
        if (node == 0)
        {
            return 0;
        }
        ContainerNode* container = dynamic_cast<ContainerNode*>(node);
        if (container != 0)
        {
            node = container->node(path[i]).get();
        }
        else if (i < path.size() - 1)
        {
            return 0;
        }
    }
    return node;
}

NodePtr DesktopNode::createPushedMessageNode(const string& path, int status_filter)
{
    if (LocalFolderNode::testStatus(path, status_filter))
    {
        NodePtr bookstore(new LocalFolderNode(this, path));
        bookstore->mutableType() = NODE_TYPE_CATEGORY_LOCAL_PUSHED;
        bookstore->mutableName() = nodeName(path);
        bookstore->mutableDisplayName() = nodeDisplayName(path);
        return bookstore;
    }
    return NodePtr();
}

// copied from local folder node
void DesktopNode::scan(const string& dir,
                       NodePtrs &result,
                       int status_filter,
                       bool sort_list)
{
    StringList current_dirs = PathManager::GetDirsInPath(dir.c_str());
    StringList current_books = PathManager::GetFilesInPath(dir.c_str());
    for (size_t i = 0; i < current_dirs.size(); i++)
    {
        string folder_path = PathManager::ConcatPath(dir.c_str(), current_dirs[i].c_str());
        if (!PathManager::IsDocumentsPath(folder_path.c_str()))
        {
            continue;
        }
        NodePtr folder_node;
        if (PathManager::IsBookStorePath(folder_path)) // Do NOT filter
        {
            folder_node = createBookStoreNode(folder_path, status_filter);
        }
        else if (PathManager::IsNewsPath(folder_path.c_str())) // Do Not filter
        {
            folder_node = createPushedMessageNode(folder_path, status_filter);
        }
        else if (LocalFolderNode::testStatus(folder_path, status_filter))
        {
            folder_node.reset(new LocalFolderNode(this, folder_path));
        }

        if (folder_node != 0)
            result.push_back(folder_node);
    }

    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    for (size_t i = 0; i < current_books.size(); i++)
    {
        string book_path = PathManager::ConcatPath(dir.c_str(), current_books[i].c_str());
        if (FileNode::testStatus(book_path, status_filter))
        {
            PCDKFile file = file_manager->GetFileByPath(book_path);
            if (file != 0)
            {
                NodePtr book_node(new FileNode(this, file));
                result.push_back(book_node);
            }
        }
    }

    // Sort.
    if (sort_list)
    {
        sort(result, by_field_, sort_order_);
    }
}

bool DesktopNode::updateChildren(int status_filter)
{
    if ((status_filter & NODE_NOT_ON_CLOUD) &&
        !checkCloudCacheOrUpdate())
    {
        // pending scaning until cloud info is retrieved
        pending_scan_ = true;
        return true;
    }    

    switch (current_display_mode_)
    {
        case BY_FOLDER:
            updateChildrenByFolder(status_filter);
            break;
        case BY_SORT:
            updateChildrenBySort(status_filter);
            break;
        case EXPAND_ALL:
            updateChildrenByExpandingAll(status_filter);
            break;
        default:
            return false;
    }
    dirty_ = false;

    NodeChildenReadyArgs children_ready_args;
    children_ready_args.current_node_path = absolutePath();
    children_ready_args.succeeded = true;
    children_ready_args.children = children_;
    FireEvent(EventChildrenIsReady, children_ready_args);
    return true;
}

void DesktopNode::updateChildrenByFolder(int status_filter)
{
    DeletePtrContainer(&children_);
    scan(absolutePath(), children_, status_filter, true);
}

void DesktopNode::updateChildrenBySort(int status_filter)
{
    DeletePtrContainer(&children_);
    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    int sort_num = file_manager->GetBookCurSortNum();
    for (size_t i = 0; i < sort_num; ++i)
    {
        PCDKFile file = file_manager->GetBookbyListIndex(i);
        if (file != 0 && FileNode::testStatus(file->GetFilePath(), status_filter))
        {
            NodePtr book_node(new FileNode(this, file));
            children_.push_back(book_node);
        }
    }
}

void DesktopNode::updateChildrenByExpandingAll(int status_filter)
{
    DeletePtrContainer(&children_);
    filterPosterity(status_filter, true);
}

/// Update children node but does not re-generate the child list.
NodePtrs& DesktopNode::updateChildrenInfo()
{
    for (NodePtrsIter iter = children_.begin(); iter != children_.end(); ++iter)
    {
        // Update information if it's file node.
        if ((*iter)->type() == NODE_TYPE_FILE_LOCAL_DOCUMENT ||
            (*iter)->type() == NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK)
        {
            dynamic_cast<FileNode *>((*iter).get())->update();
        }
    }

    if (current_display_mode_ != EXPAND_ALL)
    {
        sort(children_, by_field_, sort_order_);
    }
    return children_;
}

/// filter all posterities by status filter and replace current children
/// Do not check whether the cloud data is ready because directory mode is displayed by default
const NodePtrs& DesktopNode::filterPosterity(int status_filter, bool recursive)
{
    NodePtrs result;
    if (ContainerNode::filterPosterity(result, status_filter, recursive))
    {
        DeletePtrContainer(&children_);
        children_ = result;
        dirty_ = false;
        return children_;
    }

    static NodePtrs empty_result;
    return empty_result;
}

bool DesktopNode::onCloudRootCreated(const EventArgs& args)
{
    const BookRootDirCreatedArgs& root_dir_args = dynamic_cast<const BookRootDirCreatedArgs&>(args);
    int stat = status();
    if (root_dir_args.succeeded)
    {
        mutableId() = root_dir_args.id;
        stat |= NODE_CLOUD; // ?? need it
    }
    else
    {
        stat |= NODE_NOT_ON_CLOUD;
    }
    setStatus(stat);
    return true;
}

bool DesktopNode::onCloudRootScanned(const EventArgs& args)
{
    const NodeChildenReadyArgs& root_scan_args = dynamic_cast<const NodeChildenReadyArgs&>(args);
    if (pending_scan_)
    {
        string path = root_scan_args.current_node_path;
        ContainerNode* cloud_root_node = ModelTree::getModelTree(MODEL_MICLOUD)->root();
        if (path == cloud_root_node->absolutePath())
        {
            pending_scan_ = false;
            updateChildren(statusFilter());
        }
    }
    return true;
}

/// Not use now
void DesktopNode::fetchInfo()
{
    string cloud_folder_path = cloudFilePath();
    XiaoMiServiceFactory::GetMiCloudService()->GetInfo(cloud_folder_path);
}

/// Not use now
void DesktopNode::onInfoReturned(const EventArgs& args)
{
    const GetInfoArgs& get_info_args = dynamic_cast<const GetInfoArgs&>(args);
    if (get_info_args.succeeded && get_info_args.result != 0)
    {
        int stat = status();
        mutableId() = get_info_args.result->GetFileInfo()->id;
        stat |= NODE_CLOUD;
        setStatus(stat);
    }

    // TODO. Error handling
}

void DesktopNode::upload()
{
    if (id().empty())
    {
        // Cannot upload until the id of root is retrieved
        NodeLoadingFinishedArgs upload_done_args;
        upload_done_args.succeeded = false;
        upload_done_args.type = NodeLoadingFinishedArgs::NODE_LOADING_TYPE_UPLOAD;
        upload_done_args.reason = "Root node is missing";
        upload_done_args.current_node_path = absolutePath();
        FireEvent(EventNodeLoadingFinished, upload_done_args);
    }
    uploadChildren();
}

void DesktopNode::uploadChildren()
{
    //NodePtrs waiting_children = filterChildren(NODE_LOCAL | NODE_NOT_ON_CLOUD, true);
    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    NodePtrs waiting_children = children(ret, false, statusFilter());
    for (size_t i = 0; i < waiting_children.size(); i++)
    {
        NodePtr child = waiting_children[i];
        NodeType child_type = child->type();
        if (child_type != NODE_TYPE_FILE_LOCAL_DOCUMENT &&
            child_type != NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK)
        {
            child->upload();
        }
        else
        {
            if (child->selected() && child_type == NODE_TYPE_FILE_LOCAL_DOCUMENT)
            {
                child->upload();
            }
        }
    }
}

bool DesktopNode::checkCloudCacheOrUpdate()
{
    if (CloudFileSystemTree::isLocalCacheInitialized()) return true;

    // try to retrieve cloud nodes
    ContainerNode* cloud_root_node = ModelTree::getModelTree(MODEL_MICLOUD)->root();
    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    cloud_root_node->children(ret, true);
    return false;
}

}  // namespace document_model

}  // namespace dk
