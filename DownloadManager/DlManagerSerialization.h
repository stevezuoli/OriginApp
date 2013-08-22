#ifndef __DLMANAGERSERIALIZATION_H_
#define __DLMANAGERSERIALIZATION_H_
#include <string>
#include <vector>
#include "libxml/parser.h"

class DlSerializationParentsNode
{
public:
    DlSerializationParentsNode(){}
    DlSerializationParentsNode(xmlNodePtr _pNode)
    {
        m_pParent = _pNode;
    }
    ~DlSerializationParentsNode(){}
    void SetXmlNodePtr(xmlNodePtr _ptr){m_pParent = _ptr;}
    xmlNodePtr GetXmlNodePtr(){return this->m_pParent;}
    DlSerializationParentsNode &AddChildNode(const std::string &_name);    // 添加一个没有值的空节点
    DlSerializationParentsNode &AddChildNode(const std::string &_name,const std::string &_value);
    DlSerializationParentsNode &AddChildNode(const std::string &_name,const bool &_value);
    DlSerializationParentsNode &AddChildNode(const std::string &_name,const short &_value);
    DlSerializationParentsNode &AddChildNode(const std::string &_name,const unsigned short &_value);
    DlSerializationParentsNode &AddChildNode(const std::string &_name,const int &_value);
    DlSerializationParentsNode &AddChildNode(const std::string &_name,const unsigned int &_value);
    DlSerializationParentsNode &AddChildNode(const std::string &_name,const long &_value);
    DlSerializationParentsNode &AddChildNode(const std::string &_name,const unsigned long &_value);
    DlSerializationParentsNode &AddChildNode(const std::string &_name,const float &_value);
    DlSerializationParentsNode &AddChildNode(const std::string &_name,const double &_value);
    DlSerializationParentsNode &AddChildNode(const std::string &_name,const long double &_value);

    bool AddAttribute(const std::string &_name,const std::string &_value);
    bool AddAttribute(const std::string &_name,const bool &_value);
    bool AddAttribute(const std::string &_name,const short &_value);
    bool AddAttribute(const std::string &_name,const unsigned short &_value);
    bool AddAttribute(const std::string &_name,const int &_value);
    bool AddAttribute(const std::string &_name,const unsigned int &_value);
    bool AddAttribute(const std::string &_name,const long &_value);
    bool AddAttribute(const std::string &_name,const unsigned long &_value);
    bool AddAttribute(const std::string &_name,const float &_value);
    bool AddAttribute(const std::string &_name,const double &_value);
    bool AddAttribute(const std::string &_name,const long double &_value);
    void ClearChildNodeList(){m_ChildNodeList.clear();}
private:
    xmlNodePtr m_pParent;
    std::vector <DlSerializationParentsNode> m_ChildNodeList;
};

class DlManagerSerialization
{
public:
    DlManagerSerialization(){m_pDoc = NULL;}
    ~DlManagerSerialization()
    {
        if(m_pDoc)
        {
            xmlFreeDoc(m_pDoc);
            m_pDoc = NULL;
        }
    }
    DlSerializationParentsNode &CreateRootNode(std::string _name);
    bool SaveXmlFile(std::string _filename);
    bool ReadXmlFile(std::string _filename);
    xmlNodePtr GetRootNode();
private:
    xmlDocPtr m_pDoc;
    DlSerializationParentsNode m_RootNode;
};
#endif

