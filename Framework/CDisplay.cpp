////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/core/src/Framework/CDisplay.cpp $
// $Revision: 12 $
// Contact: yuzhan
// Latest submission: $Date: 2008/08/01 13:57:50 $ by $Author: fareast\\xzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include <sys/stat.h>
#include <unistd.h>
#include "Utility/BasicType.h"
#include "Framework/CDisplay.h"
#include "Framework/CRunnableDepot.h"
#include "Utility/CImageLoader.h"
#include "Utility/TimerThread.h"
#include "Utility/SystemUtil.h"
#include "GUI/CTpGraphics.h"
#include "GUI/GUISystem.h"
#include "GUI/FontManager.h"
#include "I18n/StringManager.h"
#include "CommonUI/CPageNavigator.h"
#include "Common/ConfigInfo.h"
#include "Common/WindowsMetrics.h"
#include "GUI/ImageFullRepaintCalculation.h"
#include "../ImageHandler/DkImageHelper.h"
#include "StopWatch.h"
#include "ValueScope.h"


#define PRINTSCREENPATH "/mnt/us/DK_ScreenShots/"

using DkFormat::StopWatch;
using DkFormat::ImageHandler;
using DkFormat::DkImageHelper;
using dk::utility::TimerThread;
using dk::utility::SystemUtil;
using namespace WindowsMetrics;

CDisplay CDisplay::s_Instance;

CDisplay::CDisplay() 
    : m_MessageDispatcher(*this)
    , m_cacheEnabled(true)
    , m_inEndCache(false)
    , m_inRepaint(false)
    , m_paintTimes(0)
    , m_stopFullPaintOnce(0)
    , m_fullRepaint(false)
{
    m_spCurrentPage = NULL;
    memset(&m_memDC, 0, sizeof(DK_IMAGE));
    memset(&m_memSecDC, 0, sizeof(DK_IMAGE));
    m_rectUndraw.MakeEmpty();
    m_rectUnflush.MakeEmpty();
    m_cachePaintFlag = PAINT_FLAG_NONE;
}

CDisplay::~CDisplay()
{
    m_spCurrentPage = NULL;

    if (m_memDC.pbData != NULL)
    {
        delete[] m_memDC.pbData;
        m_memDC.pbData = NULL;
    }
    if (m_memSecDC.pbData != NULL)
    {
        delete[] m_memSecDC.pbData;
        m_memSecDC.pbData = NULL;
    }
}

void CDisplay::StopFullRepaintOnce()
{
    m_stopFullPaintOnce = 1;
}

void CDisplay::SetCurrentPage(UIPage* spPage)
{
    m_spCurrentPage = spPage;
}

UIPage* CDisplay::GetCurrentPage()
{
    return  CPageNavigator::GetCurrentPage();
}

INT32 CDisplay::GetScreenWidth()
{
    return INativeDisplay::GetScreenWidth();
}

INT32 CDisplay::GetScreenHeight()
{
    return INativeDisplay::GetScreenHeight();
}

void CDisplay::SynchronousCall(IRunnable& rRunnable)
{
    SNativeMessage synchronousMsg;
    synchronousMsg.iType = KMessageSynchronousCall;
    synchronousMsg.iParam1 = (INT32)&rRunnable;
    INativeMessageQueue::GetMessageQueue()->Send(synchronousMsg);
}

void CDisplay::SynchronizedCall(SPtr<IRunnable> spRunnable)
{
    SNativeMessage synchronousMsg;
    synchronousMsg.iType = KMessageSynchronizedCall;
    INT32 id = CRunnableDepot::RegisterRunnable(spRunnable);
    synchronousMsg.iParam1 = id;
    INativeMessageQueue::GetMessageQueue()->Send(synchronousMsg);
}

void CDisplay::Repaint(paintflag_t flag)
{
    DebugPrintf(DLC_EINK_PAINT, "CDisplay::Repaint(%d)", flag);
    SNativeMessage repaintMsg;
    if (MakePaintMessage(0, 0, GetScreenWidth(), GetScreenHeight(), flag | PAINT_FLAG_REPAINT, &repaintMsg))
        INativeMessageQueue::GetMessageQueue()->Send(repaintMsg);
}
 

