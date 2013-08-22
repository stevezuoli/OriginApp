////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/src/Utility/CStringW.cpp $ 
// $Revision: 20 $
// Contact: yuzhan
// Latest submission: $Date: 2008/08/25 20:50:27 $ by $Author: fareast\\xzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
// 
//
////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "Utility/CStringW.h"
#include "Utility.h"
#include "dkBaseType.h"
#include "interface.h"
#include "../Common/DebugConsole.h"
#include "../../DkReader/Common/DKLogger.h"

using namespace DkFormat;

CStringW CStringW::EMPTY_STRING((const WCHAR *)L"");

CStringW::CStringW()
    : m_pStringImpl(NULL)
    , m_nLength(0)
{
}


CStringW::CStringW(INT32 iVal)
    : m_pStringImpl(NULL)
    , m_nLength(0)
{
    BOOL fIsNegative = FALSE;
    UINT32 nLength = 0;
    if (iVal < 0)
    {
        nLength++;
        fIsNegative = TRUE;
        iVal = -iVal;
    }

    INT32 i = iVal;
    do
    {
        nLength++;
        i /= 10;
    } while  (i > 0);

    m_pStringImpl = new WCHAR[nLength + 1];
    if (NULL == m_pStringImpl)
    {
        return;
    }
    
    m_nLength = nLength;
    memset(m_pStringImpl, 0, sizeof(WCHAR) * (nLength + 1));

    INT32 iIndex = nLength - 1;
    i = iVal;
    do 
    {
        m_pStringImpl[iIndex--] = (i % 10 + '0');
        i /= 10;
    } while (i > 0 && iIndex >= 0);

    if (fIsNegative)
    {
        m_pStringImpl[0] = '-';
    }
}


CStringW::CStringW(const CStringW& rSrc)
    : m_pStringImpl(NULL)
    , m_nLength(0)
{
    if (NULL != rSrc.m_pStringImpl)
    {
        CreateFromBuffer(rSrc.m_pStringImpl, rSrc.m_nLength);
    }
}


CStringW::CStringW(const WCHAR* pData, UINT32 nLength)
    : m_pStringImpl(NULL)
    , m_nLength(0)
{
    if (NULL != pData)
    {
        CreateFromBuffer(pData, nLength);
    }    
}

CStringW::CStringW(const WCHAR* pszString)
    : m_pStringImpl(NULL)
    , m_nLength(0)
{
    if (NULL != pszString)
    {
        CreateFromBuffer(pszString, GetStringLengthW(pszString));
    }
}

CStringW::CStringW(const wchar_t* pszString)
    : CStringW((const WCHAR *)pszString)
{
}

CStringW::~CStringW()
{
    if (NULL != m_pStringImpl)
    {
        delete[] m_pStringImpl;  
        m_pStringImpl = NULL;          
    }    

    m_nLength = 0;
}

WCHAR CStringW::GetChar(UINT32 nIndex) const
{
    if (nIndex >= m_nLength)
        {            
        DebugLog(DLC_ERROR, "CStringW::GetChar(): out of boundary");
        THROW 0;
    }

    return m_pStringImpl[nIndex];
}

CStringW CStringW::SubString(UINT32 iOffset, UINT32 nLength) const
{
    if (m_nLength < iOffset + nLength)
    {
        // FIXME: add exact exception number later
        DebugLog(DLC_ERROR, "CStringW::SubString(): out of boundary");
        THROW 0;
    }
    
    WCHAR *pBuf = new WCHAR[nLength + 1];
    if (NULL == pBuf)
    {
        DebugLog(DLC_ERROR, "CStringW::SubString(): out of memory");
        THROW 0;
    }

    memset(pBuf, 0, sizeof(WCHAR) * (nLength + 1));
    if (nLength > 0)
    {
        memcpy(pBuf, m_pStringImpl + iOffset, sizeof(WCHAR) * nLength);
    }

    CStringW dstString;
    dstString.m_pStringImpl = pBuf;
    dstString.m_nLength = nLength;
    return dstString;
}

CStringW& CStringW::operator =(const CStringW& rSrc)
{  
    if (&rSrc != this)
    {
        if (NULL != m_pStringImpl)
        {
            delete[] m_pStringImpl;  
            m_pStringImpl = NULL;          
        }    
        
        m_nLength = 0;

        if (NULL != rSrc.m_pStringImpl)
        {
            CreateFromBuffer(rSrc.m_pStringImpl, rSrc.m_nLength);        
        }
    }

    return *this;
}

CStringW CStringW::operator +(const CStringW& rSrc1) const
{
    CStringW resultString;    
    if (m_pStringImpl != NULL || rSrc1.m_pStringImpl != NULL)
    {
        WCHAR *pBuf = new WCHAR[m_nLength + rSrc1.m_nLength + 1];
        if (NULL != pBuf)
        {
            memset(pBuf, 0, sizeof(WCHAR) * (m_nLength + rSrc1.m_nLength + 1));
            if (m_nLength > 0 && m_pStringImpl != NULL)
            {
                memcpy(pBuf, m_pStringImpl, sizeof(WCHAR) * m_nLength);
            }
        
            if (rSrc1.m_nLength > 0 && rSrc1.m_pStringImpl != NULL)
            {
                memcpy(pBuf + m_nLength, rSrc1.m_pStringImpl, sizeof(WCHAR) * rSrc1.m_nLength);
            }

                resultString.m_pStringImpl = pBuf; 
                resultString.m_nLength = m_nLength + rSrc1.m_nLength;
        }
    }
    
    return resultString;
}
    
BOOL CStringW::operator==(const CStringW& rSrc) const
{
    if (NULL == m_pStringImpl && NULL == rSrc.m_pStringImpl)
    {
        return TRUE;
    }
     
    if (NULL == m_pStringImpl || NULL == rSrc.m_pStringImpl)
    {
        return FALSE;
    }

     if (m_nLength != rSrc.m_nLength)
    {
        return FALSE;
    }

    for (UINT32 i = 0; i < m_nLength; i++)
    {
        if (m_pStringImpl[i] != rSrc.m_pStringImpl[i])
        {
            return FALSE;
        }
    }
     
    return TRUE;
}

BOOL CStringW::operator!=(const CStringW& rSrc) const
{
    return (!this->operator ==(rSrc));
}

void CStringW::CreateFromBuffer(const WCHAR* pData, UINT32 nLength)
{        
    m_pStringImpl = new WCHAR[nLength + 1];
    if (NULL == m_pStringImpl)
    {
        return;
    }
    
    m_nLength = nLength;
    memset(m_pStringImpl, 0, sizeof(WCHAR) * (nLength + 1));
    if (nLength > 0)
    {
        memcpy(m_pStringImpl, pData, sizeof(WCHAR) * nLength);
    }
}

UINT32 CStringW::GetStringLengthW(const WCHAR* pszString)
{
    UINT32 nLength = 0;
    while (*pszString)
    {
        nLength++;
        pszString++;
    }

    return nLength;
}

        
