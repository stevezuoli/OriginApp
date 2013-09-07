#ifndef DUOKAN_BOOKSTORE_FILESYSTEM_TREE_H_
#define DUOKAN_BOOKSTORE_FILESYSTEM_TREE_H_

#include "Model/model_tree.h"
#include "Model/dkstore_desktop_node.h"

using namespace std;

namespace dk
{

namespace document_model
{

class BookStoreFileSystemTree : public ModelTree
{
public:
    BookStoreFileSystemTree();
    ~BookStoreFileSystemTree();

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

    virtual void search(const string& keyword, int start, int num);

protected:
    virtual void initialize();
    virtual ContainerNode* setCurrentNode(ContainerNode *node);

private:
    bool mapFilePathToModelPath(NodePtr node, ModelPath & model_path);
    ContainerNode * containerNodeWithinTopNode(NodeType type);

private:
    BookStoreDesktopNode root_node_;
};


};  // namespace document_model

};  // namespace dk

#endif
