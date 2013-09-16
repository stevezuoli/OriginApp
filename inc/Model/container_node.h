#ifndef DUOKAN_CONTAINER_NODE_H_
#define DUOKAN_CONTAINER_NODE_H_

#include "Model/node.h"
#include "Model/local_doc_node.h"
#include "Utility/CharUtil.h"

using namespace dk::utility;
using namespace std;

namespace dk {

namespace document_model {

enum RetrieveChildrenResult
{
    RETRIEVE_FAILED = 0,
    RETRIEVE_PENDING,
    RETRIEVE_DONE
};

class NodeKeywordMatcher
{
public:
    NodeKeywordMatcher(const char* keyword);
    virtual ~NodeKeywordMatcher(){};
    virtual bool operator()(const NodePtr node) const;
private:
    const char* keyword_;
    bool match_by_first_letters_;
    std::wstring  wide_keyword_;
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
    virtual NodePtr getNodeById(const string& id, bool recursive = false);

    virtual NodePtrs children(RetrieveChildrenResult& result,
                              bool rescan = false,
                              int status_filter = NODE_NONE,
                              const string& keyword = string());

    //virtual NodePtrs& mutableChildren(RetrieveChildrenResult& result, bool rescan = false, int status_filter = NODE_NONE);
    virtual NodePtrs updateChildrenInfo();

    virtual int filterPosterity(NodePtrs& nodes,
                                bool rescan = false,
                                int status_filter = NODE_NONE,
                                bool recursive = true,
                                const string& keyword = string());

    virtual size_t nodePosition(NodePtr node);
    virtual size_t nodePosition(const string &name);

    virtual void changeSortCriteria(Field by, SortOrder order);
    virtual void changeStatusFilter(int status_filter);
    virtual void changeNameFilter(const string& keyword);
    virtual bool sort(Field by, SortOrder order = ASCENDING);
    virtual void clearNameFilter();

    // CRUD
    virtual void createFile(const string& file_path);
    virtual void createDirectory(const string& dir_name);
    virtual void deleteFile(const string& file_id);
    virtual void deleteDirectory(const string& dir_id);

    // Get all selected nodes
    virtual NodePtrs selectedLeaves(bool recursive = true);

    // Select
    virtual void setSelected(bool selected);

    // Status Filter
    virtual NodePtrs filterChildren(NodePtrs& source, bool sort_results = true);
    int statusFilter() const { return status_filter_; }
    int& mutableStatusFilter() { return status_filter_; }

    // dirty
    void setDirty(bool dirty);
    bool isDirty() const { return dirty_; }

    // all of the posterities are busy
    bool arePosteritiesBusy();

    // clear all children
    void clearChildren();

public:
    const string & nameFilter() { return name_filter_; }
    Field sortField() const { return by_field_; }
    SortOrder sortOrder() const { return sort_order_; }

protected:
    virtual void scan(const string& dir, NodePtrs &result) = 0;
    virtual bool updateChildren();
    virtual bool sort(NodePtrs &result, Field by, SortOrder order = ASCENDING);
    virtual NodePtrs search(NodePtrs& source, const string& name_filter);

protected:
    Field by_field_;
    SortOrder sort_order_;
    NodePtrs children_;
    NodePtrs filtered_children_;

    string name_filter_;
    int status_filter_;
    bool dirty_;
    bool filtered_children_dirty_;
};

/// Define simple sort type for all nodes.
struct LessByName
{
    bool operator()( const NodePtr a, const NodePtr b ) const
    {
        //return (a->name().compare(b->name()) < 0);
        return CharUtil::StringCompareByDisplay(a->gbkName().c_str(), b->gbkName().c_str()) < 0;
    }
};

struct GreaterByName
{
    bool operator()( const NodePtr a, const NodePtr b ) const
    {
        //return (a->name().compare(b->name()) > 0);
        return CharUtil::StringCompareByDisplay(a->gbkName().c_str(), b->gbkName().c_str()) > 0;
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

struct LessByNodeType
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

struct GreaterByCreateTime
{
    bool operator()( NodePtr a, NodePtr b ) const
    {
        return (a->createTime() > b->createTime());
    }
};

struct LessByCreateTime
{
    bool operator()( NodePtr a, NodePtr b ) const
    {
        return (a->createTime() < b->createTime());
    }
};

struct GreaterByModifyTime
{
    bool operator()( NodePtr a, NodePtr b ) const
    {
        return (a->modifyTime() > b->modifyTime());
    }
};

struct LessByModifyTime
{
    bool operator()( NodePtr a, NodePtr b ) const
    {
        return (a->modifyTime() < b->modifyTime());
    }
};

struct GreaterByNodeStatus
{
    bool operator()( NodePtr a, NodePtr b ) const
    {
        return (a->status() > b->status());
    }
};

struct LessByNodeStatus
{
    bool operator()( NodePtr a, NodePtr b ) const
    {
        return (a->status() < b->status());
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
        else if ((a->type() == NODE_TYPE_FILE_LOCAL_DOCUMENT ||
                  a->type() == NODE_TYPE_MICLOUD_BOOK) &&
                 (b->type() == NODE_TYPE_CATEGORY_LOCAL_FOLDER ||
                  b->type() == NODE_TYPE_MICLOUD_CATEGORY))
        {
            return false;
        }
        else if ((a->type() == NODE_TYPE_CATEGORY_LOCAL_FOLDER ||
                  a->type() == NODE_TYPE_MICLOUD_CATEGORY) &&
                  (b->type() == NODE_TYPE_FILE_LOCAL_DOCUMENT ||
                   b->type() == NODE_TYPE_MICLOUD_BOOK))
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
