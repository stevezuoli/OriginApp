#include "Utility/RenderUtil.h"
#include "GUI/CTpImage.h"

namespace dk
{
namespace utility
{
bool RenderUtil::CreateRenderImageAndDevice32(
        int width,
        int height,
        const DK_ARGBCOLOR& backgroundColor,
        DK_IMAGE* image,
        DK_BITMAPBUFFER_DEV* dev)
{
    if (width <= 0 || height <= 0 || NULL == image || NULL == dev)
    {
        return false;
    }
	image->iColorChannels = 4;
	image->iWidth         = width;
	image->iHeight        = height;
	image->iStrideWidth   = image->iWidth * image->iColorChannels;
    size_t dataLen = image->iStrideWidth * image->iHeight;
    image->pbData = DK_MALLOCZ_OBJ_N(DK_BYTE, dataLen);
    if (NULL == image->pbData)
    {
        return false;
    }

    dev->lWidth       = width;
    dev->lHeight      = height;
    dev->nPixelFormat = DK_PIXELFORMAT_RGB32;
    dev->lStride      = width * 4;
    dev->nDPI         = RenderConst::SCREEN_DPI;
    dev->pbyData      = image->pbData;
    if (backgroundColor == DK_ARGBCOLOR(0xFF, 0xFF, 0xFF, 0xFF))
    {
        memset(dev->pbyData, 0xFF, width * 4 * height);
    }
    else
    {
        uint8_t* data = dev->pbyData;
        for (int i = 0; i < width * height; ++i)
        {
            data[0] = backgroundColor.rgbBlue;
            data[1] = backgroundColor.rgbGreen;
            data[2] = backgroundColor.rgbRed;
            data[3] = backgroundColor.rgbAlpha;
			data += 4;
        }
    }

    return true;
}

bool RenderUtil::SaveDKImage(DkFormat::DK_IMAGE image, const char* filename)
{
    BITMAPFILEHEADER bmp_file_header;
    int widthRound = (image.iWidth + 3) & 0xFFFFFFFC;
    memset(&bmp_file_header, 0, sizeof(BITMAPFILEHEADER));
    bmp_file_header.bfOffBits = 0x36 + 256*4;
    bmp_file_header.bfSize = widthRound * image.iHeight + 0x36 + 256*4;
    bmp_file_header.bfType = 0x4d42;

    BITMAPINFOHEADER bmp_info_header;
    memset(&bmp_info_header, 0, sizeof(BITMAPINFOHEADER));
    bmp_info_header.biSize = sizeof(BITMAPINFOHEADER);
    bmp_info_header.biWidth = image.iWidth;
    bmp_info_header.biHeight = image.iHeight;
    bmp_info_header.biPlanes = 1;
    bmp_info_header.biBitCount = 8;
    bmp_info_header.biSizeImage = widthRound * image.iHeight;
    bmp_info_header.biClrUsed = 256;

    int plus[256] = {0};
    for ( int i=0; i<256; ++i)
    {
        memset(plus+i, 255-i, 4);
    }

    FILE* fp = fopen(filename, "w+");
    char* data = NULL;
    if (NULL == fp)
    {
        goto err0;
    }
    data = new char[widthRound* image.iHeight];
    fwrite(&bmp_file_header, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&bmp_info_header, sizeof(BITMAPINFOHEADER), 1, fp);
    fwrite(plus, 256*4, 1, fp);
    if (data)
    {
        int i = image.iHeight;
        while (i > 0)
        {
            memcpy(data + (i-1)*widthRound, image.pbData + (image.iHeight - i)*image.iStrideWidth, image.iWidth);
            i--;
        }
        fwrite(data, widthRound*image.iHeight, 1, fp);
        delete []data;
        data = NULL;
    }
    else
    {
        goto err1;
    }

    fclose(fp);
    return true;
err1:
    fclose(fp);
err0:
    return false;
}
} // namespace utility
} // namespace dk
