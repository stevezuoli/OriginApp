#include "BookReader/PageTable.h"
#include "DkStreamFactory.h"
#include "KernelMacro.h"
#include "interface.h"
#include <string.h>
#include <stdio.h>
#include "KernelVersion.h"
#include "Utility/SystemUtil.h"
#include "Common/FileManager_DK.h"
#include "Utility/PathManager.h"

using std::vector;
using std::string;
using namespace dk::utility;

#define PAGETABLE_PAGE_STEP			"pageStep"
#define PAGETABLE_PAGE_COUNT		"pageCount"
#define PAGETABLE_PAGE_OFFSET		"PageOffset"
#define PAGETABLE_CHAPTER_COUNT		"chapterCount"
#define PAGETABLE_CHAPTER_PAGE_INDEX "chapterPageIndex"
#define PAGETABLE_CHAPTER_PAGE_COUNT "chapterPageCount"
#define PAGETABLE_CHAPTER_POS		"chapterPos"
#define PAGETABLE_CHAPTER 			"chapter"
#define PAGETABLE_KERNEL_VERSION	"KernelVersion"
#define PAGETABLE_FILE_VERSION		"fileVersion"
#define PAGETABLE_MD5				"MD5"
#define PAGETABLE_PAGE_SETTING		"pageSetting"
#define PAGETABLE_PAGETABLE			"pageTable"
#define PAGETABLE_PAGETABLES		"pageTables"

#define FILE_VERSION "1.0.0"

CPageTable::CPageTable(const string& filePath, const string& kernelVersion, int pageStep, bool clearOnKernelMismatch)
	:m_jsonPageTables(JsonObjectSPtr())
{
	m_filePath = PathManager::BookFileToDkptFile(filePath);
	m_kernelVersion = kernelVersion;
	m_iPageStep = pageStep;
    m_clearOnKernelMismatch = clearOnKernelMismatch;
	GetJsonTable();
}

CPageTable::~CPageTable()
{
}

bool CPageTable::GetPageOffSet(const JsonObjectSPtr pPageTable, vector<int> * pPageOffset) const
{
	if(!pPageTable || !pPageOffset)
	{
		return false;
	}
	
	JsonObjectSPtr jsonPagesOffset = pPageTable->GetSubObject(PAGETABLE_PAGE_OFFSET);
	if(!jsonPagesOffset)
	{
		return false;
	}

	pPageOffset->clear();
	int pageCount = jsonPagesOffset->GetArrayLength();
	for(int i = 0; i < pageCount; i++)
	{
		JsonObjectSPtr jsonPageOffset = jsonPagesOffset->GetElementByIndex(i);
		int iPageOffSet = 0;
		jsonPageOffset->GetIntValue(&iPageOffSet);
		pPageOffset->push_back(iPageOffSet);
	}
	return true;
}

bool CPageTable::GetPageIndexInChapter(const JsonObjectSPtr pPageTable, vector<int> * pPageIndex) const
{
	if(!pPageTable || !pPageIndex)
	{
		return false;
	}
	
	JsonObjectSPtr jsonPagesIndex = pPageTable->GetSubObject(PAGETABLE_CHAPTER_PAGE_INDEX);
	if(!jsonPagesIndex)
	{
		return false;
	}

	pPageIndex->clear();
	int chapterCount = jsonPagesIndex->GetArrayLength();
	for(int i = 0; i < chapterCount; i++)
	{
		JsonObjectSPtr jsonPageIndex = jsonPagesIndex->GetElementByIndex(i);
		int iPageOffSet = 0;
		jsonPageIndex->GetIntValue(&iPageOffSet);
		pPageIndex->push_back(iPageOffSet);
	}
	return true;
}

bool CPageTable::GetEndPosOfChapter(const JsonObjectSPtr pPageTable, vector<vector<CT_RefPos> >* pEndPos) const
{
	if(!pPageTable || !pEndPos)
	{
		return false;
	}
	
	JsonObjectSPtr jsonPosChapter =  pPageTable->GetSubObject(PAGETABLE_CHAPTER_POS);
	if(!jsonPosChapter)
	{
		return false;
	}

	pEndPos->clear();
	int chapterCount = jsonPosChapter->GetArrayLength();
	for(int i = 0; i < chapterCount; i++)
	{
		JsonObjectSPtr jsonPos = jsonPosChapter->GetElementByIndex(i);
		int pageCountInChapter = jsonPos->GetArrayLength();
		vector<CT_RefPos> vPos;
		for(int j = 0; j < pageCountInChapter; j++)
		{
			JsonObjectSPtr jsonPosPage = jsonPos->GetElementByIndex(j);
			int chapterIndex = 0;      //注意此处的顺序，要与写入处保持一致
			int paraIndex = 0;
			int atomIndex = 0;
			jsonPosPage->GetElementByIndex(0)->GetIntValue(&chapterIndex);
			jsonPosPage->GetElementByIndex(1)->GetIntValue(&paraIndex);
			jsonPosPage->GetElementByIndex(2)->GetIntValue(&atomIndex);
			vPos.push_back(CT_RefPos(chapterIndex, paraIndex, atomIndex, 0));
		}
		pEndPos->push_back(vPos);
	}
	return true;
}

