#include "DKXManager/BookInfo/DK_BookInfo.h"
#include <sstream>
#include "Utility/XmlUtil.h"

using namespace std;
using namespace dk::utility;



const char* DK_BookInfo::BOOKNAME = "BookName";
const char* DK_BookInfo::BOOKABSTRACT = "BookAbstract";
const char* DK_BookInfo::BOOKAUTHOR = "BookAuthor";
const char* DK_BookInfo::BOOKSOURCE = "BookSource";
const char* DK_BookInfo::FILEPATH = "FilePath";
const char* DK_BookInfo::FILEFORMAT = "FileFormat";
const char* DK_BookInfo::CREATETIME = "CreateTime";
const char* DK_BookInfo::ADDORDER   = "AddOrder";
const char* DK_BookInfo::LASTREADTIME = "LastReadTime";
const char* DK_BookInfo::READINGORDER = "ReadingOrder";
const char* DK_BookInfo::FILESIZE = "FileSize";
const char* DK_BookInfo::PASSWORD = "Password";
const char* DK_BookInfo::BOOKTAGS = "BookTags";
const char* DK_BookInfo::TAGITEM = "TagItem";
const char* DK_BookInfo::TXTTOCS = "TxtTocs";
const char* DK_BookInfo::TOCEXTRACTED = "TocExtracted";
const char* DK_BookInfo::TXTTOCITEM = "TxtTocItem"; 
const char* DK_BookInfo::BOOK_REVISION = "BookRevision";
const char* DK_BookInfo::READINGDATA_SYNC = "ReadingDataSync";
const char* DK_BookInfo::CRASHBOOK = "Crash";
const char* DK_BookInfo::BOOKID = "BookID";

DK_BookInfo::DK_BookInfo()
    : m_lAddOrder(0)
    , m_lReadingOrder(0)
    , m_bIsSerialized(false)
    , m_bTocExtracted(false)
    , m_bIsSync(0)
    , m_bIsCrash(0)
{
}

DK_BookInfo::~DK_BookInfo()
{
}
bool DK_BookInfo::SetBookFilePath(const string &_strPath)
{
    m_strFilePath = _strPath;
    return true;
}
string DK_BookInfo::GetBookFilePath() const
{
    return m_strFilePath;
}

string DK_BookInfo::GetFileType() const
{
    return m_strFileFormat;
}
bool DK_BookInfo::SetFileType(const string &_strType)
{
    m_strFileFormat = _strType;
    return true;
}

string DK_BookInfo::GetCreateTime() const
{
    return m_strCreateTime;
}

bool DK_BookInfo::SetCreateTime(const string &_strTime)
{
    m_strCreateTime = _strTime;
    return true;
}

long DK_BookInfo::GetAddOrder() const
{
    return m_lAddOrder;
}

bool DK_BookInfo::SetAddOrder(long _lAddOrder)
{
    m_lAddOrder = _lAddOrder;
    return true;
}

string DK_BookInfo::GetLastReadTime() const
{
    return m_strLastReadTime;
}

bool DK_BookInfo::SetLastReadTime(const string &_strTime)
{
    m_strLastReadTime = _strTime;
    return true;
}  

long DK_BookInfo::GetReadingOrder() const
{
    return m_lReadingOrder;
}

bool DK_BookInfo::SetReadingOrder(long _lOrder)
{
    m_lReadingOrder = _lOrder;
    return true;
}

string DK_BookInfo::GetBookName() const
{
    return m_strBookName;
}
bool DK_BookInfo::SetBookName(const string &_strBookname)
{
    m_strBookName = _strBookname;
    return true;
}

string DK_BookInfo::GetBookAuthor() const
{
    return m_strAuthor;
}
bool DK_BookInfo::SetBookAuthor(const string &_strAuthor)
{
    m_strAuthor = _strAuthor;
    return true;
}


