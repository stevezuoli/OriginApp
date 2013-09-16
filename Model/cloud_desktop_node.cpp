#include "Model/cloud_desktop_node.h"
#include "Model/cloud_file_node.h"
#include "Model/cloud_category_node.h"
#include "Model/cloud_filesystem_tree.h"

#include "Utility/SystemUtil.h"
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
    , last_update_time_(0)
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
    fetchRootID();
}

CloudDesktopNode::~CloudDesktopNode()
{
    //CloudFileSystemTree::removeNodeFromGlobalMap(id());
    CloudFileSystemTree::removeNodeFromCloudLocalMap(id());
}

void CloudDesktopNode::fetchRootID()
{
    MiCloudManager* cloud_manager = MiCloudManager::GetInstance();
    cloud_manager->FetchBookRootDirID();
}

NodePtrs CloudDesktopNode::updateChildrenInfo()
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
    return filterChildren(children_);
}

bool CloudDesktopNode::updateChildren()
{
    setDirty(false);
    if (!id().empty())
    {
        scan(id(), children_);
    }
    else
    {
        MiCloudManager* cloud_manager = MiCloudManager::GetInstance();
        cloud_manager->FetchBookRootDirID();
    }
    return true;
}

void CloudDesktopNode::scan(const string& dir, NodePtrs &result)
{
    static const int UPDATE_OVERDUE = 20000;

    // fetch the category tree from mi cloud service.
    // the caller should wait until results returns and Nodes are ready
    time_t current_time = SystemUtil::GetUpdateTimeInMs();
    if (!children_.empty() && last_update_time_ != 0)
    {
        if (current_time - last_update_time_ < UPDATE_OVERDUE) // too short to fetch the children
        {
            NodeChildenReadyArgs children_ready_args;
            children_ready_args.succeeded = true;
            setDirty(false);
            children_ready_args.current_node_path = absolutePath();
            children_ready_args.children = filterChildren(children_);
            FireEvent(EventChildrenIsReady, children_ready_args);
            return;
        }
    }
    last_update_time_ = current_time;
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

bool CloudDesktopNode::remove(bool delete_local_files_if_possible, bool exec_now)
{
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs waiting_children = children(result, false, statusFilter());
    if (result == RETRIEVE_DONE)
    {
        for (size_t i = 0; i < waiting_children.size(); i++)
        {
            NodePtr child = waiting_children[i];
            NodeType child_type = child->type();
            if (child_type != NODE_TYPE_MICLOUD_BOOK)
            {
                child->remove(delete_local_files_if_possible, exec_now);
            }
            else
            {
                if (child->selected()) // remove thie file only if it's selected in multi-selection mode
                {
                    child->remove(delete_local_files_if_possible, exec_now);
                }
            }
        }
    }

    if (!exec_now)
    {
        MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
        cloud_service->flushPendingTasks();
    }
    return true;
}

void CloudDesktopNode::updateChildren(const MiCloudFileSPtrList& cloud_file_list)
{
    clearChildren();
    for (size_t i = 0; i < cloud_file_list.size(); ++i)
    {
        MiCloudFileSPtr cloud_file = cloud_file_list[i];
        if (cloud_file->type == MiCloudFile::FT_Dir)
        {
            //if (CloudCategoryNode::testStatus(cloud_file, statusFilter()))
            {
                NodePtr dir_node(new CloudCategoryNode(this, cloud_file));
                children_.push_back(dir_node);
            }
        }
        else if (cloud_file->type == MiCloudFile::FT_File)
        {
            //if (CloudFileNode::testStatus(cloud_file, statusFilter()))
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
        if (mutableId() != root_dir_args.id)
        {
            mutableId() = root_dir_args.id;
            RetrieveChildrenResult result = RETRIEVE_FAILED;
            children(result, true); //fetch nodes at the first time
        }

        mutableAbsolutePath() = MiCloudManager::GetInstance()->GetBookRootPath();
        mutableName() = CLOUD_DESKTOP_NAME;
        stat |= NODE_CLOUD;
        
        // add this node to global map
        //CloudFileSystemTree::addNodeToGlobalMap(this);
    }
    else
    {
        stat |= NODE_NOT_ON_CLOUD;
    }
    setStatus(stat);
    return true;
}

void CloudDesktopNode::download(bool exec_now)
{
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs waiting_children = children(result, false, statusFilter());
    if (result == RETRIEVE_DONE)
    {
        for (size_t i = 0; i < waiting_children.size(); i++)
        {
            NodePtr child = waiting_children[i];
            NodeType child_type = child->type();
            if (child_type != NODE_TYPE_MICLOUD_BOOK)
            {
                child->download(exec_now);
            }
            else
            {
                if (child->selected())
                {
                    child->download(exec_now);
                }
            }
        }
    }

    if (!exec_now)
    {
        MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
        cloud_service->flushPendingTasks();
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

    setDirty(!children_ready_args.succeeded);
    children_ready_args.current_node_path = absolutePath();
    children_ready_args.children = filterChildren(children_);
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
        NodePtr existing_node = node(cloud_file_info->name);
        if (existing_node != 0)
        {
            // do not create duplicate node
            return;
        }
        children_ready_args.succeeded = true;
        NodePtr file_node(new CloudFileNode(this, cloud_file_info));
        children_.push_back(file_node);
        filtered_children_dirty_ = true; // ask for re-filter the children
    }
    else
    {
        children_ready_args.succeeded = false;
        children_ready_args.reason = file_create_args.result->GetErrorReason();
    }

    children_ready_args.current_node_path = absolutePath();
    children_ready_args.children = filterChildren(children_);
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
        children_ready_args.succeeded = true;
        NodePtr dir_node(new CloudCategoryNode(this, cloud_dir_info));
        children_.push_back(dir_node);
        filtered_children_dirty_ = true; // ask for re-filter the children
    }
    else
    {
        children_ready_args.succeeded = false;
        children_ready_args.reason = dir_create_args.result->GetErrorReason();
    }

    children_ready_args.current_node_path = absolutePath();
    children_ready_args.children = filterChildren(children_);
    FireEvent(EventChildrenIsReady, children_ready_args);
}

void CloudDesktopNode::onFileDeleted(const EventArgs& args)
{
    const FileDeletedArgs& file_delete_args = dynamic_cast<const FileDeletedArgs&>(args);
    NodeChildenReadyArgs children_ready_args;
    if (file_delete_args.succeeded)
    {
        string delete_file_id = file_delete_args.file_id;
        NodePtrs& all = children_; //mutableChildren(ret, false, statusFilter());
        for(NodePtrs::iterator it = all.begin(); it != all.end(); ++it)
        {
            if ((*it)->id() == delete_file_id)
            {
                // delete the node in cache
                all.erase(it);
                children_ready_args.succeeded = true;
                filtered_children_dirty_ = true; // ask for re-filter the children
                break;
            }
        }
    }

    children_ready_args.current_node_path = absolutePath();
    children_ready_args.children = filterChildren(children_);
    FireEvent(EventChildrenIsReady, children_ready_args);
}

void CloudDesktopNode::onDirectoryDeleted(const EventArgs& args)
{
    const DirectoryDeletedArgs& dir_delete_args = dynamic_cast<const DirectoryDeletedArgs&>(args);
    NodeChildenReadyArgs children_ready_args;
    if (dir_delete_args.succeeded)
    {
        string delete_dir_id = dir_delete_args.directory_id;
        NodePtrs& all = children_; //mutableChildren(ret, false, statusFilter());
        for(NodePtrs::iterator it = all.begin(); it != all.end(); ++it)
        {
            if ((*it)->id() == delete_dir_id)
            {
                // delete the node in cache
                all.erase(it);
                children_ready_args.succeeded = true;
                filtered_children_dirty_ = true; // ask for re-filter the children
                break;
            }
        }
    }

    children_ready_args.current_node_path = absolutePath();
    children_ready_args.children = filterChildren(children_);
    FireEvent(EventChildrenIsReady, children_ready_args);
}

}

}
