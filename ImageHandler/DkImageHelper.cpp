#include <string.h>
#include "Utility.h"
#include "dkWinError.h"
#include "DkImageHelper.h"
#include "DkImageHandler.h"

using namespace DkFormat;

static BYTE bayer4[4][4] = 
{
    { 0,  8,  2, 10}, 
    {12,  4, 14,  6}, 
    { 3, 11,  1,  9}, 
    {15,  7, 13,  5}
};

bool DkImageHelper::DitherRGB32ToGray16(const DK_IMAGE &srcImage, DK_IMAGE *pDstImage)
{
    if (NULL == pDstImage || 1 != pDstImage->iColorChannels || NULL == pDstImage->pbData 
        || 4 != srcImage.iColorChannels || NULL == srcImage.pbData)
    {
        return false;
    }

    unsigned char *pIn = srcImage.pbData;
    unsigned char *pOut = pDstImage->pbData;
    for (int h = 0; h < pDstImage->iHeight; ++h)
    {
        for (int w = 0; w < pDstImage->iWidth; ++w)
        {
            BYTE b = *(pIn++);
            BYTE g = *(pIn++);
            BYTE r = *(pIn++);
            ++pIn;
            BYTE gray256 = (r * 38 + g * 75 + b * 15) >> 7;
            BYTE gray16 = 0;
            if (gray256 < 0x08)
            {
                gray16 = 0x00;
            }
            else if (gray256 >= 0xF8)
            {
                gray16 = 0x0F;
            }
            else
            {
                gray16 = (gray256 - 0x08) >> 4;
                BYTE error = (gray256 - 0x08) & 0x0F;
                if (error > bayer4[h & 0x03][w & 0x03])
                {
                    ++gray16;
                }
            }

            *(pOut++) = ~((gray16 << 4) | gray16);
        }
    }

    return true;
}

bool DkImageHelper::ConvertRGB32ToGray256(DK_IMAGE *pImage)
{
    if (NULL == pImage || 4 != pImage->iColorChannels || NULL == pImage->pbData)
    {
        return false;
    }

    unsigned char *pNewData = (unsigned char *)malloc(pImage->iHeight * pImage->iWidth);
    if (NULL == pNewData)
    {
        return false;
    }

    unsigned char *pIn = pImage->pbData;
    unsigned char *pOut = pNewData;
    for (int h = 0; h < pImage->iHeight; ++h)
    {
        for (int w = 0; w < pImage->iWidth; ++w)
        {
            BYTE b = *(pIn++);
            BYTE g = *(pIn++);
            BYTE r = *(pIn++);
            ++pIn;
            *(pOut++) = ~((r * 38 + g * 75 + b * 15) >> 7);
        }
    }

    pImage->iColorChannels = 1;
    pImage->iStrideWidth = pImage->iWidth;
    free(pImage->pbData);
    pImage->pbData = pNewData;
    return true;
}

void DkImageHelper::ConvertGray256ToGray16(const DK_IMAGE &image)
{
    if (1 != image.iColorChannels || NULL == image.pbData)
    {
        return;
    }

    for (int h = 0; h < image.iHeight; ++h)
    {
        for (int w = 0; w < image.iWidth; ++w)
        {
            BYTE &gray256 = image.pbData[h * image.iStrideWidth + w];
            gray256 = (gray256 & 0xF0) | (gray256 >> 4);
        }
    }
}

void DkImageHelper::ConvertGray256ToBinary(const DK_IMAGE &image)
{
    if (1 != image.iColorChannels || NULL == image.pbData)
    {
        return;
    }

    for (int h = 0; h < image.iHeight; ++h)
    {
        for (int w = 0; w < image.iWidth; ++w)
        {
            BYTE &gray256 = image.pbData[h * image.iStrideWidth + w];
            gray256 = (gray256 >= 0x80)? 0xFF: 0x00;
        }
    }
}

