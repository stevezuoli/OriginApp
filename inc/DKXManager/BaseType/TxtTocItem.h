//===========================================================================
// Summary:
//        TXT目录条目结构。
// Usage:
//        可以保存目录名以及目录对应正文中的绝对偏移
// Remarks:
//        用三级索引的结构来表示某一条目录在正文中的绝对偏移
// Date:
//        2011-12-27
// Author:
//        Xu Kai (xukai@duokan.com)
//===========================================================================

#ifndef _DKXMANAGER_BOOKINFO_TXTTOCITEM_H_
#define _DKXMANAGER_BOOKINFO_TXTTOCITEM_H_

#include "DKXManager/BaseType/CT_RefPos.h"
#include "DKXManager/Serializable/DK_Serializable.h"
#include <string>

class TxtTocItem 
{
private:
    static const char* TXTTOCTITLE;
    static const char* TXTTOCPOS;
public:
    TxtTocItem();
    ~TxtTocItem();

public:

    //----------------------------------------------------
    // Summary:
    //      设置目录名
    // Parameters:
    //        [in] _strTitle    目录名
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetTitle(const std::string &_strTitle);

    //----------------------------------------------------
    // Summary:
    //      获得目录名
    // Parameters:
    //        NULL
    // Return Value:
    //        成功返回目录名,失败返回""
    // Remarks:
    //        NULL
    //----------------------------------------------------
    std::string GetTitle()const;

    //----------------------------------------------------
    // Summary:
    //      设置目录对应正文偏移
    // Parameters:
    //        [in] _clsPos    偏移量（三级索引）
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetRefPos(const CT_RefPos &_clsPos);

    //----------------------------------------------------
    // Summary:
    //      获得目录对应的正文偏移
    // Parameters:
    //        NULL
    // Return Value:
    //        返回目录相对正文的偏移（三级索引）
    // Remarks:
    //        NULL
    //----------------------------------------------------
    CT_RefPos GetRefPos()const;

    //----------------------------------------------------
    // Summary:
    //        将基本数据写进XMLNODE中
    // Parameters:
    //        [in] _xmlNode    -用于存储基本数据的结点，每一条数据
    //                         作为其子结点添加到该结点中。
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool WriteToXML(XMLDomNode& node) const;

    //----------------------------------------------------
    // Summary:
    //        把基本数据从XMLNODE读取出来
    // Parameters:
    //        [in] _xmlNode    -存储基本数据的结点，其每个子结点对应于一条其本属性，
    //                         通过对其解析把数据写回成员变量。
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool ReadFromXml(const XMLDomNode& node);

private:
    static bool ReadPosFunc(const XMLDomNode* node, void* arg);
    std::string m_strTitle;
    CT_RefPos    m_clsPos;
};
#endif//_DKXMANAGER_BOOKINFO_TXTTOCITEM_H_
