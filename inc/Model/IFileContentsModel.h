////////////////////////////////////////////////////////////////////////
// IFileContentsModel.h
// Contact: wangh
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __IFILECONTENTSMODEL_H__
#define __IFILECONTENTSMODEL_H__

#include "dkBaseType.h"
#include "dkBuffer.h"
#include "SequentialStream.h"
#include "DkSPtr.h"
#include <string>
#include <vector>
#include "Common/IFileChapterInfo.h"

using namespace std;

class IFileContentsModel
{
public:
    IFileContentsModel() {}
    virtual ~IFileContentsModel() {}

    virtual BOOL    HasPassword()=0;
    virtual INT32    GetChapterNum()=0;
    virtual void    SetPassword(LPCSTR StrPwd)=0;
    virtual BOOL    GetChapterList(vector <CFileChapterInfo> & ChapterList)=0;

    // TODO: add more
};

#endif  // __IFILECONTENTSMODEL_H__


