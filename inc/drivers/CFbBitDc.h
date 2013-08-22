////////////////////////////////////////////////////////////////////////
// CFbBitDc.h
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __CFBBITDC_H__
#define __CFBBITDC_H__

#include "GUI/dkImage.h"
#include "Framework/PaintFlag.h"

//using namespace DkFormat;
using DkFormat::DK_IMAGE;
using DkFormat::DkRect;

enum orientation_t
{
    orientation_portrait,
    orientation_portrait_upside_down,
    orientation_landscape,
    orientation_landscape_upside_down
};
typedef enum orientation_t orientation_t;

enum reboot_behavior_t
{
    reboot_screen_asis,
    reboot_screen_clear,
    reboot_screen_splash
};

typedef enum reboot_behavior_t reboot_behavior_t;


class CFbBitDc
{
public:
    CFbBitDc();

    ~CFbBitDc();

    //重新刷整个屏幕
    void ScreenRefresh();
    //将 DK_IMAGE 数据刷到屏幕
    void BitBlt(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, const DK_IMAGE& srcImg, INT32 isX, INT32 isY,  paintflag_t paintFlag);

    //物理屏幕宽度
    INT32 GetWidth() const
    {
        return m_nWidth;
    }

    //物理屏幕高度
    INT32 GetHeight() const
    {
        return m_nHeight;
    }

    bool SetOrientation(orientation_t orientation);
    bool GetOrientation(orientation_t* orientation) const;

    void Suspend() { m_suspend = true; }
    void Resume() { m_suspend = false; }

private:
    //没有用的 api
    bool SetRebootBehavior(reboot_behavior_t reboot_behavior);
    bool GetRebootBehavior(reboot_behavior_t* reboot_behavior) const;

private:
    bool UpdateHardwareScreenInfo();
    void BitBlt4BppFast(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, const DK_IMAGE& srcImg, INT32 isX, INT32 isY);
	void BitBlt8BppFast(INT32 iX, INT32 iY, INT32 iWidth, INT32 iHeight, const DK_IMAGE& srcImg, INT32 isX, INT32 isY);

    void UpdateToScreen(INT32 iX,  INT32 iY, INT32 iWidth, INT32 iHeight, paintflag_t paintFlag);
    void UpdateToScreenKindleTouch(INT32 iX,  INT32 iY, INT32 iWidth, INT32 iHeight, paintflag_t paintFlag);
    void UpdateToScreenKindleTouch510(INT32 iX,  INT32 iY, INT32 iWidth, INT32 iHeight, paintflag_t paintFlag);
    void UpdateToScreenKindlePaperwhite(INT32 iX,  INT32 iY, INT32 iWidth, INT32 iHeight, paintflag_t paintFlag);

    DkRect GetRect() const
    {
        return DkRect::FromLeftTopWidthHeight(0, 0, m_nWidth, m_nHeight);
    };

    INT32 m_nWidth;
    INT32 m_nHeight;
    INT32 m_nWidth_virtual;
    INT32 m_nHeight_virtual;

    INT8  *m_addr;
    INT32 m_nAddrLength;
    INT32 m_fb;
    bool  m_suspend;
};

#endif