CDisplay::CInnerMessageDispatcher::CInnerMessageDispatcher(CDisplay& rParent) :
    m_Parent(rParent)
{
}

void CDisplay::CInnerMessageDispatcher::OnKeyEvent(INT32 keyCode, EKeyboardEventType eventType)
{
    UIPage* spCurrentPage = CPageNavigator::GetCurrentPage();
    if (!spCurrentPage)
    {
        return;
    }

    switch (eventType)
    {
        case DK_KEYTYPE_REPEAT:
        case DK_KEYTYPE_DOWN:
        {
            clock_t start = clock();
            if(spCurrentPage->OnKeyPress(keyCode) == TRUE )
            {
                if(KEY_BACK == keyCode)
                {
                    CPageNavigator::BackToPrePage();
                }
                else if(KEY_HOME == keyCode)
                {
                    CPageNavigator::BackToHome();
                }
            }
            DebugPrintf(DLC_TIME, "spCurrentPage->OnKeyPress(keycode=%d, eventType=%d) elapsed %d ms.",
                keyCode, eventType, 1000*(clock()-start)/CLOCKS_PER_SEC);
        }
            break;
        case DK_KEYTYPE_UP:
            {
                spCurrentPage->OnKeyRelease(keyCode);
            }
            break;
    }

    TRACE("End CDisplay On Key Event");
    return;
}

void CDisplay::CInnerMessageDispatcher::OnPointerPressed(INT32 , INT32 )
{
    // not used in current implementation
    UIPage* spCurrentPage = CPageNavigator::GetCurrentPage();

    if (!spCurrentPage)
    {
        return;
    }
}

void CDisplay::CInnerMessageDispatcher::OnPointerDragged(INT32 , INT32 )
{
    // not used in current implementation
     UIPage* spCurrentPage = CPageNavigator::GetCurrentPage();

    if (!spCurrentPage)
    {
        return;
    }
}

void CDisplay::CInnerMessageDispatcher::OnPointerReleased(INT32 , INT32 )
{
    // not used in current implementation
     UIPage* spCurrentPage = CPageNavigator::GetCurrentPage();

    if (!spCurrentPage)
    {
        return;
    }
}

void CDisplay::CInnerMessageDispatcher::OnPaint(DK_IMAGE g)
{
    DebugLog(DLC_EINK_PAINT, "CDisplay::CInnerMessageDispatcher::OnPaint()");
    GUISystem* pGUI = GUISystem::GetInstance();
    if (pGUI)
    {
        UIContainer* pContainer = pGUI->GetTopFullScreenContainer();
        if (pContainer)
        {
            pContainer->Draw(g);
            return;
        }
    }

    UIPage* spCurrentPage = CPageNavigator::GetCurrentPage();
    if (spCurrentPage)
    {
        spCurrentPage->Draw(g);
    }
}


//inline methods

CDisplay* CDisplay::GetDisplay()
{
    return &s_Instance;
}

/*
CFbBitDc *CDisplay::GetScreenDC()
{
    return INativeDisplay::GetScreenGC();
}
*/

void CDisplay::SetScreenDC(CFbBitDc *pScreenDC)
{
    DebugLog(DLC_EINK_PAINT, "CDisplay::SetScreenDC()");

    if (pScreenDC)
    {
        INativeDisplay::SetScreenDC(pScreenDC);
        InitMemDC();
    }

}


