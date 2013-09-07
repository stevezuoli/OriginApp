#ifndef DUOKAN_NODE_H_
#define DUOKAN_NODE_H_

#include <string>
#include <vector>
#include <map>
#include <deque>
#include <tr1/memory>

#include "DkSPtr.h"
#include "GUI/ITpImage.h"
#include "Common/FileManager_DK.h"
#include "Model/node_types.h"
#include "Common/BookCoverLoader.h"

using namespace std;
using namespace std::tr1;

namespace dk
{

namespace document_model
{

class Node;
typedef shared_ptr<Node>   NodePtr;
typedef vector<NodePtr>    NodePtrs;
typedef NodePtrs::iterator NodePtrsIter;

typedef vector<string> StringList;
typedef StringList ModelPath;

// Events
class NodeChildenReadyArgs: public EventArgs
{
public:
    NodeChildenReadyArgs() : succeeded(false) {}
    virtual EventArgs* Clone() const {
        return new NodeChildenReadyArgs(*this);
    }
    bool succeeded;
    string reason;
    string current_node_path;
    NodePtrs children;
};

class NodeLoadingFinishedArgs: public EventArgs
{
public:
    enum LoadingType
    {
        NODE_LOADING_TYPE_UNKNOWN = 0,
        NODE_LOADING_TYPE_UPLOAD,
        NODE_LOADING_TYPE_DOWNLOAD,
        NODE_LOADING_TYPE_OPENING_BOOK,
    };

public:
    NodeLoadingFinishedArgs() : succeeded(false), type(NODE_LOADING_TYPE_UNKNOWN) {}
    virtual EventArgs* Clone() const {
        return new NodeLoadingFinishedArgs(*this);
    }
    bool succeeded;
    LoadingType type;  
    string reason;
    string current_node_path;
};

class NodeStatusUpdated: public EventArgs
{
public:
    NodeStatusUpdated() {}
    virtual EventArgs* Clone() const {
        return new NodeStatusUpdated(*this);
    }
    string current_node_path;
    int new_status;
};

class NodeUploadContext
{
public:
    NodeUploadContext(){}
    ~NodeUploadContext(){}

    inline void reset(const NodePtrs& waiting_list);
    inline NodePtr popFront();
    inline bool empty();
    inline void clear();
    inline NodePtr currentNode() { return current_node_; }
    inline bool isActive() { return (current_node_ != 0 || !waiting_queue_.empty()); }
private:
    NodePtr current_node_;
    std::deque<NodePtr> waiting_queue_;
};

/// Node base class. The abstract node can represent
/// - File system, such as file and directory including removable storage.
/// - Nodes collection and so on.
class Node : public EventSet
           , public EventListener
{
public:
    static const char* EventChildrenIsReady;
    static const char* EventNodeStatusUpdated;
    static const char* EventNodeLoadingFinished;

public:
    Node(Node* parent);
    virtual ~Node(void);

    /// Virtual functions
    virtual SPtr<ITpImage> getInitialImage();

    /// Retrieve parent node.  Every node has a parent except the root
    /// for which parent_ == NULL.
    const Node* parent() const { return parent_; }
    Node* mutableParent() { return parent_; }

    /// Retrieve the root node.
    Node* mutableRoot();

    /// The name of the node. It can be served as the
    /// unique id of the node if possible.
    /// For file or directory, the name can be the same
    /// as the file name or directory name.
    const string & name() const { return name_; }
    string & mutableName() { return name_; }

    /// Metadata or title for display.
    const string & displayName() const { return display_name_; }
    string & mutableDisplayName() { return display_name_; }

    const string & description() const { return description_; }
    string & mutableDescription() { return description_; }

    const long lastRead() const { return last_read_; }
    long & mutableLastRead() { return last_read_; }

    /// Absolute path in file system. It can also be url if necessary.
    /// When it's a path, it does not contain file:/// prefix.
    /// For url, it's caller's responsibility to interpret the url.
    const string & absolutePath() const { return absolute_path_; }
    string& mutableAbsolutePath() { return absolute_path_; }

    /// ID of the file or directory. Empty for local self book
    const string & id() const { return id_; }
    string& mutableId() { return id_; }

    /// Absolute path from root node
    string absolutePathFromAncestor();
    string relativePathFromRoot();
    virtual string cloudFilePath();

    /// Cover path of the node. It is the absolute path of cover image in local file system.
    const string & coverPath() const { return cover_path_; }
    string& mutableCoverPath() { return cover_path_; }

    /// Retrieve node type. It's used to identify node type in runtime.
    /// Note: different node type can share the same implementation.
    /// For example, USB and SD node can use the DirNode but return
    /// NODE_TYPE_USB and NODE_TYPE_SD.
    NodeType type() const { return type_; }
    NodeType & mutableType() { return type_; }

    bool selected() const { return selected_; }
    //bool& mutableSelected() { return selected_; }
    virtual void setSelected(bool selected);

    /// status of this node, for filtering
    int status() const { return status_; }
    virtual void setStatus(int status, bool mandatory = false);

    /// cloud operations
    virtual bool rename(const string& new_name, string& error_msg);
    virtual bool remove(bool delete_local_files_if_possible);
    virtual void upload();
    virtual void download();
    virtual bool loadingInfo(int& progress, int& state);

public:
    static const size_t INVALID_ORDER;
    static const string DATE_FORMAT;

protected:
    Node*  parent_;
    string name_;
    string display_name_;
    string absolute_path_;
    string description_;
    string cover_path_;
    string id_;
    long last_read_;
    NodeType type_;
    int status_; // for filter
    bool selected_;
};

string nodeDisplayName(const std::string& path);
string nodeName(const std::string& path);

inline void NodeUploadContext::reset(const NodePtrs& waiting_list)
{
    clear();
    for (size_t i = 0; i < waiting_list.size(); i++)
    {
        waiting_queue_.push_back(waiting_list[i]);
    }
}

inline NodePtr NodeUploadContext::popFront()
{
    if (empty())
    {
        return NodePtr();
    }
    current_node_ = waiting_queue_.front();
    waiting_queue_.pop_front();
    return current_node_;
}

inline bool NodeUploadContext::empty()
{
    return waiting_queue_.empty();
}

inline void NodeUploadContext::clear()
{
    current_node_ = NodePtr();
    waiting_queue_.clear();
}

};  // namespace document_model

};  // namespace dk

#endif  // DUOKAN_NODE_H_
