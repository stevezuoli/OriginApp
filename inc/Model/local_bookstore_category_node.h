#ifndef DUOKAN_LOCAL_BOOKSTORE_CATEGORY_NODE_
#define DUOKAN_LOCAL_BOOKSTORE_CATEGORY_NODE_

#include "Model/container_node.h"
#include "Model/local_doc_node.h"

using namespace std;

namespace dk {

namespace document_model {

/// Folder node represents a virtual container contains nodes.
/// It could be a directory or a category in cms library.
/// The branch node implements a directory based container.
class LocalBookStoreCategoryNode :  public ContainerNode
{
public:
    LocalBookStoreCategoryNode(Node * parent, const string& category_name);
    virtual ~LocalBookStoreCategoryNode();

public:
    virtual NodePtrs updateChildrenInfo();
    virtual string currentPath();

    bool isVirtualFolder() { return virtual_folder_; }

    virtual bool rename(const string& new_name, string& error_msg);
    virtual bool remove(bool delete_local_files_if_possible, bool exec_now = true);
    virtual bool supportedCommands(std::vector<int>& command_ids, std::vector<int>& str_ids);
    virtual bool satisfy(int status_filter);

    static bool testStatus(const string& path, int status_filter);

protected:
    virtual bool updateChildren();

protected:
    virtual void scan(const string& dir, NodePtrs &result);

protected:
    bool virtual_folder_;
};

// Report current path.
inline string LocalBookStoreCategoryNode::currentPath()
{
    // dir_.absolutePath(); // TODO.
    return absolutePath();
}

};  // namespace document_model_

};  // namespce dk

#endif  // DUOKAN_LOCAL_BOOKSTORE_CATEGORY_NODE_
