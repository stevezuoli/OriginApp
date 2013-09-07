#include "Model/dkstore_filesystem_tree.h"    
#include "BookStore/BookStoreInfoManager.h"

using namespace dk::bookstore;
using namespace dk::bookstore::model;

namespace dk {

namespace document_model {

BookStoreFileSystemTree::BookStoreFileSystemTree()
    : root_node_(0)
{
    initialize();
}

BookStoreFileSystemTree::~BookStoreFileSystemTree()
{
}

void BookStoreFileSystemTree::initialize()
{
    setCurrentNode(&root_node_);
}

ContainerNode * BookStoreFileSystemTree::root()
{
    return &root_node_;
}

ContainerNode * BookStoreFileSystemTree::currentNode()
{
    return current_node_;
}

NodeType BookStoreFileSystemTree::currentType()
{
    return current_node_->type();
}

ContainerNode * BookStoreFileSystemTree::cdRootNode()
{
    return setCurrentNode(&root_node_);
}

ContainerNode * BookStoreFileSystemTree::cdContainerNodeWithinTopNode(const string& name)
{
    Node* ptr = root_node_.node(name);
    if (ptr == 0)
    {
        return 0;
    }
    return setCurrentNode(dynamic_cast<ContainerNode *>(ptr));
}

ContainerNode * BookStoreFileSystemTree::cd(NodePtr p)
{
    return setCurrentNode(dynamic_cast<ContainerNode *>(p));
}

ContainerNode * BookStoreFileSystemTree::cd(const string& name)
{
    // Get child node.
    NodePtr ptr = current_node_->node(name);
    if (ptr != 0)
    {
        return setCurrentNode(dynamic_cast<ContainerNode *>(ptr));
    }
    return 0;
}

ContainerNode * BookStoreFileSystemTree::cdUp()
{
    if (!canGoUp())
    {
        return 0;
    }

    return setCurrentNode(dynamic_cast<ContainerNode *>(current_node_->mutableParent()));
}

bool BookStoreFileSystemTree::cdPath(const ModelPath& path)
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

bool BookStoreFileSystemTree::cdPath(const string& path)
{
    // traverse the whole tree to find the node because path here is actually the id of category or book id
    // Not support now
    return false;
}

bool BookStoreFileSystemTree::canGoUp()
{
    if (!canGoUp())
    {
        return 0;
    }
    return setCurrentNode(dynamic_cast<ContainerNode *>(current_node_->mutableParent()));
}

Field BookStoreFileSystemTree::sortField() const
{
    return by_;
}

SortOrder BookStoreFileSystemTree::sortOrder() const
{
    return order_;
}

void BookStoreFileSystemTree::setSortCriteria(Field by, SortOrder order)
{
    by_ = by;
    order_ = order;
}

void BookStoreFileSystemTree::setSortField(Field by)
{
    by_ = by;
}

void BookStoreFileSystemTree::sort()
{
    // TODO. Update sort results
    current_node_->sort(by_, order_);
}

DKDisplayMode BookStoreFileSystemTree::displayMode()
{
    return root_node_.currentDisplayMode();
}

void BookStoreFileSystemTree::setDisplayMode(DKDisplayMode display_mode)
{
    root_node_.mutableDisplayMode() = display_mode;
    root_node_.setDirty(true);
}

void BookStoreFileSystemTree::search(const string& keyword, int start, int num)
{
    root_node_.search(keyword, NODE_NONE, start, num);
}

ContainerNode* BookStoreFileSystemTree::setCurrentNode(ContainerNode *node)
{
    current_node_ = node;
    mapFilePathToModelPath(current_node_, current_path_);
    return current_node_;
}

bool BookStoreFileSystemTree::mapFilePathToModelPath(NodePtr node, ModelPath & model_path)
{
    ModelPath reversed_path;
    NodePtr current_node = node;
    while (current_node != 0)
    {
        reversed_path.push_back(current_node->absolutePath());
        current_node = current_node->mutableParent();
    }
    if (reversed_path.empty())
    {
        return false;
    }

    model_path.clear();
    // reverse
    for (int i = reversed_path.size() - 1; i >= 0; i--)
    {
        model_path.push_back(reversed_path[i]);
    }
    return true;
}

ContainerNode * BookStoreFileSystemTree::containerNodeWithinTopNode(NodeType type)
{
    ContainerNode * node = dynamic_cast<ContainerNode *>(root_node_.node(type));
    return node;
}

}

}
