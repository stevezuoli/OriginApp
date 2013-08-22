////////////////////////////////////////////////////////////////////////
// CTpGraphics.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/CTpGraphics.h"
#include "dkWinError.h"
#include "Public/Base/TPDef.h"
#include "interface.h"
#include "Utility.h"
#include "Utility/ColorManager.h"
#include "Utility/EncodeUtil.h"
#include "Utility/RenderUtil.h"
#include "BMPProcessor/IBMPProcessor.h"
#include "dkWinError.h"
#include "DKRAPI.h"
#include "DkSPtr.h"
#include "Utility/ImageManager.h"
#include "Common/WindowsMetrics.h"

using namespace DkFormat;
using namespace dk::utility;
using namespace WindowsMetrics;

// 人眼对亮度的感知： Gray = 0.299 * R + 0.587 * G + 0.114 * B  =>  1024 * Gray = 306 * R + 601 * G + 117 *B
static BYTE8 COLORTOGRAY32(BYTE8 * COLOR)
{
    return ((COLOR[2] * 306 + COLOR[1] * 601 + COLOR[0] * 117) >> 10);
}

static BYTE8 COLORTOGRAY24(BYTE8 * COLOR)
{
    return ((COLOR[2] * 306 + COLOR[1] * 601 + COLOR[0] * 117) >> 10); 
}

static BYTE8 COLORTOGRAY16(BYTE8 * COLOR)
{
    return (((COLOR[0] & 0xF8) * 306 + (((COLOR[0] & 0x7) << 5) | (((COLOR[1] & 0xE0) >> 3 ))) * 601 + ((COLOR[1] & 0x1F) <<3) * 117) >> 10);
}

