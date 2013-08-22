#include "DKXManager/BaseType/CT_RefPos.h"
#include <sstream>
#include "Utility/XmlUtil.h"

using namespace dk::utility;
using namespace std;
const char* CT_RefPos::CHAPTERINDEX = "ChapterIndex";
const char* CT_RefPos::PARAINDEX = "ParaIndex";
const char* CT_RefPos::ATOMINDEX = "AtomIndex";
const char* CT_RefPos::OFFSET_IN_CHAPTER = "OffsetInChapter";
const char* CT_RefPos::CHAPTER_ID = "ChapterID";

CT_RefPos::CT_RefPos()
    : m_chapterIndex(0)
    , m_paraIndex(0)
    , m_atomIndex(0)
    , m_offsetInChapter(-1)
{
}

CT_RefPos::CT_RefPos(int chapterIndex, int paraIndex, int elementIndex, int offsetInChapter)
: m_chapterIndex(chapterIndex)
, m_paraIndex(paraIndex)
, m_atomIndex(elementIndex)
, m_offsetInChapter(offsetInChapter)
{
}

CT_RefPos::~CT_RefPos()
{
}

bool CT_RefPos::InitFromSyncData(const XMLDomNode* node)
{
    BEGIN_XML_PARSE_DATA(parseData)
        {"ChapterID", XNDT_STRING, &m_chapterId, false},
        {"Offset", XNDT_INT, &m_offsetInChapter, false},
        {"ChapterIndex", XNDT_INT, &m_chapterIndex, false},
        {"ParaIndex", XNDT_INT, &m_paraIndex, false},
        {"AtomIndex", XNDT_INT, &m_atomIndex, false},
    END_XML_PARSE_DATA;
    XmlUtil::ParseXmlNode(node, parseData);
    return true;
}

bool CT_RefPos::SetChapterIndex(int chapterIndex)
{
    m_chapterIndex = chapterIndex;
    return true;
}

int CT_RefPos::GetChapterIndex()const
{
    return m_chapterIndex;
}

bool CT_RefPos::SetParaIndex(int paraIndex)
{
    m_paraIndex = paraIndex;
    return true;
}

int CT_RefPos::GetParaIndex()const
{
    return m_paraIndex;
}

bool CT_RefPos::SetAtomIndex(int atomIndex)
{
    m_atomIndex = atomIndex;
    return true;
}

int CT_RefPos::GetAtomIndex()const
{
    return m_atomIndex;
}

bool CT_RefPos::SetOffsetInChapter(int offsetInChapter)
{
    m_offsetInChapter = offsetInChapter;
    return true;
}

int CT_RefPos::GetOffsetInChapter() const
{
    return m_offsetInChapter;
}

bool CT_RefPos::WriteToXML(XMLDomNode& node) const
{
    BEGIN_XML_STORE_DATA(storeData)
        {CHAPTER_ID, XNDT_STRING, &m_chapterId, false},
        {OFFSET_IN_CHAPTER, XNDT_INT, &m_offsetInChapter, false},
        {CHAPTERINDEX, XNDT_INT, &m_chapterIndex, false},
        {PARAINDEX, XNDT_INT, &m_paraIndex, false},
        {ATOMINDEX, XNDT_INT, &m_atomIndex, false},
    END_XML_STORE_DATA;
    return XmlUtil::BuildXmlNode(&node, storeData);
}

bool CT_RefPos::ReadFromXml(const XMLDomNode& node)
{
    BEGIN_XML_PARSE_DATA(parseData)
        {CHAPTERINDEX, XNDT_INT, &m_chapterIndex, false},
        {PARAINDEX, XNDT_INT, &m_paraIndex, false},
        {OFFSET_IN_CHAPTER, XNDT_INT, &m_offsetInChapter, false},
        {ATOMINDEX, XNDT_INT, &m_atomIndex, false},
        {CHAPTER_ID, XNDT_STRING, &m_chapterId, false},
    END_XML_PARSE_DATA;
    return XmlUtil::ParseXmlNode(&node, parseData);
}

int CT_RefPos::CompareTo(const CT_RefPos &_clsRefPos)
{
    int _iOffset = this->m_chapterIndex - _clsRefPos.GetChapterIndex();
    if(_iOffset != 0)
    {
        return _iOffset;
    }
    _iOffset = this->m_paraIndex - _clsRefPos.GetParaIndex();
    if(_iOffset != 0)
    {
        return _iOffset;
    }

    _iOffset = this->m_atomIndex - _clsRefPos.GetAtomIndex();
    return _iOffset;
}

bool CT_RefPos::WriteToSyncDomNode(XMLDomNode* domNode) const
{
    BEGIN_XML_STORE_DATA(storeData)
        {"ChapterID", XNDT_STRING, &m_chapterId, false},
        {"Offset", XNDT_INT, &m_offsetInChapter, false},
        {"ChapterIndex", XNDT_INT, &m_chapterIndex, false},
        {"ParaIndex", XNDT_INT, &m_paraIndex, false},
        {"AtomIndex", XNDT_INT, &m_atomIndex, false},
    END_XML_STORE_DATA;
    return XmlUtil::BuildXmlNode(domNode, storeData);
}

bool CT_RefPos::WriteToXMLFunc(XMLDomNode* node, const void* arg)
{
    const CT_RefPos* pThis = (const CT_RefPos*)arg;
    return pThis->WriteToXML(*node);
}

bool CT_RefPos::ReadFromXmlFunc(const XMLDomNode* node, void* arg)
{
    CT_RefPos* pThis = (CT_RefPos*)arg;
    return pThis->ReadFromXml(*node);
}
