////////////////////////////////////////////////////////////////////////
// IBookmarkItemModel.h
// Contact: zhangjd
// Copyright (c) Duokan Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////

#ifndef __IBOOKMARKITEMMODEL_H__
#define __IBOOKMARKITEMMODEL_H__

#include <string>
#include <vector>
#include <tr1/memory>
#include "dkWinError.h"
#include "DKXManager/ReadingInfo/ICT_ReadingDataItem.h"

using std::string;

class IBookmarkItemModel
{
public:
    IBookmarkItemModel() {}
    virtual ~IBookmarkItemModel() {}

    virtual ICT_ReadingDataItem::UserDataType GetBookmarkType()=0;
    virtual const string GetCreateTime()=0;
    virtual const string GetLastModifyTime()=0;
    virtual const string GetCreator()=0;
    virtual const string  GetBookRefContent() = 0;
    virtual const string GetUserComment() = 0;
    virtual const CT_RefPos GetStartPos() = 0;
    virtual const CT_RefPos GetEndPos() = 0;
    virtual ICT_ReadingDataItem *GetBookmark()=0;
    virtual int GetDkpFileId()=0;
	virtual int GetBookmarkIndex()const =0;
};
typedef std::tr1::shared_ptr<IBookmarkItemModel> IBookmarkItemModelSPtr;
typedef std::vector<IBookmarkItemModelSPtr> IBookmarkItemModelSPtrList;

#endif  // __IBOOKMARKITEMMODEL_H__


