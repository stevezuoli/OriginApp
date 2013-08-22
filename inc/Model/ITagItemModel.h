////////////////////////////////////////////////////////////////////////
// IBookItemModel.h
// Contact: zhangxb
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __IBOOKTAGITEMMODEL_H__
#define __IBOOKTAGITEMMODEL_H__

#include "dkBaseType.h"

class ITagItemModel
{
public:
    ITagItemModel() {}
    virtual ~ITagItemModel() {}
    virtual LPCSTR GetTagName()=0;
    virtual INT32  GetFileCount()=0;
    virtual int       GetTagId()=0;

};

#endif  // __IBOOKTAGITEMMODEL_H__


