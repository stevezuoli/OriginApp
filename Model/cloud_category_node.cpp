#include "Model/cloud_category_node.h"
#include "Model/cloud_file_node.h"
#include "Model/cloud_filesystem_tree.h"

#include "XiaoMi/MiCloudService.h"
#include "XiaoMi/XiaoMiServiceFactory.h"

#include "Utility/ImageManager.h"
#include "Utility/SystemUtil.h"
using namespace xiaomi;

namespace dk {

namespace document_model {

CloudCategoryNode::CloudCategoryNode(Node * parent, MiCloudFileSPtr category_info)
    : ContainerNode(parent)
    , category_info_(category_info)
    , last_update_time_(0)
{
    status_ = NODE_CLOUD;
    mutableType() = NODE_TYPE_MICLOUD_CATEGORY;
    mutableName() = category_info_->name;
    mutableDisplayName() = category_info_->name;
    mutableGbkName() = EncodeUtil::UTF8ToGBKString(displayName());
    mutableDescription() = category_info_->name;
    mutableId() = category_info_->id;
    mutableAbsolutePath() = absolutePathFromAncestor();
    mutableCoverPath() = ImageManager::GetImagePath(IMAGE_ICON_COVER_DIR);
    //CloudFileSystemTree::addNodeToGlobalMap(this);
}

CloudCategoryNode::~CloudCategoryNode()
{
    //CloudFileSystemTree::removeNodeFromGlobalMap(id());
    CloudFileSystemTree::removeNodeFromCloudLocalMap(id());
}

NodePtrs CloudCategoryNode::updateChildrenInfo()
{
    for(NodePtrsIter iter = children_.begin(); iter != children_.end(); ++iter)
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

bool CloudCategoryNode::testStatus(MiCloudFileSPtr category, int status_filter)
{
    // TODO. add filter for bookstore category
    return true;
}

bool CloudCategoryNode::updateChildren()
{
    setDirty(false);
    scan(id(), children_);
    return true;
}

void CloudCategoryNode::scan(const string& dir, NodePtrs &result)
{
    static const int UPDATE_OVERDUE = 15000;

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

void CloudCategoryNode::collectDirectories(const string &dir, StringList & result)
{
    // TODO.
}

void CloudCategoryNode::download(bool exec_now)
{
    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    NodePtrs waiting_children = children(ret, false, statusFilter());
    if (ret == RETRIEVE_DONE)
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

    // reset selected status
    Node::download(exec_now);
}

/// Not used now
void CloudCategoryNode::createFile(const string& file_path)
{
    // this function may not be used
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    cloud_service->CreateFile(id(), file_path);
}

void CloudCategoryNode::createDirectory(const string& dir_name)
{
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    cloud_service->CreateDirectory(id(), dir_name);
}

void CloudCategoryNode::deleteFile(const string& file_id)
{
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    cloud_service->DeleteFile(file_id);
}

void CloudCategoryNode::deleteDirectory(const string& dir_id)
{
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    cloud_service->DeleteDirectory(dir_id);
}

bool CloudCategoryNode::remove(bool delete_local_files_if_possible, bool exec_now)
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

    // TODO. delete itself if selected()
    return true;
}

void CloudCategoryNode::updateChildren(const MiCloudFileSPtrList& cloud_file_list)
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

    // Sort.
    //sort(children_, by_field_, sort_order_);
}

void CloudCategoryNode::onChildrenReturned(const EventArgs& args)
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
    mutableRoot()->FireEvent(EventChildrenIsReady, children_ready_args);
}

void CloudCategoryNode::onFileCreated(const EventArgs& args)
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
        filtered_children_dirty_ = true;
    }
    else
    {
        children_ready_args.succeeded = false;
        children_ready_args.reason = file_create_args.result->GetErrorReason();
    }

    children_ready_args.current_node_path = absolutePath();
    children_ready_args.children = filterChildren(children_);
    mutableRoot()->FireEvent(EventChildrenIsReady, children_ready_args);
}

void CloudCategoryNode::onDirectoryCreated(const EventArgs& args)
{
    const DirectoryCreatedArgs & dir_create_args = dynamic_cast<const DirectoryCreatedArgs&>(args);
    NodeChildenReadyArgs children_ready_args;
    if (dir_create_args.succeeded && dir_create_args.result != 0)
    {
        MiCloudServiceResultCreateDirSPtr create_info = dir_create_args.result;
        MiCloudFileSPtr cloud_dir_info = create_info->GetDirInfo();
        //if (CloudCategoryNode::testStatus(cloud_dir_info, statusFilter()))
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

    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    children_ready_args.children = children(ret, false, statusFilter(), nameFilter());

    mutableRoot()->FireEvent(EventChildrenIsReady, children_ready_args);
}

void CloudCategoryNode::onFileDeleted(const EventArgs& args)
{
    const FileDeletedArgs& file_delete_args = dynamic_cast<const FileDeletedArgs&>(args);
    NodeChildenReadyArgs children_ready_args;
    if (file_delete_args.succeeded)
    {
        string delete_file_id = file_delete_args.file_id;
        NodePtrs& all = children_; //mutableChildren(ret, false, statusFilter());
        for (NodePtrs::iterator it = all.begin(); it != all.end(); ++it)
        {
            if ((*it)->id() == delete_file_id)
            {
                // delete the node in cache
                all.erase(it);
                children_ready_args.succeeded = true;
                filtered_children_dirty_ = true; // re-filter children
                break;
            }
        }

        // NOTE. the directory can only be deleted when all children are removed
        if (all.empty() && selected())
        {
            // delete myself
            deleteDirectory(id());
        }
    }

    children_ready_args.current_node_path = absolutePath();

    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    children_ready_args.children = children(ret, false, statusFilter(), nameFilter());

    mutableRoot()->FireEvent(EventChildrenIsReady, children_ready_args);
}

void CloudCategoryNode::onDirectoryDeleted(const EventArgs& args)
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
                filtered_children_dirty_ = true;
                break;
            }
        }
    }

    children_ready_args.current_node_path = absolutePath();
    if (!isDirty())
    {
        RetrieveChildrenResult ret = RETRIEVE_FAILED;
        children_ready_args.children = children(ret, false, statusFilter(), nameFilter());
    }
    mutableRoot()->FireEvent(EventChildrenIsReady, children_ready_args);
}

SPtr<ITpImage> CloudCategoryNode::getInitialImage()
{
    return ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_FOLDER);
}

bool CloudCategoryNode::supportedCommands(std::vector<int>& command_ids, std::vector<int>& str_ids)
{
    // TODO. Implement Me
    return false;
}

}

}