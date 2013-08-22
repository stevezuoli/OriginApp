#include "Common/UserInfo.h"
#include "Utility/XmlUtil.h"
#include "Utility/PathManager.h"
#include "Utility.h"
#include <stdio.h>

using namespace dk::utility;
using namespace DkFormat;

namespace dk
{
namespace account
{

static DK_ReturnCode ReadAtomNodeValue(XMLDomNode &_DomNode, std::string &_strValue)
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

XiaomiUserInfo::XiaomiUserInfo()
{
}

bool XiaomiUserInfo::LoadFromFile(const char* path)
{
    XMLDomDocument* pDocument = XMLDomDocument::CreateDocument();
    if (0 == pDocument)
    {
        return false;
    }

    if (!pDocument->LoadXmlFromFile(path, false))
    {
        XMLDomDocument::ReleaseDocument(pDocument);
        return false;
    }

    XMLDomNode* pRootNode = pDocument->RootElement();
    if (0 == pRootNode)
    {
        XMLDomDocument::ReleaseDocument(pDocument);
        return false;
    }

    DK_INT count = 0;
    m_infoMap.clear();
    pRootNode->GetChildrenCount(&count);
    for (DK_INT i = 0; i < count; ++i)
    {
        XMLDomNode TmpNode;
        pRootNode->GetChildByIndex(i, &TmpNode);
        std::string strTempValue = "";
        if(DK_NULL == TmpNode.GetNodeValue())
        {
            continue;
        }
        
        if(DKR_OK == ReadAtomNodeValue(TmpNode, strTempValue) && !strTempValue.empty())
        {
            m_infoMap[TmpNode.GetNodeValue()] = strTempValue;
        }
    }
    XMLDomDocument::ReleaseDocument(pDocument);
    return true;
}

std::string XiaomiUserInfo::GetXiaomiID()
{
    if (m_infoMap.find("id") != m_infoMap.end())
    {
        return m_infoMap["id"];
    }
    return "";
}

std::string XiaomiUserInfo::GetToken()
{
    if (m_infoMap.find("token") != m_infoMap.end())
    {
        return m_infoMap["token"];
    }
    return "";
}

std::string XiaomiUserInfo::GetMsg()
{
    if (m_infoMap.find("msg") != m_infoMap.end())
    {
        return m_infoMap["msg"];
    }
    return "";
}

int XiaomiUserInfo::GetCode()
{
    if (m_infoMap.find("code") != m_infoMap.end())
    {
        std::string code_str = m_infoMap["code"];
        return atoi(code_str.c_str());
    }
    return -1;
}

bool XiaomiUserInfo::IsValid()
{
    return !GetXiaomiID().empty() && !GetToken().empty();
}

void XiaomiUserInfo::Clear()
{
    m_infoMap.clear();
}

bool UserInfo::Init(const XMLDomNode* domNode)
{
    BEGIN_XML_PARSE_DATA(parseData)
    {"aliasname", XNDT_STRING, &m_alias, false},
    {"duokanmail", XNDT_STRING, &m_duokanMail, false},
    {"duokanmailpwd", XNDT_STRING, &m_duokanMailPassword, false},
    {"email", XNDT_STRING, &m_email, false},
    END_XML_PARSE_DATA;
    return XmlUtil::ParseXmlNode(domNode, parseData);
}

const std::string& UserInfo::GetAlias() const
{
    return m_alias;
}

void UserInfo::SetEmail(const char* email)
{
    m_email = email;
}

const std::string& UserInfo::GetEmail() const
{
    return m_email;
}

const std::string& UserInfo::GetDuokanMail() const
{
    return m_duokanMail;
}

const std::string& UserInfo::GetDuokanMailPassword() const
{
    return m_duokanMailPassword;
}

const std::string& UserInfo::GetToken() const
{
    return m_token;
}

void UserInfo::SetToken(const char* token)
{
    m_token = token;
}

void UserInfo::LoadFromFile()
{
    const char* path = PathManager::GetUserInfoFile();
    FILE* pFile = fopen(path, "rb");
    if(!pFile)
    {
        goto err0;
        return;
    }
    if(!CUtility::ReadStringFromFile(m_email, pFile))
    {
        goto err1;
    }

    if(!CUtility::ReadStringFromFile(m_alias, pFile))
    {
        goto err1;
    }

    if(!CUtility::ReadStringFromFile(m_token, pFile))
    {
        goto err1;
    }

    if(!CUtility::ReadStringFromFile(m_duokanMail, pFile))
    {
        goto err1;
    }

    if(!CUtility::ReadStringFromFile(m_duokanMailPassword, pFile))
    {
        goto err1;
    }

err1:
    fclose(pFile);
err0:
    return;
}

void UserInfo::SaveToFile() const
{
    const char* path = PathManager::GetUserInfoFile();
    FILE* pFile = fopen(path, "wb");
    if(!pFile)
    {
        return;
    }

    CUtility::WriteStringToFile(m_email, pFile);
    CUtility::WriteStringToFile(m_alias, pFile);
    CUtility::WriteStringToFile(m_token, pFile);
    CUtility::WriteStringToFile(m_duokanMail, pFile);
    CUtility::WriteStringToFile(m_duokanMailPassword, pFile);

    fclose(pFile);
}

void UserInfo::SetAlias(const char* alias)
{
    if (NULL == alias)
    {
        return;
    }
    m_alias = alias;
}

MigrateResult::MigrateResult()
{
}

bool MigrateResult::LoadFromFile(const char* path)
{
    XMLDomDocument* pDocument = XMLDomDocument::CreateDocument();
    if (0 == pDocument)
    {
        return false;
    }

    if (!pDocument->LoadXmlFromFile(path, false))
    {
        XMLDomDocument::ReleaseDocument(pDocument);
        return false;
    }

    XMLDomNode* pRootNode = pDocument->RootElement();
    if (0 == pRootNode)
    {
        XMLDomDocument::ReleaseDocument(pDocument);
        return false;
    }

    DK_INT count = 0;
    m_infoMap.clear();
    pRootNode->GetChildrenCount(&count);
    for (DK_INT i = 0; i < count; ++i)
    {
        XMLDomNode TmpNode;
        pRootNode->GetChildByIndex(i, &TmpNode);
        std::string strTempValue = "";
        if(DK_NULL == TmpNode.GetNodeValue())
        {
            continue;
        }
        
        if(DKR_OK == ReadAtomNodeValue(TmpNode, strTempValue) && !strTempValue.empty())
        {
            m_infoMap[TmpNode.GetNodeValue()] = strTempValue;
        }
    }
    XMLDomDocument::ReleaseDocument(pDocument);
    return true;
}

std::string MigrateResult::GetXiaomiID()
{
    if (m_infoMap.find("mi_id") != m_infoMap.end())
    {
        return m_infoMap["mi_id"];
    }
    return "";
}

std::string MigrateResult::GetToken()
{
    if (m_infoMap.find("token") != m_infoMap.end())
    {
        return m_infoMap["token"];
    }
    return "";
}

std::string MigrateResult::GetMsg()
{
    if (m_infoMap.find("message") != m_infoMap.end())
    {
        return m_infoMap["message"];
    }
    return "";
}

std::string MigrateResult::GetMigratedUserID()
{
    if (m_infoMap.find("migrated_duokan_id") != m_infoMap.end())
    {
        return m_infoMap["migrated_duokan_id"];
    }
    return "";
}

void MigrateResult::Clear()
{
    m_infoMap.clear();
}


} // namespace account
} // namespace dk
