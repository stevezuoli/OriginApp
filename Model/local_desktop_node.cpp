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

//static const char* CLOUD_DESKTOP_NAME = "BOOKS";

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
}

NodePtr DesktopNode::createBookStoreNode(const string& path)
{
    //if (LocalBookStoreNode::testStatus(path, status_filter))
    //{
        NodePtr bookstore(new LocalBookStoreNode(this, path));
        return bookstore;
    //}
    //return NodePtr();
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

NodePtr DesktopNode::createPushedMessageNode(const string& path)
{
    //if (LocalFolderNode::testStatus(path, status_filter))
    //{
        NodePtr bookstore(new LocalFolderNode(this, path));
        bookstore->mutableType() = NODE_TYPE_CATEGORY_LOCAL_PUSHED;
        bookstore->mutableName() = nodeName(path);
        bookstore->mutableDisplayName() = nodeDisplayName(path);
        return bookstore;
    //}
    //return NodePtr();
}

// copied from local folder node
void DesktopNode::scan(const string& dir, NodePtrs &result)
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
            folder_node = createBookStoreNode(folder_path);
        }
        else if (PathManager::IsNewsPath(folder_path.c_str())) // Do Not filter
        {
            folder_node = createPushedMessageNode(folder_path);
        }
        else// if (LocalFolderNode::testStatus(folder_path, status_filter))
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
        PCDKFile file = file_manager->GetFileByPath(book_path);
        if (file != 0)
        {
            NodePtr book_node(new FileNode(this, file));
            result.push_back(book_node);
        }
    }
}

/// This is a ugly deriving of filterChildren because of expanding all mode
NodePtrs DesktopNode::filterChildren(NodePtrs& source, bool sort_results)
{
    if (current_display_mode_ == EXPAND_ALL)
    {
        if (!filtered_children_dirty_)
        {
            return filtered_children_;
        }
        // filtered_children_ == children_
        filtered_children_ = filterPosterity(true);
        return filtered_children_;
    }
    return ContainerNode::filterChildren(source, sort_results);
}

bool DesktopNode::updateChildren()
{
    if ((status_filter_ & NODE_NOT_ON_CLOUD) && !checkCloudCacheOrUpdate())
    {
        // pending scaning until cloud info is retrieved
        pending_scan_ = true;
        return true;
    }    

    switch (current_display_mode_)
    {
        case BY_FOLDER:
            updateChildrenByFolder();
            break;
        case BY_SORT:
            updateChildrenBySort();
            break;
        case EXPAND_ALL:
            updateChildrenByExpandingAll();
            break;
        default:
            return false;
    }
    setDirty(false);

    NodeChildenReadyArgs children_ready_args;
    children_ready_args.current_node_path = absolutePath();
    children_ready_args.succeeded = true;
    children_ready_args.children = filterChildren(children_);
    FireEvent(EventChildrenIsReady, children_ready_args);
    return true;
}

void DesktopNode::updateChildrenByFolder()
{
    clearChildren();
    scan(absolutePath(), children_);
}

bool DesktopNode::sort(Field by, SortOrder order)
{
    changeSortCriteria(by, order);
    setDirty(true); // need rescan the children
    return true;
}

void DesktopNode::updateChildrenBySort()
{
    clearChildren();
    CDKFileManager* file_manager = CDKFileManager::GetFileManager();
    const DKFileList& all_files = file_manager->allFiles();
    DKFileList::const_iterator itr = all_files.begin();
    for (; itr != all_files.end(); itr++)
    {
        PCDKFile file = *itr;
        if (file != 0)// && FileNode::testStatus(file->GetFilePath(), status_filter))
        {
            NodePtr book_node(new FileNode(this, file));
            children_.push_back(book_node);
        }
    }
}

void DesktopNode::setDisplayMode(DKDisplayMode display_mode)
{
    if (current_display_mode_ != display_mode)
    {
        // need re-filter children
        filtered_children_dirty_ = true;
        current_display_mode_ = display_mode;
    }
}

void DesktopNode::updateChildrenByExpandingAll()
{
    //clearChildren();
    filterPosterity(true);
}

/// Update children node but does not re-generate the child list.
NodePtrs DesktopNode::updateChildrenInfo()
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

    return filterChildren(children_);
}

/// filter all posterities by status filter and replace current children
/// Do not check whether the cloud data is ready because directory mode is displayed by default
NodePtrs DesktopNode::filterPosterity(bool recursive)
{
    NodePtrs result;

    // do not rescan
    if (ContainerNode::filterPosterity(result, false, statusFilter(), recursive) > 0)
    {
        return result;
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
            updateChildren();
        }
    }
    return true;
}

NodePtrs DesktopNode::selectedLeaves(bool recursive)
{
//     if (current_display_mode_ == EXPAND_ALL)
//     {
//         NodePtrs result;
//         NodePtrs& ref = children_;
//         for (int i = 0; i < ref.size(); i++)
//         {
//             NodePtr child = ref[i];
//             if (child->selected())
//             {
//                 FileNode* file_node = dynamic_cast<FileNode*>(child.get());
//                 if (file_node != 0)
//                 {
//                     result.push_back(child);
//                 }
//             }
//         }
//         return result;
//     }
    return ContainerNode::selectedLeaves(recursive);
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

void DesktopNode::upload(bool exec_now)
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

    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    NodePtrs waiting_children = children(ret, false, statusFilter());
    if (ret == RETRIEVE_DONE)
    {
        for (size_t i = 0; i < waiting_children.size(); i++)
        {
            NodePtr child = waiting_children[i];
            NodeType child_type = child->type();
            if (child_type != NODE_TYPE_FILE_LOCAL_DOCUMENT &&
                child_type != NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK)
            {
                child->upload(exec_now);
            }
            else
            {
                if (child->selected() && child_type == NODE_TYPE_FILE_LOCAL_DOCUMENT)
                {
                    child->upload(exec_now);
                }
            }
        }
    }

    if (!exec_now)
    {
        MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
        cloud_service->flushPendingTasks();
    }
    return;
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
