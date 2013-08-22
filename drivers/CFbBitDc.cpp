
#define EINK

#include "drivers/CFbBitDc.h"

#ifdef EINK
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
//#include <pthread.h>
//#include <semaphore.h>
//#include <signal.h>
//#include <stdarg.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <termios.h>

#include <linux/fb.h>
//#include <linux/kd.h>
//#include <linux/vt.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#endif

#include "drivers/DeviceInfo.h"


#include "interface.h"
#include "Utility.h"
#include "dkRect.h"
#include "StopWatch.h"

using DkFormat::DkRect;
using DkFormat::StopWatch;

using namespace std;

#ifdef EINK
#define FBIO_MAGIC_NUMBER                   'F'

// Implemented in the eInk HAL.
#define FBIO_EINK_UPDATE_DISPLAY            _IO(FBIO_MAGIC_NUMBER, 0xdb) // 0x46db (fx_type: fx_update_full || fx_update_partial)
#define FBIO_EINK_UPDATE_DISPLAY_AREA       _IO(FBIO_MAGIC_NUMBER, 0xdd) // 0x46dd (update_area_t *)
#define FBIO_EINK_RESTORE_DISPLAY           _IO(FBIO_MAGIC_NUMBER, 0xef) // 0x46ef (fx_type: fx_update_full || fx_update_partial)
#define FBIO_EINK_SET_REBOOT_BEHAVIOR       _IO(FBIO_MAGIC_NUMBER, 0xe9) // 0x46e9 (reboot_behavior_t)
#define FBIO_EINK_GET_REBOOT_BEHAVIOR       _IO(FBIO_MAGIC_NUMBER, 0xed) // 0x46ed (reboot_behavior_t *)
#define FBIO_EINK_SET_DISPLAY_ORIENTATION   _IO(FBIO_MAGIC_NUMBER, 0xf0) // 0x46f0 (orientation_t)
#define FBIO_EINK_GET_DISPLAY_ORIENTATION   _IO(FBIO_MAGIC_NUMBER, 0xf1) // 0x46f1 (orientation_t *)


typedef unsigned int __u32 ;

typedef struct mxcfb_rect {
	__u32 top;
	__u32 left;
	__u32 width;
	__u32 height;
};

typedef struct mxcfb_alt_buffer_data {
	__u32 phys_addr;
	__u32 width;	/* width of entire buffer */
	__u32 height;	/* height of entire buffer */
	struct mxcfb_rect alt_update_region;	/* region within buffer to update */
};


typedef struct mxcfb_update_data { 
    struct mxcfb_rect update_region; 
    
    __u32 waveform_mode; // 0x0002 = WAVEFORM_MODE_GC16 
    __u32 update_mode; // 0x0000 = UPDATE_MODE_PARTIAL 
    __u32 update_marker; // 0x002a 
    int temp; // 0x1001 = TEMP_USE_PAPYRUS 
    __u32 flags; // 0x0000 
    struct mxcfb_alt_buffer_data alt_buffer_data; // must not used when flags is 0
};


typedef struct mxcfb_update_data_v510 { 
	struct mxcfb_rect update_region; 

	__u32 waveform_mode; // 0x0002 = WAVEFORM_MODE_GC16 
	__u32 update_mode; // 0x0000 = UPDATE_MODE_PARTIAL 
	__u32 update_marker; // 0x002a 
	__u32 hist_bw_waveform_mode;
	__u32 hist_gray_waveform_mode;
	int temp; // 0x1001 = TEMP_USE_PAPYRUS 
	__u32 flags; // 0x0000 
	struct mxcfb_alt_buffer_data alt_buffer_data; // must not used when flags is 0
};

//Kindle Touch 参数
#define MXCFB_WAIT_FOR_VSYNC	_IOW('F', 0x20, u_int32_t)
#define MXCFB_SET_GBL_ALPHA     _IOW('F', 0x21, struct mxcfb_gbl_alpha)
#define MXCFB_SET_CLR_KEY       _IOW('F', 0x22, struct mxcfb_color_key)
#define MXCFB_SET_OVERLAY_POS   _IOWR('F', 0x24, struct mxcfb_pos)
#define MXCFB_GET_FB_IPU_CHAN 	_IOR('F', 0x25, u_int32_t)
#define MXCFB_SET_LOC_ALPHA     _IOWR('F', 0x26, struct mxcfb_loc_alpha)
#define MXCFB_SET_LOC_ALP_BUF    _IOW('F', 0x27, unsigned long)
#define MXCFB_SET_GAMMA	       _IOW('F', 0x28, struct mxcfb_gamma)
#define MXCFB_GET_FB_IPU_DI 	_IOR('F', 0x29, u_int32_t)
#define MXCFB_GET_DIFMT	       _IOR('F', 0x2A, u_int32_t)
#define MXCFB_GET_FB_BLANK     _IOR('F', 0x2B, u_int32_t)
#define MXCFB_SET_DIFMT		_IOW('F', 0x2C, u_int32_t)

