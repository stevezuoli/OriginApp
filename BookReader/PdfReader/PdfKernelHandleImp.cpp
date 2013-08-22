//===========================================================================
// Summary:
//    PdfKernelHandleImp
// Version:
//    0.1 draft - creating.
// Usage:
//    Null
// Remarks:
//    Null
// Date:
//    2011-12-06
// Author:
//    Zhang JingDan (zhangjingdan@duokan.com)
//===========================================================================

#include "DkStreamFactory.h"
#include "KernelType.h"
#include "KernelMacro.h"
#include "interface.h"
#include "IDKPOutline.h"
#include "IDKPPageEx.h"
#include "DKDExportFunc.h"
#include "DKXManager/ReadingInfo/CT_ReadingDataItemImpl.h"
#include "BookReader/PdfKernelHandle.h"
#include "DKDParserKernelAPI.h"
#include "BookReader/SegmentImpl.h"
#include "BookReader/IDKPageAdapter.h"
#include "Utility/ThreadHelper.h"
#include "Utility/SystemUtil.h"

#ifndef _RELEASE_VERSION
//#define ENABLE_PROFILE
#endif
#include "Utility/Profiler.h"

using dk::utility::SystemUtil;
//#define DEBUG_OUTLINE_INFO // 调试目录信息
#if 0
static bool SaveImageToBmp(const DK_IMAGE* pImage)
{
    DK_ASSERT(pImage);
//     DK_ASSERT(pFilePathName);
//     DK_ASSERT(pFilePathName[0]);
    static int i = 0;
    char pFilePathName[256] = {0};
    snprintf(pFilePathName, DK_DIM(pFilePathName), "/mnt/hgfs/00_KindleTouch_Book/test_%d.bmp", i++);


    DK_SIZE_T width = (DK_SIZE_T)(pImage->iWidth);
    DK_SIZE_T height = (DK_SIZE_T)(pImage->iHeight);
    DK_SIZE_T strip = (DK_SIZE_T)(pImage->iStrideWidth);
    DK_SIZE_T saveWidth = width;
    DK_SIZE_T saveStrip = strip;
    DK_SIZE_T saveOffset = 0;
    if ((pImage->iColorChannels == 1) && (saveWidth%4))
    {
        saveOffset = (4 - saveWidth%4);
        saveWidth += saveOffset;
        saveStrip = saveWidth;
    }

    DK_BITMAPFILEHEADER fileHeader;
    DkZero(fileHeader);
    fileHeader.bfType = 0x4d42; // BM
    fileHeader.bfOffBits = sizeof(DK_BITMAPFILEHEADER) + sizeof(DK_BITMAPINFOHEADER);
    fileHeader.bfSize = saveStrip * height + fileHeader.bfOffBits;

    DK_BITMAPINFOHEADER infoHeader;
    DkZero(infoHeader);
    infoHeader.biSize = sizeof(infoHeader);
    infoHeader.biWidth = saveWidth;
    infoHeader.biHeight = -(DK_LONG)height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = pImage->iColorChannels * 8;
    infoHeader.biSizeImage = saveStrip * height;

    FILE* fp = fopen(pFilePathName, "wb+");
    if (NULL == fp)
    {
        return false;
    }

    if (infoHeader.biBitCount < 16)
    {
        fileHeader.bfOffBits += 1 << infoHeader.biBitCount;
        fileHeader.bfSize = saveStrip * height + fileHeader.bfOffBits;
    }

    fwrite(&fileHeader, sizeof(fileHeader), 1, fp);
    fwrite(&infoHeader, sizeof(infoHeader), 1, fp);

    //灰度图像，有颜色表，写入文件
    DK_INT colorTableSize = 0;
    if (infoHeader.biBitCount < 16)
    {
        colorTableSize = 1 << infoHeader.biBitCount;
        DK_RGBQUAD* pColorTable = new DK_RGBQUAD[colorTableSize];
        DK_ASSERT(pColorTable);
        for (DK_INT i = 0; i<colorTableSize; i++)   
        {   
            pColorTable[i].rgbBlue = i;
            pColorTable[i].rgbGreen = i;
            pColorTable[i].rgbRed = i;
            pColorTable[i].rgbReserved = 0;
        }

        fwrite(pColorTable, sizeof(DK_RGBQUAD), colorTableSize, fp);
        delete[] pColorTable;
    }

    const DK_BYTE* data = pImage->pbData;
    for (DK_SIZE_T i = 0; i < height; ++i)
    {
        DK_BYTE* p = (DK_BYTE*)&data[strip * i];
        fwrite(p, strip, 1, fp);
        if (saveOffset > 0)
        {
            fwrite("0", 1, saveOffset, fp);
        }
    }

    fclose(fp);
    return true;

}
#endif

static DKP_DRECT CrossRect(const DKP_DRECT& rect1, const DKP_DRECT& rect2)
{
	DKP_DRECT rectResult;
	rectResult.left = dk_max(rect1.left, rect2.left);
    rectResult.bottom = dk_max(rect1.bottom, rect2.bottom);
    rectResult.right = dk_min(rect1.right, rect2.right);
    rectResult.top = dk_min(rect1.top, rect2.top);
	if(rectResult.top <= rectResult.bottom || rectResult.right <= rectResult.left)
	{
		rectResult.left = 0;
	    rectResult.bottom = 0;
	    rectResult.right = 0;
	    rectResult.top = 0;
		
	}
	return rectResult;
}

#if 0
// 图片旋转 当iStrideWidth 大于iColorChannels * iWidth 时此函数会有问题.
static void RotateImage(DK_IMAGE* pImage, int angle)
{
    DK_ASSERT(pImage);
    DK_ASSERT(pImage->pbData);
    if (NULL == pImage || NULL == pImage->pbData || 0 == angle)
    {
        return;
    }

    if (pImage->iColorChannels == 1)
    {
        unsigned char* pDes = pImage->pbData;
        const int iLineLength = pImage->iStrideWidth;
        int iHeight = pImage->iHeight;
        unsigned char* pSrc = new unsigned char[iLineLength * iHeight];
        DK_ASSERT(pSrc);
        memcpy(pSrc, pDes, iLineLength * iHeight);

        if (180 == angle)
        {
            int iLoop = pImage->iWidth * iHeight;
            int iBufLen = iLoop;
            while(iLoop > 0)
            {
                pDes[iBufLen - iLoop] = pSrc[iLoop];
                iLoop--;
            }
        }
        else if (270 == angle)
        {
            int iSrcLineWidth = pImage->iWidth;
            dk_swap(pImage->iWidth, pImage->iHeight);
            pImage->iStrideWidth = pImage->iWidth * pImage->iColorChannels;
            int iDesLineWidth = pImage->iWidth;
            int h = pImage->iHeight;
            int w = pImage->iWidth;
            for (int i = 0; i < h; i++)
            {
                for(int j = 0; j < w; j++)
                {
                     pDes[i * iDesLineWidth + j] = pSrc[(w - j - 1) * iSrcLineWidth + i];  
                }
            }
        }
        else if (90 == angle)
        {
            int iSrcLineWidth = pImage->iWidth;
            dk_swap(pImage->iWidth, pImage->iHeight);
            pImage->iStrideWidth = pImage->iWidth * pImage->iColorChannels;
            int iDesLineWidth = pImage->iWidth;
            int h = pImage->iHeight;
            int w = pImage->iWidth;
            for (int i = 0; i < h; i++)
            {
                for(int j = 0; j < w; j++)
                {
                     pDes[i * iDesLineWidth + j] = pSrc[j * iSrcLineWidth + h - 1 - i];  
                }
            }
        }
        else
        {
            DK_ASSERT(false);
        }

        delete[] pSrc;
        pSrc = NULL;
    }
    else if (pImage->iColorChannels == 4)
    {
        int* pDes = (int*)pImage->pbData;
        const int iLineLength = pImage->iStrideWidth;
        int iHeight = pImage->iHeight;
        int* pSrc = (int*)new unsigned char[iLineLength * iHeight];
        DK_ASSERT(pSrc);
        memcpy(pSrc, pDes, iLineLength * iHeight);

        if (180 == angle)
        {
            int iLoop = pImage->iWidth * iHeight;
            int iBufLen = iLoop;
            while(iLoop > 0)
            {
                pDes[iBufLen - iLoop] = pSrc[iLoop];
                iLoop--;
            }
        }
        else if (90 == angle)
        {
            int iSrcLineWidth = pImage->iWidth;
            dk_swap(pImage->iWidth, pImage->iHeight);
            pImage->iStrideWidth = pImage->iWidth * pImage->iColorChannels;
            int iDesLineWidth = pImage->iWidth;
            int h = pImage->iHeight;
            int w = pImage->iWidth;
            for (int i = 0; i < h; i++)
            {
                for(int j = 0; j < w; j++)
                {
                     pDes[i * iDesLineWidth + j] = pSrc[(w - j - 1) * iSrcLineWidth + i];  
                }
            }
        }
        else if (270 == angle)
        {
            int iSrcLineWidth = pImage->iWidth;
            dk_swap(pImage->iWidth, pImage->iHeight);
            pImage->iStrideWidth = pImage->iWidth * pImage->iColorChannels;
            int iDesLineWidth = pImage->iWidth;
            int h = pImage->iHeight;
            int w = pImage->iWidth;
            for (int i = 0; i < h; i++)
            {
                for(int j = 0; j < w; j++)
                {
                     pDes[i * iDesLineWidth + j] = pSrc[j * iSrcLineWidth + h - 1 - i];  
                }
            }
        }
        else
        {
            DK_ASSERT(false);
        }

        delete[] pSrc;
        pSrc = NULL;
    }
    
}
#endif
static DK_IMAGE* CreateImage(int w, int h, int channels, int stride, int color = 0xff)
{
    DK_IMAGE* pRetImage = new DK_IMAGE;
    if (pRetImage)
    {
        pRetImage->iColorChannels = channels;
        pRetImage->iHeight = h;
        pRetImage->iWidth = w;
        pRetImage->iStrideWidth = stride;
        pRetImage->pbData = DK_MALLOC_OBJ_N(BYTE8, pRetImage->iHeight * pRetImage->iStrideWidth);
        if (pRetImage->pbData)
        {
            memset(pRetImage->pbData, color, pRetImage->iHeight * pRetImage->iStrideWidth);
            return pRetImage;
        }

        delete pRetImage;
    }
    
    return NULL;
}

static void DestroyImage(DK_IMAGE *& pImage)
{
    if (pImage)
    {
        if (pImage->pbData)
        {
            DK_FREE(pImage->pbData);
        }
        pImage->pbData = NULL;
        delete pImage;
        pImage = NULL;
    }
}

static bool ImageBitBlt(DK_IMAGE* pDes,int dx,int dy,int dw,int dh, const DK_IMAGE* pSrc, int sx,int sy)
{
    DK_ASSERT(pDes);
    DK_ASSERT(pSrc);
    DK_ASSERT(pDes->iColorChannels == pSrc->iColorChannels);
    DK_ASSERT(dw + dx <= pDes->iWidth);
    DK_ASSERT(dh + dy <= pDes->iHeight);
    DK_ASSERT(dw + sx <= pSrc->iWidth);
    DK_ASSERT(dh + sy <= pSrc->iHeight);

    unsigned char* pDesLine = pDes->pbData + dy * pDes->iStrideWidth + dx * pDes->iColorChannels;
    unsigned char* pSrcLine = pSrc->pbData + sy * pSrc->iStrideWidth + sx * pSrc->iColorChannels;
    int iCopyLength = dw * pSrc->iColorChannels;
    if (1 == pDes->iColorChannels || 4 == pDes->iColorChannels)
    {
        for(int i = 0; i < dh; i++)
        {
            memcpy(pDesLine, pSrcLine, iCopyLength);
            pDesLine += pDes->iStrideWidth;
            pSrcLine += pSrc->iStrideWidth;
        }
    }
    else
    {
        DK_ASSERT(false);
        return false;
    }

    return true;
}

static bool ImageFillBox(DK_IMAGE* pDes,int x,int y,int w,int h, int color)
{
    DK_ASSERT(pDes);
    DK_ASSERT(pDes->pbData);

    unsigned char* pDesLine = pDes->pbData + y * pDes->iStrideWidth + x * pDes->iColorChannels;
    if (1 == pDes->iColorChannels)
    {
        for(int i = 0; i < h; i++)
        {
            memset(pDesLine, color, w);
            pDesLine += pDes->iStrideWidth;
        }
    }
    else if (4 == pDes->iColorChannels)
    {
        for(int i = 0; i < h; i++)
        {
            int* pData = (int*)pDesLine;
            for (int j = 0; j < w; j++)
            {
                pData[j] = color;
            }
            
            pDesLine += pDes->iStrideWidth;
        }
    }
    else
    {
        DK_ASSERT(false);
        return false;
    }

    return true;
}

static double CalcRearrangeFontSizeScale(int fontSize)
{
    return 3 * (double)fontSize / 40 - ((fontSize > 23) ? 0.0 : 0.15);
}

static bool operator == (const DKP_REARRANGE_PARSER_OPTION& left, const DKP_REARRANGE_PARSER_OPTION& right)
{
    return left.lineGap == right.lineGap
           && left.paraSpacing == right.paraSpacing
           && left.scale == right.scale;
}

static int kiRepeatHeight = 20;
static int kiSplitLineHeight = 2;
static int kiSmartCutPageEdge = 5; // 智能切边的预留边界
static const unsigned int kuMaxSubPageNum = 1000;
static double kdMaxZoom = 3.0;

PdfKernelHandle::PdfKernelHandle()
    : m_uCurSubPageNum(0)
    , m_uCurSubPageCount(0)
    , m_uPageCount(0)
    , m_uCacheListSize(5)
    , m_uCurPageNum(0)
    , m_uCurPageEndNum(0)
    , m_uScreenWidth(0)
    , m_uScreenHeight(0)
    , m_bMainWait(false)
    , m_bMainWaitForReflowPage(false)
    , m_bMainWaitForReflowImage(false)
    , m_bUpdateCache(true)
    , m_bCancelThread(false)
    , m_uPageWidth(0)
    , m_uPageHeight(0)
    , m_uPriorityLoadPage(0)
    , m_dLastPageZoom(1.0)
    , m_uBookSetScreenWidth(0)
    , m_uBookSetScreenHeight(0)
    , m_pDoc(NULL)
    , m_pReflowPage(0)
    , m_textGamma(1.0)
    , m_graphicGamma(1.0)
    , m_eFontSmoothType(DK_FONT_SMOOTH_NORMAL)
    , m_eThreadStatus(E_THREAD_EXIT)
    , m_pOutputImage(NULL)
    , m_pBookTextController(NULL)
    , m_bIsInstant(FALSE)
    , m_parseSingle(false)
{
    m_stModeController.m_iRotation = 0;
    m_stModeController.m_eCuttingEdgeMode = PDF_CEM_Normal;
    m_stModeController.m_eReadingMode = PDF_RM_NormalPage;
    m_stModeController.m_eTurnPageMode = PDF_TPM_SinglePage;

    m_rearrangeOption.dpi = RenderConst::SCREEN_DPI;
    m_rearrangeOption.contentFilter = (DKP_REARRANGE_CONTENT)(DKP_REARRANGE_CONTENT_TEXT | DKP_REARRANGE_CONTENT_IMAGE);
    memset(&m_subPageEdge, 0, sizeof (m_subPageEdge));
    m_startPos.nChapterIndex = 1;

#ifdef KINDLE_FOR_TOUCH
    m_pBookTextController = new SelectedControler();
#else
    m_pBookTextController = new CursorControler();
#endif
    
    pthread_mutex_init(&m_listLock,0);
    pthread_cond_init(&m_listSignal,0);
    pthread_cond_init(&m_threadSignal,0);
    pthread_mutex_init(&m_pdfLibLock, 0);
}

PdfKernelHandle::~PdfKernelHandle()
{
    CloseCache();

    pthread_mutex_destroy(&m_listLock);
    pthread_cond_destroy(&m_listSignal);
    pthread_cond_destroy(&m_threadSignal);
    pthread_mutex_destroy(&m_pdfLibLock);

    for(size_t i = 0; i < m_vTOCList.size(); i++)
	{
		SafeDeletePointer (m_vTOCList[i]);
	}
	m_vTOCList.clear();

    if (m_pBookTextController)
    {
        m_pBookTextController->Release();
		SafeDeletePointer(m_pBookTextController);
    }

    if (m_pDoc)
    {
        m_pDoc->CloseDoc();
        DKP_DestroyDoc(m_pDoc);
    }
    DestroyImage(m_pOutputImage);
}

bool PdfKernelHandle::Initialize(const wchar_t* pFilePathName)
{
    DK_ASSERT(pFilePathName);
    if (NULL == pFilePathName)
    {
        return false;
    }

    if (m_pDoc) //防止Initialize调用多次.
    {
        CloseCache();
        DKP_DestroyDoc(m_pDoc);
        m_pDoc = NULL;
    }

    DKP_CreateDoc(m_pDoc);
    IDkStream* pStream = DkStreamFactory::GetFileStream(pFilePathName);
    if (pStream && m_pDoc && DK_SUCCEEDED(m_pDoc->OpenDoc(pStream)))
    {
        m_uPageCount = m_pDoc->GetPageCount();
        SafeDeletePointer(pStream);
        return true;
    }

    SafeDeletePointer(pStream);
    return false;
}

void PdfKernelHandle::SetTypeSetting(const DKTYPESETTING& typeSetting)
{
	if(m_pDoc)
	{
    	m_pDoc->SetTypeSetting(typeSetting);
	}
}

