//===========================================================================
// Summary:
//	    XMLDomNode.h
// Usage:
//      封装DkTiXmlNode, 遍历节点树，或删除，或添加节点
//  		...
// Remarks:
//	    Null
// Date:
//	    2011-09-15
// Author:
//	    Zhang Jiafang(zhangjf@duokan.com)
//===========================================================================
#ifndef __KERNEL_COMMONLIBS_XMLBASE_XMLDOMNODE_H__
#define __KERNEL_COMMONLIBS_XMLBASE_XMLDOMNODE_H__

#include "XMLBase.h"

enum DK_XML_NODE_TYPE
{
    DK_XML_NODE_UNKNOWN,
    DK_XML_NODE_DOCUMENT,
    DK_XML_NODE_DECLARATION,
    DK_XML_NODE_ELEMENT,
    DK_XML_NODE_TEXT,
    DK_XML_NODE_COMMENT,
    DK_XML_NODE_TYPECOUNT
};

class XMLDomNode
{
    friend class XMLDomDocument;

public:
    XMLDomNode();
    virtual ~XMLDomNode();

public:
    //-------------------------------------------
    //	Summary:
    //	    get the node type
    //	Parameters:
    //	    [out]type       - the type of the node
    //	Remarks:
    //	    null
    //	Return Value:
    //	    the node type
    //-------------------------------------------
    DK_XML_NODE_TYPE GetNodeType() const;

    //-------------------------------------------
    //	Summary:
    //	    get the node value:
    //      1)DK_XML_NODE_UNKNOWN: the tag content
    //      2)DK_XML_NODE_DOCUMENT: the filename of the xml document
    //      3)DK_XML_NODE_DECLARATION: none
    //      4)DK_XML_NODE_ELEMENT: the element name
    //      5)DK_XML_NODE__TEXT: the text string
    //      7)DK_XML_NODE_COMMENT: the comment text string
    //	Parameters:
    //	    null
    //	Remarks:
    //	    null
    //	Return Value:
    //	    the node value in utf8
    //-------------------------------------------
    const DK_CHAR* GetNodeValue() const;

    //-------------------------------------------
    //	Summary:
    //	    get the node prefix
    //	Parameters:
    //	    null
    //	Remarks:
    //	    only the element node have prefix
    //	Return Value:
    //	    the node prefix in utf8 
    //-------------------------------------------
    const DK_CHAR* GetNodePrefix() const;
    
public:
    //-------------------------------------------
    //	Summary:
    //	    get the parent of the node
    //	Parameters:
    //	    [out]pDocument       - the parent node, 
    //                             need to be alloced outside
    //	Remarks:
    //	    null
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    DK_ReturnCode GetParent(XMLDomNode* pParent) const;

    //-------------------------------------------
    //	Summary:
    //	    get the direct first child of the node or 
    //      whose name is pNodeName, overloaded
    //	Parameters:
    //      [in]pNodeName            - the name of the node
    //	    [out]pFirstChild       - the first child node, 
    //                               need to be alloced outside
    //	Remarks:
    //	    If the current node is xml element, the namespace prefix will not be used
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    DK_ReturnCode GetFirstChild(XMLDomNode* pFirstChild) const;
    DK_ReturnCode GetFirstChild(const DK_CHAR* pNodeName, XMLDomNode* pFirstChild) const;
    DK_ReturnCode GetFirstChild(const DK_WCHAR* pNodeName, XMLDomNode* pFirstChild) const;

    //-------------------------------------------
    //	Summary:
    //	    get the direct first child of the node or 
    //      whose name is pNodeName, overloaded
    //	Parameters:
    //      [in]pNodeName         - the name of the node
    //	    [out]pLastChild       - the last child node, 
    //                               need to be alloced outside
    //	Remarks:
    //	    If the current node is xml element, the namespace prefix will not be used
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    DK_ReturnCode GetLastChild(XMLDomNode* pLastChild) const;
    DK_ReturnCode GetLastChild(const DK_CHAR* pNodeName, XMLDomNode* pLastChild) const;
    DK_ReturnCode GetLastChild(const DK_WCHAR* pNodeName, XMLDomNode* pLastChild) const;
    
