////////////////////////////////////////////////////////////////////////
// ITpFont.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __ITPFONT_H__
#define __ITPFONT_H__

#include "Utility/BasicType.h"
#include "Utility/CString.h"
#include "GUI/dkImage.h"

using namespace DkFormat;
//using namespace DkFormat;

enum FONT_FACE
{
    FACE_MONOSPACE,
    FACE_PROPORTIONAL,
    FACE_SYSTEM
};

enum FONT_STYLE
{
    STYLE_BOLD = 1,
    STYLE_ITALIC = 2,
    STYLE_PLAIN = 4,
    STYLE_UNDERLINED = 8
};

enum FONT_SIZE
{
    SIZE_LARGE,
    SIZE_MEDIUM,
    SIZE_SMALL
};

typedef struct FontAttributes
{
    FontAttributes()
    {
        m_iFace = FACE_SYSTEM;
        m_iStyle = STYLE_PLAIN;
        m_iSize = SIZE_MEDIUM;
    }

    void SetFontAttributes(int iFace, int iStyle, int iSize)
    {
            // The arguments will not be checked here, when they are used in FontManager,
        // they will be checked.
        m_iFace = iFace;
        m_iStyle = iStyle;
        m_iSize = iSize;
    }

    INT32 m_iFace;
    INT32 m_iStyle;
    INT32 m_iSize;
}DKFontAttributes;

class ITpFont
{
public:
    virtual ~ITpFont()
    {
        // empty
    }

    virtual INT32 GetSize() = 0;
    virtual INT32 GetFace() = 0;
    virtual INT32 GetStyle() = 0;

    virtual BOOL IsBold() = 0;
    virtual BOOL IsItalic() = 0;
    virtual BOOL IsUnderlined() = 0;

    virtual BOOL SetFontSize(UINT32 nFontSize) = 0;
    virtual BOOL LoadFont(const char * lpszFileName) = 0;
    
    virtual void UnLoadFont() = 0;
    virtual INT32 GetHeight() = 0;
    virtual INT32 StringWidth(const char* text, int length = 0) = 0;

    // Unicode width in pixels
    virtual INT32 StringWidth(const wchar_t * wText, int length = 0) = 0;
    virtual INT32 CharWidth(UINT32 charCode) = 0;

    // UTF8 ×Ö·û´®
    virtual void DrawStringUtf8(
        DK_IMAGE &rImage, 
        const DK_RECT& rDrawRect, 
        const char *pStr,
        BOOL fDrawEllipsis = TRUE,
        UINT nCharSpacing=0
    )=0;

    // Unicode ×Ö·û´®
    virtual void DrawStringW(
        DK_IMAGE &rImage, 
        const DK_RECT &rDrawRect, 
        const wchar_t *pStr,
        BOOL fDrawEllipsis = TRUE,
        UINT nCharSpacing=0
    )=0;

    virtual void SetFontColor(unsigned char r, unsigned char g, unsigned char b)=0;   
};

#endif
