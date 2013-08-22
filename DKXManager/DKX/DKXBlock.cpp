#include "DKXManager/DKX/DKXBlock.h"
#include "Common/CAccountManager.h"
//#include "Utility/FileSystem.h"
#include    <iostream>
#include "Utility/StringUtil.h"
#include "BookStore/ReadingBookInfo.h"
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include "Utility/XmlUtil.h"
#include "Utility/PathManager.h"
#include "Common/File_DK.h"
#include <sys/stat.h>
#include <fstream>
#include <iostream>

using namespace dk::utility;

using namespace std;
using namespace dk::bookstore::sync;
using namespace dk::account;
using namespace dk::utility;

#define DKXVERSIONS	"DKX-1.0"

const char* DKXBlock::FILEITEM = "FileItem";
const char* DKXBlock::FILEPATH = "FilePath";
const char* DKXBlock::SUBFILERELATIVEPATH = "SubFileRelativePath";
const char* DKXBlock::HEADER = "Header";
const char* DKXBlock::VERSION = "Version";
const char* DKXBlock::BOOKINFO = "BookInfo";
const char* DKXBlock::READINGDATA = "ReadingData";
const char* DKXBlock::SETTINGDATA = "SettingData";
const char* DKXBlock::USERNAME = "UserName";

DKXBlock::DKXBlock(const string _strFilePath)
    : m_bookInfo()
    , m_SettingData()
    , m_strFilePath(_strFilePath)
	, m_strVersion(DKXVERSIONS)
{
    UnSerialize(_strFilePath);
}

DKXBlock::~DKXBlock()
{
}


DK_BookInfo* DKXBlock::GetBookInfo()
{
    return &m_bookInfo;
}


CT_ReadingData* DKXBlock::GetReadingData()
{
    //return &m_ReadingData;
    DK_AUTO(pos, m_readingDatas.find(GetCurrentUserName()));
    if (pos == m_readingDatas.end())
    {
        m_readingDatas[GetCurrentUserName()] = CT_ReadingData();
        pos = m_readingDatas.find(GetCurrentUserName());
    }
    return &pos->second;
}

DK_BookSettingInfo* DKXBlock::GetSettingInfo()
{
    return &m_SettingData;
}


bool DKXBlock::RemoveCurFileNodeFromRootNode(XMLDomNode *_pRootNode, const string &_strFilePath)
{
    DK_INT iCnt = 0;
    _pRootNode->GetChildrenCount(&iCnt);
    for(DK_INT i = 0;i < iCnt;i++)
    {
        XMLDomNode tmpNode;
        if(DKR_OK != _pRootNode->GetChildByIndex(i,&tmpNode))
        {
            continue;
        }
        if(strcmp(tmpNode.GetNodeValue(),FILEITEM) == 0)
        {
			// 第一个版本中没有子文件路径，且不支持压缩包，所以如果读不到该属性则说明该ITEM是当前书的ITEM
			const char *pAttribute = tmpNode.GetAttribute(SUBFILERELATIVEPATH);
            if(pAttribute == DK_NULL || strcmp(pAttribute,_strFilePath.c_str()) == 0)
            {
                _pRootNode->RemoveChildNode(tmpNode);
                return true;
            }
        }
    }
    return false;
}

XMLDomNode *DKXBlock::LoadXmlAndGetRootElemnet(XMLDomDocument *_pDocument,const string &_strDkxPath)
{
    if(_strDkxPath.empty() || access(_strDkxPath.c_str(), F_OK))//不存在
    {
        return NULL;
    }
    XMLDomNode *pRootNode = NULL;
    if(!_pDocument->LoadXmlFromFile(_strDkxPath.c_str(),false))
	{
		return NULL;
	}
    pRootNode = _pDocument->RootElement();
    return pRootNode;
}

