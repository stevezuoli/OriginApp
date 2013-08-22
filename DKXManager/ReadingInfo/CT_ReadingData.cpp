
#include "DKXManager/ReadingInfo/CT_ReadingData.h"
#include <algorithm>


using namespace std;

const string CT_ReadingData::VERSION = "Version";
const string CT_ReadingData::BOOKMARK = "ReadingDataItem";
const string CT_ReadingData::COMMENT = "Comment";
const string CT_ReadingData::DIGEST = "Digest";
const string CT_ReadingData::PROGRESS = "Progress";
const string CT_ReadingData::USERNAME = "UserName";
const string CT_ReadingData::MODIFIED = "Modified";
const string CT_ReadingData::DATA_VERSION = "DataVersion"; 

CT_ReadingData::CT_ReadingData()
    : m_strVersion("")
    , m_Progress()
    , m_modified(false)
    , m_dataVersion(0)
{
    m_vBookmarks.clear();
}

CT_ReadingData::~CT_ReadingData()
{
    m_vBookmarks.clear();
}

string CT_ReadingData::GetVersion() const
{
    return m_strVersion;
}

bool CT_ReadingData::SetVersion(const string &_strVersion)
{
    m_strVersion = _strVersion;
    return true;
}

int CT_ReadingData::GetBookmarkCount() const
{
    return m_vBookmarks.size();
}

bool CT_ReadingData::AddBookmark(const CT_ReadingDataItemImpl &_bookmark)
{
    m_vBookmarks.push_back(_bookmark);
    (m_vBookmarks.begin() + (m_vBookmarks.size() -1))->CreateId();
    return true;
}
bool CT_ReadingData::RemoveBookmark(int _iIndex)
{
    if(_iIndex >= (int)m_vBookmarks.size() || _iIndex < 0)
    {
        return false;
    }
    m_vBookmarks.erase(m_vBookmarks.begin() + _iIndex);
    return true;
}

bool CT_ReadingData::RemoveBookmark(const std::string& dataID)
{
    for (size_t i = 0; i < m_vBookmarks.size(); ++i)
    {
        if (strcmp(m_vBookmarks.at(i).GetId().c_str(), dataID.c_str()) == 0)
        {
            m_vBookmarks.erase(m_vBookmarks.begin() + i);
            return true;
        }
    }
    return false;
}

void CT_ReadingData::RemoveUploadedBookmarks()
{
    DK_AUTO(newEnd,
            std::remove_if(m_vBookmarks.begin(), m_vBookmarks.end(), std::mem_fun_ref(&CT_ReadingDataItemImpl::IsUploaded)));
    m_vBookmarks.erase(newEnd, m_vBookmarks.end());
}

int CT_ReadingData::GetDataVersion() const
{
    return m_dataVersion;
}

bool CT_ReadingData::SetDataVersion(int dataVersion)
{
    m_dataVersion = dataVersion;
    return true;
}

//bool CT_ReadingData::RemoveBookmark(const CT_SimpleRefContent &_SimRefContent)
//{
//    vector<CT_UserDataItemImpl>::iterator itr = std::find(m_vBookmarks.begin(),m_vBookmarks.end(),_SimRefContent);
//    if(itr == m_vBookmarks.end())
//    {
//        return false;
//    }
//    m_vBookmarks.erase(itr);
//    return true;
//}

bool CT_ReadingData::GetBookmarkByIndex(int _iIndex,CT_ReadingDataItemImpl *_pclsMark) const
{
    if(_iIndex < 0 || _iIndex >= (int)m_vBookmarks.size() || NULL == _pclsMark)
    {
        return false;
    }
    *_pclsMark = m_vBookmarks[_iIndex];
    assert(strcmp(_pclsMark->GetColor().c_str(), m_vBookmarks[_iIndex].GetColor().c_str()) == 0);
    return true;
}



bool CT_ReadingData::SetProgress(const CT_ReadingDataItemImpl &_Progress)
{
    m_Progress = _Progress;
    m_Progress.CreateId();
    return true;
}

bool CT_ReadingData::GetProgress(CT_ReadingDataItemImpl *_pclsProgress) const
{
    if(NULL == _pclsProgress)
    {
        return false;
    }
    *_pclsProgress = m_Progress;
    return true;
}


