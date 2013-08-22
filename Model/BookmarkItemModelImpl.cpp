////////////////////////////////////////////////////////////////////////
// CBookmarkItemModelImpl.cpp
// Contact: zhangjd
// Copyright (c) Duokan Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////

#include "Model/BookmarkItemModelImpl.h"
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include <stdio.h>
#include <stdlib.h>

static void ConvertTimeToNormal(string* s)
{
    size_t spacePos = s->find('T');
    if (spacePos != string::npos)
    {
        s->replace(spacePos, 1, 1, ' ');
    }
}

CBookmarkItemModelImpl::CBookmarkItemModelImpl(ICT_ReadingDataItem *_pclsReadingItem, int _iFileID, int _iBookmarkIndex)
: m_pclsBookMark(_pclsReadingItem)
, m_iFileID(_iFileID)
, m_iBookmarkIndex(_iBookmarkIndex)
{
    if(!m_pclsBookMark || m_iFileID < 0)
    {
        return;
    }
    m_strCreateTime = m_pclsBookMark->GetCreateTime();
    ConvertTimeToNormal(&m_strCreateTime);
    m_strCreator = m_pclsBookMark->GetCreator();
    m_strLastModifyTime = m_pclsBookMark->GetLastModifyTime();
    ConvertTimeToNormal(&m_strLastModifyTime);
    m_strBookRefContent = m_pclsBookMark->GetBookContent();
    m_strUserComment = m_pclsBookMark->GetUserContent();
    m_clsStartPos = m_pclsBookMark->GetBeginPos();
    m_clsEndPos = m_pclsBookMark->GetEndPos();
}
CBookmarkItemModelImpl::~CBookmarkItemModelImpl()
{
    if (NULL != m_pclsBookMark)
    {
        DKXReadingDataItemFactory::DestoryReadingDataItem(m_pclsBookMark);
    }
}

int CBookmarkItemModelImpl::GetBookmarkIndex() const
{
	return m_iBookmarkIndex;
}


ICT_ReadingDataItem::UserDataType CBookmarkItemModelImpl::GetBookmarkType()
{
    if(!m_pclsBookMark)
    {
        return ICT_ReadingDataItem::UNKNOWN;
    }
    return m_pclsBookMark->GetUserDataType();
}
const string CBookmarkItemModelImpl::GetCreateTime()
{
    return m_strCreateTime;
}
const string CBookmarkItemModelImpl::GetLastModifyTime()
{
    return m_strLastModifyTime;
}
const string CBookmarkItemModelImpl::GetCreator()
{
    return m_strCreator;
}
const string  CBookmarkItemModelImpl::GetBookRefContent()
{
    return m_strBookRefContent;
}
const string CBookmarkItemModelImpl::GetUserComment()
{
    return m_strUserComment;
}
const CT_RefPos CBookmarkItemModelImpl::GetStartPos()
{
    return m_clsStartPos;
}
const CT_RefPos CBookmarkItemModelImpl::GetEndPos()
{
    return m_clsEndPos;
}
ICT_ReadingDataItem *CBookmarkItemModelImpl::GetBookmark()
{
    return m_pclsBookMark;
}
int CBookmarkItemModelImpl::GetDkpFileId()
{
    return m_iFileID;
}

