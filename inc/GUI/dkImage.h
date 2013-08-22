#ifndef __GUI_DKIMAGE_H__
#define __GUI_DKIMAGE_H__

#include <stdint.h>
#include "dkBaseType.h"
#include "dkRect.h"
#include "Common/Defs.h"

namespace DkFormat
{
struct DK_IMAGE{
    int iColorChannels;            // 色彩通道数，目前可为1（8位灰度图）或3（24位彩色图）
    uint8_t *pbData;                // 图象数据
    int iHeight;                // 图象高度(以象素点为单位, 每象素点所占用的字节数由"图象类型"决定)
    int iWidth;                    // 图象实际宽度(以象素点为单位, 每象素点所占用的字节数由"图象类型"决定 )
    int iStrideWidth;            // 图象存储宽度(以字节为单位)

    DK_IMAGE()
        : iColorChannels(1)
          , pbData(NULL)
          , iHeight(0)
          , iWidth(0)
          , iStrideWidth(0)
    {}
    void Reset(int colorChannels, int width, int height)
    {
        if (NULL != pbData)
        {
            free(pbData);
            pbData = NULL;
        }
        iColorChannels = colorChannels;
        iWidth = width;
        iHeight = height;
        iStrideWidth = width * iColorChannels;
        int dataLen = height * iStrideWidth;
        if (dataLen > 0)
        {
            pbData = (unsigned int8_t*)malloc(dataLen);
        }
    }
    uint8_t* GetPointAddress(int x, int y) const;
    HRESULT EraseBackground(dkColor color);
    HRESULT DrawLine(int x, int y, int width, int height, int strokeStyle);
    HRESULT FillRect(int x, int y, int width, int height, dkColor brushColor);
    HRESULT Reverse();
    HRESULT ReverseRect(int x, int y, int width, int height);
};


inline DkRect DkRectFromImage(const DK_IMAGE& img)
{
    return DkRect::FromLeftTopWidthHeight(0, 0, img.iWidth, img.iHeight);
}

HRESULT CropImage(
        DK_IMAGE imgInput, 
        DK_RECT& rcCrop,
        DK_IMAGE *pimgOutput,
        int cBytesPerPixel = 1
        );

HRESULT CopyImage(
        DK_IMAGE pimgOutput,
        DK_IMAGE imgInput, 
        int cBytesPerPixel = 1
        );
HRESULT CopyImageEx(
        DK_IMAGE imgDst,
        int dstX,
        int dstY,
        DK_IMAGE imgSrc, 
        int srcX,
        int srcY,
        int width,
        int height
        );

HRESULT ReverseImage(
        DK_IMAGE imgHandle,
        int cBytesPerPixel = 1
        );
}
#endif        // #ifndef __DKIMAGE_INCLUDED__