bool CPageTable::GetPageTableFromJsonPageTable(
        JsonObjectSPtr jsonPageTable,
        PageTable* pageTable)
{
    if (!jsonPageTable)
    {
        return false;
    }
	jsonPageTable->GetStringValue(PAGETABLE_MD5 , &pageTable->strMD5);
	jsonPageTable->GetIntValue(PAGETABLE_PAGE_COUNT, &pageTable->pageCount);
	jsonPageTable->GetIntValue(PAGETABLE_CHAPTER_COUNT, &pageTable->chapterCont);

	GetPageOffSet(jsonPageTable, &pageTable->vPageOffset);
	GetPageIndexInChapter(jsonPageTable, &pageTable->vPageIndexOfChapter);
	GetEndPosOfChapter(jsonPageTable, &pageTable->vEndPosOfChapter);
    return true;
}

PageTable CPageTable::GetPageTable(const string& strMD5)
{
	PageTable pageTable;
	JsonObjectSPtr jsonPageTable = GetJsonPageTable(strMD5);
	if(!jsonPageTable)
	{
		return pageTable;
	}
	
    GetPageTableFromJsonPageTable(jsonPageTable, &pageTable);
	return pageTable;
}

void CPageTable::SetEndPosOfChapter(JsonObjectSPtr pPageTable, const vector<vector<CT_RefPos> >& vPageIndex) const
{
	JsonObjectSPtr jsonChapterPos = JsonObject::CreateJsonArrayObject();
	for(int i=0; i < (int)vPageIndex.size(); i++)
	{
		JsonObjectSPtr pagePosInChapter = JsonObject::CreateJsonArrayObject();
		for(int j = 0; j < (int)vPageIndex[i].size(); j++)
		{
			const CT_RefPos& endPos = vPageIndex[i][j];
			int chapter = endPos.GetChapterIndex();
			int para = endPos.GetParaIndex();
			int atom = endPos.GetAtomIndex();
			JsonObjectSPtr jsonPos = JsonObject::CreateJsonArrayObject();
			JsonObjectSPtr jsonSubChapter = JsonObject::CreateJsonIntObject(chapter);
			JsonObjectSPtr jsonSubPara = JsonObject::CreateJsonIntObject(para);
			JsonObjectSPtr jsonSubAtom = JsonObject::CreateJsonIntObject(atom);
			
			jsonPos->AddJsonObjectArray(jsonSubChapter);
			jsonPos->AddJsonObjectArray(jsonSubPara);
			jsonPos->AddJsonObjectArray(jsonSubAtom);
			pagePosInChapter->AddJsonObjectArray(jsonPos);
		}
		jsonChapterPos->AddJsonObjectArray(pagePosInChapter);
	}
	pPageTable->AddJsonObject(PAGETABLE_CHAPTER_POS, jsonChapterPos);
}

bool CPageTable::SavePageTable(const PageTable& _pageTable)
{
	if(!m_jsonPageTables)
	{
		return false;
	}
	

    CDKFileManager* fileManager = CDKFileManager::GetFileManager();
    PCDKFile file = fileManager->GetFileByPath(m_filePath);
    if (NULL != file)
    {
        file->SetPageCount(_pageTable.pageCount);
    }
	JsonObjectSPtr jsonPageTable = GetJsonPageTable(_pageTable.strMD5);
	if(jsonPageTable)
 	{
 		return true;
 	}
	jsonPageTable  = JsonObject::CreateJsonObject();

	JsonObjectSPtr jsonPageTableArray = m_jsonPageTables->GetSubObject(PAGETABLE_PAGETABLES);
	if(!jsonPageTableArray)
	{
		jsonPageTableArray = JsonObject::CreateJsonArrayObject();
	}
	
	jsonPageTable->AddStringValue(PAGETABLE_MD5, _pageTable.strMD5.c_str());
	jsonPageTable->AddIntValue(PAGETABLE_PAGE_COUNT, _pageTable.pageCount);
	jsonPageTable->AddIntValue(PAGETABLE_CHAPTER_COUNT, _pageTable.chapterCont);
	jsonPageTable->AddVector(PAGETABLE_PAGE_OFFSET, _pageTable.vPageOffset);
	jsonPageTable->AddVector(PAGETABLE_CHAPTER_PAGE_INDEX, _pageTable.vPageIndexOfChapter);
	
	SetEndPosOfChapter(jsonPageTable, _pageTable.vEndPosOfChapter);
	jsonPageTableArray->AddJsonObjectArray(jsonPageTable);										//添加新增的分页表

	m_jsonPageTables->AddStringValue(PAGETABLE_KERNEL_VERSION, m_kernelVersion.c_str());
	m_jsonPageTables->AddStringValue(PAGETABLE_FILE_VERSION, FILE_VERSION);
	m_jsonPageTables->AddIntValue(PAGETABLE_PAGE_STEP, m_iPageStep);
	m_jsonPageTables->AddJsonObject(PAGETABLE_PAGETABLES, jsonPageTableArray);

	string strJson = m_jsonPageTables->GetJsonString();
	FILE* fp = fopen(m_filePath.c_str(),"wb+");
	fwrite(strJson.c_str(), strJson.length(), 1, fp);
	fclose(fp);
	return true;
}