static int s_rgiDitherMatrix16[] = {
    0, 235, 59, 219, 15, 231, 55, 215, 2, 232, 56, 217, 12, 229, 52, 213,
    128, 64, 187, 123, 143, 79, 183, 119, 130, 66, 184, 120, 140, 76, 180, 116,
    33, 192, 16, 251, 47, 207, 31, 247, 34, 194, 18, 248, 44, 204, 28, 244,
    161, 97, 144, 80, 175, 111, 159, 95, 162, 98, 146, 82, 172, 108, 156, 92,
    8, 225, 48, 208, 5, 239, 63, 223, 10, 226, 50, 210, 6, 236, 60, 220,
    136, 72, 176, 112, 133, 69, 191, 127, 138, 74, 178, 114, 134, 70, 188, 124,
    41, 200, 24, 240, 36, 197, 20, 255, 42, 202, 26, 242, 38, 198, 22, 252,
    169, 105, 152, 88, 164, 100, 148, 84, 170, 106, 154, 90, 166, 102, 150, 86,
    3, 233, 57, 216, 13, 228, 53, 212, 1, 234, 58, 218, 14, 230, 54, 214,
    131, 67, 185, 121, 141, 77, 181, 117, 129, 65, 186, 122, 142, 78, 182, 118,
    35, 195, 19, 249, 45, 205, 29, 245, 32, 193, 17, 250, 46, 206, 30, 246,
    163, 99, 147, 83, 173, 109, 157, 93, 160, 96, 145, 81, 174, 110, 158, 94,
    11, 227, 51, 211, 7, 237, 61, 221, 9, 224, 49, 209, 4, 238, 62, 222,
    139, 75, 179, 115, 135, 71, 189, 125, 137, 73, 177, 113, 132, 68, 190, 126,
    43, 203, 27, 243, 39, 199, 23, 253, 40, 201, 25, 241, 37, 196, 21, 254,
    171, 107, 155, 91, 167, 103, 151, 87, 168, 104, 153, 89, 165, 101, 149, 85};


    CTpGraphics::CTpGraphics(DK_IMAGE drawingTarget)
        : m_drawingTarget(drawingTarget)
    {
        // do nothing
    }

    CTpGraphics::~CTpGraphics()
    {
        // do nothing
    }

    HRESULT CTpGraphics::DrawLine(INT32 iX1, INT32 iY1, INT32 iWidth, INT32 iHeight, INT32 iStrokeStyle)
    {
        return m_drawingTarget.DrawLine(iX1, iY1, iWidth, iHeight, iStrokeStyle);
    }

    HRESULT CTpGraphics::FillRect(INT32 iLeft, INT32 iTop, INT32 iRight, INT32 iBottom, INT32 iBrushColor)
    {
        return m_drawingTarget.FillRect(iLeft, iTop, iRight - iLeft, iBottom - iTop, iBrushColor);
    }

    HRESULT CTpGraphics::DrawBorder(int leftMargin, int topMargin, int rightMargin, int bottomMargin, int titleHeight, BORDER_TYPE type, BORDER_CORNER corner)
    {
        HRESULT hr(S_OK);
        const int width = m_drawingTarget.iWidth;
        const int height = m_drawingTarget.iHeight;
        const int left = leftMargin;
        const int top = topMargin;
        const int right = width - rightMargin;
        const int bottom = height - bottomMargin;

        const int borderLine = GetWindowMetrics((BORDER_BTN == type) ? UIBtnBorderLineIndex : UIDialogBorderLineIndex);
        RTN_HR_IF_FAILED(FillRect(left, top, right, bottom, ColorManager::knBlack));

        int titleBottom = top + borderLine;
        if (titleHeight > 0)
        {
            titleBottom += titleHeight;
            RTN_HR_IF_FAILED(FillRect(left + borderLine, top + borderLine
                , right - borderLine, titleBottom, ColorManager::GetColor(COLOR_TITLE_BACKGROUND)));
        }
        RTN_HR_IF_FAILED(FillRect(left + borderLine, titleBottom
            , right - borderLine, bottom - borderLine, ColorManager::knWhite));

        if (BORDER_CORNER_ROUNDED == corner)
        {
            SPtr<ITpImage> pUpLeftImage, pUpRightImage, pLeftDownImage, pRightDownImage;
            if (BORDER_BTN == type)
            {
#ifdef KINDLE_FOR_TOUCH
                pUpLeftImage = ImageManager::GetImage(IMAGE_TOUCH_ICON_BTN_UPLEFT);
                pUpRightImage = ImageManager::GetImage(IMAGE_TOUCH_ICON_BTN_UPRIGHT);
                pLeftDownImage = ImageManager::GetImage(IMAGE_TOUCH_ICON_BTN_LEFTDOWN);
                pRightDownImage = ImageManager::GetImage(IMAGE_TOUCH_ICON_BTN_RIGHTDOWN);
#else
                pUpLeftImage = ImageManager::GetImage(IMAGE_IME_ICON_R4);
                pUpRightImage = ImageManager::GetImage(IMAGE_IME_ICON_R1);
                pLeftDownImage = ImageManager::GetImage(IMAGE_IME_ICON_R3);
                pRightDownImage = ImageManager::GetImage(IMAGE_IME_ICON_R2);
#endif
            }
            else
            {
                if (titleHeight > 0)
                {
#ifdef KINDLE_FOR_TOUCH
                    pUpLeftImage = ImageManager::GetImage(IMAGE_TOUCH_MENU_GRAYBG_01);
                    pUpRightImage = ImageManager::GetImage(IMAGE_TOUCH_MENU_GRAYBG_02);
#else
                    pUpLeftImage = ImageManager::GetImage(IMAGE_MENU_GRAYBG_01);
                    pUpRightImage = ImageManager::GetImage(IMAGE_MENU_GRAYBG_02);
#endif
                }
                else
                {
                    pUpLeftImage = ImageManager::GetImage(IMAGE_IME_ICON_R4);
                    pUpRightImage = ImageManager::GetImage(IMAGE_IME_ICON_R1);
                }
                pLeftDownImage = ImageManager::GetImage(IMAGE_IME_ICON_R3);
                pRightDownImage = ImageManager::GetImage(IMAGE_IME_ICON_R2);
            }

            if ((NULL == pUpLeftImage.Get()) || (NULL == pUpRightImage.Get()) || (NULL == pLeftDownImage.Get()) || (NULL == pRightDownImage.Get()))
            {
                return S_FALSE;
            }

            RTN_HR_IF_FAILED(DrawImage(pUpLeftImage.Get(), left, top));
            RTN_HR_IF_FAILED(DrawImage(pUpRightImage.Get(), right - pUpRightImage->GetWidth(), top));
            RTN_HR_IF_FAILED(DrawImage(pLeftDownImage.Get(), left, bottom - pLeftDownImage->GetHeight()));
            RTN_HR_IF_FAILED(DrawImage(pRightDownImage.Get(), right - pRightDownImage->GetWidth(), bottom - pRightDownImage->GetHeight()));
        }
        return hr;
    }

    HRESULT CTpGraphics::DrawImage(ITpImage* pImg, INT32 iX, INT32 iY)
    {
        if (NULL == pImg)
        {
            return E_INVALIDARG;
        }

        return DrawImage(pImg, iX, iY, 0, 0, pImg->GetWidth(), pImg->GetHeight());
    }

    HRESULT CTpGraphics::DrawImage(
        ITpImage* pImg,
        INT32 iXInTarget, INT32 iYInTarget,
        INT32 iXInImage, INT32 iYInImage,
        INT32 iWidth, INT32 iHeight
        )
    {
        DebugLog(DLC_FRAMEBUF, "CTpGraphics::DrawImage()");

        if (NULL == m_drawingTarget.pbData)
        {
            return E_FAIL;
        }

        if (NULL == pImg || NULL == pImg->bmBits || iWidth < 0 || iHeight < 0)
        {
            return E_INVALIDARG;
        }

        DK_RECT drawingRect;
        drawingRect.left = 0;
        drawingRect.top = 0;
        drawingRect.right = m_drawingTarget.iWidth;
        drawingRect.bottom = m_drawingTarget.iHeight;
        DK_RECT requiredRect;
        requiredRect.left = iXInTarget;
        requiredRect.top = iYInTarget;
        requiredRect.right = iXInTarget + iWidth;
        requiredRect.bottom = iYInTarget + iHeight;

        DK_RECT targetRect = CUtility::GetIntersectRect(drawingRect, requiredRect);
        if (CUtility::IsEmptyRect(targetRect))
        {
            return S_OK;
        }

        DK_RECT imgRect;
        imgRect.left = 0;
        imgRect.top = 0;
        imgRect.right = pImg->GetWidth();
        imgRect.bottom = pImg->GetHeight();

        DK_RECT imgSrcRect;
        imgSrcRect.left = iXInImage;
        imgSrcRect.top = iYInImage;
        imgSrcRect.right = iXInImage + iWidth;
        imgSrcRect.bottom = iYInImage + iHeight;

        DK_RECT srcRect = CUtility::GetIntersectRect(imgRect, imgSrcRect);
        if (CUtility::IsEmptyRect(srcRect))
        {
            return S_OK;
        }

        INT32 iSrcWidth = srcRect.right - srcRect.left;
        INT32 iSrcHeight = srcRect.bottom - srcRect.top;
        INT32 iDestWidth = targetRect.right - targetRect.left;
        INT32 iDestHeight = targetRect.bottom - targetRect.top;

        INT32 nCopyWidth = iSrcWidth < iDestWidth ? iSrcWidth : iDestWidth;
        INT32 nCopyHeight = iSrcHeight < iDestHeight ? iSrcHeight : iDestHeight;

        INT32 iSrcStride = pImg->bmWidth * pImg->bmPitch;
        INT32 iDestStride = m_drawingTarget.iStrideWidth;

        BYTE8 *pbSrc = pImg->bmBits + (srcRect.top * pImg->bmWidth + srcRect.left) * pImg->bmPitch;
        BYTE8 *pbDest = m_drawingTarget.pbData + (targetRect.top * m_drawingTarget.iStrideWidth + targetRect.left);

        INT32 i, j;
        INT32 iGrayValue;
        unsigned char bTempL;
        unsigned char bTempNum;

        switch(pImg->bmPitch)
        {
        case 1:
            for (i = 0; i < nCopyHeight; i++)
            {
                for (j = 0; j < nCopyWidth; j++)
                {
                    pbDest[j] = (~pbSrc[j]) |(pbSrc[j] & 0xF);
                }
                pbSrc += iSrcStride;
                pbDest += iDestStride;
            }
            break;
        case 2:
            for (i = 0; i < nCopyHeight; i++)
            {
                for (j = 0; j < nCopyWidth; j++)
                {
                    iGrayValue = COLORTOGRAY16(pbSrc + j * 2);
                    bTempL = s_rgiDitherMatrix16[((i & 0xF) * 0x10 + (j & 0xF)) & 0xFF];
                    bTempNum = iGrayValue / 0x11;
                    if  (bTempNum != 0xf)
                    {
                        // 计算当前的误差与误差表中的误差进行比较，当大于相应误差是，则加一。
                        bTempNum += ((((iGrayValue - bTempNum * 0x11) * 0x100 / 0x11) > bTempL) & 1);
                    }

                    pbDest[j] = bTempNum << 4;
                    pbDest[j] = (~pbDest[j]) | 0xF;
                }

                pbSrc += iSrcStride;
                pbDest += iDestStride;
            }

            break;
        case 3:
            for(i = 0; i < nCopyHeight; i++)
            {
                for(j = 0; j < nCopyWidth; j++)
                {
                    iGrayValue = COLORTOGRAY24(pbSrc + j * 3);  // + src1[j * 3 + 1] + src1[j * 3 + 2];
                    bTempL = s_rgiDitherMatrix16[((i & 0xF) * 0x10 + (j & 0xF)) & 0xFF];
                    bTempNum = iGrayValue / 0x11;
                    if  (bTempNum != 0xf)
                    {
                        // 计算当前的误差与误差表中的误差进行比较，当大于相应误差是，则加一。
                        bTempNum += ((((iGrayValue - bTempNum * 0x11) * 0x100 / 0x11) > bTempL) & 1);
                    }

                    pbDest[j] = bTempNum << 4;
                    pbDest[j] = (~pbDest[j]) | 0xF;
                }
                pbSrc += iSrcStride;
                pbDest += iDestStride;
            }
            break;
        case 4:
            for(i = 0; i < nCopyHeight; i++)
            {
                for(j = 0; j < nCopyWidth; j++)
                {
                    iGrayValue = COLORTOGRAY32( pbSrc + j * 4);  // + src1[j * 3 + 1] + src1[j * 3 + 2];
                    bTempL = s_rgiDitherMatrix16[((i & 0xF) * 0x10 + (j & 0xF)) & 0xFF];
                    bTempNum = iGrayValue / 0x11;
                    if  (bTempNum != 0xf)
                    {
                        // 计算当前的误差与误差表中的误差进行比较，当大于相应误差是，则加一。
                        bTempNum += ((((iGrayValue - bTempNum * 0x11) * 0x100 / 0x11) > bTempL) & 1);
                    }

                    pbDest[j] = bTempNum << 4;
                    pbDest[j] = (~pbDest[j]) | ((*(pbSrc + j * 4 + 1)) >> 4 | 0xF);
                }

                pbSrc += iSrcStride;
                pbDest += iDestStride;
            }
            break;
        default:
            return E_FAIL;
            break;
        }

        return S_OK;
    }


    HRESULT CTpGraphics::EraserBackGround(int iColor)
    {
        FillRect(0, 0, m_drawingTarget.iWidth, m_drawingTarget.iHeight, iColor);
        return S_OK;
    }


    HRESULT CTpGraphics::DrawImageBlend(ITpImage* pImg, INT32 iXInTarget, INT32 iYInTarget, INT32 iXInImage, INT32 iYInImage, INT32 iWidth, INT32 iHeight)
    {
        if (NULL == m_drawingTarget.pbData)
        {
            return E_FAIL;
        }

        if (NULL == pImg || NULL == pImg->bmBits || iWidth < 0 || iHeight < 0)
        {
            return E_INVALIDARG;
        }

        DK_RECT drawingRect;
        drawingRect.left = 0;
        drawingRect.top = 0;
        drawingRect.right = m_drawingTarget.iWidth;
        drawingRect.bottom = m_drawingTarget.iHeight;
        DK_RECT requiredRect;
        requiredRect.left = iXInTarget;
        requiredRect.top = iYInTarget;
        requiredRect.right = iXInTarget + iWidth;
        requiredRect.bottom = iYInTarget + iHeight;

        DK_RECT targetRect = CUtility::GetIntersectRect(drawingRect, requiredRect);
        if (CUtility::IsEmptyRect(targetRect))
        {
            return S_OK;
        }

        DK_RECT imgRect;
        imgRect.left = 0;
        imgRect.top = 0;
        imgRect.right = pImg->GetWidth();
        imgRect.bottom = pImg->GetHeight();

        DK_RECT imgSrcRect;
        imgSrcRect.left = iXInImage;
        imgSrcRect.top = iYInImage;
        imgSrcRect.right = iXInImage + iWidth;
        imgSrcRect.bottom = iYInImage + iHeight;

        DK_RECT srcRect = CUtility::GetIntersectRect(imgRect, imgSrcRect);
        if (CUtility::IsEmptyRect(srcRect))
        {
            return S_OK;
        }

        INT32 iSrcWidth = srcRect.right - srcRect.left;
        INT32 iSrcHeight = srcRect.bottom - srcRect.top;
        INT32 iDestWidth = targetRect.right - targetRect.left;
        INT32 iDestHeight = targetRect.bottom - targetRect.top;

        INT32 nCopyWidth = iSrcWidth < iDestWidth ? iSrcWidth : iDestWidth;
        INT32 nCopyHeight = iSrcHeight < iDestHeight ? iSrcHeight : iDestHeight;

        INT32 iSrcStride = pImg->bmWidth * pImg->bmPitch;
        INT32 iDestStride = m_drawingTarget.iStrideWidth;

        BYTE8 *pbSrc = pImg->bmBits + (srcRect.top * pImg->bmWidth + srcRect.left) * pImg->bmPitch;
        BYTE8 *pbDest = m_drawingTarget.pbData + (targetRect.top * m_drawingTarget.iStrideWidth + targetRect.left);

        INT32 i, j;
        INT32 iGrayValue;
        unsigned char bTempL;
        unsigned char bTempNum;
        UINT32 uAlpha;
        UINT32 uTempColor;

        switch(pImg->bmPitch)
        {
        case 1:
            for(i = 0; i < nCopyHeight; i++)
            {
                for(j = 0; j < nCopyWidth; j++)
                {
                    uAlpha = pbSrc[j] & 0xF;
                    bTempNum = (pbSrc[j] >> 4) & 0xF;
                    if(0xF == uAlpha)
                    {
                        uTempColor = 0xF - bTempNum;
                    }
                    else
                    {
                        uTempColor = (((0xF - bTempNum) * uAlpha) >> 4) + (((pbDest[j] >> 4) & 0xF) * (0xF - uAlpha) >> 4);
                    }
                    pbDest[j] = ((uTempColor << 4) & 0xF0) | (pbDest[j] & 0xF);

                }
                pbSrc += iSrcStride;
                pbDest += iDestStride;
            }
            break;
        case 4:
            for(i = 0; i < nCopyHeight; i++)
            {
                for(j = 0; j < nCopyWidth; j++)
                {
                    iGrayValue = COLORTOGRAY32( pbSrc + j * 4); // + src1[j * 3 + 1] + src1[j * 3 + 2];
                    bTempL = s_rgiDitherMatrix16[((i & 0xF) * 0x10 + (j & 0xF)) & 0xFF];
                    bTempNum = iGrayValue / 0x11;
                    if  (bTempNum != 0xf)// 计算当前的误差与误差表中的误差进行比较，当大于相应误差是，则加一。
                        bTempNum += ((((iGrayValue - bTempNum * 0x11) * 0x100 / 0x11) > bTempL) & 1);


                    if(pbSrc[j * 4 + 3] & 0xFF)
                    {
                        uAlpha = (pbSrc[j * 4 + 3] >> 4) & 0xF;
                        if(0xF == uAlpha)
                        {
                            uTempColor = 0xF - bTempNum;
                        }
                        else
                        {
                            uTempColor = (((0xF - bTempNum) * uAlpha) >> 4) + (((pbDest[j] >> 4) & 0xF) * (0xF - uAlpha) >> 4);
                        }
                        pbDest[j] = ((uTempColor << 4) & 0xF0) | (pbDest[j] & 0xF);
                    }
                }
                pbSrc += iSrcStride;
                pbDest += iDestStride;
            }
            break;
        default:
            return DrawImage(pImg, iXInTarget, iYInTarget, iXInImage, iYInImage,iWidth, iHeight);
            break;
        }

        return S_OK;
    }

    HRESULT CTpGraphics::DrawImageStretchBlend(ITpImage* pImg, INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight,INT32 iLineWidth, INT32 iMargin)
    {
        if (NULL == m_drawingTarget.pbData)
        {
            return E_FAIL;
        }

        if (NULL == pImg || NULL == pImg->bmBits || iWidth < 0 || iHeight < 0)
        {
            return E_INVALIDARG;
        }

        INT32 iImageWidth = pImg->GetWidth();
        INT32 iImageHeight = pImg->GetHeight();
        //    INT32 i = 0;
        //    INT32 j = 0;
        //    INT32 iOffsetX = 0;
        //    INT32 iOffsetY = 0;

        //---------------------for four corners------------------------------
        INT32 iImgX11 = 0;
        INT32 iImgY11 = 0;

        INT32 iImgX21 = 0;
        INT32 iImgY21 = iImageHeight -iMargin;

        INT32 iImgX31 = iImageWidth - iMargin;
        INT32 iImgY31 = 0;

        INT32 iImgX41 = iImageWidth - iMargin;
        INT32 iImgY41 = iImageHeight -iMargin;

        INT32 iGraphX11 = iX;
        INT32 iGraphY11 = iY;
        //    INT32 iGraphX12 = iMargin;
        //    INT32 iGraphY12 = iMargin;

        INT32 iGraphX21 = iX; //iX + iWidth - iMargin;
        INT32 iGraphY21 = iY + iHeight - iMargin;
        //    INT32 iGraphX22 = iMargin;
        //    INT32 iGraphY22 = iMargin;

        INT32 iGraphX31 = iX + iWidth - iMargin;
        INT32 iGraphY31 = iY;
        //    INT32 iGraphX32 = iMargin;
        //    INT32 iGraphY32 = iMargin;

        INT32 iGraphX41 = iX + iWidth - iMargin;
        INT32 iGraphY41 = iY + iHeight - iMargin;
        //    INT32 iGraphX42 = iMargin;
        //    INT32 iGraphY42 = iMargin;

        DrawImageBlend(pImg, iGraphX11, iGraphY11, iImgX11, iImgY11, iMargin, iMargin);
        DrawImageBlend(pImg, iGraphX21, iGraphY21, iImgX21, iImgY21, iMargin, iMargin);
        DrawImageBlend(pImg, iGraphX31, iGraphY31, iImgX31, iImgY31, iMargin, iMargin);
        DrawImageBlend(pImg, iGraphX41, iGraphY41, iImgX41, iImgY41, iMargin, iMargin);

        DrawLine(iX + iMargin, iY ,iWidth - 2 * iMargin + 1, iLineWidth ,SOLID_STROKE);
        DrawLine(iX + iMargin, iY + iHeight - iLineWidth, iWidth - 2 * iMargin + 1, iLineWidth ,SOLID_STROKE);
        DrawLine(iX,iY + iMargin ,iLineWidth  + 1,iHeight - 2 * iMargin, SOLID_STROKE);
        DrawLine(iX + iWidth - 3 * iLineWidth + 1, iY + iMargin, iLineWidth * 3 ,iHeight - 2 * iMargin,SOLID_STROKE);

        FillRect(iX + iMargin, iY + iLineWidth, iX + iWidth - iMargin, iY + iHeight -  iLineWidth, 0);
        FillRect(iX + iLineWidth, iY + iMargin, iMargin + iX, iY + iHeight -  iMargin, 0);
        FillRect(iX + iWidth - iMargin, iY + iMargin, iX + iWidth - iLineWidth, iY + iHeight - iMargin, 0);
        return S_OK;
    }

