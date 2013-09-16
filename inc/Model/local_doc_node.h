#ifndef DUOKAN_LOCAL_DOC_NODE_H_
#define DUOKAN_LOCAL_DOC_NODE_H_

#include "Model/node.h"
#include "XiaoMi/XiaoMiServiceFactory.h"
#include "XiaoMi/MiCloudService.h"
#include "XiaoMi/MiCloudManager.h"

using namespace xiaomi;

namespace dk {

namespace document_model {

/// File Node implement. The file node can contain
/// another virtual file system.
class FileNode : public Node
{
public:
    FileNode(Node * parent, PCDKFile file_info);
    ~FileNode();

public:
    size_t fileSize() const { return size_; }
    void update();
    void sync();
    PCDKFile fileInfo() { return file_info_; }

    virtual SPtr<ITpImage> getInitialImage();
    virtual const string artist() const;
    virtual bool isTrialBook() const;
    virtual DkFileFormat fileFormat();
    virtual bool isDuokanBook() const;

    // cloud operations
    virtual bool rename(const string& new_name, string& error_msg);
    virtual bool remove(bool delete_local_files_if_possible, bool exec_now = true);
    virtual bool removeFromCategory();
    virtual void upload(bool exec_now = true);
    virtual void fetchInfo();
    virtual void stopLoading();
    virtual bool loadingInfo(int& progress, int& state);

    virtual bool satisfy(int status_filter);
    virtual bool supportedCommands(std::vector<int>& command_ids, std::vector<int>& str_ids);

    // event handler
    void onInfoReturned(const EventArgs& args);
    void onCreateFileFinished(const EventArgs& args);
    void onUploadingProgress(int progress, int state);
    void onFileCommitted(const EventArgs& args);

    static bool isOnCloud(const string& name, int64_t size);

private:
    void updateByUploadingTask();
    void updateDisplayName();

protected:
    PCDKFile file_info_;
    MiCloudFileSPtr cloud_file_info_;
    size_t size_;

    // uploading info
    int uploading_progress_;
    int uploading_state_;
};

}  // namespace document_model

}  // namespce dk

#endif
