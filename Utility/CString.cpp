////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/src/Utility/CString.cpp $ 
// $Revision: 20 $
// Contact: yuzhan
// Latest submission: $Date: 2008/08/25 20:50:27 $ by $Author: fareast\\xzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
// 
//
////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "Utility/CString.h"
#include "Utility.h"
#include "dkBaseType.h"
#include "interface.h"
#include "Common/DebugConsole.h"
#include "../../DkReader/Common/DKLogger.h"

using namespace DkFormat;

static __inline int wstrlen(const WCHAR *wstr)
{
	const WCHAR *t=wstr;
	while(*t++);
	return (t - wstr - 1);
}

CString CString::EMPTY_STRING("");

CString::CString()
    : m_pStringImpl(NULL)
    , m_iLength(0)
{
}

CString::CString(INT32 iVal)
    : m_pStringImpl(NULL)
    , m_iLength(0)
{
    // just allocate enough space for any 32-bit decimal
    static const UINT32 KPossibleDecimalLength = 12;
    
    LPSTR pBuf = new CHAR[KPossibleDecimalLength];
    if (NULL == pBuf)
    {
      return;
    }
    
    memset(pBuf, 0, KPossibleDecimalLength);
    sprintf(pBuf, "%d", iVal);
    m_pStringImpl = pBuf;
    m_iLength = strlen(pBuf);
}


CString::CString(INT32 iVal, const char *format)
    : m_pStringImpl(NULL)
    , m_iLength(0)
{
    // just allocate enough space for any 32-bit decimal
    static const UINT32 KPossibleDecimalLength = 12;
    
    LPSTR pBuf = new CHAR[KPossibleDecimalLength];
    if (NULL == pBuf)
    {
      return;
    }
    
    memset(pBuf, 0, KPossibleDecimalLength);
    sprintf(pBuf, format, iVal);
    m_pStringImpl = pBuf;
    m_iLength = strlen(pBuf);
}


CString::CString(const CString& rSrc)
    : m_pStringImpl(NULL)
    , m_iLength(0)
{  
    if (rSrc.m_pStringImpl == NULL)
    {
       return;
    }
    
    // get length of source string
    LPSTR pBuf = new CHAR[rSrc.m_iLength + 1];
    if (NULL == pBuf)
    {
      return;
    }
    
    m_iLength = rSrc.m_iLength;
    if (m_iLength > 0)
    {
       memcpy(pBuf, rSrc.m_pStringImpl, m_iLength);
    }
    
    pBuf[m_iLength] = 0;
    m_pStringImpl = pBuf;
}


CString::CString(LPCSTR pData, INT32 iLen)
    : m_pStringImpl(NULL)
    , m_iLength(0)
{
    if (NULL == pData)
    {
        return;
    }

    if (iLen < 0)
    {
        return;
    }

    LPSTR pBuf = new CHAR[iLen + 1];
    if (NULL == pBuf)
    {
      return;
    }
    
    m_iLength = iLen;
    if (m_iLength > 0)
    {
       memcpy(pBuf, pData, m_iLength);
    }
    pBuf[m_iLength] = 0;
    m_pStringImpl = pBuf;
}

CString::CString(LPCSTR pszString)
    : m_pStringImpl(NULL)
    , m_iLength(0)
{
    if (NULL == pszString)
    {
       return;
    }

    // get length of source string
    INT32 iLen = strlen(pszString);
    LPSTR pBuf = new CHAR[iLen + 1];
    if (NULL == pBuf)
    {
      return;
    }
    
    m_iLength = iLen;
    if (m_iLength > 0)
    {
        memcpy(pBuf, pszString, m_iLength);
    }
    pBuf[m_iLength] = 0;
    m_pStringImpl = pBuf;
}

CString::~CString()
{
    if (NULL != m_pStringImpl)
    {
        delete[] m_pStringImpl;  
        m_pStringImpl = NULL;          
    }    
}

UINT32 CString::Length() const
{
    return m_iLength;
}

