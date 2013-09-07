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
    virtual bool remove(bool delete_local_files_if_possible);
    virtual void upload();
    virtual void fetchInfo();
    virtual bool loadingInfo(int& progress, int& state);

    // event handler
    void onInfoReturned(const EventArgs& args);
    void onCreateFileFinished(const EventArgs& args);
    void onUploadingProgress(int progress, int state);
    void onFileCommitted(const EventArgs& args);

    static bool testStatus(const string& path, int status_filter);
    static bool isOnCloud(const string& name, int64_t size);

private:
    enum MetaDataState
    {
        MD_INVALID = -1,
        MD_TOSCAN,              ///< Need to scan.
        MD_SCANNED              ///< Alaredy scanned.
    };

protected:
    MetaDataState data_state_;
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
