#ifndef DUOKAN_CLOUD_FILESYSTEM_TREE_H_
#define DUOKAN_CLOUD_FILESYSTEM_TREE_H_

#include <map>
#include "Model/model_tree.h"
#include "Model/cloud_desktop_node.h"

#include "Common/FileManager_DK.h"

using namespace std;

namespace dk
{

namespace document_model
{

typedef std::map<string, Node*> CloudNodes;
typedef std::map<string, PCDKFile> CloudLocalMap;

class CloudFileSystemTree : public ModelTree
{
public:
    CloudFileSystemTree();
    virtual ~CloudFileSystemTree();

public:
    virtual ContainerNode * root();
    virtual ContainerNode * currentNode();
    virtual NodeType currentType();

    virtual ContainerNode * cdRootNode();
    virtual ContainerNode * cdContainerNodeWithinTopNode(const string& name);
    virtual ContainerNode * cd(NodePtr p);
    virtual ContainerNode * cd(const string& name);
    virtual ContainerNode * cdUp();
    virtual bool cdPath(const ModelPath& path);
    virtual bool cdPath(const string& path);
    virtual bool canGoUp();

    virtual Field sortField() const;
    virtual SortOrder sortOrder() const;
    virtual void setSortCriteria(Field by, SortOrder order);
    virtual void setSortField(Field by);
    virtual void sort();

    virtual DKDisplayMode displayMode();
    virtual void setDisplayMode(DKDisplayMode display_mode);
    virtual void search(const string& keyword);

    virtual NodePtrs getSelectedNodesInfo(int64_t& total_size, int& number, bool& exceed);
    virtual NodePtr getNodeById(const string& id);

    void FetchQuota();

public:
    //static bool addNodeToGlobalMap(Node* node);
    //static bool removeNodeFromGlobalMap(const string& id);
    static bool removeNodeFromCloudLocalMap(const string& id);
    static Node* getNodeFromGlobalMap(const string& id);

    static bool isCloudFileInLocalStorage(const string& cloud_id,
                                          const string& file_name,
                                          const int64_t size,
                                          PCDKFile& local_file);
    static bool isLocalFileInCloud(const string& file_name,
                                   const int64_t size,
                                   MiCloudFileSPtr& cloud_file);
    static bool isLocalCacheInitialized() { return local_cache_inited_; }
    //static void clearCloudNodesCache() { all_nodes_.clear(); }
    static void clearCloudLocalMap() { cloud_local_map_.clear(); }
    static void setCacheInitialized(bool initialized) { local_cache_inited_ = initialized; }
    static bool cacheInitialized() { return local_cache_inited_; }

protected:
    virtual void initialize();
    virtual ContainerNode* setCurrentNode(ContainerNode *node);

private:
    bool mapFilePathToModelPath(Node* node, ModelPath & model_path);
    ContainerNode * containerNodeWithinTopNode(NodeType type);

private:
    bool onFileCreated(const EventArgs& args);
    bool onQuotaRetrieved(const EventArgs& args);
    bool onChildrenRetrieved(const EventArgs& args);
    bool onDirectoryCreated(const EventArgs& args);
    bool onFileDeleted(const EventArgs& args);
    bool onDirectoryDeleted(const EventArgs& args);

    // downloading events
    bool onRequestDownloadFinished(const EventArgs& args);
    bool onDownloadProgress(const EventArgs& args);

    // local file system
    bool onLocalFileSystemChanged(const EventArgs& args);

    // account event
    bool onAccountEvent(const EventArgs& args);

public:
    static int64_t total_size_;
    static int64_t available_;
    static int64_t ns_used_;

private:
    CloudDesktopNode root_node_;
    static bool local_cache_inited_;
    //static CloudNodes all_nodes_;
    static CloudLocalMap cloud_local_map_;
};


};  // namespace document_model

};  // namespace dk

#endif