    //-------------------------------------------
    //	Summary:
    //	    get the direct first element child of the node or 
    //      whose name is strName, overloaded
    //	Parameters:
    //      [in]pNodeName                 - the name of the node
    //	    [out]pFirstElementChild       - the first element child node, 
    //                                      need to be alloced outside
    //	Remarks:
    //	    not using the namespace prefix, 
    //      so when finding the first child element with the name, then return 
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    DK_ReturnCode GetFirstElementChild(XMLDomNode* pFirstElementChild) const;
    DK_ReturnCode GetFirstElementChild(const DK_CHAR* pNodeName, XMLDomNode* pFirstElementChild) const;
    DK_ReturnCode GetFirstElementChild(const DK_WCHAR* pNodeName, XMLDomNode* pFirstElementChild) const;

    //-------------------------------------------
    //	Summary:
    //	    get the direct first element child of the node or 
    //      whose name is pNodeName with the pNodePrefix, overloaded
    //	Parameters:
    //      [in]pNodeName                     - the name of the node
    //      [in]pNodePrefix                   - the namespace prefix
    //	    [out]pFirstElementChild           - the first element child node, 
    //                                          need to be alloced outside
    //	Remarks:
    //	    using the namespace prefix, 
    //      so when finding the first element with the name and the prefix, then return 
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    DK_ReturnCode GetFirstElementChild(const DK_CHAR* pNodeName, const DK_CHAR* pNodePrefix, XMLDomNode* pFirstElementChild) const;
    DK_ReturnCode GetFirstElementChild(const DK_WCHAR* pNodeName, const DK_WCHAR* pNodePrefix, XMLDomNode* pFirstElementChild) const;

    //-------------------------------------------
    //	Summary:
    //	    get the direct previous sibling of the node or 
    //      whose name is pNodeName, overloaded
    //	Parameters:
    //      [in]pNodeName                 - the name of the node
    //	    [out]pPrevSibling             - the previous sibling node, 
    //                                      need to be alloced outside
    //	Remarks:
    //	    If the current node is xml element, the namespace prefix will not be used
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    DK_ReturnCode GetPreviousSibling(XMLDomNode* pPrevSibling) const;
    DK_ReturnCode GetPreviousSibling(const DK_CHAR* pNodeName, XMLDomNode* pPrevSibling) const;
    DK_ReturnCode GetPreviousSibling(const DK_WCHAR* pNodeName, XMLDomNode* pPrevSibling) const;

    //-------------------------------------------
    //	Summary:
    //	    get the direct next sibling of the node or 
    //      whose name is pNodeName, overloaded
    //	Parameters:
    //      [in]pNodeName                 - the name of the node
    //	    [out]pPrevSibling             - the next sibling node, 
    //                                      need to be alloced outside
    //	Remarks:
    //	    If the current node is xml element, the namespace prefix will not be used
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    DK_ReturnCode GetNextSibling(XMLDomNode* pNextSibling) const;
    DK_ReturnCode GetNextSibling(const DK_CHAR* pNodeName, XMLDomNode* pNextSibling) const;
    DK_ReturnCode GetNextSibling(const DK_WCHAR* pNodeName, XMLDomNode* pNextSibling) const;

    //-------------------------------------------
    //	Summary:
    //	    get the next sibling element of the node or 
    //      whose name is strName, overloaded
    //	Parameters:
    //      [in]strName                     - the name of the node
    //	    [out]pNextSiblingElement        - the next sibling element node, 
    //                                        need to be alloced outside
    //	Remarks:
    //	    not using the namespace prefix, 
    //      so when finding the next first sibling element with the name, then return 
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    DK_ReturnCode GetNextSiblingElement(XMLDomNode* pNextSiblingElement) const;
    DK_ReturnCode GetNextSiblingElement(const DK_CHAR* pNodeName, XMLDomNode* pNextSiblingElement) const;
    DK_ReturnCode GetNextSiblingElement(const DK_WCHAR* pNodeName, XMLDomNode* pNextSiblingElement) const;

    //-------------------------------------------
    //	Summary:
    //	    get the next sibling element of the node  
    //      whose name is pNodeName with the pNodePrefix, overloaded
    //	Parameters:
    //      [in]pNodeName                       - the name of the node
    //      [in]pNodePrefix                     - the namespace prefix
    //	    [out]pNextSiblingElement            - the next sibling element node, 
    //                                            need to be alloced outside
    //	Remarks:
    //	    using the namespace prefix, 
    //      so when finding the next first sibling element with the name and the prefix, then return 
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    DK_ReturnCode GetNextSiblingElement(const DK_CHAR* pNodeName, const DK_CHAR* pNodePrefix, XMLDomNode* pNextSiblingElement) const;
    DK_ReturnCode GetNextSiblingElement(const DK_WCHAR* pNodeName, const DK_WCHAR* pNodePrefix, XMLDomNode* pNextSiblingElement) const;
    

