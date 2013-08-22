///////////////////////////////////////////////////////////////////////
// dk_chst2cp.h
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////
#ifndef _IDK_CHARSET2CODEPAGE_H_
#define _IDK_CHARSET2CODEPAGE_H_

#include "KernelType.h"
#include "KernelBaseType.h"

typedef struct _tagDK_CHST2CP
{
    DK_CHARSET_TYPE eCharset;    // 当前字符集
    DK_UINT16       uCodepage;   // 当前字符集对应的codepage
} DK_CHST2CP;

#define CODEPAGE_NOT_EXIST  0xFFFF

extern DK_UINT16 DK_GetCPByChst(DK_CHARSET_TYPE eCharset);
extern DK_CHARSET_TYPE DK_GetChstByCP(DK_UINT16 uCodepage);

#endif // _IDK_CHARSET2CODEPAGE_H_


