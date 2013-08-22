////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/porting/inc/CStringW.h $ 
// $Revision: 16 $
// Contact: yuzhan
// Latest submission: $Date: 2008/06/18 19:22:14 $ by $Author: fareast\\xzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
// 
//
////////////////////////////////////////////////////////////////////////

#ifndef __CSTRINGW_H__
#define __CSTRINGW_H__

#include <wchar.h>
#include "BasicType.h"


// CStringW中按照UTF16来保存
class CStringW
{
public:
    CStringW();

    // 输入为UTF16
    CStringW(const WCHAR* pData, UINT32 nLength);

    // 输入为UTF16
    CStringW(const WCHAR* pszString);

    // 输入为UTF16
    CStringW(const wchar_t* pszString);

    CStringW(INT32 iVal);
    CStringW(const CStringW&);

    ~CStringW();

    // the string length by character
    UINT32 Length() const
    {
        return m_nLength;
    }

    WCHAR GetChar(UINT32 nIndex) const;
    CStringW SubString(UINT32 iOffset, UINT32 nLength) const;
    CStringW& operator =(const CStringW&);
    CStringW operator +(const CStringW&) const;
    BOOL operator==(const CStringW&) const;
    BOOL operator!=(const CStringW&) const;

    operator const WCHAR * () const throw()
    {
        return m_pStringImpl;
    }

    static CStringW EMPTY_STRING;

private:
    void CreateFromBuffer(const WCHAR* pData, UINT32 nLength);
    static UINT32 GetStringLengthW(const WCHAR* pszString);
    
    WCHAR *m_pStringImpl;
    UINT32 m_nLength;
};

#endif