void CDisplay::InitMemDC()
{
    m_memDC.iWidth = INativeDisplay::GetScreenWidth();
    m_memDC.iHeight = INativeDisplay::GetScreenHeight();
    m_memDC.iColorChannels = 1;
    m_memDC.iStrideWidth = m_memDC.iWidth * m_memDC.iColorChannels;

    if (m_memDC.pbData != NULL)
    {
        delete [] m_memDC.pbData;
    }

    // TODO: 为什么这里还要有一层呢 ？
    m_memDC.pbData = new BYTE8[m_memDC.iStrideWidth * m_memDC.iHeight];
    memset(m_memDC.pbData, 0, m_memDC.iStrideWidth * m_memDC.iHeight);

    m_memSecDC.iWidth = INativeDisplay::GetScreenWidth();
    m_memSecDC.iHeight = INativeDisplay::GetScreenHeight();
    m_memSecDC.iColorChannels = 1;
    m_memSecDC.iStrideWidth = m_memSecDC.iWidth * m_memSecDC.iColorChannels;

    if (m_memSecDC.pbData != NULL)
    {
        delete [] m_memSecDC.pbData;
    }

    // TODO: 为什么这里还要有一层呢 ？
    m_memSecDC.pbData = new BYTE8[m_memSecDC.iStrideWidth * m_memSecDC.iHeight];
    memset(m_memSecDC.pbData, 0, m_memSecDC.iStrideWidth * m_memSecDC.iHeight);
}

ISystemMessageListener& CDisplay::GetMessageDispatcher()
{
    return m_MessageDispatcher;
}


void CDisplay::ScreenRepaint(paintflag_t paintFlag, bool drawImmediately)
{
    ScreenRepaint(0, 0, GetScreenWidth(), GetScreenHeight(), paintFlag, drawImmediately);
}


void CDisplay::ScreenRepaint(int x,int y,int w,int h, paintflag_t paintFlag, bool drawImmediately)
{
    if (m_inRepaint)
        return;

    DkRect dstRect = DkRect::FromLeftTopWidthHeight(x, y, w, h);
    DebugLog(DLC_EINK_PAINT, "CDisplay::ScreenRepaint(RECT)");

    //DebugPrintf(DLC_DIAGNOSTIC, "CDisplay::ScreenRepaint(%d, %d, %d, %d, 0x%08X)", x, y, w, h, paintFlag);
    
    INativeMessageQueue* pMessageQueue = INativeMessageQueue::GetMessageQueue();

    if (pMessageQueue != NULL && m_cacheEnabled && !drawImmediately)
    {
        SNativeMessage msg;
        if (MakePaintMessage(dstRect, paintFlag, &msg))
            pMessageQueue->Send(msg);
    }
    else
    {
        DoCache(dstRect, paintFlag);
        EndCache(true);
    }
}


//cache to paint
void CDisplay::DoRepaint(int x,int y,int w,int h,int srcx,int srcy, paintflag_t paintFlag, bool flushToFrameBuffer)
{
    StopWatch watch(StopWatch::INIT_START);
    DebugLog(DLC_EINK_PAINT, "CDisplay::DoRepaint(RECT)");

    if (paintFlag & PAINT_FLAG_REPAINT)
    {
        ValueScope<bool> inRepaint(m_inRepaint, true);
        GetMessageDispatcher().OnPaint(m_memDC);
        bool fullRepaint = (m_stopFullPaintOnce == 0) && m_fullRepaint;
        //DebugPrintf(DLC_DIAGNOSTIC, "CDisplay::DoRepaint fullRepaint = %d", fullRepaint);
        if (m_stopFullPaintOnce  > 0)
        {
            --m_stopFullPaintOnce;
        }
        else
        {
            ++m_paintTimes;
        }

        if (fullRepaint || atoi(Config::GetConfigValueString(CKI_RepaintPage)) <= m_paintTimes) 
        {
            m_fullRepaint = false;
            paintFlag |= PAINT_FLAG_FULL;
            m_paintTimes = 0;
            ImageFullRepaintCalculation* pFullRepaint = ImageFullRepaintCalculation::GetInstance();
            if (pFullRepaint)
            {
                pFullRepaint->ResetFullRepaintFlag();
            }
        }
    }
    if (flushToFrameBuffer)
    {
        CTpGraphics grf(m_memDC);
        if (!m_tips.empty())
        {
            DKFontAttributes fontattr;
            fontattr.SetFontAttributes(0,0,GetWindowFontSize(FontSize18Index));
            ITpFont* pFont = FontManager::GetInstance()->GetFont(fontattr, ColorManager::knWhite);
            if (NULL != pFont)
            {
                const int height = pFont->GetHeight();
                const int width = pFont->StringWidth(m_tips.c_str());
                int hPadding = GetWindowMetrics(UIPixelValue10Index);
                int vPadding = GetWindowMetrics(UIElementSpacingIndex);
                const int left = (GetScreenWidth() - width) >> 1;
                const int top = GetScreenHeight() - GetWindowMetrics(UIPixelValue100Index);
                //DebugPrintf(DLC_DIAGNOSTIC, "CDisplay Draw Tips, left: %d, top: %d, hPadding: %d, vPadding: %d, width: %d, height: %d", left, top, hPadding, vPadding, width, height);
                grf.FillRect(left - hPadding, top - vPadding, left + width + hPadding, top + height + vPadding,ColorManager::knBlack);
                grf.DrawStringUtf8(m_tips.c_str(), left, top, pFont);
            }
        }
        CFbBitDc *pScreen = INativeDisplay::GetScreenGC();
        pScreen->BitBlt(x, y, w, h, m_memDC, srcx, srcy, paintFlag);
    }
    //DebugPrintf(DLC_DIAGNOSTIC, "DoRepaint(%d, %d)-(%d, %d)  flag: %08X, take %d ms", x, y, w, h, paintFlag, watch.Stop());
}