DK_IMAGE* PdfKernelHandle::GetPage(bool needPreRendering)
{
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::GetPage() Start");
    if (m_pOutputImage)
    {
        DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::GetPage() return m_pOutputImage directly");
        return m_pOutputImage;
    }

    if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode)
    {
        DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::GetPage() return GetReflowPageImage");
        GetReflowPageImage(m_pReflowPage, needPreRendering);
        return m_pReflowPage ? m_pReflowPage->pImage : NULL;
    }
    
    unsigned int cacheIndex(0);
    DK_IMAGE* pCurPage = GetPage(m_uCurPageNum, &cacheIndex, needPreRendering);
    if(NULL == pCurPage)
    {
        return NULL;
    }
    DebugPrintf (DLC_LIUHJ, "m_eTurnPageMode    = %d", m_stModeController.m_eTurnPageMode);
    DebugPrintf (DLC_LIUHJ, "m_eCuttingEdgeMode = %d", m_stModeController.m_eCuttingEdgeMode);
    DebugPrintf (DLC_LIUHJ, "mcacheIndex        = %d", cacheIndex);
    const int& iPageWidth = pCurPage->iWidth;
    const int& iPageHeight = pCurPage->iHeight;
    const int& iColorChannels = pCurPage->iColorChannels;
    
    DK_IMAGE* pRetImage = NULL;
    switch (m_stModeController.m_eReadingMode)
    {
    case PDF_RM_Split4Page:
        {
            cacheIndex = 0;
            int x = (int)(iPageWidth * m_stModeController.m_dWidth);
            int y = (int)(iPageHeight * m_stModeController.m_dHeight);
            int w = 0;
            int h = 0;
            static const int SplitOrderTable[4][4] = 
            {
                {0, 1, 2, 3},
                {1, 0, 3, 2},
                {0, 2, 1, 3},
                {1, 3, 0, 2}
            };

            switch (SplitOrderTable[m_stModeController.m_uSubPageOrder%4][m_stModeController.m_uCurSubPage%4])
            {
            case 1:
                w = iPageWidth - x;
                h = y;
                x = x;
                y = 0;
                break;
            case 2:
                w = x;
                h = iPageHeight - y;
                x = 0;
                y = y;
                break;
            case 3:
                w = iPageWidth - x;
                h = iPageHeight - y;
                x = x;
                y = y;
                break;
            default:
            case 0:
                w = x;
                h = y;
                x = 0;
                y = 0;
                break;
            }

            m_pOutputImage = CreateImage(w, h, iColorChannels, iColorChannels * w, 0);
            ImageBitBlt(m_pOutputImage, 0, 0, w, h, pCurPage, x, y);
            pRetImage = m_pOutputImage;
        }
        break;
    case PDF_RM_Split2Page:
        {
            cacheIndex = 0;
            int x = (int)(iPageWidth * m_stModeController.m_dWidth);
            int y = 0;
            int w = 0;
            int h = 0;
            static const int SplitOrderTable[2][2] = 
            {
                {0, 1},
                {1, 0}
            };

            switch (SplitOrderTable[m_stModeController.m_uSubPageOrder%2][m_stModeController.m_uCurSubPage%2])
            {
            case 1:
                w = iPageWidth - x;
                h = iPageHeight;
                x = x;
                y = 0;
                break;
            default:
            case 0:
                w = x;
                h = iPageHeight;
                x = 0;
                y = 0;
                break;
            }

            m_pOutputImage = CreateImage(w, h, iColorChannels, iColorChannels * w, 0);
            ImageBitBlt(m_pOutputImage, 0, 0, w, h, pCurPage, x, y);
            pRetImage = m_pOutputImage;
        }
        break;
    case PDF_RM_ZoomPage: // 分栏,漫画,放大时不支持滚轴。
        {
            //const DK_DOUBLE& userZoom = m_stModeController.m_dUserZoom;
            int x = 0, y = 0, w = iPageWidth, h = iPageHeight;
            m_pOutputImage = CreateImage(w, h, iColorChannels, iColorChannels * w, 0);
            ImageBitBlt(m_pOutputImage, 0, 0, w, h, pCurPage, x, y);
            pRetImage = m_pOutputImage;
        }
        break;
    default :
        {
            if (PDF_TPM_SinglePage == m_stModeController.m_eTurnPageMode)
            {
                const int& iStrideWidth = pCurPage->iStrideWidth;
                m_pOutputImage = CreateImage(iPageWidth, iPageHeight, iColorChannels, iStrideWidth, 0);
                if(NULL == m_pOutputImage)
                {
                    return NULL;
                }
                memcpy(m_pOutputImage->pbData, pCurPage->pbData, iPageHeight * iStrideWidth);
                pRetImage = m_pOutputImage;
                DebugPrintf (DLC_LIUHJ, "PDF Get Single Page Mode. CuttingEdgeMode %d, cacheIndex %d", m_stModeController.m_eCuttingEdgeMode, cacheIndex);
#if 0
                DebugPrintf (DLC_LIUHJ, "CuttingEdge L %f, R %f, T %f, B %f, Scale %f", m_cacheList[cacheIndex]->mode.m_dLeftEdge,
                    m_cacheList[cacheIndex]->mode.m_dRightEdge, m_cacheList[cacheIndex]->mode.m_dTopEdge, m_cacheList[cacheIndex]->mode.m_dBottomEdge,
                    m_cacheList[cacheIndex]->scale);
                DebugPrintf (DLC_LIUHJ, "Render PageBox width %f, height %f", m_cacheList[cacheIndex]->pageBox.Width(), m_cacheList[cacheIndex]->pageBox.Height());
#endif
            }
            else
            {
                ScrollModeGetPage();
                pRetImage = m_pOutputImage;
            }
        }
        break;
    }
    
    // 建立原版式页的光标表，如果是分栏或者横屏，则需要涉及到单个页面文本元素分拆和多个页面文本元素的拼接。
    // REMARK: 由于应用层的横屏并非界面横屏，而只是将图片做了旋转，所以这里不支持横屏的光标表
    MakeCursorTable(cacheIndex);
    if (!IsZoomedPdf())
    {
        if(cacheIndex >= 0 && cacheIndex < m_cacheList.size())
        {
            CacheData* pCacheData = m_cacheList[cacheIndex];
            if (pCacheData)
            {
                PdfModeController modeController = m_stModeController;
                m_stModeController = pCacheData->mode;
                switch(modeController.m_eReadingMode)
                {
                case PDF_RM_Split4Page:
                case PDF_RM_Split2Page:
                    m_stModeController.m_uCurSubPage = modeController.m_uCurSubPage;
                    m_stModeController.m_uSubPageOrder = modeController.m_uSubPageOrder;
                    break;
                case PDF_RM_AdaptiveWidth:
                    m_stModeController.m_uCurPageStartPos = modeController.m_uCurPageStartPos;
                    break;
                default:
                    break;
                }
            }
        }
    }

    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::GetPage() Finish");
    return pRetImage;
}

bool PdfKernelHandle::SetFontSmoothType(DK_FONT_SMOOTH_TYPE fontSmoothType)
{
    if (m_eFontSmoothType != fontSmoothType)
    {
        m_eFontSmoothType = fontSmoothType;
        IBMPProcessor* pProcessor = BMPProcessFactory::CreateInstance(IBMPProcessor::DK_BMPPROCESSOR_SHARPEN);
        if (NULL != pProcessor)
        {
            if ((m_eFontSmoothType == DK_FONT_SMOOTH_SHARP) && (PDF_RM_Rearrange != m_stModeController.m_eReadingMode))
            {
                AddBMPProcessor(pProcessor);
            }
            else
            {
                RemoveBMPProcessor(pProcessor);
            }
        }
    }
    return true;
}

DK_IMAGE* PdfKernelHandle::GetPage(unsigned int pageNum,
                                   unsigned int* pCacheIndex/* = NULL*/,
                                   bool needPreRendering)
{
    // TODO:处理中间页为NULL 的情况
    DK_IMAGE* pRetImage = NULL;
    
    pthread_mutex_lock(&m_listLock);
    unsigned int i(0);
    for(; i < this->m_cacheList.size(); i++)
    {
        if(pageNum == m_cacheList.at(i)->uPageNum)
        {
            if (m_uCurSubPageNum == kuMaxSubPageNum)
            {
                m_uCurSubPageNum = m_cacheList.at(i)->uSubPageCount - 1;
            }

            if(m_uCurSubPageNum == m_cacheList.at(i)->uSubPageNum)
            {
                pRetImage = m_cacheList.at(i)->pImage;
                break;
            }
        }
    }

    if(NULL == pRetImage)
    {
        WakeUpThread();
        m_bMainWait = true;
        m_uPriorityLoadPage = pageNum;
        pthread_cond_wait(&m_listSignal, &m_listLock);
        for(i = 0; i < this->m_cacheList.size(); i++)
        {
            if(pageNum == m_cacheList.at(i)->uPageNum)
            {
                if (m_uCurSubPageNum == kuMaxSubPageNum)
                {
                    m_uCurSubPageNum = m_cacheList.at(i)->uSubPageCount - 1;
                }

                if(m_uCurSubPageNum == m_cacheList.at(i)->uSubPageNum)
                {
                    pRetImage = m_cacheList.at(i)->pImage;
                    break;
                }
            }
        }
    }

    if (pCacheIndex)
    {
        *pCacheIndex = i;
    }

    pthread_mutex_unlock(&m_listLock);

    if (needPreRendering)
        WakeUpThread();
    return pRetImage;
}

PdfKernelHandle::ReflowCacheData* PdfKernelHandle::GetReflowPage(const DK_FLOWPOSITION& pos,
                                                                 IDKPDoc::REARRANGE_PAGE_POSITION_TYPE posType)
{
    // If the target has not been rendered, render it in prerender thread
    pthread_mutex_lock(&m_listLock);

    // TODO:处理中间页为NULL 的情况
    // NOTE: Reference position must be protected
    m_posForReflowReference = pos;
    m_posTypeForReflowReference = posType;
    m_bMainWaitForReflowPage = true;
    m_bUpdateCache = true;
    WakeUpThread();

    // wait for parsing finishing
    pthread_cond_wait(&m_listSignal, &m_listLock);
    pthread_mutex_unlock(&m_listLock);

    // NOTE: reference position may have been updated
    PdfKernelHandle::ReflowCacheData* pCache = FindCachedReflowPage(m_posForReflowReference, m_rearrangeOption, m_posTypeForReflowReference);
    return pCache;
}

DK_IMAGE* PdfKernelHandle::GetReflowPageImage(ReflowCacheData *pReflowPage, bool needPreRendering)
{
    // TODO. the image should be protected
    // If target page has been rendered, return it directly
    if (pReflowPage != 0 && pReflowPage->pImage != 0)
    {
        DebugPrintf(DLC_LIUHJ, "pReflowPage->pImage has already been ready, Return it directly");
    }
    else
    {
        // If the target has not been rendered, render it in prerender thread
        pthread_mutex_lock(&m_listLock);
        m_bMainWaitForReflowImage = true;
        m_bUpdateCache = true;
        WakeUpThread();

        // wait for parsing finishing
        pthread_cond_wait(&m_listSignal, &m_listLock);
        pthread_mutex_unlock(&m_listLock);
    }

    // wake up prerendering thread in page flipping
    if (needPreRendering)
    {
        DebugPrintf (DLC_LIUHJ, "Wakeup Prerendering Thread");
        WakeUpThread();
    }
    return pReflowPage != 0 ? pReflowPage->pImage : 0;
}

bool PdfKernelHandle::SetPage(DK_FLOWPOSITION pos)
{
    if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode)
    {
        // replaced by asynchronized rendering
        ReflowCacheData* updatedPage  = GetReflowPage(DK_FLOWPOSITION(pos.nChapterIndex + 1, pos.nParaIndex, pos.nElemIndex),
                                                      IDKPDoc::LOCATION_PAGE);
        if (updatedPage != 0)
        {
            m_pReflowPage = updatedPage;
            UpdateReflowScreen();
            return true;
        }
    }
    else
    {
        unsigned int pageNum = pos.nChapterIndex;

        // start page is 0
        if (pageNum < m_uPageCount)
        {
            m_uCurPageNum = m_uCurPageEndNum = pageNum;
            m_uPriorityLoadPage = m_uCurPageNum;
            m_uCurSubPageCount = 1;
            m_uCurSubPageNum = 0;
            m_stModeController.m_uCurSubPage = 0;
            m_bUpdateCache = true;
            DestroyImage(m_pOutputImage);
            return true;
        }
    }

    return false;
}

unsigned int PdfKernelHandle::GetCurPageNum()
{
    return m_uCurPageNum;
}

bool PdfKernelHandle::SetScreenRes(unsigned int width, unsigned int height)
{
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::SetScreenRes width %d, height %d", width, height);
    DK_ASSERT(width > 0);
    DK_ASSERT(height > 0);

    if(width == m_uBookSetScreenWidth && height == m_uBookSetScreenHeight)
    {
        return true;
    }

    CloseCache();
    m_uBookSetScreenWidth = width;
    m_uBookSetScreenHeight = height;
    InitPageSize();
    return true;
}

void PdfKernelHandle::GetScreenRes(unsigned int* pWidth, unsigned int* pHeight)
{
    if (pWidth)
    {
        *pWidth = m_uBookSetScreenWidth;
    }
    if (pHeight)
    {
        *pHeight = m_uBookSetScreenHeight;
    }
}

unsigned int PdfKernelHandle::GetPageCount()
{
    return m_uPageCount;
}

bool PdfKernelHandle::GetPageText(unsigned int pageNum, char** ppTextBuf, unsigned int* pTextLen /*＝ NULL*/)
{
    if (NULL == ppTextBuf)
    {
        return false;
    }

    *ppTextBuf = NULL;
    if (pTextLen)
    {
        *pTextLen = 0;
    }

    if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode && m_pReflowPage)
    {
        wchar_t* pPageContent = m_pReflowPage->pReflowPage->GetTextContent();
        if(!pPageContent)
        {
            return false;
        }
        *ppTextBuf = EncodingUtil::WCharToChar (pPageContent, EncodingUtil::UTF8);
        if (*ppTextBuf)
        {
            if (pTextLen)
            {
                *pTextLen = strlen (*ppTextBuf);
            }
            DebugPrintf(DLC_LIUHJ, "PdfKernelHandle::Get Rearrange Page Text\n%s\n", *ppTextBuf);
        }

        m_pReflowPage->pReflowPage->FreeTextContent(pPageContent);
        pPageContent = NULL;
        return true;
    }
    else
    {
        IDKPPage *pPage = m_pDoc->GetPage(pageNum + 1);
        if(pPage)
        {
            wchar_t* pPageContent (NULL);
            int textLen (0);
            pPage->GetPageTextContent (pPageContent, textLen, true, DKP_TEXTCONTENT_RAW);
            if (pPageContent && 0 < textLen)
            {
                *ppTextBuf = EncodingUtil::WCharToChar (pPageContent, EncodingUtil::UTF8);
                if (*ppTextBuf)
                {
                    if (pTextLen)
                    {
                        *pTextLen = strlen (*ppTextBuf);
                    }
                    DebugPrintf(DLC_LIUHJ, "PdfKernelHandle::GetPageText\n%s\n", *ppTextBuf);
                    return true;
                }
            }
        }
    }

    return false;
}

void PdfKernelHandle::CloseCache()
{
    StopThread();
    CleanCache();
}

//---------------------------private---------------------------

void PdfKernelHandle::StopThread()
{
    if(E_THREAD_EXIT != m_eThreadStatus)
    {
        m_bCancelThread = true;
        int waiting_count = 0; // wait 10s
        while(E_THREAD_EXIT != m_eThreadStatus && waiting_count < 100000)
        {
            if(E_THREAD_SLEEP == m_eThreadStatus)
            {
                pthread_cond_signal(&m_threadSignal);
            }
            usleep(100);
            waiting_count++;
        }
    }
}

void PdfKernelHandle::CleanCache()
{
    if (m_pBookTextController)
    {
        m_pBookTextController->Release();
    }
    for(unsigned int i = 0; i < m_cacheList.size(); i++)
    {
        DeleteCacheData(m_cacheList.at(i));
    }

    for(unsigned int i = 0; i < m_cacheReflowData.size(); i++)
    {
        ClearReflowCacheData(m_cacheReflowData.at(i));
    }

    m_cacheList.clear();
    m_cacheReflowData.clear();

    DestroyImage(m_pOutputImage);
}

void PdfKernelHandle::DeleteCacheData(PdfKernelHandle::CacheData *pData)
{
    if(pData)
    {
        if(pData->pImage)
        {
            DestroyImage(pData->pImage);
        }
        pData->textElementsInPage.clear();
        delete pData;
        pData = NULL;
    }
}

void PdfKernelHandle::ClearReflowCacheData(ReflowCacheData *pData)
{
    if(pData)
    {
        if(pData->pImage)
        {
            DebugPrintf (DLC_LIUHJ, "Destroy Image");
            DestroyImage(pData->pImage);
        }

        if(pData->pReflowPage)
        {
            DK_FLOWPOSITION position;
            pData->pReflowPage->GetPageStartPos(&position);
            DebugPrintf (DLC_LIUHJ, "Destroy page (%d, %d, %d)", position.nChapterIndex, position.nParaIndex, position.nElemIndex);
            m_pDoc->ReleasePageEx(pData->pReflowPage, true);
        }
        if (m_pReflowPage == pData)
        {
            m_pReflowPage = NULL;
        }

        delete pData;
        pData = NULL;
    }
}

bool PdfKernelHandle::WakeUpThread()
{
    if(E_THREAD_EXIT != m_eThreadStatus)
    {
        pthread_cond_signal(&m_threadSignal);
        return true;
    }
    else
    {
        pthread_attr_t attr;
        pthread_attr_init (&attr);
        pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);
        pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
        m_bCancelThread = false;
        pthread_t tid = 0;
        int iRet = pthread_create(&tid, &attr, CacheThread, this);
        pthread_attr_destroy(&attr);
        while(0 == iRet)
        {
            if(E_THREAD_EXIT != m_eThreadStatus)
            {
                return true;
            }

            usleep(100);
        }
    }

    return false;
}

