#include "DlManagerSerialization.h"
#include "libxml/tree.h"
#include<sstream>
#include<iostream>
#include<fstream>
using namespace std;

DlSerializationParentsNode &DlSerializationParentsNode::AddChildNode(const std::string &_name)
{
    xmlNodePtr pnode = NULL;
    if(this->m_pParent)
    {
        pnode = xmlNewNode(NULL,BAD_CAST(_name.c_str()));
        if(pnode)
        {
            xmlAddChild(m_pParent,pnode);
        }

    }
    DlSerializationParentsNode node(pnode);
    this->m_ChildNodeList.push_back(node);
    return m_ChildNodeList[m_ChildNodeList.size() - 1];
}

DlSerializationParentsNode &DlSerializationParentsNode::AddChildNode(const std::string &_name,const std::string &_value)
{
    xmlNodePtr pnode = NULL;
    if(this->m_pParent)
    {
        pnode = xmlNewNode(NULL,BAD_CAST(_name.c_str()));
        if(pnode)
        {
            xmlNodePtr content = xmlNewText(BAD_CAST(_value.c_str()));

            xmlAddChild(pnode,content);

            xmlAddChild(m_pParent,pnode);
        }

    }
    DlSerializationParentsNode node(pnode);
    this->m_ChildNodeList.push_back(node);
    return m_ChildNodeList[m_ChildNodeList.size() - 1];
}


DlSerializationParentsNode &DlSerializationParentsNode::AddChildNode(const std::string &_name,const bool &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddChildNode(_name,value);

}
DlSerializationParentsNode &DlSerializationParentsNode::AddChildNode(const std::string &_name,const short &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddChildNode(_name,value);

}
DlSerializationParentsNode &DlSerializationParentsNode::AddChildNode(const std::string &_name,const unsigned short &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddChildNode(_name,value);
}
DlSerializationParentsNode &DlSerializationParentsNode::AddChildNode(const std::string &_name,const int &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddChildNode(_name,value);
}
DlSerializationParentsNode &DlSerializationParentsNode::AddChildNode(const std::string &_name,const unsigned int &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddChildNode(_name,value);
}
DlSerializationParentsNode &DlSerializationParentsNode::AddChildNode(const std::string &_name,const long &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddChildNode(_name,value);
}
DlSerializationParentsNode &DlSerializationParentsNode::AddChildNode(const std::string &_name,const unsigned long &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddChildNode(_name,value);
}
DlSerializationParentsNode &DlSerializationParentsNode::AddChildNode(const std::string &_name,const float &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddChildNode(_name,value);
}
DlSerializationParentsNode &DlSerializationParentsNode::AddChildNode(const std::string &_name,const double &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddChildNode(_name,value);
}
DlSerializationParentsNode &DlSerializationParentsNode::AddChildNode(const std::string &_name,const long double &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddChildNode(_name,value);
}


bool DlSerializationParentsNode::AddAttribute(const std::string &_name,const std::string &_value)
{
    if(!this->m_pParent || _name.empty() || _value.empty())
    {
        return false;
    }
    xmlAttrPtr pAttr = xmlNewProp(m_pParent,BAD_CAST(_name.c_str()),BAD_CAST (_value.c_str()));
    if(!pAttr)
    {
        return false;
    }
    return true;
}


bool DlSerializationParentsNode::AddAttribute(const std::string &_name,const bool &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddAttribute(_name,value);

}
bool DlSerializationParentsNode::AddAttribute(const std::string &_name,const short &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddAttribute(_name,value);

}
bool DlSerializationParentsNode::AddAttribute(const std::string &_name,const unsigned short &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddAttribute(_name,value);
}
bool DlSerializationParentsNode::AddAttribute(const std::string &_name,const int &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddAttribute(_name,value);
}
bool DlSerializationParentsNode::AddAttribute(const std::string &_name,const unsigned int &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddAttribute(_name,value);
}
bool DlSerializationParentsNode::AddAttribute(const std::string &_name,const long &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddAttribute(_name,value);
}
bool DlSerializationParentsNode::AddAttribute(const std::string &_name,const unsigned long &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddAttribute(_name,value);
}
bool DlSerializationParentsNode::AddAttribute(const std::string &_name,const float &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddAttribute(_name,value);
}
bool DlSerializationParentsNode::AddAttribute(const std::string &_name,const double &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddAttribute(_name,value);
}
bool DlSerializationParentsNode::AddAttribute(const std::string &_name,const long double &_value)
{
    string value = "";
    //TODO:是否需要初始化。怎么初始化?
    stringstream spd;
    spd << _value;
    value = spd.str();
    return AddAttribute(_name,value);
}


DlSerializationParentsNode &DlManagerSerialization::CreateRootNode(std::string _name)
{
    if(m_pDoc)
    {
        xmlFreeDoc(m_pDoc);
        m_pDoc = NULL;
        this->m_RootNode.ClearChildNodeList();
    }
    m_pDoc = xmlNewDoc(BAD_CAST"1.0");
    if(m_pDoc)
    {
        xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST(_name.c_str()));

        //设置根节点
        if(root_node)
        {
            xmlDocSetRootElement(m_pDoc,root_node);
            this->m_RootNode.SetXmlNodePtr(root_node);
        }
    }
    return m_RootNode;
}

bool DlManagerSerialization::SaveXmlFile(std::string _filename)
{
    if(m_pDoc)
    {
        int nRel = xmlSaveFileEnc(_filename.c_str(),this->m_pDoc,"utf8");

        xmlFreeDoc(m_pDoc);
        m_pDoc = NULL;
        if (nRel != -1)
        {

           return true;

        }
    }
    return false;
}

bool DlManagerSerialization::ReadXmlFile(std::string _filename)
{
    xmlDocPtr doc = NULL;           //定义解析文档指针

    xmlNodePtr curNode = NULL;      //定义结点指针(你需要它为了在各个结点间移动)

    FILE *fp = fopen(_filename.c_str(),"rb");
    if(fp == NULL)
    {
        return false;
    }
    fseek(fp,0,SEEK_END);
    int iLength = ftell(fp);
    char *pBuffer = new char[iLength + 1];
    if(pBuffer == NULL)
    {
        fclose(fp);
        return false;
    }
    fseek(fp,0,SEEK_SET);
    fread(pBuffer,1,iLength,fp);
    fclose(fp);
    fp = NULL;
    pBuffer[iLength] = 0;
    doc = xmlReadMemory(pBuffer,iLength,NULL,"utf8",XML_PARSE_RECOVER);
    delete[] pBuffer;
    pBuffer = NULL;
    if (NULL == doc)
    {
       return false;
    }

    curNode = xmlDocGetRootElement(doc); //确定文档根元素
    if(NULL == curNode)
    {
    	xmlFreeDoc(doc);
    	doc = NULL;
        return false;
    }

    this->m_RootNode.SetXmlNodePtr(curNode);
    m_pDoc = doc;
    return true;
}


xmlNodePtr DlManagerSerialization::GetRootNode()
{
    return m_RootNode.GetXmlNodePtr();
}
