#include "Utility/XmlUtil.h"
#include <algorithm>
#include <stdio.h>
#include "DKXManager/BaseType/CT_RefPos.h"
#include <algorithm>

namespace dk
{
namespace utility
{
bool XmlUtil::ParseXmlNode(const XMLDomNode* domNode, XmlParseData* parseData)
{
    if (domNode == NULL || parseData == NULL)
    {
        return false;
    }

    XMLDomNode tmpNode1, tmpNode2;
    if (domNode->GetFirstElementChild(&tmpNode1) != DKR_OK)
    {
        return false;
    }

    XMLDomNode* nextNode = &tmpNode1;
    XMLDomNode* currentNode = &tmpNode2;
    do
    {
        std::swap(currentNode, nextNode);
        ParseSingleXmlNode(currentNode, parseData);
    } while (currentNode->GetNextSibling(nextNode) == DKR_OK);
    return true;
}

bool XmlUtil::ParseSingleXmlNode(const XMLDomNode* domNode,
        XmlParseData* parseData)
{
    const char* currentNodeValue = domNode->GetNodeValue();
    for (XmlParseData* currentParseData = parseData;
            NULL != currentParseData->nodeName;
            ++currentParseData)
    {
        if (strcmp(currentNodeValue, currentParseData->nodeName))
        {
            continue;
        }
        void* dataPointer = currentParseData->dataPointer;
        if (currentParseData->callback)
        {
            if (XNDT_COMPLEX == currentParseData->dataType)
            {
                currentParseData->isDataSet = 
                    currentParseData->callback(domNode, dataPointer);
            }
            else if (XNDT_FOREACH_CHILD == currentParseData->dataType)
            {
                XMLDomNode child1;
                XMLDomNode child2;
                if (DKR_OK == domNode->GetFirstElementChild(&child2))
                {
                    XMLDomNode* pChild1 = &child1;
                    XMLDomNode* pChild2 = &child2;
                    do
                    {
                        std::swap(pChild1, pChild2);
                        currentParseData->callback(pChild1, dataPointer);
                    } while (pChild1->GetNextSibling(pChild2) == DKR_OK);
                    currentParseData->isDataSet = true;
                }
            }
        }
        else
        {
            if (XNDT_CT_REFPOS == currentParseData->dataType)
            {
                currentParseData->isDataSet =
                    ((CT_RefPos*)dataPointer)->ReadFromXml(*domNode);
            }
            else
            {
            XMLDomNode  textNode;
            if (DKR_OK == domNode->GetLastChild(&textNode)
                    && DK_XML_NODE_TEXT == textNode.GetNodeType())
            {
                const char* textNodeValue = textNode.GetNodeValue();
                switch (currentParseData->dataType)
                {
                    case XNDT_INT:
                        *(int*)dataPointer = atoi(textNodeValue);
                        break;
                    case XNDT_STRING:
                        *(std::string*)dataPointer = textNodeValue;
                        break;
                    default:
                        break;
                }

            }
            }
        }
    }
    return  true;
}

bool XmlUtil::BuildXmlNode(XMLDomNode* domNode, XmlStoreData* storeData)
{
    for (XmlStoreData* currentStoreData = storeData;
            NULL != currentStoreData->nodeName;
            ++currentStoreData)
    {
        domNode->AddElement(currentStoreData->nodeName);
        XMLDomNode child;
        domNode->GetLastChild(&child);

        const void* dataPointer = currentStoreData->dataPointer;
        char buf[100];
        switch (currentStoreData->dataType)
        {
            case XNDT_INT:
                snprintf(buf, DK_DIM(buf), "%d", *(const int*)dataPointer);
                child.AddText(buf);
                break;
            case XNDT_STRING:
                child.AddText(((const std::string*)dataPointer)->c_str());
                break;
            case XNDT_COMPLEX:
            case XNDT_FOREACH_CHILD:
                currentStoreData->callback(&child, dataPointer);
                break;
            default:
                break;
        }
    }
    return true;
}

} // namespace utility
} // namespace dk