void *PdfKernelHandle::CacheThread(void *pHandler)
{
    if(NULL == pHandler)
    {
        return NULL;
    }

    pthread_mutex_t tmpMutex;
    pthread_mutex_init(&tmpMutex,0);
    PdfKernelHandle *pPdfHandle = (PdfKernelHandle *)pHandler;

    while(1)
    {
        if(pPdfHandle->m_bCancelThread)
        {
            break;
        }
        pPdfHandle->m_eThreadStatus = E_THREAD_ACTIVE;
        if (PDF_RM_Rearrange == pPdfHandle->m_stModeController.m_eReadingMode)
        {
            if (pPdfHandle->m_bMainWaitForReflowPage)
            {
                pPdfHandle->LoadReflowPages();
            }
            else
            {
                pPdfHandle->LoadReflowImages();
            }
        }
        else
        {
            pPdfHandle->LoadCache();
        }

        pPdfHandle->m_eThreadStatus = E_THREAD_SLEEP;
        if (PDF_RM_Rearrange == pPdfHandle->m_stModeController.m_eReadingMode)
        {
            if(!pPdfHandle->m_bMainWaitForReflowPage &&
               !pPdfHandle->m_bMainWaitForReflowImage &&
               !pPdfHandle->m_bUpdateCache)
            {
                DebugPrintf (DLC_LIUHJ, "Prerender thread sleeps");
                pthread_mutex_lock(&tmpMutex);
                pthread_cond_wait(&pPdfHandle->m_threadSignal, &tmpMutex);
                pthread_mutex_unlock(&tmpMutex);
            }
        }
        else
        {
            if(!pPdfHandle->m_bMainWait && !pPdfHandle->m_bUpdateCache)
            {
                pthread_mutex_lock(&tmpMutex);
                pthread_cond_wait(&pPdfHandle->m_threadSignal, &tmpMutex);
                pthread_mutex_unlock(&tmpMutex);
            }
            
        }
        pPdfHandle->m_bUpdateCache = false;
    }
    pthread_mutex_destroy(&tmpMutex);
    pPdfHandle->m_eThreadStatus = E_THREAD_EXIT;
    ThreadHelper::ThreadExit(NULL);
    return NULL;
}

void PdfKernelHandle::LoadCache()
{
    int iMaxPage = m_uCurPageNum + m_uCacheListSize / 2;
    int iMinPage = iMaxPage + 1 - m_uCacheListSize;
    iMaxPage = iMaxPage < (int)m_uPageCount? iMaxPage: m_uPageCount - 1;
    iMinPage = iMinPage < 1? 0: iMinPage;

    LoadCachePage(m_uPriorityLoadPage);

    if (m_parseSingle || IsZoomedPdf())
    {
        return;
    }

    for(int i = m_uCurPageNum; i < iMaxPage + 1; i++)
    {
        if(m_bUpdateCache || m_bCancelThread)
        {
            break;
        }

        LoadCachePage(i);
    }

    for(int i = m_uCurPageNum; i >= iMinPage + 1; i--)
    {
        if(m_bUpdateCache || m_bCancelThread)
        {
            break;
        }

        LoadCachePage(i);
    }
}

void PdfKernelHandle::LoadCachePage(unsigned int pageNum)
{
    //检查当前页是否存在
    if(CheckPageIsExistence(pageNum))
    {
        return;
    }

    CacheData* pData = new CacheData;
    DK_ASSERT(pData);
    if (NULL == pData)
    {
        return;
    }

    pData->uPageNum = pageNum;
    pData->uSubPageNum = 0;
    pData->uSubPageCount = 1;
    pData->mode = m_stModeController;
    pData->pImage = PdfDrawPage(pData->uPageNum, &pData->scale, &pData->pageBox, &pData->mode, &pData->textElementsInPage);
    // 在些进行pdf的bmpprocessor处理
    if(pData->pImage)
    {
        vector<IBMPProcessor*>::iterator vi;
        for (vi = m_lstProcessor.begin(); vi != m_lstProcessor.end(); ++vi)
        {
            if (NULL == *vi || !(*vi)->Process(pData->pImage))
            {
                break;
            }
        }
    }

    //把加载的页放入缓存链中
    pthread_mutex_lock(&m_listLock);
    m_cacheList.push_back(pData);
    //如果当前主线程被阻塞&&当前页是主线程正在等待的页面则发送信号给主线程.
    if (m_bMainWait && pageNum == m_uPriorityLoadPage)
    {
        pthread_cond_signal(&m_listSignal);
        m_bMainWait = false;
    }

    pthread_mutex_unlock(&m_listLock);
    DeleteBeyondPage();    //去掉多余的页面
}

void PdfKernelHandle::LoadReflowPages()
{
    // Load current reflow page, m_posForReflowReference might be changed after parsing
    PdfKernelHandle::ReflowCacheData* pCurrentPageCache = LoadReflowCachePage(m_posForReflowReference,
                                                                              m_posTypeForReflowReference);
    if (pCurrentPageCache == 0)
    {
        DebugPrintf (DLC_LIUHJ, "Loading Reflow Cache Page Failed");
        return;
    }

    // Remove redundant pages
    pthread_mutex_lock(&m_listLock);
    RemoveRedundantReflowPages(pCurrentPageCache);
    pthread_mutex_unlock(&m_listLock);

    // Prerender next screen
    //printf("\n m_bCancelThread(%s), m_bUpdateReflowPage(%s) \n", m_bCancelThread ? "true":"false",
    //       m_bUpdateReflowPage ? "true":"false");

    //if (!m_bCancelThread && !m_bUpdateReflowPage &&
    //    (m_posTypeForReflowReference == IDKPDoc::LOCATION_PAGE || m_posTypeForReflowReference == IDKPDoc::NEXT_PAGE))
    //{
    //    DK_FLOWPOSITION nextScreenPos;
    //    if (pCurrentPageCache->pReflowPage->GetPageEndPos(&nextScreenPos))
    //    {
    //        printf("\n Prerender Next Page ----------- \n");
    //        LoadReflowCachePage(nextScreenPos, IDKPDoc::NEXT_PAGE);
    //    }
    //}

    //// Prerender previous screen
    //if (!m_bCancelThread && !m_bUpdateReflowPage &&
    //    (m_posTypeForReflowReference == IDKPDoc::LOCATION_PAGE || m_posTypeForReflowReference == IDKPDoc::PREV_PAGE))
    //{
    //    DK_FLOWPOSITION prevScreenPos;
    //    if (pCurrentPageCache->pReflowPage->GetPageStartPos(&prevScreenPos))
    //    {
    //        printf("\n Prerender Previous Page ----------- \n");
    //        LoadReflowCachePage(prevScreenPos, IDKPDoc::PREV_PAGE);
    //    }
    //}
}

void PdfKernelHandle::LoadReflowImages()
{
    if (m_pReflowPage != 0 && m_pReflowPage->pReflowPage != 0)
    {
        DebugPrintf (DLC_LIUHJ, "\n Rendering Image of Current Page");
        LoadReflowCacheImage(m_pReflowPage);
    }
    else
    {
        DebugPrintf (DLC_LIUHJ, "Loading Reflow Cache Page Failed");
        m_pReflowPage = LoadReflowCachePage(m_posForReflowReference, IDKPDoc::LOCATION_PAGE);
        if (!m_bCancelThread && m_pReflowPage != 0)
        {
            DebugPrintf (DLC_LIUHJ, "Prerender current Image");
            LoadReflowCacheImage(m_pReflowPage);
        }
        return;
    }

    // Prerender next screen
    DebugPrintf (DLC_LIUHJ, "m_bCancelThread(%s), m_bUpdateCache(%s)", m_bCancelThread ? "true":"false",
           m_bUpdateCache ? "true":"false");

    if (//m_posTypeForReflowReference == IDKPDoc::LOCATION_PAGE ||
        m_posTypeForReflowReference == IDKPDoc::NEXT_PAGE)
    {
        DK_FLOWPOSITION nextScreenPos;
        if (m_pReflowPage->pReflowPage->GetPageEndPos(&nextScreenPos))
        {
            PdfKernelHandle::ReflowCacheData* pNextPageCache = LoadReflowCachePage(nextScreenPos, IDKPDoc::NEXT_PAGE);
            if (!m_bCancelThread && !m_bUpdateCache && pNextPageCache != 0)
            {
                DebugPrintf (DLC_LIUHJ, "Prerender Next Image");
                LoadReflowCacheImage(pNextPageCache);
            }
        }
    }

    // Prerender previous screen
    if (//m_posTypeForReflowReference == IDKPDoc::LOCATION_PAGE ||
        m_posTypeForReflowReference == IDKPDoc::PREV_PAGE)
    {
        DK_FLOWPOSITION prevScreenPos;
        if (m_pReflowPage->pReflowPage->GetPageStartPos(&prevScreenPos))
        {
            PdfKernelHandle::ReflowCacheData* pPrevPageCache = LoadReflowCachePage(prevScreenPos, IDKPDoc::PREV_PAGE);
            if (!m_bCancelThread && !m_bUpdateCache && pPrevPageCache != 0)
            {
                DebugPrintf (DLC_LIUHJ, "Prerender Previous Image");
                LoadReflowCacheImage(pPrevPageCache);
            }
        }
    }
}

bool PdfKernelHandle::IsWantedReflowPage(PdfKernelHandle::ReflowCacheData* page,
                                         const DK_FLOWPOSITION& currentPos,
                                         const DKP_REARRANGE_PARSER_OPTION& reflowParserOption,
                                         IDKPDoc::REARRANGE_PAGE_POSITION_TYPE posType)
{
    DK_FLOWPOSITION cachedPagePos;
    if (posType == IDKPDoc::LOCATION_PAGE)
    {
        if(page->pReflowPage->GetPageStartPos(&cachedPagePos))
        {
            if (cachedPagePos == currentPos && page->parserOption == reflowParserOption)
            {
                return true;
            }
        }
    }
    else
    {
        if (((posType == IDKPDoc::NEXT_PAGE &&
            page->pReflowPage->GetPageStartPos(&cachedPagePos) &&
            cachedPagePos == currentPos) ||
            (posType == IDKPDoc::PREV_PAGE &&
            page->pReflowPage->GetPageEndPos(&cachedPagePos) &&
            cachedPagePos == currentPos)) &&
            page->parserOption == reflowParserOption)
        {
            return true;
        }
    }
    return false;
}

bool PdfKernelHandle::ReleaseReflowPage(IDKPPageEx* page)
{
    bool succeed = false;
    pthread_mutex_lock(&m_listLock);
    for(unsigned int i = 0; i < m_cacheReflowData.size(); i++)
    {
        if (m_cacheReflowData.at(i)->pReflowPage == page)
        {
            ClearReflowCacheData(m_cacheReflowData[i]);
            m_cacheReflowData.erase(m_cacheReflowData.begin() + i);
            succeed = true;
            break;
        }
    }
    pthread_mutex_unlock(&m_listLock);
    return succeed;
}

// TODO: Check parser option
PdfKernelHandle::ReflowCacheData* PdfKernelHandle::FindCachedReflowPage(const DK_FLOWPOSITION& currentPos,
                                                                        const DKP_REARRANGE_PARSER_OPTION& reflowParserOption,
                                                                        IDKPDoc::REARRANGE_PAGE_POSITION_TYPE posType)
{
    pthread_mutex_lock(&m_listLock);
    PdfKernelHandle::ReflowCacheData* pData = 0;
    // find reference page for pre-load
    for(unsigned int i = 0; i < m_cacheReflowData.size(); i++)
    {
        if (IsWantedReflowPage(m_cacheReflowData.at(i), currentPos, reflowParserOption, posType))
        {
            pData = m_cacheReflowData.at(i);
        }
    }
    pthread_mutex_unlock(&m_listLock);
    return pData;
}

PdfKernelHandle::ReflowCacheData* PdfKernelHandle::FindCachedReflowPage(IDKPPageEx* target)
{
    pthread_mutex_lock(&m_listLock);
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::FindCachedReflowPage:%d", m_cacheReflowData.size());
    PdfKernelHandle::ReflowCacheData* pData = 0;
    // find reference page for pre-load
    for(unsigned int i = 0; i < m_cacheReflowData.size(); i++)
    {
        if (m_cacheReflowData.at(i)->pReflowPage == target)
        {
            pData = m_cacheReflowData.at(i);
        }
    }
    pthread_mutex_unlock(&m_listLock);
    return pData;
}

void PdfKernelHandle::RemoveRedundantReflowPages(PdfKernelHandle::ReflowCacheData* pCurrentPageCache)
{
    DK_FLOWPOSITION currentPagePos;
    if (pCurrentPageCache->pReflowPage->GetPageStartPos(&currentPagePos))
    {
        int currentPageNum = currentPagePos.nChapterIndex - 1;
        int iMaxPage = currentPageNum + m_uCacheListSize / 2;
        int iMinPage = iMaxPage + 1 - m_uCacheListSize;
        iMaxPage = iMaxPage < (int)m_uPageCount? iMaxPage: m_uPageCount - 1;
        iMinPage = iMinPage < 1 ? 0 : iMinPage;
        for (unsigned int i = 0; i < m_cacheReflowData.size(); i++)
        {
            bool needClearThisPage = false;
            DK_FLOWPOSITION pageStart;
            if (m_cacheReflowData[i]->pReflowPage->GetPageStartPos(&pageStart))
            {
                int pageNum = pageStart.nChapterIndex - 1;
                if ((pageNum > iMaxPage || pageNum < iMinPage) || // page is out of date
                    !(m_cacheReflowData[i]->parserOption == pCurrentPageCache->parserOption)) // parser option is out of date
                {
                    DebugPrintf (DLC_LIUHJ,"Delete page (%d, %d, %d)", pageStart.nChapterIndex, pageStart.nParaIndex, pageStart.nElemIndex);
                    needClearThisPage = true;
                }
            }
            else
            {
                needClearThisPage = true;
            }

            if (needClearThisPage)
            {
                ClearReflowCacheData(m_cacheReflowData[i]);
                m_cacheReflowData.erase(m_cacheReflowData.begin() + i);
            }
        }
    }
}

PdfKernelHandle::ReflowCacheData* PdfKernelHandle::LoadReflowCachePage(DK_FLOWPOSITION& currentPos,
                                                 IDKPDoc::REARRANGE_PAGE_POSITION_TYPE posType)
{
    // It may not find target page even though it has been cached,
    // because the parsed position might not be equal to the reference value
    PdfKernelHandle::ReflowCacheData* pData = FindCachedReflowPage(currentPos, m_rearrangeOption, posType);

    if (pData == 0)
    {
        DebugPrintf (DLC_LIUHJ, "Page(%d, %d, %d), PosType(%d) is parsing!", currentPos.nChapterIndex, currentPos.nParaIndex, currentPos.nElemIndex, posType);

        // Rendering
        pData = new PdfKernelHandle::ReflowCacheData;
        DK_ASSERT(pData);

        // TODO. Add lock
        pData->parserOption = m_rearrangeOption;
        //pData->mode = m_stModeController;
        if (!PdfGetReflowPage(currentPos, posType, pData->parserOption, &(pData->pReflowPage)))
        {
            DebugPrintf (DLC_LIUHJ, "Failed to get reflow page, notify main thread to stop waiting");
            delete pData;
            pData = 0;
        }
        else
        {
            pthread_mutex_lock(&m_listLock);

            DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::LoadReflowCachePage After Lock");
            // Reference position should be updated
            if (posType == IDKPDoc::LOCATION_PAGE || posType == IDKPDoc::NEXT_PAGE)
                pData->pReflowPage->GetPageStartPos(&currentPos);
            else
                pData->pReflowPage->GetPageEndPos(&currentPos);

            // If repeat submit parsing request, replace the previous one
            std::vector<ReflowCacheData*>::iterator i = m_cacheReflowData.begin();
            while (i != m_cacheReflowData.end())
            {
                if (IsWantedReflowPage(*i, currentPos, pData->parserOption, posType))
                {
                    DebugPrintf (DLC_LIUHJ,"Find repeat page, use latest replace previous one");
                    ClearReflowCacheData(*i);
                    m_cacheReflowData.erase(i);
                }
                else
                {
                    i++;
                }
            }

            // Cache the new page
            m_cacheReflowData.push_back(pData);
            pthread_mutex_unlock(&m_listLock);
        }
    }

    pthread_mutex_lock(&m_listLock);
    //如果当前主线程被阻塞&&当前页是主线程正在等待的页面则发送信号给主线程.
    if (m_bMainWaitForReflowPage &&
        (pData == 0 || IsWantedReflowPage(pData, m_posForReflowReference, m_rearrangeOption, m_posTypeForReflowReference)))
    {
        DebugPrintf (DLC_LIUHJ,"PRERENDER: PdfKernelHandle::LoadReflowCachePage NOTIFY Main thread");
        pthread_cond_signal(&m_listSignal);
        m_bMainWaitForReflowPage = false;
    }

    pthread_mutex_unlock(&m_listLock);
    return pData;
}

DK_IMAGE* PdfKernelHandle::LoadReflowCacheImage(PdfKernelHandle::ReflowCacheData* page)
{
    if (NULL == page || NULL == page->pReflowPage)
    {
        return NULL;
    }

    DK_IMAGE *pImage = 0;
    if (page->pImage == 0)
    {
        // Ensure the image could be rendered in background
        int width  = m_uBookSetScreenWidth;
        int height = m_uBookSetScreenHeight;
        DK_FONT_SMOOTH_TYPE fontSmoothType = m_eFontSmoothType;
        pImage = PdfDrawReflowPage(page->pReflowPage, &(pImage), width, height, fontSmoothType);
    }

    pthread_mutex_lock(&m_listLock);

    // Update image of current page
    if (page->pImage == 0 && pImage != 0)
        page->pImage = pImage;

    //如果当前主线程被阻塞&&当前页是主线程正在等待的页面则发送信号给主线程.
    if (m_bMainWaitForReflowImage &&
        IsWantedReflowPage(page, m_posForReflowReference, m_rearrangeOption, m_posTypeForReflowReference)) // TODO. Redefine this function
    {
        DebugPrintf (DLC_LIUHJ,"PRERENDER: PdfKernelHandle::LoadReflowCacheImage NOTIFY Main thread");
        m_bMainWaitForReflowImage = false;
        pthread_cond_signal(&m_listSignal);
    }

    pthread_mutex_unlock(&m_listLock);
    return page->pImage;
}

