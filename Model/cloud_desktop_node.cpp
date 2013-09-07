#include "Model/cloud_desktop_node.h"
#include "Model/cloud_file_node.h"
#include "Model/cloud_category_node.h"
#include "Model/cloud_filesystem_tree.h"

#include <tr1/functional>
#include "XiaoMi/MiCloudService.h"
#include "XiaoMi/XiaoMiServiceFactory.h"
#include "XiaoMi/MiCloudManager.h"

using namespace std;
using namespace dk::bookstore;

namespace dk {

namespace document_model {

static const char* CLOUD_DESKTOP_NAME = "BOOKS";

CloudDesktopNode::CloudDesktopNode(Node * parent)
    : ContainerNode(parent)
    , current_display_mode_(BY_FOLDER)
{
    status_ = NODE_NONE;
    mutableType() = NODE_TYPE_MICLOUD_DESKTOP;

    // fetch root node
    MiCloudManager* cloud_manager = MiCloudManager::GetInstance();
    SubscribeMessageEvent(
        MiCloudManager::BookRootDirCreated,
            *cloud_manager,
            std::tr1::bind(
            std::tr1::mem_fn(&CloudDesktopNode::onCloudRootCreated),
                this,
                std::tr1::placeholders::_1));
    cloud_manager->FetchBookRootDirID();
}

CloudDesktopNode::~CloudDesktopNode()
{
    CloudFileSystemTree::removeNodeFromGlobalMap(id());
    DeletePtrContainer(&children_);
}

NodePtrs& CloudDesktopNode::updateChildrenInfo()
{
    for (NodePtrsIter iter = children_.begin(); iter != children_.end(); ++iter)
    {
        // Update information if it's file node.
        if ((*iter)->type() == NODE_TYPE_MICLOUD_BOOK)
        {
            dynamic_cast<CloudFileNode*>((*iter).get())->update();
        }
        else if ((*iter)->type() == NODE_TYPE_MICLOUD_CATEGORY)
        {
            dynamic_cast<CloudCategoryNode*>((*iter).get())->updateChildrenInfo();
        }
    }
    sort(children_, by_field_, sort_order_);
    return children_;
}

void CloudDesktopNode::search(const string& keyword)
{
    // TODO.
}

bool CloudDesktopNode::updateChildren(int status_filter)
{
    if (!id().empty())
    {
        scan(id(), children_, status_filter, true);
        return true;
    }
    return false;
}

void CloudDesktopNode::scan(const string& dir, NodePtrs &result, int status_filter, bool sort_list)
{
    // fetch the category tree from mi cloud service.
    // the caller should wait until results returns and Nodes are ready
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    cloud_service->GetChildren(dir);
}

void CloudDesktopNode::createFile(const string& file_path)
{
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    cloud_service->CreateFile(id(), file_path);
}

void CloudDesktopNode::createDirectory(const string& dir_name)
{
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    cloud_service->CreateDirectory(id(), dir_name);
}

void CloudDesktopNode::deleteFile(const string& file_id)
{
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    cloud_service->DeleteFile(file_id);
}

void CloudDesktopNode::deleteDirectory(const string& dir_id)
{
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    cloud_service->DeleteDirectory(dir_id);
}

bool CloudDesktopNode::remove(bool delete_local_files_if_possible)
{
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs waiting_children = children(result, false);
    if (result = RETRIEVE_DONE)
    {
        for (size_t i = 0; i < waiting_children.size(); i++)
        {
            NodePtr child = waiting_children[i];
            NodeType child_type = child->type();
            if (child_type != NODE_TYPE_MICLOUD_BOOK)
            {
                child->remove(delete_local_files_if_possible);
            }
            else
            {
                if (child->selected()) // remove thie file only if it's selected in multi-selection mode
                {
                    child->remove(delete_local_files_if_possible);
                }
            }
        }
    }
    return true;
}

void CloudDesktopNode::updateChildren(const MiCloudFileSPtrList& cloud_file_list)
{
    DeletePtrContainer(&children_);
    for (size_t i = 0; i < cloud_file_list.size(); ++i)
    {
        MiCloudFileSPtr cloud_file = cloud_file_list[i];
        if (cloud_file->type == MiCloudFile::FT_Dir)
        {
            if (CloudCategoryNode::testStatus(cloud_file, statusFilter()))
            {
                NodePtr dir_node(new CloudCategoryNode(this, cloud_file));
                children_.push_back(dir_node);
            }
        }
        else if (cloud_file->type == MiCloudFile::FT_File)
        {
            if (CloudFileNode::testStatus(cloud_file, statusFilter()))
            {
                NodePtr file_node(new CloudFileNode(this, cloud_file));
                children_.push_back(file_node);
            }
        }
    }
}

bool CloudDesktopNode::onCloudRootCreated(const EventArgs& args)
{
    const BookRootDirCreatedArgs& root_dir_args = dynamic_cast<const BookRootDirCreatedArgs&>(args);
    int stat = status();
    if (root_dir_args.succeeded)
    {
        mutableId() = root_dir_args.id;
        mutableAbsolutePath() = MiCloudManager::GetInstance()->GetBookRootPath();
        mutableName() = CLOUD_DESKTOP_NAME;
        stat |= NODE_CLOUD;
        
        // add this node to global map
        CloudFileSystemTree::addNodeToGlobalMap(this);

        RetrieveChildrenResult result = RETRIEVE_FAILED;
        children(result, true); //fetch nodes at the first time
    }
    else
    {
        stat |= NODE_NOT_ON_CLOUD;
    }
    setStatus(stat);
    return true;
}

void CloudDesktopNode::download()
{
    downloadChildren();
}

void CloudDesktopNode::downloadChildren()
{
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs waiting_children = children(result, false, NODE_CLOUD | NODE_NOT_ON_LOCAL);
    if (result = RETRIEVE_DONE)
    {
        for (size_t i = 0; i < waiting_children.size(); i++)
        {
            NodePtr child = waiting_children[i];
            NodeType child_type = child->type();
            if (child_type != NODE_TYPE_MICLOUD_BOOK)
            {
                child->download();
            }
            else
            {
                if (child->selected())
                {
                    child->download();
                }
            }
        }
    }
}

void CloudDesktopNode::onChildrenReturned(const EventArgs& args)
{
    const ChildrenRetrievedArgs& children_args = dynamic_cast<const ChildrenRetrievedArgs&>(args);
    NodeChildenReadyArgs children_ready_args;
    if (children_args.succeeded && children_args.result != 0)
    {
        children_ready_args.succeeded = true;
        updateChildren(children_args.result->GetFileList());   
    }
    else
    {
        children_ready_args.succeeded = false;
        children_ready_args.reason = children_args.result->GetErrorReason();
    }

    dirty_ = !children_ready_args.succeeded;
    children_ready_args.current_node_path = absolutePath();
    children_ready_args.children = children_;
    FireEvent(EventChildrenIsReady, children_ready_args);
}

void CloudDesktopNode::onFileCreated(const EventArgs& args)
{
    const FileCreatedArgs & file_create_args = dynamic_cast<const FileCreatedArgs&>(args);
    NodeChildenReadyArgs children_ready_args;
    if (file_create_args.succeeded && file_create_args.result != 0)
    {
        MiCloudServiceResultCreateFileSPtr create_info = file_create_args.result;
        MiCloudFileSPtr cloud_file_info = create_info->GetFileInfo();
        if (CloudFileNode::testStatus(cloud_file_info, statusFilter()))
        {
            children_ready_args.succeeded = true;
            NodePtr file_node(new CloudFileNode(this, cloud_file_info));
            children_.push_back(file_node);
            sort(children_, by_field_, sort_order_);
        }
    }
    else
    {
        children_ready_args.succeeded = false;
        children_ready_args.reason = file_create_args.result->GetErrorReason();
    }

    children_ready_args.current_node_path = absolutePath();
    children_ready_args.children = children_;
    FireEvent(EventChildrenIsReady, children_ready_args);
}

void CloudDesktopNode::onDirectoryCreated(const EventArgs& args)
{
    const DirectoryCreatedArgs & dir_create_args = dynamic_cast<const DirectoryCreatedArgs&>(args);
    NodeChildenReadyArgs children_ready_args;
    if (dir_create_args.succeeded && dir_create_args.result != 0)
    {
        MiCloudServiceResultCreateDirSPtr create_info = dir_create_args.result;
        MiCloudFileSPtr cloud_dir_info = create_info->GetDirInfo();
        if (CloudCategoryNode::testStatus(cloud_dir_info, statusFilter()))
        {
            children_ready_args.succeeded = true;
            NodePtr dir_node(new CloudCategoryNode(this, cloud_dir_info));
            children_.push_back(dir_node);
            sort(children_, by_field_, sort_order_);
        }
    }
    else
    {
        children_ready_args.succeeded = false;
        children_ready_args.reason = dir_create_args.result->GetErrorReason();
    }

    children_ready_args.current_node_path = absolutePath();
    children_ready_args.children = children_;
    FireEvent(EventChildrenIsReady, children_ready_args);
}

void CloudDesktopNode::onFileDeleted(const EventArgs& args)
{
    const FileDeletedArgs& file_delete_args = dynamic_cast<const FileDeletedArgs&>(args);
    NodeChildenReadyArgs children_ready_args;
    if (file_delete_args.succeeded)
    {
        string delete_file_id = file_delete_args.file_id;
        RetrieveChildrenResult ret = RETRIEVE_FAILED;
        NodePtrs& all = mutableChildren(ret, false, statusFilter());
        if (ret == RETRIEVE_DONE)
        {
            for(NodePtrs::iterator it = all.begin(); it != all.end(); ++it)
            {
                if ((*it)->id() == delete_file_id)
                {
                    // delete the node in cache
                    all.erase(it);
                    children_ready_args.succeeded = true;
                    break;
                }
            }
        }
    }

    children_ready_args.current_node_path = absolutePath();
    children_ready_args.children = children_;
    FireEvent(EventChildrenIsReady, children_ready_args);
}

void CloudDesktopNode::onDirectoryDeleted(const EventArgs& args)
{
    const DirectoryDeletedArgs& dir_delete_args = dynamic_cast<const DirectoryDeletedArgs&>(args);
    NodeChildenReadyArgs children_ready_args;
    if (dir_delete_args.succeeded)
    {
        string delete_dir_id = dir_delete_args.directory_id;
        RetrieveChildrenResult ret = RETRIEVE_FAILED;
        NodePtrs& all = mutableChildren(ret, false, statusFilter());
        if (ret == RETRIEVE_DONE)
        {
            for(NodePtrs::iterator it = all.begin(); it != all.end(); ++it)
            {
                if ((*it)->id() == delete_dir_id)
                {
                    // delete the node in cache
                    all.erase(it);
                    children_ready_args.succeeded = true;
                    break;
                }
            }
        }
    }

    children_ready_args.current_node_path = absolutePath();
    children_ready_args.children = children_;
    FireEvent(EventChildrenIsReady, children_ready_args);
}

}

}
