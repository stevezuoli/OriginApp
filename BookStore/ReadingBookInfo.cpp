#include "BookStore/ReadingBookInfo.h"
#include "Utility/XmlUtil.h"
#include "DKXManager/ReadingInfo/ICT_ReadingDataItem.h"
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"

using namespace dk::utility;

namespace dk
{
namespace bookstore
{
namespace sync
{

const char* ReadingBookInfo::CURRENT_VERSION = "1.1";
std::string ReadingDataItem::TypeToString(ReadingDataItem::Type type)
{
    const char* typeStrings[] = {"BOOKMARK", "PROGRESS", "COMMENT", NULL}; 
    Type types[] = {BOOKMARK, PROGRESS, COMMENT, UNKNOWN };
    for (int i = 0; typeStrings[i]; ++i)
    {
        if (types[i] == type)
        {
            return typeStrings[i];
        }
    }
    return "";
}

ReadingDataItem::Type ReadingDataItem::StringToType(const char* typeString)
{
    const char* typeStrings[] = {"BOOKMARK", "PROGRESS", "COMMENT", NULL}; 
    Type types[] = {BOOKMARK, PROGRESS, COMMENT};
    for (int i = 0; typeStrings[i]; ++i)
    {
        if (strcmp(typeStrings[i], typeString) == 0)
        {
            return types[i];
        }
    }
    return UNKNOWN;
}

bool ReadingDataItem::WriteToDomNode(XMLDomNode* domNode) const
{
    string typeString = TypeToString(m_type);
    
    BEGIN_XML_STORE_DATA(bookMarkStoreData)
        {"Type", XNDT_STRING, &typeString, false},
        {"DataID", XNDT_STRING, &m_dataId, false},
        {"Color", XNDT_STRING, &m_color, false},
        {"Tag", XNDT_STRING, &m_tag, false},
        {"CreateTime", XNDT_STRING, &m_createTime, false},
        {"LastModifyTime", XNDT_STRING, &m_lastModifyTime, false},
        {"RefContent", XNDT_STRING, &m_refContent, false},
        {"ChapterTitle", XNDT_STRING, &m_chapterTitle, false},
        {"RefPos", XNDT_COMPLEX, this, false, WriteRefPosFunc},
    END_XML_STORE_DATA;
    BEGIN_XML_STORE_DATA(commentStoreData)
        {"Type", XNDT_STRING, &typeString, false},
        {"DataID", XNDT_STRING, &m_dataId, false},
        {"Color", XNDT_STRING, &m_color, false},
        {"Tag", XNDT_STRING, &m_tag, false},
        {"CreateTime", XNDT_STRING, &m_createTime, false},
        {"LastModifyTime", XNDT_STRING, &m_lastModifyTime, false},
        {"RefContent", XNDT_STRING, &m_refContent, false},
        {"Content", XNDT_STRING, &m_content, false},
        {"ChapterTitle", XNDT_STRING, &m_chapterTitle, false},
        {"BeginRefPos", XNDT_COMPLEX, this, false, WriteBeginRefPosFunc},
        {"EndRefPos", XNDT_COMPLEX, this, false, WriteEndRefPosFunc},
    END_XML_STORE_DATA;
    BEGIN_XML_STORE_DATA(digestStoreData)
        {"Type", XNDT_STRING, &typeString, false},
        {"DataID", XNDT_STRING, &m_dataId, false},
        {"Color", XNDT_STRING, &m_color, false},
        {"Tag", XNDT_STRING, &m_tag, false},
        {"CreateTime", XNDT_STRING, &m_createTime, false},
        {"LastModifyTime", XNDT_STRING, &m_lastModifyTime, false},
        {"RefContent", XNDT_STRING, &m_refContent, false},
        {"ChapterTitle", XNDT_STRING, &m_chapterTitle, false},
        {"BeginRefPos", XNDT_COMPLEX, this, false, WriteBeginRefPosFunc},
        {"EndRefPos", XNDT_COMPLEX, this, false, WriteEndRefPosFunc},
    END_XML_STORE_DATA;
    XmlStoreData* storeData = NULL;
    if (m_type == PROGRESS || m_type == BOOKMARK)
    {
        storeData = bookMarkStoreData;
    }
    // 书摘或批注
    else if (HasContent())
    {
        storeData = commentStoreData;
    }
    else
    {
        storeData = digestStoreData;
    }
    XmlUtil::BuildXmlNode(domNode, storeData);
    return true;
}

bool ReadingDataItem::WriteBeginRefPosFunc(XMLDomNode* domNode, const void* arg)
{
    const ReadingDataItem* pThis = (const ReadingDataItem*)arg;
    return pThis->m_beginRefPos.WriteToSyncDomNode(domNode);
}

bool ReadingDataItem::WriteEndRefPosFunc(XMLDomNode* domNode, const void* arg)
{
    const ReadingDataItem* pThis = (const ReadingDataItem*)arg;
    return pThis->m_endRefPos.WriteToSyncDomNode(domNode);
}

bool ReadingDataItem::WriteRefPosFunc(XMLDomNode* domNode, const void* arg)
{
    const ReadingDataItem* pThis = (const ReadingDataItem*)arg;
    return pThis->m_refPos.WriteToSyncDomNode(domNode);
}

ReadingDataItem::ReadingDataItem(const XMLDomNode* domNode)
{
    std::string typeString;
    BEGIN_XML_PARSE_DATA(parseData)
        {"Type", XNDT_STRING, &typeString, false},
        {"DataID", XNDT_STRING, &m_dataId, false},
        {"Color", XNDT_STRING, &m_color, false},
        {"Tag", XNDT_STRING, &m_tag, false},
        {"CreateTime", XNDT_STRING, &m_createTime, false},
        {"LastModifyTime", XNDT_STRING, &m_lastModifyTime, false},
        {"RefContent", XNDT_STRING, &m_refContent, false},
        {"Content", XNDT_STRING, &m_content, false},
        {"ChapterTitle", XNDT_STRING, &m_chapterTitle, false},
        {"BeginRefPos", XNDT_COMPLEX, this, false, AddBeginRefPosFunc},
        {"EndRefPos", XNDT_COMPLEX, this, false, AddEndRefPosFunc},
        {"RefPos", XNDT_COMPLEX, this, false, AddRefPosFunc},
    END_XML_PARSE_DATA;
    XmlUtil::ParseXmlNode(domNode, parseData);
    m_type = StringToType(typeString.c_str());
}

bool ReadingDataItem::AddRefPosFunc(const XMLDomNode* node, void* arg)
{
    ReadingDataItem* pThis = (ReadingDataItem*)arg;
    pThis->m_refPos.InitFromSyncData(node);
    return true;
}

bool ReadingDataItem::AddBeginRefPosFunc(const XMLDomNode* node, void* arg)
{
    ReadingDataItem* pThis = (ReadingDataItem*)arg;
    pThis->m_beginRefPos.InitFromSyncData(node);
    return true;
}

bool ReadingDataItem::AddEndRefPosFunc(const XMLDomNode* node, void* arg)
{
    ReadingDataItem* pThis = (ReadingDataItem*)arg;
    pThis->m_endRefPos.InitFromSyncData(node);
    return true;
}

ReadingBookInfo::ReadingBookInfo()
    : m_hasSyncResult(false)
{
}

void ReadingBookInfo::InitFromDomNode(const XMLDomNode* node)
{
    XmlParseData parseData[] =
    {
    	{"DeviceID", XNDT_STRING, &m_deviceId, false},
        {"Version", XNDT_STRING, &m_version, false},
        {"BookID", XNDT_STRING, &m_bookId, false},
        {"BookRevision", XNDT_STRING, &m_bookRevision, false},
        {"KernelVersion", XNDT_STRING, &m_kernelVersion, false},
        {"ReadingData", XNDT_COMPLEX, this, false, AddReadingDataFunc},
        {"result", XNDT_COMPLEX, this, false, ParseResultFunc},
        {NULL, XNDT_NONE, NULL}
    };
    XmlUtil::ParseXmlNode(node, parseData);
}

bool ReadingBookInfo::WriteToDomNode(XMLDomNode* domNode) const
{
    BEGIN_XML_STORE_DATA(storeData)
        {"Version", XNDT_STRING, &m_version, false},
        {"BookID", XNDT_STRING, &m_bookId, false},
        {"BookRevision", XNDT_STRING, &m_bookRevision, false},
        {"KernelVersion", XNDT_STRING, &m_kernelVersion, false},
        {"ReadingData", XNDT_COMPLEX, this, false, WriteReadingDataFunc},
    END_XML_STORE_DATA;
    XmlUtil::BuildXmlNode(domNode, storeData);
    return true;
}

bool ReadingBookInfo::WriteReadingDataFunc(XMLDomNode* domNode, const void* arg)
{
    ReadingBookInfo* pThis = (ReadingBookInfo*)arg;
    for (size_t i = 0; i < pThis->m_readingDataItems.size(); ++i)
    {
        if (!pThis->m_readingDataItems[i].IsValid())
        {
            continue;
        }
        domNode->AddElement("ReadingDataItem");
        XMLDomNode child;
        domNode->GetLastChild(&child);
        pThis->m_readingDataItems[i].WriteToDomNode(&child);
    }
    return true;
}
bool ReadingBookInfo::ParseResultFunc(const XMLDomNode* domNode, void* arg)
{
    ReadingBookInfo* pThis = (ReadingBookInfo*)arg;
    pThis->m_syncResult.InitFromDomNode(domNode);
    pThis->m_hasSyncResult = true;
    return true;
}

bool ReadingBookInfo::AddReadingDataFunc(const XMLDomNode* node, void* arg)
{
    XmlParseData parseData[] =
    {
        {"ReadingDataItem", XNDT_COMPLEX, arg, false, AddReadingDataItemFunc},
        {NULL, XNDT_NONE, NULL}
    };
    XmlUtil::ParseXmlNode(node, parseData);
    return true;
}

bool ReadingBookInfo::AddReadingDataItemFunc(const XMLDomNode* node, void* arg)
{
    ReadingDataItem readingDataItem(node);
    ReadingBookInfo* pThis = (ReadingBookInfo*)arg;
    pThis->m_readingDataItems.push_back(readingDataItem);
    return true;
}

void ReadingBookInfo::RemoveReadingDataItem(int index)
{
    if (index >= 0 && index < m_readingDataItems.size())
    {
        m_readingDataItems.erase(m_readingDataItems.begin() + index);
    }
}

void ReadingBookInfo::ClearReadingCommentDataItem()
{
    for (int i = m_readingDataItems.size() - 1; i >= 0; --i)
    {
        if (m_readingDataItems[i].GetType() == ReadingDataItem::COMMENT)
        {
            RemoveReadingDataItem(i);
        }
    }
}

std::string ReadingBookInfo::ToPostData() const
{
    XMLDomDocument* newDoc = XMLDomDocument::CreateDocument(false);
    XMLDomNode* docNode = newDoc->ToDomNode();
    docNode->AddRootElement("BookInfo");
    XMLDomNode newDocRoot;
    docNode->GetLastChild(&newDocRoot);
	
	if(!m_deviceId.empty())
	{
		//只有阅读进度上传时才需要上传此值
		newDocRoot.AddElement("DeviceID");
	    XMLDomNode child;
	    newDocRoot.GetLastChild(&child);
	    child.AddText(m_deviceId.c_str());
	}
	
    WriteToDomNode(&newDocRoot);
    char* xmlText = NULL;
    DK_SIZE_T xmlTextSize = 0;
    newDoc->SaveXmlToBuffer(&xmlText, &xmlTextSize);
    std::string result;
    if (NULL != xmlText)
    { 
        result = xmlText;
    }
    newDoc->FreeXmlBuffer(xmlText);
    XMLDomDocument::ReleaseDocument(newDoc);
    return result;
}

void SyncResult::InitFromDomNode(const XMLDomNode* domNode)

{
    BEGIN_XML_PARSE_DATA(parseData)
    {"code", XNDT_INT, &m_code, false},
    {"latestversion", XNDT_INT, &m_latestVersion, false},
    {"bookid", XNDT_STRING, &m_bookId, false},
    {"message", XNDT_STRING, &m_message, false},
    END_XML_PARSE_DATA;

    XmlUtil::ParseXmlNode(domNode, parseData);
}

ReadingProgressGetResult::ReadingProgressGetResult()
    : m_hasSyncResult(false)
    , m_hasReadingProgress(false)
{
}

void ReadingProgressGetResult::InitFromDomNode(const XMLDomNode* domNode)
{
    const char* currentNodeValue = domNode->GetNodeValue();
    if (strcmp("result", currentNodeValue) == 0)
    {
        ParseResultFunc(domNode, this);
    }
    else if (strcmp("Progress", currentNodeValue) == 0)
    {
        BEGIN_XML_PARSE_DATA(parseData)
        {"BookInfo", XNDT_COMPLEX, this, false, ParseProgressFunc},
        END_XML_PARSE_DATA;
        XmlUtil::ParseXmlNode(domNode, parseData);
    }
}

bool ReadingProgressGetResult::ParseResultFunc(const XMLDomNode* domNode, void* arg)
{
    ReadingProgressGetResult* pThis = (ReadingProgressGetResult*)arg;
    pThis->m_syncResult.InitFromDomNode(domNode);
    pThis->m_hasSyncResult = false;
    return true;
}

bool ReadingProgressGetResult::ParseProgressFunc(const XMLDomNode* domNode, void* arg)
{
    ReadingProgressGetResult* pThis = (ReadingProgressGetResult*)arg;
    pThis->m_readingProgress.InitFromDomNode(domNode);
    pThis->m_hasReadingProgress = true;
    return true;
}

ReadingDataSyncResult::ReadingDataSyncResult()
    : m_hasSyncResult(false)
    , m_hasReadingData(false)
{
}

void ReadingDataSyncResult::InitFromDomNode(const XMLDomNode* domNode)
{
    const char* currentNodeValue = domNode->GetNodeValue();
    if (strcmp("result", currentNodeValue) == 0)
    {
        ParseResultFunc(domNode, this);
    }
    else if (strcmp("Reading", currentNodeValue) == 0)
    {
        BEGIN_XML_PARSE_DATA(parseData)
        {"BookInfo", XNDT_COMPLEX, this, false, ParseReadingDataFunc},
        END_XML_PARSE_DATA;
        XmlUtil::ParseXmlNode(domNode, parseData);
    }
}

bool ReadingDataSyncResult::ParseResultFunc(const XMLDomNode* domNode, void* arg)
{
    ReadingDataSyncResult* pThis = (ReadingDataSyncResult*)arg;
    pThis->m_syncResult.InitFromDomNode(domNode);
    pThis->m_hasSyncResult = true;
    return true;
}

bool ReadingDataSyncResult::ParseReadingDataFunc(const XMLDomNode* domNode, void* arg)
{
    ReadingDataSyncResult* pThis = (ReadingDataSyncResult*)arg;
    pThis->m_readingData.InitFromDomNode(domNode);
    pThis->m_hasReadingData = true;
    return true;
}

ICT_ReadingDataItem* CreateICTReadingDataItemFromReadingDataItem(
        const ReadingDataItem& readingDataItem)
{
    ICT_ReadingDataItem* localItem = DKXReadingDataItemFactory::CreateReadingDataItem();
    if (NULL == localItem)
    {
        return NULL;
    }
    localItem->SetId(readingDataItem.GetDataId().c_str());
    localItem->SetCreateTime(readingDataItem.GetCreateTime());
    localItem->SetLastModifyTime(readingDataItem.GetLastModifyTime());
    localItem->SetColor(readingDataItem.GetColor().c_str());
    localItem->SetTag(readingDataItem.GetTag().c_str());
    localItem->SetChapterTitle(readingDataItem.GetChapterTitle());

    switch (readingDataItem.GetType())
    {
    case ReadingDataItem::PROGRESS:
        localItem->SetUserDataType(ICT_ReadingDataItem::PROGRESS);
        localItem->SetBeginPos(readingDataItem.GetRefPos());
        break;
    case ReadingDataItem::BOOKMARK:
        localItem->SetUserDataType(ICT_ReadingDataItem::BOOKMARK);
        localItem->SetBeginPos(readingDataItem.GetRefPos());
        localItem->SetEndPos(readingDataItem.GetRefPos());
        localItem->SetBookContent(readingDataItem.GetRefContent());
        break;
    case ReadingDataItem::COMMENT:
        localItem->SetUserDataType(
                readingDataItem.IsDigest() 
                ? ICT_ReadingDataItem::DIGEST 
                : ICT_ReadingDataItem::COMMENT);
        localItem->SetBeginPos(readingDataItem.GetBeginRefPos());
        localItem->SetEndPos(readingDataItem.GetEndRefPos());
        localItem->SetBookContent(readingDataItem.GetRefContent());
        localItem->SetUserContent(readingDataItem.GetContent());
        break;
    default:
        break;
    }
    localItem->SetUploaded(true);
    return localItem;
}

ReadingDataItem* CreateReadingDataItemFromICTReadingDataItem(
        const ICT_ReadingDataItem& localReadingDataItem)
{
    ReadingDataItem* cloudItem = new ReadingDataItem();
    if (NULL == cloudItem)
    {
        return NULL;
    }
    ReadingDataItem& readingDataItem = *cloudItem;
    readingDataItem.SetDataId(localReadingDataItem.GetId().c_str());
    readingDataItem.SetCreateTime(localReadingDataItem.GetCreateTime().c_str());
    readingDataItem.SetLastModifyTime(localReadingDataItem.GetLastModifyTime().c_str());
    readingDataItem.SetColor(localReadingDataItem.GetColor().c_str());
    readingDataItem.SetTag(localReadingDataItem.GetTag().c_str());
    readingDataItem.SetChapterTitle(localReadingDataItem.GetChapterTitle());

    if (ICT_ReadingDataItem::PROGRESS == localReadingDataItem.GetUserDataType())
    {
        readingDataItem.SetType(ReadingDataItem::PROGRESS);
        readingDataItem.SetRefPos(localReadingDataItem.GetBeginPos());
    }
    else if (ICT_ReadingDataItem::BOOKMARK == localReadingDataItem.GetUserDataType())
    {
        readingDataItem.SetType(ReadingDataItem::BOOKMARK);
        readingDataItem.SetRefPos(localReadingDataItem.GetBeginPos());
        readingDataItem.SetRefContent(localReadingDataItem.GetBookContent().c_str());
    }
    else if ((ICT_ReadingDataItem::DIGEST == localReadingDataItem.GetUserDataType())
            || (ICT_ReadingDataItem::COMMENT == localReadingDataItem.GetUserDataType()))
    {
        readingDataItem.SetType(ReadingDataItem::COMMENT);
        readingDataItem.SetBeginRefPos(localReadingDataItem.GetBeginPos());
        readingDataItem.SetEndRefPos(localReadingDataItem.GetEndPos());
        readingDataItem.SetRefContent(localReadingDataItem.GetBookContent().c_str());
        readingDataItem.SetContent(localReadingDataItem.GetUserContent().c_str());
    }
    return cloudItem;
}

bool IsEqual(const ICT_ReadingDataItem& localItem, const ReadingDataItem& readingDataItem)
{
    bool equal = true;
    equal = (localItem.GetId() == readingDataItem.GetDataId()
         &&  localItem.GetCreateTime() == readingDataItem.GetCreateTime()
         &&  localItem.GetLastModifyTime() == readingDataItem.GetLastModifyTime()
         &&  localItem.GetColor() == readingDataItem.GetColor()
         &&  localItem.GetTag() == readingDataItem.GetTag());
    if (equal)
    {
        switch (readingDataItem.GetType())
        {
        case ReadingDataItem::PROGRESS:
            equal = (localItem.GetUserDataType() == ICT_ReadingDataItem::PROGRESS
                    && localItem.GetBeginPos() == readingDataItem.GetRefPos());
            break;
        case ReadingDataItem::BOOKMARK:
            equal = (localItem.GetUserDataType() == ICT_ReadingDataItem::BOOKMARK
                    && localItem.GetBeginPos() == readingDataItem.GetRefPos()
                    && localItem.GetEndPos() == readingDataItem.GetRefPos()
                    && localItem.GetBookContent() == readingDataItem.GetRefContent());
            break;
        case ReadingDataItem::COMMENT:
            equal = (localItem.GetUserDataType() == (readingDataItem.IsDigest() ? ICT_ReadingDataItem::DIGEST : ICT_ReadingDataItem::COMMENT)
                    && localItem.GetBeginPos() == readingDataItem.GetBeginRefPos()
                    && localItem.GetEndPos() == readingDataItem.GetEndRefPos()
                    && localItem.GetBookContent() == readingDataItem.GetRefContent()
                    && localItem.GetUserContent() == readingDataItem.GetContent());
            break;
        default:
            break;
        }
    }

    return equal;
}
} // namespace sync
} // namespace bookstore
} // namespace dk