HRESULT CTpGraphics::DrawStringUnicode(const wchar_t *pStr, INT32 iX, INT32 iY, ITpFont *pFont, BOOL fDrawEllipsis)
{
    if (NULL == m_drawingTarget.pbData)
    {
        return E_FAIL;
    }

    if (NULL == pFont)
    {
        return E_INVALIDARG;
    }

    DK_RECT rect;
    rect.left = iX;
    rect.top = iY;
    rect.right = m_drawingTarget.iWidth;
    rect.bottom = m_drawingTarget.iHeight;

    pFont->DrawStringW(m_drawingTarget, rect, pStr, fDrawEllipsis);
    return S_OK;
}


HRESULT CTpGraphics::DrawStringUtf8(const char* text, INT32 iX, INT32 iY, ITpFont *pFont, BOOL fDrawEllipsis)
{
    if (NULL == text)
    {
        return E_FAIL;
    }

    if (NULL == m_drawingTarget.pbData)
    {
        return E_FAIL;
    }

    if (NULL == pFont)
    {
        return E_INVALIDARG;
    }

    DK_RECT rect;
    rect.left = iX;
    rect.top = iY;
    rect.right = m_drawingTarget.iWidth;
    rect.bottom = m_drawingTarget.iHeight;

    pFont->DrawStringUtf8(m_drawingTarget, rect, text, fDrawEllipsis);
    return S_OK;
}