string DK_BookInfo::GetBookSource() const
{
    return m_strBookSource;
}
bool DK_BookInfo::SetBookSource(const string &_strSource)
{
    m_strBookSource = _strSource;
    return true;
}

string DK_BookInfo::GetBookAbstract() const
{
    return m_strBookAbstract;
}
bool DK_BookInfo::SetBookAbstract(const string &_strAbstract)
{
    m_strBookAbstract = _strAbstract;
    return true;
}

string DK_BookInfo::GetFileSize() const
{
    return m_strFileSize;
}
bool DK_BookInfo::SetFileSize(const string &_strFileSize)
{
    m_strFileSize = _strFileSize;
    return true;
}


string DK_BookInfo::GetBookPassword() const
{
    return m_strPassword;
}

bool DK_BookInfo::SetBookPassword(const string &_strPassword)
{
    m_strPassword = _strPassword;
    return true;
}

string DK_BookInfo::GetBookRevision() const
{
    return m_bookRevision;
}

bool DK_BookInfo::SetBookRevision(const char* bookRevision)
{
    m_bookRevision = bookRevision;
    return true;
}

bool DK_BookInfo::SetSync(bool bIsSync)
{
	m_bIsSync = (int)bIsSync;
    return true;
}

bool DK_BookInfo::GetSync() const
{
	return (bool)m_bIsSync;
}

bool DK_BookInfo::SetCrash(bool bIsCrash)
{
	m_bIsCrash = (int)bIsCrash;
    return true;
}

bool DK_BookInfo::GetCrash() const
{
	return (bool)m_bIsCrash;
}

bool DK_BookInfo::SetBookID(const char* bookID)
{
	m_strBookID = bookID;
    return true;
}

string DK_BookInfo::GetBookID() const
{
	return m_strBookID;
}

std::vector<string> DK_BookInfo::GetFileTag() const
{
    return m_vFileTag;
}

bool DK_BookInfo::SetFileTag(const vector<string> &_vFileTag)
{
    m_vFileTag = _vFileTag;
    return true;
}

std::vector<TxtTocItem> DK_BookInfo::GetTxtToc() const
{
    return m_vTxtToc;
}

bool DK_BookInfo::SetTxtToc(const std::vector<TxtTocItem> &_vTxtToc)
{
    m_vTxtToc = _vTxtToc;
    return true;
}

bool DK_BookInfo::GetTocExtractedState()
{
	return m_bTocExtracted;
}

bool DK_BookInfo::SetTocExtractedState(bool _bExtracted)
{
	m_bTocExtracted = _bExtracted;
	return true;
}


bool DK_BookInfo::WriteToXML(XMLDomNode& node) const
{
    BEGIN_XML_STORE_DATA(storeData)
        {BOOKNAME, XNDT_STRING, &m_strBookName, false},
        {BOOKABSTRACT, XNDT_STRING, &m_strBookAbstract, false},
        {BOOKAUTHOR, XNDT_STRING, &m_strAuthor, false},
        {BOOKSOURCE, XNDT_STRING, &m_strBookSource, false},
        {FILEPATH, XNDT_STRING, &m_strFilePath, false},
        {FILEFORMAT, XNDT_STRING, &m_strFileFormat, false},
        {CREATETIME, XNDT_STRING, &m_strCreateTime, false},
        {ADDORDER, XNDT_INT, &m_lAddOrder, false},
        {LASTREADTIME, XNDT_STRING, &m_strLastReadTime, false},
        {READINGORDER, XNDT_INT, &m_lReadingOrder, false},
        {FILESIZE, XNDT_STRING, &m_strFileSize, false},
        {PASSWORD, XNDT_STRING, &m_strPassword, false},
        {BOOKTAGS, XNDT_COMPLEX, this, false, WriteTagsFunc},
        {TXTTOCS, XNDT_FOREACH_CHILD, this, false, WriteTxtTocFunc},
        {TOCEXTRACTED, XNDT_INT, &m_bTocExtracted, "false"},
        {BOOK_REVISION, XNDT_STRING, &m_bookRevision, false},
        {READINGDATA_SYNC, XNDT_INT, &m_bIsSync, false},
        {CRASHBOOK, XNDT_INT, &m_bIsCrash, false},
        {BOOKID, XNDT_STRING, &m_strBookID, false},
        
    END_XML_STORE_DATA;
    return XmlUtil::BuildXmlNode(&node, storeData);
}

