//===========================================================================
// Summary:
//        书籍在DKX中存储的数据结构
// Usage:
//        可以获取/设置每本书的书籍信息，阅读数据，设置数据等
// Remarks:
//        NULL
// Date:
//        2011-12-09
// Author:
//        Xu Kai (xukai@duokan.com)
//===========================================================================
#ifndef __CT_DKX_H__
#define __CT_DKX_H__

//===========================================================================

#include "DKXManager/BookInfo/DK_BookInfo.h"
#include "DKXManager/ReadingInfo/CT_ReadingData.h"
#include "DKXManager/SettingInfo/DK_BookSettingInfo.h"
#include <string>
#include <map>
#include <vector>
#include "BookReader/IBookReader.h"

namespace dk
{
namespace bookstore
{
namespace sync
{
    class ReadingBookInfo;
}
}
}

using namespace dk::bookstore::sync;

//===========================================================================


class DKXBlock :public DK_Serializable
{
private:
    static const char* FILEITEM;
    static const char* FILEPATH;
    static const char* HEADER;
	static const char* VERSION;
    static const char* BOOKINFO;
    static const char* READINGDATA;
    static const char* SETTINGDATA;
	static const char* SUBFILERELATIVEPATH;
	static const char* USERNAME;
public:
    DKXBlock(const string _strFilePath);
    virtual ~DKXBlock();

public:
#define DECL_SETFUNC(funcName, type) \
    bool Set##funcName(type v); 

#define DECL_GETFUNC(funcName, type) \
    type Get##funcName();

#define DECL_FUNC(funcName, typeset, typeget) \
    DECL_GETFUNC(funcName, typeget);\
    DECL_SETFUNC(funcName, typeset);

    DECL_FUNC(AddOrder, long, long)
    DECL_FUNC(CreateTime, const std::string&, std::string)
    DECL_FUNC(LastReadTime, const std::string&, std::string)
    DECL_FUNC(ReadingOrder, long, long)
    DECL_FUNC(FileType, const std::string&, std::string)
    DECL_FUNC(BookName, const std::string&, std::string)
    DECL_FUNC(BookAuthor, const std::string&, std::string)
    DECL_FUNC(BookID, const char*, std::string)
    DECL_FUNC(BookSource, const std::string&, std::string)
    DECL_FUNC(BookAbstract, const std::string&, std::string)
    DECL_FUNC(BookPassword, const std::string&, std::string)
    DECL_FUNC(BookRevision, const char*, std::string)
    DECL_FUNC(FileSize, const std::string&, std::string)
    DECL_FUNC(Sync, bool, bool)
    DECL_FUNC(Crash, bool, bool)
    DECL_FUNC(TocExtractedState, bool, bool)
    DECL_FUNC(TxtToc, const std::vector<TxtTocItem>&, std::vector<TxtTocItem>)
    int GetDataVersion();
    bool SetDataVersion(int dataVersion);
    bool SetReadingDataModified(bool modified);
    bool IsReadingDataModified();
    bool MarkReadingDataAsUploaded();
    int GetBookmarkCount();
    bool GetBookmarkByIndex(int index, ICT_ReadingDataItem* readingDataItem);
    bool AddBookmark(const ICT_ReadingDataItem* readingDataItem);
    bool SetProgress(const ICT_ReadingDataItem* readingDataItem);
    bool GetProgress(ICT_ReadingDataItem* readingDataItem);
    bool RemoveBookmarkByIndex(int index);
    bool GetAllBookmarks(std::vector<ICT_ReadingDataItem*>* bookmarks);
#undef DECL_FUNC
#undef DECL_GETFUNC
#undef DECL_SETFUNC
    bool GetPDFModeController(PdfModeController* modeController);
    bool SetPDFModeController(const PdfModeController& modeController);
    bool GetPDFDarkenLevel(int* value);
    bool SetPDFDarkenLevel(int value);




    //----------------------------------------------------
    // Summary:
    //        获取当前书籍的书籍信息
    // Parameters:
    //        NULL
    // Return Value:
    //        成功返回书籍信息数据块的指针,失败返回NULL
    // Remarks:
    //        NULL
    //----------------------------------------------------
    DK_BookInfo *GetBookInfo();

    //----------------------------------------------------
    // Summary:
    //        获取当前书籍的阅读信息包括书签书摘，阅读进度等
    // Parameters:
    //        NULL
    // Return Value:
    //        成功返回阅读信息数据块的指针,失败返回NULL
    // Remarks:
    //        NULL
    //----------------------------------------------------
    CT_ReadingData *GetReadingData();

    //----------------------------------------------------
    // Summary:
    //        获取当前书籍的设置信息
    // Parameters:
    //        NULL
    // Return Value:
    //        成功返回设置信息数据块的指针,失败返回NULL
    // Remarks:
    //        NULL
    //----------------------------------------------------
    DK_BookSettingInfo *GetSettingInfo();
    bool Serialize(const string &_strFilePath);
    bool Serialize()
    {
        return Serialize(m_strFilePath);
    }
	bool UnSerialize(const string &_strFilePath);
	bool UnSerialize()
	{
		return UnSerialize(m_strFilePath);
	}

    bool MergeCloudBookmarks(const ReadingBookInfo& readingBookInfo, int cloudDataVersion);

    const std::string& GetFilePath() const
    {
        return m_strFilePath;
    }
private:
    string GetPDFSettingString(const string& name);
    bool SetPDFSettingString(const string& name, const string& value);    

    bool SetPDFSettingInt(const std::string& name, int value);
    bool GetPDFSettingInt(const std::string& name, int* value);

    bool SetPDFSettingUInt(const std::string& name, unsigned int value);
    bool GetPDFSettingUInt(const std::string& name, unsigned int* value);

    bool SetPDFSettingDouble(const std::string& name, double value);
    bool GetPDFSettingDouble(const std::string& name, double* value);
    
    
    virtual bool WriteToXML(XMLDomNode& _xmlNode) const;
    virtual bool ReadFromXml(const XMLDomNode &_xmlNode);
	string GetDkxErrorFilePathByBookPath(string _strBookPath);
    XMLDomNode *LoadXmlAndGetRootElemnet(XMLDomDocument *_pDocument,const string &_strDkxPath);
    bool RemoveCurFileNodeFromRootNode(XMLDomNode *_pRootNode,const string &_strDkxPath);
	void DeleteWrongData(const std::string& dkxFilePath);
private:
    static std::string GetCurrentUserName();
    DK_BookInfo m_bookInfo;
    std::map<std::string, CT_ReadingData> m_readingDatas;
    DK_BookSettingInfo m_SettingData;
    string m_strFilePath;
	string m_strVersion;
    //TODO: using this one to write the xml file
    //CT_DkxSerializeProxy m_SerializeProxy;

};

#endif    //__CT_DKXDATA_H__

