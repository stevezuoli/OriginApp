////////////////////////////////////////////////////////////////////////
// ITpGraphics.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __ITPGRAPHICS_H__
#define __ITPGRAPHICS_H__

#include "Utility/BasicType.h"
#include "Utility/CString.h"
#include "GUI/ITpFont.h"
#include "GUI/ITpImage.h"
#include "drivers/CFbBitDc.h"

class ITpGraphics
{
    ////////////////////Constructor Section/////////////////////////
public:
    virtual ~ITpGraphics()
    {
        // empty
    }
    ////////////////////Constructor Section/////////////////////////

    ////////////////////Method Section/////////////////////////
public:
    virtual void Translate(INT32 iX, INT32 iY) = 0;
    virtual INT32 GetTranslateY() = 0;
    virtual INT32 GetTranslateX() = 0;

    virtual void SetDimension(INT32 iWidth, INT32 iHeight) = 0;
    virtual INT32 GetHeight() = 0;
    virtual INT32 GetWidth() = 0;

    virtual void SetOffset(INT32 iX, INT32 iY) = 0;
    virtual void Offset(INT32 iX, INT32 iY) = 0;
    virtual INT32 GetOffsetX() = 0;
    virtual INT32 GetOffsetY() = 0;

    virtual void RestoreClip() = 0;
    virtual void CleanClip() = 0;
    virtual INT32 GetClipX() = 0;
    virtual INT32 GetClipY() = 0;
    virtual INT32 GetClipWidth() = 0;
    virtual INT32 GetClipHeight() = 0;
    virtual void SetClip(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight) = 0;

    virtual INT32 GetBlueComponent() = 0;
    virtual INT32 GetGreenComponent() = 0;
    virtual INT32 GetRedComponent() = 0;

    virtual INT32 GetStrokeStyle() = 0;
    virtual void SetStrokeStyle(INT32 iStyle) = 0;

    virtual ITpFont* GetFont() = 0;
    virtual void SetFont(ITpFont* font) = 0;
    virtual void SetFont(DKFontAttributes * fontAttr)=0;
    virtual void DiscardFont() = 0;

    virtual void SetColor(INT32 iRed, INT32 iGreen, INT32 iBlue) = 0;
    virtual void SetColor(INT32 iRGB) = 0;
    virtual INT32 GetColor() = 0;

    virtual CFbBitDc * GetpGraphics() = 0;
    virtual void BitBlt(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, ITpGraphics *srcDC, int isX, int isY) = 0;
    virtual void BitBltBlend(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, ITpGraphics *srcDC, int isX, int isY) = 0;

    virtual void DrawImage(ITpImage* pImg, INT32 iX, INT32 iY) = 0;
    virtual void DrawImage(ITpImage* pImg, INT32 iX, INT32 iY, INT32 iXInImage, INT32 iYInImage, INT32 iWidth,INT32 iHeight) = 0;
    //virtual void DrawImageBlend(ITpImage* pImg, INT32 iX, INT32 iY, INT32 iXInImage, INT32 iYInImage, INT32 iWidth,INT32 iHeight) = 0;
    virtual void DrawImageBlend(ITpImage* pImg, INT32 iX, INT32 iY, INT32 iXInImage, INT32 iYInImage, INT32 iWidth,INT32 iHeight) = 0;
    virtual void DrawImageStretch(ITpImage* pImg, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight) = 0;
    virtual void DrawImageStretchBlend(ITpImage* pImg, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight) = 0;
    virtual void
            DrawImageStretchWithAlpha(ITpImage* pImg, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, INT32 iAlpha) = 0;
    virtual void DrawImageStretch(ITpImage* pImg, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, INT32 iLeftMargin,
            INT32 iRightMargin, INT32 iTopMargin, INT32 iBottomMargin) = 0;
    virtual void DrawImageStretch(ITpImage* pImg, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, INT32 iMargin) = 0;
    virtual void DrawImageStretchBlend(ITpImage* pImg, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, INT32 iMargin) = 0;
    virtual void DrawImageStretch(ITpImage* pImg, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight,
            INT32 iHorizonMargin, INT32 iVerticalMargin) = 0;
    virtual void DrawImageStretchWithAlpha(ITpImage* pImg, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight,
            INT32 iLeftMargin, INT32 iRightMargin, INT32 iTopMargin, INT32 iBottomMargin, INT32 iAlpha) = 0;

    virtual void DrawChar(TCHAR cCharacter, INT32 iX, INT32 iY) = 0;
    virtual void DrawString(const CString& rstrText, INT32 iX, INT32 iY) = 0;
    virtual void DrawSubstring(const CString& rstrText, INT32 iOffset, INT32 iLen, INT32 iX, INT32 iY) = 0;
     //Draw the specified String using the current font and color.
     //If String's length is greater than width, then draw "..." in the end.
    virtual void DrawStringEllipsis(const CString& rstrText, INT32 iX, INT32 iY, INT32 iWidth) = 0;

    virtual void CopyArea(INT32 iX_src, INT32 iY_src, INT32 iWidth, INT32 iHeight, INT32 iX_dest, INT32 iY_dest) = 0;
    virtual void DrawArc(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, INT32 iStartAngle, INT32 iArcAngle) = 0;

    virtual void DrawLine(INT32 iX1, INT32 iY1, INT32 iX2, INT32 iY2) = 0;
    virtual void DrawRect(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight) = 0;
    virtual void DrawRoundRect(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, INT32 iArcWidth, INT32 iArcHeight) = 0;
    virtual void FillRect(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight) = 0;
    virtual void FillRect(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, INT32 iAlpha) = 0;
    virtual void FillArc(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, INT32 iStartAngle, INT32 iArcAngle) = 0;
    virtual void FillRoundRect(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, INT32 iArcWidth, INT32 iArcHeight) = 0;
    virtual void FillRoundRect(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, INT32 iArcWidth, INT32 iArcHeight,
            INT32 iAlpha) = 0;
    virtual void FillTriangle(INT32 iX1, INT32 iY1, INT32 iX2, INT32 iY2, INT32 iX3, INT32 iY3) = 0;
    virtual void ClipRect(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight) = 0;
    ////////////////////Method Section/////////////////////////
};

#endif
