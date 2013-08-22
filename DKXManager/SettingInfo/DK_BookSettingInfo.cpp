#include "DKXManager/SettingInfo/DK_BookSettingInfo.h"

using namespace std;
DK_BookSettingInfo::DK_BookSettingInfo()
{
    m_mPdfSettingField.clear();
}
DK_BookSettingInfo::~DK_BookSettingInfo()
{
    m_mPdfSettingField.clear();
}

bool DK_BookSettingInfo::SetPdfSettingField(const std::string& _strName,const std::string& _strValue)
{
    if(_strName.empty() || _strValue.empty())
    {
        return false;
    }
    m_mPdfSettingField[_strName] = _strValue;
    return true;
}

string DK_BookSettingInfo::GetPdfSettingField(const std::string& _strName) const
{
    if(_strName.empty())
    {
        return "";
    }
    map<string,string>::const_iterator  itr = m_mPdfSettingField.find(_strName);
    if(itr == m_mPdfSettingField.end())
    {
        return "";
    }
    return itr->second;
}


bool DK_BookSettingInfo::WriteToXML(XMLDomNode& _xmlNode) const
{
    for(map<string,string>::const_iterator itr = m_mPdfSettingField.begin(); itr != m_mPdfSettingField.end(); itr++)
    {
        AddAtomSubNode(_xmlNode,itr->first,itr->second);
    }
    return true;
}
bool DK_BookSettingInfo::ReadFromXml(const XMLDomNode &_xmlNode)
{
    XMLDomNode TmpNode;
    if(DKR_OK == _xmlNode.GetFirstElementChild(&TmpNode))
    {
        do
        {
            string strTempValue = "";
            if(DK_NULL == TmpNode.GetNodeValue())
            {
                continue;
            }

            if(DKR_OK == ReadAtomNodeValue(TmpNode,strTempValue) && !strTempValue.empty())
            {
                m_mPdfSettingField[TmpNode.GetNodeValue()] = strTempValue;
            }
        }while(TmpNode.GetNextSibling(&TmpNode) == DKR_OK);
    }

    return true;
}
