
#include "DKXManager/Serializable/DK_Serializable.h"
DK_ReturnCode DK_Serializable::AddCommentNodeToXML(string _strName,XMLDomNode& _xmlNode/*,XMLDomNode& _SubNode*/) const
{
    if(_strName.empty())
    {
        return DKR_FAILED;
    }

    return _xmlNode.AddElement(_strName.c_str());
    //return _xmlNode.GetLastChild(&_SubNode);

}

DK_ReturnCode DK_Serializable::AddAtomSubNode(XMLDomNode &_DomNode,const string &_strName,const string &_strValue) const
{
    if(_strName.empty() || _strValue.empty())
    {
        return DKR_FAILED;
    }

    XMLDomNode TmpNode;
    _DomNode.AddElement(_strName.c_str());
    DK_ReturnCode rc = _DomNode.GetLastChild(&TmpNode);
    if(rc != DKR_OK)
    {
        return rc;
    }

    TmpNode.AddText(_strValue.c_str());
    return DKR_OK;

}


DK_ReturnCode DK_Serializable::ReadAtomNodeValue(XMLDomNode &_DomNode,string &_strValue) const
{
    XMLDomNode TmpNode;
    DK_ReturnCode rc = _DomNode.GetLastChild(&TmpNode);
    if(rc != DKR_OK)
    {
        return rc;
    }
    if(TmpNode.GetNodeType() == DK_XML_NODE_TEXT)
    {
        if(DK_NULL != TmpNode.GetNodeValue())
        {
            _strValue = TmpNode.GetNodeValue();
            return DKR_OK;
        }
    }
    return DKR_FAILED;
}