CString CString::SubString(UINT32 iOffset, UINT32 iLength) const
{
    CString dstString;
    if ((UINT32)m_iLength < iOffset + iLength)
    {
        // FIXME: add exact exception number later
        DebugLog(DLC_ERROR, "CString::SubString()");
        return dstString;
    }
    
    LPSTR pBuf = new CHAR[iLength + 1];
    if (NULL == pBuf)
    {
      return dstString;
    }
    
    if (iLength > 0)
    {
       memcpy(pBuf, m_pStringImpl + iOffset, iLength);
    }
    
    pBuf[iLength] = 0;
    dstString.m_pStringImpl = pBuf;
    dstString.m_iLength = iLength;
    return dstString;
}

const CString&  CString::operator =( const CString& rSrc)    
{  
    if (&rSrc != this)
    {
        LPCSTR buf = rSrc.GetBuffer();
        if (NULL != buf)
        {
            INT32 srcLen = rSrc.Length();
            LPSTR pBuf = new CHAR[srcLen + 1];
            if (NULL == pBuf)
            {
                return (*this);
            }
         
            if (srcLen > 0)
            {
                memcpy(pBuf, buf, srcLen);
            }
            
            pBuf[srcLen] = 0;
            
            if (NULL != m_pStringImpl)
            {
                delete[] m_pStringImpl;  
                m_pStringImpl = NULL;   
            }  
            
            m_pStringImpl = pBuf;
            m_iLength = srcLen;
        }
        else
        {
            if (NULL != m_pStringImpl)
            {
                delete[] m_pStringImpl;  
                m_pStringImpl = NULL;   
            }                   
            m_iLength = 0;
        }
    }

    return *this;
}

CString CString::operator +(const CString& rSrc1) const
{
    CString resultString;
    INT32 iLen0 = 0;
    INT32 iLen1 = 0;
    BOOL fHasContent = FALSE;
    if (m_pStringImpl != NULL)
    {
        iLen0 = m_iLength;
        fHasContent = TRUE;
    }
    
    LPCSTR buf = rSrc1.GetBuffer();
    if (buf != NULL)
    {
        iLen1 = rSrc1.m_iLength;
        fHasContent = TRUE;
    }
    
    if (fHasContent)
    {
        LPSTR  pBuf = new CHAR[iLen0 + iLen1 + 1];
     if (NULL == pBuf)
     {
        return resultString;
     }
     
        if (iLen0 > 0)
        {
            memcpy(pBuf, m_pStringImpl, iLen0);
        }
        
        if (iLen1 > 0)
        {
            memcpy(pBuf + iLen0, buf, iLen1);
        }
        
        pBuf[iLen0 + iLen1] = 0;
        resultString.m_pStringImpl = pBuf; 
        resultString.m_iLength = iLen0 + iLen1;
    }
      DebugPrintf(DLC_STRING, "m_pStringImpl = %s, m_iLength = %d", resultString.m_pStringImpl, resultString.m_iLength);
  
    return resultString;
}

BOOL CString::operator==(const CString& rSrc) const
{
     if (NULL == m_pStringImpl && NULL == rSrc.m_pStringImpl)
     {
         return TRUE;
     }
     
     if (NULL == m_pStringImpl || NULL == rSrc.m_pStringImpl)
     {
         return FALSE;
     }
     
     LPSTR  buf = rSrc.m_pStringImpl;
     INT32 iLen0 = m_iLength;
     INT32 iLen1 = rSrc.m_iLength;
     if (iLen0 != iLen1)
     {
         return FALSE;
     }

     LPSTR  selfBuf = m_pStringImpl;
     for (INT32 i = 0; i < iLen0; i++)
     {
         if (selfBuf[i] != buf[i])
         {
             return FALSE;
         }
     }
     
     return TRUE;
}

BOOL CString::operator!=(const CString& rSrc) const
{
    return (!this->operator ==(rSrc));
}

LPCSTR CString::GetBuffer() const
{
    return m_pStringImpl;
}

const CString& CString::operator +=(const CString& strSrcString)
{
    if (&strSrcString != this)
    {
        int iThisLength = this->Length();
        int iSrcLength = strSrcString.Length();
        int iTotalength = 0;
        LPSTR pBuf = NULL;
     iTotalength = iThisLength + iSrcLength;
     if(iTotalength > 0)
     {
         pBuf = new CHAR[iTotalength + 1];
     }
     if(NULL == pBuf)
         return *this;
     else
        memset(pBuf, 0, iTotalength + 1);
     
     if(this->m_pStringImpl)
            memcpy(pBuf, this->m_pStringImpl, iThisLength);
     
        LPCSTR buf = strSrcString.GetBuffer();
        if (NULL != buf)
            memcpy(pBuf + iThisLength, buf, iSrcLength);
     if(this->m_pStringImpl)    
           delete [] this->m_pStringImpl;
        this->m_pStringImpl = pBuf;
        this->m_iLength = iTotalength;
    }    
    DebugPrintf(DLC_STRING, "m_pStringImpl = %s, m_iLength = %d", m_pStringImpl, m_iLength);

    return *this;
}

