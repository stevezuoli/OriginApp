#ifndef DUOKAN_CLOUD_CATEGORY_NODE_
#define DUOKAN_CLOUD_CATEGORY_NODE_

#include "Model/container_node.h"

#include "XiaoMi/MiCloudFile.h"

using namespace std;
using namespace xiaomi;

namespace dk {

namespace document_model {

/// Folder node represents a virtual container contains nodes.
/// It could be a directory or a category in cms library.
/// The branch node implements a directory based container.
class CloudCategoryNode :  public ContainerNode
{
public:
    CloudCategoryNode(Node * parent, MiCloudFileSPtr category_info);
    virtual ~CloudCategoryNode();

public:
    MiCloudFileSPtr directoryInfo() { return category_info_; }

    virtual SPtr<ITpImage> getInitialImage();
    virtual NodePtrs updateChildrenInfo();
    virtual string currentPath();

    // CRUD
    virtual bool remove(bool delete_local_files_if_possible, bool exec_now = true);
    virtual void download(bool exec_now = true);
    virtual void createFile(const string& file_path);
    virtual void createDirectory(const string& dir_name);
    virtual void deleteFile(const string& file_id);
    virtual void deleteDirectory(const string& dir_id);

    virtual bool supportedCommands(std::vector<int>& command_ids, std::vector<int>& str_ids);

    void onChildrenReturned(const EventArgs& args);
    void onFileCreated(const EventArgs& args);
    void onDirectoryCreated(const EventArgs& args);
    void onFileDeleted(const EventArgs& args);
    void onDirectoryDeleted(const EventArgs& args);

    static bool testStatus(MiCloudFileSPtr category, int status_filter);

protected:
    virtual bool updateChildren();

protected:
    virtual void scan(const string& dir, NodePtrs &result);
    void collectDirectories(const string &dir, StringList & result);

private:
    void updateChildren(const MiCloudFileSPtrList& cloud_file_list);

    // Event Handler

protected:
    MiCloudFileSPtr category_info_;
    time_t last_update_time_;
};

// Report current path.
inline string CloudCategoryNode::currentPath()
{
    // dir_.absolutePath(); // TODO.
    return absolutePath();
}

};  // namespace document_model_

};  // namespce dk

#endif  // DUOKAN_BOOKSTORE_CATEGORY_NODE_
