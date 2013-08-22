////////////////////////////////////////////////////////////////////////
// CBookItemModelImpl.h
// Contact: zhangxb
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __TAGITEMMODELIMPL_H__
#define __TAGITEMMODELIMPL_H__

#include "Model/ITagItemModel.h"

class CTagItemModelImpl : public ITagItemModel
{
public:
    CTagItemModelImpl(int iTagIndex) : m_iTagIndex(iTagIndex) {}

    virtual ~CTagItemModelImpl() {}

    virtual LPCSTR GetTagName();
    virtual INT32  GetFileCount();
    virtual int      GetTagId();

    void SetTagIndex(INT32 iTagIndex)
    {
        m_iTagIndex = iTagIndex;
    }

    INT32 GetTagIndex()
    {
        return m_iTagIndex;
    }

private:
    int m_iTagIndex;

};

typedef CTagItemModelImpl* PCTagItemModelImpl;
#endif  // __TAGITEMMODELIMPL_H__