HRESULT CTpGraphics::DrawSubstring(const CString& rstrText, INT32 iOffset, INT32 iLen, INT32 iX, INT32 iY, ITpFont *pFont, BOOL fDrawEllipsis)
{
    if (NULL == m_drawingTarget.pbData)
    {
        return E_FAIL;
    }

    if (NULL == pFont)
    {
        return E_INVALIDARG;
    }

    return DrawStringUtf8(rstrText.SubString(iOffset, iLen).GetBuffer(), iX, iY, pFont, fDrawEllipsis);
}
HRESULT CTpGraphics::Do2Gray(INT32 iX,INT32 iY,INT32 iWidth,INT32 iHeight,INT32 iThreshold,BOOL fReverse, INT32 grayValue)
{
    if (NULL == m_drawingTarget.pbData)
    {
        return E_FAIL;
    }
    for(int i = 0 ; i< iHeight ; i++)
    {
        for(int j=0;j< iWidth;j++)
        {
            BYTE8 *tmp = m_drawingTarget.pbData + m_drawingTarget.iStrideWidth * (i+iY)  + j+iX;
            if(!fReverse)
            {
                if(*tmp > iThreshold)
                {
                    *tmp = grayValue;
                }
                else
                {
                    *tmp = 0;
                }
            }
            else
            {
                if(*tmp > iThreshold)
                {
                    *tmp = 0;
                }
                else
                {
                    *tmp = grayValue;
                }
            }
        }
    }
    return S_OK;
}

