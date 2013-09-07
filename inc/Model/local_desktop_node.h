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
    virtual NodePtrs& updateChildrenInfo();

    // cloud operation
    virtual void upload();
    virtual void fetchInfo();

    const NodePtrs& filterPosterity(int status_filter, bool recursive = true);
    Node* getNodeByModelPath(const ModelPath &path);

    DKDisplayMode currentDisplayMode() const { return current_display_mode_; }
    DKDisplayMode& mutableDisplayMode() { return current_display_mode_; }

    // event handler
    void onInfoReturned(const EventArgs& args);
    void onCreateCloudDirectoryFinished(const EventArgs& args);

protected:
    virtual bool updateChildren(int status_filter);

private:
    virtual void scan(const string& dir, NodePtrs &result, int status_filter = NODE_NONE, bool sort_list = true);

private:
    void updateChildrenByFolder(int status_filter);
    void updateChildrenBySort(int status_filter);
    void updateChildrenByExpandingAll(int status_filter);

    NodePtr createBookStoreNode(const string& path, int status_filter);
    NodePtr createPushedMessageNode(const string& path, int status_filter);

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