void DkImageHelper::DitherGray256ToGray16(const DK_IMAGE &image)
{
    if (1 != image.iColorChannels || NULL == image.pbData)
    {
        return;
    }

    for (int h = 0; h < image.iHeight; ++h)
    {
        for (int w = 0; w < image.iWidth; ++w)
        {
            BYTE &gray256 = image.pbData[h * image.iStrideWidth + w];
            BYTE gray16 = 0;
            if (gray256 < 0x08)
            {
                gray16 = 0x00;
            }
            else if (gray256 >= 0xF8)
            {
                gray16 = 0x0F;
            }
            else
            {
                gray16 = (gray256 - 0x08) >> 4;
                BYTE error = (gray256 - 0x08) & 0x0F;
                if (error > bayer4[h & 0x03][w & 0x03])
                {
                    ++gray16;
                }
            }

            gray256 = (gray16 << 4) | gray16;
        }
    }
}

static UINT AdjustArray[256] = 
{
      0, 235,  59, 219,  15, 231,  55, 215,   2, 232,  56, 217,  12, 229,  52, 213,
    128,  64, 187, 123, 143,  79, 183, 119, 130,  66, 184, 120, 140,  76, 180, 116,
     33, 192,  16, 251,  47, 207,  31, 247,  34, 194,  18, 248,  44, 204,  28, 244,
    161,  97, 144,  80, 175, 111, 159,  95, 162,  98, 146,  82, 172, 108, 156,  92,
      8, 225,  48, 208,   5, 239,  63, 223,  10, 226,  50, 210,   6, 236,  60, 220,
    136,  72, 176, 112, 133,  69, 191, 127, 138,  74, 178, 114, 134,  70, 188, 124,
     41, 200,  24, 240,  36, 197,  20, 255,  42, 202,  26, 242,  38, 198,  22, 252,
    169, 105, 152,  88, 164, 100, 148,  84, 170, 106, 154,  90, 166, 102, 150,  86,
      3, 233,  57, 216,  13, 228,  53, 212,   1, 234,  58, 218,  14, 230,  54, 214,
    131,  67, 185, 121, 141,  77, 181, 117, 129,  65, 186, 122, 142,  78, 182, 118,
     35, 195,  19, 249,  45, 205,  29, 245,  32, 193,  17, 250,  46, 206,  30, 246,
    163,  99, 147,  83, 173, 109, 157,  93, 160,  96, 145,  81, 174, 110, 158,  94,
     11, 227,  51, 211,   7, 237,  61, 221,   9, 224,  49, 209,   4, 238,  62, 222,
    139,  75, 179, 115, 135,  71, 189, 125, 137,  73, 177, 113, 132,  68, 190, 126,
     43, 203,  27, 243,  39, 199,  23, 253,  40, 201,  25, 241,  37, 196,  21, 254,
    171, 107, 155,  91, 167, 103, 151,  87, 168, 104, 153,  89, 165, 101, 149,  85
};

void DkImageHelper::DitherGray256ToBinary(const DK_IMAGE &image)
{
    if (1 != image.iColorChannels || NULL == image.pbData)
    {
        return;
    }

    for (int h = 0; h < image.iHeight; ++h)
    {
        for (int w = 0; w < image.iWidth; ++w)
        {
            BYTE &gray256 = image.pbData[h * image.iStrideWidth + w];
            gray256 = (gray256 > AdjustArray[((h & 0xF) << 4) + (w & 0xF)])? 0xFF: 0x00;
        }
    }
}

ImageHandler * DkImageHelper::CreateImageHandlerInstance(const char *pFilePath)
{
    if(NULL == pFilePath || 0 == strlen(pFilePath))
    {
        return NULL;
    }

    ImageHandler *pImgHandler = new DkImageHandler();
    if (pImgHandler)
    {
        if (!(pImgHandler->LoadFromFile(pFilePath)))
        {
            delete pImgHandler;
            pImgHandler = NULL;
        }
    }

    return pImgHandler;
}

ImageHandler * DkImageHelper::CreateImageHandlerInstance(IDkStream *pImgStream)
{
    if(NULL == pImgStream)
    {
        return NULL;
    }

    ImageHandler *pImgHandler = new DkImageHandler();
    if (pImgHandler)
    {
        if (!(pImgHandler->LoadFromStream(pImgStream)))
        {
            delete pImgHandler;
            pImgHandler = NULL;
        }
    }

    return pImgHandler;
}
