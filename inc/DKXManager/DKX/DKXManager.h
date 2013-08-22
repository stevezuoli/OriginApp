//===========================================================================
// Summary:
//        DKX管理接口，用于给上层提供设置/读取数据操作
// Usage:
//        通过传入不同的书籍，以获得书籍的所有信息，或者设置书籍的相关信息
// Remarks:
//        NULL
// Date:
//        2011-12-09
// Author:
//        Xu Kai (xukai@duokan.com)
//===========================================================================
#ifndef __DKXMANAGER_H__
#define __DKXMANAGER_H__

#include "DKXManager/DKX/DKXBlock.h"
#include "Common/SystemSetting_DK.h"
#include "singleton.h"
#include <string>
#include <vector>

using std::vector;
using std::string;
class DKXManager
{
    SINGLETON_H(DKXManager)
    DKXManager()
        : m_strCurFilePath("")
        , m_pCurDkx(NULL)
    {
    }
    
public:
    static DKXBlock* CreateDKXBlock(const char* path);
    static void DestoryDKXBlock(DKXBlock* block);

    virtual ~DKXManager()
    {
        SafeDeletePointer(m_pCurDkx);
    };


    /*********************************************************************
    *                     Interface on BookInfo                          *
    *********************************************************************/


    //----------------------------------------------------
    // Summary:
    //        设置指定书籍的创建时间
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] createTime    指定目的书籍的创建时间
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetBookCreateTime(const string &bookPath,const string &createTime);



    //----------------------------------------------------
    // Summary:
    //        设置指定书籍的创建序列
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] addOrder    指定目的书籍的创建时间
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetBookAddOrder(const string &bookPath,long addOrder);

    //----------------------------------------------------
    // Summary:
    //        读取指定书籍的创建序列
    // Parameters:
    //        [in] bookPath    指定目的书籍
    // Return Value:
    //        成功返回指定书籍的创建时间,失败返回空串
    // Remarks:
    //        NULL
    //----------------------------------------------------
    long FetchBookAddOrder(const string &bookPath);

    //----------------------------------------------------
    // Summary:
    //        设置指定书籍的最后阅读时间
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] lastReadTime    指定目的书籍的最后阅读时间
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetBookLastReadTime(const string &bookPath,const string &lastReadTime);

    //----------------------------------------------------
    // Summary:
    //        读取指定书籍的最后阅读时间
    // Parameters:
    //        [in] bookPath    指定目的书籍
    // Return Value:
    //        成功返回指定书籍的最后阅读时间,失败返回空串
    // Remarks:
    //        NULL
    //----------------------------------------------------
    string FetchBookLastReadTime(const string &bookPath);
    
    //----------------------------------------------------
    // Summary:
    //        设置指定书籍的阅读次序
    // Parameters:
    //        [in] bookPath      指定目的书籍
    //        [in] readingOrder    指定目的书籍的阅读次序
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetBookReadingOrder(const string &bookPath, long readingOrder);

    //----------------------------------------------------
    // Summary:
    //        读取指定书籍的阅读次序
    // Parameters:
    //        [in] bookPath    指定目的书籍
    // Return Value:
    //        成功返回指定书籍的阅读次序,失败返回-1
    // Remarks:
    //        NULL
    //----------------------------------------------------
    long FetchBookReadingOrder(const string &bookPath);

    //----------------------------------------------------
    // Summary:
    //        读取指定书籍的文件类型
    // Parameters:
    //        [in] bookPath    指定目的书籍
    // Return Value:
    //        成功返回指定书籍的文件类型,失败返回空串
    // Remarks:
    //        NULL
    //----------------------------------------------------
    string FetchBookFileType(const string &bookPath);

    //----------------------------------------------------
    // Summary:
    //        设置指定书籍的文件类型
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] fileType    指定目的书籍的文件类型
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetBookFileType(const string &bookPath, const string &fileType);    

    //----------------------------------------------------
    // Summary:
    //        读取指定书籍的书名
    // Parameters:
    //        [in] bookPath    指定目的书籍
    // Return Value:
    //        成功返回指定书籍的书名,失败返回空串
    // Remarks:
    //        NULL
    //----------------------------------------------------
    string FetchBookName(const string &bookPath);

    //----------------------------------------------------
    // Summary:
    //        设置指定书籍的书名
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] bookName    指定目的书籍的书名
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetBookName(const string &bookPath, const string &bookName);
    
    //----------------------------------------------------
    // Summary:
    //        读取指定书籍的作者
    // Parameters:
    //        [in] bookPath    指定目的书籍
    // Return Value:
    //        成功返回指定书籍的作者,失败返回空串
    // Remarks:
    //        NULL
    //----------------------------------------------------
    string FetchBookAuthor(const string &bookPath);

    //----------------------------------------------------
    // Summary:
    //        设置指定书籍的作者
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] author        指定目的书籍的作者
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetBookAuthor(const string &bookPath, const string &author);

	
    //----------------------------------------------------
    // Summary:
    //        读取指定书籍的作者与书名是否与dkx文件同步
    // Parameters:
    //        [in] bookPath    指定目的书籍
    // Return Value:
    //        返回是否同步
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool FetchSync(const string &bookPath);

    //----------------------------------------------------
    // Summary:
    //        设置指定书籍的作者与书名是否与dkx文件同步
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] isSync        是否同步
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetSync(const string &bookPath, bool isSync);

	//----------------------------------------------------
    // Summary:
    //        读取指定书籍是否会死机
    // Parameters:
    //        [in] bookPath    指定目的书籍
    // Return Value:
    //        返回是否会死机
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool FetchCrash(const string &bookPath);

    //----------------------------------------------------
    // Summary:
    //        设置指定书籍是否会死机
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] isCrash        是否会死机
    // Return Value:
    //		死机返回true,否则返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetCrash(const string &bookPath, bool isCrash);

	//----------------------------------------------------
    // Summary:
    //        设置指定书籍的bookID
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] bookID        bookID
    // Remarks:
    //        NULL
    //----------------------------------------------------
	bool SetBookID(const string& bookPath, const char* bookID);

	//----------------------------------------------------
    // Summary:
    //        获取指定书籍的bookID
    // Parameters:
    //        [in] bookPath    指定目的书籍
    // Return Value:
    //		返回指定书籍的bookID
    // Remarks:
    //        NULL
    //----------------------------------------------------
	string FetchBookID(const string& bookPath);

    //----------------------------------------------------
    // Summary:
    //        读取指定书籍的来源
    // Parameters:
    //        [in] bookPath    指定目的书籍
    // Return Value:
    //        成功返回指定书籍的来源,失败返回空串
    // Remarks:
    //        NULL
    //----------------------------------------------------
    string FetchBookSource(const string &bookPath);

    //----------------------------------------------------
    // Summary:
    //        设置指定书籍的作者
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] source        指定目的书籍的来源
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetBookSource(const string &bookPath, const string &source);    

    
    //----------------------------------------------------
    // Summary:
    //        读取指定书籍的简介
    // Parameters:
    //        [in] bookPath    指定目的书籍
    // Return Value:
    //        成功返回指定书籍的简介,失败返回空串
    // Remarks:
    //        NULL
    //----------------------------------------------------
    string FetchBookAbstract(const string &bookPath);

    //----------------------------------------------------
    // Summary:
    //        设置指定书籍的简介
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] abstract    指定目的书籍的简介
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetBookAbstract(const string &bookPath, const string &abstract);


    //----------------------------------------------------
    // Summary:
    //        读取指定书籍的加密密码
    // Parameters:
    //        [in] bookPath    指定目的书籍
    // Return Value:
    //        成功返回指定书籍的加密密码,失败返回空串
    // Remarks:
    //        NULL
    //----------------------------------------------------
    string FetchBookPassword(const string &bookPath);

    //----------------------------------------------------
    // Summary:
    //        设置指定书籍的加密密码
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] password    指定目的书籍的加密密码
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetBookPassword(const string &bookPath, const string &password);

    //----------------------------------------------------
    // Summary:
    //        读取指定书籍的大小
    // Parameters:
    //        [in] bookPath    指定目的书籍
    // Return Value:
    //        成功返回指定书籍的大小,失败返回空串
    // Remarks:
    //        NULL
    //----------------------------------------------------
    string FetchBookSize(const string &bookPath);

    //----------------------------------------------------
    // Summary:
    //        设置指定书籍的大小
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] strSize    指定目的书籍的大小
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetBookSize(const string &bookPath, const string &strSize);


 
    //-------------------------------------
    //    Summary:
    //        获取TXT文件的目录信息
    //    Parameters:
    //        [in] bookPath    TXT文件的路径
    //    Return Value:
    //        返回当前TXT文件的目录信息
    //    Remarks:
    //        NULL
    //-------------------------------------
    std::vector<TxtTocItem> FetchTxtToc(const string &bookPath);

    //-------------------------------------
    //    Summary:
    //        设置TXT文件的目录信息
    //    Parameters:
    //        [in] bookPath    TXT文件的路径
    //        [in] txtToc        TXT文件的目录列表
    //    Return Value:
    //        成功返回true,失败返回false
    //    Remarks:
    //        NULL
    //-------------------------------------
    bool SetTxtToc(const string &bookPath,const std::vector<TxtTocItem> &txtToc);

	//----------------------------------------------------
	// Summary:
	//        判断目录是否已经提取
	// Parameters:
	//        [in] bookPath    指定目的书籍
	// Return Value:
	//        已经提取返回true,没有提取返回false
	// Remarks:
	//        NULL
	//---------------------------------------------------
	bool FetchTocExtractedState(const string &bookPath);

	//---------------------------------------------------
	// Summary:
	//        设置当前书籍目录是否已经提取
	// Parameters:
	//        [in] bookPath		指定目的书籍
	//        [in] hasExtracted   指定是否已经提取目录，是为true,不是为false
	// Return Value:
	//        设置成功返回true,失败返回false
	// Remarks:
	//        NULL
	//---------------------------------------------------
	bool SetTocExtractedState(const string &bookPath,bool hasExtracted);


    
    /*********************************************************************
    *                     Interface on ReadingData                       *
    *********************************************************************/
    int FetchBookMarkNumber(const string &bookPath);

    //----------------------------------------------------
    // Summary:
    //        添加一个书签
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] userDataItem        书签的基本数据（ID,创建时间，作者等）
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool AddBookmark(const string &bookPath, const ICT_ReadingDataItem *userDataItem);

	//----------------------------------------------------
    // Summary:
    //      获取所有书摘
    // Parameters:
    //		[in] bookPath    指定目的书籍
    //		[out]bookmarks  所有的书摘数据
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        _pvBookmarks的内存由调用者释放
    //----------------------------------------------------
	bool FetchAllBookmarks(const string &bookPath, vector<ICT_ReadingDataItem *> *bookmarks);

    //----------------------------------------------------
    // Summary:
    //        获得一个书签
    // Parameters:
    //        [in] bookPath            指定目的书籍
    //        [in] index                指定要获得哪一个书签
    //        [out] userDataItem        用于获取书签的数据（ID,创建时间，作者等）
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool FetchBookmarkByIndex(const string &bookPath, int index, ICT_ReadingDataItem *userDataItem);

    //----------------------------------------------------
    // Summary:
    //        删除一个书签
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] index        指定要获得哪一个书签
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool RemoveBookmarkByIndex(const string &bookPath, int index);



    //----------------------------------------------------
    // Summary:
    //        设置阅读进度
    // Parameters:
    //        [in] bookPath            指定目的书籍
    //        [in] userDataItem        阅读进度数据
    // Return Value:
    //        成功返回true,失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetProgress(const string &bookPath, const ICT_ReadingDataItem *userDataItem);
    
    //----------------------------------------------------
    // Summary:
    //        读取阅读进度
    //    Parameters:
    //        [in] bookPath        指定目的书籍
    //        [out]userDataItem    用于获取阅读进度的基本数据
    //    Return Value:
    //        成功返回true,失败返回false
    //    Remarks:
    //        出参必须在外面分配空间
    //-----------------------------------------------------
    bool FetchProgress(const string &bookPath,ICT_ReadingDataItem *userDataItem);

    /*********************************************************************
    *                     Interface on Setting Data                       *
    *********************************************************************/

    /**********************  PDF 专有参数    具体所需数据未定************************/

    //----------------------------------------------------
    // Summary:
    //        获取pdf设置中某个字段的值
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] strField        指定要获取的字段
    // Return Value:
    //        成功返回该字段的值，失败返回空串
    // Remarks:
    //        NULL
    //----------------------------------------------------
    string FetchPDFSettingFieldValue(
            const string &bookPath,
            const string& strField);

    //----------------------------------------------------
    // Summary:
    //        设置pdf某个字段的值
    // Parameters:
    //        [in] bookPath    指定目的书籍
    //        [in] strField        指定要设置的字段
    //        [in] fieldValue    指定要设置的值
    // Return Value:
    //        成功返回true，失败返回false
    // Remarks:
    //        NULL
    //----------------------------------------------------
    bool SetPDFSettingFieldValue(
            const string &bookPath,
            const string &strField,
            const string &fieldValue);    

    
    /**********************  非PDF 专有参数    ************************/

    bool SaveCurDkx();

    bool SetBookRevision(const string& bookPath, const char* bookRevision);
    std::string GetBookRevision(const string& bookPath);

    int GetDataVersion(const string& bookPath);
    bool SetDataVersion(const string& bookPath, int dataVersion);

    bool MergeCloudBookmarks(const string& bookPath,
            const ReadingBookInfo& readingBookInfo,
            int cloudDataVersion);
    void SetReadingDataModified(const string& bookPath, bool modified);
    bool IsReadingDataModified(const string& bookPath);
    void MarkReadingDataAsUploaded(const string& bookPath);
    void Reset();


private:
    bool InitCurDkx(string _strFilePath);
    bool TestIsThisDkx(string _strFilePath);
    
private:
    string m_strCurFilePath;
    DKXBlock *m_pCurDkx;
};

#endif    //__DKXMANAGER_H__

