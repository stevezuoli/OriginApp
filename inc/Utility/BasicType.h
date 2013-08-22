////////////////////////////////////////////////////////////////////////
// BasicType.h
// Contact: wzh
// Copyright (c) Duokan Corporation. All rights reserved.
// 
// Basic type definations
////////////////////////////////////////////////////////////////////////

#ifndef __BASICTYPE_H__
#define __BASICTYPE_H__

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "dkBaseType.h"

#define UNUSED(x) (void(x))

struct SByteArray
{
    SByteArray() :
        m_pBuf(NULL), m_iLen(0)
    {
    }

    SByteArray(UINT8* pBuf, INT32 iLen) :
        m_pBuf(pBuf), m_iLen(iLen)
    {
    }

    UINT8* m_pBuf;
    INT32 m_iLen;

    // This method is only for test purpose
    bool operator==(const SByteArray& rByteArray) const
    {
        if (m_iLen == rByteArray.m_iLen)
        {
            for (INT32 i = 0; i < m_iLen; i++)
            {
                if (m_pBuf[i] != rByteArray.m_pBuf[i])
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
};

class CString;
class UIWindow;
class UIMenu;

extern void TPAssert(const INT8* pszFileName, INT32 iLineNumber, const INT8* pszExpression);
// Implementation of TPPrintLn will be put in CTrace.cpp
extern void TPPrintLn(const INT8* pszFileName, const INT8* pszText);
// Implementation of TPPerformance will be put in CTrace.cpp
//extern void TPPerformance(const CString& rstrCategory, const CString& rContext);

//#define PERFORMANCE(x, y) TPPerformance(x, y)
#define PERFORMANCE(x, y) 

#if 0 // def _DEBUG
#define TRACE(x) TPPrintLn(__FILE__, x)
#define TP_ASSERT(e) ((e) ? (void)0 : TPAssert(__FILE__, __LINE__, #e))
#else
#define TRACE(x)
#define TP_ASSERT(e)
#endif

// note, when using THROW macro, '{}' is required for if statement with only throw cause
#define THROW TRACE(CString("---------------Line ") + CString(__LINE__) + CString(" issues an exception---------------")); \
    throw

#endif
