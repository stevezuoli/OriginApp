////////////////////////////////////////////////////////////////////////
// GUIHelper.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/GUIHelper.h"
#include "GUI/UIPage.h"


#define ALPHA_EXTRACTOR(value) (((value) & 0xFF000000) >> 24)
#define RED_EXTRACTOR(value) (((value) & 0xFF0000) >> 16)
#define GREEN_EXTRACTOR(value) (((value) & 0xFF00) >> 8)
#define BLUE_EXTRACTOR(value) ((value) & 0xFF)

INT32 GUIHelper::m_isScreenSaved = 0;

void GUIHelper::DrawCheckBox(ITpGraphics *pGraphics, BOOL bIsChecked, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight)
{
    //    UNUSED(iWidth);
    //    UNUSED(iHeight);
#if 0
    if (NULL == pGraphics)
    {
        return;
    }

    SPtr<ITpImage> spImg = ImageManager::GetImage(bIsChecked ? IMAGE_CHECKBOX_CHECKED : IMAGE_CHECKBOX_UNCHECKED);
    if (spImg)
    {
        pGraphics->DrawImage(spImg.Get(), iX, iY);
    }
#endif
}

void GUIHelper::DrawRadioBox(ITpGraphics *pGraphics, BOOL bIsChecked, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight)
{
    //    UNUSED(iWidth);
    //    UNUSED(iHeight);
#if 0
    if (NULL == pGraphics)
    {
        return;
    }

    SPtr<ITpImage> spImg = ImageManager::GetImage(bIsChecked ? IMAGE_RADIOBOX_CHECKED : IMAGE_RADIOBOX_UNCHECKED);
    if (spImg)
    {
        pGraphics->DrawImage(spImg.Get(), iX, iY);
    }
#endif
}

CString GUIHelper::CreateEllipsisString(const CString& rstrText, ITpFont* pFont, INT32 iWindowWidth)
{
#if 0
    if (rstrText.Length()== 0 || NULL == pFont || iWindowWidth <= 0)
    {
        return CString::EMPTY_STRING;
    }

    // if the text width is less than window width, using the original text
    if (pFont->StringWidth(rstrText) <= iWindowWidth)
    {
        return rstrText;
    }

    INT32 iTextWidth = pFont->StringWidth(rstrText)+ pFont->StringWidth(TEXT("..."));

    if (iTextWidth > iWindowWidth)
    {
        INT32 iIndex = rstrText.Length()- 1;
        for (; iIndex > 0; iIndex--)
        {
            INT32 iCharWidth = pFont->CharWidth(rstrText.GetChar(iIndex));

            if (iTextWidth - iCharWidth <= iWindowWidth)
            {
                break;
            }

            iTextWidth -= iCharWidth;
        }

        // here the iIndex represents the string length
        return rstrText.SubString(0, iIndex)+ TEXT("...");
    }
    else
    {
        return rstrText;
    }
#endif
    return CString::EMPTY_STRING;
}

CString GUIHelper::CreatePrefixEllipsisString(const CString& rstrText, ITpFont* pFont, INT32 iWindowWidth)
{
#if 0
    if (rstrText.Length()== 0 || NULL == pFont || iWindowWidth <= 0)
    {
        return CString::EMPTY_STRING;
    }

    // if the text width is less than window width, using the original text
    if (pFont->StringWidth(rstrText) <= iWindowWidth)
    {
        return rstrText;
    }

    INT32 iTextWidth = pFont->StringWidth(rstrText)+ pFont->StringWidth(TEXT("..."));

    if (iTextWidth > iWindowWidth)
    {
        UINT32 iIndex = 0;
        for (; iIndex < rstrText.Length(); iIndex++)
        {
            INT32 iCharWidth = pFont->CharWidth(rstrText.GetChar(iIndex));

            if (iTextWidth - iCharWidth <= iWindowWidth)
            {
                break;
            }

            iTextWidth -= iCharWidth;
        }

        // here the iIndex represents the string length
        return CString("...") + rstrText.SubString(iIndex + 1, rstrText.Length() - iIndex -1);
    }
    else
    {
        return rstrText;
    }
#endif
    return CString::EMPTY_STRING;
}

INT32 GUIHelper::ExtractBlueComponent(INT32 iRgb)
{
    return BLUE_EXTRACTOR(iRgb);
}

INT32 GUIHelper::ExtractRedComponent(INT32 iRgb)
{
    return RED_EXTRACTOR(iRgb);
}

INT32 GUIHelper::ExtractGreenComponent(INT32 iRgb)
{
    return GREEN_EXTRACTOR(iRgb);
}

INT32 GUIHelper::ExtractAlphaComponent(INT32 iRgb)
{
    return ALPHA_EXTRACTOR(iRgb);
}

INT32 GUIHelper::ComposeRGB(INT32 iRed, INT32 iGreen, INT32 iBlue, INT32 iAlpha)
{
    return ((iAlpha << 24) | (iRed << 16) | (iGreen << 8) | iBlue);
}

DOUBLE GUIHelper::GetRadian(DOUBLE dDegree)
{
    return (dDegree * DK_PI / 180);
}

DOUBLE GUIHelper::GetDegree(DOUBLE dRadian)
{
    return (dRadian * 180 / DK_PI);
}

BOOL GUIHelper::IsTopUIPageVisible(UIWindow* pWindow)
{
    if (pWindow)
    {
        UIPage* pPage = pWindow->GetUIPage();

        return pPage ? pPage->IsVisible() : FALSE;
    }

    return FALSE;
}

INT32 GUIHelper::GetScreenSaverStatus()
{
    return m_isScreenSaved;
}

void GUIHelper::SetScreenSaverStatus(INT32 status)
{
    DebugPrintf(DLC_DIAGNOSTIC, "SetScreenSaverStatus:%d", status);
    m_isScreenSaved = status;
}

