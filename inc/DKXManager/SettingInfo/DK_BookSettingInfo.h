//===================================
//    Summary:
//        书籍信息相关数据的接口
//    Usage:
//        书籍信息的每一项属性提供了一对GET/SET的接口，分别用来获取和设置相关属性
//    Author:
//        Xuk
//====================================

#ifndef _DK_BOOKSETTINGINFO_H_
#define _DK_BOOKSETTINGINFO_H_
#include "DKXManager/Serializable/DK_Serializable.h"
#include <map>
class DK_BookSettingInfo : public DK_Serializable
{

public:
    DK_BookSettingInfo();
    virtual ~DK_BookSettingInfo();

public:
    
    bool SetPdfSettingField(const std::string& _strName,const std::string& _strValue);

    string GetPdfSettingField(const std::string& _strName) const;
    virtual bool WriteToXML(XMLDomNode& _xmlNode) const;
    virtual bool ReadFromXml(const XMLDomNode &_xmlNode);

private:
    std::map <string,string> m_mPdfSettingField;
};

#endif//_DK_BOOKSETTINGINFO_H_
