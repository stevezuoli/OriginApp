#include "DkImageHandler.h"
#include "DkImageHelper.h"
#include "DKRAPI.h"
#include "DkStreamFactory.h"
#include "interface.h"
#include "dkWinError.h"

using namespace DkFormat;

DkImageHandler::DkImageHandler()
    : m_pStream(NULL)
    , m_iWidth(-1)
    , m_iHeight(-1)
{
    memset(&m_image, 0, sizeof(m_image));
}

DkImageHandler::~DkImageHandler()
{
    SafeDeletePointer(m_pStream);
    SafeFreePointer(m_image.pbData);
}

bool DkImageHandler::LoadFromFile(const char *pFileName)
{
    if (NULL == pFileName)
    {
        return false;
    }

    m_pStream = DkStreamFactory::GetFileStream(pFileName);
    return Initialize();
}

bool DkImageHandler::LoadFromStream(IDkStream *pStream)
{
    if (NULL == pStream)
    {
        return false;
    }

    m_pStream = pStream->CreateCopy();
    return Initialize();
}

bool DkImageHandler::CropAndResize(DK_RECT srcRect, int iNewWidth, int iNewHeight, int iColorChannels)
{
    if (srcRect.left < 0 || srcRect.top < 0 
        || srcRect.right <= srcRect.left || srcRect.bottom <= srcRect.top 
        || iNewWidth <= 0 || iNewHeight <= 0)
    {
        return false;
    }

    m_pStream->Reset();
    if (!ResetImage(&m_image, iColorChannels, iNewWidth, iNewHeight))
    {
        return false;
    }

    // DEV信息根据DK_IMAGE来生成
    DK_BITMAPBUFFER_DEV dev;
    dev.lWidth       = m_image.iWidth;
    dev.lHeight      = m_image.iHeight;
    dev.nPixelFormat = DK_PIXELFORMAT_RGB32;
    dev.lStride      = m_image.iStrideWidth;
    dev.nDPI         = RenderConst::SCREEN_DPI;
    dev.pbyData      = m_image.pbData;

    DK_RENDERGAMMA  gma;
    gma.dTextGamma = 1.0;

    DK_FLOWRENDEROPTION renderOption;
    renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
    renderOption.pDevice     = &dev;
    renderOption.gamma       = gma;

    DK_BOX srcBox(srcRect.left, srcRect.top, srcRect.right, srcRect.bottom);
    DK_BOX dstBox(0.0, 0.0, m_image.iWidth, m_image.iHeight);
    if (DKR_OK != DKR_RenderImageStream(renderOption, dstBox, srcBox, 1.0, m_pStream))
    {
        DebugPrintf(DLC_ZHAIGH, "DkImageHandler::%s() DKR_RenderImageStream fail", __FUNCTION__);
        ResetImage(&m_image);
        return false;
    }

    TransformImage();
    return true;
}

bool DkImageHandler::Rotate(int iAngle)
{
    if (NULL == m_image.pbData || 1 != m_image.iColorChannels)
    {
        return false;
    }

    if (0 == iAngle)
    {
        return true;
    }

    DK_IMAGE newImage;
    memset(&newImage, 0, sizeof(newImage));

    bool ret = false;
    switch (iAngle)
    {
    case 270:
        ret = ResetImage(&newImage, 1, m_image.iHeight, m_image.iWidth);
        if (ret)
        {
            for (int i = 0; i < newImage.iHeight; ++i)
            {
                for (int j = 0; j < newImage.iWidth; ++j)
                {
                    newImage.pbData[i * newImage.iWidth + j] = 
                        m_image.pbData[(m_image.iHeight - 1 - j) * m_image.iWidth + i];
                }
            }
        }

        break;
    case 180:
        ret = ResetImage(&newImage, 1, m_image.iWidth, m_image.iHeight);
        if (ret)
        {
            for (int i = 0; i < newImage.iHeight; ++i)
            {
                for (int j = 0; j < newImage.iWidth; ++j)
                {
                    newImage.pbData[i * newImage.iWidth + j] = 
                        m_image.pbData[(m_image.iHeight - 1 - i) * m_image.iWidth + (m_image.iWidth - 1 - j)];
                }
            }
        }

        break;
    case 90:
        ret = ResetImage(&newImage, 1, m_image.iHeight, m_image.iWidth);
        if (ret)
        {
            for (int i = 0; i < newImage.iHeight; ++i)
            {
                for (int j = 0; j < newImage.iWidth; ++j)
                {
                    newImage.pbData[i * newImage.iWidth + j] = 
                        m_image.pbData[j * m_image.iWidth + (m_image.iWidth - 1 - i)];
                }
            }
        }

        break;
    default:
        break;
    }

    if (ret)
    {
        m_image.iWidth = newImage.iWidth;
        m_image.iStrideWidth = newImage.iStrideWidth;
        m_image.iHeight = newImage.iHeight;
        memcpy(m_image.pbData, newImage.pbData, m_image.iStrideWidth * m_image.iHeight);
    }

    return ret;
}

bool DkImageHandler::GetImage(DK_IMAGE *pImg)
{
    if (NULL == pImg 
        || NULL == pImg->pbData 
        || NULL == m_image.pbData)
    {
        DebugPrintf(DLC_ZHAIGH, "DkImageHandler::%s() Fail", __FUNCTION__);
        return false;
    }

    return (S_OK == CopyImage(*pImg, m_image));
}

bool DkImageHandler::Initialize()
{
    if (NULL == m_pStream)
    {
        DebugPrintf(DLC_ZHAIGH, "DkImageHandler::%s() m_pStream is Null", __FUNCTION__);
        return false;
    }

    if (DKR_OK != m_pStream->Open())
    {
        DebugPrintf(DLC_ZHAIGH, "DkImageHandler::%s() m_pStream Open Fail", __FUNCTION__);
        return false;
    }

    unsigned int uWidth = 0;
    unsigned int uHeight = 0;
    if (DKR_OK == DKR_AnalyseImageStream(m_pStream, &uWidth, &uHeight))
    {
        m_iWidth = uWidth;
        m_iHeight = uHeight;
    }

    DebugPrintf(DLC_ZHAIGH, "DkImageHandler::%s() m_iWidth %d, m_iHeight %d", __FUNCTION__, m_iWidth, m_iHeight);
    return (DKR_OK == m_pStream->Reset());
}

bool DkImageHandler::ResetImage(DK_IMAGE *pImage, int iColorChannels, int iWidth, int iHeight)
{
    if (NULL == pImage || iColorChannels < 0 || iWidth < 0 || iHeight < 0)
    {
        return false;
    }

    pImage->iColorChannels = iColorChannels;
    pImage->iWidth = iWidth;
    pImage->iStrideWidth = pImage->iWidth * pImage->iColorChannels;
    pImage->iHeight = iHeight;

    SafeFreePointer(pImage->pbData);
    if (pImage->iStrideWidth * pImage->iHeight > 0)
    {
        pImage->pbData = DK_MALLOCZ_OBJ_N(BYTE, pImage->iStrideWidth * pImage->iHeight);
        if (NULL == pImage->pbData)
        {
            return false;
        }
    }

    return true;
}

void DkImageHandler::TransformImage()
{
    DK_IMAGE image;
    memset(&image, 0, sizeof(image));
    if (!ResetImage(&image, 1, m_image.iWidth, m_image.iHeight))
    {
        return;
    }

    if (DkImageHelper::DitherRGB32ToGray16(m_image, &image))
    {
        SafeFreePointer(m_image.pbData);
        m_image = image;
    }
    else
    {
        SafeFreePointer(image.pbData);
    }
}
