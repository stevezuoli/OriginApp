//===========================================================================
// Summary:
//        IBookReader接口定义
// Version:
//    0.1 draft - creating.
//    
// Usage:
//        Null
// Remarks:
//        Null
// Date:
//        2011-11-21
// Author:
//        Chen Mao (chenm@duokan.com)
//===========================================================================
#include "BMPProcessor/IBMPProcessor.h"
#include "interface.h"

const int MAX_DARKEN_LEVEL = 5;
const unsigned int KINDLE_BLACK = 255;
const unsigned int KINDLE_WHITE = 0;

// BMPProcessFactory implementation
IBMPProcessor* BMPProcessFactory::CreateInstance(IBMPProcessor::TYPE eType, int iPara)
{
    switch(eType)
    {
    case IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE:
        return GrayScaleProcessor::GetInstance();
    case IBMPProcessor::DK_BMPPROCESSOR_SHARPEN:
        return SharpenProcessor::GetInstance();
    case IBMPProcessor::DK_BMPPROCESSOR_DARKEN:
        return DarkenProcessor::GetInstance();
        /*
	case IBMPProcessor::DK_BMPPROCESSOR_BINARIZATION:
		return BinarizationProcessor::GetInstance();
		*/
    default:
        break;
    }
    return 0;
}

// GrayScaleProcessor implementation
/*
static unsigned int AdjustArray[256] = {
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
*/

//-------------------------------------------
// Summary:
//        彩色变换会灰度处理器
//-------------------------------------------
IBMPProcessor* GrayScaleProcessor::m_pInstance = 0;

IBMPProcessor* GrayScaleProcessor::GetInstance()
{
    if (0 == m_pInstance)
    {
        m_pInstance = new GrayScaleProcessor;
    }
    return m_pInstance;
}

bool GrayScaleProcessor::Process(DK_IMAGE* pImage)
{
    if (0 == pImage || 0 == pImage->pbData)
    {
        DebugPrintf(DLC_JUGH, "GrayScaleProcessor::Process PARA IS NULL!!!");
        return false;
    }
    if (!RGBToGray(pImage))
    {
        DebugPrintf(DLC_JUGH, "GrayScaleProcessor::Process RGBToGray ERROR!!!");
        return false;
    }
    return (0 != pImage->pbData);
}

bool GrayScaleProcessor::RGBToGray(DK_IMAGE* pImage)
{
    DK_ASSERT (0 != pImage && 0 != pImage->pbData);
    if (1 == pImage->iColorChannels)
        return true;
    else if (4 != pImage->iColorChannels)
        return false;
    
    int  &lHeight   = pImage->iHeight;        
    int  &lWidth    = pImage->iWidth;        
    int  &lStride   = pImage->iStrideWidth;    
    unsigned char  *pFromData = pImage->pbData;
    unsigned char  *pToData   = DK_MALLOCZ_OBJ_N(unsigned char, lWidth*lHeight);
    if (0 == pToData)
    {
        DebugPrintf(DLC_JUGH, "GrayScaleProcessor::RGBToGray DK_MALLOCZ_OBJ_N IS NULL!!!");
        return false;
    }
    memset(pToData, 0, lWidth*lHeight);
    
    for(int i = 0; i < lHeight; i++)
    {
        for(int j = 0; j < lWidth; j++)
        {
            int   uOffset = i * lStride + j * 4;
            unsigned char& bBlue   = pFromData[uOffset + 0];
            unsigned char& bGreen  = pFromData[uOffset + 1];
            unsigned char& bRed    = pFromData[uOffset + 2];
            pToData[i * lWidth + j] = ConvertRGBToGray(i, j, ~bBlue, ~bGreen, ~bRed);
        }
    }
    pImage->iColorChannels = 1;
    pImage->iStrideWidth   = lWidth;
    pImage->pbData         = pToData;
    SafeFreePointer(pFromData);
    return true;
}

unsigned char GrayScaleProcessor::ConvertRGBToGray(int x, int y, unsigned char blue, unsigned char green, unsigned char red)
{
    unsigned int uBlue = (unsigned int)blue;
    unsigned int uGreen = (unsigned int)green;
    unsigned int uRed = (unsigned int)red;
    if(x < 0 || y < 0 || uBlue > 256 || uGreen > 256 || uRed > 256)
    {
        return (unsigned char)256;
    }

    unsigned int grayValue = ((28 * uBlue + 151 * uGreen + 77 * uRed)>>8);
    // 不使用抖动
    return ((unsigned char)grayValue & 0xF0) | ((unsigned char)grayValue >> 4);
}