DK_IMAGE CTpGraphics::GetImage()
{
    return m_drawingTarget;
}


//TODO 没有参数检查
HRESULT CTpGraphics::BitBlt(INT32 iX,INT32 iY,INT32 iWidth,INT32 iHeight, CTpGraphics *pGraphics, INT32 iSX,INT32 iSY)
{
    if(NULL == pGraphics)
        return E_FAIL;
    DK_IMAGE srcImage = pGraphics->GetImage();
    if (NULL == m_drawingTarget.pbData)
    {
        return E_FAIL;
    }

    if (NULL == srcImage.pbData || iWidth < 0 || iHeight < 0)
    {
        return E_INVALIDARG;
    }


    INT32 nCopyWidth = iWidth; //iSrcWidth < srcImage.iWidth ? iSrcWidth : srcImage.iWidth;
    INT32 nCopyHeight = iHeight;    //iSrcHeight < iDestHeight ? iSrcHeight : iDestHeight;

    INT32 iSrcStride = srcImage.iStrideWidth;
    INT32 iDestStride = m_drawingTarget.iStrideWidth;

    BYTE8 *pbSrc = srcImage.pbData + srcImage.iStrideWidth * iSY + iSX;
    BYTE8 *pbDest = m_drawingTarget.pbData + m_drawingTarget.iStrideWidth * iY + iX;

    for(INT32 i = 0; i < nCopyHeight; i++)
    {
        memcpy(pbDest, pbSrc, nCopyWidth);
        pbSrc += iSrcStride;
        pbDest += iDestStride;
    }

    return 0;
}

