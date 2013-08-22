// SegmentImpl.cpp : Defines the entry point for the console application.
//
//#include "stdafx.h"
#include "BookReader/SegmentImpl.h"
#include "KernelDef.h"

HRESULT CalcBinTh(BYTE8 *pbGray, int iWidth, int iHeight, int iGrayStride, int iStepW, int &iGrayTh, bool fAdjust, int &iAvg, int *paiImhistCenter)
{
    if (NULL == pbGray || NULL == paiImhistCenter)
    {
        return E_POINTER;
    }
    // 计算中心部分的直方图
    int i(0), j(0), iPosY(0), iTemp1(0), iTemp2(0), iTemp3(0), iTemp4(0);
    BYTE *pbPosY(NULL), *pbPosX(NULL);
    for (i = 0, pbPosY = pbGray; i < iHeight; i++, pbPosY += iGrayStride)
    {
        for (j = 0, pbPosX = pbPosY; j < iWidth; j++, pbPosX += iStepW)
        {
            paiImhistCenter[*pbPosX]++;
        }
    }
    j = (iWidth * iHeight * 26) >> 8;// * 0.1
    iPosY = 0;
    iGrayTh = 128;
    for (i = 0; i < 255; i++ )
    {
        iPosY += paiImhistCenter[i];
        if ( iPosY > j )
        {
            iGrayTh = i;
            break;
        }
    }    
    //计算阈值
    while (true)
    {
        iTemp1 = 0, iTemp2 = 0, iTemp3 = 0, iTemp4 = 0;
        for (i = 0; i <= iGrayTh; i++)
        {
            iTemp1 += paiImhistCenter[i] * i;
        }
        for (i = 0; i <= iGrayTh; i++)
        {
            iTemp2 += paiImhistCenter[i];
        }
        for (i = iGrayTh + 1; i < 256; i++)
        {
            iTemp3 += paiImhistCenter[i] * i;
        }
        for (i = iGrayTh + 1; i < 256; i++)
        {
            iTemp4 += paiImhistCenter[i];
        }
        if (0 == iTemp2 || 0 == iTemp4)
        {
            break;
        }
        iPosY = ( ( (int)( (float)iTemp1 / iTemp2 + (float)iTemp3 / iTemp4 ) ) >> 1 );
        if (iPosY == iGrayTh)
        {
            break;
        }
        else
        {
            iGrayTh = iPosY;
        }
    }
    
    int iGrayOld(iGrayTh);
    iTemp1 = 0, iTemp2 = 0;
    if (fAdjust)
    {
        for (i = 0, pbPosY = pbGray; i < iHeight; i++, pbPosY += iGrayStride)
        {
            for (j = 0, pbPosX = pbPosY; j < iWidth; j++, pbPosX += iStepW)
            {
                iTemp1 += *pbPosX;
            }
        }
        iAvg = iTemp1 / (iWidth * iHeight);
    }
    if (iGrayTh == iGrayOld)
    {
        iGrayTh--;
    }
    return S_OK;
}

