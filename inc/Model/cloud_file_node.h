#ifndef DUOKAN_CLOUD_FILE_NODE_H_
#define DUOKAN_CLOUD_FILE_NODE_H_

#include "Model/node.h"
#include "XiaoMi/MiCloudFile.h"

#include "Common/File_DK.h"

using namespace xiaomi;

namespace dk {

namespace document_model {

/// File Node implement. The file node can contain
/// another virtual file system.
class CloudFileNode : public Node
{
public:
    CloudFileNode(Node * parent, MiCloudFileSPtr file_info);
    ~CloudFileNode();

public:
    // size of the cloud file
    size_t fileSize() const { return size_; }

    // cloud file info
    MiCloudFileSPtr fileInfo() { return cloud_file_info_; }
    PCDKFile localFileInfo() { return local_file_info_; }

    // operations to this file
    void update();

    // cloud operations
    virtual bool remove(bool delete_local_files_if_possible, bool exec_now = true);
    virtual void download(bool exec_now = true);
    virtual void fetchInfo();
    virtual bool loadingInfo(int& progress, int& state);

    /// retrieve basic info of this file
    virtual SPtr<ITpImage> getInitialImage();
    virtual DkFileFormat fileFormat();
    virtual const string artist() const;

    virtual bool satisfy(int status_filter);
    virtual bool supportedCommands(std::vector<int>& command_ids, std::vector<int>& str_ids);

    // event handler
    void onInfoReturned(const EventArgs& args);
    void onRequestDownloadFinished(const EventArgs& args);
    void onDownloadingProgress(const std::string& task_id, int progress, int state);

    static bool testStatus(MiCloudFileSPtr book_info, int status_filter);
    
private:
    bool createLocalFileIfExist();
    int updateStatusByDownloadTask(int status);

protected:
    MiCloudFileSPtr cloud_file_info_;
    PCDKFile local_file_info_; // if the cloud file is also on local, it is not null
    int64_t size_;

    // downloading info
    std::string downloading_task_id_;
    int downloading_progress_;
    int downloading_state_;
};

}  // namespace document_model

}  // namespce dk

#endif