bool DKXBlock::Serialize(const string &_strFilePath)
{
    string strDkxPath = PathManager::BookFileToDkxFile(_strFilePath);
    if(strDkxPath.empty())
    {
        return false;
    }
	// TODO: 如果支持压缩包，该值应该从外面传入，或从_strFilePath解析获得
	string strSubFilePath = "/";

    XMLDomDocument *pDocument = XMLDomDocument::CreateDocument();
    if(NULL == pDocument)
    {
        return false;
    }

    XMLDomNode TmpNode;
    DkFileFormat format = GetFileFormatbyExtName(strDkxPath.c_str());
    XMLDomNode *pRootNode = NULL;
    if (format == DFF_ZipFile || format == DFF_RoshalArchive)
    {
        pRootNode = LoadXmlAndGetRootElemnet(pDocument, strDkxPath);
    }
    if(NULL != pRootNode)
    {
        RemoveCurFileNodeFromRootNode(pRootNode,strSubFilePath);
    }
    else
    {
        XMLDomNode *pDomNode = pDocument->ToDomNode();
        XMLDomNode RootNode;
        pDomNode->AddRootElement("DKXROOT");
        pDomNode->GetLastChild(&RootNode);
        pRootNode = &RootNode;
    }

    AddCommentNodeToXML(FILEITEM,*pRootNode);
    DK_ReturnCode rc = pRootNode->GetLastChild(&TmpNode);
    if(rc == DKR_OK)
    {
		// 添加子文件的相对路径属性
		TmpNode.AddAttribute(SUBFILERELATIVEPATH, strSubFilePath.c_str());

		// 添加文件的路径
        TmpNode.AddAttribute(FILEPATH, _strFilePath.c_str());
        WriteToXML(TmpNode);

        pDocument->SaveXmlToFile(strDkxPath.c_str());
    }
    XMLDomDocument::ReleaseDocument(pDocument);
    return true;

}