// IOCTLs for Kindle Touch E-ink panel updates 
// IOCTL 命令
#define MXCFB_SET_WAVEFORM_MODES	_IOW('F', 0x2B, struct mxcfb_waveform_modes)
#define MXCFB_SET_TEMPERATURE		_IOW('F', 0x2C, int32_t)
#define MXCFB_SET_AUTO_UPDATE_MODE	_IOW('F', 0x2D, __u32)
#define MXCFB_SEND_UPDATE		_IOW('F', 0x2E, struct mxcfb_update_data)
#define MXCFB_WAIT_FOR_UPDATE_COMPLETE	_IOW('F', 0x2F, __u32)
#define MXCFB_SET_PWRDOWN_DELAY		_IOW('F', 0x30, int32_t)
#define MXCFB_GET_PWRDOWN_DELAY		_IOR('F', 0x31, int32_t)
#define MXCFB_SET_UPDATE_SCHEME		_IOW('F', 0x32, __u32)
#define MXCFB_SET_PAUSE			_IOW('F', 0x33, __u32)
#define MXCFB_GET_PAUSE			_IOR('F', 0x34, __u32)
#define MXCFB_SET_RESUME		_IOW('F', 0x35, __u32)
#define MXCFB_CLEAR_UPDATE_QUEUE	_IOW('F', 0x36, __u32)
#define MXCFB_SEND_UPDATE_V510		_IOW('F', 0x2E, struct mxcfb_update_data_v510)

// Kindle Touch update 参数
#define GRAYSCALE_8BIT				0x1
#define GRAYSCALE_8BIT_INVERTED			0x2

#define AUTO_UPDATE_MODE_REGION_MODE		0
#define AUTO_UPDATE_MODE_AUTOMATIC_MODE		1
#define AUTO_UPDATE_MODE_AUTOMATIC_MODE_FULL	AUTO_UPDATE_MODE_AUTOMATIC_MODE // Lab126 
#define AUTO_UPDATE_MODE_AUTOMATIC_MODE_PART	2 // Lab126 

#define UPDATE_SCHEME_SNAPSHOT			0
#define UPDATE_SCHEME_QUEUE			1
#define UPDATE_SCHEME_QUEUE_AND_MERGE		2

#define UPDATE_MODE_PARTIAL			0x0
#define UPDATE_MODE_FULL			0x1

//#define WAVEFORM_MODE_INIT	0x0	// Screen goes to white (clears) 
//#define WAVEFORM_MODE_DU	0x1	// Grey->white/grey->black 
//#define WAVEFORM_MODE_GC16	0x2	// High fidelity (flashing)
//#define WAVEFORM_MODE_GC4	0x3	// Lower fidelity 
//#define WAVEFORM_MODE_A2	0x4	// Faster but even lower fidelity 
//#define WAVEFORM_MODE_GL16	0x5	// High fidelity from white transition 
#define WAVEFORM_MODE_INIT	0x0	/* Screen goes to white (clears) */
#define WAVEFORM_MODE_DU	0x1	/* Grey->white/grey->black */
#define WAVEFORM_MODE_GC16	0x2	/* High fidelity (flashing) */
#define WAVEFORM_MODE_GC4	WAVEFORM_MODE_GC16 /* For compatibility */
#define WAVEFORM_MODE_GC16_FAST	0x3	/* Medium fidelity */
#define WAVEFORM_MODE_A2	0x4	/* Faster but even lower fidelity */
#define WAVEFORM_MODE_GL16	0x5	/* High fidelity from white transition */
#define WAVEFORM_MODE_GL16_FAST	0x6	/* Medium fidelity from white transition */

#define WAVEFORM_MODE_AUTO			257

#define TEMP_USE_AMBIENT			0x1000
#define TEMP_USE_PAPYRUS			0X1001

#define EPDC_FLAG_ENABLE_INVERSION		0x01
#define EPDC_FLAG_FORCE_MONOCHROME		0x02
#define EPDC_FLAG_USE_CMAP				0x04
#define EPDC_FLAG_USE_ALT_BUFFER		0x100

#define FB_POWERDOWN_DISABLE			-1