HRESULT CTpGraphics::BitBltBlend(INT32 iX,INT32 iY,INT32 iWidth,INT32 iHeight, CTpGraphics *pGraphics, INT32 iSX,INT32 iSY)
{
    return 0;
}

HRESULT CTpGraphics::DrawImageFile(const wchar_t* file, const DK_BOX& srcBox, const DK_BOX& dstBox)
{
    if (NULL == file || file[0] == 0)
    {
        return E_FAIL;
    }
    DK_IMAGE imgDraw;
    DK_BITMAPBUFFER_DEV dev;
    DK_ARGBCOLOR white(0xFF, 0xFF, 0xFF, 0xFF);
    int width = dstBox.Width();
    int height = dstBox.Height();
    if (!RenderUtil::CreateRenderImageAndDevice32(
                width, height, white, &imgDraw, &dev))
    {
        return E_FAIL;
    }
    CopyImageEx(imgDraw, 0, 0, m_drawingTarget, dstBox.X0, dstBox.Y0, width, height);
    DK_FLOWRENDEROPTION renderOption;
    renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
    renderOption.pDevice = &dev;
    DKR_RenderImageFile(renderOption,
            DK_BOX(0, 0, width, height), 
            srcBox,
            1,
            file);
	IBMPProcessor* grayProcessor = BMPProcessFactory::CreateInstance(IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE);
	if(NULL == grayProcessor || !grayProcessor->Process(&imgDraw))
    {
        DK_FREE(imgDraw.pbData);
		return E_FAIL;
	}
    CopyImageEx(m_drawingTarget, dstBox.X0, dstBox.Y0, imgDraw, 0, 0, width, height);
    DK_FREE(imgDraw.pbData);
    return S_OK;
}

