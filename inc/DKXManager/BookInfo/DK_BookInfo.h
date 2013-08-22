//===========================================================================
// Summary:
//        书籍信息相关数据的接口
// Usage:
//        书籍信息的每一项属性提供了一对GET/SET的接口，分别用来获取和设置相关属性
// Remarks:
//        NULL
// Date:
//        2011-12-09
// Author:
//        Xu Kai (xukai@duokan.com)
//===========================================================================

#ifndef _DK_BookInfo_H_
#define _DK_BookInfo_H_

//===========================================================================

#include "DKXManager/Serializable/DK_Serializable.h"
#include "DKXManager/BaseType/TxtTocItem.h"
#include <vector>
#include <string>
using std::string;

//===========================================================================

class DK_BookInfo:public DK_Serializable
{
private:
    static const char* BOOKNAME;
    static const char* BOOKABSTRACT;
    static const char* BOOKAUTHOR;
    static const char* COVERIMAGE;
    static const char* BOOKSOURCE;
    static const char* BOOKDOWNLOADURL;
    static const char* FILEPATH; 
    static const char* FILEFORMAT;
    static const char* CREATETIME;
    static const char* ADDORDER;
    static const char* LASTREADTIME;
    static const char* READINGORDER;
    static const char* FILESIZE;
    static const char* BOOKISBN;
    static const char* PASSWORD;
    static const char* ISSERIALIZED;
    static const char* DOWNLOADEDCHAPTER;
    static const char* LATESTCHAPTERTITLE;
    static const char* LATESTCHAPTER;
    static const char* BOOKTAGS;
    static const char* TAGITEM;
    static const char* TXTTOCS;
    static const char* TXTTOCITEM;
    static const char* TOCEXTRACTED;
    static const char* BOOK_REVISION;
	static const char* READINGDATA_SYNC;
	static const char* CRASHBOOK;
	static const char* BOOKID;
public:
    DK_BookInfo();
    virtual ~DK_BookInfo();

public:
    
    //-----------------------------------
    //    Summary:                       
    //            图书文件路径访问函数  
    //-----------------------------------
    bool SetBookFilePath(const string &_strPath);
    string GetBookFilePath()const;

    //-----------------------------------
    //    Summary:                       
    //            图书文件格式访问函数 
    //-----------------------------------
    string GetFileType()const;
    bool SetFileType(const string &_strType);

    //-----------------------------------
    //    Summary:                       
    //            图书文件创建时间访问函数 
    //-----------------------------------
    string GetCreateTime()const;
    bool SetCreateTime(const string &_strTime);    

    //-----------------------------------
    //    Summary:                       
    //            图书文件创建序列访问函数 
    //-----------------------------------
    long GetAddOrder()const;
    bool SetAddOrder(long _lAddOrder);
    
    //-----------------------------------
    //    Summary:                       
    //            图书文件最后阅读时间访问函数 
    //-----------------------------------
    string GetLastReadTime()const;
    bool SetLastReadTime(const string &_strTime);  
    
    //-----------------------------------
    //    Summary:                       
    //            图书文件阅读次序访问函数 
    //-----------------------------------  
    long GetReadingOrder()const;
    bool SetReadingOrder(long _lOrder);  

    //-----------------------------------
    //    Summary:                       
    //            图书名字访问函数 
    //-----------------------------------
    string GetBookName()const;
    bool SetBookName(const string &_strBookname);

    //-----------------------------------
    //    Summary:                       
    //            图书作者访问函数 
    //-----------------------------------
    string GetBookAuthor()const;
    bool SetBookAuthor(const string &_strAuthor);

    //-----------------------------------
    //    Summary:                       
    //            书籍的来源访问函数 
    //-----------------------------------
    string GetBookSource()const;
    bool SetBookSource(const string &_strSource);

    //-----------------------------------
    //    Summary:                       
    //            书籍的网络链接地址访问函数 
    //-----------------------------------
    string GetBookUrl()const;
    bool SetBookUrl(const string &_strUrl);

    //-----------------------------------
    //    Summary:                       
    //            书籍简介访问函数 
    //-----------------------------------
    string GetBookAbstract()const;
    bool SetBookAbstract(const string &_strAbstract);

    //-----------------------------------
    //    Summary:                       
    //            书籍大小访问函数 
    //-----------------------------------
    string GetFileSize()const;
    bool SetFileSize(const string &_strFileSize);

