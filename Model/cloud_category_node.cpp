#include "Model/cloud_category_node.h"
#include "Model/cloud_file_node.h"
#include "Model/cloud_filesystem_tree.h"

#include "XiaoMi/MiCloudService.h"
#include "XiaoMi/XiaoMiServiceFactory.h"

#include "Utility/ImageManager.h"

using namespace xiaomi;

namespace dk {

namespace document_model {

CloudCategoryNode::CloudCategoryNode(Node * parent, MiCloudFileSPtr category_info)
    : ContainerNode(parent)
    , category_info_(category_info)
{
    status_ = NODE_CLOUD;
    mutableType() = NODE_TYPE_MICLOUD_CATEGORY;
    mutableName() = category_info_->name;
    mutableDisplayName() = category_info_->name;
    mutableDescription() = category_info_->name;
    mutableId() = category_info_->id;
    mutableAbsolutePath() = absolutePathFromAncestor();
    mutableCoverPath() = ImageManager::GetImagePath(IMAGE_ICON_COVER_DIR);
    CloudFileSystemTree::addNodeToGlobalMap(this);
}

CloudCategoryNode::~CloudCategoryNode()
{
    CloudFileSystemTree::removeNodeFromGlobalMap(id());
    DeletePtrContainer(&children_);
}

NodePtrs& CloudCategoryNode::updateChildrenInfo()
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
    sort(children_, by_field_, sort_order_);
    return children_;
}

size_t CloudCategoryNode::nodePosition(NodePtr node)
{
    // check
    if (node == 0)
    {
        return INVALID_ORDER;
    }

    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    const NodePtrs& nodes  = children(ret, false, statusFilter());
    if (ret == RETRIEVE_DONE)
    {
        NodePtrs::const_iterator it = find(nodes.begin(), nodes.end(), node);
        if (it == nodes.end())
        {
            return INVALID_ORDER;
        }
        return it - nodes.begin();
    }
    return INVALID_ORDER;
}

size_t CloudCategoryNode::nodePosition(const string &name)
{
    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    const NodePtrs& all = children(ret, false, statusFilter());
    if (ret == RETRIEVE_DONE)
    {
        for(NodePtrs::const_iterator it = all.begin(); it != all.end(); ++it)
        {
            if ((*it)->name() == name)
            {
                return it - all.begin();
            }
        }
    }
    return INVALID_ORDER;
}

bool CloudCategoryNode::search(const StringList& filters, bool recursive, bool & stop)
{
    // Not support now
    return false;
}

bool CloudCategoryNode::testStatus(MiCloudFileSPtr category, int status_filter)
{
    // TODO. add filter for bookstore category
    return true;
}

bool CloudCategoryNode::updateChildren(int status_filter)
{
    // id must be available
    scan(id(), children_, status_filter, true);
    return true;
}

void CloudCategoryNode::scan(const string& dir, NodePtrs &result, int status_filter, bool sort_list)
{
    MiCloudService* cloud_service = XiaoMiServiceFactory::GetMiCloudService();
    cloud_service->GetChildren(dir);
}

void CloudCategoryNode::collectDirectories(const string &dir, StringList & result)
{
    // TODO.
}

void CloudCategoryNode::download()
{
    downloadChildren();
}

void CloudCategoryNode::downloadChildren()
{
    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    NodePtrs waiting_children = children(ret, false, NODE_CLOUD | NODE_NOT_ON_LOCAL);
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

bool CloudCategoryNode::remove(bool delete_local_files_if_possible)
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

    // TODO. delete itself if selected()
    return true;
}

void CloudCategoryNode::updateChildren(const MiCloudFileSPtrList& cloud_file_list)
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

    // Sort.
    sort(children_, by_field_, sort_order_);
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

    dirty_ = !children_ready_args.succeeded;
    children_ready_args.current_node_path = absolutePath();
    children_ready_args.children = children_;
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
    mutableRoot()->FireEvent(EventChildrenIsReady, children_ready_args);
}

void CloudCategoryNode::onFileDeleted(const EventArgs& args)
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
            for (NodePtrs::iterator it = all.begin(); it != all.end(); ++it)
            {
                if ((*it)->id() == delete_file_id)
                {
                    // delete the node in cache
                    all.erase(it);
                    children_ready_args.succeeded = true;
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
    }

    children_ready_args.current_node_path = absolutePath();
    children_ready_args.children = children_;
    mutableRoot()->FireEvent(EventChildrenIsReady, children_ready_args);
}

void CloudCategoryNode::onDirectoryDeleted(const EventArgs& args)
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
    mutableRoot()->FireEvent(EventChildrenIsReady, children_ready_args);
}

SPtr<ITpImage> CloudCategoryNode::getInitialImage()
{
    return ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_FOLDER);
}

}

}