#ifndef DUOKAN_LOCAL_BOOKSTORE_NODE_
#define DUOKAN_LOCAL_BOOKSTORE_NODE_

#include "Model/container_node.h"
#include "Model/local_doc_node.h"

using namespace std;

namespace dk {

namespace document_model {

/// Folder node represents a virtual container contains nodes.
/// It could be a directory or a category in cms library.
/// The branch node implements a directory based container.
class LocalBookStoreNode :  public ContainerNode
{
public:
    LocalBookStoreNode(Node * parent, const string& category_path);
    virtual ~LocalBookStoreNode();

public:
    virtual SPtr<ITpImage> getInitialImage();

    virtual NodePtrs& updateChildrenInfo();
    virtual size_t nodePosition(NodePtr node);
    virtual size_t nodePosition(const string &name);
    virtual bool search(const StringList& filters, bool recursive, bool & stop);

    static bool testStatus(const string& path, int status_filter);

public:
    string currentPath();
    bool isVirtualFolder() { return virtual_folder_; }

protected:
    virtual bool updateChildren(int status_filter);

protected:
    void scan(const string& dir, NodePtrs &result, int status_filter = NODE_NONE, bool sort_list = true);
    void collectDirectories(StringList & result);

protected:
    bool virtual_folder_;
};

// Report current path.
inline string LocalBookStoreNode::currentPath()
{
    // dir_.absolutePath(); // TODO.
    return absolutePath();
}

};  // namespace document_model_

};  // namespce dk

#endif  // DUOKAN_LOCAL_BOOKSTORE_NODE_
