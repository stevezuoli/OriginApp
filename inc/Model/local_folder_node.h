#ifndef DUOKAN_LOCAL_CATEGORY_NODE_
#define DUOKAN_LOCAL_CATEGORY_NODE_

#include "Model/container_node.h"
#include "Model/local_doc_node.h"

#include "XiaoMi/MiCloudFile.h"

using namespace std;
using namespace xiaomi;

namespace dk {

namespace document_model {

/// Folder node represents a virtual container contains nodes.
/// It could be a directory or a category in cms library.
/// The branch node implements a directory based container.
class LocalFolderNode :  public ContainerNode
{
public:
    LocalFolderNode(Node * parent, const string& category_path);
    virtual ~LocalFolderNode();

public:
    virtual SPtr<ITpImage> getInitialImage();

    virtual NodePtrs& updateChildrenInfo();

    virtual size_t nodePosition(NodePtr node);
    virtual size_t nodePosition(const string &name);

    virtual bool search(const StringList& filters, bool recursive, bool & stop);
    virtual string currentPath();

    bool isVirtualFolder() { return virtual_folder_; }
    static bool testStatus(const string& path, int status_filter);

    // cloud operation
    virtual bool rename(const string& new_name, string& error_msg);
    virtual bool remove(bool delete_local_files_if_possible);
    virtual void upload();
    virtual void createCloudDirectory();
    virtual void fetchInfo();
    
    // event handler
    void onInfoReturned(const EventArgs& args);
    void onCreateCloudDirectoryFinished(const EventArgs& args);

protected:
    virtual bool updateChildren(int status_filter);
    virtual void scan(const string& dir, NodePtrs &result, int status_filter = NODE_NONE, bool sort_list = true);

protected:
    void collectDirectories(const string &dir, StringList & result);

private:
    bool isOnCloud();
    void uploadChildren();

protected:
    bool virtual_folder_;

    // cloud info for uploading
    MiCloudFileSPtr cloud_file_info_;
    NodeUploadContext upload_ctx_;
};

// Report current path.
inline string LocalFolderNode::currentPath()
{
    // dir_.absolutePath(); // TODO.
    return absolutePath();
}

};  // namespace document_model_

};  // namespce dk

#endif  // DUOKAN_LOCAL_CATEGORY_NODE_
