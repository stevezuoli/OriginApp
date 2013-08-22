#ifndef __UTILITY_XMLUTIL_H__
#define __UTILITY_XMLUTIL_H__

#include "XMLDomNode.h"
#include <tr1/functional>

namespace dk
{
namespace utility
{

enum XmlNodeDataType
{
    XNDT_INT,
    XNDT_STRING,
    XNDT_COMPLEX, // 复杂样式
    XNDT_CT_REFPOS,
    XNDT_FOREACH_CHILD, // 遍历子节点，调用callback
    XNDT_NONE
};

struct XmlParseData
{
    const char* nodeName;
    XmlNodeDataType dataType;
    void* dataPointer;
    bool isDataSet;
    std::tr1::function<bool (const XMLDomNode*, void* dataPointer)> callback; 
};

struct XmlStoreData
{
    const char* nodeName;
    XmlNodeDataType dataType;
    const void* dataPointer;
    bool isDataSet;
    std::tr1::function<bool (XMLDomNode*, const void* dataPointer)> callback; 
};

#define BEGIN_XML_PARSE_DATA(x) XmlParseData x[] = {
#define END_XML_PARSE_DATA {NULL, XNDT_NONE, NULL}}

#define BEGIN_XML_STORE_DATA(x) XmlStoreData x[] = {
#define END_XML_STORE_DATA {NULL, XNDT_NONE, NULL}}


class XmlUtil
{
public:
    static bool ParseXmlNode(const XMLDomNode* domNode, XmlParseData* parseData);
    static bool BuildXmlNode(XMLDomNode* domNode, XmlStoreData* storeData);
private:
    static bool ParseSingleXmlNode(const XMLDomNode* domNode, XmlParseData* parseData);
    XmlUtil();

};

} // namespace utility
} // namespace dk
#endif