HRESULT CTpGraphics::DrawRectBorder(int left, int top, int right, int bottom, int thickness, dkColor color)
{
    FillRect(left, top, right, top + thickness, color);
    FillRect(left, top, left + thickness, bottom, color);
    FillRect(right - thickness, top, right, bottom, color);
    FillRect(left, bottom - thickness, right, bottom, color);
    return S_OK;
}

HRESULT CTpGraphics::ReverseRect(int left, int top, int right, int bottom)
{
    DK_RECT drawingRect = {0, 0, m_drawingTarget.iWidth, m_drawingTarget.iHeight};
    DK_RECT requiredRect = {left, top, right, bottom};

    DK_RECT targetRect = CUtility::GetIntersectRect(drawingRect, requiredRect);
    if (CUtility::IsEmptyRect(targetRect))
    {
        return S_OK;
    }
    for (int y = targetRect.top; y < targetRect.bottom; ++y)
    {
        unsigned char* begin = (m_drawingTarget.pbData + y * m_drawingTarget.iStrideWidth + targetRect.left);
        for (int x = targetRect.left; x < targetRect.right; ++x, ++begin)
        {
            *begin ^= 0xFF;
        }
    }
    return S_OK;
}

HRESULT CTpGraphics::BinarizeRect(int left, int top, int right, int bottom)
{
    DK_RECT drawingRect = {0, 0, m_drawingTarget.iWidth, m_drawingTarget.iHeight};
    DK_RECT requiredRect = {left, top, right, bottom};

    DK_RECT targetRect = CUtility::GetIntersectRect(drawingRect, requiredRect);
    if (CUtility::IsEmptyRect(targetRect))
    {
        return S_OK;
    }
    for (int y = targetRect.top; y < targetRect.bottom; ++y)
    {
        unsigned char* begin = (m_drawingTarget.pbData + y * m_drawingTarget.iStrideWidth + targetRect.left);
        for (int x = targetRect.left; x < targetRect.right; ++x, ++begin)
        {
            if (*begin & 0x80)
            {
                *begin = 0xFF;
            }
            else
            {
                *begin = 0;
            }
        }
    }
    return S_OK;
}
