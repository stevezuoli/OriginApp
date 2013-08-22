////////////////////////////////////////////////////////////////////////
// UIProgressBar.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UIProgressBar.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ColorManager.h"

#define COLOR_BORDER 0x565656
#define COLOR_FG 0x91CE00
#define COLOR_BG 0x898989

UIProgressBar::UIProgressBar() :
    UIWindow(NULL, IDSTATIC)
    ,    m_bIsDisposed(FALSE)
    ,    m_iMax(0)
    ,    m_iCurrent(0)
    ,    m_iBarLengthInPixel(0)
    ,    m_iBarHeightInPixel(0)
    ,    m_fDrawBold(FALSE)
{
    Init();
}

UIProgressBar::UIProgressBar(UIContainer* pParent, const DWORD dwId) :
    UIWindow(pParent, dwId)
    ,    m_bIsDisposed(FALSE)
    ,    m_iMax(0)
    ,    m_iCurrent(0)
    ,    m_iBarLengthInPixel(0)
    ,    m_iBarHeightInPixel(0)
    ,    m_fDrawBold(FALSE)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }

    Init();
}

UIProgressBar::~UIProgressBar()
{
    Dispose();
}

void UIProgressBar::Dispose()
{
    UIWindow::Dispose();
    OnDispose(m_bIsDisposed);
}

void UIProgressBar::OnDispose(BOOL bIsDisposed)
{
    if (bIsDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;
}

void UIProgressBar::Init()
{
    m_bIsDisposed = FALSE;
    m_iMax = 100;
    m_iCurrent = 0;
    m_iBarLengthInPixel = 0;
    m_minHeight = m_iBarHeightInPixel = 1;
    m_fDrawBold = FALSE;

 //   m_minHeight = m_iBarHeightInPixel;
}

BOOL UIProgressBar::GetDrawBold()
{
    return m_fDrawBold;
}

void UIProgressBar::SetDrawBold(BOOL fDrawBold)
{
    m_fDrawBold = fDrawBold;
}

void UIProgressBar::SetBarHeight(INT32 iHeight)
{
    if (iHeight >= 0)
    {
        m_minHeight = m_iBarHeightInPixel = iHeight;
    //    m_minHeight = m_iBarHeightInPixel;
    }
}

INT32 UIProgressBar::GetBarHeight() const
{
    return m_iBarHeightInPixel;
}

INT32 UIProgressBar::GetBarLength() const
{
    return m_iBarLengthInPixel;
}

BOOL UIProgressBar::CanSetFocus()
{
    return TRUE;
}

void UIProgressBar::SetProgress(INT32 iProgress)
{
    if (iProgress <= 0)
    {
        iProgress = 0;
    }
    else if (iProgress >= m_iMax)
    {
        iProgress = m_iMax;
    }

    m_iCurrent = iProgress;

    RecalcBarLength();
    //Repaint();
}

INT32 UIProgressBar::GetProgress()
{
    return m_iCurrent;
}

HRESULT UIProgressBar::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
    {
        return S_OK;
    }

    if (drawingImg.pbData == NULL)// || !m_IsChanged)
    {
        return E_FAIL;
    }

    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;

    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        drawingImg,
        rcSelf,
        &imgSelf
        ));

    CTpGraphics grf(imgSelf);

    if(m_fDrawBold)
    {
        RTN_HR_IF_FAILED(grf.DrawLine(
            0,
            0,
            m_iWidth,
            m_iHeight,
            SOLID_STROKE));
    }
    else
    {
        // clear the area
        RTN_HR_IF_FAILED(grf.FillRect(
            0,
            0,
            m_iWidth,
            m_iHeight,
            ColorManager::knWhite));

        INT32 iDrawHeight = m_iBarHeightInPixel > m_iHeight ? m_iHeight : m_iBarHeightInPixel;
        int top = (m_iHeight - iDrawHeight) / 2;
        // draw progress
        if(m_iBarLengthInPixel > 0)
        {
            RTN_HR_IF_FAILED(grf.DrawLine(
                0,
                top,
                m_iBarLengthInPixel,
                iDrawHeight,  //(m_iHeight + iDrawHeight) / 2,
                SOLID_STROKE));
        }

        int left = m_iBarLengthInPixel + 3;
        while (left < m_iWidth)
        {
            int squareWidth = dk_min(iDrawHeight, m_iWidth - left);
            grf.FillRect(left, top, left + squareWidth, top + iDrawHeight, ColorManager::knBlack);
            left += squareWidth + 4;
        }
    }

    return S_OK;
}

void UIProgressBar::SetMaximum(INT32 iMax)
{
    if (iMax >= 0)
    {
        m_iMax = iMax;

        if (m_iCurrent > m_iMax)
        {
            m_iCurrent = m_iMax;
        }
    }

    RecalcBarLength();
    //Repaint();
}

INT32 UIProgressBar::GetMaximum()
{
    return m_iMax;
}

void UIProgressBar::OnResize(INT32 iWidth, INT32 iHeight)
{
    UIWindow::OnResize(iWidth, iHeight);
    RecalcBarLength();
}

void UIProgressBar::RecalcBarLength()
{
    m_iBarLengthInPixel = (0 == m_iMax) ? 0 : m_iWidth * m_iCurrent / m_iMax;
}

