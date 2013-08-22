//===========================================================================
// Summary:
//        相对引用点数据。
// Usage:
//        通过一对Set/Get来设置/获得相对引用点对应的章，段，字节偏移
// Remarks:
//        用三级索引的结构来表示某一个点在正文中的绝对偏移
// Date:
//        2011-12-09
// Author:
//        Xu Kai (xukai@duokan.com)
//===========================================================================
#ifndef _CT_REFPOS_H_
#define _CT_REFPOS_H_

//===========================================================================

#include "DKXManager/Serializable/DK_Serializable.h"
#include "Utility/EncodeUtil.h"

using dk::utility::EncodeUtil;

//===========================================================================


class CT_RefPos: public DK_Serializable
{
private:
    static const char* CHAPTERINDEX;
    static const char* PARAINDEX;
    static const char* ATOMINDEX;
    static const char* OFFSET_IN_CHAPTER;
    static const char* CHAPTER_ID;
public:
    
    CT_RefPos();
    bool InitFromSyncData(const XMLDomNode* node);
    CT_RefPos(int chapterIndex, int paraIndex, int elementIndex, int offsetInChapter);
    virtual ~CT_RefPos();


    //----------------------------------------------------
    // Summary:
    //        获得/设置章节索引
    //----------------------------------------------------
    bool SetChapterIndex(int chapterIndex);
    int GetChapterIndex()const;
    
    //----------------------------------------------------
    // Summary:
    //        获得/设置段落索引
    //----------------------------------------------------
    bool SetParaIndex(int paraIndex);
    int GetParaIndex()const;
    
    //----------------------------------------------------
    // Summary:
    //        获得/设置字节索引
    //----------------------------------------------------
    bool SetAtomIndex(int atomIndex);
    int GetAtomIndex()const;

    bool SetOffsetInChapter(int offsetInChapter);
    int GetOffsetInChapter() const;
    bool HasOffsetInChapter() const
    {
        return -1 != m_offsetInChapter;
    }

    void SetChapterId(const DK_WCHAR* chapterId)
    {
        m_chapterId = EncodeUtil::ToString(chapterId);
    }

    std::string GetChapterId() const
    {
        return m_chapterId;
    }

    bool HasChapterId() const
    {
        return !m_chapterId.empty();
    }

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
    virtual bool WriteToXML(XMLDomNode& node) const;

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
    virtual bool ReadFromXml(const XMLDomNode& node);

    //----------------------------------------------------
    // Summary:
    //        重载“==”运算符，使其通过判断是否所有的成员变量相等来决定二个对象是否相等
    // Parameters:
    //        [in] rhs    -待比较的对象
    // Return Value:
    //        所有成员变量相等返回true,有一个不相等返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool operator==(const CT_RefPos& rhs)
    {
        return ((m_chapterIndex == rhs.m_chapterIndex)
                && (m_paraIndex == rhs.m_paraIndex)
                && (m_atomIndex == rhs.m_atomIndex)
                && (m_offsetInChapter == rhs.m_offsetInChapter));
     }

    //----------------------------------------------------
    // Summary:
    //        比较两个三级索引的位置关系
    // Parameters:
    //        [in] _clsRefPos    待比较的三级索引
    // Return Value:
    //        int  0表示相等,正数表示本三级索引比_clsRefPos位置靠后， 负数表示本三级索引位置靠前
    // Remarks:
    //        NULL
    //----------------------------------------------------
    virtual int CompareTo(const CT_RefPos &_clsRefPos);

    DK_FLOWPOSITION ToFlowPosition() const
    {
        return DK_FLOWPOSITION(m_chapterIndex, m_paraIndex, m_atomIndex);
    }

    bool IsValid() const
    {
        return m_chapterIndex >= 0
            && m_paraIndex >= 0
            && m_atomIndex >= 0;
    }
    bool WriteToSyncDomNode(XMLDomNode* domNode) const;
    static bool WriteToXMLFunc(XMLDomNode* node, const void* arg);
    static bool ReadFromXmlFunc(const XMLDomNode* node, void* arg);
private:
    int m_chapterIndex;
    int m_paraIndex;
    int m_atomIndex;    
    int m_offsetInChapter;
    std::string m_chapterId;
};
#endif

