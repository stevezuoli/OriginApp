#ifndef DUOKAN_DOC_NODE_H_
#define DUOKAN_DOC_NODE_H_

#include "node.h"

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

    void update(); // call PCDKFile->SyncFile();
    PCDKFile metadata(bool force_update = false);
    bool remove(string& error);

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
    size_t size_;
};

}  // namespace document_model

}  // namespce dk

#endif