void CDisplay::ScreenShowImageRepaint(int x,int y,int w,int h, ITpImage *pImg, int srcx,int srcy)
{
    DebugLog(DLC_EINK_PAINT, "CDisplay::ScreenRepaint(RECT)");

    CTpGraphics grf(m_memSecDC);
    CTpGraphics grfsrc(m_memDC);

    grf.BitBlt(x, y, w, h, &grfsrc, x, y);
    if(pImg)
        grf.DrawImageBlend(pImg, x, y, srcx, srcy, w, h);

    CFbBitDc *pScreen = INativeDisplay::GetScreenGC();
    pScreen->BitBlt(x, y, w, h, m_memSecDC, x, y, PAINT_FLAG_NONE);
}


void CDisplay::ScreenRefresh()
{
    DebugLog(DLC_EINK_PAINT, "CDisplay::ScreenRefresh()");
    EndCache(true);
    CFbBitDc *pScreen = INativeDisplay::GetScreenGC();
    pScreen->ScreenRefresh();
}


bool CDisplay::DrawFullScreenPicture(const char *pImgPath)
{
    ImageHandler *pImgHandler = DkImageHelper::CreateImageHandlerInstance(pImgPath);
    if(NULL == pImgHandler)
    {
        return false;
    }

    //DebugPrintf(DLC_DIAGNOSTIC, "CDisplay::DrawFullScreenPicture(%s)", pImgPath);
    int iHeight = pImgHandler->GetHeight();
    int iScreenHeight = GetScreenHeight();

    int iWidth = pImgHandler->GetWidth();
    int iScreenWidth = GetScreenWidth();

    double scale = (double)iScreenWidth / iWidth;
    if (scale > (double)iScreenHeight / iHeight)
    {
        scale = (double)iScreenHeight / iHeight;
    }

    int iNewWidth = (int)(scale * iWidth);
    int iNewHeight = (int)(scale * iHeight);

    DK_RECT srcRect = {0, 0, iWidth, iHeight};
    if(!(pImgHandler->CropAndResize(srcRect, iNewWidth, iNewHeight)))
    {
        delete pImgHandler;
        //DebugPrintf(DLC_DIAGNOSTIC, "CDisplay::DrawFullScreenPicture(%s) failed 1", pImgPath);
        return false;
    }

    DK_RECT rcSelf = 
    {
        (iScreenWidth - iNewWidth) >> 1,
        (iScreenHeight - iNewHeight) >> 1,
        ((iScreenWidth - iNewWidth) >> 1) + iNewWidth,
        ((iScreenHeight - iNewHeight) >> 1) + iNewHeight
    };

    EndCache(false);
    CTpGraphics grf(m_memDC);
    grf.EraserBackGround();

    DK_IMAGE DestImg;
    if(FAILED(CropImage(m_memDC, rcSelf, &DestImg)))
    {
        delete pImgHandler;
        DebugPrintf(DLC_DIAGNOSTIC, "CDisplay::DrawFullScreenPicture(%s) failed 2", pImgPath);
        return false;
    }

    if (!(pImgHandler->GetImage(&DestImg)))
    {
        delete pImgHandler;
        DebugPrintf(DLC_DIAGNOSTIC, "CDisplay::DrawFullScreenPicture(%s) failed 3", pImgPath);
        return false;
    }

    m_fullRepaint = true;
    DoRepaint(0, 0, iScreenWidth, iScreenHeight, 0, 0, PAINT_FLAG_FULL, true);
    //DebugPrintf(DLC_DIAGNOSTIC, "CDisplay::DrawFullScreenPicture(%s) succeeded", pImgPath);

    delete pImgHandler;
	return true;
}