    //-------------------------------------------
    //	Summary:
    //	    get the child whose index is the param index in the direct children
    //	Parameters:
    //      [in]index                   - the name of the node
    //	    [out]pPrevSibling             - the next sibling node, 
    //                                      need to be alloced outside
    //	Remarks:
    //	    null
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    DK_ReturnCode GetChildByIndex(DK_INT index, XMLDomNode* pChild) const;

    //-------------------------------------------
    //	Summary:
    //	    get the children count
    //	Parameters:
    //	    [out]count             - the children count 
    //	Remarks:
    //	    null
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    DK_ReturnCode GetChildrenCount(DK_INT* pCount) const;

    //-------------------------------------------
    //	Summary:
    //	    Overloaded, Get the attribute value by the attribute name
    //	Parameters:
    //      [in]pAttrName            - the attribute name
    //      [in]pAttrPrefix          - the attribute prefix
    //	Remarks:
    //	    only element nodes have attribute.
    //      using the attribute's namespace information when pAttrPrefix is not null.
    //      so when finding the first attribute with the name and the prefix, then return.  
    //	Return Value:
    //	    the attribute value DK_CHAR* or DK_INT or DK_DOUBLE
    //-------------------------------------------
    const DK_CHAR* GetAttribute(const DK_CHAR* pAttrName, const DK_CHAR* pAttrPrefix = DK_NULL) const;
    DK_ReturnCode GetAttribute(const DK_CHAR* pAttrName, DK_INT* pInt) const;
    DK_ReturnCode GetAttribute(const DK_CHAR* pAttrName, DK_DOUBLE* pDouble) const;
    const DK_CHAR* GetAttribute(const DK_WCHAR* pAttrName, const DK_WCHAR* pAttrPrefix = DK_NULL) const;
    DK_ReturnCode GetAttribute(const DK_WCHAR* pAttrName, DK_INT* pInt) const;
    DK_ReturnCode GetAttribute(const DK_WCHAR* pAttrName, DK_DOUBLE* pDouble) const;

    //-------------------------------------------
    //	Summary:
    //	    Overloaded, Get the attribute value by the attribute index
    //	Parameters:
    //      [in]index               - the attribute index
    //	Remarks:
    //	    only element nodes have attribute.
    //      not using the attribute's namespace information  
    //	Return Value:
    //	    the attribute value DK_CHAR*
    //-------------------------------------------
    const DK_CHAR* GetAttribute(DK_INT index) const;

    //-------------------------------------------
    //	Summary:
    //	    get the count of the attributes
    //	Parameters:
    //	    [out]count          - the attribute count
    //	Remarks:
    //	    only element nodes have attribute
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    DK_ReturnCode GetAttributeCount(DK_INT* pCount) const;

    //-------------------------------------------
    //	Summary:
    //	    get the namespace uri with the name
    //	Parameters:
    //      [in]pNSName         - the namespace name
    //	Remarks:
    //	    only element nodes have namespaces.
    //      if pNSName is null, the default namespace uri will be returned.
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    const DK_CHAR* GetNamespace(const DK_CHAR* pNSName) const;

    //-------------------------------------------
    //	Summary:
    //	    get the default namespace uri
    //	Parameters:
    //      null
    //	Remarks:
    //	    only element nodes have the default namespaces.
    //      the default namespace have no name.
    //	Return Value:
    //	    DK_ReturnCode 
    //-------------------------------------------
    const DK_CHAR* GetDefaultNamespace() const;
    
public:
    //-------------------------------------------
    //	Summary:
    //	    check if the node has children
    //	Parameters:
    //      null
    //	Remarks:
    //	    null
    //	Return Value:
    //	    DK_TRUE has children or not
    //-------------------------------------------
    DK_BOOL HasChildren() const;
    
    //-------------------------------------------
    //	Summary:
    //	    check if the node has element children
    //	Parameters:
    //      null
    //	Remarks:
    //	    null
    //	Return Value:
    //	    DK_TRUE has element children or not
    //-------------------------------------------
    DK_BOOL HasElementChildren() const;
    
