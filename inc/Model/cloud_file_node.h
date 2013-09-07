#ifndef DUOKAN_CLOUD_FILE_NODE_H_
#define DUOKAN_CLOUD_FILE_NODE_H_

#include "Model/node.h"
#include "XiaoMi/MiCloudFile.h"

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
    virtual bool remove(bool delete_local_files_if_possible);
    virtual void download();
    virtual void fetchInfo();
    virtual bool loadingInfo(int& progress, int& state);

    /// retrieve basic info of this file
    virtual SPtr<ITpImage> getInitialImage();
    virtual DkFileFormat fileFormat();
    virtual const string artist() const;

    // event handler
    void onInfoReturned(const EventArgs& args);
    void onRequestDownloadFinished(const EventArgs& args);
    void onDownloadingProgress(int progress, int state);

    static bool testStatus(MiCloudFileSPtr book_info, int status_filter);
    
private:
    bool createLocalFileIfExist();

protected:
    enum MetaDataState
    {
        MD_INVALID = -1,
        MD_TOSCAN,              ///< Need to scan.
        MD_SCANNED              ///< Alaredy scanned.
    };

protected:
    MetaDataState data_state_;
    MiCloudFileSPtr cloud_file_info_;
    PCDKFile local_file_info_; // if the cloud file is also on local, it is not null
    int64_t size_;

    // downloading info
    int downloading_progress_;
    int downloading_state_;
};

}  // namespace document_model

}  // namespce dk

#endif