/*void CDisplay::ShowRebootTip()
{
    CTpGraphics grf(m_memDC);
    grf.EraserBackGround();

    DKFontAttributes iFontAttribute;
    iFontAttribute.SetFontAttributes(0, 0, 37);
    INT32 iColor = ColorManager::GetColor(COLOR_SOFTKEY_TXT);
    ITpFont *pFont = FontManager::GetInstance()->GetFont(iFontAttribute, iColor);
    if(pFont == NULL)
    {
        return ;
    }
    
    int iStringWidth = pFont->StringWidth(StringManager::GetStringById(SYSTEM_REBOOTING_PLEASE_WAIT));
    int iLeft = (GetScreenWidth() - iStringWidth) / 2;
    int iTop = GetScreenHeight() / 2;
    grf.DrawString(StringManager::GetStringById(SYSTEM_REBOOTING_PLEASE_WAIT), iLeft, iTop, pFont);

    DoRepaint(0, 0, GetScreenWidth(), GetScreenHeight(), 0, 0, PAINT_FLAG_FULL, true);
}*/

///无用 api，清理
/*
void CDisplay::UpdateUI()
{
    SNativeMessage updateMsg;
    updateMsg.iType = KMessageUpdateUI;
    INativeMessageQueue::GetMessageQueue()->Send(updateMsg);
}
*/

DK_IMAGE CDisplay::GetMemDC()
{
    // if there're caches, flush to DC before paint on mem dc
    EndCache(false);

    return m_memDC;
}

INT CDisplay::GetScreenDpi() const
{
    return 160;
}

void CDisplay::ForceDraw(paintflag_t paintFlag)
{
    DebugPrintf(DLC_PENGF, "CDisplay::ForceDraw(%08X)", paintFlag);
    ForceDraw(
            DkRect::FromLeftTopWidthHeight(0, 0, GetScreenWidth(), GetScreenHeight()),
            paintFlag);
}
void CDisplay::ForceDraw(const DkRect& rect, paintflag_t paintFlag)
{
    DebugPrintf(DLC_PENGF, "CDisplay::ForceDraw(%08X), (%d, %d)-(%d, %d)", paintFlag, rect.Left(), rect.Top(), rect.Width(), rect.Height());
    StopWatch watch(StopWatch::INIT_START);
    DebugLog(DLC_EINK_PAINT, "CDisplay::ForceDraw() Entering Force draw!");
    DebugLog(DLC_GUI, "CDisplay::ForceDraw() Entering Force draw!");
    if (!m_cacheEnabled)
    {
        paintFlag &= ~PAINT_FLAG_CACHABLE;
    }
    if (paintFlag & PAINT_FLAG_CACHABLE)
    {
        DoCache(rect, paintFlag);
    }
    else
    {
        DoCache(rect, paintFlag); // take this one as cache too and end cache
        EndCache(true);
    }
    watch.Stop();
    DebugPrintf(DLC_TIME, "CDisplay::ForceDraw() Force draw done. Time elapsed=%d ms.",
            watch.DurationInMs());
}

