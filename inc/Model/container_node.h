#ifndef DUOKAN_CONTAINER_NODE_H_
#define DUOKAN_CONTAINER_NODE_H_

#include "Model/node.h"
#include "Model/local_doc_node.h"

using namespace std;

namespace dk {

namespace document_model {

enum RetrieveChildrenResult
{
    RETRIEVE_FAILED = 0,
    RETRIEVE_PENDING,
    RETRIEVE_DONE
};

/// Container node represents a virtual container contains nodes.
/// It could be a directory or a category.
/// The Container node implements a directory based container.
class ContainerNode :  public Node
{
public:
    ContainerNode(Node * parent);
    virtual ~ContainerNode();

public:
    virtual NodePtr node(const string &name, bool recursive = false);
    virtual NodePtr node(NodeType type);

    virtual const NodePtrs& children(RetrieveChildrenResult& result, bool rescan = false, int status_filter = NODE_NONE);
    virtual NodePtrs& mutableChildren(RetrieveChildrenResult& result, bool rescan = false, int status_filter = NODE_NONE);
    virtual NodePtrs& updateChildrenInfo();

    virtual int filterPosterity(NodePtrs& nodes, int status_filter, bool recursive = true);

    virtual size_t nodePosition(NodePtr node);
    virtual size_t nodePosition(const string &name);

    virtual void changeSortCriteria(Field by, SortOrder order);
    virtual bool sort(Field by, SortOrder order = ASCENDING);
    virtual bool search(const StringList& filters, bool recursive, bool & stop);
    virtual void clearNameFilters();

    // CRUD
    virtual void createFile(const string& file_path);
    virtual void createDirectory(const string& dir_name);
    virtual void deleteFile(const string& file_id);
    virtual void deleteDirectory(const string& dir_id);

    // Select
    virtual void setSelected(bool selected);

    // Status Filter
    int statusFilter() const { return status_filter_; }
    int& mutableStatusFilter() { return status_filter_; }

    // dirty
    void setDirty(bool dirty) { dirty_ = dirty; }
    bool isDirty() const { return dirty_; }

public:
    const StringList & name_filters() { return name_filters_; }
    Field sortField() const { return by_field_; }
    SortOrder sortOrder() const { return sort_order_; }

protected:
    virtual void scan(const string& dir, NodePtrs &result, int status_filter = NODE_NONE, bool sort_list = true) = 0;
    virtual bool updateChildren(int status_filter);
    virtual bool sort(NodePtrs &result, Field by, SortOrder order = ASCENDING);

protected:
    void clearChildren();

protected:
    Field by_field_;
    SortOrder sort_order_;
    StringList name_filters_;
    NodePtrs children_;

    int status_filter_;
    bool dirty_;
};

/// Define simple sort type for all nodes.
struct LessByName
{
    bool operator()( const NodePtr a, const NodePtr b ) const
    {
        return (a->name().compare(b->name()) < 0);
    }
};

struct GreaterByName
{
    bool operator()( const NodePtr a, const NodePtr b ) const
    {
        return (a->name().compare(b->name()) > 0);
    }
};

struct LessByLastRead
{
    bool operator()( NodePtr a,  NodePtr b ) const
    {
        return (a->lastRead() < b->lastRead());
    }
};

struct GreaterByLastRead
{
    bool operator()( NodePtr a, NodePtr b ) const
    {
        return (a->lastRead() > b->lastRead());
    }
};

struct LessByNodetype
{
    bool operator()( NodePtr a, NodePtr b ) const
    {
        return (a->type() < b->type());
    }
};

struct GreaterByNodeType
{
    bool operator()( NodePtr a, NodePtr b ) const
    {
        return (a->type() > b->type());
    }
};

/// Define simple sort type for file nodes. Use dynamic_cast here.
/// All nodes are in the vector. Need to check the type
/// as caller may want to compare DirNode with FileNode.
struct LessBySize
{
    bool operator()( const NodePtr a, const NodePtr b ) const
    {
        if (a->type() == NODE_TYPE_FILE_LOCAL_DOCUMENT &&
            b->type() == NODE_TYPE_FILE_LOCAL_DOCUMENT)
        {
            return dynamic_cast<const FileNode *>(a.get())->fileSize() <
                   dynamic_cast<const FileNode *>(b.get())->fileSize();
        }
        else if (a->type() == NODE_TYPE_FILE_LOCAL_DOCUMENT &&
                 b->type() == NODE_TYPE_CATEGORY_LOCAL_FOLDER)
        {
            return false;
        }
        else if (a->type() == NODE_TYPE_CATEGORY_LOCAL_FOLDER &&
                 b->type() == NODE_TYPE_FILE_LOCAL_DOCUMENT)
        {
            return true;
        }
        return (a->displayName().compare(b->displayName())) < 0;
    }
};

struct GreaterBySize
{
    bool operator()( const NodePtr a, const NodePtr b ) const
    {
        if (a->type() == NODE_TYPE_FILE_LOCAL_DOCUMENT &&
            b->type() == NODE_TYPE_FILE_LOCAL_DOCUMENT)
        {
            return dynamic_cast<const FileNode *>(a.get())->fileSize() >
                   dynamic_cast<const FileNode *>(b.get())->fileSize();
        }
        else if (a->type() == NODE_TYPE_FILE_LOCAL_DOCUMENT &&
                 b->type() == NODE_TYPE_CATEGORY_LOCAL_FOLDER)
        {
            return true;
        }
        else if (a->type() == NODE_TYPE_CATEGORY_LOCAL_FOLDER &&
                 b->type() == NODE_TYPE_FILE_LOCAL_DOCUMENT)
        {
            return false;
        }
        return (a->displayName().compare(b->displayName())) > 0;
    }
};

};  // namespace document_model

};  // namespce dk

#endif  // CONTAINER_NODE_H_