enum einkfb_events_t
{
    einkfb_event_update_display = 0,        // FBIO_EINK_UPDATE_DISPLAY
    einkfb_event_update_display_area,       // FBIO_EINK_UPDATE_DISPLAY_AREA

    einkfb_event_blank_display,             // FBIOBLANK (fb.h)
    einkfb_event_rotate_display,            // FBIO_EINK_SET_DISPLAY_ORIENTATION

    einkfb_event_null = -1
};
typedef enum einkfb_events_t einkfb_events_t;

enum fx_type
{
    // Deprecated from HAL, but still supported by Shim.
    //
    fx_stipple_posterize_dark = 5,          // Row 0: WPWP...; row 1: PBPB... (W=white, B=black, P=posterize).
    fx_stipple_posterize_lite = 6,          // Row 0: WPWP...; row 1: PWPW... (W=white, P=posterize).

    fx_stipple_lighten_dark = 7,            // Row 0: WXWX...; row 1: XBXB... (W=white, B=black, X=keep).
    fx_stipple_lighten_lite = 8,            // Row 0: WXWX...; row 1: XWXW... (W=white, X=keep).

    fx_mask = 11,                           // Only for use with update_area_t's non-NULL buffer which_fx.
    fx_buf_is_mask = 14,                    // Same as fx_mask, but doesn't require a doubling (i.e., the buffer & mask are the same).

    fx_flash = 20,                          // Only for use with update_area_t (for faking a flashing update).

    fx_none = -1,                           // No legacy-FX to apply.

    // Screen-update FX, supported by HAL.
    //
    fx_update_partial = 0,                  // eInk GU/PU/MU-style (non-flashing) update.
    fx_update_full = 1                      // eInk GC-style (slower, flashing) update.
};

typedef enum fx_type fx_type;

struct update_area_t
{
    // Note:  The bottom-right (x2, y2) coordinate is actually such that (x2 - x1) and (y2 - y1)
    //        are xres and yres, respectively, when normally xres and yres would be
    //        (x2 - x1) + 1 and (y2 - y1) + 1, respectively.
    //
    int         x1, y1,                     // Top-left...
                x2, y2;                     // ...bottom-right.

    fx_type     which_fx;                   // FX to use.

    INT8        *buffer;                    // If NULL, extract from framebuffer, top-left to bottom-right, by rowbytes.
};

typedef struct update_area_t update_area_t;

#endif