bool DK_BookInfo::ReadFromXml(const XMLDomNode&  node)
{
	m_vTxtToc.clear();
    BEGIN_XML_PARSE_DATA(parseData)
        {BOOKNAME, XNDT_STRING, &m_strBookName, false},
        {BOOKABSTRACT, XNDT_STRING, &m_strBookAbstract, false},
        {BOOKAUTHOR, XNDT_STRING, &m_strAuthor, false},
        {BOOKSOURCE, XNDT_STRING, &m_strBookSource, false},
        {FILEPATH, XNDT_STRING, &m_strFilePath, false},
        {FILEFORMAT, XNDT_STRING, &m_strFileFormat, false},
        {CREATETIME, XNDT_STRING, &m_strCreateTime, false},
        {ADDORDER, XNDT_INT, &m_lAddOrder, false},
        {LASTREADTIME, XNDT_STRING, &m_strLastReadTime, false},
        {READINGORDER, XNDT_INT, &m_lReadingOrder, false},
        {FILESIZE, XNDT_STRING, &m_strFileSize, false},
        {PASSWORD, XNDT_STRING, &m_strPassword, false},
        {BOOKTAGS, XNDT_COMPLEX, this, false, ReadTagsFunc},
        {TXTTOCS, XNDT_FOREACH_CHILD, this, false, AddTxtTocFunc},
        {TOCEXTRACTED, XNDT_INT, &m_bTocExtracted, "false"},
        {BOOK_REVISION, XNDT_STRING, &m_bookRevision, false},
        {READINGDATA_SYNC, XNDT_INT, &m_bIsSync, false},
        {CRASHBOOK, XNDT_INT, &m_bIsCrash, false},
    	{BOOKID, XNDT_STRING, &m_strBookID, false},
        
    END_XML_PARSE_DATA;
    return XmlUtil::ParseXmlNode(&node, parseData);  
}

bool DK_BookInfo::ReadTagsFunc(const XMLDomNode* node, void* arg)
{
    // TODO: 添加tag解析
    //DK_BookInfo* pThis = (DK_BookInfo*)arg;
    return true;
}

bool DK_BookInfo::WriteTagsFunc(XMLDomNode* node, const void* arg)
{
    // TODO: 添加tag写入
    //DK_BookInfo* pThis = (DK_BookInfo*)arg;
    return true;
}

bool DK_BookInfo::AddTxtTocFunc(const XMLDomNode* node, void* arg)
{
    DK_BookInfo* pThis = (DK_BookInfo*)arg;
    TxtTocItem item;
    if (item.ReadFromXml(*node))
    {
        pThis->m_vTxtToc.push_back(item);
    }
    return true;
}


bool DK_BookInfo::WriteTxtTocFunc(XMLDomNode* node, const void* arg)
{
    DK_BookInfo* pThis = (DK_BookInfo*)arg;
    for (DK_AUTO(iter, pThis->m_vTxtToc.begin());
            iter != pThis->m_vTxtToc.end();
            ++iter)
    {
        node->AddElement(TXTTOCITEM);
        XMLDomNode child;
        node->GetLastChild(&child);
        iter->WriteToXML(child);
    }
    return true;
}

bool DK_BookInfo::WriteToXMLFunc(XMLDomNode* node, const void* arg)
{
    const DK_BookInfo* pThis = (const DK_BookInfo*)arg;
    return pThis->WriteToXML(*node);
}