void PdfKernelHandle::DeleteBeyondPage()
{
    int iMaxPage = m_uCurPageNum + m_uCacheListSize / 2;
    int iMinPage = iMaxPage + 1 - m_uCacheListSize;
    iMaxPage = iMaxPage < (int)m_uPageCount? iMaxPage: m_uPageCount - 1;
    iMinPage = iMinPage < 1? 0: iMinPage;
    int iTmpPage = 0;
    for (unsigned int i = 0; i < m_cacheList.size(); i++)
    {
        iTmpPage = m_cacheList[i]->uPageNum;
        if ((iTmpPage > iMaxPage || iTmpPage < iMinPage) && iTmpPage != (int)m_uPriorityLoadPage)
        {
            DeleteCacheData(m_cacheList.at(i));
            m_cacheList.erase(m_cacheList.begin() + i);
        }
    }
}

DK_IMAGE* PdfKernelHandle::PdfDrawPage(
    unsigned int pageNum, 
    double* pScale/* = NULL*/, 
    DK_BOX* pPageBox/* = NULL*/, 
    PdfModeController* pModeController/* = NULL*/, 
    std::vector<PDFTextElement>* pTextElementsInPage/* = NULL*/)
{
    PdfModeController modeController = m_stModeController;
    if (pModeController)
    {
        modeController = *pModeController;
    }
    if (PDF_CEM_Custom == modeController.m_eCuttingEdgeMode)//自定义切边
    {
        double modeLeftEdge = 0;
        double modeRightEdge = 0;
        double modeTopEdge = 0;
        double modeBottomEdge = 0;
        ValidEdge(pageNum, modeController, modeLeftEdge, modeTopEdge, modeRightEdge, modeBottomEdge);

        double dWidthZoom = 1.0 / (1.0 - modeLeftEdge - modeRightEdge);
        double dHeightZoom = 1.0 / (1.0 - modeTopEdge - modeBottomEdge);
        double dZoom = dk_min(dWidthZoom, dHeightZoom);

        DK_IMAGE* pImage = PdfDrawPage(pageNum, (unsigned int)(m_uPageWidth * dZoom), (unsigned int)(m_uPageHeight * dZoom), pScale, pPageBox, pTextElementsInPage);
        if (NULL == pImage)
        {
            return NULL;
        }

        DK_IMAGE* pDestImage = GetStripEdgeImage(pageNum, pImage, pModeController);
        DestroyImage(pImage);
        return pDestImage;
    }
    else if (PDF_CEM_Smart == modeController.m_eCuttingEdgeMode)// 智能切边
    {
        // 跟根上一次切边情况取图
        double dZoom = m_dLastPageZoom;
        DK_IMAGE* pImage = PdfDrawPage(pageNum, (unsigned int)(m_uPageWidth * dZoom), (unsigned int)(m_uPageHeight * dZoom), pScale, pPageBox, pTextElementsInPage);
        if (NULL == pImage)
        {
            return NULL;
        }

        DK_RECT edgeRect;
        memset(&edgeRect, 0, sizeof(edgeRect));
        // TODO:返回值判断
        GetImageEdge(*pImage, &edgeRect);
        if (NeedReCutEdge(edgeRect, pImage->iWidth, pImage->iHeight, pModeController))
        {
            // 需要重新取图
            DestroyImage(pImage);
            pImage = PdfDrawPage(pageNum, (unsigned int)(m_uPageWidth * m_dLastPageZoom), (unsigned int)(m_uPageHeight * m_dLastPageZoom), pScale, pPageBox, pTextElementsInPage);
        }

        DK_IMAGE* pDestImage = GetStripEdgeImage(pageNum, pImage, pModeController);
        DestroyImage(pImage);
        return pDestImage;
    }
    else // 默认不切边
    {
        return PdfDrawPage(pageNum, m_uPageWidth, m_uPageHeight, pScale, pPageBox, pTextElementsInPage);    
    }

    return NULL;
}

DK_IMAGE* PdfKernelHandle::PdfDrawPage(unsigned int pageNum,
                                       unsigned int width,
                                       unsigned int height,
                                       double* pScale/* = NULL*/,
                                       DK_BOX* pPageBox/* = NULL*/, 
                                       std::vector<PDFTextElement>* pTextElementsInPage/* = NULL*/)
{
    DebugPrintf(DLC_AUTOTEST, "PdfKernelHandle::PdfDrawPage pageNum = %d", pageNum);

    AutoLock lock(&m_pdfLibLock);
    IDKPPage *pPage = m_pDoc->GetPage(pageNum + 1);
    if(NULL == pPage)
    {
        return NULL;
    }

    
    DKP_DRECT stRect;
    DKP_DRECT stTempRect;
    memset(&stRect, 0, sizeof(stRect));

    pPage->GetPageBBox(DKP_PAGEBOX_MEDIABOX, &stRect);
    pPage->GetPageBBox(DKP_PAGEBOX_CROPBOX, &stTempRect);
    stRect.left = dk_max(stRect.left, stTempRect.left);
    stRect.right = dk_min(stRect.right, stTempRect.right);
    stRect.top = dk_min(stRect.top, stTempRect.top);
    stRect.bottom = dk_max(stRect.bottom, stTempRect.bottom);
    const DK_DOUBLE& stRectWidth = (stRect.right - stRect.left);
    const DK_DOUBLE& stRectHeight = (stRect.top - stRect.bottom);
    if (0 == width || 0 == height)
    {
        width = (int)stRectWidth;
        height = (int)stRectHeight;
    }

    double dScale = dk_min((double)width / stRectWidth, (double)height / stRectHeight);
    DK_RENDERINFO renderInfo;
    DkZero(renderInfo);
    renderInfo.nPageNum = pageNum + 1;
    renderInfo.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
    renderInfo.dXScale = dScale;
    renderInfo.dYScale = dScale;
    renderInfo.dRotate = m_stModeController.m_iRotation;
    renderInfo.dwSmoothTag = (DK_SMOOTH_IMAGE | DK_SMOOTH_GRAPH | DK_SMOOTH_TEXT);
    DK_IMAGE* renderImage = NULL;
    double pageWidth = stRectWidth * dScale;
    double pageHeight = stRectHeight * dScale;
    if (m_stModeController.m_iRotation % 180)
    {
        swap(pageWidth, pageHeight);
    }
    DebugPrintf(DLC_AUTOTEST, "PdfKernelHandle::PdfDrawPage inSize = [%f, %f]", pageWidth, pageHeight);
    DK_BOX rcPage(0, 0, stRectWidth, stRectHeight);
    if (IsZoomedPdf())
    {
        ValidZoomedXY(m_stModeController.m_zoomX0, m_stModeController.m_zoomY0);
        renderImage = GetZoomRenderImage(rcPage, dScale, renderInfo);
    }
    else
    {
        // 像素
        renderInfo.rcPage = rcPage;
        renderImage = GetRenderImage((int)pageWidth, (int)pageHeight, renderInfo);
    }
    DebugPrintf(DLC_AUTOTEST, "PdfKernelHandle::PdfDrawPage outSize = [%d, %d]", renderImage->iWidth, renderImage->iHeight);
    if (pTextElementsInPage)
    {
        pTextElementsInPage->clear();
        unsigned int codeIndex = 0;
        PDKPTEXTINFONODE pStreamHead(DK_NULL);
        if (pPage->GetPageTextContentStream(pStreamHead))
        {
            while (pStreamHead)
            {
                const DKPTEXTINFO& node = pStreamHead->Node;
                PDKPDISPLAYCHARNODE pNext = node.pCodeContent;
                const DK_FLOWPOSITION& curPos = pStreamHead->pos;
                codeIndex = 0;

                while (pNext)
                {
                    const DKPDISPLAYCHAR& charNode = pNext->charNode;
                    const DKP_DRECT& rcBox = charNode.rcBox;

                    const DK_BOX box(rcBox.left, rcBox.top, rcBox.right, rcBox.bottom);
                    const DK_POS pos(rcBox.left, rcBox.top);
                    const DK_FLOWPOSITION flowposition(curPos.nChapterIndex, curPos.nParaIndex, codeIndex);
                    pTextElementsInPage->push_back(PDFTextElement(flowposition, box, pos.X, pos.Y, (DK_WCHAR)charNode.uUnicode[0], node.dXFontSize));

                    ++codeIndex;
                    pNext = pNext->pNext;
                }

                pStreamHead = pStreamHead->pNext;
            }
        }
        pPage->FreePageContentStream();
    }
    
    m_pDoc->ReleasePage(pPage, true);
    if (pPageBox)
    {
        *pPageBox = rcPage;
    }
    if (pScale)
    {
        *pScale = dScale;
    }
    return renderImage;
}

DK_IMAGE* PdfKernelHandle::GetZoomRenderImage(const DK_BOX& pageBox, double dScale, DK_RENDERINFO& renderInfo)
{
    const double& imageWidth = pageBox.Width() * dScale;
    const double& imageHeight = pageBox.Height() * dScale;
    m_stModeController.m_imageX = (m_uPageWidth - imageWidth)/2;
    m_stModeController.m_imageY = (m_uPageHeight - imageHeight)/2;
    const DK_DOUBLE& zoomX0 = m_stModeController.m_zoomX0;
    const DK_DOUBLE& zoomY0 = m_stModeController.m_zoomY0;

	//大图中需要裁减出的区域
	DKP_DRECT rectCrop;
	rectCrop.left = zoomX0;
    rectCrop.bottom = zoomY0;
    rectCrop.right = rectCrop.left + m_uScreenWidth;
    rectCrop.top = rectCrop.bottom + m_uScreenHeight;

	//原图像放大后的区域
	DKP_DRECT rectImage;
	rectImage.left = ((double)m_uPageWidth - imageWidth)/2;
	rectImage.right = rectImage.left + imageWidth;
	rectImage.bottom = ((double)m_uPageHeight - imageHeight)/2;
	rectImage.top = rectImage.bottom + imageHeight;
	
	//交叉图像的区域
	DKP_DRECT rectCross = CrossRect(rectCrop, rectImage);
	int crossImageWidth = (int)(rectCross.right - rectCross.left);
	int crossImageHeight = (int)(rectCross.top - rectCross.bottom);

    renderInfo.rcPage.X0 = (rectCross.left - rectImage.left)/dScale;
    renderInfo.rcPage.Y0 = (rectCross.bottom - rectImage.bottom)/dScale;
    renderInfo.rcPage.X1 = renderInfo.rcPage.X0 + crossImageWidth;
    renderInfo.rcPage.Y1 = renderInfo.rcPage.Y0 + crossImageHeight;

	// 图像的实际大小
	DK_IMAGE* pImage = CreateImage(m_uScreenWidth, m_uScreenHeight, 4, m_uScreenWidth * 4);
	if(pImage)
	{
		DK_IMAGE* crossImage = GetRenderImage(crossImageWidth, crossImageHeight, renderInfo);
		if(crossImage)
        {
            DK_DOUBLE xOffset = 0, yOffset = 0;
            ValidZoomedImageOffset(pageBox, dScale, xOffset, yOffset);
			CopyImageEx(*pImage,
				(int)xOffset,
				(int)yOffset,
				*crossImage, 0, 0,
				(int)crossImage->iWidth,
				(int)crossImage->iHeight);
		}
		DestroyImage(crossImage);
	}
	return pImage;
}

void PdfKernelHandle::ValidZoomedImageOffset(const DK_BOX& pageBox, double scale, DK_DOUBLE& xOffset, DK_DOUBLE& yOffset)
{
    const DK_DOUBLE& zoomX0 = m_stModeController.m_zoomX0;
    const DK_DOUBLE& zoomY0 = m_stModeController.m_zoomY0;
    const DK_DOUBLE& xMargin = ((double)m_uPageWidth - pageBox.Width() * scale)/2;
    const DK_DOUBLE& yMargin = ((double)m_uPageHeight - pageBox.Height() * scale)/2;
    xOffset = dk_max(zoomX0, xMargin);
    yOffset = dk_max(zoomY0, yMargin);
    xOffset -= zoomX0;
    yOffset -= zoomY0;
}

DK_IMAGE* PdfKernelHandle::GetRenderImage(int width, int height, DK_RENDERINFO& renderInfo)
{
    DK_PROFILE("PdfKernelHandle::GetRenderImage");
	DK_IMAGE* pImage = CreateImage(width, height, 4, width*4);
	if (pImage)
	{
		DK_BITMAPBUFFER_DEV dev;
		dev.lHeight = pImage->iHeight;
		dev.lStride = pImage->iStrideWidth;
		dev.lWidth	= pImage->iWidth;
		dev.nDPI	= 72;
		dev.nPixelFormat = DK_PIXELFORMAT_RGB32;
		dev.pbyData = pImage->pbData;
		renderInfo.pDevice = &dev;
		m_pDoc->RenderPage(&renderInfo);
		return pImage;
	}
	return NULL;
}

DK_IMAGE* PdfKernelHandle::GetPageStatic()
{
    if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode)
    {
        return PdfDrawReflowPage(m_pReflowPage->pReflowPage,
                                 &m_pOutputImage,
                                 m_uBookSetScreenWidth,
                                 m_uBookSetScreenHeight,
                                 m_eFontSmoothType);
    }

    DK_IMAGE* pImg (NULL);
    pImg = PdfDrawPage (m_uCurPageNum, m_uBookSetScreenWidth, m_uBookSetScreenHeight);
    if (pImg)
    {
        vector<IBMPProcessor*>::iterator vi;
        for (vi = m_lstProcessor.begin(); vi != m_lstProcessor.end(); ++vi)
        {
            if (NULL == *vi || !(*vi)->Process(pImg))
            {
                break;
            }
        }
    }
    return pImg;
}

bool PdfKernelHandle::CheckPageIsExistence(unsigned int pageNum)
{
    bool bRet = false;
    pthread_mutex_lock(&m_listLock);

    for(unsigned int i = 0; i < m_cacheList.size(); i++)
    {
        if(m_cacheList.at(i)->uPageNum == pageNum)
        {
            bRet = true;
            break;
        }
    }

    pthread_mutex_unlock(&m_listLock);
    return bRet;
}

bool PdfKernelHandle::IsFirstPageInChapter()
{
    return false;
}

bool PdfKernelHandle::IsLastPageInChapter()
{
    return false;
}

bool PdfKernelHandle::Next()
{
    bool bRet(false);
    if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode)
    {
        if (m_endPos.nChapterIndex <= m_uPageCount)
        {
            // Replace by asynchronize parsing
            ReflowCacheData* updatedPage = GetReflowPage(m_endPos, IDKPDoc::NEXT_PAGE);
            if (updatedPage != 0)
            {
                m_pReflowPage = updatedPage;
                UpdateReflowScreen();
            }
            return (updatedPage != 0 && m_endPos.nChapterIndex <= m_uPageCount + 1);
        }
        else
        {
            return false;
        }
    }

    if (PDF_TPM_ScrollPage == m_stModeController.m_eTurnPageMode)
    {
        return ScrollModeNext();
    }
    
    bRet = false;
    pthread_mutex_lock(&m_listLock);

    if (PDF_RM_Split4Page == m_stModeController.m_eReadingMode
        || PDF_RM_Split2Page == m_stModeController.m_eReadingMode)
    {
        unsigned int uSubPageCount = PDF_RM_Split4Page == m_stModeController.m_eReadingMode? 4: 2;
        if (m_stModeController.m_uCurSubPage < uSubPageCount - 1)
        {
            m_stModeController.m_uCurSubPage++;
            bRet = true;
        }
        else
        {
            if (m_uCurPageNum < m_uPageCount - 1)
            {
                m_uCurPageNum++;
                m_uCurPageEndNum++;
                m_stModeController.m_uCurSubPage = 0;
                bRet = true;
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i < m_cacheList.size(); i++)
        {
            if (m_cacheList.at(i)->uPageNum == m_uCurPageNum)
            {
                if (m_uCurSubPageNum < m_cacheList.at(i)->uSubPageCount - 1)
                {
                    m_uCurSubPageNum++;
                    bRet = true;
                }
                else
                {
                    if (m_uCurPageNum < m_uPageCount - 1)
                    {
                        m_uCurPageNum++;
                        m_uCurPageEndNum++;
                        m_uCurSubPageNum = 0;
                        m_uCurSubPageCount = 1;
                        bRet = true;
                    }
                }

                break;
            }
        }
    }

    m_bUpdateCache = true;
    pthread_mutex_unlock(&m_listLock);
    DestroyImage(m_pOutputImage);
    return bRet;
}

bool PdfKernelHandle::Prev()
{
    bool bRet(false);
    if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode)
    {
        if (m_startPos > DK_FLOWPOSITION(1, 0, 0))
        {
            DK_FLOWPOSITION prevStart = m_startPos;
            DK_FLOWPOSITION prevEnd = m_endPos;
            
            // Replace by asynchronize reading
            //bRet = PdfGetReflowPage(m_startPos, IDKPDoc::PREV_PAGE, m_rearrangeOption, &m_pReflowPage);
            ReflowCacheData* updatedPage = GetReflowPage(m_startPos, IDKPDoc::PREV_PAGE);
            if (updatedPage != 0)
            {
                m_pReflowPage = updatedPage;
                UpdateReflowScreen();
            }
            bRet = (updatedPage != 0 && (prevStart != m_startPos || prevEnd != m_endPos));
        }
        return bRet;
    }

    if (PDF_TPM_ScrollPage == m_stModeController.m_eTurnPageMode)
    {
        return ScrollModePrev();
    }
    
    bRet = false;
    pthread_mutex_lock(&m_listLock);
    if (PDF_RM_Split4Page == m_stModeController.m_eReadingMode
    || PDF_RM_Split2Page == m_stModeController.m_eReadingMode)
    {
        unsigned int uSubPageCount = PDF_RM_Split4Page == m_stModeController.m_eReadingMode? 4: 2;
        if (m_stModeController.m_uCurSubPage > 0)
        {
            m_stModeController.m_uCurSubPage--;
            bRet = true;
        }
        else
        {
            if (m_uCurPageNum > 0)
            {
                m_uCurPageNum--;
                m_uCurPageEndNum--;
                m_uPriorityLoadPage = m_uCurPageNum;
                m_stModeController.m_uCurSubPage = uSubPageCount - 1;
                bRet = true;
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i < m_cacheList.size(); i++)
        {
            if (m_cacheList.at(i)->uPageNum == m_uCurPageNum)
            {
                if (m_uCurSubPageNum > 0)
                {
                    m_uCurSubPageNum--;
                    bRet = true;
                }
                else
                {
                    if (m_uCurPageNum > 0)
                    {
                        m_uCurPageNum--;
                        m_uCurPageEndNum--;
                        m_uPriorityLoadPage = m_uCurPageNum;
                        m_uCurSubPageNum = 0;
                        m_uCurSubPageCount = kuMaxSubPageNum;
                        bRet = true;
                    }
                }

                break;
            }
         }
     }

    m_bUpdateCache = true;
    pthread_mutex_unlock(&m_listLock);
    DestroyImage(m_pOutputImage);
    return bRet;
}