void DKXBlock::DeleteWrongData(const std::string& dkxFilePath)
{
	struct stat fileInfo;
    stat(dkxFilePath.c_str(), &fileInfo);
	size_t size = fileInfo.st_size;
	if(size > 2*1024*1024)
	{
		ifstream inputFile(dkxFilePath.c_str());
		string fileData((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
		string strEnd("</TocExtracted>");
		size_t endPosition = fileData.rfind(strEnd.c_str());
		if(endPosition != std::string::npos && endPosition + strEnd.size() < size)
		{
			string latterPart = fileData.substr(endPosition + strEnd.size());
			size_t startPosition = fileData.find("<TxtTocs>");
			if(startPosition != std::string::npos)
			{
				fileData = fileData.substr(0, startPosition) + latterPart;
				ofstream outFile(dkxFilePath.c_str());
				outFile << fileData;
				outFile.close();
			}
		}
	}
}

bool DKXBlock::UnSerialize(const string &_strFilePath)
{
    string strDkxPath = PathManager::BookFileToDkxFile(_strFilePath);
    if(_strFilePath.empty() || strDkxPath.empty())
    {
        return false;
    }

	// TODO: 如果支持压缩包，该值应该从外面传入，或从_strFilePath解析获得
	string strSubFilePath = "/";
	DeleteWrongData(strDkxPath);
    XMLDomDocument *pDocument = XMLDomDocument::CreateDocument();
    if(NULL == pDocument)
    {
        return false;
    }
    XMLDomNode *pRootNode = NULL;
    if(!pDocument->LoadXmlFromFile(strDkxPath.c_str(), false))
	{
		XMLDomDocument::ReleaseDocument(pDocument);
		return false;
	}
    pRootNode = pDocument->RootElement();
    if(NULL == pRootNode)
    {
        XMLDomDocument::ReleaseDocument(pDocument);
        return false;
    }
    DK_INT iCnt = 0;
    pRootNode->GetChildrenCount(&iCnt);
    for(DK_INT i = 0;i < iCnt;i++)
    {
        XMLDomNode TmpNode;
        pRootNode->GetChildByIndex(i,&TmpNode);
        if(strcmp(TmpNode.GetNodeValue(),FILEITEM) == 0)
        {
			// 第一个版本中没有子文件路径，且不支持压缩包，所以如果读不到该属性则说明该ITEM是当前书的ITEM
            const char *pAttribute = TmpNode.GetAttribute(SUBFILERELATIVEPATH);
            if(pAttribute == DK_NULL || strcmp(pAttribute, strSubFilePath.c_str()) == 0)
            {
                ReadFromXml(TmpNode);
                break;
            }
        }
    }
    XMLDomDocument::ReleaseDocument(pDocument);
    return true;
}

bool DKXBlock::WriteToXML(XMLDomNode& node) const
{
    std::string dkxVersion(DKXVERSIONS);
    BEGIN_XML_STORE_DATA(storeData)
        {VERSION, XNDT_STRING, &dkxVersion, false},
        {BOOKINFO, XNDT_COMPLEX, &m_bookInfo, false, DK_BookInfo::WriteToXMLFunc},
    END_XML_STORE_DATA;
    XmlUtil::BuildXmlNode(&node, storeData);
    // Add ReadingData
    DK_ReturnCode rc;
    XMLDomNode child;
    for (DK_AUTO(pos, m_readingDatas.begin());
            pos != m_readingDatas.end();
            ++pos)
    {
        node.AddElement(READINGDATA);
        rc = node.GetLastChild(&child);
        if(rc == DKR_OK)
        {
            pos->second.WriteToXML(child);
        }
    }

    node.AddElement(SETTINGDATA);
    rc = node.GetLastChild(&child);
    if(rc == DKR_OK)
    {
        m_SettingData.WriteToXML(child);
    }

    return true;
}

bool DKXBlock::ReadFromXml(const XMLDomNode &_xmlNode)
{
    XMLDomNode TmpNode;
    if(_xmlNode.GetFirstElementChild(&TmpNode) == DKR_OK)
    {
        do 
        {
            if(strcmp(TmpNode.GetNodeValue(),BOOKINFO) == 0)
            {
                m_bookInfo.ReadFromXml(TmpNode);
            }
			else if(strcmp(TmpNode.GetNodeValue(),VERSION) == 0)
			{
				ReadAtomNodeValue(TmpNode, m_strVersion);
				if(m_strVersion.empty())
				{
					// 第一次发布的没有版本号，则将其同步到第一个版本号
					m_strVersion = "DKX-1.0";
				}
			}
            else if(strcmp(TmpNode.GetNodeValue(),READINGDATA) == 0)
            {
                CT_ReadingData readingData;
                readingData.ReadFromXml(TmpNode);
                m_readingDatas[readingData.GetUser()] = readingData;
            }
            else if(strcmp(TmpNode.GetNodeValue(),SETTINGDATA) == 0)
            {
                m_SettingData.ReadFromXml(TmpNode);
            }
        } while (TmpNode.GetNextSibling(&TmpNode) == DKR_OK);
    }
    string currentUser = GetCurrentUserName();
    if (!currentUser.empty() && m_readingDatas.find(currentUser) == m_readingDatas.end())
    {
        DK_AUTO(pos, m_readingDatas.find(""));
        if (pos != m_readingDatas.end())
        {
            CT_ReadingData readingData = pos->second;
            readingData.SetUser(currentUser.c_str());
            m_readingDatas.erase(pos);
            m_readingDatas[currentUser] = readingData;
        }
    }
    std::vector<std::string> duokanAccount;
    if(CAccountManager::GetInstance()->GetDuokanByXiaomiAccount(currentUser.c_str(), &duokanAccount))
    {
        bool isFileChanged = false;
        for(std::vector<std::string>::iterator itr = duokanAccount.begin(); itr != duokanAccount.end(); itr++)
        {
            if(m_readingDatas.find(*itr) != m_readingDatas.end())
            {
                isFileChanged = true;
                m_readingDatas[currentUser].SetUser(currentUser.c_str());
                m_readingDatas[currentUser].MigrateReadingData(m_readingDatas[*itr]);
                m_readingDatas.erase(*itr);
                m_readingDatas[currentUser].MarkAsUploaded(false);
                m_readingDatas[currentUser].SetModified(true);
            }
        }
        isFileChanged && Serialize();
    }
    return true;
}

std::string DKXBlock::GetCurrentUserName()
{
    return StringUtil::ToLower(CAccountManager::GetInstance()->GetEmailFromFile().c_str());
}

// 前提：ReadingBookInfo成功转化到本地的书籍revision上
bool DKXBlock::MergeCloudBookmarks(const ReadingBookInfo& readingBookInfo, int cloudDataVersion)
{
    CT_ReadingData* readingData = GetReadingData();
    if (NULL == readingData)
    {
        return false;
    }
    size_t cloudItemCount = readingBookInfo.GetReadingDataItemCount();
    readingData->RemoveUploadedBookmarks();
    readingData->SetDataVersion(cloudDataVersion);
    for (size_t i = 0; i < cloudItemCount; ++i)
    {
        const ReadingDataItem& cloudItem = readingBookInfo.GetReadingDataItem(i);
        ICT_ReadingDataItem* localItem = CreateICTReadingDataItemFromReadingDataItem(cloudItem);
        if (NULL != localItem)
        {
            readingData->RemoveBookmark(localItem->GetId());
            readingData->AddBookmark(*(CT_ReadingDataItemImpl*)localItem);
            DKXReadingDataItemFactory::DestoryReadingDataItem(localItem);
        }
    }
    return true;
}



#define IMPL_SETFUNC(funcName,  type) \
    bool DKXBlock::Set##funcName(type v) \
{\
    return GetBookInfo()->Set##funcName(v);\
}
#define IMPL_GETFUNC(funcName, type) \
type DKXBlock::Get##funcName()\
{\
    return GetBookInfo()->Get##funcName();\
}

#define IMPL_FUNC(funcName, typeset, typeget) \
    IMPL_SETFUNC(funcName, typeset) \
    IMPL_GETFUNC(funcName, typeget)





IMPL_FUNC(AddOrder, long, long)
IMPL_FUNC(CreateTime, const std::string&, std::string)
IMPL_FUNC(LastReadTime, const std::string&, std::string)
IMPL_FUNC(ReadingOrder, long, long)
IMPL_FUNC(FileType, const std::string&, std::string)
IMPL_FUNC(BookName, const std::string&, std::string)
IMPL_FUNC(BookAuthor, const std::string&, std::string)
IMPL_FUNC(BookID, const char*, std::string)
IMPL_FUNC(BookSource, const std::string&, std::string)
IMPL_FUNC(BookAbstract, const std::string&, std::string)
IMPL_FUNC(BookPassword, const std::string&, std::string)
IMPL_FUNC(BookRevision, const char*, std::string)
IMPL_FUNC(FileSize, const std::string&, std::string)
IMPL_FUNC(Sync, bool, bool)
IMPL_FUNC(Crash, bool, bool)
IMPL_FUNC(TocExtractedState, bool, bool)
IMPL_FUNC(TxtToc, const std::vector<TxtTocItem>&, std::vector<TxtTocItem>)

int DKXBlock::GetDataVersion()
{
    CT_ReadingData* readingData = GetReadingData();
    if (NULL == readingData)
    {
        return false;
    }
    return readingData->GetDataVersion();
}

bool DKXBlock::SetDataVersion(int dataVersion)
{
    CT_ReadingData* readingData = GetReadingData();
    if (NULL == readingData)
    {
        return false;
    }
    readingData->SetDataVersion(dataVersion);
    return true;
}

bool DKXBlock::SetReadingDataModified(bool modified)
{
    CT_ReadingData* readingData = GetReadingData();
    if (NULL == readingData)
    {
        return false;
    }
    readingData->SetModified(modified);
    return true;
}
bool DKXBlock::IsReadingDataModified()
{
    CT_ReadingData* readingData = GetReadingData();
    if (NULL == readingData)
    {
        return false;
    }
    return readingData->IsModified();
}
bool DKXBlock::MarkReadingDataAsUploaded()
{
    CT_ReadingData* readingData = GetReadingData();
    if (NULL == readingData)
    {
        return false;
    }
    readingData->MarkAsUploaded();
    return true;
}

int DKXBlock::GetBookmarkCount()
{
    CT_ReadingData* readingData = GetReadingData();
    if (NULL == readingData)
    {
        return 0;
    }
    return readingData->GetBookmarkCount();
}

bool DKXBlock::GetBookmarkByIndex(
        int index,
        ICT_ReadingDataItem* readingDataItem)
{
    CT_ReadingData* readingData = GetReadingData();
    if (NULL == readingData)
    {
        return 0;
    }

    return readingData->GetBookmarkByIndex(
            index,
            (CT_ReadingDataItemImpl*)readingDataItem);
}

bool DKXBlock::AddBookmark(const ICT_ReadingDataItem* readingDataItem)
{
    if (NULL == readingDataItem)
    {
        return false;
    }
    CT_ReadingData* readingData = GetReadingData();
    if (NULL == readingData)
    {
        return false;
    }
    return readingData->AddBookmark(*(CT_ReadingDataItemImpl*)readingDataItem);
}

bool DKXBlock::RemoveBookmarkByIndex(int index)
{
    CT_ReadingData* readingData = GetReadingData();
    if (NULL == readingData)
    {
        return false;
    }
    return readingData->RemoveBookmark(index);
}

bool DKXBlock::GetAllBookmarks(std::vector<ICT_ReadingDataItem*>* bookmarks)
{
    CT_ReadingData* readingData = GetReadingData();
    if (NULL == readingData || NULL == bookmarks)
    {
        return false;
    }
	int count = readingData->GetBookmarkCount();
	for( int i = 0; i < count; i++)
	{
		ICT_ReadingDataItem *readingDataItem = DKXReadingDataItemFactory::CreateReadingDataItem();
		if(!readingDataItem)
		{
			continue;
		}

		if(!readingData->GetBookmarkByIndex(i, reinterpret_cast<CT_ReadingDataItemImpl *>(readingDataItem)))
		{
			DKXReadingDataItemFactory::DestoryReadingDataItem(readingDataItem);
			continue;
		}
		else
		{
			bookmarks->push_back(readingDataItem);
		}
	}
	return true;

}

bool DKXBlock::SetProgress(const ICT_ReadingDataItem* readingDataItem)
{
    if (NULL == readingDataItem)
    {
        return false;
    }
    CT_ReadingData* readingData = GetReadingData();
    if (NULL == readingData)
    {
        return false;
    }
    return readingData->SetProgress(*(CT_ReadingDataItemImpl*)readingDataItem);
}


bool DKXBlock::GetProgress(ICT_ReadingDataItem* readingDataItem)
{
    if (NULL == readingDataItem)
    {
        return false;
    }
    CT_ReadingData* readingData = GetReadingData();
    if (NULL == readingData)
    {
        return false;
    }
    return readingData->GetProgress((CT_ReadingDataItemImpl*)readingDataItem);
}

string DKXBlock::GetPDFSettingString(const string& name)
{
    return GetSettingInfo()->GetPdfSettingField(name);
}

bool DKXBlock::SetPDFSettingString(const string& name, const string& value)    
{
    return GetSettingInfo()->SetPdfSettingField(name, value);
}

bool DKXBlock::SetPDFSettingInt(const std::string& name, int value)
{
    char buf[20];
    snprintf(buf, DK_DIM(buf), "%d", value);
    return SetPDFSettingString(name, buf);
}

bool DKXBlock::GetPDFSettingInt(const std::string& name, int* value)
{
    std::string v = GetPDFSettingString(name);
    if (!v.empty())
    {
        *value = atoi(v.c_str());
        return true;
    }
    else
    {
        return false;
    }
}

bool DKXBlock::SetPDFSettingUInt(const std::string& name, unsigned int value)
{
    char buf[20];
    snprintf(buf, DK_DIM(buf), "%u", value);
    return SetPDFSettingString(name, buf);
}

bool DKXBlock::GetPDFSettingUInt(const std::string& name, unsigned int* value)
{
    std::string v = GetPDFSettingString(name);
    if (!v.empty())
    {
        *value = (unsigned int)atoi(v.c_str());
        return true;
    }
    else
    {
        return false;
    }
}

bool DKXBlock::SetPDFSettingDouble(const std::string& name, double value)
{
    char buf[20];
    snprintf(buf, DK_DIM(buf), "%f", value);
    return SetPDFSettingString(name, buf);
}

bool DKXBlock::GetPDFSettingDouble(const std::string& name, double* value)
{
    std::string v = GetPDFSettingString(name);
    if (!v.empty())
    {
        *value = atof(v.c_str());
        return true;
    }
    else
    {
        return false;
    }
}

bool DKXBlock::GetPDFModeController(PdfModeController* modeController)
{
    if (NULL == modeController)
    {
        return false;
    }
    GetPDFSettingInt("PDF_iRotation", &modeController->m_iRotation);
    GetPDFSettingInt("PDF_eTurnPageMode", (int*)&modeController->m_eTurnPageMode);
    GetPDFSettingUInt("PDF_uCurSubPage", &modeController->m_uCurSubPage);
    GetPDFSettingUInt("PDF_uCurPageStartPos", &modeController->m_uCurPageStartPos);
    GetPDFSettingInt("PDF_eReadingMode", (int*)&modeController->m_eReadingMode);
    GetPDFSettingUInt("PDF_uSubPageOrder", &modeController->m_uSubPageOrder);
    GetPDFSettingDouble("PDF_dWidth", &modeController->m_dWidth);
    GetPDFSettingDouble("PDF_dHeight", &modeController->m_dHeight);
    GetPDFSettingInt("PDF_eCuttingEdgeMode", (int*)&modeController->m_eCuttingEdgeMode);
    int tmp = 0;
    GetPDFSettingInt("PDF_bNormalCutting", &tmp);
    modeController->m_bIsNormalCutting = tmp != 0;
    GetPDFSettingDouble("PDF_dTopEdge", &modeController->m_dTopEdge);
    GetPDFSettingDouble("PDF_dBottomEdge", &modeController->m_dBottomEdge);
    GetPDFSettingDouble("PDF_dLeftEdge", &modeController->m_dLeftEdge);
    GetPDFSettingDouble("PDF_dRightEdge", &modeController->m_dRightEdge);
    return true;
}

bool DKXBlock::SetPDFModeController(const PdfModeController& modeController)
{
    SetPDFSettingInt("PDF_iRotation", modeController.m_iRotation);
    SetPDFSettingInt("PDF_eTurnPageMode", modeController.m_eTurnPageMode);
    SetPDFSettingUInt("PDF_uCurSubPage", modeController.m_uCurSubPage);
    SetPDFSettingUInt("PDF_uCurPageStartPos", modeController.m_uCurPageStartPos);
    SetPDFSettingInt("PDF_eReadingMode", modeController.m_eReadingMode);
    SetPDFSettingUInt("PDF_uSubPageOrder", modeController.m_uSubPageOrder);
    SetPDFSettingDouble("PDF_dWidth", modeController.m_dWidth);
    SetPDFSettingDouble("PDF_dHeight", modeController.m_dHeight);
    SetPDFSettingInt("PDF_eCuttingEdgeMode", modeController.m_eCuttingEdgeMode);
    SetPDFSettingInt("PDF_bNormalCutting", modeController.m_bIsNormalCutting);
    SetPDFSettingDouble("PDF_dTopEdge", modeController.m_dTopEdge);
    SetPDFSettingDouble("PDF_dBottomEdge", modeController.m_dBottomEdge);
    SetPDFSettingDouble("PDF_dLeftEdge", modeController.m_dLeftEdge);
    SetPDFSettingDouble("PDF_dRightEdge", modeController.m_dRightEdge);
    return true;
}

bool DKXBlock::GetPDFDarkenLevel(int* value)
{
    return GetPDFSettingInt("PDF_BoldLevel", value);
}

bool DKXBlock::SetPDFDarkenLevel(int value)
{
    return SetPDFSettingInt("PDF_BoldLevel", value);
}
