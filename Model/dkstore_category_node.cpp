#include "Model/dkstore_category_node.h"

using namespace dk::bookstore;

namespace dk {

namespace document_model {

BookStoreCategoryNode::BookStoreCategoryNode(Node * parent, CategoryInfo* category)
    : ContainerNode(parent)
    , category_info_(model::CategoryInfoSPtr(dynamic_cast<model::CategoryInfo*>(category->Clone())))
{
    mutableType() = NODE_TYPE_BOOKSTORE_CATEGORY;
    mutableAbsolutePath() = category_info_->GetCategoryId();
    mutableName() = category_info_->GetCategoryId();
    mutableDisplayName() = category_info_->GetLabel();
    mutableDescription() = category_info_->GetDescription();
    if (hasChildrenCategories())
    {
        children_total_size_ = category_info_->mutableChildrenNodeList().size();
    }
    else
    {
        children_total_size_ = category_info_->GetBookCount();
    }
}

BookStoreCategoryNode::~BookStoreCategoryNode()
{
    DeletePtrContainer(&children_);
}

int BookStoreCategoryNode::childrenTotalSize()
{
    return children_total_size_;
}

bool BookStoreCategoryNode::hasChildrenCategories()
{
    return !category_info_->mutableChildrenNodeList().empty();
}

NodePtrs& BookStoreCategoryNode::updateChildrenInfo()
{
    for(NodePtrsIter iter = children_.begin(); iter != children_.end(); ++iter)
    {
        // Update information if it's file node.
        if ((*iter)->type() == NODE_TYPE_BOOKSTORE_BOOK)
        {
            dynamic_cast<FileNode *>((*iter).get())->update();
        }
    }
    sort(children_, by_field_, sort_order_);
    return children_;
}

size_t BookStoreCategoryNode::nodePosition(NodePtr node)
{
    // check
    if (node == 0)
    {
        return INVALID_ORDER;
    }

    const NodePtrs& nodes  = children();
    NodePtrs::const_iterator it = find(nodes.begin(), nodes.end(), node);
    if (it == nodes.end())
    {
        return INVALID_ORDER;
    }
    return it - nodes.begin();
}

size_t BookStoreCategoryNode::nodePosition(const string &name)
{
    const NodePtrs& all = children();
    for(NodePtrs::const_iterator it = all.begin(); it != all.end(); ++it)
    {
        if ((*it)->name() == name)
        {
            return it - all.begin();
        }
    }
    return INVALID_ORDER;
}

bool BookStoreCategoryNode::sort(Field by, SortOrder order)
{
    if (!sort(children_, by, order))
    {
        return false;
    }

    by_field_ = by;
    sort_order_ = order;
    return true;
}

bool BookStoreCategoryNode::search(const StringList& filters, bool recursive, bool & stop)
{
    // Not support now
    return false;
}

string BookStoreCategoryNode::currentPath()
{
    return absolutePath();
}

bool BookStoreCategoryNode::testStatus(CategoryInfo* category, int status_filter)
{
    // TODO. add filter for bookstore category
    return true;
}

void BookStoreCategoryNode::updateChildren(int status_filter)
{
    DeletePtrContainer(&children_);
    scan(absolutePath(), children_, status_filter, true);
}

void BookStoreCategoryNode::scan(const string& dir, NodePtrs &result, int status_filter, bool sort_list)
{
    // if it has children categories, just initialize children by children
    if (hasChildrenCategories())
    {
        BasicObjectList& cat_list = category_info_->mutableChildrenNodeList();
        for (size_t i = 0; i < cat_list.size(); ++i)
        {
            model::CategoryInfo* child = dynamic_cast<model::CategoryInfo*>(cat_list[i].get());
            if (child != 0 &&
                BookStoreCategoryNode::testStatus(child, status_filter))
            {
                // create a category node
                NodePtr cat_node = new BookStoreCategoryNode(this, child);
                children_.push_back(cat_node);
            }
        }
        fireChildrenReadyEvent(absolutePath(), children_);
    }
    else
    {
        // get books
    }
}

void BookStoreCategoryNode::collectDirectories(const string &dir, StringList & result)
{
}

bool BookStoreCategoryNode::onChildrenBooksUpdated(const EventArgs& args)
{
}

}

}