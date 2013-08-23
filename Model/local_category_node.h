#ifndef DUOKAN_LOCAL_CATEGORY_NODE_
#define DUOKAN_LOCAL_CATEGORY_NODE_

#include "container_node.h"
#include "doc_node.h"

using namespace std;

namespace dk {

namespace document_model {

/// Folder node represents a virtual container contains nodes.
/// It could be a directory or a category in cms library.
/// The branch node implements a directory based container.
class LocalCategoryNode :  public ContainerNode
{
public:
    LocalCategoryNode(Node * parent, const string& root);
    virtual ~LocalCategoryNode();

public:
    const NodePtrs& children(bool rescan = false);
    NodePtrs& mutableChildren(bool rescan = false);
    NodePtrs& updateChildrenInfo();

    size_t nodePosition(Node * node);
    size_t nodePosition(const string &name);

    string leafNodeName();

    bool sort(Field by, SortOrder order = ASCENDING);

    bool setRoot(const string &root_dir);
    const string & root() const { return root_dir_; }

    bool search(const StringList& filters, bool recursive, bool & stop);
    void clearNameFilters();
    const StringList & name_filters() { return name_filters_; }

    bool cdRoot();
    bool cd(const string & dir);
    bool canGoUp();
    bool cdUp();

    bool fileSystemChanged();
    string currentPath();

    bool isVirtualFolder() { return virtual_folder_; }

protected:
    virtual void updateChildren();
    virtual bool sort(NodePtrs &result, Field by, SortOrder order = ASCENDING);

    void scan(const string& dir, const StringList &filters, NodePtrs &result, bool sort = true);
    void collectDirectories(const string &dir, StringList & result);

protected:
    bool   dirty_;
    string root_dir_;
    string dir_;
    bool   virtual_folder_;
};

// Report current path.
inline string LocalCategoryNode::currentPath()
{
    // dir_.absolutePath(); // TODO.
    return dir_;
}

inline string LocalCategoryNode::leafNodeName()
{
    // return dir_.dirName(); // TODO.
    return dir_;
}

/// Define simple sort type for file nodes. Use down_cast here.
/// All nodes are in the vector. Need to check the type
/// as caller may want to compare DirNode with FileNode.
struct LessBySize
{
    bool operator()( const Node * a, const Node *b ) const
    {
        if (a->type() == NODE_TYPE_FILE &&
            b->type() == NODE_TYPE_FILE)
        {
            return down_cast<const FileNode *>(a)->fileSize() <
                   down_cast<const FileNode *>(b)->fileSize();
        }
        else if (a->type() == NODE_TYPE_FILE &&
                 b->type() == NODE_TYPE_DIRECTORY)
        {
            return false;
        }
        else if (a->type() == NODE_TYPE_DIRECTORY &&
                 b->type() == NODE_TYPE_FILE)
        {
            return true;
        }
        return (a->display_name().compare(b->display_name(), Qt::CaseInsensitive)) < 0;
    }
};

struct GreaterBySize
{
    bool operator()( const Node * a, const Node *b ) const
    {
        if (a->type() == NODE_TYPE_FILE &&
            b->type() == NODE_TYPE_FILE)
        {
            return down_cast<const FileNode *>(a)->fileSize() >
                   down_cast<const FileNode *>(b)->fileSize();
        }
        else if (a->type() == NODE_TYPE_FILE &&
                 b->type() == NODE_TYPE_DIRECTORY)
        {
            return true;
        }
        else if (a->type() == NODE_TYPE_DIRECTORY &&
                 b->type() == NODE_TYPE_FILE)
        {
            return false;
        }
        return (a->display_name().compare(b->display_name(), Qt::CaseInsensitive) > 0);
    }
};

struct LessByRating
{
    bool operator()( Node * a,  Node *b ) const
    {
        if (a->type() == NODE_TYPE_FILE &&
            b->type() == NODE_TYPE_FILE)
        {
            return down_cast< FileNode *>(a)->metadata().rating() <
                   down_cast< FileNode *>(b)->metadata().rating();
        }
        if (a->type() == NODE_TYPE_FILE &&
            b->type() == NODE_TYPE_DIRECTORY)
        {
            return true;
        }
        else if (a->type() == NODE_TYPE_DIRECTORY &&
                 b->type() == NODE_TYPE_FILE)
        {
            return false;
        }
        return false;
    }
};

struct GreaterByRating
{
    bool operator()( Node * a, Node *b ) const
    {
        if (a->type() == NODE_TYPE_FILE &&
            b->type() == NODE_TYPE_FILE)
        {
            return down_cast< FileNode *>(a)->metadata().rating() >
                   down_cast< FileNode *>(b)->metadata().rating();
        }
        if (a->type() == NODE_TYPE_FILE &&
            b->type() == NODE_TYPE_DIRECTORY)
        {
            return true;
        }
        else if (a->type() == NODE_TYPE_DIRECTORY &&
                 b->type() == NODE_TYPE_FILE)
        {
            return false;
        }
        return (a->display_name().compare(b->display_name(), Qt::CaseInsensitive) > 0);
    }
};

}  // namespace document_model_

}  // namespce dk

#endif  // DUOKAN_LOCAL_CATEGORY_NODE_
