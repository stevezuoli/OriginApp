#ifndef DUOKAN_LOCAL_FILESYSTEM_TREE_H_
#define DUOKAN_LOCAL_FILESYSTEM_TREE_H_

#include "Model/model_tree.h"
#include "Model/local_desktop_node.h"

using namespace std;

namespace dk
{

namespace document_model
{

class LocalFileSystemTree : public ModelTree
{
public:
    LocalFileSystemTree();
    ~LocalFileSystemTree();

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

protected:
    virtual void initialize();
    virtual ContainerNode* setCurrentNode(ContainerNode *node);

private:
    bool mapFilePathToModelPath(const string & path, ModelPath & model_path);
    ContainerNode * containerNodeWithinTopNode(NodeType type);

    // Events Slots
    bool onFileListChanged(const EventArgs& args);
    bool onCoverLoaded(const EventArgs& args);

    // Cloud Events Slots
    bool onDirectoryCreated(const EventArgs& args);
    bool onGetInfoReturned(const EventArgs& args);

    // downloading events
    void onFileCommitted(const EventArgs& args);
    bool onCreateFileFinished(const EventArgs& args);
    bool onUploadingProgress(const EventArgs& args);

private:
    DesktopNode root_node_;
};


};  // namespace document_model

};  // namespace dk

#endif