    //-------------------------------------------
    //	Summary:
    //	    check if the node has attribute
    //	Parameters:
    //      null
    //	Remarks:
    //	    only element has attributes
    //	Return Value:
    //	    DK_TRUE the element has attribute or not
    //-------------------------------------------
    DK_BOOL HasAttribute() const;

    //-------------------------------------------
    //	Summary:
    //	    check if the node has namespace
    //	Parameters:
    //      null
    //	Remarks:
    //	    only element has namespaces
    //	Return Value:
    //	    DK_TRUE the element has namespace or not
    //-------------------------------------------
    DK_BOOL HasNamespace() const;

    //-------------------------------------------
    //	Summary:
    //	    check if the node is good
    //	Parameters:
    //      null
    //	Remarks:
    //	    only element has attributes
    //	Return Value:
    //	    DK_TRUE m_pDkTiXmlNode != NULL;
    //      or not
    //-------------------------------------------
    DK_BOOL IsGoodNode() const;

public:
    //-------------------------------------------
    //	Summary:
    //	    Add a declaration node
    //	Parameters:
    //      [in]pVersion         - the xml version
    //      [in]pEncoding        - the xml encoding(only support utf-8)
    //      [in]pStandalone      - whether the xml is standalone, default empty
    //	Remarks:
    //	    only the document node can call this function,
    //      and if the document has any element node, do not call this function
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode AddDeclaration(const DK_CHAR* pVersion, const DK_CHAR* pEncoding, const DK_CHAR* pStandalone = DK_NULL);
    DK_ReturnCode AddDeclaration(const DK_WCHAR* pVersion, const DK_WCHAR* pEncoding, const DK_WCHAR* pStandalone = DK_NULL);
    
    
    //-------------------------------------------
    //	Summary:
    //	    Add a comment node to the end child of current node
    //	Parameters:
    //      [in]pComment         - the comment content
    //	Remarks:
    //	    only the document node and the element node can
    //      add a comment node
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode AddComment(const DK_CHAR* pComment);
    DK_ReturnCode AddComment(const DK_WCHAR* pComment);

    //-------------------------------------------
    //	Summary:
    //	    Add an the root element to the document node
    //	Parameters:
    //      [in]pElementName         - the element name
    //      [in]pPrefix              - the element prefix
    //	Remarks:
    //	    only the document node can call this function to add a root element node,
    //      and the document must not contain any element node before
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode AddRootElement(const DK_CHAR* pElementName, const DK_CHAR* pPrefix = DK_NULL);
    DK_ReturnCode AddRootElement(const DK_WCHAR* pElementName, const DK_WCHAR* pPrefix = DK_NULL);
    
    //-------------------------------------------
    //	Summary:
    //	    Add an element node to the end child of current node
    //	Parameters:
    //      [in]pElementName         - the element name
    //      [in]pPrefix              - the element prefix 
    //	Remarks:
    //	    only the element node can call this function to add a child element node
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode AddElement(const DK_CHAR* pElementName, const DK_CHAR* pPrefix = DK_NULL);
    DK_ReturnCode AddElement(const DK_WCHAR* pElementName, const DK_WCHAR* pPrefix = DK_NULL);
    
    //-------------------------------------------
    //	Summary:
    //	    Add an attribute to an element node. If the attribute exists,
    //      the value will be replaced by the new value.
    //	Parameters:
    //      [in]pAttrName            - the attribute name
    //      [in]pAttrValue           - the attribute value
    //      [in]pPrefix              - the attribute prefix
    //	Remarks:
    //	    only the element node can call this function to 
    //      add an attribute to the element node
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode AddAttribute(const DK_CHAR* pAttrName, const DK_CHAR* pAttrValue, const DK_CHAR* pPrefix = DK_NULL);
    DK_ReturnCode AddAttribute(const DK_CHAR* pAttrName, DK_INT attrValue, const DK_CHAR* pPrefix = DK_NULL);
    DK_ReturnCode AddAttribute(const DK_CHAR* pAttrName, DK_DOUBLE attrValue, const DK_CHAR* pPrefix = DK_NULL);
    DK_ReturnCode AddAttribute(const DK_WCHAR* pAttrName, const DK_WCHAR* pAttrValue, const DK_WCHAR* pPrefix = DK_NULL);
    DK_ReturnCode AddAttribute(const DK_WCHAR* pAttrName, DK_INT attrValue, const DK_WCHAR* pPrefix = DK_NULL);
    DK_ReturnCode AddAttribute(const DK_WCHAR* pAttrName, DK_DOUBLE attrValue, const DK_WCHAR* pPrefix = DK_NULL);