CFbBitDc::CFbBitDc()
    : m_addr(NULL)
    , m_nAddrLength(0)
    , m_suspend(false)
{
#ifndef _X86_LINUX
    fb_fix_screeninfo fb_fix;
    m_nHeight = 0;
    m_fb = open("/dev/fb0", O_RDWR);
    if (m_fb < 0 )
    {
        DebugLog(DLC_ERROR, "Can not open frame buffer fb0!!!");
        DebugPrintf(DLC_CHENM, "Can not open frame buffer fb0!!!");
        goto err0;
    }
    
    if (ioctl(m_fb,FBIOGET_FSCREENINFO, &fb_fix) < 0 )
    {
        DebugLog(DLC_ERROR, "Can not get screeninfo!!!");
        goto err1;
    }

    m_addr = (INT8*)mmap(NULL, fb_fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, m_fb, 0);
    if (NULL == m_addr)
    {
        DebugLog(DLC_ERROR, "m_addr == NULL\n");
        goto err1;
    }

    m_nAddrLength =  fb_fix.smem_len;


    if (! this->UpdateHardwareScreenInfo())
    {
        DebugLog(DLC_ERROR, "Can not get screeninfo!!!");
        goto err2;
    }
    return;
err2:
    munmap(m_addr, fb_fix.smem_len);
    m_addr = NULL;
err1:
    close(m_fb);
    m_fb = -1;
err0:
    return;
#else
    m_nWidth = DeviceInfo::GetDisplayScreenWidth();
    m_nHeight = DeviceInfo::GetDisplayScreenHeight();
    m_nWidth_virtual = m_nWidth;
    m_nHeight_virtual = m_nHeight;
    const char EMULATOR_FB_MAP[] = "/DuoKan/fb_shmem";

    m_fb = open(EMULATOR_FB_MAP, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (m_fb >= 0)
    {
        lseek(m_fb, m_nWidth * m_nHeight, SEEK_SET);
        write(m_fb, &m_fb, 1);
        struct stat statbuf;
        int flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
        stat(EMULATOR_FB_MAP, &statbuf);
        chmod(EMULATOR_FB_MAP, statbuf.st_mode | flags);
        m_addr = (INT8*)mmap(NULL, m_nWidth * m_nHeight, PROT_READ | PROT_WRITE, MAP_SHARED, m_fb, 0);
    }
#endif
}


CFbBitDc::~CFbBitDc()
{
#ifdef EINK
    if(NULL != m_addr)
    {
         munmap(m_addr,  m_nAddrLength);
         m_addr = NULL;
    }

    if(m_fb >= 0)
    {
        close(m_fb);
        m_fb = -1;
    }

#endif
}

/* @ 2011.12.20 luoxuping, for Kindle 4 slim,  e-ink driver uses 8 bits per pixel */
void CFbBitDc::BitBlt8BppFast(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, const DK_IMAGE& srcImg, INT32 isX, INT32 isY)
{
    int srcStrip = srcImg.iStrideWidth;
    int bpp = 8; //  for kindle 4 slim,  dst bpp is 8
    int dstStrip = (m_nWidth_virtual * bpp + 7)/8 ;



    __u8* srcStartPerRow = srcImg.pbData + srcStrip * isY + isX;
    __u8* dstStartPerRow = (__u8*)m_addr + dstStrip * iY + iX;
    //__u8* buffer = new __u8[iWidth];
    for (int y = 0; y < iHeight; ++y)
    {
        //if (NULL != buffer)
        //{
        //    for (int x = 0; x < iWidth; ++x)
        //    {
        //        __u8 c = srcStartPerRow[x];
        //        c = c & 0xf0;
        //        buffer[x] = c;
        //    }
        //    memcpy(dstStartPerRow, buffer, iWidth);
        //}
        //else
        //{
        memcpy(dstStartPerRow, srcStartPerRow, iWidth);
        //}
        srcStartPerRow += srcStrip;
        dstStartPerRow += dstStrip;
    }
    //if (NULL == buffer)
    //{
    //    delete []buffer;
    //}

	return;
}

void CFbBitDc::BitBlt4BppFast(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, const DK_IMAGE& srcImg, INT32 isX, INT32 isY)
{
    int srcStrip = srcImg.iStrideWidth;
    int bpp = 4; // here we assume dst bpp is 4
    int dstStrip = (m_nWidth_virtual * bpp + 31) / 32 * 4;

    __u8* srcStartPerRow = srcImg.pbData + srcStrip * isY + isX;
    __u8* dstStartPerRow = (__u8*)m_addr + dstStrip * iY + iX / 2;
    for (int y = 0; y < iHeight; ++y)
    {
        __u8* curSrcU8 = srcStartPerRow;
        __u8* curDstU8 = dstStartPerRow;
        int iXTemp = iX;

        if(iXTemp % 2)
        {
            *curDstU8 &= 0xF0;
            *curDstU8 |= ((*curSrcU8) & 0xF0) >> 4;
            ++iXTemp;
            ++curDstU8;
            ++curSrcU8;
        }
        if(iXTemp % 4)
        {
            *curDstU8 = ((*curSrcU8) & 0xF0) | (((*(curSrcU8 + 1)) & 0xF0) >> 4);
            ++curDstU8;
            iXTemp += 2;
            curSrcU8 += 2;
        }

        __u32 *curSrc = (__u32*)(curSrcU8);
        __u16 *curDst = (__u16*)(curDstU8);
        __u32 *srcRealEnd = (__u32*)(srcStartPerRow + iWidth);
        __u32 *srcLoopEnd = (__u32*)(srcStartPerRow + iWidth - 3);
        for (;curSrc < srcLoopEnd; ++curSrc, ++curDst)
        {
            __u32 t = *curSrc;
            if (0 == t)
            {
                *curDst = 0;
            }
            else
            {
                // PENGF: assume little endian here
                // source bytes are AB CD EF GH
                // __u32 is 0xGHEFCDAB
                // related dst bytes should be AC EG
                // __u16 is 0xEGAC

                // initially t = 0xGHEFCDAB
                t &= 0xF0F0F0F0; // t = 0xG0E0C0A0
                t |= t >> 12; // t = (0xG0E0C0A0 | 0x000G0E0C) = 0xG0EGCEAC
                t &= 0x00FF00FF; // t = 0x00EG00AC
                t |= t >> 8; // t = 0x00EGEGAC
                *curDst = __u16(t & 0xFFFF); // *curDst = 0xEGAC
            }
        }
        if (curSrc < srcRealEnd)
        {
            curSrcU8 = (__u8*)curSrc;
            curDstU8 = (__u8*)curDst;
            int remainToCopy = (__u8*)srcRealEnd - (__u8*)curSrc;
            if (remainToCopy >= 2)
            {
                *curDstU8=((*curSrcU8) & 0xF0) | (((*(curSrcU8 + 1)) & 0xF0) >> 4);
                remainToCopy-=2;
                ++curDstU8;
                curSrcU8+=2;
            }
            if (remainToCopy > 0)
            {
                *curDstU8 &= 0x0F;
                *curDstU8 |= (*curSrcU8) & 0xF0;
            }
        }
        srcStartPerRow += srcStrip;
        dstStartPerRow += dstStrip;
    }
}

void CFbBitDc::BitBlt(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, const DK_IMAGE& srcImg, INT32 isX, INT32 isY, paintflag_t paintFlag)
{
    if (m_suspend)
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "Screen has been suspended");
        return;
    }