bool PdfKernelHandle::AddBMPProcessor(IBMPProcessor* pProcessor)
{
    DK_ASSERT(pProcessor);
    if (m_lstProcessor.end() != find(m_lstProcessor.begin(), m_lstProcessor.end(), pProcessor))
    {
        return false;  
    }
    
    m_lstProcessor.push_back(pProcessor);
    return true;
}

bool PdfKernelHandle::RemoveBMPProcessor(IBMPProcessor* pProcessor)
{
    DK_ASSERT(pProcessor);
    vector<IBMPProcessor*>::iterator vi;
    vi = find(m_lstProcessor.begin(), m_lstProcessor.end(), pProcessor); 
    if (vi != m_lstProcessor.end())
    {
        m_lstProcessor.erase(vi);
        return true;
    }
    
    return false;
}

bool PdfKernelHandle::GetAllBMPProcessor(std::vector<IBMPProcessor*>& rProcessorList)
{
    rProcessorList = m_lstProcessor;
    return true;
}

bool PdfKernelHandle::SetModeController(const PdfModeController& modeController)
{
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::SetModeController ENTRANCE");
    CloseCache();
    m_stModeController = modeController;

    if (PDF_CEM_Smart == m_stModeController.m_eCuttingEdgeMode)
    {
        m_stModeController.m_dLeftEdge = 0.0;
        m_stModeController.m_dRightEdge = 0.0;
        m_stModeController.m_dTopEdge = 0.0;
        m_stModeController.m_dBottomEdge = 0.0;
    }
    
    if (PDF_CEM_Custom == m_stModeController.m_eCuttingEdgeMode)
    {
        m_subPageEdge.leftEdge = m_stModeController.m_dLeftEdge;
        m_subPageEdge.rightEdge = m_stModeController.m_dRightEdge;
        m_subPageEdge.topEdge = m_stModeController.m_dTopEdge;
        m_subPageEdge.bottomEdge = m_stModeController.m_dBottomEdge;
    }
    else
    {
        m_subPageEdge.leftEdge = 0.0;
        m_subPageEdge.rightEdge = 0.0;
        m_subPageEdge.topEdge = 0.0;
        m_subPageEdge.bottomEdge = 0.0;
    }

    if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode)
    {
        m_startPos.nChapterIndex = m_uCurPageNum + 1;
        m_startPos.nParaIndex = 0;
        m_startPos.nElemIndex = 0;
    }

    InitPageSize();
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::SetModeController FINISH");
    return true;
}

void PdfKernelHandle::InitPageSize()
{
    double dWidthZoom = 1.0;
    double dHeightZoom = 1.0;
    double dZoom = 1.0;
    if (0 != m_stModeController.m_iRotation % 180)
    {
        m_uScreenHeight = m_uBookSetScreenWidth;
        m_uScreenWidth  = m_uBookSetScreenHeight;
    }
    else
    {
        m_uScreenWidth = m_uBookSetScreenWidth;
        m_uScreenHeight = m_uBookSetScreenHeight;

    }
    
    switch (m_stModeController.m_eReadingMode)
    {
    case PDF_RM_Split4Page://四分栏
        dWidthZoom = 1.0 / dk_max(m_stModeController.m_dWidth, 1 - m_stModeController.m_dWidth);
        dHeightZoom = 1.0 / dk_max(m_stModeController.m_dHeight, 1 - m_stModeController.m_dHeight);
        dZoom = dk_min(dWidthZoom, dHeightZoom);
        m_uPageWidth = (unsigned int)(m_uScreenWidth * dZoom);
        m_uPageHeight = (unsigned int)(m_uScreenHeight * dZoom);
        break;
    case PDF_RM_Split2Page://两分栏
        dZoom = 1.0 / dk_max(m_stModeController.m_dWidth, 1 - m_stModeController.m_dWidth);
        m_uPageWidth = (unsigned int)(m_uScreenWidth * dZoom);
        m_uPageHeight = (unsigned int)(m_uScreenHeight);
        break;
    case PDF_RM_ZoomPage://放大
        m_uPageWidth = (unsigned int)(m_uScreenWidth * m_stModeController.m_dUserZoom);
        m_uPageHeight = (unsigned int)(m_uScreenHeight * m_stModeController.m_dUserZoom);
        break;
    case PDF_RM_AdaptiveWidth:
        m_uPageWidth = m_uScreenWidth;
        // 宽度自适应下,默认把高度放到最大,这样就会按照宽度的放大比例去缩放
        m_uPageHeight = m_uScreenHeight * 5;
        break;
    default:
    case PDF_RM_NormalPage://正常模式
        m_uPageWidth = m_uScreenWidth;
        m_uPageHeight = m_uScreenHeight;
        break;
    }
}

bool PdfKernelHandle::GetImageEdge(const DK_IMAGE& rImage, DK_RECT* pRect)
{
    DK_ASSERT(pRect);
    HRESULT hr = E_FAIL;
    if (NULL == pRect)
    {
        return false;
    }

    if (1 == rImage.iColorChannels)
    {
        hr = GetPageEdge(rImage.pbData, rImage.iWidth, rImage.iHeight, rImage.iStrideWidth, 1, pRect);
    }
    else if (4 == rImage.iColorChannels)
    {
        DK_IMAGE* pTmp = CreateImage(rImage.iWidth, rImage.iHeight, 1, rImage.iWidth);
        if (NULL == pTmp)
        {
            return false;
        }

        unsigned char* pSrc = rImage.pbData;
        unsigned char* pDes = pTmp->pbData;
        unsigned char* pSrcLine = pSrc;
        unsigned char* pDesLine = pDes;
        for (int i = 0; i < rImage.iHeight; i++)
        {
            for (int j = 0; j < rImage.iWidth; j++)
            {
                *pDesLine = (pSrcLine[0] + pSrcLine[1] + pSrcLine[2]) / 3;
                pSrcLine += 4;
                pDesLine++;
            }
            
            pSrc += rImage.iStrideWidth;
            pDes += pTmp->iStrideWidth;
            pSrcLine = pSrc;
            pDesLine = pDes;
        }

        hr = GetPageEdge(pTmp->pbData, pTmp->iWidth, pTmp->iHeight, pTmp->iStrideWidth, 1, pRect);
        DestroyImage(pTmp);
    }
    else
    {
        return false;
    }

    //空白页处理
    if (pRect->left > pRect->right || pRect->top > pRect->bottom)
    {
        pRect->left = 0;
        pRect->top = 0;
        pRect->right = rImage.iWidth;
        pRect->bottom = rImage.iHeight;
    }

    if (pRect->left > kiSmartCutPageEdge)
    {
        pRect->left -= kiSmartCutPageEdge;
    }
    else
    {
        pRect->left = 0;
    }

    if (pRect->top > kiSmartCutPageEdge)
    {
        pRect->top -= kiSmartCutPageEdge;
    }
    else
    {
        pRect->top = 0;
    }

    if (pRect->right + kiSmartCutPageEdge < rImage.iWidth)
    {
        pRect->right += kiSmartCutPageEdge;
    }
    else
    {
        pRect->right = rImage.iWidth;
    }

    if (pRect->bottom + kiSmartCutPageEdge < rImage.iHeight)
    {
        pRect->bottom += kiSmartCutPageEdge;
    }
    else
    {
        pRect->bottom = rImage.iHeight;
    }
    
    return hr == S_OK;
}

const PdfModeController* PdfKernelHandle::GetModeController()
{
    return &m_stModeController;
}

bool PdfKernelHandle::NeedReCutEdge(DK_RECT& rEdgeRect, int imageWidth, int imageHeight, PdfModeController* pModeController)
{
    double dTopEdge = (double)rEdgeRect.top / imageHeight;
    double dBottomEdge = 1 - (double)rEdgeRect.bottom / imageHeight;
    double dLeftEdge = (double)rEdgeRect.left / imageWidth;
    double dRightEdge = 1 - (double)rEdgeRect.right/ imageWidth;

    if (m_dLastPageZoom < 0.0)
    {
        m_dLastPageZoom = 1.0;
    }
    double pageWidth = (double)m_uPageWidth;
    double pageHeight = (double)m_uPageHeight;
    if (pModeController)
    {
        if (pModeController->m_iRotation % 180)
        {
            swap(pageWidth, pageHeight);
        }
    }
    else if (m_stModeController.m_iRotation % 180)
    {
        swap(pageWidth, pageHeight);
    }
    double dCurWidthZoom = m_dLastPageZoom * pageWidth / (rEdgeRect.right - rEdgeRect.left);
    double dCurHeightZoom = m_dLastPageZoom * pageHeight / (rEdgeRect.bottom - rEdgeRect.top);
    double dCurZoom = dk_min(dCurWidthZoom, dCurHeightZoom);

    double dResult = m_dLastPageZoom < dCurZoom? dCurZoom - m_dLastPageZoom: m_dLastPageZoom - dCurZoom;
    m_dLastPageZoom = dCurZoom;
    if (pModeController)
    {
        pModeController->m_dLeftEdge = dLeftEdge;
        pModeController->m_dRightEdge = dRightEdge;
        pModeController->m_dTopEdge = dTopEdge;
        pModeController->m_dBottomEdge = dBottomEdge;
    }
    else
    {
        m_stModeController.m_dLeftEdge = dLeftEdge;
        m_stModeController.m_dRightEdge = dRightEdge;
        m_stModeController.m_dTopEdge = dTopEdge;
        m_stModeController.m_dBottomEdge = dBottomEdge;
    }
    if (m_dLastPageZoom > kdMaxZoom)
    {
        m_dLastPageZoom = kdMaxZoom;
    }

    return dResult > 0.001;
}

DK_IMAGE* PdfKernelHandle::GetStripEdgeImage(unsigned int pageNum, const DK_IMAGE* pSrc, PdfModeController* pModeController)
{
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::GetStripEdgeImage() Start");
    PdfModeController modeController = m_stModeController;
    if (pModeController)
    {
        modeController = *pModeController;
    }
    double modeLeftEdge = 0, modeRightEdge = 0, modeTopEdge = 0, modeBottomEdge = 0;
    ValidEdge(pageNum, modeController, modeLeftEdge, modeTopEdge, modeRightEdge, modeBottomEdge);
    const int srcWidth = pSrc->iWidth;
    const int srcHeight = pSrc->iHeight;

    int sx = (int)(srcWidth * modeLeftEdge);
    int sy = (int)(srcHeight * modeTopEdge);
    int dw = (int)(srcWidth * (1.0 - modeLeftEdge - modeRightEdge));
    int dh = (int)(srcHeight * (1.0 - modeTopEdge - modeBottomEdge));

    int pageWidth = (int)m_uPageWidth, pageHeight = (int)m_uPageHeight;
    if (modeController.m_iRotation % 180)
    {
        swap(pageWidth, pageHeight);
    }

    if (dw > pageWidth)
    {
        sx += (dw - pageWidth) >> 1;
        dw = pageWidth;
    }

    if (dh > pageHeight)
    {
        sy += (dh - pageHeight) >> 1;
        dh = pageHeight;
    }
    
    DK_IMAGE* pDestImage = CreateImage(dw, dh, pSrc->iColorChannels, pSrc->iColorChannels * dw);
    ImageBitBlt(pDestImage, 0, 0, dw, dh, pSrc, sx, sy);
    return pDestImage;
}

DK_IMAGE* PdfKernelHandle::ScrollModeGetPage()
{
    unsigned int uCurPage = m_uCurPageNum;
    DK_IMAGE* pCurImage = GetPage(uCurPage);
    if (NULL == pCurImage)
    {
        return NULL;
    }
    int outImageWidth = m_uBookSetScreenWidth;
    int outImageHeight = m_uBookSetScreenHeight;
    m_pOutputImage = CreateImage(outImageWidth, outImageHeight, pCurImage->iColorChannels, pCurImage->iColorChannels * outImageWidth, 0);
    if (NULL == m_pOutputImage)
    {
        return NULL;
    }

    int iScreenX = 0;
    int iScreenY = 0;
    int iBltWidth = 0;
    int iBltHeight = 0;
    int imageWidth = pCurImage->iWidth;
    int imageHeight = pCurImage->iHeight;
    if ((int)m_stModeController.m_uCurPageStartPos > ((m_stModeController.m_iRotation % 180) ? imageWidth : imageHeight))
    {
        m_stModeController.m_uCurPageStartPos = 0;
    }
    int iCurPos = m_stModeController.m_uCurPageStartPos;
    bool bWhile = true;
    while (bWhile)
    {
        int dx = 0, dy = 0;
        int sx = 0, sy = 0;
        switch (m_stModeController.m_iRotation)
        {
        case 0:
            {
                iScreenX = (int)outImageWidth > imageWidth? ((int)outImageWidth - imageWidth)>>1: 0;
                iBltWidth = dk_min((int)outImageWidth, imageWidth);
                iBltHeight = dk_min((int)outImageHeight - iScreenY, imageHeight - iCurPos);
                dx = iScreenX;
                dy = iScreenY;
                sx = 0;
                sy = iCurPos;
                iScreenY += iBltHeight;
                bWhile = iScreenY < (int)outImageHeight;
            }
            break;
        case 90:
            {
                iScreenY = (int)outImageHeight > imageHeight? ((int)outImageHeight - imageHeight)>>1: 0;
                iBltWidth = dk_min((int)outImageWidth - iScreenX, imageWidth - iCurPos);
                iBltHeight = dk_min((int)outImageHeight, imageHeight);
                dx = iScreenX;
                dy = iScreenY;
                sx = iCurPos;
                sy = 0;
                iScreenX += iBltWidth;
                bWhile = iScreenX < (int)outImageWidth;
            }
            break;
        case 180:
            {
                iScreenX = (int)outImageWidth > imageWidth? ((int)outImageWidth - imageWidth)>>1: 0;
                iBltWidth = dk_min((int)outImageWidth, imageWidth);
                iBltHeight = dk_min((int)outImageHeight - iScreenY, imageHeight - iCurPos);
                dx = iScreenX;
                dy = outImageHeight - iScreenY - iBltHeight;
                sx = 0;
                sy = imageHeight - iBltHeight - iCurPos;
                iScreenY += iBltHeight;
                bWhile = iScreenY < (int)outImageHeight;
            }
            break;
        case 270:
            {
                iScreenY = (int)outImageHeight > imageHeight? ((int)outImageHeight - imageHeight)>>1: 0;
                iBltWidth = dk_min((int)outImageWidth - iScreenX, imageWidth - iCurPos);
                iBltHeight = dk_min((int)outImageHeight, imageHeight);
                dx = outImageWidth - iBltWidth - iScreenX;
                dy = iScreenY;
                sx = imageWidth - iBltWidth - iCurPos;
                sy = 0;
                iScreenX += iBltWidth;
                bWhile = iScreenX < (int)outImageWidth;
            }
            break;
        default:
            break;
        }
        ImageBitBlt(m_pOutputImage, dx, dy, iBltWidth, iBltHeight, pCurImage, sx, sy);
        if (!bWhile)
        {
            break;
        }

        if (uCurPage >= m_uPageCount - 1)
        {
            break;
        }
        int x = 0;
        int y = (iScreenY > kiSplitLineHeight? iScreenY - kiSplitLineHeight: iScreenY);
        int w = m_uBookSetScreenWidth;
        int h = kiSplitLineHeight;
        switch (m_stModeController.m_iRotation)
        {
        case 90:
            x = (iScreenX > kiSplitLineHeight? iScreenX - kiSplitLineHeight: iScreenX);
            y = 0;
            w = kiSplitLineHeight;
            h = m_uBookSetScreenHeight;
            break;
        case 180:
            x = 0;
            y = m_uBookSetScreenHeight - (iScreenY > kiSplitLineHeight? iScreenY - kiSplitLineHeight: iScreenY);
            w = m_uBookSetScreenWidth;
            h = kiSplitLineHeight;
            break;
        case 270:
            x = m_uBookSetScreenWidth - (iScreenX > kiSplitLineHeight? iScreenX - kiSplitLineHeight : iScreenX);
            y = 0;
            w = kiSplitLineHeight;
            h = m_uBookSetScreenHeight;
            break;
            break;
        default:
            break;
        }
        ImageFillBox(m_pOutputImage, x, y, w, h, 0xff);

        uCurPage++;
        iCurPos = 0;
        pCurImage = GetPage(uCurPage);
        if (pCurImage)
        {
            imageWidth = pCurImage->iWidth;
            imageHeight = pCurImage->iHeight;
        }
        bWhile = bWhile && (pCurImage != NULL);
    }
    return m_pOutputImage;
}