    //-------------------------------------------
    //	Summary:
    //	    Add a namespace to an element node. If the namespace exists,
    //      the value will be replaced by the new uri.
    //	Parameters:
    //      [in]pNsName              - the namespace name
    //      [in]pNsUri               - the namespace value
    //	Remarks:
    //	    only the element node can call this function to 
    //      add a namespace to the element node
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode AddNamespace(const DK_CHAR* pNsName, const DK_CHAR* pNsUri);
    DK_ReturnCode AddNamespace(const DK_WCHAR* pNsName, const DK_WCHAR* pNsUri);

    //-------------------------------------------
    //	Summary:
    //	    Add the default namespace to an element node. If the default namespace exists,
    //      the value will be replaced by the new uri.
    //	Parameters:
    //      [in]pNsUri               - the namespace value
    //	Remarks:
    //	    only the element node can call this function to 
    //      add a default namespace to the element node
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode AddDefaultNamespace(const DK_CHAR* pNsUri);
    DK_ReturnCode AddDefaultNamespace(const DK_WCHAR* pNsUri);

    //-------------------------------------------
    //	Summary:
    //	    Add a text node to an element node
    //	Parameters:
    //      [in]pText               - the text string
    //      [in]isCDATA             - whether the text is CDATA section
    //	Remarks:
    //	    only the element node can call this function to 
    //      add an text node
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode AddText(const DK_CHAR* pText, DK_BOOL isCDATA = DK_FALSE);
    DK_ReturnCode AddText(const DK_WCHAR* pText, DK_BOOL isCDATA = DK_FALSE);

public:
    //-------------------------------------------
    //	Summary:
    //	    Insert an element node before a child node of current node
    //	Parameters:
    //      [in]node                        - the child node inserting before
    //      [in]pElementName                - the element node name
    //      [out]pInsertedNode              - the inserted node
    //	Remarks:
    //	    only the element node can call this function to insert an element node
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode InsertElementBefore(XMLDomNode& node, const DK_CHAR* pElementName, XMLDomNode* pInsertedNode);
    DK_ReturnCode InsertElementBefore(XMLDomNode& node, const DK_WCHAR* pElementName, XMLDomNode* pInsertedNode);
    
    //-------------------------------------------
    //	Summary:
    //	    Insert an element node before a child node of current node
    //	Parameters:
    //      [in]node                        - the child node inserting before
    //      [in]pElementName                - the element node name
    //      [in]pPrefix                     - the element namespace prefix
    //      [out]pInsertedNode              - the inserted node
    //	Remarks:
    //	    only the element node can call this function to insert an element node
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode InsertElementBefore(XMLDomNode& node, const DK_CHAR* pElementName, const DK_CHAR* pPrefix, XMLDomNode* pInsertedNode);
    DK_ReturnCode InsertElementBefore(XMLDomNode& node, const DK_WCHAR* pElementName, const DK_WCHAR* pPrefix, XMLDomNode* pInsertedNode);   
    
    //-------------------------------------------
    //	Summary:
    //	    Insert an element node after a child node of current node
    //	Parameters:
    //      [in]node                        - the child node inserting before
    //      [in]pElementName                - the element node name
    //      [out]pInsertedNode              - the inserted node
    //	Remarks:
    //	    only the element node can call this function to insert an element node
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode InsertElementAfter(XMLDomNode& node, const DK_CHAR* pElementName, XMLDomNode* pInsertedNode);
    DK_ReturnCode InsertElementAfter(XMLDomNode& node, const DK_WCHAR* pElementName, XMLDomNode* pInsertedNode);    
    
    //-------------------------------------------
    //	Summary:
    //	    Insert an element node after a child node of current node
    //	Parameters:
    //      [in]node                        - the child node inserting before
    //      [in]pElementName                - the element node name
    //      [in]pPrefix                     - the element namespace prefix
    //      [out]pInsertedNode              - the inserted node
    //	Remarks:
    //	    only the element node can call this function to insert an element node
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode InsertElementAfter(XMLDomNode& node, const DK_CHAR* pElementName, const DK_CHAR* pPrefix, XMLDomNode* pInsertedNode);
    DK_ReturnCode InsertElementAfter(XMLDomNode& node, const DK_WCHAR* pElementName, const DK_WCHAR* pPrefix, XMLDomNode* pInsertedNode);
    