//     DebugPrintf(DLC_DIAGNOSTIC, "CFbBitDc::BitBlt, (%d,%d)~(%d,%d),%08X", iX, iY, iWidth, iHeight, paintFlag);
#ifdef _X86_LINUX
    if (0 != m_addr)
    {
        memcpy(m_addr, srcImg.pbData, m_nWidth * m_nHeight);
    }
    return;
#endif
    

    DebugPrintf(DLC_EINK_PAINT, "CFbBitDc::BitBlt(iX=%d, iY=%d, iWidth=%d, iHeight=%d, isX=%d, isY=%d)", iX, iY, iWidth, iHeight, isX, isY);

    clock_t start = clock();
    if(NULL == srcImg.pbData || NULL == m_addr)
    {
        DebugPrintf(DLC_EINK_PAINT, " CFbBitDc::BitBlt failed @ 1");
        return;
    }

    DkRect targetRect = GetRect() & DkRect::FromLeftTopWidthHeight(iX, iY, iWidth, iHeight); 
    if (targetRect.IsEmpty())
    {
        DebugPrintf(DLC_EINK_PAINT, " CFbBitDc::BitBlt failed @ 2");
        return;
    }

    DkRect srcRect = DkRectFromImage(srcImg) & DkRect::FromLeftTopWidthHeight(isX, isY, iWidth, iHeight);
    if (srcRect.IsEmpty())
    {
        DebugPrintf(DLC_EINK_PAINT, " CFbBitDc::BitBlt failed @ 3");
        return;
    }


    INT32 nCopyWidth = std::min(srcRect.Width(), targetRect.Width());
    INT32 nCopyHeight = std::min(srcRect.Height(), targetRect.Height());

#ifdef EINK
    if (DeviceInfo::IsK4NT() ) {
        DebugPrintf(DLC_EINK_PAINT, " CFbBitDc::BitBlt on Kindle 4 non-touch");
		BitBlt8BppFast(iX, iY, nCopyWidth, nCopyHeight, srcImg, isX, isY);
    }else if (DeviceInfo::IsK4Touch())
    {
        DebugPrintf(DLC_EINK_PAINT, " CFbBitDc::BitBlt on Kindle 4 Touch");
		BitBlt8BppFast(iX, iY, nCopyWidth, nCopyHeight, srcImg, isX, isY);
    }else if (DeviceInfo::IsKPW())
	{
        DebugPrintf(DLC_EINK_PAINT, " CFbBitDc::BitBlt on Kindle Paperwhite");
		BitBlt8BppFast(iX, iY, nCopyWidth, nCopyHeight, srcImg, isX, isY);
	}
    else    
    {
		BitBlt4BppFast(iX, iY, nCopyWidth, nCopyHeight, srcImg, isX, isY);
	}

    DebugPrintf(DLC_TIME, "CFbBitDc::BitBlt() Time elapsed=%d ms.",
            1000*(clock()-start)/CLOCKS_PER_SEC);

    if (DeviceInfo::IsK4Touch())
    {
        DebugPrintf(DLC_EINK_PAINT, " CFbBitDc::BitBlt update screen on Kindle Touch");
		if(DeviceInfo::IsTouch510()) 
		{

			UpdateToScreenKindleTouch510(targetRect.Left(), targetRect.Top(), nCopyWidth, nCopyHeight, paintFlag);
		}else 
		{
			UpdateToScreenKindleTouch(targetRect.Left(), targetRect.Top(), nCopyWidth, nCopyHeight, paintFlag);
		}
    }
    else if (DeviceInfo::IsKPW())
	{
		UpdateToScreenKindlePaperwhite(targetRect.Left(), targetRect.Top(), nCopyWidth, nCopyHeight, paintFlag);
	}
	else 
	{
        UpdateToScreen(targetRect.Left(), targetRect.Top(), nCopyWidth, nCopyHeight, paintFlag);
	}