// SharpenProcessor implementation

//-------------------------------------------
// Summary:
//    图像锐化处理器
//-------------------------------------------
IBMPProcessor* SharpenProcessor::m_pInstance = 0;

IBMPProcessor* SharpenProcessor::GetInstance()
{
    if (0 == m_pInstance)
    {
        m_pInstance = new SharpenProcessor;
    }
    return m_pInstance;
}

SharpenProcessor::SharpenProcessor ()
    : m_iSharpenLevel (0)
{
}

bool SharpenProcessor::Process(DK_IMAGE* pImage)
{
    if (!pImage || !pImage->pbData)
    {
        DebugPrintf(DLC_LIUHJ, "ERR - SharpenProcessor::Process INVALID_ARG");
        return false;
    }

    if (1 != pImage->iColorChannels)
    {
        DebugPrintf(DLC_LIUHJ, "ERR - SharpenProcessor::Process Only process sharpen with gray image");
        return false;
    }

    if (m_iSharpenLevel > 0)
    {
        return SharpenImage(pImage->pbData, pImage->iWidth, pImage->iHeight, pImage->iStrideWidth / pImage->iWidth, m_iSharpenLevel);
    }
    else
    {
        DebugPrintf(DLC_LIUHJ, "WARRNING - SharpenProcessor::Process Sharpen Image with level %d (out of range), do nothing", m_iSharpenLevel);
        return true;
    }
}

bool SharpenProcessor::SharpenImage(unsigned char* pBuf, unsigned int uWidth, unsigned int uHeight, unsigned int uN, int iLevel)
{
    DebugPrintf(DLC_LIUHJ, "SharpenProcessor::SharpenImage uWidth %d, uHeight %d, uN %d, iLevel %d", uWidth, uHeight, uN, iLevel);

    if(!pBuf || 0 == uWidth || 0 == uHeight)
    {
        return false;
    }

    if(uN > 1)
    {
        unsigned char* pTemSrc = pBuf;
        unsigned char* pTemDes = pBuf;
        unsigned int uX (0);
        unsigned int uY (0);
        for(uY = 0; uY < uHeight; ++uY)
        {
            pTemSrc = pBuf + (uY * uWidth) * uN;
            pTemDes = pBuf + uY * uWidth;
            for (uX = 0; uX < uWidth; ++uX)
            {
                *pTemDes = *pTemSrc;
                pTemDes++;
                pTemSrc += uN;
            }
        }
    }

    unsigned char* pResult = DK_MALLOCZ_OBJ_N (unsigned char, uWidth * uHeight);
    if(pResult)
    {
        SharpenImage3x3(pBuf, uWidth, uHeight, uN, iLevel, pResult);
        memcpy(pBuf, pResult, uWidth * uHeight * sizeof(unsigned char));
        SafeFreePointer (pResult);
        return true;
    }
    return false;
}

DK_VOID SharpenProcessor::SharpenImage3x3(unsigned char* pBuf, unsigned int uWidth, unsigned int uHeight, unsigned int uN, int iLevel, unsigned char* pResult)
{
    DebugPrintf(DLC_LIUHJ, "SharpenProcessor::SharpenImage3x3 uWidth %d, uHeight %d, uN %d, iLevel %d", uWidth, uHeight, uN, iLevel);

    int iTemplates[9] = {-1,-2,-1,
                        -2,28,-2,
                        -1,-2,-1};
    
    unsigned int i (0);
    for(i = 0; i < 9; ++i)
    {
        iTemplates[i] *= iLevel;
    }

    memcpy (pResult, pBuf, uWidth * uHeight * sizeof(unsigned char));

    unsigned char* pContent (0);
    unsigned char* pRowHead (0);
    unsigned char* pDrsContent (0);
    unsigned char* pDrsRowHead (0);
    int* pIndex (0);
    unsigned int iSum (0);
    unsigned int iOffset (0);
    unsigned int j (0);

    for(j = 1; j < uHeight - 1; ++j)
    {
        iOffset = j * uWidth;
        pRowHead = pBuf + iOffset;
        pDrsRowHead = pResult + iOffset;

        for(i = 1; i < uWidth - 1; ++i)
        {
            iSum = 0;
            pIndex = iTemplates;

            pContent =  pRowHead - uWidth + i;
            pDrsContent = pDrsRowHead + i;
            iSum += (*(pContent-1)) * (*(pIndex++));
            iSum += (*(pContent)) * (*(pIndex++));
            iSum += (*(pContent+1)) * (*(pIndex++));
            pContent += uWidth;
            iSum += (*(pContent-1)) * (*(pIndex++));
            iSum += (*(pContent)) * (*(pIndex++));
            iSum += (*(pContent+1)) * (*(pIndex++));
            pContent += uWidth;
            iSum += (*(pContent-1)) * (*(pIndex++));
            iSum += (*(pContent)) * (*(pIndex++));
            iSum += (*(pContent+1)) * (*(pIndex++));
            iSum=(iSum>>4);

            if (iSum > 255)
            {
                iSum = 255;
            }
            if (iSum <0)
            {
                iSum = 0;
            }

            *pDrsContent = iSum;
        }
    }
}


