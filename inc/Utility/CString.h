////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/porting/inc/CString.h $ 
// $Revision: 16 $
// Contact: yuzhan
// Latest submission: $Date: 2008/06/18 19:22:14 $ by $Author: fareast\\xzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
// 
//
////////////////////////////////////////////////////////////////////////

#ifndef __STRING_H__
#define __STRING_H__

#include "BasicType.h"

// TODO: 我们假定所有构造函数中传进来的INT8数据都是UTF8编码

// CString中按照UTF8来保存
class CString
{
public:
    CString();

    CString(LPCSTR pData, INT32 iLen);

    CString(LPCSTR pszString);

    CString(INT32 iVal);
    CString(INT32 iVal, const char *format);

	CString(const wchar_t* pData);
	CString(const wchar_t* pData, INT32 iLen);

    CString(const CString&);

    ~CString();

    UINT32 Length() const;
    CString SubString(UINT32 iOffset, UINT32 iLength) const;

    CString operator +(const CString&) const;
    BOOL operator==(const CString&) const;
    BOOL operator!=(const CString&) const;
    LPCSTR GetBuffer() const;

    const CString& operator +=(const CString& strSrcString);
    const CString& operator +=(LPCSTR csSrc);
    
    const CString& operator =(LPCSTR csSrc);
    const CString& operator =( const CString&);    

    UINT32 GetChar(UINT32 ioffset)
    {
        ioffset = 0;
        return 0;
    }
    operator LPCSTR () const throw()
    {
        return( m_pStringImpl );
    }

    static CString EMPTY_STRING;

private:
    LPSTR m_pStringImpl;
    INT32 m_iLength;
};

#endif
