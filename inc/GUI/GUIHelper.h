////////////////////////////////////////////////////////////////////////
// GUIHelper.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __GUIHELPER_H__
#define __GUIHELPER_H__

#include "GUI/ITpGraphics.h"
#include "GUI/UIWindow.h"

class GUIHelper
{
public:
    // graphic related
    static void DrawCheckBox(ITpGraphics *pGraphics, BOOL bIsChecked, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight);
    static void DrawRadioBox(ITpGraphics *pGraphics, BOOL bIsChecked, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight);

    static CString CreateEllipsisString(const CString& rstrText, ITpFont* pFont, INT32 iWindowWidth);
    static CString CreatePrefixEllipsisString(const CString& rstrText, ITpFont* pFont, INT32 iWindowWidth);

    // Color related
    static INT32 ExtractBlueComponent(INT32 iRgb);
    static INT32 ExtractRedComponent(INT32 iRgb);
    static INT32 ExtractGreenComponent(INT32 iRgb);
    static INT32 ExtractAlphaComponent(INT32 iRgb);
    static INT32 ComposeRGB(INT32 iRed, INT32 iGreen, INT32 iBlue, INT32 iAlpha);

    // math related
    static DOUBLE GetRadian(DOUBLE dDegree);
    static DOUBLE GetDegree(DOUBLE dRadian);

    // UI window related
    /// <summary>
    /// Return true if the top UIPage window containing the given window is visible,
    /// otherwise return false;
    /// </summary>
    static BOOL IsTopUIPageVisible(UIWindow* pWindow);
    static INT32 GetScreenSaverStatus();
    static void SetScreenSaverStatus(INT32 status);

public:
    static INT32 m_isScreenSaved;
};

#endif