// DarkenProcessor implementation

//-------------------------------------------
// Summary:
//    图像加黑处理器
//-------------------------------------------
IBMPProcessor* DarkenProcessor::m_pInstance = 0;

IBMPProcessor* DarkenProcessor::GetInstance()
{
    if (0 == m_pInstance)
    {
        m_pInstance = new DarkenProcessor;
    }
    return m_pInstance;
}

DarkenProcessor::DarkenProcessor ()
    : m_iDarkenLevel (0)
{
}

bool DarkenProcessor::Process(DK_IMAGE* pImage)
{
    if (!pImage || !pImage->pbData)
    {
        DebugPrintf(DLC_LIUHJ, "ERR - DarkenProcessor::Process INVALID_ARG");
        return false;
    }

    if (1 != pImage->iColorChannels)
    {
        DebugPrintf(DLC_LIUHJ, "ERR - DarkenProcessor::Process Only process sharpen with gray image");
        return false;
    }

    if (m_iDarkenLevel > 0 && m_iDarkenLevel <= MAX_DARKEN_LEVEL)
    {
        return EmboldenImage(pImage->pbData, pImage->iWidth, pImage->iHeight, pImage->iStrideWidth / pImage->iWidth, m_iDarkenLevel);
    }
    else
    {
        DebugPrintf(DLC_LIUHJ, "WARRNING - DarkenProcessor::Process Darken Image with level %d (out of range), do nothing", m_iDarkenLevel);
        return true;
    }
}

bool DarkenProcessor::EmboldenImage(unsigned char* pBuf, unsigned int uWidth, unsigned int uHeight, unsigned int uN, int iLevel)
{
    DebugPrintf(DLC_LIUHJ, "DarkenProcessor::EmboldenImage uWidth %d, uHeight %d, uN %d, iLevel %d", uWidth, uHeight, uN, iLevel);

    if(!pBuf || 0 == uWidth || 0 == uHeight || iLevel > MAX_DARKEN_LEVEL)
    {
        return false;
    }

    const int TEMPLATE_RANGE = 16;
    unsigned int iT[TEMPLATE_RANGE] = {0, 4, 4, 4,
                                       3, 3, 3, 3, 
                                       1, 1, 1, 1, 
                                       1, 1, 1, 0};

    // 根据加深级别对模板进行调整
    for (int i (1); i <= iLevel; ++i)
    {
        for (int j = MAX_DARKEN_LEVEL - i + 1; j < TEMPLATE_RANGE - (MAX_DARKEN_LEVEL - i) - 1; ++j)
        {
            iT[j]++;
        }
    }

    unsigned char* pResult (0);
    pResult = DK_MALLOCZ_OBJ_N (unsigned char, uWidth * uHeight);
    if(pResult)
    {
        unsigned char* pTmpSrc = pBuf;
        unsigned char* pTmpDst = pResult;
        unsigned int y (0);
        unsigned int x (0);
        for(y = 0 ; y < uHeight; ++y )
        {
            pTmpSrc = pBuf + (y * uWidth) * uN;
            pTmpDst = pResult + y * uWidth;
            for (x = 0; x < uWidth; ++x)
            {
                *pTmpDst = *pTmpSrc;
                pTmpDst++;
                pTmpSrc += uN;
            }
        }

        unsigned char* pDst (0);
        unsigned char* pSrc (0);
        unsigned int iGray16 (0);
        for (y = 0; y < uHeight; y++)
        {
            pDst = pBuf + y * uWidth;
            pSrc = pResult + y * uWidth;
            for (x = 0; x < uWidth; x++)
            {          
                iGray16 = pSrc[x] >> 4;

                // 通过模板调整灰阶
                iGray16 += iT [iGray16];

                if (iGray16 > 15)
                {
                    iGray16 = 15;
                }
                else if (iGray16 < 0)
                {
                    iGray16 = 0;
                }

                pDst[x] = (unsigned char)iGray16 << 4 | iGray16;
            }
        }

        SafeFreePointer (pResult);
    }
    
    return true;
}