#endif
}


// 猜想: the difference between FBIO_EINK_UPDATE_DISPLAY_AREA and FBIO_EINK_UPDATE_DISPLAY,
// Maybe FBIO_EINK_UPDATE_DISPLAY won't do a full pixel compare before sending to EPSON for refreshing.

void CFbBitDc::ScreenRefresh()
{
#ifdef _X86_LINUX
    return;
#endif
    StopWatch watch(StopWatch::INIT_START);
    DebugLog(DLC_EINK_PAINT, "CFbBitDc::ScreenRefresh()");
    if (DeviceInfo::IsK4Touch())
    {
        DebugPrintf(DLC_EINK_PAINT, " CFbBitDc::BitBlt update screen on Kindle Touch");
        if(DeviceInfo::IsTouch510()) 
        {

            UpdateToScreenKindleTouch510(0, 0, GetWidth(), GetHeight(), PAINT_FLAG_FULL);
        }else 
        {
            UpdateToScreenKindleTouch(0, 0, GetWidth(), GetHeight(), PAINT_FLAG_FULL);
        }
    }
	else if (DeviceInfo::IsKPW())
	{
		UpdateToScreenKindlePaperwhite(0, 0, GetWidth(), GetHeight(), PAINT_FLAG_FULL);
	}
    else
    {
        UpdateToScreen(0, 0, GetWidth(), GetHeight(), PAINT_FLAG_FULL);
    }
    //ioctl(m_fb, FBIO_EINK_RESTORE_DISPLAY, fx_update_full);
    //ioctl(m_fb, FBIO_EINK_RESTORE_DISPLAY, fx_update_full);
    watch.Stop();
    DebugPrintf(DLC_TIME, "CFbBitDc::ScreenRefresh() Time elapsed=%d ms.",
      watch.DurationInMs());
    DebugPrintf(DLC_PENGF, "CFbBitDc::ScreenRefresh() Time elapsed=%d ms.",
        watch.DurationInMs());
}


void CFbBitDc::UpdateToScreen(INT32 iX,  INT32 iY, INT32 iWidth, INT32 iHeight, paintflag_t paintFlag)
{
#ifdef _X86_LINUX
    return;
#endif
    StopWatch watch(StopWatch::INIT_START);
    DebugPrintf(DLC_EINK_PAINT, "CFbBitDc::UpdateToScreen(iX=%d, iY=%d, iWidth=%d, iHeight=%d)",
        iX,  iY, iWidth, iHeight);

    struct update_area_t update_area;
    memset(&update_area,0,sizeof(update_area_t));
    update_area.x1 = iX;
    update_area.y1 = iY;
    update_area.x2 = iX + iWidth;
    update_area.y2 = iY + iHeight;
    update_area.buffer = (INT8*)NULL;
    if(paintFlag & PAINT_FLAG_FULL)
    {
        DebugPrintf(DLC_EINK_PAINT, "CFbBitDc::UpdateToScreen PAINT_FLAG_FULL");
        ++m_addr[0]; // 如果CRC不发生改变，驱动不会刷屏
        update_area.which_fx = ( enum fx_type)fx_update_full;
        ioctl(m_fb, FBIO_EINK_UPDATE_DISPLAY, (enum fx_type)fx_update_full);
        //ioctl(m_fb, FBIO_EINK_UPDATE_DISPLAY_AREA, (unsigned long)&update_area);
    }
    else
    {
        update_area.which_fx =( enum fx_type)fx_update_partial;
        if (iWidth > (m_nWidth - 50) && iHeight > (m_nHeight - 50))
            ioctl(m_fb, FBIO_EINK_UPDATE_DISPLAY, update_area.which_fx);
        else
            ioctl(m_fb, FBIO_EINK_UPDATE_DISPLAY_AREA, (unsigned long)&update_area);
    }

    watch.Stop();
    DebugPrintf(DLC_TIME, "CFbBitDc::UpdateToScreen() Time elapsed=%d ms.",
        watch.DurationInMs());

    DebugPrintf(DLC_PENGF, "CFbBitDc::UpdateToScreen() Time elapsed=%d ms.",
        watch.DurationInMs());
}


