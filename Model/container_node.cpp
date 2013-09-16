#include <algorithm>
#include <map>

#include "Utility/Misc.h"
#include "Model/container_node.h"
#include "Utility/StringUtil.h"

using namespace dk::utility;

namespace dk {

namespace document_model {

NodeKeywordMatcher::NodeKeywordMatcher(const char* keyword)
        : keyword_(keyword)
        , match_by_first_letters_(false)
{
    if (NULL != keyword)
    {
        wide_keyword_ = StringUtil::ToLower(EncodeUtil::ToWString(keyword).c_str());
        while (('a' <= *keyword && *keyword <= 'z') || ('A' <= *keyword && *keyword <= 'Z'))
        {
            ++keyword;
        }
        if (0 == *keyword)
        {
            match_by_first_letters_ = true;
        }
    }
}

bool NodeKeywordMatcher::operator()(const NodePtr node) const
{
    if (wide_keyword_.empty())
    {
        return false;
    }

    // 7 hardcode for /mnt/us
    std::wstring wPath = StringUtil::ToLower(EncodeUtil::ToWString(node->name()).c_str());
    const wchar_t* lastDot = wcsrchr(wPath.c_str(), L'.');
    if (lastDot != 0)
    {
        wPath = wPath.substr(0, lastDot - wPath.c_str());
    }
        
    DK_AUTO(pos, wPath.find(wide_keyword_));
    if (pos != std::wstring::npos)
    {
        return true;
    }
    std::wstring wName = StringUtil::ToLower(EncodeUtil::ToWString(node->name()).c_str());
    if (wName.find(wide_keyword_) != std::wstring::npos)
    {
        return true;
    }
    if (!match_by_first_letters_)
    {
        return false;
    }
	if(StringUtil::MatchByFirstLetter(wPath, wide_keyword_))
	{
		return true;
	}
	if(StringUtil::MatchByFirstLetter(wName, wide_keyword_))
	{
		return true;
	}
    return false;
}

ContainerNode::ContainerNode(Node * p)
    : Node(p)
    , by_field_(NAME)
    , sort_order_(ASCENDING)
    , children_()
    , status_filter_(NODE_NONE)
    , dirty_(true)
    , filtered_children_dirty_(true)
{
}

ContainerNode::~ContainerNode()
{
    clearChildren();
}

NodePtrs ContainerNode::children(RetrieveChildrenResult& result,
                                 bool rescan,
                                 int status_filter,
                                 const string& keyword)
{
    changeStatusFilter(status_filter);
    changeNameFilter(keyword);
    if (rescan || dirty_)
    {
        setDirty(true);
        if (!updateChildren())
        {
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
        filterChildren(children_);
    }
    return filtered_children_;
}

//NodePtrs& ContainerNode::mutableChildren(RetrieveChildrenResult& result, bool rescan, int status_filter)
//{
//    if (statusFilter() != status_filter)
//    {
//        mutableStatusFilter()= status_filter;
//    }
//
//    if (rescan || dirty_)
//    {
//        dirty_ = false;
//        if (!updateChildren(status_filter))
//        {
//            dirty_ = true;
//            result = RETRIEVE_FAILED;
//        }
//        else
//        {
//            result = RETRIEVE_PENDING;
//        }
//    }
//    else
//    {
//        result = RETRIEVE_DONE;
//    }
//    return children_;
//}

bool ContainerNode::updateChildren()
{
    // Do Nothing
    return false;
}

/// Filter exisiting children
NodePtrs ContainerNode::filterChildren(NodePtrs& source, bool sort_results)
{
    if (!filtered_children_dirty_)
    {
        return filtered_children_;
    }

    DeletePtrContainer(&filtered_children_);
    NodePtrs::iterator itr = source.begin();
    while (itr != source.end())
    {
        NodePtr child = *itr;
        if (child->satisfy(statusFilter()))
        {
            filtered_children_.push_back(child);
        }
        itr++;
    }

    if (sort_results)
    {
        sort(filtered_children_, by_field_, sort_order_);
    }

    if (!name_filter_.empty())
    {
        filtered_children_ = search(filtered_children_, name_filter_);
    }
    filtered_children_dirty_ = false;
    return filtered_children_;
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
NodePtrs ContainerNode::updateChildrenInfo()
{
    return children_;
}

void ContainerNode::clearChildren()
{
    DeletePtrContainer(&filtered_children_);
    DeletePtrContainer(&children_);
    setDirty(true);
}

void ContainerNode::setDirty(bool dirty)
{
    dirty_ = dirty;
    if (dirty_)
    {
        // if children is dirty, the filtered children should also be set to dirty
        filtered_children_dirty_ = true;
    }
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
            std::sort(nodes.begin(), nodes.end(), LessByNodeType());
        }
        else if (order == DESCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), GreaterByNodeType());
        }
        break;
    // TODO. Add support for these types
    case BY_DIRECTORY:
        if (order == ASCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), LessBySize());
        }
        else if (order == DESCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), GreaterBySize());
        }
        break;
    case RECENTLY_ADD:
        if (order == ASCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), LessByCreateTime());
        }
        else if (order == DESCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), GreaterByCreateTime());
        }
        break;
    case NODE_STATUS:
        if (order == ASCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), LessByNodeStatus());
        }
        else if (order == DESCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), GreaterByNodeStatus());
        }
        break;
    case CREATE_TIME:
        if (order == ASCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), LessByCreateTime());
        }
        else if (order == DESCENDING)
        {
            std::sort(nodes.begin(), nodes.end(), GreaterByCreateTime());
        }
        break;        
    default:
        return false;
    }
    return true;
}

