#ifndef DUOKAN_MODEL_TREE_H_
#define DUOKAN_MODEL_TREE_H_

#include "node.h"
#include "container_node.h"
#include "desktop_node.h"

using namespace std;

namespace dk
{

namespace document_model
{

class ModelTree
{
public:
    ModelTree();
    ~ModelTree();

public:
    ContainerNode * root();
    ContainerNode * currentNode();
    NodeType currentType();
    ContainerNode * containerNode(NodeType type);
    ContainerNode * cdDesktop();
    ContainerNode * cdBranch(NodeType type);
    ContainerNode * cdBranch(const string &name);
    ContainerNode * cd(NodePtr p);
    ContainerNode * cdUp();

    const ModelPath & current_path() { return current_path_; }

    Field sort_field() const;
    SortOrder sort_order() const;

    bool modelPath(const string & path, ModelPath & model);
    bool cdPath(const ModelPath &path);
    ContainerNode * cd(const string &name);
    bool canGoUp();

    void updateDisplayNames();
    void changeSortCriteria(Field by, SortOrder order);
    void saveSettings();

    FolderNode *folderNode(Node *node);

private:
    void initialize();

    ContainerNode* changeCurrentNode(ContainerNode *node);
    void updatePath();
    bool folderPath(NodeType type, ModelPath & path);

private:
    DesktopNode root_node_;
    ContainerNode *current_node_;
    ModelPath current_path_;

    Field by_;
    SortOrder order_;
};


};  // namespace document_model

};  // namespace dk

#endif
