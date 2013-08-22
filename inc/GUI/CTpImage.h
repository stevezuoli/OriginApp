////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/inc/GUI/CTpImage.h $
// $Revision: 11 $
// Contact: genchw, yuzhan
// Latest submission: $Date: 2008/08/25 20:50:27 $ by $Author: fareast\\xzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __CTPIMAGE_H__
#define __CTPIMAGE_H__

#include "GUI/ITpImage.h"

#include <stdio.h>
#include <stdint.h>
#include "dkImage.h"
#include "KernelDef.h"

using DkFormat::DK_IMAGE;

#pragma pack(1)

struct BITMAPFILEHEADER
{
    uint16_t bfType;  //BM
    uint32_t bfSize;   //file size
    uint16_t bfReserved1;  //0
    uint16_t bfReserved2;  //0
    uint32_t bfOffBits;      //bitmap data offset to  file header
};

struct BITMAPINFOHEADER
{
    uint32_t biSize;  //this structure occupy size
    uint32_t biWidth; //bitmap width (pixel unit)
    uint32_t biHeight; //bitmap height (pixel unit)
    uint16_t biPlanes; // 1
    uint16_t biBitCount;//bits of erery pixel . must be one of follow values 1(double color)
            // 4(16color) 8(256 color) 24(true color)
    uint32_t biCompression; // bitmap compresstion type must be one of 0 (uncompress)
                      // 1(BI_RLE8) 2(BI_RLE4)
    uint32_t biSizeImage;     // bitmap size
    uint32_t biXPelsPerMeter; //bitmap horizontal resolution.
    uint32_t biYPelsPerMeter; //bitmap vertical resolution.
    uint32_t biClrUsed;    //bitmap used color number.
    uint32_t biClrImportant;//bitmap most important color number during display process.
};
#pragma pack()

class CTpImage : public ITpImage
{
public:
    CTpImage(const char *path);
    CTpImage(const DK_IMAGE* img);

    virtual ~CTpImage();
    virtual int32_t GetWidth() const;
    virtual int32_t GetHeight() const;
    bool HasAlpha() const;

private:
    DISALLOW_COPY_AND_ASSIGN(CTpImage);
    static int LoadBmpData(FILE* fpBmp, uint8_t *bmp, const BITMAPFILEHEADER *file_header, const BITMAPINFOHEADER *info_header);
    int ConvertBmpData(uint8_t *dst, const uint8_t* src, uint32_t bpp, int32_t width, int32_t height);
    bool m_hasAlpha;
};

#endif
