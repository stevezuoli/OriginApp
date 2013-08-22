//===========================================================================
// Summary:
//        用户阅读数据的实现
// Usage:
//        用于表示书签书摘笔记阅读进度等
// Remarks:
//        NULL
// Date:
//        2011-12-16
// Author:
//        Xu Kai (xukai@duokan.com)
//===========================================================================

#ifndef _CT_ReadingDataItemImpl_H_
#define _CT_ReadingDataItemImpl_H_

#include "DKXManager/ReadingInfo/ICT_ReadingDataItem.h"
#include "DKXManager/Serializable/DK_Serializable.h"
#include <vector>
#include <tr1/memory>

using std::string;
class CT_ReadingDataItemImpl : public ICT_ReadingDataItem , public DK_Serializable
{
private:
    static const char* USERDATATYPE;
    static const char* USERDATAID;
    static const char* CREATETIME;
    static const char* LASTMODTIME;
    static const char* CREATOR;
    static const char* BEGINPOS;
    static const char* ENDPOS;
    static const char* BOOKCONTENT;
    static const char* USERCONTENT;
	static const char* KERNELVERSION;
	static const char* COLOR; 
	static const char* TAG;
	static const char* UPLOADED;
    static const char* PERCENT;
    static const char* COMICSFRAMEMODE;
    static const char* CHAPTERTITLE;
public:
    CT_ReadingDataItemImpl();
    CT_ReadingDataItemImpl(UserDataType _eType);
    virtual ~CT_ReadingDataItemImpl();

	virtual std::string GetKernelVersion() const;

    //----------------------------------------------------
    // Summary:
    //        获得用户数据类型
    // Parameters:
    //        NULL
    // Return Value:
    //        返回用户数据的类型
    // Remarks:
    //        NULL
    //----------------------------------------------------
    virtual UserDataType GetUserDataType() const;

    virtual bool SetUserDataType(UserDataType _eType);

    //----------------------------------------------------
    // Summary:
    //        获得/创建ID属性，ID作为用户数据在DKX系统中的唯一标识
    //----------------------------------------------------
    virtual string GetId() const;
    virtual void SetId(const char*);
    bool CreateId();

    //----------------------------------------------------
    // Summary:
    //        获取/设置数据的创建时间
    //----------------------------------------------------
    virtual string GetCreateTime() const;
    virtual bool SetCreateTime(const string &_strCreateTime);    

    //----------------------------------------------------
    // Summary:
    //        获取/设置数据最后修改时间
    //----------------------------------------------------
    virtual string GetLastModifyTime() const;
    virtual bool SetLastModifyTime(const string &_strLastModifyTime);    

    //----------------------------------------------------
    // Summary:
    //        获取/设置数据的创建者
    //----------------------------------------------------
    virtual string GetCreator() const;
    virtual bool SetCreator(const string &_strCreator);    

    //----------------------------------------------------
    // Summary:
    //        获取/设置数据的百分比
    //----------------------------------------------------
    virtual int GetPercent() const;
    virtual bool SetPercent(int percent);

    //----------------------------------------------------
    // Summary:
    //        获取阅读数据的第一个索引点
    // Parameters:
    //        NULL
    // Return Value:
    //        返回阅读数据的第一个索引点
    // Remarks:
    //        NULL
    //----------------------------------------------------
    virtual CT_RefPos GetBeginPos() const;

    //----------------------------------------------------
    // Summary:
    //        设置阅读数据的第一个索引点
    // Parameters:
    //        [in] _clsRefPos    阅读数据在正文中的开始索引点
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    virtual bool SetBeginPos(const CT_RefPos &_clsRefPos);

    //----------------------------------------------------
    // Summary:
    //        获取阅读数据的第二个索引点
    // Parameters:
    //        NULL
    // Return Value:
    //        返回阅读数据的第二个索引点
    // Remarks:
    //        NULL
    //----------------------------------------------------
    virtual CT_RefPos GetEndPos() const;

    //----------------------------------------------------
    // Summary:
    //        设置阅读数据的第二个索引点
    // Parameters:
    //        [in] _clsRefPos    阅读数据在正文中的结束索引点
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    virtual bool SetEndPos(const CT_RefPos &_clsRefPos);

    //----------------------------------------------------
    // Summary:
    //        设置漫画书籍视图模式是否是桢模式
    // Parameters:
    //        [in] _clsRefPos    阅读数据在正文中的结束索引点
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        只保存在本地，不上传服务器
    //----------------------------------------------------
    virtual void SetComicsFrameMode(bool frameMode);
    virtual bool IsComicsFrameMode() const;

    //----------------------------------------------------
    // Summary:
    //        获取原文内容用于内核升级时位置的校对
    // Parameters:
    //        NULL
    // Return Value:
    //        返回原文内容
    // Remarks:
    //        NULL
    //----------------------------------------------------
    virtual string GetBookContent() const;

    //----------------------------------------------------
    // Summary:
    //        设置当前用户数据对应的原文内容
    // Parameters:
    //        [in] _strContent    阅读数据对应的原文内容
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    virtual bool SetBookContent(const string &_strContent);

    //----------------------------------------------------
    // Summary:
    //        获取用户所做内容
    // Parameters:
    //        NULL
    // Return Value:
    //        返回用户所做内容
    // Remarks:
    //        NULL
    //----------------------------------------------------
    virtual string GetUserContent() const;

    //----------------------------------------------------
    // Summary:
    //        设置用户所做内容
    // Parameters:
    //        [in] _strContent    用户所做内容
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    virtual bool SetUserContent(const string &_strContent);

    //----------------------------------------------------
    // Summary:
    //        Setting chapter title
    // Parameters:
    //        [in] _strChapterTitle The title of chapter at start position
    // Return Value:
    //        Return empty string if title is null
    // Remarks:
    //        只保存在本地，不上传服务器
    //----------------------------------------------------
    virtual void SetChapterTitle(const string &_strChapterTitle);
    virtual string GetChapterTitle() const;

    bool WriteToXML(XMLDomNode& _xmlNode) const;
    bool ReadFromXml(const XMLDomNode &_xmlNode);

    virtual void SetColor(const char* color)
    {
        if (NULL != color)
        {
            m_color = color;
        }
    }
    virtual string GetColor() const
    {
        return m_color;
    }

    virtual void SetTag(const char* tag)
    {
        if (NULL != tag)
        {
            m_tag = tag;
        }
    }
    virtual string GetTag() const
    {
        return m_tag;
    }

    virtual bool IsUploaded() const
    {
        return m_uploaded != 0;
    }

    virtual void SetUploaded(bool uploaded)
    {
        m_uploaded = uploaded;
    }

private:
    UserDataType m_eType;
    string m_strID;
    string m_strCreateTime;
    string m_strLastModifyTime;
    string m_strCreator;
    string m_strBookContent;
    string m_strUserContent;
	string m_strKernelVersion;
    string m_strChapterTitle;
    CT_RefPos m_clsBeginPos;
    CT_RefPos m_clsEndPos;
    string m_color;
    string m_tag;
    // 阅读数据是否上传
    int m_uploaded;
    int m_percent;
    int m_comicsFrame;
};
typedef std::tr1::shared_ptr<CT_ReadingDataItemImpl> CT_ReadingDataItemImplSPtr;
typedef std::vector<CT_ReadingDataItemImplSPtr> CT_ReadingDataItemImplSPtrList;
#endif//_CT_ReadingDataItemImpl_H_

