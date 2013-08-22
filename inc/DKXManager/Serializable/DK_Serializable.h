
//===================================
//    Summary:
//        序列化接口
//    Usage:
//        将数据信息写入/读取 XML 的操作流程。
//  Remarks：
//      
//    Author:
//        Xuk
//====================================

#ifndef __DK_Serializable_H__
#define __DK_Serializable_H__

//========================================

#include<string>
#include<sstream>
//#include<iostream>
#include "XMLDomDocument.h"
#include "XMLDomNode.h"
using std::string;

//=========================================


class DK_Serializable
{
public:
    DK_Serializable(){};
    virtual ~DK_Serializable(){};

public:

    //----------------------------------------------------
    // Summary:
    //        将基本数据写进XMLNODE中
    // Parameters:
    //        [in] _xmlNode    -用于存储基本数据的结点，每一条数据
    //                         作为其子结点添加到该结点中。
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //         所有需要序列化的对象，都要实现这个接口
    //----------------------------------------------------
    virtual bool WriteToXML(XMLDomNode& node) const =0;

    //----------------------------------------------------
    // Summary:
    //        把基本数据从XMLNODE读取出来
    // Parameters:
    //        [in] _xmlNode    -存储基本数据的结点，其每个子结点对应于一条基本属性，
    //                         通过对其解析把数据写回成员变量。
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //         所有需要序列化的对象，都要实现这个接口
    //----------------------------------------------------
    virtual bool ReadFromXml(const XMLDomNode& node)=0;
public:
    DK_ReturnCode AddCommentNodeToXML(string _strName,XMLDomNode& _xmlNode/*,XMLDomNode& _SubNode*/) const;

    //-------------------------------------
    //    Summary:
    //        为父结点添加原子子结点
    //    Parameters:
    //        [in]_pNode            父结点，将会为其添加一个原子子结点
    //        [in]_pName            原子结点的名字
    //        [in]_pValue            原子结点的值
    //    Remarks:
    //        所谓原子结点就是没有子结点，只有值的结点
    //-------------------------------------
    DK_ReturnCode AddAtomSubNode(XMLDomNode &_pNode,const string &_strName,const string &_strValue) const;

    //-----------------------------------
    //    Summary:
    //        读取原子结点的值
    //    Parameters:
    //        [in]_pNode            XML原子结点
    //        [in]_pValueStr        用于存储从结点中读出来的值
    //-----------------------------------
    DK_ReturnCode ReadAtomNodeValue(XMLDomNode &_pNode,string &_strValue) const;

};

#endif//__DK_Serializable_H__


