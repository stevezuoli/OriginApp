////////////////////////////////////////////////////////////////////////
// CBookmarkItemModelImpl.h
// Contact: zhangjd
// Copyright (c) Duokan Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////

#ifndef __BOOKMARKITEMMODELIMPL_H__
#define __BOOKMARKITEMMODELIMPL_H__

#include "Model/IBookmarkItemModel.h"
#include "Bookmark.h"
#include "DKXManager/ReadingInfo/ICT_ReadingDataItem.h"

class CBookmarkItemModelImpl : public IBookmarkItemModel
{
public:
    CBookmarkItemModelImpl(ICT_ReadingDataItem *_pclsReadingItem, int _iFileId, int _iBookmarkIndex);
    virtual ~CBookmarkItemModelImpl();

    virtual ICT_ReadingDataItem::UserDataType GetBookmarkType();
    virtual const string GetCreateTime();
    virtual const string GetLastModifyTime();
    virtual const string GetCreator();
    virtual const string  GetBookRefContent();
    virtual const string GetUserComment();
    virtual const CT_RefPos GetStartPos();
    virtual const CT_RefPos GetEndPos();
    virtual ICT_ReadingDataItem *GetBookmark();
    virtual int GetDkpFileId();
	virtual int GetBookmarkIndex() const;

private:
    ICT_ReadingDataItem *m_pclsBookMark;
    string    m_strCreateTime;
    string    m_strLastModifyTime;
    string    m_strCreator;
    string    m_strBookRefContent;
    string    m_strUserComment;
    CT_RefPos m_clsStartPos;
    CT_RefPos m_clsEndPos;
    int m_iFileID;
	int m_iBookmarkIndex;
};

#endif  // __BOOKMARKITEMMODELIMPL_H__



