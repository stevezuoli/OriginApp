#ifndef DUOKAN_NODE_H_
#define DUOKAN_NODE_H_

#include <string>
#include <string>
#include <vector>
#include <tr1/memory>

#include "DkSPtr.h"
#include "GUI/ITpImage.h"
#include "Common/FileManager_DK.h"
#include "node_types.h"

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

/// Node base class. The abstract node can represent
/// - File system, such as file and directory including removable storage.
/// - Nodes collection and so on.
class Node
{
public:
    Node(Node* parent);
    virtual ~Node(void);

    /// Virtual functions
    virtual SPtr<ITpImage> getInitialImage();
    virtual const string artist() const;
    virtual bool isTrialBook() const;
    virtual DkFileFormat fileFormat();
    virtual bool isDuokanBook() const;
    virtual string coverUrl();

    /// Retrieve parent node.  Every node has a parent except the root
    /// for which parent_ == NULL.
    const Node* parent() const { return parent_; }
    Node* mutableParent() const { return parent_; }

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

    const string & lastRead() const { return last_read_; }
    string & mutableLastRead() { return last_read_; }

    /// Absolute path in file system. It can also be url if necessary.
    /// When it's a path, it does not contain file:/// prefix.
    /// For url, it's caller's responsibility to interpret the url.
    const string & absolutePath() const { return absolute_path_; }
    string& mutableAbsolutePath() { return absolute_path_; }

    /// Retrieve node type. It's used to identify node type in runtime.
    /// Note: different node type can share the same implementation.
    /// For example, USB and SD node can use the DirNode but return
    /// NODE_TYPE_USB and NODE_TYPE_SD.
    NodeType type() const { return type_; }
    NodeType & mutableType() { return type_; }

public:
    static const size_t INVALID_ORDER;
    static const string DATE_FORMAT;

protected:
    Node*  parent_;
    string name_;
    string display_name_;
    string absolute_path_;
    string description_;
    string last_read_;
    NodeType type_;
};

string nodeDisplayName(NodeType type);
string nodeName(NodeType type);


}  // namespace document_model

}  // namespace dk

#endif  // DUOKAN_NODE_H_
