#ifndef DUOKAN_BOOKSTORE_DESKTOP_NODE_H_
#define DUOKAN_BOOKSTORE_DESKTOP_NODE_H_

#include "Model/container_node.h"
#include "BookStore/CategoryInfo.h"

using namespace dk::bookstore::model;

namespace dk {

namespace document_model {

/// Desktop node represents a virtual container contains all nodes.
/// It could be a directory or a category in cms library.
/// The branch node implements a directory based container.
class BookStoreDesktopNode :  public ContainerNode
{
public:
    BookStoreDesktopNode(Node * parent = 0);
    virtual ~BookStoreDesktopNode();

public:
    virtual NodePtrs updateChildrenInfo();

    void search(const string& keyword, int status_filter, int start, int num);
    DKDisplayMode currentDisplayMode() const { return current_display_mode_; }
    DKDisplayMode& mutableDisplayMode() { return current_display_mode_; }

protected:
    virtual bool updateChildren(int status_filter);

private:
    virtual void scan(const string& dir, NodePtrs &result, int status_filter = NODE_NONE, bool sort_list = true);

private:
    // Events Slots
    bool onCategoryTreeUpdated(const EventArgs& args);
    bool onSearchUpdated(const EventArgs& args);

public:
    static const char* DKSTORE_DESKTOP_PATH;

private:
    DKDisplayMode current_display_mode_;
};

};  // namespace document_model

};  // namespce dk

#endif  // DUOKAN_DESKTOP_NODE_H_
