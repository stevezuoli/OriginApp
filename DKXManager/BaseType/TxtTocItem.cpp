#include "DKXManager/BaseType/TxtTocItem.h"
#include "Utility/XmlUtil.h"

using namespace dk::utility;

const char* TxtTocItem::TXTTOCTITLE = "TxtTocTitle";
const char* TxtTocItem::TXTTOCPOS = "TxtTocPos";

TxtTocItem::TxtTocItem()
: m_strTitle("")
{

}

TxtTocItem::~TxtTocItem()
{

}

bool TxtTocItem::SetTitle(const std::string &_strTitle)
{
    m_strTitle = _strTitle;
    return true;
}

std::string TxtTocItem::GetTitle()const
{
    return m_strTitle;
}

bool TxtTocItem::SetRefPos(const CT_RefPos &_clsPos)
{
    m_clsPos = _clsPos;
    return true;
}

CT_RefPos TxtTocItem::GetRefPos()const
{
    return m_clsPos;
}

bool TxtTocItem::WriteToXML(XMLDomNode& node) const
{
    BEGIN_XML_STORE_DATA(storeData)
        {TXTTOCTITLE, XNDT_STRING, &m_strTitle, false},
        {TXTTOCPOS, XNDT_COMPLEX, &m_clsPos, false, CT_RefPos::WriteToXMLFunc},
    END_XML_STORE_DATA;

    return XmlUtil::BuildXmlNode(&node, storeData);
}

bool TxtTocItem::ReadFromXml(const XMLDomNode& node)
{
    BEGIN_XML_PARSE_DATA(parseData)
        {TXTTOCTITLE, XNDT_STRING, &m_strTitle, false},
        {TXTTOCPOS, XNDT_COMPLEX, &m_clsPos, false, CT_RefPos::ReadFromXmlFunc},
    END_XML_PARSE_DATA;
    return XmlUtil::ParseXmlNode(&node, parseData);
}

