#ifndef DUOKAN_BOOKSTORE_CATEGORY_NODE_
#define DUOKAN_BOOKSTORE_CATEGORY_NODE_

#include "Model/container_node.h"
#include "BookStore/CategoryInfo.h"

using namespace std;
using namespace dk::bookstore::model;

namespace dk {

namespace document_model {

/// Folder node represents a virtual container contains nodes.
/// It could be a directory or a category in cms library.
/// The branch node implements a directory based container.
class BookStoreCategoryNode :  public ContainerNode
{
public:
    BookStoreCategoryNode(Node * parent, CategoryInfo* category);
    virtual ~BookStoreCategoryNode();

public:
    virtual NodePtrs updateChildrenInfo();

    virtual size_t nodePosition(NodePtr node);
    virtual size_t nodePosition(const string &name);

    virtual bool sort(Field by, SortOrder order = ASCENDING);
    virtual bool search(const StringList& filters, bool recursive, bool & stop);

    virtual string currentPath();
    virtual int childrenTotalSize();

    CategoryInfoSPtr categoryInfo() { return category_info_; }

    static bool testStatus(CategoryInfo* category, int status_filter);

protected:
    virtual bool updateChildren(int status_filter);

protected:
    virtual void scan(const string& dir, NodePtrs &result, int status_filter = NODE_NONE, bool sort_list = true);
    void collectDirectories(const string &dir, StringList & result);

    bool hasChildrenCategories();

private:
    // Event Handler
    bool onChildrenBooksUpdated(const EventArgs& args); // only for sorted mode

protected:
    CategoryInfoSPtr category_info_;
    int children_total_size_;
    int children_size_in_current_window_;
    int children_start_index_;
};

// Report current path.
inline string BookStoreCategoryNode::currentPath()
{
    // dir_.absolutePath(); // TODO.
    return absolutePath();
}

};  // namespace document_model_

};  // namespce dk

#endif  // DUOKAN_BOOKSTORE_CATEGORY_NODE_