bool PdfKernelHandle::ScrollModeNext()
{
    unsigned int uCurPage = m_uCurPageNum;
    DK_IMAGE* pCurImage = GetPage(uCurPage);
    if (NULL == pCurImage || NULL == pCurImage->pbData)
    {
        return false;
    }

    int iCurPos = m_stModeController.m_uCurPageStartPos;
    const bool rotationHorizontal = (m_stModeController.m_iRotation % 180) != 0;
    int iRemainLength = rotationHorizontal ? m_uBookSetScreenWidth : m_uBookSetScreenHeight;
    int iBltLength = 0;
    int imageHeight = rotationHorizontal ? pCurImage->iWidth : pCurImage->iHeight;

    while (iRemainLength > 0)
    {    
        iBltLength = dk_min(imageHeight - iCurPos, iRemainLength);
        iRemainLength -= iBltLength;
        iCurPos += iBltLength;
        if (iRemainLength < 1)
        {
            break;
        }
        
        if (uCurPage >= m_uPageCount - 1)
        {
            return false;
        }
        
        uCurPage++;
        pCurImage = GetPage(uCurPage);
        if (pCurImage)
        {
            imageHeight = rotationHorizontal ? pCurImage->iWidth : pCurImage->iHeight;
        }
        iCurPos = 0;
    }

    iCurPos -= kiRepeatHeight;
    iCurPos = iCurPos > 0? iCurPos: 0;
    m_uCurPageNum = m_uCurPageEndNum = uCurPage;
    m_uPriorityLoadPage = m_uCurPageNum;
    m_stModeController.m_uCurPageStartPos = (unsigned int)iCurPos;
    DestroyImage(m_pOutputImage);
    m_bUpdateCache = true;
    return true;
}

bool PdfKernelHandle::ScrollModePrev()
{
    if (m_uCurPageNum < 1 && m_stModeController.m_uCurPageStartPos < 1)
    {
        return false;
    }
    
    unsigned int uCurPage = m_uCurPageNum;
    DK_IMAGE* pCurImage = GetPage(uCurPage);
    if (NULL == pCurImage || NULL == pCurImage->pbData)
    {
        return false;
    }

    int iCurPos = m_stModeController.m_uCurPageStartPos;
    const bool rotationHorizontal = (m_stModeController.m_iRotation % 180) != 0;
    int iRemainLength = rotationHorizontal ? m_uBookSetScreenWidth : m_uBookSetScreenHeight;
    int iBltLength = 0;
    int imageHeight = rotationHorizontal ? pCurImage->iWidth : pCurImage->iHeight;
    while (iRemainLength > 0)
    {
        iBltLength = dk_min(iCurPos, iRemainLength);
        iRemainLength -= iBltLength;
        iCurPos -= iBltLength;
        if (iRemainLength < 1)
        {
            break;
        }
        
        if (uCurPage < 1)
        {
            break;
        }
        
        uCurPage--;
        pCurImage = GetPage(uCurPage);
        if (pCurImage)
        {
            imageHeight = rotationHorizontal ? pCurImage->iWidth : pCurImage->iHeight;
        }
        iCurPos = imageHeight;
    }

    iCurPos += kiRepeatHeight;
    if (iCurPos >= imageHeight)
    {
        iCurPos = 0;
        uCurPage++;
    }
    m_uCurPageEndNum = m_uCurPageNum;
    m_uCurPageNum = uCurPage;
    m_uPriorityLoadPage = m_uCurPageNum;
    m_stModeController.m_uCurPageStartPos = (unsigned int)iCurPos;
    DestroyImage(m_pOutputImage);
    m_bUpdateCache = true;
    return true;
}

vector<DK_TOCINFO*>* PdfKernelHandle::GetTOC()
{
    AutoLock lock(&m_pdfLibLock);
    if (m_pDoc && m_vTOCList.empty())
    {
        // 打开书时只将PageTree爬取至当前页面，
	    // 但获取目录时，需要爬取完整的PageTree，否则无法得到目录对应的页码
	    IDKPOutline* pOutline = NULL;
	    for (int i = 0; i < (int)m_uPageCount; ++i)
	    {
	        m_pDoc->GetPage (i + 1);
	    }

	    // 获得目录对象
	    pOutline = m_pDoc->GetOutline();
	    if (pOutline)
	    {
#ifdef DEBUG_OUTLINE_INFO
	        int iCount = 0;
	        GetOutlineCount (pOutline, 0, &iCount);
	        DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::GetTOC count %d", iCount);
#endif
	    	FormatToc(pOutline, 0, 1);
	    }
    }
    return &m_vTOCList;
}

void PdfKernelHandle::GetOutlineCount (IDKPOutline* pOutline, DK_WORD parentIndex, int* pCount)
{
    if (NULL == pOutline || NULL == pCount)
    {
        return;
    }
    
    // 得到第一个子节点
    DKPOUTLINEITEMINFO outlineInfo;
    if (!pOutline->GetOutlineNextItemInfo (parentIndex, &outlineInfo, DKP_TQ_FIRSTCHILD))
    {
        return;
    }

    (*pCount)++;
    if (outlineInfo.bStartOpen || outlineInfo.bHasChild)
    {
        GetOutlineCount (pOutline, outlineInfo.dwIndex, pCount);
    }

    while (pOutline->GetOutlineNextItemInfo (outlineInfo.dwIndex, &outlineInfo, DKP_TQ_NEXTSIBLING))
    {
        (*pCount)++;
        if (outlineInfo.bStartOpen || outlineInfo.bHasChild)
        {
            GetOutlineCount (pOutline, outlineInfo.dwIndex, pCount);
        }
    }
}

void PdfKernelHandle::FormatToc (IDKPOutline* pOutline, int parentIndex, int depth)
 {    
    if (!pOutline)
    {
        return;
    }
    
    // 得到第一个子节点
    DKPOUTLINEITEMINFO outlineInfo;
    if (!pOutline->GetOutlineNextItemInfo ((DK_WORD)parentIndex, &outlineInfo, DKP_TQ_FIRSTCHILD))
    {
        return;
    }

    // 创建 DK_TOCINFO
    DK_TOCINFO* pChildTOC = new DK_TOCINFO;
    if (!pChildTOC)
    {
        return;
    }
    string chapterName = EncodeUtil::ToString(outlineInfo.pwszTitle);
    if (chapterName.empty())
    {
        DebugPrintf (DLC_LIUHJ, "ERR - PdfKernelHandle::FormatToc Convert chapter name failed");
        return;
    }
    DK_FLOWPOSITION pos (dk_max(0, pOutline->GetOutlineDestPageNum (outlineInfo.dwIndex) - 1), 0, 0);
    pChildTOC->SetCurTOC (chapterName.c_str(), pos);
	pChildTOC->SetDepth(depth);
    m_vTOCList.push_back(pChildTOC);

#ifdef DEBUG_OUTLINE_INFO
    printf ("title:[%-80s], pageNum:(%d)(0-base), outlineIndex:(%d)\n", chapterName.c_str(), pos.nChapterIndex, outlineInfo.dwIndex);
#endif

    if (outlineInfo.bStartOpen || outlineInfo.bHasChild)
    {
        FormatToc (pOutline, outlineInfo.dwIndex, depth + 1);
    }

    while (pOutline->GetOutlineNextItemInfo (outlineInfo.dwIndex, &outlineInfo, DKP_TQ_NEXTSIBLING))
    {
        // 创建 DK_TOCINFO
        DK_TOCINFO* pChildTOC = new DK_TOCINFO;
        if (!pChildTOC)
        {
            continue;
        }
		string chapterItemName = EncodeUtil::ToString(outlineInfo.pwszTitle);
        if (chapterItemName.empty())
        {
            DebugPrintf (DLC_LIUHJ, "ERR - PdfKernelHandle::FormatToc Convert chapter name failed");
            continue;
        }
        DK_FLOWPOSITION pos (dk_max(0, pOutline->GetOutlineDestPageNum (outlineInfo.dwIndex) - 1), 0, 0);
        pChildTOC->SetCurTOC (chapterItemName.c_str(), pos);
		pChildTOC->SetDepth(depth);
        m_vTOCList.push_back(pChildTOC);

#ifdef DEBUG_OUTLINE_INFO
        printf ("title:[%-80s], pageNum:(%d)(0-base), outlineIndex:(%d)\n", chapterItemName.c_str(), pos.nChapterIndex, outlineInfo.dwIndex);
#endif

        if (outlineInfo.bStartOpen || outlineInfo.bHasChild)
        {
            FormatToc (pOutline, outlineInfo.dwIndex, depth + 1);
        }
    }
}

void PdfKernelHandle::RegisterFont (const wchar_t* pFontFaceName, const wchar_t* pFontFilePath)
{
    AutoLock lock(&m_pdfLibLock);
    DKP_RegisterFontFaceName (pFontFaceName, pFontFilePath);
}

void PdfKernelHandle::SetDefaultFont (const wchar_t* pDefaultFontFaceName, DK_CHARSET_TYPE charset)
{
    CloseCache();
    AutoLock lock(&m_pdfLibLock);
    if (m_pDoc)
    {
        m_pDoc->SetDefaultFontFaceName (pDefaultFontFaceName, charset);
    }
    else
    {
        DebugPrintf (DLC_LIUHJ, "WARRNING PdfKernelHandle::SetDefaultFont Called while PDF Kernel is not initialized, do nothing");
    }
}

//重排页面和GetPage() 的内存都需要外部释放
//如果不手动释放的话内核也会释放内存
IDKPPageEx* PdfKernelHandle::GetRenderPageEx(const DK_FLOWPOSITION& pos,
                                             const IDKPDoc::REARRANGE_PAGE_POSITION_TYPE& posType,
                                             DKP_REARRANGE_PARSER_OPTION & reflowParserOption,
                                             IDKPPage** pPage)
{
    
    if(!pPage || !m_pDoc)
    {
        return NULL;
    }

    *pPage = m_pDoc->GetPage(pos.nChapterIndex > 0 ? pos.nChapterIndex : 1);
    if(!(*pPage))
    {
        return NULL;
    }
    
    DOUBLE width (0.0);
    DOUBLE height (0.0);
    DKP_DRECT stRect;
    DKP_DRECT stTempRect;
    
    memset(&stRect, 0, sizeof(stRect));
    (*pPage)->GetPageBBox(DKP_PAGEBOX_MEDIABOX, &stRect);
    (*pPage)->GetPageBBox(DKP_PAGEBOX_CROPBOX, &stTempRect);
    stRect.left = dk_max(stRect.left, stTempRect.left);
    stRect.right = dk_min(stRect.right, stTempRect.right);
    stRect.top = dk_min(stRect.top, stTempRect.top);
    stRect.bottom = dk_max(stRect.bottom, stTempRect.bottom);
    width = stRect.right - stRect.left;
    height = stRect.top - stRect.bottom;

    // 重排页面渲染大小
    reflowParserOption.pageBox.X1 = m_uBookSetScreenWidth;
    reflowParserOption.pageBox.Y0 = m_uBookSetScreenHeight;
    
    // 设置重排区域
    reflowParserOption.subPageBoxUnitType = DKP_UNIT_PX;
    reflowParserOption.subPageBoxOdd.left = width * m_subPageEdge.leftEdge;
    reflowParserOption.subPageBoxOdd.right = width * (1 - m_subPageEdge.rightEdge);
    reflowParserOption.subPageBoxOdd.top = height * (1 - m_subPageEdge.topEdge);
    reflowParserOption.subPageBoxOdd.bottom = height * m_subPageEdge.bottomEdge;
    reflowParserOption.subPageBoxEven = reflowParserOption.subPageBoxOdd;

    // 获取重排页面
    IDKPPageEx* pRearrangePage = NULL;
    m_pDoc->GetPageEx(pos, reflowParserOption, posType, &pRearrangePage);
    return pRearrangePage;
}

bool PdfKernelHandle::UpdateReflowScreen()
{
    if (m_pBookTextController)
    {
        m_pBookTextController->Release();
        m_pBookTextController->Initialize<IDKPPageExTraits>(this, m_pReflowPage->pReflowPage);
    }

    m_pReflowPage->pReflowPage->GetPageStartPos (&m_startPos);
    m_pReflowPage->pReflowPage->GetPageEndPos (&m_endPos);
    DebugPrintf (DLC_LIUHJ, "Update Screen, startPos(%d, %d, %d), endPos(%d, %d, %d)",
                        m_startPos.nChapterIndex, m_startPos.nParaIndex, m_startPos.nElemIndex,
                        m_endPos.nChapterIndex, m_endPos.nParaIndex, m_endPos.nElemIndex);
    // 更新版式页需要的页码信息
    unsigned int pageNum = m_startPos.nChapterIndex - 1;
    if (pageNum < m_uPageCount)
    {
        m_uCurPageNum = m_uCurPageEndNum = pageNum;
        m_uPriorityLoadPage = m_uCurPageNum;
        m_uCurSubPageCount = 1;
        m_uCurSubPageNum = 0;
        m_stModeController.m_uCurSubPage = 0;
        m_stModeController.m_uCurPageStartPos = 0;
    }
    return true;
}

bool PdfKernelHandle::PdfGetReflowPage(const DK_FLOWPOSITION& pos,
                                       const IDKPDoc::REARRANGE_PAGE_POSITION_TYPE& posType,
                                       DKP_REARRANGE_PARSER_OPTION& reflowParserOption,
                                       IDKPPageEx** pRefPage)
{
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::PdfGetReflowPage ENTRANCE, startPos(%d, %d, %d), type %d", 
                 pos.nChapterIndex, pos.nParaIndex, pos.nElemIndex, posType);

    AutoLock lock(&m_pdfLibLock);
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::PdfGetReflowPage After Lock" );
    // 重排页面上翻页时, 无法通过当前页起始位置判断是否可以上翻页.
    // 所以这里先不考虑释放当前页及光标控制对象, 只有当获取新页面成功时才释放.
    IDKPPage* pTempPage = NULL;
    IDKPPageEx* pNewPageEx = GetRenderPageEx(pos, posType, reflowParserOption, &pTempPage);
    if (pNewPageEx == 0)
    {
        return false;
    }

    *pRefPage = pNewPageEx;
    return true;
}

DK_IMAGE* PdfKernelHandle::PdfDrawReflowPage(IDKPPageEx *pRearrangePage,
                                             DK_IMAGE** pImage,
                                             int width,
                                             int height,
                                             DK_FONT_SMOOTH_TYPE fontSmoothType)
{
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::PdfDrawReflowPage ENTRANCE");

    if (pRearrangePage == 0)
    {
        return 0;
    }
    DestroyImage(*pImage);
    
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::PdfDrawReflowPage CreateImage width %d, height %d", m_uBookSetScreenWidth, m_uBookSetScreenHeight);
    *pImage = CreateImage(width, height, 4, width * 4);
    if (*pImage == 0 || pRearrangePage == 0)
    {
        DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::PdfDrawReflowPage CreateImage FAILED");
        return 0;
    }
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::PdfDrawReflowPage CreateImage SUCCEEDED");

    AutoLock lock(&m_pdfLibLock);
    DK_BITMAPBUFFER_DEV dev;
    dev.lHeight = (*pImage)->iHeight;
    dev.lStride = (*pImage)->iStrideWidth;
    dev.lWidth  = (*pImage)->iWidth;
    dev.nDPI    = RenderConst::SCREEN_DPI;
    dev.nPixelFormat = DK_PIXELFORMAT_RGB32;
    dev.pbyData = (*pImage)->pbData;
    
    // 渲染选项
    DK_FLOWRENDEROPTION renderOption;
    renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
    renderOption.pDevice = &dev;
    renderOption.gamma.dTextGamma = m_textGamma;
    renderOption.fontSmoothType = fontSmoothType;
    if (m_bIsInstant)
    {
        renderOption.fontSmoothType = DK_FONT_SMOOTH_BINARY;;
    }
    
    DebugPrintf (DLC_LIUHJ, "PdfDrawReflowPage RenderPage Start m_textGamma %d", m_textGamma);
    pRearrangePage->RenderPage (renderOption);
    DebugPrintf (DLC_LIUHJ, "PdfDrawReflowPage RenderPage Succeeded");

    vector<IBMPProcessor*>::iterator vi;
    for (vi = m_lstProcessor.begin(); vi != m_lstProcessor.end(); ++vi)
    {
        if (*vi && IBMPProcessor::DK_BMPPROCESSOR_DARKEN != (*vi)->GetType())   // 加黑由 IDKPPageEx 处理
        {
            if (!(*vi)->Process(*pImage))
            {
                break;
            }
        }
    }
    return *pImage;
}

bool PdfKernelHandle::SetTextGrayScaleLevel(double gamma)
{
    if (m_textGamma != gamma)
    {
        m_textGamma = gamma;
        CloseCache();
    }
    return true;
}

DK_FLOWPOSITION PdfKernelHandle::GetCurPageLocation()
{
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::GetCurPageLocation ENTRANCE");
    if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode)
    {
        if((m_pReflowPage != NULL) && (m_pReflowPage->pReflowPage != NULL))
        {
            m_pReflowPage->pReflowPage->GetPageStartPos(&m_startPos);
        }
        DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::GetCurPageLocation for PDF_RM_Rearrange SUCCEEDED (%d, %d, %d)", 
                    m_startPos.nChapterIndex - 1, m_startPos.nParaIndex, m_startPos.nElemIndex);
        return DK_FLOWPOSITION (m_startPos.nChapterIndex - 1, m_startPos.nParaIndex, m_startPos.nElemIndex);
    }
    else
    {
        DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::GetCurPageLocation for Normal page SUCCEEDED");
        
        return DK_FLOWPOSITION (m_uCurPageNum, 0, 0);
    }
}

DK_FLOWPOSITION PdfKernelHandle::GetCurPageEndLocation()
{
	if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode)
    {
        if(m_pReflowPage)
        {
            m_pReflowPage->pReflowPage->GetPageEndPos(&m_endPos);
        }
        return DK_FLOWPOSITION (m_endPos.nChapterIndex - 1, m_endPos.nParaIndex, m_endPos.nElemIndex);
    }
    else
    {
        
        return DK_FLOWPOSITION (m_uCurPageEndNum, 0, 0);
    }
}

