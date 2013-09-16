#ifndef DUOKAN_MODEL_TREE_H_
#define DUOKAN_MODEL_TREE_H_

#include "Model/node.h"
#include "Model/container_node.h"

#include "GUI/EventListener.h"
#include "GUI/EventSet.h"
#include "GUI/EventArgs.h"

using namespace std;

namespace dk
{

namespace document_model
{

// Events
class FileSystemChangedArgs: public EventArgs
{
public:
    FileSystemChangedArgs() {}
    virtual ~FileSystemChangedArgs() {}
    virtual EventArgs* Clone() const {
        return new FileSystemChangedArgs(*this);
    }
};

class ModelTreeCurrentNodeChangedArgs: public EventArgs
{
public:
    ModelTreeCurrentNodeChangedArgs() : current_node(0) {}
    virtual ~ModelTreeCurrentNodeChangedArgs() {}
    virtual EventArgs* Clone() const {
        return new ModelTreeCurrentNodeChangedArgs(*this);
    }

public:
    ContainerNode* current_node;
};

class CloudQuotaRetrieved : public EventArgs
{
public:
    CloudQuotaRetrieved() : total_size(-1), available(-1), ns_used(-1) {}
    virtual ~CloudQuotaRetrieved() {}
    virtual EventArgs* Clone() const {
        return new CloudQuotaRetrieved(*this);
    }

public:
    int64_t total_size;
    int64_t available;
    int64_t ns_used;
};

class ModelTree : public EventSet
                , public EventListener
{
public:
    static const char* EventFileSystemChanged;
    static const char* EventCoverLoaded;
    static const char* EventCurrentNodeChanged;
    static const char* EventQuotaRetrieved;

public:
    ModelTree();
    ~ModelTree();

    const ModelPath & currentPath() { return current_path_; }
public:
    virtual ContainerNode * root() = 0;
    virtual ContainerNode * currentNode() = 0;
    virtual NodeType currentType() = 0;

    virtual ContainerNode * cdRootNode() = 0;
    virtual ContainerNode * cdContainerNodeWithinTopNode(const string& name) = 0;
    virtual ContainerNode * cd(NodePtr p) = 0;
    virtual ContainerNode * cdUp() = 0;
    virtual bool cdPath(const ModelPath& path) = 0;
    virtual bool cdPath(const string& path) = 0;
    virtual bool canGoUp() = 0;

    virtual Field sortField() const = 0;
    virtual SortOrder sortOrder() const = 0;
    virtual void setSortField(Field by) = 0;
    virtual void setSortCriteria(Field by, SortOrder order) = 0;
    virtual void sort() = 0;

    virtual DKDisplayMode displayMode() = 0;
    virtual void setDisplayMode(DKDisplayMode display_mode) = 0;

    // Leave this function for compatibility of local model tree
    // Prefer to use children(name_filter)
    virtual void search(const string& keyword) = 0;
    virtual NodePtr getNodeById(const string& id);

    virtual NodePtrs getSelectedNodesInfo(int64_t& total_size, int& number, bool& exceed) = 0;

    static ModelTree* getModelTree(ModelType tree_type);

public:
    // Fire Events
    inline void fireFileSystemChangedEvent();
    inline void fireCoverLoadedEvent(const string& node_path, const string& cover_path);

protected:
    virtual void initialize() = 0;
    virtual ContainerNode* setCurrentNode(ContainerNode *node) = 0;

protected:
    ContainerNode *current_node_;
    ModelPath current_path_;
    Field by_;
    SortOrder order_;
};

inline void ModelTree::fireFileSystemChangedEvent()
{
    FileSystemChangedArgs args;
    FireEvent(EventFileSystemChanged, args);
}

inline void ModelTree::fireCoverLoadedEvent(const string& node_path, const string& cover_path)
{
    BookCoverLoadedArgs args;
    args.bookPath = node_path;
    args.coverPath = cover_path;
    FireEvent(EventCoverLoaded, args);
}

};  // namespace document_model

};  // namespace dk

#endif
