#ifndef CONTAINER_NODE_H_
#define CONTAINER_NODE_H_

#include "node.h"

using namespace std;

namespace dk {

namespace document_model {

/// Container node represents a virtual container contains nodes.
/// It could be a directory or a category.
/// The Container node implements a directory based container.
class ContainerNode :  public Node
{
public:
    ContainerNode(Node * parent);
    virtual ~ContainerNode();

public:
    virtual const NodePtrs& children(bool rescan = false);
    virtual NodePtrs& mutableChildren(bool rescan = false);
    virtual NodePtrs& updateChildrenInfo();

    NodePtr node(const string &name);
    NodePtr node(NodeType type);

    size_t nodePosition(NodePtr node);
    size_t nodePosition(const string &name);

    Field sortField() const { return by_field_; }
    SortOrder sortOrder() const { return sort_order_; }

    void changeSortCriteria(Field by, SortOrder order);
    virtual bool sort(Field by, SortOrder order = ASCENDING);

    virtual bool search(const StringList& filters, bool recursive, bool & stop);
    void clearNameFilters();
    const StringList & name_filters() { return name_filters_; }

protected:
    virtual void updateChildren();
    virtual bool sort(NodePtrs &result, Field by, SortOrder order = ASCENDING);
    void clearChildren();

protected:
    Field by_field_;
    SortOrder sort_order_;
    StringList name_filters_;
    NodePtrs children_;
};

/// Define simple sort type for all nodes.
struct LessByName
{
    bool operator()( const Node * a, const Node *b ) const
    {
        return (a->name().compare(b->name()) < 0);
    }
};

struct GreaterByName
{
    bool operator()( const Node * a, const Node *b ) const
    {
        return (a->name().compare(b->name()) > 0);
    }
};

struct LessByLastRead
{
    bool operator()( Node * a,  Node *b ) const
    {
        return (a->lastRead() < b->lastRead());
    }
};

struct GreaterByLastRead
{
    bool operator()( Node * a, Node *b ) const
    {
        return (a->lastRead() > b->lastRead());
    }
};

struct LessByNodetype
{
    bool operator()( Node * a,  Node *b ) const
    {
        return (a->type() < b->type());
    }
};

struct GreaterByNodeType
{
    bool operator()( Node * a, Node *b ) const
    {
        return (a->type() > b->type());
    }
};

};  // namespace document_model

};  // namespce dk

#endif  // CONTAINER_NODE_H_