void PdfKernelHandle::SetLayoutSettings(const DK_LAYOUTSETTING& layoutSetting)
{
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::SetLayoutSettings ENTRANCE");
    if (m_rearrangeOption.lineGap != layoutSetting.dLineGap ||
        m_rearrangeOption.paraSpacing != layoutSetting.dParaSpacing ||
        !DkFloatEqual(m_rearrangeOption.scale, CalcRearrangeFontSizeScale(layoutSetting.dFontSize)))
    {
        // TODO. 
        pthread_mutex_lock(&m_listLock);
        m_rearrangeOption.lineGap = layoutSetting.dLineGap;
        m_rearrangeOption.paraSpacing = layoutSetting.dParaSpacing;
        m_rearrangeOption.scale = CalcRearrangeFontSizeScale(layoutSetting.dFontSize);
        pthread_mutex_unlock(&m_listLock);
        CloseCache();
    }
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::SetLayoutSettings SUCCEEDED, scale %f", m_rearrangeOption.scale);
}

bool PdfKernelHandle::IsPositionInCurrentPage(const DK_FLOWPOSITION& pos)
{
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::IsPositionInCurrentPage ENTRANCE");
    if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode && m_pReflowPage != 0)
    {
        // 重排页位置坐标的章索引是从 1 开始
        DK_FLOWPOSITION rearrangePos = DK_FLOWPOSITION(pos.nChapterIndex + 1, pos.nParaIndex, pos.nElemIndex);
        DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::IsPositionInCurrentPage SUCCEEDED pos (%d, %d, %d), CurPage startPos(%d, %d, %d), endPos(%d, %d, %d)",
            rearrangePos.nChapterIndex, rearrangePos.nParaIndex, rearrangePos.nElemIndex,
            m_startPos.nChapterIndex, m_startPos.nParaIndex, m_startPos.nElemIndex,
            m_endPos.nChapterIndex, m_endPos.nParaIndex, m_endPos.nElemIndex);

        return (rearrangePos >= m_startPos && rearrangePos < m_endPos);
    }
    else
    {
        DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::IsPositionInCurrentPage SUCCEEDED");
        return m_uCurPageNum <= pos.nChapterIndex && pos.nChapterIndex <= m_uCurPageEndNum;
    }
}

BookTextController* PdfKernelHandle::GetBookTextControlerOfCurPage()
{
    DebugPrintf (DLC_ZHY, "PdfKernelHandle::GetCursorControlerOfCurPage ENTRANCE");
    if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode && m_pReflowPage != 0)
    {
        DebugPrintf (DLC_ZHY, "PdfKernelHandle::GetCursorControlerOfCurPage return Rearrange Page cursor");
        return m_pBookTextController;
    }
    else if (m_pBookTextController && m_pBookTextController->IsInitialized())
    {
        DebugPrintf (DLC_ZHY, "PdfKernelHandle::GetCursorControlerOfCurPage return Src Page cursor");
        return m_pBookTextController;
    }

    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::GetCursorControlerOfCurPage return DK_NULL");
    return DK_NULL;
}

void PdfKernelHandle::SetInstantPageModel(bool _bIsInstantMode)
{
    m_bIsInstant = _bIsInstantMode;
}

void PdfKernelHandle::ConvertViewRect(int offset, DKP_DRECT& rect)
{
    if (m_stModeController.m_eTurnPageMode == PDF_TPM_ScrollPage)
    {
        const int& rotation = m_stModeController.m_iRotation;
        switch(rotation)
        {
        case 0:
            rect.top -= offset;
            rect.bottom -= offset;
            break;
        case 90:
            rect.left -= offset;
            rect.right -= offset;
            break;
        case 180:
            rect.top += offset;
            rect.bottom += offset;
            break;
        case 270:
            rect.left += offset;
            rect.right += offset;
            break;
        default:
            break;
        }
        if (rect.top < 0)
        {
            rect.top = 0;
        }
        if (rect.bottom < 0)
        {
            rect.bottom = 0;
        }
        const DK_DOUBLE& screenHeight = (DK_DOUBLE)m_uBookSetScreenHeight;
        if (rect.top > screenHeight)
        {
            rect.top = screenHeight;
        }
        if (rect.bottom > screenHeight)
        {
            rect.bottom = screenHeight;
        }
        if (rect.left < 0)
        {
            rect.left = 0;
        }
        if (rect.right < 0)
        {
            rect.right = 0;
        }
        const DK_DOUBLE& screenWidth = (DK_DOUBLE)m_uBookSetScreenWidth;
        if (rect.left > screenWidth)
        {
            rect.left = screenWidth;
        }
        if (rect.right > screenWidth)
        {
            rect.right = screenWidth;
        }
    }
}

DK_POS PdfKernelHandle::ConvertViewPointToDocPoint(const PdfModeController& modeController, const DK_POS& originalCoodinate) const
{
    DK_POS pos = originalCoodinate;
    if ((modeController.m_eReadingMode == PDF_RM_ZoomPage) && (modeController.m_dUserZoom > 1.0))
    {
        double zoomX0 = 0, zoomY0 = 0;
        if (GetZoomedAttrbutes(zoomX0, zoomY0))
        {
            pos.X += zoomX0;
            pos.Y += zoomY0;
        }
    }
    return pos;
}

void PdfKernelHandle::ConvertPdfRectToDocRect(const DK_BOX& pageBox, const DK_BOX& cuttingPageBox, double scale, DKP_DRECT& rect)
{
    DK_DOUBLE pageHeight = pageBox.Height();
    if (m_stModeController.m_eReadingMode == PDF_RM_ZoomPage)
    {
        double xOffset = 0, yOffset = 0;
        ValidZoomedImageOffset(pageBox, scale, xOffset, yOffset);
        rect.left = rect.left * scale + xOffset;
        rect.right = rect.right * scale + xOffset;
        rect.top = (pageHeight - rect.top) * scale + yOffset;
        rect.bottom = (pageHeight - rect.bottom) * scale + yOffset;
    }
    else
    {
        rect.left = ((rect.left * scale) - cuttingPageBox.X0);
        rect.right = ((rect.right * scale) - cuttingPageBox.X0);
        rect.top = ((pageHeight - rect.top) * scale - cuttingPageBox.Y0);
        rect.bottom = ((pageHeight - rect.bottom) * scale - cuttingPageBox.Y0);
    }
}

void PdfKernelHandle::ConvertDocRectToViewRect(const PdfModeController& modeController, double xScreenOffset, double yScreenOffset, DKP_DRECT& rect)
{
    DK_DOUBLE width = m_uBookSetScreenWidth;
    DK_DOUBLE height = m_uBookSetScreenHeight;
    DKP_DRECT result;
    result.left = result.top = result.right = result.bottom = 0;
    switch (modeController.m_iRotation)
    {
    case 0:
        result.left = rect.left + xScreenOffset;
        result.top = rect.top + yScreenOffset;
        result.right = rect.right + xScreenOffset;
        result.bottom = rect.bottom + yScreenOffset;
        break;
    case 90:
        result.left = rect.top + xScreenOffset;
        result.top = height - rect.right - yScreenOffset;
        result.right = rect.bottom + xScreenOffset;
        result.bottom = height - rect.left - yScreenOffset;
        break;
    case 180:
        result.left = width - rect.right - xScreenOffset;
        result.top = height - rect.bottom - yScreenOffset;
        result.right = width - rect.left - xScreenOffset;
        result.bottom = height - rect.top - yScreenOffset;
        break;
    case 270:
        result.left = width - rect.bottom - xScreenOffset;
        result.top = rect.left + yScreenOffset;
        result.right = width - rect.top - xScreenOffset;
        result.bottom = rect.right + yScreenOffset;
        break;
    default:
        break;
    }
    rect = result;
}

DKP_DRECT PdfKernelHandle::GetCuttingPageRect(const DK_BOX& cuttingPageBox, const DK_BOX& pageBox, int topOffset, int bottomOffset, double scale)
{
    const DK_DOUBLE& pageHeight = pageBox.Height();
    DKP_DRECT cuttingPageRect;
    cuttingPageRect.left = cuttingPageBox.X0 / scale;
    if (m_stModeController.m_eTurnPageMode == PDF_TPM_ScrollPage)
    {
        cuttingPageRect.top = pageHeight - ((DK_DOUBLE)topOffset + cuttingPageBox.Y0) / scale;
        cuttingPageRect.bottom = pageHeight - (cuttingPageBox.Y1 - (DK_DOUBLE)bottomOffset) / scale;
    }
    else
    {
        cuttingPageRect.top = pageHeight - cuttingPageBox.Y0 / scale;
        cuttingPageRect.bottom = pageHeight - cuttingPageBox.Y1 / scale;
    }
    cuttingPageRect.right = cuttingPageBox.X1 / scale;
    return cuttingPageRect;
}

void PdfKernelHandle::AddTextElement(unsigned int cacheIndex)
{
    DK_PROFILE_FUNCTION;
    assert (cacheIndex >= 0 && cacheIndex < m_cacheList.size());

    CacheData* pCacheData = m_cacheList[cacheIndex];
    if (NULL == pCacheData)
    {
        return;
    }
    DK_BOX pageBox = pCacheData->pageBox;

    std::vector<PDFTextElement>* pTextElementsInPage = &pCacheData->textElementsInPage;
    PdfModeController modeController = pCacheData->mode;
    const unsigned int& pageNum = pCacheData->uPageNum;
    double scale = pCacheData->scale;
    m_startPos = DK_FLOWPOSITION(pageNum + 1, 0, 0);
    m_endPos = DK_FLOWPOSITION(pageNum + 1, 0, 0);

    bool bWhile = true;
    unsigned int uCurPage = pageNum;
    int iCurPos = m_stModeController.m_uCurPageStartPos;
    int usedLen = 0, offset = iCurPos;
    int bottomOffset = 0;
    bool isStartPosFound = false;
    while (bWhile && (uCurPage < m_uPageCount))
    {
        bWhile = false;
        bottomOffset = 0;
        if (m_stModeController.m_eTurnPageMode == PDF_TPM_ScrollPage)
        {
            unsigned int newCacheIndex = 0;
            DK_IMAGE* pCurImage = GetPage(uCurPage, &newCacheIndex);
            if (pCurImage)
            {
                const int imageWidth = pCurImage->iWidth;
                const int imageHeight = pCurImage->iHeight;
                const int outImageWidth = (int)m_uBookSetScreenWidth;
                const int outImageHeight = (int)m_uBookSetScreenHeight;

                int iBltLength = 0;
                switch (m_stModeController.m_iRotation)
                {
                case 0:
                case 180:
                    {
                        iBltLength = dk_min(outImageHeight - usedLen, imageHeight - iCurPos);
                        usedLen += iBltLength;
                        bWhile = usedLen < outImageHeight;
                        bottomOffset = imageHeight - iBltLength - iCurPos;
                    }
                    break;
                case 90:
                case 270:
                    {
                        iBltLength = dk_min((int)outImageWidth - usedLen, imageWidth - iCurPos);
                        usedLen += iBltLength;
                        bWhile = usedLen < (int)outImageWidth;
                        bottomOffset = imageWidth - iBltLength - iCurPos;
                    }
                    break;
                default:
                    break;
                }
            }
            if (newCacheIndex >= 0 && newCacheIndex < m_cacheList.size())
            {
                CacheData* pNewCacheData = m_cacheList[newCacheIndex];
                if (pNewCacheData)
                {
                    scale = pNewCacheData->scale;
                    pageBox = pNewCacheData->pageBox;
                    modeController = pNewCacheData->mode;
                    pTextElementsInPage = &pNewCacheData->textElementsInPage;
                    //iCurPos = modeController.m_uCurPageStartPos;
//                    DebugPrintf(DLC_TRACE, "newCacheIndex = %d, scale = %f", newCacheIndex, scale);
                }
            }
        }
//        DebugPrintf(DLC_TRACE, "m_uCurPageStartPos = %d", m_stModeController.m_uCurPageStartPos);
//        DebugPrintf(DLC_TRACE, "iCurPos = %d, bottomOffset = %d, offset = %d", iCurPos, bottomOffset, offset);
        if (pTextElementsInPage && (pTextElementsInPage->size() > 0))
        {
            DK_BOX cuttingPageBox = GetCuttingPageBox(uCurPage, modeController, pageBox, scale);
//            DebugPrintf(DLC_TRACE, " pageBox        =[%f, %f, %f, %f]", pageBox.X0, pageBox.Y0, pageBox.X1, pageBox.Y1);
//            DebugPrintf(DLC_TRACE, " cuttingPageBox =[%f, %f, %f, %f]", cuttingPageBox.X0, cuttingPageBox.Y0, cuttingPageBox.X1, cuttingPageBox.Y1);

            // 获取在原始页面尺寸中，被裁边后的区域，只有与此区域相交的页面元素才会被加入到光标拾取中
            DKP_DRECT cuttingPageRect = GetCuttingPageRect(cuttingPageBox, pageBox, iCurPos, bottomOffset, scale);
//            DebugPrintf(DLC_TRACE, " %d : cuttingPageRect =[%f, %f, %f, %f]", uCurPage, cuttingPageRect.left, cuttingPageRect.top, cuttingPageRect.right, cuttingPageRect.bottom);

            // 从PDFLib中获取的图片尺寸会小于BookReader设置的ScreenRes，最终到屏幕上会被强制居中，所以光标位置也应做相应的偏移实现"屏幕居中"
            double xScreenOffset = 0, yScreenOffset = 0;
            GetScreenOffset(modeController, cuttingPageBox, xScreenOffset, yScreenOffset);

            for (size_t i = 0; i < pTextElementsInPage->size(); i++)
            {
                PDFTextElement curElement = pTextElementsInPage->at(i);
                DKP_DRECT rcBox = {curElement.m_box.X0, curElement.m_box.Y0, curElement.m_box.X1, curElement.m_box.Y1};
                const DK_FLOWPOSITION& flowposition = curElement.m_pos;
                if (DK_SUCCEEDED(IsIntersectDRect(&cuttingPageRect, &rcBox)))
                {
                    // 计算出显示在屏幕上的矩形
                    ConvertPdfRectToDocRect(pageBox, cuttingPageBox, scale, rcBox);
                    ConvertDocRectToViewRect(modeController, xScreenOffset, yScreenOffset, rcBox);
                    ConvertViewRect(offset, rcBox);
                    
                    if (!isStartPosFound)
                    {
                        m_startPos = flowposition;
                        isStartPosFound = true;
                    }

                    // 添加页面文本元素
                    curElement.m_box = DK_BOX(rcBox.left, rcBox.top, rcBox.right, rcBox.bottom);
                    curElement.m_xPos = rcBox.left;
                    curElement.m_yPos = rcBox.top;
                    m_textElmHandler.AddTextElement(curElement);
                }
                m_endPos = flowposition;
            }
        }
        uCurPage++;
        iCurPos = 0;
        offset = 0 - usedLen;
    }
    m_uCurPageEndNum = uCurPage;
    m_endPos.nChapterIndex = uCurPage;
}

bool PdfKernelHandle::GetScreenOffset(const PdfModeController& modeController, const DK_BOX& pageBox, double& xScreenOffset, double& yScreenOffset)
{
    xScreenOffset = yScreenOffset = 0;
    if (modeController.m_eReadingMode == PDF_RM_ZoomPage)
    {
        return true;
    }
    DK_DOUBLE pageWidth = pageBox.Width();
    DK_DOUBLE pageHeight = pageBox.Height();
    bool adaptiveMode = (modeController.m_eReadingMode == PDF_RM_AdaptiveWidth);
    bool rotationVertical = modeController.m_iRotation % 180;
    if (rotationVertical)
    {
        swap(pageWidth, pageHeight);
    }
    const DK_DOUBLE& screenWidth = (DK_DOUBLE)m_uBookSetScreenWidth;
    const DK_DOUBLE& screenHeight = (DK_DOUBLE)m_uBookSetScreenHeight;

    if (!DkFloatEqual(pageWidth, screenWidth) && (pageWidth < screenWidth) && !(adaptiveMode && rotationVertical))
    {
        xScreenOffset = (screenWidth - pageWidth) / 2;
    }

    if (!DkFloatEqual(pageHeight, screenHeight) && (pageHeight < screenHeight) && !(adaptiveMode && !rotationVertical))
    {
        yScreenOffset = (screenHeight - pageHeight) / 2;
    }
    return true;
}

void PdfKernelHandle::ValidEdge(unsigned int pageNum, const PdfModeController& modeController, double& left, double& top, double& right, double& bottom, bool forCuttingPageBox)
{
    if (PDF_CEM_Custom == modeController.m_eCuttingEdgeMode)
    {
        double leftEdge = modeController.m_dLeftEdge;
        double rightEdge = modeController.m_dRightEdge;
        const double& topEdge = modeController.m_dTopEdge;
        const double& bottomEdge = modeController.m_dBottomEdge;
        if (!modeController.m_bIsNormalCutting && ((pageNum % 2) > 0))
        {
            swap(leftEdge, rightEdge);
        }

        left = leftEdge;
        top = topEdge;
        right = rightEdge;
        bottom = bottomEdge;
        if (!forCuttingPageBox)
        {
            switch (modeController.m_iRotation)
            {
            case 90:
                {
                    left = topEdge;
                    top = rightEdge;
                    right = bottomEdge;
                    bottom = leftEdge;
                }
                break;
            case 180:
                {
                    left = rightEdge;
                    right = leftEdge;
                    top = bottomEdge;
                    bottom = topEdge;
                }
                break;
            case 270:
                {
                    left = bottomEdge;
                    bottom = rightEdge;
                    right = topEdge;
                    top = leftEdge;
                }
                break;
            default:
                break;
            }
        }
    }
    else if (PDF_CEM_Smart == modeController.m_eCuttingEdgeMode)
    {
        left = modeController.m_dLeftEdge;
        right = modeController.m_dRightEdge;
        top = modeController.m_dTopEdge;
        bottom = modeController.m_dBottomEdge;
        if (forCuttingPageBox)
        {
            switch (modeController.m_iRotation)
            {
            case 90:
                {
                    left = bottom;
                    bottom = right;
                    right = top;
                    top = modeController.m_dLeftEdge;;
                }
                break;
            case 180:
                {
                    left = right;
                    right = modeController.m_dLeftEdge;
                    top = bottom;;
                    bottom = modeController.m_dTopEdge;
                }
                break;
            case 270:
                {
                    left = top;
                    top = right;
                    right = bottom;
                    bottom = modeController.m_dLeftEdge;
                }
                break;
            default:
                break;
            }
        }
    }
}

