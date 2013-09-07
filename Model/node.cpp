#include "Model/node.h"

#include "Utility/PathManager.h"
#include "Utility/FileSystem.h"

#include "XiaoMi/MiCloudService.h"
#include "XiaoMi/XiaoMiServiceFactory.h"
#include "XiaoMi/MiCloudManager.h"

using namespace std;
using namespace dk::utility;
using namespace xiaomi;

namespace dk {

namespace document_model {

const char* Node::EventChildrenIsReady = "EventChildrenIsReady";
const char* Node::EventNodeStatusUpdated = "EventNodeStatusUpdated";
const char* Node::EventNodeLoadingFinished = "EventNodeLoadingFinished";

const size_t Node::INVALID_ORDER = 0xffffffff;
const string Node::DATE_FORMAT  = "yyyy-MM-dd hh:mm";

Node::Node(Node*p)
    : parent_(p)
    , last_read_(0)
    , type_(NODE_TYPE_NULL)
    , status_(NODE_NONE)
    , selected_(parent_ != 0 ? parent_->selected() : false) // derive the status of selecting from parent
{
}

Node::~Node()
{
}

SPtr<ITpImage> Node::getInitialImage()
{
    return SPtr<ITpImage>();
}

string nodeDisplayName(const std::string& path)
{
    return PathManager::GetDirectoryDisplayName(path);
}

string nodeName(const std::string& path)
{
    std::string part1, part2;
    PathManager::SplitLastPath(path.c_str(), &part1, &part2);
    return part2;
}

Node* Node::mutableRoot()
{
    Node* ancester = mutableParent();
    if (ancester == 0)
    {
        // itself is the rootnode
        return this;
    }
    while (ancester->mutableParent() != 0)
    {
        ancester = ancester->mutableParent();
    }
    return ancester;
}

string Node::absolutePathFromAncestor()
{
    Node* parent = mutableParent();
    string current_path;
    if (parent != 0)
    {
        string parent_path = parent->absolutePathFromAncestor();
        current_path = PathManager::ConcatPath(parent_path, name());
    }
    else
    {
        current_path = absolutePath();
    }
    return current_path;
}

string Node::relativePathFromRoot()
{
    Node* parent = mutableParent();
    string current_path;
    if (parent != 0)
    {
        string parent_path = parent->relativePathFromRoot();
        current_path = PathManager::ConcatPath(parent_path, name());
    }
    else
    {
        current_path = "/";
    }
    return current_path;
}

/// File path on cloud
/// NOTE: this function can only work for file node now and the cloud file path is constructed as:
/// "/books/filename"
string Node::cloudFilePath()
{
    string path;
    if (parent() == 0)
    {
        // for root node, return the root path of cloud filesystem directly
        path = MiCloudManager::GetInstance()->GetBookRootPath();
    }
    else
    {
        // TODO. support directory tree for cloud
        //path = PathManager::ConcatPath(mutableParent()->cloudFilePath(), name());
        path = PathManager::ConcatPath(mutableRoot()->cloudFilePath(), name());
    }
    return path;
}

bool Node::rename(const string& new_name, string& error_msg)
{
    // Do nothing by default
    return false;
}

bool Node::remove(bool delete_local_files_if_possible)
{
    // Do nothing by default
    return false;
}

void Node::upload()
{
    // Do nothing by default
}

void Node::download()
{
    // Do nothing by default
}

bool Node::loadingInfo(int& progress, int& state)
{
    // Return nothing by default
    return false;
}

void Node::setSelected(bool selected)
{
    selected_ = selected;
}

void Node::setStatus(int status, bool mandatory)
{
    if (status_ != status || mandatory)
    {
        status_ = status;
        NodeStatusUpdated status_update_args;
        status_update_args.current_node_path = absolutePath();
        status_update_args.new_status = status_;
        FireEvent(EventNodeStatusUpdated, status_update_args);
    }
}

}  // namespace model

}  // namespace explorer