NodePtrs ContainerNode::search(NodePtrs& source, const string& name_filter)
{
    NodePtrs search_results;
    NodeKeywordMatcher matcher(name_filter.c_str()); 
    copy_if(source.begin(), source.end(), std::back_inserter(search_results), matcher);
    
    // TODO. support recursive search?
    return search_results;
}

size_t ContainerNode::nodePosition(NodePtr node)
{
    // check
    if (node == 0)
    {
        return INVALID_ORDER;
    }

    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    NodePtrs nodes  = children(ret, false, statusFilter());
    if (ret != RETRIEVE_FAILED)
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
    NodePtrs all = children(ret, false, statusFilter());
    if (ret == RETRIEVE_FAILED)
    {
        return NodePtr();
    }

    for(NodePtrs::iterator it = all.begin(); it != all.end(); ++it)
    {
        if ((*it)->name() == name)
        {
            return *it;
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

NodePtr ContainerNode::getNodeById(const string& id, bool recursive)
{
    NodePtrs all = children_;
    for(NodePtrs::iterator it = all.begin(); it != all.end(); ++it)
    {
        if ((*it)->id() == id)
        {
            return *it;
        }
    }

    if (recursive)
    {
        for(NodePtrs::iterator it = all.begin(); it != all.end(); ++it)
        {
            ContainerNode* child_container = dynamic_cast<ContainerNode*>((*it).get());
            if (child_container != 0)
            {
                NodePtr result = child_container->getNodeById(id, recursive);
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
    NodePtrs all = children(ret, false, statusFilter());
    if (ret != RETRIEVE_FAILED)
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
    NodePtrs all = children(ret, false, statusFilter());
    if (ret != RETRIEVE_FAILED)
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
    if (by_field_ != by || sort_order_ != order)
    {
        filtered_children_dirty_ = true;
    }
    by_field_ = by;
    sort_order_ = order;
}

void ContainerNode::changeStatusFilter(int status_filter)
{
    if (statusFilter() != status_filter)
    {
        mutableStatusFilter() = status_filter;
        filtered_children_dirty_ = true;
    }
}

void ContainerNode::changeNameFilter(const string& keyword)
{
    if (nameFilter() != keyword)
    {
        name_filter_ = keyword;
        filtered_children_dirty_ = true;
    }
}

/// Change sort criteria and sort. Only sort the filtered children
bool ContainerNode::sort(Field by, SortOrder order)
{
    if (!sort(filtered_children_, by, order))
    {
        return false;
    }

    changeSortCriteria(by, order);
    return true;
}

void ContainerNode::clearNameFilter()
{
    name_filter_.clear();
}

/// Filter all posterities to support expand mode. NOTE: this function does not work on asynchronous mode
int ContainerNode::filterPosterity(NodePtrs& nodes,
                                   bool rescan,
                                   int status_filter,
                                   bool recursive,
                                   const string& keyword)
{
    NodePtrs result;
    changeStatusFilter(status_filter);
    changeNameFilter(keyword);

    // Search from current directory.
    if (!recursive)
    {
        if (rescan || dirty_)
        {
            scan(absolutePath(), result);
            children_ = result;
        }
        result = ContainerNode::filterChildren(children_);
        nodes.insert(nodes.end(), result.begin(), result.end());
        setDirty(false);
        return result.size();
    }

    int posterity_num = 0;
    if (rescan || dirty_)
    {
        scan(absolutePath(), result);
        children_ = result;
    }
    result = ContainerNode::filterChildren(children_, statusFilter());
    setDirty(false);
    posterity_num = result.size(); // record number of children

    for (size_t i = 0; i < result.size(); i++)
    {
        NodePtr child = result[i];
        nodes.push_back(child);
        ContainerNode* container = dynamic_cast<ContainerNode*>(child.get());
        if (container != 0)
        {
            int sub_posterity_num = container->filterPosterity(nodes, rescan, status_filter, recursive);
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

bool ContainerNode::arePosteritiesBusy()
{
    NodePtrs& all = children_; //mutableChildren(ret, false, statusFilter());
    for (NodePtrs::const_iterator it = all.begin(); it != all.end(); ++it)
    {
        ContainerNode* container = dynamic_cast<ContainerNode*>((*it).get());
        if (container != 0)
        {
            if (!container->arePosteritiesBusy())
            {
                return false;
            }
        }
        else
        {
            int node_status = (*it)->status();
            if (!(node_status & (NODE_IS_UPLOADING | NODE_IS_DOWNLOADING)))
            {
                // if node is neither uploading or downloading, it's not busy
                return false;
            }
        }
    }
    return true;
}

NodePtrs ContainerNode::selectedLeaves(bool recursive)
{
    NodePtrs results;
    NodePtrs& all = children_; //mutableChildren(ret, false, statusFilter());
    NodePtrs::iterator itr = all.begin();
    while (itr != all.end())
    {
        NodePtr child = *itr;
        ContainerNode* container = dynamic_cast<ContainerNode*>(child.get());
        if (container != 0)
        {
            if (recursive)
            {
                NodePtrs sub_results = container->selectedLeaves(recursive);
                if (!sub_results.empty())
                {
                    results.insert(results.end(), sub_results.begin(), sub_results.end());
                }
            }
        }
        else
        {
            if (child->selected())
            {
                results.push_back(child);
            }
        }
        itr++;
    }
    return results;
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
