#include <algorithm>
#include <map>

#include "Model/container_node.h"

namespace dk {

namespace document_model {

ContainerNode::ContainerNode(Node * p)
    : Node(p)
    , by_field_(NAME)
    , sort_order_(ASCENDING)
    , children_()
    , status_filter_(NODE_NONE)
    , dirty_(true)
{
}

ContainerNode::~ContainerNode()
{
    DeletePtrContainer(&children_);
}

const NodePtrs& ContainerNode::children(RetrieveChildrenResult& result, bool rescan, int status_filter)
{
    mutableStatusFilter()= status_filter;
    if (rescan || dirty_)
    {
        dirty_ = false;
        if (!updateChildren(status_filter))
        {
            dirty_ = true;
            result = RETRIEVE_FAILED;
        }
        else
        {
            result = RETRIEVE_PENDING;
        }
    }
    else
    {
        result = RETRIEVE_DONE;
    }
    return children_;
}

NodePtrs& ContainerNode::mutableChildren(RetrieveChildrenResult& result, bool rescan, int status_filter)
{
    mutableStatusFilter()= status_filter;
    if (rescan || dirty_)
    {
        dirty_ = false;
        if (!updateChildren(status_filter))
        {
            dirty_ = true;
            result = RETRIEVE_FAILED;
        }
        else
        {
            result = RETRIEVE_PENDING;
        }
    }
    else
    {
        result = RETRIEVE_DONE;
    }
    return children_;
}

bool ContainerNode::updateChildren(int status_filter)
{
    // Do Nothing
    return false;
}

void ContainerNode::setSelected(bool selected)
{
    Node::setSelected(selected);

    // select all children
    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    NodePtrs selected_children = children(ret, false, statusFilter());
    if (ret == RETRIEVE_DONE)
    {
        for (size_t i = 0; i < selected_children.size(); i++)
        {
            selected_children[i]->setSelected(selected);
        }
    }
}

/// Update children node but does not re-generate the child list.
NodePtrs& ContainerNode::updateChildrenInfo()
{
    return children_;
}

void ContainerNode::clearChildren()
{
    DeletePtrContainer(&children_);
}

bool ContainerNode::sort(NodePtrs &nodes, Field by, SortOrder order)
{
    switch (by)
    {
    case NAME:
        if (order == ASCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), LessByName());
        }
        else if (order == DESCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), GreaterByName());
        }
        break;
    case SIZE:
        if (order == ASCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), LessBySize());
        }
        else if (order == DESCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), GreaterBySize());
        }
        break;
    case RATING:
        //if (order == ASCENDING)
        //{
        //    std::sort(nodes.begin(), nodes.end(), LessByRating());
        //}
        //else if (order == DESCENDING)
        //{
        //    std::sort(nodes.begin(), nodes.end(), GreaterByRating());
        //}
        break;
    case LAST_ACCESS_TIME:
        if (order == ASCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), LessByLastRead());
        }
        else if (order == DESCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), GreaterByLastRead());
        }
        break;
    case NODE_TYPE:
        if (order == ASCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), LessByNodetype());
        }
        else if (order == DESCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), GreaterByNodeType());
        }
        break;
    // TODO. Add support for these types
    case BY_DIRECTORY:
    case RECENTLY_ADD:
    case TITLE:
    default:
        return false;
    }
    return true;
}


size_t ContainerNode::nodePosition(NodePtr node)
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
        else
        {
            return it - nodes.begin();
        }
    }
    return INVALID_ORDER;
}

NodePtr ContainerNode::node(const string &name, bool recursive)
{
    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    NodePtrs& all = mutableChildren(ret, false, statusFilter());
    if (ret == RETRIEVE_DONE)
    {
        for(NodePtrs::iterator it = all.begin(); it != all.end(); ++it)
        {
            if ((*it)->name() == name)
            {
                return *it;
            }
        }
    }

    if (recursive)
    {
        for(NodePtrs::iterator it = all.begin(); it != all.end(); ++it)
        {
            ContainerNode* child_container = dynamic_cast<ContainerNode*>((*it).get());
            if (child_container != 0)
            {
                NodePtr result = child_container->node(name, recursive);
                if (result != 0)
                {
                    return result;
                }
            }
        }
    }
    return NodePtr();
}

NodePtr ContainerNode::node(NodeType type)
{
    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    NodePtrs& all = mutableChildren(ret, false, statusFilter());
    if (ret == RETRIEVE_DONE)
    {
        for(NodePtrs::iterator it = all.begin(); it != all.end(); ++it)
        {
            if ((*it)->type() == type)
            {
                return *it;
            }
        }
    }
    return NodePtr();
}

/// Return node position by name.
size_t ContainerNode::nodePosition(const string &name)
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

/// Only change the sort criteria does not really sort.
void ContainerNode::changeSortCriteria(Field by, SortOrder order)
{
    by_field_ = by;
    sort_order_ = order;
}

/// Change sort criteria and sort.
bool ContainerNode::sort(Field by, SortOrder order)
{
    if (!sort(children_, by, order))
    {
        return false;
    }

    changeSortCriteria(by, order);
    return true;
}

/// Search from current directory by using the specified name filters.
/// Recursively search if needed.
bool ContainerNode::search(const StringList &name_filters,
                           bool recursively,
                           bool & stop)
{
    return false;
}

void ContainerNode::clearNameFilters()
{
    name_filters_.clear();
}

/// Filter all posterities to support expand mode. NOTE: this function does not work on asynchronous mode
int ContainerNode::filterPosterity(NodePtrs& nodes, int status_filter, bool recursive)
{
    NodePtrs result;
    status_filter_ = status_filter;
    //DeletePtrContainer(&children_);

    // Search from current directory.
    if (!recursive)
    {
        scan(absolutePath(), result, status_filter, false);
        nodes.insert(nodes.end(), result.begin(), result.end());
        children_ = result;
        dirty_ = false;
        return result.size();
    }

    int posterity_num = 0;
    scan(absolutePath(), result, status_filter, false);
    children_ = result;
    dirty_ = false;
    posterity_num = result.size(); // record number of children

    for (size_t i = 0; i < result.size(); i++)
    {
        NodePtr child = result[i];
        nodes.push_back(child);
        ContainerNode* container = dynamic_cast<ContainerNode*>(child.get());
        if (container != 0)
        {
            int sub_posterity_num = container->filterPosterity(nodes, status_filter, true);
            if (sub_posterity_num > 0)
            {
                posterity_num += sub_posterity_num;
            }
            else
            {
                nodes.pop_back();
                posterity_num--; // if this container does not have any qualified posterity, do not take it int account.
            }
        }
    }
    return posterity_num;
}

// CRUD
void ContainerNode::createFile(const string& file_path)
{
    // do nothing
}

void ContainerNode::createDirectory(const string& dir_name)
{
    // do nothing
}

void ContainerNode::deleteFile(const string& file_id)
{
    // do nothing
}

void ContainerNode::deleteDirectory(const string& dir_id)
{
    // do nothing
}

}  // namespace document_model

}  // namespace dk