void CFbBitDc::UpdateToScreenKindleTouch(INT32 iX,  INT32 iY, INT32 iWidth, INT32 iHeight, paintflag_t paintFlag)
{
#ifdef _X86_LINUX
    return;
#endif
    StopWatch watch(StopWatch::INIT_START);
    DebugPrintf(DLC_EINK_PAINT, "CFbBitDc::UpdateToScreenKindleTouch(iX=%d, iY=%d, iWidth=%d, iHeight=%d)",
        iX,  iY, iWidth, iHeight);

    mxcfb_update_data update_data;
    mxcfb_rect update_region;
    update_region.top = iY;
    update_region.left = iX;
    update_region.width = iWidth;
    update_region.height = iHeight;

    DebugPrintf(DLC_EINK_PAINT, " CFbBitDc::UpdateToScreenKindleTouch with (%d,%d,%d,%d)",iY,iX,iWidth,iHeight);

    memset(&update_data,0,sizeof(update_data));
    update_data.update_region = update_region;
    update_data.waveform_mode = WAVEFORM_MODE_AUTO;
    update_data.temp = TEMP_USE_AMBIENT;

    update_data.flags = EPDC_FLAG_ENABLE_INVERSION;
    update_data.update_marker = 0x002a; //同步用

    update_data.alt_buffer_data.alt_update_region = update_region;
    update_data.alt_buffer_data.phys_addr = (__u32) m_addr;
    update_data.alt_buffer_data.height = iHeight;
    update_data.alt_buffer_data.width = iWidth;

    if(paintFlag & PAINT_FLAG_FULL)
    {
        update_data.update_mode =   UPDATE_MODE_FULL;
        update_data.waveform_mode = WAVEFORM_MODE_GC16;
    }
    else if (paintFlag & PAINT_FLAG_A2)
    {
        update_data.update_mode =   UPDATE_MODE_PARTIAL;
        update_data.waveform_mode = WAVEFORM_MODE_A2;
    }
    else if (paintFlag & PAINT_FLAG_DU)
    {
        update_data.update_mode = UPDATE_MODE_PARTIAL;
        update_data.waveform_mode = WAVEFORM_MODE_DU;
    }
    else
    {
        update_data.update_mode =   UPDATE_MODE_PARTIAL;
    }

    //ioctl(m_fb, MXCFB_SET_AUTO_UPDATE_MODE, AUTO_UPDATE_MODE_AUTOMATIC_MODE);

    ioctl(m_fb, MXCFB_SEND_UPDATE,  (unsigned long)&update_data);

    //if(paintFlag & PAINT_FLAG_FULL)
    //{
    //    DebugPrintf(DLC_EINK_PAINT, "CFbBitDc::UpdateToScreen PAINT_FLAG_FULL");
    //    //++m_addr[0]; // Remark(zhaigh): 去掉这句话会导致手动刷屏无效，原因不确定
    //    ioctl(m_fb, MXCFB_SEND_UPDATE, (unsigned long)&update_data);
    //}
    //else
    //{
    //    if (iWidth > (m_nWidth - 50) && iHeight > (m_nHeight - 50))
    //        ioctl(m_fb, MXCFB_SEND_UPDATE, (unsigned long)&update_data);
    //    else
    //        ioctl(m_fb, MXCFB_SEND_UPDATE,  (unsigned long)&update_data);
    //}

    watch.Stop();
    DebugPrintf(DLC_TIME, "CFbBitDc::UpdateToScreenKindleTouch() Time elapsed=%d ms.",
        watch.DurationInMs());

    DebugPrintf(DLC_PENGF, "CFbBitDc::UpdateToScreenKindleTouch() Time elapsed=%d ms.",
        watch.DurationInMs());
}