HRESULT Gray2Bin(BYTE8 *pbBin, BYTE8 *pbGray, int iWidthOri, int iHeightOri, int iGrayStrideOri, int &iResult, bool fAdjust, int iMinTh, int iDownSample,
             int *piThreshold = NULL)
{
    iResult = 1;
    if (pbBin == NULL || pbGray == NULL)
    {
        iResult = 0;
        return E_POINTER;
    }
    
    int iWidth(iWidthOri), iHeight(iHeightOri), iGrayStride(iGrayStrideOri);
    int iTh(128), iCountW(iGrayStride), i(iWidth), j(iHeight), iPosY(1), iAvg(0);
    if (iDownSample && iWidth > 200 && iHeight > 50)
    {
        i = iWidth >> 2;
        iPosY = 4;
        j = iHeight >> 1;
        iCountW = iGrayStride << 1;
    }
    else
    {
        if (iWidth > 40)
        {
            i = iWidth >> 1;
            iPosY = 2;
        }
        if (iHeight > 8)
        {
            j = iHeight >> 1;
            iCountW = iGrayStride << 1;
        }
    }
    
    int rgiImhistCenter[256] = {0};
    CalcBinTh(pbGray, i, j, iCountW, iPosY, iTh, fAdjust, iAvg, rgiImhistCenter);

    iPosY = i * j;
    iTh = dk_max(iTh, iMinTh);//合理性？
    if (fAdjust && iAvg > 150)
    {
        iTh = dk_max(iTh, iAvg - 30);
    }
    iTh = dk_min(255, iTh);
    
    if (piThreshold)
    {
        *piThreshold = iTh;
    }
    iCountW = 0;
    for (i = 0; i <= iTh; i++)
    {
        iCountW += rgiImhistCenter[i];
    }
    if (iCountW > (iPosY >> 1))
    {
        iResult = 2;
    }
    
    //设置二值图
    int iStepW(1);
    BYTE *pbDst1(NULL), *pbDst2(NULL);
    BYTE *pbSrc1(NULL), *pbSrc2(NULL);
    if (iDownSample)
    {
        iWidth = (iWidthOri >> 2) + 2;
        iHeight = (iHeightOri >> 1) + 2;
        iStepW = 4;
        iGrayStride = iGrayStrideOri << 1;
        if (iResult == 2)
        {
            for (i = 2, pbSrc1 = pbGray, pbDst1 = pbBin + iWidth + 1; i < iHeight; i++, pbSrc1 += iGrayStride, pbDst1 += iWidth)
            {
                for (j = 2, pbSrc2 = pbSrc1, pbDst2 = pbDst1; j < iWidth; j++, pbSrc2 += iStepW, pbDst2++)
                {
                    if (*pbSrc2 > iTh || *(pbSrc2 + 1) > iTh || *(pbSrc2 + 2) > iTh || *(pbSrc2 + 3) > iTh
                        || *(pbSrc2 + iGrayStrideOri) > iTh || *(pbSrc2 + iGrayStrideOri + 1) > iTh
                        || *(pbSrc2 + iGrayStrideOri + 2) > iTh || *(pbSrc2 + iGrayStrideOri + 3) > iTh)
                    {
                        *pbDst2 = 255;
                    }
                    else
                    {
                        *pbDst2 = 0;
                    }
                }
            }
        }
        else
        {
            for (i = 2, pbSrc1 = pbGray, pbDst1 = pbBin + iWidth + 1; i < iHeight; i++, pbSrc1 += iGrayStride, pbDst1 += iWidth)
            {
                for (j = 2, pbSrc2 = pbSrc1, pbDst2 = pbDst1; j < iWidth; j++, pbSrc2 += iStepW, pbDst2++)
                {
                    if (*pbSrc2 <= iTh || *(pbSrc2 + 1) <= iTh || *(pbSrc2 + 2) <= iTh || *(pbSrc2 + 3) <= iTh
                        || *(pbSrc2 + iGrayStrideOri) <= iTh || *(pbSrc2 + iGrayStrideOri + 1) <= iTh
                        || *(pbSrc2 + iGrayStrideOri + 2) <= iTh || *(pbSrc2 + iGrayStrideOri + 3) <= iTh)
                    {
                        *pbDst2 = 255;
                    }
                    else
                    {
                        *pbDst2 = 0;
                    }
                }
            }
        }
    }
    else
    {
        pbDst1 = pbBin;
        if (iResult == 2)
        {
            for (i = 0, pbSrc1 = pbGray; i < iHeight; i++, pbSrc1 += iGrayStride)
            {
                for (j = 0, pbSrc2 = pbSrc1; j < iWidth; j++, pbSrc2++, pbDst1++)
                {
                    if (*pbSrc2 > iTh)
                    {
                        *pbDst1 = 255;
                    }
                    else
                    {
                        *pbDst1 = 0;
                    }
                }
            }
        }
        else
        {
            for (i = 0, pbSrc1 = pbGray; i < iHeight; i++, pbSrc1 += iGrayStride)
            {
                for (j = 0, pbSrc2 = pbSrc1; j < iWidth; j++, pbSrc2++, pbDst1++)
                {
                    if (*pbSrc2 <= iTh)
                    {
                        *pbDst1 = 255;
                    }
                    else
                    {
                        *pbDst1 = 0;
                    }
                }
            }
        }
    }
    
    //将二值图最外一行列置为黑点，便于分割
    iPosY = iWidth * (iHeight - 1);
    for (i = 0; i < iWidth; i++)
    {
        pbBin[i] = 0;
        pbBin[iPosY + i] = 0;
    }
    for (i = 0, iPosY = 0; i < iHeight; i++, iPosY += iWidth)
    {
        pbBin[iPosY] = 0;
        pbBin[iPosY + iWidth - 1] = 0;
    }
    return S_OK;
}