// TOD0:内部使用
void CDisplay::PrintSrceen()
{
#if 1
    //get image name
    char ScreenPath[512] = "";
    char name[256] = "";
    time_t rawtime;
    struct tm *info = NULL;
    memset(ScreenPath,0,512);
    memset(name,0,256);
    sprintf(ScreenPath,"%s",PRINTSCREENPATH);
    if(access(ScreenPath,F_OK) == -1)
    {
        if(mkdir(ScreenPath,S_IREAD|S_IWUSR|S_IWRITE) == -1)
        {
            return;
        }
    }
    time ( &rawtime );
    info = localtime ( &rawtime );
    strftime(name,256,"20%y%m%d%H%M%S",info);
    sprintf(ScreenPath,"%s%s.bmp",PRINTSCREENPATH,name);

    // fill the bmp format header
    BITMAPFILEHEADER bmp_file_header;
    memset(&bmp_file_header,0,sizeof(BITMAPFILEHEADER));
    bmp_file_header.bfOffBits =  0x36 + 256 * 4;
    bmp_file_header.bfSize = m_memDC.iWidth * m_memDC.iHeight + 0x36 + 256 * 4;
    bmp_file_header.bfType = 0x4d42;
    BITMAPINFOHEADER bmp_info_header;
    memset(&bmp_info_header,0,sizeof(BITMAPINFOHEADER));
    bmp_info_header.biSize = sizeof(BITMAPINFOHEADER);
    bmp_info_header.biWidth = m_memDC.iWidth;
    bmp_info_header.biHeight = m_memDC.iHeight;
    bmp_info_header.biPlanes = 1;
    bmp_info_header.biBitCount = 8;
    bmp_info_header.biSizeImage = m_memDC.iWidth * m_memDC.iHeight;
    bmp_info_header.biClrUsed = 256;


    int plus[256];
    memset(plus, 0 , 256 * sizeof(int));
    for(int i=0; i < 256; i++)
    {
        memset(plus + i, 255 - i, 4);
    }
    FILE * fp = fopen(ScreenPath,"w+");
    if (fp)
    {
        int width = (m_memDC.iWidth + 3) & ~(0x3);
        // TODO:check the disk space
        fwrite(&bmp_file_header, sizeof(BITMAPFILEHEADER), 1, fp);
        fwrite(&bmp_info_header, sizeof(BITMAPINFOHEADER), 1, fp);
        fwrite(plus, 256 * 4, 1, fp);
        int dataLen = width * m_memDC.iHeight;
        char *data = new char[dataLen];

        memset(data, 0, dataLen); 
        if(data)
        {
            int i = m_memDC.iHeight;
            while(i > 0)
            {
                memcpy(data + (i - 1) * width,m_memDC.pbData + (m_memDC.iHeight - i)* m_memDC.iWidth,m_memDC.iWidth);
                i--;
            }
            fwrite(data, dataLen, 1, fp);
            delete []data;
            data = NULL;
        }
        fclose(fp);
    }

    ScreenRefresh();
   return ;

#endif
}
void CDisplay::DoCache(const DkRect& rect, paintflag_t paintFlag)
{
    DebugPrintf(DLC_PENGF, "CDisplay::DoCache() (%d, %d)-(%d, %d), flag: %08X", rect.Left(), rect.Top(), rect.Width(), rect.Height(), paintFlag);
    if (paintFlag & PAINT_FLAG_REPAINT)
    {
        if (m_rectUndraw.IsEmpty())
        {
            m_rectUndraw = rect;
            m_cachePaintFlag = paintFlag;
        }
        else
        {
            m_rectUndraw |= rect;
            m_cachePaintFlag |= (paintFlag & (PAINT_FLAG_FULL | PAINT_FLAG_REPAINT));
        }
    }
    else if (paintFlag & PAINT_FLAG_A2)
    {
        m_cachePaintFlag = PAINT_FLAG_A2;
    }
    else if (paintFlag & PAINT_FLAG_DU)
    {
        m_cachePaintFlag = PAINT_FLAG_DU;
    }

    if (m_rectUnflush.IsEmpty())
    {
        m_rectUnflush = rect;
    }
    else
    {
        m_rectUnflush |= rect;
    }
}

void CDisplay::SetScreenTips(const char* tips, unsigned int displayMSec)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "###### CDisplay::SetScreenTips %s, %d", tips, displayMSec);
    //TimerThread::GetInstance()->CancelTimer(CDisplay::ClearScreenTips, (void*)(m_tips.c_str()));

    m_tips.assign(tips);
    TimerThread::GetInstance()->AddTimer(CDisplay::ClearScreenTips, (void*)(m_tips.c_str()), SystemUtil::GetUpdateTimeInMs() + displayMSec, false, false, 0);
    ScreenRepaint();
}

