#include "GUI/dkImage.h"
#include "dkWinError.h"
#include "interface.h"
#include <string.h>
#include "Utility.h"
#include "Public/Base/TPDef.h"

namespace DkFormat
{
// 人眼对亮度的感知： Gray = 0.299 * R + 0.587 * G + 0.114 * B  =>  1024 * Gray = 306 * R + 601 * G + 117 *B
static BYTE8 COLORTOGRAY32(BYTE8 * COLOR)
{
    return ((COLOR[2] * 306 + COLOR[1] * 601 + COLOR[0] * 117) >> 10);
}

HRESULT CropImage(
    DK_IMAGE imgInput,
    DK_RECT& rcCrop,
    DK_IMAGE *pimgOutput,
    int cBytesPerPixel
)
{
    if (rcCrop.right<=rcCrop.left || rcCrop.bottom<=rcCrop.top ||
        rcCrop.left < 0 || rcCrop.top < 0 ||
        rcCrop.right > imgInput.iWidth ||
        rcCrop.bottom > imgInput.iHeight)
    {

        return E_INVALIDARG;
    }

    int nOffset = (imgInput.iStrideWidth * rcCrop.top + rcCrop.left) *
        cBytesPerPixel;

    pimgOutput->pbData = imgInput.pbData + nOffset;
    pimgOutput->iWidth = rcCrop.right - rcCrop.left;
    pimgOutput->iHeight = rcCrop.bottom - rcCrop.top;
    pimgOutput->iStrideWidth = imgInput.iStrideWidth;
    pimgOutput->iColorChannels = imgInput.iColorChannels;

    return S_OK;
}

HRESULT CopyImage(
        DK_IMAGE imgOutput,
        DK_IMAGE imgInput,
        int cBytesPerPixel
    )
{
    if (NULL == imgOutput.pbData ||     NULL == imgInput.pbData)
        return E_INVALIDARG;

    if (imgInput.iWidth != imgOutput.iWidth   ||
        imgInput.iHeight != imgOutput.iHeight ||
        imgInput.iColorChannels != imgOutput.iColorChannels)
    {
        return E_INVALIDARG;
    }

    BYTE8 * inputOffset = imgInput.pbData;
    BYTE8 *outputOffset = imgOutput.pbData;

    int srcShiftWidth = imgInput.iStrideWidth * cBytesPerPixel;
    int desShiftWidth = imgOutput.iStrideWidth * cBytesPerPixel;

    int copyWidth = imgInput.iWidth * cBytesPerPixel;

    for (int iLine =0; iLine< imgInput.iHeight; iLine++)
    {
        memcpy(outputOffset , inputOffset, copyWidth);
        outputOffset = outputOffset + desShiftWidth;
        inputOffset = inputOffset + srcShiftWidth;
    }

    return S_OK;
}

HRESULT CopyImageEx(
        DK_IMAGE imgDst,
        int dstX,
        int dstY,
        DK_IMAGE imgSrc,
        int srcX,
        int srcY,
        int width,
        int height
    )
{
    if (NULL == imgDst.pbData || NULL == imgSrc.pbData)
    {
        return E_INVALIDARG;
    }
    if (dstX >= imgDst.iWidth || dstY >= imgDst.iHeight)
    {
        return E_INVALIDARG;
    }
    if (srcX >= imgSrc.iWidth || srcY >= imgSrc.iHeight)
    {
        return E_INVALIDARG;
    }
    if (imgSrc.iColorChannels != imgDst.iColorChannels)
    {
        if (!(1 == imgSrc.iColorChannels && 4 == imgDst.iColorChannels))
        {
            return E_INVALIDARG;
        }
    }
    if (width > imgDst.iWidth - dstX)
    {
        width = imgDst.iWidth - dstX;
    }
    if (width > imgSrc.iWidth - srcX)
    {
        width = imgSrc.iWidth - srcX;
    }
    if (height > imgDst.iHeight - dstY)
    {
        height = imgDst.iHeight - dstY;
    }
    if (height > imgSrc.iHeight - srcY)
    {
        height = imgSrc.iHeight - srcY;
    }


    BYTE8 * srcOffset = imgSrc.pbData
        + srcY * imgSrc.iStrideWidth
        + srcX * imgSrc.iColorChannels;
    BYTE8* dstOffset = imgDst.pbData
        + dstY * imgDst.iStrideWidth
        + dstX * imgDst.iColorChannels;
    if (1 == imgSrc.iColorChannels && 4 == imgDst.iColorChannels)
    {
        for (int y = 0; y < height; ++y)
        {
            int* dstData = (int*)dstOffset;
            for (int x = 0; x < width; ++x, ++dstData)
            {
                unsigned char c = ((~srcOffset[x]) & 0xff);
                *dstData = (c | (c << 8) | (c << 16));
            }
            srcOffset += imgSrc.iStrideWidth;
            dstOffset += imgDst.iStrideWidth;
        }
        return S_OK;
    }

    int copyWidth = width * imgSrc.iColorChannels;

    for (int y = 0; y < height; ++y)
    {
        memcpy(dstOffset, srcOffset, copyWidth);
        srcOffset += imgSrc.iStrideWidth;
        dstOffset += imgDst.iStrideWidth;
    }

    return S_OK;
}


HRESULT ReverseImage(
        DK_IMAGE imgHandle,
        int cBytesPerPixel
    )
{
    if (NULL == imgHandle.pbData)
    {
        return E_INVALIDARG;
    }

    if (imgHandle.iWidth <= 0 ||
        imgHandle.iHeight <= 0 ||
        imgHandle.iColorChannels <= 0)
    {
        return E_INVALIDARG;
    }

    BYTE8 * pOffset = imgHandle.pbData;

    int     shiftWidth = imgHandle.iStrideWidth * cBytesPerPixel;
    int     reverseWidth = imgHandle.iWidth * cBytesPerPixel;

    for (int iLine =0; iLine< imgHandle.iHeight; iLine++)
    {
        for(int iPixiel =0 ; iPixiel < reverseWidth ; iPixiel++)
        {
            *(pOffset + iPixiel) = 255 - *(pOffset + iPixiel);
        }

        pOffset = pOffset + shiftWidth;
    }

    return S_OK;
}

HRESULT DK_IMAGE::EraseBackground(dkColor color)
{
    return FillRect(0, 0, iWidth, iHeight, color);
}

HRESULT DK_IMAGE::FillRect(int x, int y, int width, int height, dkColor brushColor)
{
    if (NULL == pbData || 1 != iColorChannels)
    {
        return E_FAIL;
    }

    DK_RECT srcRect = {0, 0, iWidth, iHeight};
    DK_RECT dstRect = {x, y, x + width, y + height};

    DK_RECT targetRect = srcRect.Intersect(dstRect);
    if (!targetRect.IsEmpty())
    {
        uint8_t * pBegin = GetPointAddress(targetRect.left, targetRect.top);
        int iFillWidth = targetRect.GetWidth();
        int iFillHeight = targetRect.GetHeight();
        int iGrayValue = COLORTOGRAY32((uint8_t*)&brushColor) & 0xFF;
        for(int i = 0; i < iFillHeight; i++)
        {
            memset(pBegin, iGrayValue, iFillWidth);
            pBegin += iStrideWidth;
        }
    }
    return S_OK;
}

uint8_t* DK_IMAGE::GetPointAddress(int x, int y) const
{
    return pbData + y * iStrideWidth + x * iColorChannels;
}

HRESULT DK_IMAGE::Reverse()
{
    return ReverseRect(0, 0, iWidth, iHeight);
}

HRESULT DK_IMAGE::ReverseRect(int x, int y, int width, int height)
{
    if (NULL == pbData || 1 != iColorChannels)
    {
        return E_FAIL;
    }

    DK_RECT srcRect = {0, 0, iWidth, iHeight};
    DK_RECT dstRect = {x, y, x + width, y + height};
    DK_RECT targetRect = srcRect.Intersect(dstRect);

    if (!targetRect.IsEmpty())
    {
        uint8_t* begin = GetPointAddress(targetRect.left, targetRect.top);
        int fillWidth = targetRect.GetWidth();
        int fillHeight = targetRect.GetHeight();
        for(int iy = 0; iy < fillHeight; iy++, begin += iStrideWidth)
        {
            for (int ix = 0; ix < fillWidth; ++ix)
            {
                begin[ix] = 0xFF - begin[ix];
            }
        }
    }
    return S_OK;
}

HRESULT DK_IMAGE::DrawLine(int x, int y, int width, int height, int strokeStyle)
{
    if (NULL == pbData || 1 != iColorChannels)
    {
        return E_FAIL;
    }

    unsigned short DrawColor = 0;
    switch (strokeStyle)
    {
    case SOLID_STROKE:
        DrawColor = 0xffff;
        break;
    case DOTTED_STROKE:
        DrawColor = 0xff00;
        break;
    default:
        return E_INVALIDARG;
        break;
    }

    DK_RECT srcRect = {0, 0, iWidth, iHeight};
    DK_RECT dstRect = {x, y, x + width, y + height};
    DK_RECT targetRect = srcRect.Intersect(dstRect);

    if (!targetRect.IsEmpty())
    {
        int drawWidth = targetRect.GetWidth() >> 1;
        bool fillAtTail = targetRect.GetWidth() & 1;
        if (drawWidth > 0)
        {
            unsigned short * begin = NULL;
            for(int iy = targetRect.top; iy < targetRect.bottom; ++iy)
            {
                begin = (unsigned short*)GetPointAddress(targetRect.left, iy);

                for(int ix = 0; ix < drawWidth ; ix++)
                {
                    begin[ix] = DrawColor;
                    if (strokeStyle == DOTTED_STROKE)
                    {
                        ix++;
                    }
                }
                if (fillAtTail)
                {
                    *(unsigned char*)(begin + drawWidth) = 0xFF;
                }
            }
        }
        else
        {
            for(int iy = targetRect.top; iy < targetRect.bottom; ++iy)
            {
                *GetPointAddress(targetRect.left, iy) = 0xFF;
            }
        }
    }

    return S_OK;
}

} // namespace DkFormat

