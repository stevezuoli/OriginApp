#include "DKXManager/ReadingInfo/CT_ReadingDataItemImpl.h"
#include "Utility/Misc.h"
#include "KernelVersion.h"
#include <sstream>
#include "Utility/XmlUtil.h"

using namespace dk::utility;
using namespace std;

static const string g_strTypeName[ICT_ReadingDataItem::TYPECOUNT] = 
{
    TYPE_LIST(TYPE_LIST_NAME)
};

const char* CT_ReadingDataItemImpl::USERDATATYPE = "Type";
const char* CT_ReadingDataItemImpl::USERDATAID = "DataId";
const char* CT_ReadingDataItemImpl::CREATETIME = "CreateTime";
const char* CT_ReadingDataItemImpl::LASTMODTIME = "LastModifyTime";
const char* CT_ReadingDataItemImpl::CREATOR = "Creator";
const char* CT_ReadingDataItemImpl::BEGINPOS = "BeginPos";
const char* CT_ReadingDataItemImpl::ENDPOS = "EndPos";
const char* CT_ReadingDataItemImpl::BOOKCONTENT = "BookContent";
const char* CT_ReadingDataItemImpl::USERCONTENT = "UserContent";
const char* CT_ReadingDataItemImpl::KERNELVERSION = "KernelVersion";
const char* CT_ReadingDataItemImpl::COLOR = "Color"; 
const char* CT_ReadingDataItemImpl::TAG = "Tag";
const char* CT_ReadingDataItemImpl::UPLOADED = "Uploaded";
const char* CT_ReadingDataItemImpl::PERCENT = "Percent";
const char* CT_ReadingDataItemImpl::COMICSFRAMEMODE = "ComicsFrameMode";
const char* CT_ReadingDataItemImpl::CHAPTERTITLE = "ChapterTitle";

CT_ReadingDataItemImpl::CT_ReadingDataItemImpl()
    : m_eType(ICT_ReadingDataItem::BOOKMARK)
    , m_strID("")
    , m_strCreateTime("")
    , m_strLastModifyTime("")
    , m_strCreator("")
    , m_strBookContent("")
    , m_strUserContent("")
    , m_uploaded(false)
    , m_percent(-1)
    , m_comicsFrame(false)
{
	m_strKernelVersion = DK_GetKernelVersion();
}

CT_ReadingDataItemImpl::CT_ReadingDataItemImpl(ICT_ReadingDataItem::UserDataType _eType)
    : m_eType(_eType)
    , m_strID("")
    , m_strCreateTime("")
    , m_strLastModifyTime("")
    , m_strCreator("")
    , m_strBookContent("")
    , m_strUserContent("")
    , m_uploaded(false)
    , m_percent(-1)
    , m_comicsFrame(false)
{
	m_strKernelVersion = DK_GetKernelVersion();
}

CT_ReadingDataItemImpl::~CT_ReadingDataItemImpl()
{

}

std::string CT_ReadingDataItemImpl::GetKernelVersion() const
{
	return m_strKernelVersion;
}

ICT_ReadingDataItem::UserDataType CT_ReadingDataItemImpl::GetUserDataType() const
{
    return m_eType;
}

bool CT_ReadingDataItemImpl::SetUserDataType(UserDataType _eType)
{
    m_eType = _eType;
    return true;
}

string CT_ReadingDataItemImpl::GetId() const
{
    return m_strID;
}

void CT_ReadingDataItemImpl::SetId(const char* id)
{
    m_strID = id;
}

bool CT_ReadingDataItemImpl::CreateId()
{
    if (m_strID.empty())
    {
        m_strID = GenerateUUID();
    }
    return true;
}

static void ConvertTime(string* s)
{
    size_t spacePos = s->find(' ');
    if (spacePos != string::npos)
    {
        s->replace(spacePos, 1, 1, 'T');
    }
}

string CT_ReadingDataItemImpl::GetCreateTime() const
{
    return m_strCreateTime;
}
bool CT_ReadingDataItemImpl::SetCreateTime(const string &_strCreateTime)
{
    m_strCreateTime = _strCreateTime;
    ConvertTime(&m_strCreateTime);
    return true;
}

string CT_ReadingDataItemImpl::GetLastModifyTime() const
{
    return m_strLastModifyTime;
}
bool CT_ReadingDataItemImpl::SetLastModifyTime(const string &_strLastModifyTime)
{
    m_strLastModifyTime = _strLastModifyTime;
    ConvertTime(&m_strLastModifyTime);
    return true;
}

string CT_ReadingDataItemImpl::GetCreator() const
{
    return m_strCreator;
}
bool CT_ReadingDataItemImpl::SetCreator(const string &_strCreator)
{
    m_strCreator = _strCreator;
    return true;
}

int CT_ReadingDataItemImpl::GetPercent() const
{
    return m_percent;
}

bool CT_ReadingDataItemImpl::SetPercent(int  percent)
{
    if (percent < 0)
        return false;
    m_percent = percent;
    return true;
}

CT_RefPos CT_ReadingDataItemImpl::GetBeginPos() const
{
    return m_clsBeginPos;
}

