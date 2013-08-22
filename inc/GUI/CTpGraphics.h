////////////////////////////////////////////////////////////////////////
// CTpGraphics.h
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __CTPGRAPHICS_H__
#define __CTPGRAPHICS_H__

#include "GUI/ITpImage.h"
#include "GUI/ITpFont.h"
#include "dkImage.h"
#include "dkBaseType.h"
#include "KernelType.h"
#include "Utility/ColorManager.h"

#define OPACITY_ALPHA 255

using DkFormat::DK_IMAGE;

enum BORDER_TYPE
{
    BORDER_BTN = 0,
    BORDER_DIALOG
};
enum BORDER_CORNER
{
    BORDER_CORNER_SQUARE = 0,
    BORDER_CORNER_ROUNDED,
};

class CTpGraphics
{
public:
    CTpGraphics(DK_IMAGE drawingTarget);

    ~CTpGraphics();

    HRESULT EraserBackGround(INT32 iColor = ColorManager::knWhite); // ColorManager::knWhite = 0x0000

    HRESULT DrawLine(INT32 iX1, INT32 iY1, INT32 iWidth, INT32 iHeight, INT32 iStrokeStyle);
    HRESULT DrawRectBorder(int left, int top, int right, int bottom, int thickness, dkColor color);

    HRESULT FillRect(INT32 iLeft, INT32 iTop, INT32 iRight, INT32 iBottom, INT32 iBrushColor);
    HRESULT DrawBorder(
        int leftMargin, 
        int topMargin, 
        int rightMargin, 
        int bottomMargin, 
        int titleHeight = 0, 
        BORDER_TYPE type = BORDER_DIALOG, 
        BORDER_CORNER corner = BORDER_CORNER_ROUNDED);

    HRESULT DrawImageFile(const wchar_t* file, const DK_BOX& srcBox, const DK_BOX& dstBox);

    HRESULT DrawImage(ITpImage* pImg, INT32 iX, INT32 iY);

    HRESULT DrawImage(ITpImage* pImg, INT32 iXInTarget, INT32 iYInTarget, INT32 iXInImage, INT32 iYInImage, INT32 iWidth, INT32 iHeight);

    HRESULT DrawImageBlend(ITpImage* pImg, INT32 iXInTarget, INT32 iYInTarget, INT32 iXInImage, INT32 iYInImage, INT32 iWidth, INT32 iHeight);

    HRESULT DrawImageStretchBlend(ITpImage* pImg, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, INT32 iLineWidth = 3, INT32 iMargin = 10);

//    HRESULT DrawChar(TCHAR cCharacter, INT32 iX, INT32 iY, ITpFont *pFont);

    HRESULT DrawStringUtf8(const char* text, INT32 iX, INT32 iY, ITpFont *pFont, BOOL fDrawEllipsis=TRUE);

    HRESULT DrawStringUnicode(const wchar_t *pStr, INT32 iX, INT32 iY, ITpFont *pFont, BOOL fDrawEllipsis=TRUE);

    HRESULT DrawSubstring(const CString& rstrText, INT32 iOffset, INT32 iLen, INT32 iX, INT32 iY, ITpFont *pFont, BOOL fDrawEllipsis=TRUE);

    HRESULT Do2Gray(INT32 iX,INT32 iY,INT32 iWidth,INT32 iHeight,INT32 iThreshold = 80,BOOL fReverse = FALSE, INT32 grayValue = 0xff);

    DK_IMAGE GetImage();

    HRESULT BitBlt(INT32 iX,INT32 iY,INT32 iWidth,INT32 iHeight, CTpGraphics *pGraphics, INT32 iSX,INT32 iSY);
    HRESULT BitBltBlend(INT32 iX,INT32 iY,INT32 iWidth,INT32 iHeight, CTpGraphics *pGraphics, INT32 iSX,INT32 iSY);
    HRESULT ReverseRect(int left, int top, int right, int bottom);
    HRESULT ReverseRect(DK_RECT rc)
    {
        return ReverseRect(rc.left, rc.top, rc.right, rc.bottom);
    }
    HRESULT BinarizeRect(int left, int top, int right, int bottom);
    HRESULT BinarizeRect(DK_RECT rc)
    {
        return BinarizeRect(rc.left, rc.top, rc.right, rc.bottom);
    }
    
private:
    DK_IMAGE m_drawingTarget;
};

#endif

