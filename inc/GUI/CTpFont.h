////////////////////////////////////////////////////////////////////////
// CTpFont.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __CTPFONT_H__
#define __CTPFONT_H__

#include "GUI/ITpFont.h"
#include "TextFont.h"

//using namespace DkFormat;
using DkFormat::DK_IMAGE;
using DkFormat::ITextFont;

class CTpFont : public ITpFont 
{
public:
    CTpFont();
    virtual ~CTpFont();

    virtual INT32 GetSize();

    virtual INT32 GetFace()
    {
        return FACE_SYSTEM;
    }

    virtual INT32 GetStyle()
    {
        return STYLE_PLAIN;
    }

    virtual BOOL IsBold()
    {
        return FALSE;
    }

    virtual BOOL IsItalic()
    {
        return FALSE;
    }

    virtual BOOL IsUnderlined()
    {
        return FALSE;
    }

    virtual BOOL SetFontSize(UINT32 nFontSize);
    virtual BOOL LoadFont(const char * lpszFileName);
    virtual void UnLoadFont();
    virtual INT32 GetHeight();
    virtual INT32 StringWidth(const char* s, int length = 0);

    virtual INT32 StringWidth(const wchar_t* ws, int length = 0);
    virtual INT32 CharWidth(UINT32 charCode);

    INT32 GetDescender();
    INT32 GetAscender();

    virtual void DrawStringUtf8(
            DK_IMAGE &rImage, 
            const DK_RECT& rDrawRect, 
            const char *pStr,
            BOOL fDrawEllipsis=TRUE,
            UINT nCharSpacing=0
            );

    // Unicode ×Ö·û´®
    virtual void DrawStringW(
            DK_IMAGE &rImage, 
            const DK_RECT &rDrawRect, 
            const wchar_t *pStr,
            BOOL fDrawEllipsis=TRUE,
            UINT nCharSpacing=0
            );


    virtual void SetFontColor(unsigned char r, unsigned char g, unsigned char b);   

private:
    ITextFont *m_pFont;
};

#endif

