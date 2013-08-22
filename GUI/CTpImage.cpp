////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/src/GUI/CTpImage.cpp $
// $Revision: 8 $
// Contact: genchw, yuzhan
// Latest submission: $Date: 2007/11/29 15:08:38 $ by $Author: fareast\\genchw $
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/CTpImage.h"

#include  <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include "interface.h"
#include "Utility/PathManager.h"

using namespace std;
using namespace dk::utility;

CTpImage::CTpImage(const char* path) : m_hasAlpha(FALSE)
{
    uint8_t *temp = NULL;
    FILE* fpBmp = NULL;
    bmBits = NULL;
    if(path)
    {
        std::string ext = PathManager::GetFileExt(path);
        if (ext.c_str() && !strcasecmp("bmp", ext.c_str()))
        {

            BITMAPFILEHEADER bmp_file_header;
            BITMAPINFOHEADER bmp_info_header;

            fpBmp = fopen(path, "r");
            if (NULL == fpBmp)
            {
                goto err_ret;
            }

            DebugPrintf(DLC_GUI, "sizeof(BITMAPFILEHEADER)=%d, sizeof(BITMAPINFOHEADER)=%d", sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
            if (0 >= fread(&bmp_file_header, sizeof(BITMAPFILEHEADER), 1, fpBmp))
            {
                goto err_ret;
            }

            if (0 >= fread(&bmp_info_header, sizeof(BITMAPINFOHEADER), 1, fpBmp))
            {
                goto err_ret;
            }

            bmWidth = bmp_info_header.biWidth;
            bmHeight = bmp_info_header.biHeight;
            bmPitch = bmp_info_header.biBitCount >> 3;
            if(bmPitch > 4)
                goto err_ret;
            int nlinewidth = bmp_info_header.biWidth * bmp_info_header.biBitCount  /  8;
            nlinewidth = (nlinewidth + 3) & ~3;

            temp = new uint8_t [nlinewidth * bmHeight];
            if( 0 > LoadBmpData(fpBmp, temp, &bmp_file_header, &bmp_info_header))
            {
                DebugPrintf(DLC_ERROR, "load bmp data ");
                goto err_ret;
            }

            bmBits = new uint8_t[bmWidth * bmHeight * bmPitch];
            if ( 0 > ConvertBmpData(bmBits, temp, bmp_info_header.biBitCount,
                bmp_info_header.biWidth, bmp_info_header.biHeight))
            {
                DebugPrintf(DLC_ERROR, "convert bmp data ");
                goto err_ret;
            }
        }

err_ret:
        if (temp)
        {
            delete []temp;
        }

        if (fpBmp)
        {
            fclose(fpBmp);
        }
    }
}

CTpImage::CTpImage(const DK_IMAGE* img)
{
    bmWidth = img->iWidth;
    bmHeight = img->iHeight;
    bmPitch = img->iColorChannels;
    int lineWidth = bmWidth * img->iColorChannels;
    //lineWidth = (lineWidth + 3) & (~3);
    bmBits = new uint8_t[bmHeight * lineWidth];
    if (NULL == bmBits)
    {
        return;
    }
    memset(bmBits, 0, bmHeight * lineWidth);

    for (int i = 0; i < (int)bmHeight; ++i)
    {
        memcpy(bmBits + lineWidth * i, img->pbData + img->iStrideWidth * i, lineWidth);
        for (int j = 0; j < lineWidth; ++j)
        {
            bmBits[lineWidth * i +j] ^= 0xF0;
            bmBits[lineWidth * i +j] |= 0xF;

        }
    }
    m_hasAlpha = false;
} 

int CTpImage::ConvertBmpData(uint8_t *dst, const uint8_t *src, uint32_t bpp, int32_t width, int32_t height) 
{
    int i = 0;
    int dstWidth = width * bpp / 8;
    int srcWidth = (dstWidth + 3) & ~3;
    for(i = 0; i < height; i++)
    {
        memcpy(dst + i * dstWidth, src + (height - i - 1) * srcWidth, dstWidth);
    }

    return 0;
}

int CTpImage::LoadBmpData(FILE* fpBmp, uint8_t *bmp, const BITMAPFILEHEADER *file_header, const BITMAPINFOHEADER *info_header)
{
    int nlinewidth = 0;
    if (0 > fseek(fpBmp, 0, SEEK_SET))
    {
        DebugPrintf(DLC_ERROR, "Seek to start");
        return 0;
    }

    if (0 > fseek(fpBmp, file_header->bfOffBits, SEEK_SET))
    {
        DebugPrintf(DLC_ERROR, "Read data");
        return 0;
    }
    // begin to read real bitmap data
    nlinewidth = info_header->biWidth * info_header->biBitCount / 8;
    nlinewidth = (nlinewidth + 3) & ~3;
    if (0> fread((char*)bmp, 1, nlinewidth  * info_header->biHeight, fpBmp))
    {
        return 0;
    }
    return 0;
}

CTpImage::~CTpImage()
{
    if(bmBits)
    {
        delete [] bmBits;
        bmBits = NULL;
    }

}

int32_t CTpImage::GetWidth() const
{
    return bmWidth;
}

int32_t CTpImage::GetHeight() const
{
    return bmHeight;
}

bool CTpImage::HasAlpha() const
{
    return m_hasAlpha;
}


