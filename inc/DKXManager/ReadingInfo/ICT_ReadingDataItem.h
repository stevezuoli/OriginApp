//===========================================================================
// Summary:
//        用户阅读数据的接口
// Usage:
//        用于读取用户相关阅读数据
// Remarks:
//        NULL
// Date:
//        2011-12-16
// Author:
//        Xu Kai (xukai@duokan.com)
//===========================================================================

#ifndef _ICT_ReadingDataItem_H_
#define _ICT_ReadingDataItem_H_

#include "DKXManager/BaseType/CT_RefPos.h"
#include <string>
#include <vector>
#include <tr1/memory>

#define TYPE_LIST(MACRO_NAME)\
    MACRO_NAME(UNKNOWN)\
    MACRO_NAME(BOOKMARK)\
    MACRO_NAME(COMMENT)\
    MACRO_NAME(DIGEST)\
    MACRO_NAME(PROGRESS)\

#define TYPE_LIST_INDEX(TypeId)\
    TypeId,

#define TYPE_LIST_NAME(TypeId)\
    #TypeId,

using std::string;
class ICT_ReadingDataItem;
typedef std::tr1::shared_ptr<ICT_ReadingDataItem> ICT_ReadingDataItemSPtr;
typedef std::vector<ICT_ReadingDataItemSPtr> ICT_ReadingDataItemSPtrList;

class ICT_ReadingDataItem
{
public:
    ICT_ReadingDataItem(){}
    virtual ~ICT_ReadingDataItem(){}

    static bool CompareByCreateTime(ICT_ReadingDataItemSPtr lhs, ICT_ReadingDataItemSPtr rhs)
    {
        return lhs->GetCreateTime().compare(rhs->GetCreateTime()) > 0;
    }
public:
    enum UserDataType
    {
        TYPE_LIST(TYPE_LIST_INDEX)
        TYPECOUNT
    };

	//----------------------------------------------------
	// Summary:
	//        获得内核版本信息
	// Parameters:
	//        NULL
	// Return Value:
	//        返回内核版本信息
	// Remarks:
	//        NULL
	//----------------------------------------------------
	virtual std::string GetKernelVersion() const = 0;

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
    virtual UserDataType GetUserDataType() const = 0;

    //----------------------------------------------------
    // Summary:
    //        设置用户数据类型
    // Parameters:
    //        [in] _eType    用户数据类型
    // Return Value:
    //        成功返回true失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    virtual bool SetUserDataType(UserDataType _eType) = 0;

    //----------------------------------------------------
    // Summary:
    //        获得ID属性，ID作为用户数据在DKX系统中的唯一标识
    //----------------------------------------------------
    virtual string GetId() const = 0;
    virtual void SetId(const char* id) = 0;

    //----------------------------------------------------
    // Summary:
    //        获取/设置数据的创建时间
    //----------------------------------------------------
    virtual string GetCreateTime() const = 0;
    virtual bool SetCreateTime(const string &_strCreateTime) = 0;    

    //----------------------------------------------------
    // Summary:
    //        获取/设置数据最后修改时间
    //----------------------------------------------------
    virtual string GetLastModifyTime() const = 0;
    virtual bool SetLastModifyTime(const string &_strLastModTime) = 0;    

    //----------------------------------------------------
    // Summary:
    //        获取/设置数据的创建者
    //----------------------------------------------------
    virtual string GetCreator() const = 0;
    virtual bool SetCreator(const string &_strCreator) = 0;    

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
    virtual CT_RefPos GetBeginPos() const = 0;

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
    virtual bool SetBeginPos(const CT_RefPos &_clsRefPos) = 0;

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
    virtual CT_RefPos GetEndPos() const = 0;

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
    virtual bool SetEndPos(const CT_RefPos &_clsRefPos) = 0;

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
    virtual void SetComicsFrameMode(bool frameMode) = 0;
    virtual bool IsComicsFrameMode() const = 0;

    //----------------------------------------------------
    // Summary:
    //        获取阅读数据占全文百分比
    // Parameters:
    //        NULL
    // Return Value:
    //        返回阅读数据占全文百分比
    // Remarks:
    //        NULL
    //----------------------------------------------------
    virtual int GetPercent() const = 0;

    //----------------------------------------------------
    // Summary:
    //        设置阅读数据占全文百分比
    // Parameters:
    //        [int] percent    阅读数据在正文中的百分比
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    virtual bool SetPercent(int  percent) = 0;

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
    virtual string GetBookContent() const = 0;

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
    virtual bool SetBookContent(const string &_strContent) = 0;

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
    virtual string GetUserContent() const = 0;

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
    virtual bool SetUserContent(const string &_strContent) = 0;

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
    virtual void SetChapterTitle(const string &_strChapterTitle) = 0;
    virtual string GetChapterTitle() const = 0;

    virtual void SetColor(const char* color) = 0;
    virtual string GetColor() const = 0;

    virtual void SetTag(const char* tag) = 0;
    virtual string GetTag() const = 0;

    virtual bool IsUploaded() const = 0;
    virtual void SetUploaded(bool uploaded) = 0;
};
#endif//_ICT_ReadingDataItem_H_