bool CT_ReadingData::WriteToXML(XMLDomNode& _xmlNode) const
{
    XMLDomNode TmpNode;
    _xmlNode.AddAttribute(USERNAME.c_str(), GetUser().c_str());

    stringstream spd;
    spd.clear();
    spd.str("");
    spd << m_modified;
    AddAtomSubNode(_xmlNode, MODIFIED, spd.str());

    spd.clear();
    spd.str("");
    spd << m_dataVersion;
    AddAtomSubNode(_xmlNode, DATA_VERSION, spd.str());

    // add reading data version
    AddAtomSubNode(_xmlNode,VERSION,m_strVersion);

    // add book mark
    for(vector<CT_ReadingDataItemImpl>::const_iterator itr = m_vBookmarks.begin();itr != m_vBookmarks.end();itr++)
    {
        AddCommentNodeToXML(BOOKMARK,_xmlNode);
        DK_ReturnCode rc = _xmlNode.GetLastChild(&TmpNode);
        if(rc == DKR_OK)
        {
            itr->WriteToXML(TmpNode);
        }
    }

    // add progress
    AddCommentNodeToXML(PROGRESS,_xmlNode);
    DK_ReturnCode rc = _xmlNode.GetLastChild(&TmpNode);
    if(rc == DKR_OK)
    {
        m_Progress.WriteToXML(TmpNode);
    }

    return true;
}

bool CT_ReadingData::ReadFromXml(const XMLDomNode &_xmlNode)
{
    XMLDomNode TmpNode;
    const char* user = _xmlNode.GetAttribute("UserName");
    if (NULL != user)
    {
        m_user = user;
    }
    if(DKR_OK == _xmlNode.GetFirstElementChild(&TmpNode))
    {
        do
        {
            if(strcmp(TmpNode.GetNodeValue(),VERSION.c_str()) == 0)
            {
                ReadAtomNodeValue(TmpNode,m_strVersion);
            }
            else if (strcmp(TmpNode.GetNodeValue(), DATA_VERSION.c_str()) == 0)
            {
                string tmp;
                ReadAtomNodeValue(TmpNode, tmp);
                m_dataVersion = atoi(tmp.c_str());
            }
            else if (strcmp(TmpNode.GetNodeValue(), MODIFIED.c_str()) == 0)
            {
                string tmp;
                ReadAtomNodeValue(TmpNode, tmp);
                m_modified = atoi(tmp.c_str()) != 0;
            }
            else if(strcmp(TmpNode.GetNodeValue(),BOOKMARK.c_str()) == 0)
            {
                CT_ReadingDataItemImpl tmpMark;
                tmpMark.ReadFromXml(TmpNode);
                m_vBookmarks.push_back(tmpMark);
            }
            else if(strcmp(TmpNode.GetNodeValue(),PROGRESS.c_str()) == 0)
            {
                m_Progress.ReadFromXml(TmpNode);
            }
        }while(TmpNode.GetNextSibling(&TmpNode) == DKR_OK);
    }

    return true;
}

void CT_ReadingData::MarkAsUploaded(bool uploaded)
{
    m_Progress.SetUploaded(uploaded);
    for (size_t i = 0; i < m_vBookmarks.size(); ++i)
    {
        m_vBookmarks[i].SetUploaded(uploaded);
    }
}

bool CT_ReadingData::MigrateReadingData(const CT_ReadingData& readingData)
{
	vector<string> bookMarkIds;
	for(vector<CT_ReadingDataItemImpl>::iterator itr = m_vBookmarks.begin(); itr != m_vBookmarks.end(); itr++)
	{
		bookMarkIds.push_back(itr->GetId());
	}
	for(int i = 0; i < readingData.GetBookmarkCount(); i++)
	{
		CT_ReadingDataItemImpl bookMarkItem;
		if(readingData.GetBookmarkByIndex(i, &bookMarkItem))
		{
			if(std::find(bookMarkIds.begin(), bookMarkIds.end(), bookMarkItem.GetId()) == bookMarkIds.end())
			{
				m_vBookmarks.push_back(bookMarkItem);
			}
		}
	}
	CT_ReadingDataItemImpl progressItem;
	if(readingData.GetProgress(&progressItem))
	{
		m_Progress = progressItem;
	}
	return false;
}
