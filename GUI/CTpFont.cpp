////////////////////////////////////////////////////////////////////////
// CTpFont.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "GUI/CTpFont.h"
#include "dkWinError.h"
#include "interface.h"

using DkFormat::CreateTextFontInstance;


CTpFont::CTpFont()
{
    // TODO: Should be in Initialize func.
    // TODO: Should handle error.
    CreateTextFontInstance(&m_pFont);
}

CTpFont::~CTpFont()
{
    if (m_pFont)
    {
        delete m_pFont;
        m_pFont = NULL;
    }
}

INT32 CTpFont::GetHeight()
{
    if (NULL == m_pFont)
    {
        return -1;
    }

    INT32 iHeight = -1;
    this->m_pFont->GetLineHeight(&iHeight);
    return iHeight;
}

INT32 CTpFont::GetDescender()
{
    if (NULL == m_pFont)
    {
        return -1;
    }

    INT32 iDescender = -1;
    this->m_pFont->GetDescender(&iDescender);
    return iDescender;
}


INT32 CTpFont::GetAscender()
{
    if (NULL == m_pFont)
    {
        return -1;
    }

    INT32 iAscender = -1;
    this->m_pFont->GetAscender(&iAscender);
    return iAscender;
}

INT32 CTpFont::GetSize()
{
    if (NULL == m_pFont)
    {
        return -1;
    }

    INT32 iSize = 0;
    this->m_pFont->GetFontSize(&iSize);
    return iSize;
}

BOOL CTpFont::LoadFont(const char * lpszFileName)
{
    if (NULL == lpszFileName)
    {
        return FALSE;
    }

    HRESULT hr;
    if (NULL == m_pFont)
    {    
        hr = CreateTextFontInstance(&m_pFont);
        if (FAILED(hr))
        {
            return FALSE;
        }
    }

    hr = this->m_pFont->LoadFont(lpszFileName);
    return FAILED(hr) ? FALSE : TRUE;
}

void CTpFont::UnLoadFont()
{
    if (NULL != m_pFont)
    {
        delete m_pFont;
        m_pFont = NULL;
    }
}

BOOL CTpFont::SetFontSize(UINT32 nFontSize)
{
    if (NULL == m_pFont)
    {
        return FALSE;
    }

    HRESULT hr = this->m_pFont->SetFontSize(nFontSize);
    return FAILED(hr) ? FALSE : TRUE;
}

INT32 CTpFont::StringWidth(const char* text, int length)
{
    if (NULL == m_pFont)
    {
        return -1;
    }
    if (NULL == text)
    {
        return -1;
    }
    if (length <= 0)
    {
        length = strlen(text);
    }

    INT32 iLength;
    HRESULT hr = this->m_pFont->GetStringWidth(text, length, &iLength);
    return FAILED(hr) ? -1 : iLength;
}

INT32 CTpFont::StringWidth(const wchar_t* wText, int length)
{
	//£ºprintf("CTpFont::StringWidth *pStr: %d, nLength: %d, %x\n", *pStr, nLength, m_pFont);
    if (NULL == m_pFont)
    {
        return -1;
    }

    INT32 iLength;
    HRESULT hr = this->m_pFont->GetStringWidth(wText, length, &iLength);
    return FAILED(hr) ? -1 : iLength;
}

INT32 CTpFont::CharWidth(UINT32 charCode)
{    
    if (NULL == m_pFont)
    {
        return 0;
    }

    INT32 iWidth = 0;
    this->m_pFont->GetCharWidth(charCode, &iWidth);
    return iWidth;
}

void CTpFont::DrawStringUtf8(
    DK_IMAGE &rImage, 
    const DK_RECT& rDrawRect, 
    const char *pStr,
    BOOL fDrawEllipsis,
    UINT nCharSpacing
    )
{
    if (NULL == m_pFont)
    {
        return;
    }

    m_pFont->DrawStringUTF8(rImage, rDrawRect, pStr, fDrawEllipsis, nCharSpacing);
}

// Unicode ×Ö·û´®
void CTpFont::DrawStringW(
    DK_IMAGE &rImage, 
    const DK_RECT &rDrawRect, 
    const wchar_t *pStr,
    BOOL fDrawEllipsis,
    UINT nCharSpacing
    )
{
    if (NULL == m_pFont)
    {
        return;
    }

    m_pFont->DrawStringW(rImage, rDrawRect, pStr, fDrawEllipsis, nCharSpacing);
}


void CTpFont::SetFontColor(unsigned char r, unsigned char g, unsigned char b)
{
    if (NULL == m_pFont)
    {
        return;
    }

    m_pFont->SetFontColor(r, g, b);
}