void CDisplay::ClearScreenTips(void*)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "###### CDisplay::ClearScreenTips");
    CDisplay::GetDisplay()->ClearScreen();
}

void CDisplay::ClearScreen()
{
    if (!m_tips.empty())
    {
        m_tips.clear();
        StopFullRepaintOnce();
        ScreenRepaint(PAINT_FLAG_REPAINT);
    }
}

void CDisplay::EndCache(bool flushToFrameBuffer)
{
    // Mutex lock(this); 用这句替代以下代码可能死锁
    if (m_inEndCache)
        return;

    ValueScope<bool> inEndCacheScope(m_inEndCache, true);

#if 1
    DebugPrintf(DLC_PENGF, "CDisplay::EndCache(): flushToFrameBuffer: %d, undraw rect: (%d,%d)-(%d, %d), unflush rect: (%d, %d)-(%d, %d), flag: %08X", flushToFrameBuffer,
            m_rectUndraw.Left(), m_rectUndraw.Top(), m_rectUndraw.Width(), m_rectUndraw.Height(),
            m_rectUnflush.Left(), m_rectUnflush.Top(), m_rectUnflush.Width(), m_rectUnflush.Height(),
            m_cachePaintFlag
            );
#endif

    StopWatch watch(StopWatch::INIT_START);
    if (!m_rectUnflush.IsEmpty() && flushToFrameBuffer)
    {
        DoRepaint(m_rectUnflush.Left(), m_rectUnflush.Top(), m_rectUnflush.Width(), m_rectUnflush.Height(),
                m_rectUnflush.Left(), m_rectUnflush.Top(), m_cachePaintFlag, flushToFrameBuffer);
        m_rectUnflush.MakeEmpty();
        m_rectUndraw.MakeEmpty();
        m_cachePaintFlag = PAINT_FLAG_NONE;
    }
    else if (!m_rectUndraw.IsEmpty())
    {
        if (flushToFrameBuffer)
        {
            DebugPrintf(DLC_ERROR, "CDisplay::EndCache(): CANNOT REACH HERE. SOMETHING MUST GO WRONG!");
        }
        DoRepaint(m_rectUndraw.Left(), m_rectUndraw.Top(), m_rectUndraw.Width(), m_rectUndraw.Height(),
                m_rectUndraw.Left(), m_rectUndraw.Top(), m_cachePaintFlag, flushToFrameBuffer);
        m_cachePaintFlag &= (~PAINT_FLAG_REPAINT) & (~PAINT_FLAG_A2) & (~PAINT_FLAG_DU);
        m_rectUnflush.MakeEmpty();
        m_rectUndraw.MakeEmpty();
    }

    watch.Stop();

#if 1
    DebugPrintf(DLC_PENGF, "CDisplay::EndCache() takes %d ms, undraw: (%d,%d)-(%d, %d)", watch.DurationInMs(),
            m_rectUndraw.Left(), m_rectUndraw.Top(), m_rectUndraw.Right(), m_rectUndraw.Bottom());
    DebugPrintf(DLC_PENGF, "CDisplay::EndCache() takes %d ms, unflush: (%d,%d)-(%d, %d)", watch.DurationInMs(),
            m_rectUnflush.Left(), m_rectUnflush.Top(), m_rectUnflush.Right(), m_rectUnflush.Bottom());
#endif
}
void CDisplay::Suspend()
{
    CFbBitDc *pScreen = INativeDisplay::GetScreenGC();
    pScreen->Suspend();
}

void CDisplay::Resume()
{
    CFbBitDc *pScreen = INativeDisplay::GetScreenGC();
    pScreen->Resume();
}


CDisplay::CacheDisabler::CacheDisabler()
    :m_scope(CDisplay::GetDisplay()->m_cacheEnabled, false)
{
}

CDisplay::FullRepainter::FullRepainter()
    : m_scope(CDisplay::GetDisplay()->m_fullRepaint, true)
{
}