#if 1
HRESULT GetPageEdge(BYTE *pbData, int iWidth, int iHeight, int iStrideWidth, int iStepW, RECT *prcCutPos)
{
    if (pbData == NULL || prcCutPos == NULL)
    {
        return E_POINTER;
    }

    prcCutPos->top = 0;
    prcCutPos->bottom = iHeight - 1;
    prcCutPos->left = 0;
    prcCutPos->right = iWidth - 1;

    if (iWidth * iHeight > 5000000) //怕内存不够处理用
    {
        return S_FALSE;
    }

    BYTE *pbGray = (BYTE*)malloc(iWidth * iHeight * sizeof(BYTE));
    BYTE *pbBin = (BYTE*)malloc(iWidth * iHeight * sizeof(BYTE));
    if (NULL == pbGray || NULL == pbBin)
    {
        free(pbGray);
        free(pbBin);
        return E_OUTOFMEMORY;
    }

    BYTE *pbSrc = pbData;
    BYTE *pbDst = pbGray;
    BYTE *pbSrcTemp(NULL), *pbDstTemp(NULL);
    for (int i = 0; i < iHeight; i++, pbSrc += iStrideWidth * iStepW, pbDst += iWidth)
    {
        pbSrcTemp = pbSrc;
        pbDstTemp = pbDst;
        for (int j = 0; j < iWidth; j++, pbSrcTemp += iStepW, pbDstTemp++)
        {
            *pbDstTemp = *pbSrcTemp;
        }
    }

    int iBwR(0);
    Gray2Bin(pbBin, pbGray, iWidth, iHeight, iWidth, iBwR, 0, 0, 0);

    int iStop(0), iCount(0);
    pbSrc = pbBin;
    for (int i = 0; i < iHeight - 2; i++, pbSrc += iWidth)
    {
        iCount = 0;
        pbSrcTemp = pbSrc;
        for (int j = 0; j < iWidth - 2; j++, pbSrcTemp++)
        {
            if (*pbSrcTemp)
            {
                iCount++;
                if ((*(pbSrcTemp + 1) && *(pbSrcTemp + 2)) || (*(pbSrcTemp + iWidth) && *(pbSrcTemp + iWidth + iWidth)))
                {
                    iStop = 1;
                    break;
                }
            }
        }
        if (iStop || iCount > 5)
        {
            prcCutPos->top = i;
            break;
        }
    }
    
    iStop = 0;
    pbSrc = pbBin + iWidth * (iHeight - 1);
    for (int i = iHeight - 1; i > 1; i--, pbSrc -= iWidth)
    {
        iCount = 0;
        pbSrcTemp = pbSrc;
        for (int j = 0; j < iWidth - 2; j++, pbSrcTemp++)
        {
            if (*pbSrcTemp)
            {
                iCount++;
                if ((*(pbSrcTemp + 1) && *(pbSrcTemp + 2)) || (*(pbSrcTemp - iWidth) && *(pbSrcTemp - iWidth - iWidth)))
                {
                    iStop = 1;
                    break;
                }
            }
        }
        if (iStop || iCount > 5)
        {
            prcCutPos->bottom = i;
            break;
        }
    }

    iStop = 0;
    pbSrc = pbBin;
    for (int i = 0; i < iWidth - 2; i++, pbSrc++)
    {
        iCount = 0;
        pbSrcTemp = pbSrc;
        for (int j = 0; j < iHeight - 2; j++, pbSrcTemp += iWidth)
        {
            if (*pbSrcTemp)
            {
                iCount++;
                if ((*(pbSrcTemp + 1) && *(pbSrcTemp + 2)) || (*(pbSrcTemp + iWidth) && *(pbSrcTemp + iWidth + iWidth)))
                {
                    iStop = 1;
                    break;
                }
            }
        }
        if (iStop || iCount > 5)
        {
            prcCutPos->left = i;
            break;
        }
    }

    iStop = 0;
    pbSrc = pbBin + iWidth - 1;
    for (int i = iWidth - 1; i > 1; i--, pbSrc--)
    {
        iCount = 0;
        pbSrcTemp = pbSrc;
        for (int j = 0; j < iHeight - 2; j++, pbSrcTemp += iWidth)
        {
            if (*pbSrcTemp)
            {
                iCount++;
                if ((*(pbSrcTemp - 1) && *(pbSrcTemp - 2)) || (*(pbSrcTemp + iWidth) && *(pbSrcTemp + iWidth + iWidth)))
                {
                    iStop = 1;
                    break;
                }
            }
        }
        if (iStop || iCount > 5)
        {
            prcCutPos->right = i;
            break;
        }
    }

    if (prcCutPos->bottom - prcCutPos->top < (iHeight >> 4)    || prcCutPos->right - prcCutPos->left < (iWidth >> 3))
    {
        prcCutPos->top = 0;
        prcCutPos->bottom = iHeight - 1;
        prcCutPos->left = 0;
        prcCutPos->right = iWidth - 1;
    }

    free(pbGray);
    free(pbBin);
        
    return S_OK;
}
#else
HRESULT GetPageEdge(BYTE *pbData, int iWidth, int iHeight, int iStrideWidth, int iStepW, RECT *prcCutPos)
{
    DK_ASSERT(pbData);
    int iLeft = iWidth;
    int iTop = 0;
    int iBottom = 0;
    int iRight = 0;

    BYTE* pLine = pbData;
    bool isNullLine = true;

    //left
    pLine = pbData;
    for (int i = 0; i < iHeight; i++)
    {
        for (int j = 0; j < iWidth; j++)
        {
            if (pLine[j] != 0xff)//空白
            {
                if (j < iLeft)
                {
                    iLeft = j;
                    break;
                }
            }
        }
        
        if (iLeft < 1)
        {
            break;
        }
        
        pLine += iStrideWidth;
    }

    pLine = pbData;
    for (int i = 0; i < iHeight; i++)
    {
        for (int j = iWidth - 1; j >= 0; j--)
        {
            if (pLine[j] != 0xff)//空白
            {
                if (j > iRight)
                {
                    iRight = j;
                    break;
                }
            }
        }

        pLine += iStrideWidth;
    }

    // top
    pLine = pbData;
    for (int i = 0; i < iHeight; i++)
    {
        isNullLine = true;
        for (int j = 0; j < iWidth; j++)
        {
            if (pLine[j] != 0xff)//空白
            {
                isNullLine = false;
                break;
            }
        }

        if (isNullLine)
        {
            iTop++;
        }
        else
        {
            break;
        }
        pLine += iStrideWidth;
    }

    // bottom
    pLine = pbData;
    for (int i = 0; i < iHeight; i++)
    {
        isNullLine = true;
        for (int j = 0; j < iWidth; j++)
        {
            if (pLine[j] != 0xff)//空白
            {
                isNullLine = false;
                break;
            }
        }

        if (!isNullLine)
        {
            iBottom = i;
        }
        
        pLine += iStrideWidth;
    }

    if (iBottom < iWidth - 1)
    {
        iBottom++;
    }

    prcCutPos->top = iTop;
    prcCutPos->bottom = iBottom;
    prcCutPos->left = iLeft;
    prcCutPos->right = iRight;
    return true;
}
#endif