    //-------------------------------------------
    //	Summary:
    //	    Insert a text node before a child node of current node
    //	Parameters:
    //      [in]node                        - the child node inserting before
    //      [in]pText                       - the text
    //      [out]isCDATA                    - is in CDATA block
    //	Remarks:
    //	    only the element node can call this function to insert a text node
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode InsertTextBefore(XMLDomNode& node, const DK_CHAR* pText, DK_BOOL isCDATA = DK_FALSE);
    DK_ReturnCode InsertTextBefore(XMLDomNode& node, const DK_WCHAR* pText, DK_BOOL isCDATA = DK_FALSE);
    
    //-------------------------------------------
    //	Summary:
    //	    Insert a text node after a child node of current node
    //	Parameters:
    //      [in]node                        - the child node inserting before
    //      [in]pText                       - the text
    //      [out]isCDATA                    - is in CDATA block
    //	Remarks:
    //	    only the element node can call this function to insert a text node
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode InsertTextAfter(XMLDomNode& node, const DK_CHAR* pText, DK_BOOL isCDATA = DK_FALSE);
    DK_ReturnCode InsertTextAfter(XMLDomNode& node, const DK_WCHAR* pText, DK_BOOL isCDATA = DK_FALSE);
    
    //-------------------------------------------
    //	Summary:
    //	    Set the text of current text node
    //	Parameters:
    //      [in]pText                       - the text
    //	Remarks:
    //	    only the text node can call this function to set text
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode SetText(const DK_CHAR* pText);
    DK_ReturnCode SetText(const DK_WCHAR* pText);

public:
    //-------------------------------------------
    //	Summary:
    //	    remove a child node from cur node
    //	Parameters:
    //      [in]node             - the node to be removed
    //	Remarks:
    //	    only the document and element node can call this function to 
    //      remove a child node
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode RemoveChildNode(XMLDomNode& node);

    //-------------------------------------------
    //	Summary:
    //	    remove an attribute from cur node, overloaded
    //	Parameters:
    //      [in]pAttrName               - the attribute name
    //      [in]pAttrPrefix             - the attribute namespace prefix
    //      or
    //      [in]index               - the index of the attribute
    //	Remarks:
    //	    only element node can call this function to 
    //      remove an attribute
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode RemoveAttribute(const DK_CHAR* pAttrName, const DK_CHAR* pAttrPrefix = DK_NULL);
    DK_ReturnCode RemoveAttribute(const DK_WCHAR* pAttrName, const DK_WCHAR* pAttrPrefix = DK_NULL);
    DK_ReturnCode RemoveAttribute(const DK_INT index);
    
    //-------------------------------------------
    //	Summary:
    //	    remove the default namespace from cur node, overloaded
    //	Parameters:
    //      null
    //	Remarks:
    //	    only element node can call this function to 
    //      remove an attribute
    //	Return Value:
    //	    DK_ReturnCode
    //-------------------------------------------
    DK_ReturnCode RemoveDefaultNamespace();

private:
    DK_ReturnCode CheckCurNode(DK_XML_NODE_TYPE nodeType);
    DK_ReturnCode CheckCurNode(DK_XML_NODE_TYPE nodeType1, DK_XML_NODE_TYPE nodeType2);
    DK_ReturnCode InnerGetFirstElementChild(XMLDomNode* pFirstElementChild, const DK_CHAR* pNodeName, const DK_CHAR* pNodePrefix = DK_NULL) const;
    DK_ReturnCode InnerGetNextSiblingElement(XMLDomNode* pNextSiblingElement, const DK_CHAR* pNodeName, const DK_CHAR* pNodePrefix = DK_NULL) const;
    DK_ReturnCode InnerInsertElementBefore(XMLDomNode& node, XMLDomNode* pInsertedNode, const DK_CHAR* pElementName, const DK_CHAR* pPrefix = DK_NULL);
    DK_ReturnCode InnerInsertElementAfter(XMLDomNode& node, XMLDomNode* pInsertedNode, const DK_CHAR* pElementName, const DK_CHAR* pPrefix = DK_NULL);

private:
    DK_VOID* m_pDkTiXmlNode;
    DK_BOOL m_isInDomTree;
};

#endif
