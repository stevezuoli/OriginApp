//===========================================================================
// Summary:
//        BookTable 头文件定义
// Version:
//    0.1 draft - creating.
//    
// Usage:
//        Null
// Remarks:
//        Null
// Date:
//        2012-1-9
// Author:
//        juguanhui (juguanhui@duokan.com)
//===========================================================================

#ifndef __KINDLEAPP_INC_PAGETABLE_H__
#define __KINDLEAPP_INC_PAGETABLE_H__

#include <vector>
#include <string>
#include "DKXManager/BaseType/CT_RefPos.h"
#include "Utility/JsonObject.h"
#ifdef KINDLE_FOR_TOUCH
#include "Common/File_DK.h"
#else
#include "Common/File_DK.h"
#endif

struct PageTable
{
    int pageCount;
	vector<int> vPageOffset;
	int chapterCont;
    vector<int> vPageIndexOfChapter;
    vector<vector<CT_RefPos> > vEndPosOfChapter;
	string strMD5;
	PageTable()
	{
	    pageCount = 0;
		vPageOffset.clear();
		chapterCont = 0;
	    vPageIndexOfChapter.clear();
	    vEndPosOfChapter.clear();
	}
	void Clear()
	{
		vPageOffset.clear();
		vPageIndexOfChapter.clear();
		vEndPosOfChapter.clear();
		pageCount = 0;
		chapterCont = 0;
	}
	bool IsNull() const
	{
		return pageCount == 0;
	}
};

class CPageTable
{
public:
    CPageTable(const string& _strFilePath, const string& _strKernelVersion, int pageStep = 0, bool clearOnKernelMismatch = true);
    ~CPageTable();
    void Clear();
	
	PageTable GetPageTable(const string& strMD5);
	bool SavePageTable(const PageTable& _pageTable);
	PageTable GetMatchedOrFirstPageTable(const std::string& md5);
private:
	bool GetPageOffSet(const JsonObjectSPtr pPageTable, vector<int> * pPageOffset) const;
	bool GetPageIndexInChapter(const JsonObjectSPtr pPageTable, vector<int> * pPageIndex) const;
	bool GetEndPosOfChapter(const JsonObjectSPtr pPageTable, vector<vector<CT_RefPos> >* pPageIndex) const;
	void GetJsonTable();
	JsonObjectSPtr GetJsonPageTable(const string& strMD5) const;

	//添加三级索引
	void SetEndPosOfChapter(JsonObjectSPtr pPageTable,const vector<vector<CT_RefPos> >& vPageIndex) const;
    bool GetPageTableFromJsonPageTable(JsonObjectSPtr jsonPageTable, PageTable* pageTable);
private:
    string m_filePath;
	string m_kernelVersion;
	int m_iPageStep;
	JsonObjectSPtr m_jsonPageTables;
    bool m_clearOnKernelMismatch;
};

#endif // __KINDLEAPP_INC_PAGETABLE_H__