    //----------------------------------
    // Summary:
    //        书籍密码访问函数
    //----------------------------------
    string GetBookPassword()const;
    bool SetBookPassword(const string &_strPassword);

    //-----------------------------------
    //    Summary:                       
    //            获得/设置该书籍多看书籍版本
    //-----------------------------------
    string GetBookRevision() const;
    bool SetBookRevision(const char* bookRevision);

	//----------------------------------
    // Summary:
    //        标记该书是否同步过，用于时候解析过该书的书名
    //----------------------------------
	bool SetSync(bool bIsSync);
	bool GetSync() const;

	//----------------------------------
    // Summary:
    //        标记该书打开时时候会crash
    //----------------------------------
	bool SetCrash(bool bIsCrash);
	bool GetCrash() const;

	//----------------------------------
    // Summary:
    //        书籍ID 的访问函数
    //        书城书的id 与非书城书的ID 计算方式不同
    //----------------------------------
	bool SetBookID(const char* bookID);
	string GetBookID() const;

    //-------------------------------------
    //    Summary:
    //        获取文件的标签信息
    //    Parameters:
    //        NULL
    //    Return Value:
    //        返回当前文件的标签列表
    //-------------------------------------
    std::vector<string> GetFileTag() const;

    //-------------------------------------
    //    Summary:
    //        设置文件的标签信息
    //    Parameters:
    //        [in] _vFileTag    文件的标签列表
    //    Return Value:
    //        成功返回true,失败返回false
    //-------------------------------------
    bool SetFileTag(const std::vector<string> &_vFileTag);

	//-------------------------------------
	//    Summary:
	//        获取是否提取过目录的标识
	//    Parameters:
	//        NULL
	//    Return Value:
	//        已经提取过返回true,没提取过返回false
	//-------------------------------------
	bool GetTocExtractedState();

	//-------------------------------------
	//    Summary:
	//        获取是否提取过目录的标识
	//    Parameters:
	//        [in] _bExtracted 如果提取了TOC则传true,没有则传false
	//    Return Value:
	//        成功返回true,失败返回false
	//-------------------------------------
	bool SetTocExtractedState(bool _bExtracted);

	//-------------------------------------
    //    Summary:
    //        获取Txt的目录
    //    Parameters:
    //        NULL
    //    Return Value:
    //        返回当前文件的目录列表
    //-------------------------------------
    std::vector<TxtTocItem> GetTxtToc() const;

    //-------------------------------------
    //    Summary:
    //        设置Txt的目录
    //    Parameters:
    //        [in] _vTxtToc    Txt的目录
    //    Return Value:
    //        成功返回true,失败返回false
    //-------------------------------------
    bool SetTxtToc(const std::vector<TxtTocItem> &_vTxtToc);

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
    virtual bool WriteToXML(XMLDomNode& _xmlNode) const;

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
    virtual bool ReadFromXml(const XMLDomNode &_xmlNode);


    static bool WriteToXMLFunc(XMLDomNode* node, const void* arg);
private:
    static bool ReadTagsFunc(const XMLDomNode* node, void* arg);
    static bool WriteTagsFunc(XMLDomNode* node, const void* arg);
    static bool AddTxtTocFunc(const XMLDomNode* node, void* arg);
    static bool WriteTxtTocFunc(XMLDomNode* node, const void* arg);
    string		m_strBookName;
    string		m_strBookAbstract;    
    string		m_strAuthor;
    string		m_strCoverImage;
    string		m_strBookSource;
    string		m_strBookDownloadUrl;

    string		m_strFilePath;
    string		m_strFileFormat;
    string		m_strCreateTime;
    int         m_lAddOrder;
    string		m_strLastReadTime;
    int         m_lReadingOrder;
    string		m_strISBN;
    string		m_strFileSize;
    string		m_strPassword;
    int		    m_bIsSerialized;
	int		    m_bTocExtracted;
    
    std::vector<string> m_vFileTag;

    std::vector<TxtTocItem> m_vTxtToc;
    std::string  m_bookRevision;
	int			m_bIsSync;				//int值表示bool值，因为dkx保存时只有int值
	int			m_bIsCrash;
	string 		m_strBookID;
    
};

#endif//_DK_BookInfo_H_