const CString& CString::operator +=(LPCSTR csSrc)
{
    if (NULL == csSrc)
    {
        return *this;
    }

    // get length of source string
    int iThisLength = this->Length();
    int iSrcLength = strlen(csSrc);
    int iTotalength = 0;
    LPSTR pBuf = NULL;
    iTotalength = iThisLength + iSrcLength;

    if(iTotalength > 0)
    {
        pBuf = new CHAR[iTotalength + 1];
    }
    if(NULL == pBuf)
        return *this;
    else
        memset(pBuf, 0, iTotalength + 1);

    if(this->m_pStringImpl)
        memcpy(pBuf, this->m_pStringImpl, iThisLength);
    memcpy(pBuf + iThisLength, csSrc, iSrcLength);
    if(this->m_pStringImpl)    
        delete [] this->m_pStringImpl;
    this->m_pStringImpl = pBuf;
    this->m_iLength = iTotalength;
    DebugPrintf(DLC_STRING, "m_pStringImpl = %s, m_iLength = %d", m_pStringImpl, m_iLength);

    return *this;
}

const CString& CString::operator =(LPCSTR csSrc)
{
    if (NULL == csSrc)
    {
        return *this;
    }

    int iSrcLength = strlen(csSrc);
    LPSTR pBuf = NULL;
//    iTotalength = iThisLength + iSrcLength;

    if(iSrcLength > 0)
    {
        pBuf = new CHAR[iSrcLength + 1];
    }
    if(NULL == pBuf)
        return *this;
    else
        memset(pBuf, 0, iSrcLength + 1);

    memcpy(pBuf, csSrc, iSrcLength);
    if(this->m_pStringImpl)    
        delete [] this->m_pStringImpl;
    this->m_pStringImpl = pBuf;
    this->m_iLength = iSrcLength;
    DebugPrintf(DLC_STRING, "m_pStringImpl = %s, m_iLength = %d", m_pStringImpl, m_iLength);
  
    return *this;
}


/* Cstring≤…”√ utf-8 ±‡¬Î */
CString::CString(const wchar_t* pData, INT32 iWLen)
	: m_pStringImpl(NULL)
	, m_iLength(0)
{
	if (NULL == pData)
	{
		return;
	}

	if (iWLen < 0)
	{
		return;
	}

	int len = 3 * iWLen + 1;
	unsigned long clen = 0;

	LPSTR pBuf = new CHAR[len];
	if (NULL == pBuf)
	{
		return;
	}

	memset(pBuf, 0 , len);
	if(iWLen != 0)
    {
		CUtility::ConvertEncoding(DK_CHARSET_UTF8, DK_CHARSET_UTF32_LE,  (const char *)pData, iWLen*sizeof(wchar_t),  pBuf, len, &clen, TRUE, FALSE );
    }
	m_iLength = clen;
	pBuf[m_iLength] = 0;
	m_pStringImpl = pBuf;
}

CString::CString(const wchar_t* pData)
	: m_pStringImpl(NULL)
	, m_iLength(0)
{
	if (NULL == pData)
	{
		return;
	}

	// get length of source string
	INT32 iWLen = wcslen(pData);
	int len = 3 * iWLen + 1;
	unsigned long clen = 0;
	LPSTR pBuf = new CHAR[len];
	if (NULL == pBuf)
	{
		return;
	}
	memset(pBuf, 0 , len);
	if(iWLen != 0)
    {
		CUtility::ConvertEncoding(DK_CHARSET_UTF8, DK_CHARSET_UTF32_LE,  (const char *)pData, iWLen*sizeof(wchar_t),  pBuf, len, &clen, TRUE, FALSE );
    }
	m_iLength = clen;
	pBuf[m_iLength] = 0;
	m_pStringImpl = pBuf;
}

