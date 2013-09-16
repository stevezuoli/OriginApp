#ifndef DUOKAN_DESKTOP_NODE_H_
#define DUOKAN_DESKTOP_NODE_H_

#include "Model/container_node.h"

namespace dk {

namespace document_model {

/// Desktop node represents a virtual container contains all nodes.
/// It could be a directory or a category in cms library.
/// The branch node implements a directory based container.
class DesktopNode :  public ContainerNode
{
public:
    DesktopNode(Node * parent = 0);
    virtual ~DesktopNode();

public:
    virtual NodePtrs updateChildrenInfo();

    virtual bool sort(Field by, SortOrder order = ASCENDING);

    // cloud operation
    virtual void upload(bool exec_now = false);
    virtual void fetchInfo();
    virtual NodePtrs filterChildren(NodePtrs& source, bool sort_results = true);
    virtual NodePtrs selectedLeaves(bool recursive = true);

    NodePtrs filterPosterity(bool recursive = true);
    Node* getNodeByModelPath(const ModelPath &path);

    DKDisplayMode currentDisplayMode() const { return current_display_mode_; }
    //DKDisplayMode& mutableDisplayMode() { return current_display_mode_; }
    void setDisplayMode(DKDisplayMode display_mode);

    // event handler
    void onInfoReturned(const EventArgs& args);
    void onCreateCloudDirectoryFinished(const EventArgs& args);

private:
    virtual bool updateChildren();
    virtual void scan(const string& dir, NodePtrs &result);

private:
    void updateChildrenByFolder();
    void updateChildrenBySort();
    void updateChildrenByExpandingAll();

    NodePtr createBookStoreNode(const string& path);
    NodePtr createPushedMessageNode(const string& path);

    bool checkCloudCacheOrUpdate();
    void uploadChildren();

    // Events Slots
    bool onCloudRootCreated(const EventArgs& args);
    bool onCloudRootScanned(const EventArgs& args);

private:
    DKDisplayMode current_display_mode_;
    bool          pending_scan_;
};

};  // namespace document_model

};  // namespce dk

#endif  // DUOKAN_DESKTOP_NODE_H_