bool CT_ReadingDataItemImpl::SetBeginPos(const CT_RefPos &_clsRefPos)
{
    m_clsBeginPos = _clsRefPos;
    return true;
}

CT_RefPos CT_ReadingDataItemImpl::GetEndPos() const
{
    return m_clsEndPos;
}

bool CT_ReadingDataItemImpl::SetEndPos(const CT_RefPos &_clsRefPos)
{
    m_clsEndPos = _clsRefPos;
    return true;
}

void CT_ReadingDataItemImpl::SetComicsFrameMode(bool frameMode)
{
    m_comicsFrame = frameMode;
}

bool CT_ReadingDataItemImpl::IsComicsFrameMode() const
{
    return m_comicsFrame;
}

string CT_ReadingDataItemImpl::GetBookContent() const
{
    return m_strBookContent;
}

bool CT_ReadingDataItemImpl::SetBookContent(const string &_strContent)
{
    m_strBookContent = _strContent;
    return true;
}

string CT_ReadingDataItemImpl::GetUserContent() const
{
    return m_strUserContent;
}

bool CT_ReadingDataItemImpl::SetUserContent(const string &_strContent)
{
    m_strUserContent = _strContent;
    return true;
}

void CT_ReadingDataItemImpl::SetChapterTitle(const string &_strChapterTitle)
{
    m_strChapterTitle = _strChapterTitle;
}

string CT_ReadingDataItemImpl::GetChapterTitle() const
{
    return m_strChapterTitle;
}

bool CT_ReadingDataItemImpl::WriteToXML(XMLDomNode& node) const
{
    BEGIN_XML_STORE_DATA(storeData)
        {USERDATATYPE, XNDT_STRING, &g_strTypeName[m_eType], false},
        {KERNELVERSION, XNDT_STRING, &m_strKernelVersion, false},
        {COLOR, XNDT_STRING, &m_color, false},
        {TAG, XNDT_STRING, &m_tag, false},
        {USERDATAID, XNDT_STRING, &m_strID, false},
        {CREATETIME, XNDT_STRING, &m_strCreateTime, false},
        {LASTMODTIME, XNDT_STRING, &m_strLastModifyTime, false},
        {CREATOR, XNDT_STRING, &m_strCreator, false},
        {BOOKCONTENT, XNDT_STRING, &m_strBookContent, false},
        {USERCONTENT, XNDT_STRING, &m_strUserContent, false},
        {CHAPTERTITLE, XNDT_STRING, &m_strChapterTitle, false},
        {BEGINPOS, XNDT_COMPLEX, &m_clsBeginPos, false, CT_RefPos::WriteToXMLFunc},
        {ENDPOS, XNDT_COMPLEX, &m_clsEndPos, false, CT_RefPos::WriteToXMLFunc},
        {UPLOADED, XNDT_INT, &m_uploaded, false},
        {PERCENT, XNDT_INT, &m_percent, false},
        {COMICSFRAMEMODE, XNDT_INT, &m_comicsFrame, false},
    END_XML_STORE_DATA;
    return XmlUtil::BuildXmlNode(&node, storeData);
}

bool CT_ReadingDataItemImpl::ReadFromXml(const XMLDomNode& node)
{
    string typeName;
    BEGIN_XML_PARSE_DATA(parseData)
        {USERDATATYPE, XNDT_STRING, &typeName, false},
        {KERNELVERSION, XNDT_STRING, &m_strKernelVersion, false},
        {COLOR, XNDT_STRING, &m_color, false},
        {TAG, XNDT_STRING, &m_tag, false},
        {USERDATAID, XNDT_STRING, &m_strID, false},
        {CREATETIME, XNDT_STRING, &m_strCreateTime, false},
        {LASTMODTIME, XNDT_STRING, &m_strLastModifyTime, false},
        {CREATOR, XNDT_STRING, &m_strCreator, false},
        {BOOKCONTENT, XNDT_STRING, &m_strBookContent, false},
        {USERCONTENT, XNDT_STRING, &m_strUserContent, false},
        {CHAPTERTITLE, XNDT_STRING, &m_strChapterTitle, false},
        {BEGINPOS, XNDT_COMPLEX, &m_clsBeginPos, false, CT_RefPos::ReadFromXmlFunc},
        {ENDPOS, XNDT_COMPLEX, &m_clsEndPos, false, CT_RefPos::ReadFromXmlFunc},
        {UPLOADED, XNDT_INT, &m_uploaded, false},
        {PERCENT, XNDT_INT, &m_percent, false},
        {COMICSFRAMEMODE, XNDT_INT, &m_comicsFrame,false},
    END_XML_PARSE_DATA;
    XmlUtil::ParseXmlNode(&node, parseData);
    for (size_t i = 0; i < ICT_ReadingDataItem::TYPECOUNT; ++i)
    {
        if (strcmp(typeName.c_str(), g_strTypeName[i].c_str()) == 0)
        {
            m_eType = (ICT_ReadingDataItem::UserDataType)i;
        }
    }
    return true;
}