// Touch v5.1.0 mxcfb_update_data结构体参数改为mxcfb_update_data_v510
void CFbBitDc::UpdateToScreenKindleTouch510(INT32 iX,  INT32 iY, INT32 iWidth, INT32 iHeight, paintflag_t paintFlag)
{
#ifdef _X86_LINUX
    return;
#endif
	StopWatch watch(StopWatch::INIT_START);
	DebugPrintf(DLC_EINK_PAINT, "CFbBitDc::UpdateToScreenKindleTouch510(iX=%d, iY=%d, iWidth=%d, iHeight=%d)",
		iX,  iY, iWidth, iHeight);

	mxcfb_update_data_v510 update_data;
	mxcfb_rect update_region;
	update_region.top = iY;
	update_region.left = iX;
	update_region.width = iWidth;
	update_region.height = iHeight;

	DebugPrintf(DLC_EINK_PAINT, " CFbBitDc::UpdateToScreenKindleTouch510 with (%d,%d,%d,%d)",iY,iX,iWidth,iHeight);

	memset(&update_data,0,sizeof(update_data));
	update_data.update_region = update_region;
	update_data.waveform_mode = WAVEFORM_MODE_AUTO;
	update_data.temp = TEMP_USE_AMBIENT;

	update_data.flags = EPDC_FLAG_ENABLE_INVERSION;
	update_data.update_marker = 0x002a; //同步用

	update_data.alt_buffer_data.alt_update_region = update_region;
	update_data.alt_buffer_data.phys_addr = (__u32) m_addr;
	update_data.alt_buffer_data.height = iHeight;
	update_data.alt_buffer_data.width = iWidth;

	update_data.hist_bw_waveform_mode = WAVEFORM_MODE_AUTO;
	update_data.hist_gray_waveform_mode =  WAVEFORM_MODE_AUTO;

	if(paintFlag & PAINT_FLAG_FULL)
	{
        update_data.waveform_mode = WAVEFORM_MODE_GC16_FAST;
		update_data.update_mode =   UPDATE_MODE_FULL;
    }
    else if (paintFlag & PAINT_FLAG_A2)
    {
        update_data.waveform_mode = WAVEFORM_MODE_A2;
        update_data.update_mode = UPDATE_MODE_PARTIAL;
    }
    else if (paintFlag & PAINT_FLAG_DU)
    {
        update_data.waveform_mode = WAVEFORM_MODE_DU;
        update_data.update_mode = UPDATE_MODE_PARTIAL;
    }
	else
	{
        update_data.waveform_mode = WAVEFORM_MODE_GL16_FAST;
		update_data.update_mode =   UPDATE_MODE_PARTIAL;
	}


	ioctl(m_fb, MXCFB_SEND_UPDATE_V510,  (unsigned long)&update_data);
	watch.Stop();
	DebugPrintf(DLC_TIME, "CFbBitDc::UpdateToScreenKindleTouch510() Time elapsed=%d ms.",
		watch.DurationInMs());

	DebugPrintf(DLC_PENGF, "CFbBitDc::UpdateToScreenKindleTouch510() Time elapsed=%d ms.",
		watch.DurationInMs());
}

void CFbBitDc::UpdateToScreenKindlePaperwhite(INT32 iX,  INT32 iY, INT32 iWidth, INT32 iHeight, paintflag_t paintFlag)
{
	UpdateToScreenKindleTouch510(iX, iY, iWidth, iHeight, paintFlag);
}

bool CFbBitDc::SetOrientation(orientation_t orientation)
{
    return UpdateHardwareScreenInfo();
}

bool CFbBitDc::GetOrientation(orientation_t* orientation) const
{
    if (NULL == orientation)
    {
        return false;
    }

    if (ioctl(m_fb, FBIO_EINK_GET_DISPLAY_ORIENTATION, orientation))
    {
        DebugPrintf(DLC_ERROR, "Set orientation failed\n");
        return false;
    }
    return true;
}

bool CFbBitDc::SetRebootBehavior(reboot_behavior_t reboot_behavior)
{
    if (ioctl(m_fb, FBIO_EINK_SET_REBOOT_BEHAVIOR, reboot_behavior))
    {
        DebugPrintf(DLC_ERROR, "CFbBitDc::SetRebootBehavior() failed\n");
        return false;
    }
    return true;
}

bool CFbBitDc::GetRebootBehavior(reboot_behavior_t* reboot_behavior) const
{
    if (NULL == reboot_behavior)
    {
        return false;
    }

    if (ioctl(m_fb, FBIO_EINK_GET_REBOOT_BEHAVIOR, reboot_behavior))
    {
        DebugPrintf(DLC_ERROR, "CFbBitDc::GetRebootBehavior() failed");
        return false;
    }
    return true;
}

bool CFbBitDc::UpdateHardwareScreenInfo()
{
#ifndef _X86_LINUX
    fb_var_screeninfo fb_var;
    if(ioctl(m_fb, FBIOGET_VSCREENINFO, &fb_var) == -1)
    {
        DebugPrintf(DLC_CHENM, "FBIOGET_VSCREENINFO can't open!!!");
        return false;
    }
    m_nWidth = fb_var.xres;
    m_nHeight = fb_var.yres;

    m_nWidth_virtual = fb_var.xres_virtual;
    m_nHeight_virtual = fb_var.yres_virtual;

    DebugPrintf(DLC_CHENM, "framebuffer with size %d x %d", m_nWidth, m_nHeight);
    DebugPrintf(DLC_CHENM, "framebuffer with virtual size %d x %d", m_nWidth_virtual, m_nHeight_virtual);
#else
    m_nWidth = DeviceInfo::GetDisplayScreenWidth();
    m_nHeight = DeviceInfo::GetDisplayScreenHeight();
    m_nWidth_virtual = m_nWidth;
    m_nHeight_virtual = m_nHeight;
#endif

    return true;
}