DK_BOX PdfKernelHandle::GetCuttingPageBox(unsigned int pageNum, const PdfModeController& modeController, const DK_BOX& pageBox, double scale)
{
    DK_BOX cuttingPageBox;
    if (modeController.m_eReadingMode == PDF_RM_ZoomPage)
    {
        double zoomX0 = 0, zoomY0 = 0;
        GetZoomedAttrbutes(zoomX0, zoomY0);
        cuttingPageBox.X0 = zoomX0;
        cuttingPageBox.X1 = cuttingPageBox.X0 + m_uScreenWidth;
        cuttingPageBox.Y0 = zoomY0;
        cuttingPageBox.Y1 = cuttingPageBox.Y0 + m_uScreenHeight;
    }
    else
    {
        double modeLeftEdge = 0, modeRightEdge = 0, modeTopEdge = 0, modeBottomEdge = 0;
        ValidEdge(pageNum, modeController, modeLeftEdge, modeTopEdge, modeRightEdge, modeBottomEdge, true);

        double renderWidth = pageBox.Width() * scale;  // 传入PDFLib的渲染宽高
        double renderHeight = pageBox.Height() * scale;

        cuttingPageBox = DK_BOX(renderWidth * modeLeftEdge,                 // 渲染结果的最终显示区域（如被裁边）
            renderHeight * modeTopEdge,
            renderWidth * (1 - modeRightEdge),
            renderHeight * (1 - modeBottomEdge));
    }
    return cuttingPageBox;
}

void PdfKernelHandle::MakeCursorTable(unsigned int cacheIndex)
{
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::MakeCursorTable ENTRANCE cacheIndex %d", cacheIndex);
    assert(m_pDoc);
    if (!m_pDoc || cacheIndex >= m_cacheList.size())
    {
        DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::MakeCursorTable return");
        return;
    }

    m_pBookTextController->Release();

    m_textElmHandler.ClearAll();

    // 该函数只处理原版式页面的光标
    if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode)
    {
        DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::MakeCursorTable return with PDF_RM_Rearrange mode");
        return;
    }

    m_textElmHandler.SetModeController(m_stModeController);

    if (PDF_RM_NormalPage == m_stModeController.m_eReadingMode
        || PDF_RM_AdaptiveWidth == m_stModeController.m_eReadingMode
        || PDF_RM_ZoomPage == m_stModeController.m_eReadingMode)
    {
        AddTextElement(cacheIndex);
    }

    m_textElmHandler.EndPage(m_endPos);

    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::MakeCursorTable CurPage startPos(%d, %d, %d), endPos(%d, %d, %d)",
                            m_startPos.nChapterIndex, m_startPos.nParaIndex, m_startPos.nElemIndex,
                            m_endPos.nChapterIndex, m_endPos.nParaIndex, m_endPos.nElemIndex);

    if (!m_textElmHandler.IsEmpty())
    {    
        m_pBookTextController->Initialize<IDKPPageTraits>(NULL, this);
        m_pBookTextController->SetModeController(GetModeController());
    }

    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::MakeCursorTable SUCCEEDED");
}

wchar_t* PdfKernelHandle::GetTextContentOfRange(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos)
{
    std::wstring us;
    if (m_pDoc)
    {
        DK_FLOWPOSITION newStartPos = startPos;
        DK_FLOWPOSITION newEndPos = endPos;
        if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode)
        {
            newStartPos.nChapterIndex++;
            newEndPos.nChapterIndex++;
        }
        for (DK_UINT curChapter = newStartPos.nChapterIndex; curChapter <= newEndPos.nChapterIndex; ++curChapter)
        {
            AutoLock lock(&m_pdfLibLock);
            IDKPPage* pPage = m_pDoc->GetPage(curChapter);
            if (pPage)
            {
                int codeIndex = 0;
                PDKPTEXTINFONODE pStreamHead(DK_NULL);
                if (pPage->GetPageTextContentStream(pStreamHead))
                {
                    while (pStreamHead)
                    {
                        const DKPTEXTINFO& node = pStreamHead->Node;
                        PDKPDISPLAYCHARNODE pNext = node.pCodeContent;
                        const DK_FLOWPOSITION& curPos = pStreamHead->pos;
                        pStreamHead = pStreamHead->pNext;
                        if ((curPos.nChapterIndex < newStartPos.nChapterIndex)
                            || (curPos.nChapterIndex == newStartPos.nChapterIndex && curPos.nParaIndex < newStartPos.nParaIndex))
                        {
                            continue;
                        }
                        if ((curPos.nChapterIndex > newEndPos.nChapterIndex)
                            || (curPos.nChapterIndex == newEndPos.nChapterIndex && curPos.nParaIndex > newEndPos.nParaIndex))
                        {
                            break;
                        }
                        codeIndex = 0;
                        while (pNext)
                        {
                            const DKPDISPLAYCHAR& charNode = pNext->charNode;
                            DK_FLOWPOSITION flowposition(curPos.nChapterIndex, curPos.nParaIndex, codeIndex);
                            if (flowposition >= newStartPos && flowposition < newEndPos)
                            {
                                us += (DK_WCHAR)charNode.uUnicode[0];
                            }
                            ++codeIndex;
                            pNext = pNext->pNext;
                        }
                    }
                }
                pPage->FreePageContentStream();
                m_pDoc->ReleasePage(pPage, true);
            }
        }
    }

    wchar_t* pText = DK_MALLOCZ_OBJ_N(wchar_t, us.size() + 1);
    if (NULL == pText)
    {
        return NULL;
    }

    wcsncpy(pText, us.c_str(), us.size() + 1);

    return pText;
}

void PdfKernelHandle::ValidTextRects(DK_BOX** ppTextRects, unsigned int* pRectCount)
{
    if (NULL == pRectCount || *pRectCount <= 0
        || NULL == ppTextRects || NULL == *ppTextRects)
    {
        return ;
    }

    if (IsZoomedPdf())
    {
        double zoomX0 = 0, zoomY0 = 0;
        if (GetZoomedAttrbutes(zoomX0, zoomY0))
        {
            for (unsigned int i = 0; i < *pRectCount; ++i)
            {
                (*ppTextRects)[i].X0 -= zoomX0;
                (*ppTextRects)[i].Y0 -= zoomY0;
                (*ppTextRects)[i].X1 -= zoomX0;
                (*ppTextRects)[i].Y1 -= zoomY0;
            }
        }
    }
}

DK_ReturnCode PdfKernelHandle::GetTextRects(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DK_BOX** ppTextRects, unsigned int* pRectCount)
{
    DK_ReturnCode result = m_textElmHandler.GetTextRects(startPos, endPos, ppTextRects, pRectCount);
    if (DK_SUCCEEDED(result))
    {
        ValidTextRects(ppTextRects, pRectCount);
    }
    return result;
}

void PdfKernelHandle::FreeRects(DK_BOX* pRects)
{
    m_textElmHandler.FreeRects(pRects);
}

PDFTextIterator* PdfKernelHandle::GetTextIterator()
{
    return m_textElmHandler.GetTextIterator();
}

void PdfKernelHandle::FreeTextIterator(PDFTextIterator* pPDFTextIterator)
{
    m_textElmHandler.FreeTextIterator(pPDFTextIterator);
}

DK_ReturnCode PdfKernelHandle::HitTestTextRange(const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
{
    DK_POS transfromPoint = ConvertViewPointToDocPoint(m_stModeController, point);
    return m_textElmHandler.HitTestTextRange(transfromPoint, pStartPos, pEndPos);
}

bool PdfKernelHandle::GetPageStartPos (DK_FLOWPOSITION* pPos)
{
    // REMARK: 仅供在使用原始页面光标表时做边界判断用
    *pPos = m_startPos;

	//返回给应用的nChapterIndex应该是内核-1后的结果
	if(pPos && pPos->nChapterIndex > 0)
	{
		pPos->nChapterIndex--;
	}
    return true;
}

bool PdfKernelHandle::GetPageEndPos (DK_FLOWPOSITION* pPos)
{
    // REMARK: 仅供在使用原始页面光标表时做边界判断用
    *pPos = m_endPos;

	//返回给应用的nChapterIndex应该是内核-1后的结果
	if(pPos && pPos->nChapterIndex > 0)
	{
		pPos->nChapterIndex--;
	}
    return true;
}

bool PdfKernelHandle::SearchInCurrentBook(const string& strText, const DK_FLOWPOSITION& startPos, const int& iMaxResultCount, SEARCH_RESULT_DATA* _pSearchResultData, unsigned int* puResultCount)
{
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::SearchInCurrentBook ENTRANCE");
    if (strText.empty() || startPos.nChapterIndex >= m_uPageCount || iMaxResultCount <= 0 || !_pSearchResultData || !_pSearchResultData->pSearchResultTable || !puResultCount)
    {
        DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::SearchInCurrentBook INVALID_ARG");
        return false;
    }
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::SearchInCurrentBook startPos(%d, %d, %d)", startPos.nChapterIndex, startPos.nParaIndex, startPos.nElemIndex);

    DK_WCHAR* pStrText = EncodingUtil::CharToWChar(strText.c_str());
    if (!pStrText)
    {
        return false;
    }

    AutoLock lock(&m_pdfLibLock);

    DK_FLOWPOSITION actualStartPos(startPos);
    ++actualStartPos.nChapterIndex; // 应用层出参和入参的nChapterIndex从0开始，内核则是从1开始
    DKP_SEARCHOPTION option;
    option.szPattern = pStrText;
    option.bCaseSensitive = DK_FALSE;
    option.bWholeWord = DK_FALSE;
    option.bMultiMatch = DK_FALSE;
    option.nContextLen = 30;

    unsigned int resultCount(0);
    unsigned int curPage(actualStartPos.nChapterIndex);
    while (curPage <= m_uPageCount && resultCount < (unsigned int)iMaxResultCount)
    {
        IDKPPage* pPage = m_pDoc->GetPage(curPage);

        DKP_SEARCHRESULT* pResults(DK_NULL);
        DK_UINT matchCount(0);
        
        if (DKP_FIND_SUCCESS == pPage->FindTextInPage(option, pResults, matchCount))
        {
            for (unsigned int i(0); i < matchCount; ++i)
            {
                if (pResults[i].matchStartFlowPos < actualStartPos)
                {
                    continue;
                }

                _pSearchResultData->pSearchResultTable[resultCount] = new CT_ReadingDataItemImpl(ICT_ReadingDataItem::BOOKMARK);
		        if(!_pSearchResultData->pSearchResultTable[resultCount])
		        {
			        break;
		        }

                DK_CHAR* pText = EncodingUtil::WCharToChar(pResults[i].szText);
                if (!pText)
                {
                    continue;
                }
                string text = pText;
		        SafeFreePointer(pText);

                // 记录位置
                // 应用层出参和入参的nChapterIndex从0开始，内核则是从1开始
                _pSearchResultData->pSearchResultTable[resultCount]->SetBeginPos(CT_RefPos(pResults[i].matchStartFlowPos.nChapterIndex - 1,
                                                                                           pResults[i].matchStartFlowPos.nParaIndex,
                                                                                           pResults[i].matchStartFlowPos.nElemIndex,
                                                                                           -1));
                _pSearchResultData->pSearchResultTable[resultCount]->SetEndPos(CT_RefPos(pResults[i].matchEndFlowPos.nChapterIndex - 1,
                                                                                         pResults[i].matchEndFlowPos.nParaIndex, 
                                                                                         pResults[i].matchEndFlowPos.nElemIndex,
                                                                                         -1));

                _pSearchResultData->pSearchResultTable[resultCount]->SetBookContent(text);
                _pSearchResultData->pHighlightStartPosTable[resultCount] = pResults[i].nMatchBegin;
                _pSearchResultData->pHighlightEndPosTable[resultCount] = pResults[i].nMatchBegin + pResults[i].nMatchLength;
                _pSearchResultData->pLocationTable[resultCount] = (int)(100 * curPage / m_uPageCount);
                
                DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::SearchInCurrentBook match text %s, startpos(%d, %d, %d), endpos(%d, %d, %d), match begin %d, match end %d", text.c_str(), 
                    pResults[i].matchStartFlowPos.nChapterIndex - 1, pResults[i].matchStartFlowPos.nParaIndex, pResults[i].matchStartFlowPos.nElemIndex,
                    pResults[i].matchEndFlowPos.nChapterIndex - 1, pResults[i].matchEndFlowPos.nParaIndex, pResults[i].matchEndFlowPos.nElemIndex,
                    _pSearchResultData->pHighlightStartPosTable[resultCount], _pSearchResultData->pHighlightEndPosTable[resultCount]);

                ++resultCount;
                if (resultCount >= (unsigned int)iMaxResultCount)
                {
                    break;
                }
            }
        }
        DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::SearchInCurrentBook Searched page %d, match count %d", curPage, matchCount);

        m_pDoc->ReleasePage(pPage, true);

        ++curPage;
    }
    
    *puResultCount = resultCount;

    SafeFreePointer(pStrText);
    DebugPrintf (DLC_LIUHJ, "PdfKernelHandle::SearchInCurrentBook FINISH");
    return true;
}

bool PdfKernelHandle::IsZoomedPdf() const
{ 
    return (m_stModeController.m_eReadingMode == PDF_RM_ZoomPage) && (m_stModeController.m_dUserZoom > 1.0); 
}

const DK_TOCINFO* PdfKernelHandle::GetChapterInfo(const DK_FLOWPOSITION& posPage, CHAPTER_POSITION_MODE positionMode)
{
	if(!m_pDoc)
    {
        return NULL;
    }
    GetTOC();
    
    AutoLock lock(&m_pdfLibLock);

	int iChapterIndex = m_vTOCList.size() - 1;
	for(size_t i = 1; i < m_vTOCList.size(); i++)
	{
		DK_TOCINFO* pTocInfo = m_vTOCList.at(i);
		if(posPage < pTocInfo->GetBeginPos())
		{
			iChapterIndex = i - 1;
			break;
		}
	}
    
	iChapterIndex += (positionMode - CURRENT_CHAPTER);
	if(iChapterIndex >= 0 && iChapterIndex < (int)m_vTOCList.size())
	{
		DK_TOCINFO* pChapterTocInfo = m_vTOCList.at(iChapterIndex);
		if(pChapterTocInfo)
		{
			DK_FLOWPOSITION posChapterStart = pChapterTocInfo->GetBeginPos();
			//内核索引要+1
			posChapterStart.nChapterIndex++;
			
			//因为是只能重排，所以实际排出来的章节起始处的DK_FLOWPOSITION 并不一定等于posChapterStart(chapterIndex + 1, 0, 0)
		    if (PDF_RM_Rearrange == m_stModeController.m_eReadingMode)
		    {
		        IDKPPage* pNewPage = NULL;
		        IDKPPageEx* pPageEx = GetRenderPageEx(posChapterStart, IDKPDoc::LOCATION_PAGE, m_rearrangeOption, &pNewPage);
		        if(pPageEx)
		        {
		            pPageEx->GetPageStartPos(&posChapterStart);
		            m_pDoc->ReleasePageEx(pPageEx, true);
		            pPageEx = NULL;
		        }
		        if(pNewPage)
		        {
		            m_pDoc->ReleasePage(pNewPage, true);
		            pNewPage = NULL;
		        }
		    }
			//传回是应该-1
    		posChapterStart.nChapterIndex -= 1;
			pChapterTocInfo->SetBeginPos(posChapterStart);
            return pChapterTocInfo;
		}
	}
    return NULL;
}

void PdfKernelHandle::ParseSinglePage(bool parseSingle)
{
    m_parseSingle = parseSingle;
}

bool PdfKernelHandle::GetSelectionRange(const DK_POS& startPos, const DK_POS& endPos, DKP_SELECTION_MODE selectionMode, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
{
    DK_POS posStart = ConvertViewPointToDocPoint(m_stModeController, startPos);
    DK_POS posEnd = ConvertViewPointToDocPoint(m_stModeController, endPos);
    return DK_SUCCEEDED(m_textElmHandler.GetSelectionRange(posStart, posEnd, selectionMode, pStartPos, pEndPos));
}

DK_POS PdfKernelHandle::ScreenCoord2ZoomedPdfCoord(const DK_POS& screenCoodinate) const
{
    double zoomX0 = 0, zoomY0 = 0;
    if (GetZoomedAttrbutes(zoomX0, zoomY0))
    {
        return DK_POS(zoomX0 + screenCoodinate.X, zoomY0 + screenCoodinate.Y);
    }
    return DK_POS(-1,-1);
}

DK_POS PdfKernelHandle::ZoomedPdfCoord2ScreenCoord(const DK_POS& originalCoodinate) const
{
    double zoomX0 = 0, zoomY0 = 0;
    if (GetZoomedAttrbutes(zoomX0, zoomY0))
    {
        return DK_POS(originalCoodinate.X - zoomX0, originalCoodinate.Y - zoomY0);
    }
    return DK_POS(-1,-1);
}

void PdfKernelHandle::ValidZoomedXY(double& zoomX0, double& zoomY0) const
{
    const double maxZoomX0 = (double)(m_uPageWidth - m_uScreenWidth);
    const double maxZoomY0 = (double)(m_uPageHeight - m_uScreenHeight);
    zoomX0 = dk_max(0.0, zoomX0); 
    zoomY0 = dk_max(0.0, zoomY0); 
    zoomX0 = dk_min(zoomX0, maxZoomX0); 
    zoomY0 = dk_min(zoomY0, maxZoomY0); 
}

bool PdfKernelHandle::GetZoomedAttrbutes(double& zoomX0, double& zoomY0) const
{
    if (IsZoomedPdf())
    {
        zoomX0 = m_stModeController.m_zoomX0 - m_stModeController.m_imageX;
        zoomY0 = m_stModeController.m_zoomY0 - m_stModeController.m_imageY;
        ValidZoomedXY(zoomX0, zoomY0);
        return true;
    }
    return false;
}