JsonObjectSPtr CPageTable::GetJsonPageTable(const string& strMD5) const
{
	if(!m_jsonPageTables)
	{
		return JsonObjectSPtr();
	}

	JsonObjectSPtr jsonArray = m_jsonPageTables->GetSubObject(PAGETABLE_PAGETABLES);
	if(!jsonArray)
	{
		return JsonObjectSPtr();
	}
	
	int tableCount = jsonArray->GetArrayLength();
	for(int i = 0; i < tableCount; i++)
	{
		string strLocalMD5 = "";
		JsonObjectSPtr jsonPageTable = jsonArray->GetElementByIndex(i);
		if(!jsonPageTable)
		{
			continue; 
		}
		
		jsonPageTable->GetStringValue(PAGETABLE_MD5, &strLocalMD5);
		if(strMD5 == strLocalMD5)
		{
			return jsonPageTable;
		}
	}
	return JsonObjectSPtr();
}

void CPageTable::GetJsonTable()
{
	string strJson = SystemUtil::ReadStringFromFile(m_filePath.c_str(),false);
    //DebugPrintf(DLC_DIAGNOSTIC, "Enter CPageTable::GetJsonTable");
	if(strJson.empty())
	{
		m_jsonPageTables = JsonObject::CreateJsonObject();
		return;
	}

    // DebugPrintf(DLC_DIAGNOSTIC, "before CPageTable::GetJsonTable CreateFromString");
	m_jsonPageTables = JsonObject::CreateFromString(strJson.c_str());
	if(!m_jsonPageTables)
	{
		m_jsonPageTables = JsonObject::CreateJsonObject();
		return;
	}
    // DebugPrintf(DLC_DIAGNOSTIC, "after CPageTable::GetJsonTable CreateFromString");

	//如果内核版本号升级的话清空以前的数据
	string kernelVersion = "";
	string fileVersion  = "";
	int pageStep = -1;
	m_jsonPageTables->GetStringValue(PAGETABLE_KERNEL_VERSION, &kernelVersion);
	m_jsonPageTables->GetStringValue(PAGETABLE_FILE_VERSION, &fileVersion);
	m_jsonPageTables->GetIntValue(PAGETABLE_PAGE_STEP, &pageStep);
	if (m_clearOnKernelMismatch && 
            (kernelVersion != m_kernelVersion || 
             fileVersion != FILE_VERSION || 
             pageStep != m_iPageStep))
	{
        m_jsonPageTables = JsonObject::CreateJsonObject();
        DebugPrintf(DLC_DIAGNOSTIC, "Leave CPageTable::GetJsonTable mismatch");
		return;
	}
    //DebugPrintf(DLC_DIAGNOSTIC, "Leave CPageTable::GetJsonTable ");
	return;
}

PageTable CPageTable::GetMatchedOrFirstPageTable(const std::string& md5)
{
    PageTable pageTable = GetPageTable(md5);
    if (!pageTable.IsNull())
    {
        return pageTable;
    }
	if(!m_jsonPageTables)
	{
		return pageTable;
	}

	JsonObjectSPtr jsonArray = m_jsonPageTables->GetSubObject(PAGETABLE_PAGETABLES);
	if(!jsonArray)
	{
		return pageTable;
	}
	
	int tableCount = jsonArray->GetArrayLength();
	for(int i = 0; i < tableCount; i++)
	{
		string strLocalMD5 = "";
		JsonObjectSPtr jsonPageTable = jsonArray->GetElementByIndex(i);
		if(!jsonPageTable)
		{
			continue; 
		}
        GetPageTableFromJsonPageTable(jsonPageTable, &pageTable);
        break;
	}
    return pageTable;
}

void CPageTable::Clear()
{
    m_jsonPageTables = JsonObject::CreateJsonObject();
}
