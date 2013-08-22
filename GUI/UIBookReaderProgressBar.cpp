////////////////////////////////////////////////////////////////////////
// UIBookReaderProgressBar.cpp
// Contact: 
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UIBookReaderProgressBar.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ColorManager.h"
#include "GUI/FontManager.h"
#include "Common/SystemSetting_DK.h"
#include "drivers/DeviceInfo.h"
#include "Common/WindowsMetrics.h"
#include "Utility/Misc.h"
using namespace WindowsMetrics;

UIBookReaderProgressBar::UIBookReaderProgressBar() :
    UIProgressBar()
    , m_bIsDisposed(FALSE)
    , m_startPos(0)
    , m_eDirection(PBD_LTR_Horizontal)
    , m_eProgressBarMode(NormalMode)
    , m_iProgressBarColor(0x999999)
    , m_refreshCallBack(NULL)
    , m_callBackParm(NULL)
    , m_scrollable(false)
    , m_transparent(true)
{
    Init();
}

UIBookReaderProgressBar::UIBookReaderProgressBar(UIContainer* pParent, const DWORD dwId) :
    UIProgressBar(pParent, dwId)
    , m_bIsDisposed(FALSE)
    , m_startPos(0)
    , m_eDirection(PBD_LTR_Horizontal)
    , m_eProgressBarMode(NormalMode)
    , m_iProgressBarColor(0x999999)
    , m_refreshCallBack(NULL)
    , m_callBackParm(NULL)
    , m_scrollable(false)
    , m_transparent(true)
{
    Init();
}

UIBookReaderProgressBar::~UIBookReaderProgressBar()
{
    Dispose();
}

void UIBookReaderProgressBar::Init()
{
	HookTouch();
	m_gestureListener.SetProgressBar(this);
    m_gestureDetector.SetListener(&m_gestureListener);
	m_gestureDetector.EnableLongPress(true);
    m_gestureDetector.EnableHolding(true);
	
    m_bIsDisposed = FALSE;
    SetTopMargin(GetWindowMetrics(UIProgressBarTopMarginIndex));
    m_iDistance = GetWindowMetrics(UIProgressBarDistanceIndex);
    m_iBorder = GetWindowMetrics(UIProgressBarBorderIndex);
    m_iLineHeight = GetWindowMetrics(UIProgressBarLineHeightIndex);
	m_iBarHeightInPixel = m_iDistance*2 + m_iBorder*2 + m_iLineHeight;

	DKFontAttributes fontattr;
	m_progress.SetFontSize(GetWindowFontSize(FontSize16Index));
	m_progress.SetEraseBackground(false);
	m_progress.SetEnglishGothic();
}

void UIBookReaderProgressBar::SetStartPos(int startPos)
{
    startPos = (startPos < 0) ? 0 : ((startPos > m_iCurrent) ? m_iCurrent : startPos);
    m_startPos = startPos;
}

void UIBookReaderProgressBar::SetBarHeight(int barHeight)
{
	m_iLineHeight = barHeight;
	m_iBarHeightInPixel = m_iDistance*2 + m_iBorder*2 + m_iLineHeight;
}

HRESULT UIBookReaderProgressBar::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)  //UIWindow    attribute
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
	if(!m_transparent)
	{
		grf.EraserBackGround();
	}
    int totalLength = m_iWidth;
    if (m_eDirection == PBD_Vertical)
    {
        totalLength = m_iHeight;
    }
    int _iBarLength = totalLength;

    if(NormalMode == m_eProgressBarMode)
    {
        int leftMargin = m_iLeftMargin + m_iDistance;
        int topMargin = m_iTopMargin + m_iDistance;
        int rightMargin =  leftMargin;
        int bottomMargin = topMargin;

        if (!m_strProgressText.empty())
        { 
            int stringWidth = m_progress.GetTextWidth();
            _iBarLength -= (stringWidth + 10);
            DK_RECT rect;
            if (m_eDirection == PBD_RTL_Horizontal)
            {
                rect.left = leftMargin;
                rect.right = leftMargin + stringWidth;
                leftMargin += (stringWidth + 10);
            }
            else if (m_eDirection == PBD_LTR_Horizontal)
            {
                rect.left   = m_iLeft + m_iWidth - rightMargin - stringWidth;
                rect.right  = m_iLeft + m_iWidth - rightMargin;
            }
            rect.top    = m_iTop - 5;
            rect.bottom = rect.top + GetWindowFontSize(FontSize16Index) + 2;
			m_progress.MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.top - rect.bottom);
			m_progress.Draw(drawingImg);
        }
        int horizontalWidth = _iBarLength - (rightMargin << 1);
        int horizontalHeight = m_iBorder;
        int verticalHeight = m_iLineHeight + (m_iDistance << 1);
        int verticalWidth = m_iBorder << 1;
        int verticalLeftMargin = m_iLeftMargin;
        int verticalRightMargin = (_iBarLength - rightMargin - 1);
        if (m_eDirection == PBD_RTL_Horizontal)
        {
            verticalLeftMargin = leftMargin;
            verticalRightMargin = m_iWidth - rightMargin - 1;
        }
        if (m_eDirection == PBD_Vertical)
        {
            horizontalWidth = m_iLineHeight + (m_iDistance << 1);
            horizontalHeight = m_iBorder << 1;
            verticalHeight = _iBarLength - topMargin - bottomMargin;
            verticalWidth = m_iBorder;
            rightMargin = topMargin + m_iDistance;
            leftMargin = m_iWidth - rightMargin - horizontalWidth - m_iDistance;
            verticalLeftMargin = leftMargin - m_iBorder;
            verticalRightMargin = (m_iWidth - rightMargin - 1);
        }

        RTN_HR_IF_FAILED(grf.DrawLine(
            leftMargin,
            m_iTopMargin,
            horizontalWidth,
            horizontalHeight,
            SOLID_STROKE));
        RTN_HR_IF_FAILED(grf.DrawLine(
            leftMargin,
            topMargin + verticalHeight,
            horizontalWidth,
            horizontalHeight,
            SOLID_STROKE));
        RTN_HR_IF_FAILED(grf.DrawLine(
            verticalLeftMargin,
            topMargin,
            verticalWidth,
            verticalHeight, 
            SOLID_STROKE));
        RTN_HR_IF_FAILED(grf.DrawLine(
            verticalRightMargin,
            topMargin,
            verticalWidth,
            verticalHeight,
            SOLID_STROKE));

        if (m_eDirection == PBD_Vertical)
        {
            topMargin += m_iDistance + 1;
            _iBarLength -= (topMargin + bottomMargin + (m_iDistance << 1));
        }
        else
        {
            leftMargin += m_iDistance + 1;
            _iBarLength -= ((rightMargin << 1) + (m_iDistance << 1));
        }

        int progressStart = 0;
        int progressEnd = 0;
        if (m_iMax > 0)
        {
            progressStart = ((_iBarLength * m_startPos) / m_iMax);
            progressEnd = ((_iBarLength * m_iCurrent) / m_iMax);
        }

        DebugPrintf(DLC_UIBOOKREADERPROGRESSBAR, "m_startPos = %d, m_iCurrent = %d, m_iMax= %d", m_startPos, m_iCurrent, m_iMax);
        DebugPrintf(DLC_UIBOOKREADERPROGRESSBAR, "progressStart = %d, progressEnd= %d, _iBarLength = %d", progressStart, progressEnd, _iBarLength);
        if (m_eDirection == PBD_Vertical)
        {
            const int progressbar_leftMargin = leftMargin + m_iDistance;
            RTN_HR_IF_FAILED(grf.FillRect(
                progressbar_leftMargin,
                topMargin + progressStart,
                progressbar_leftMargin + m_iLineHeight,
                topMargin + progressEnd,
                m_iProgressBarColor)); 
        }
        else
        {
            const int progressbar_topMargin = topMargin + m_iDistance;
            if (m_eDirection == PBD_RTL_Horizontal)
            {
                RTN_HR_IF_FAILED(grf.FillRect(
                    m_iWidth - rightMargin - progressEnd,
                    progressbar_topMargin,
                    m_iWidth - rightMargin,
                    progressbar_topMargin + m_iLineHeight,
                    m_iProgressBarColor));  
            }
            else
            {
                RTN_HR_IF_FAILED(grf.FillRect(
                    leftMargin + progressStart,
                    progressbar_topMargin,
                    leftMargin + progressEnd,
                    progressbar_topMargin + m_iLineHeight,
                    m_iProgressBarColor));
            }
        }

		
    }
    else
    {
#ifndef KINDLE_FOR_TOUCH
        int iHeight = (DeviceInfo::IsK4Series()) ? 4 : 2;
#else
        int iHeight = (DeviceInfo::IsK4NT()) ? 4 : 2;
#endif
        iHeight = (m_iHeight < iHeight) ? m_iHeight : iHeight;
        DebugPrintf(DLC_UIBOOKREADERPROGRESSBAR, "iHeight = %d, [%d, %d, %d, %d]\n", iHeight, m_iLeft, m_iTop, m_iWidth, m_iHeight);
        if (m_eDirection == PBD_LTR_Horizontal)
        {
            RTN_HR_IF_FAILED(grf.DrawLine(
                0,
                m_iHeight - iHeight,
                (0 == m_iMax) ? 0 : _iBarLength * m_iCurrent / m_iMax,
                iHeight,
                SOLID_STROKE));
        }
        else if (m_eDirection == PBD_RTL_Horizontal)
        {
            int left = (0 == m_iMax) ? 0 : _iBarLength*(m_iMax - m_iCurrent)/m_iMax;
            int right = (0 == m_iMax) ? 0 : _iBarLength;
            RTN_HR_IF_FAILED(grf.DrawLine(
                left,
                m_iHeight - iHeight,
                right,
                iHeight,
                SOLID_STROKE));
        }
    }
    return S_OK;
}

void UIBookReaderProgressBar::SetProgressBarMode(int iMode)
{
    m_eProgressBarMode = ProgressBarMode(iMode);
}

void UIBookReaderProgressBar::SetProgressBarMode(ProgressBarMode iMode)
{
    m_eProgressBarMode = iMode;
}

UIBookReaderProgressBar::ProgressBarMode UIBookReaderProgressBar::GetProgressBarMode() const
{
	return m_eProgressBarMode;
}

void UIBookReaderProgressBar::SetDirection(ProgressBarDirection direction)
{
	m_eDirection = direction;
}

UIBookReaderProgressBar::ProgressBarDirection UIBookReaderProgressBar::GetDirection() const
{
	return m_eDirection;
}

void UIBookReaderProgressBar::SetProgressColor(int color)
{
	m_iProgressBarColor = color;
}

string UIBookReaderProgressBar::GetProgressText() const
{
    return m_strProgressText;
}

void UIBookReaderProgressBar::SetProgressText(const string &_strText)
{
    m_strProgressText = _strText;
	m_progress.SetText(m_strProgressText);
}

void UIBookReaderProgressBar::RecalcBarLength()
{
    m_iBarLengthInPixel = (0 == m_iMax) ? 0 : ((m_iWidth-m_iLeftMargin*2) * m_iCurrent / m_iMax);
}

void UIBookReaderProgressBar::SetCallBackFun(RefreshProgress funcCallBack, void* parm)
{
	m_callBackParm = parm;
	m_refreshCallBack = funcCallBack;
}

void UIBookReaderProgressBar::SetTransparent(bool transparent)
{
	m_transparent = transparent;
}

void UIBookReaderProgressBar::SetEnableScroll(bool scrollable)
{
	m_scrollable = scrollable;
}

void UIBookReaderProgressBar::UIProgressBarGestureListener::OnHolding(int holdingX, int holdingY)
{
    m_pProgressBar->OnHolding(holdingX, holdingY);
}

bool UIBookReaderProgressBar::UIProgressBarGestureListener::OnScroll(MoveEvent* moveEvent1, MoveEvent* moveEvent2, int distanceX, int distanceY)
{
	return m_pProgressBar->OnScroll(moveEvent1, moveEvent2, distanceX, distanceY);
}

bool UIBookReaderProgressBar::UIProgressBarGestureListener::OnDown(MoveEvent* moveEvent)
{
	return m_pProgressBar->OnDown(moveEvent);
}

void UIBookReaderProgressBar::OnHolding(int holdingX, int holdingY)
{
	UpdateLevelByX(holdingX);
	if(m_refreshCallBack && m_callBackParm)
	{
		m_refreshCallBack(m_callBackParm, m_iCurrent*1.0/m_iMax, false);
	}
	return;
}

bool UIBookReaderProgressBar::OnScroll(MoveEvent* moveEvent1, MoveEvent* moveEvent2, int distanceX, int distanceY)
{
	UpdateLevelByX(moveEvent2->GetX());
    return true;
}

bool UIBookReaderProgressBar::OnDown(MoveEvent* moveEvent)
{
	UpdateLevelByX(moveEvent->GetX());
	return true;
}

bool UIBookReaderProgressBar::OnTouchEvent(MoveEvent* moveEvent)
{
	if(!m_scrollable)
	{
		return false;
	}
	if (moveEvent->GetActionMasked() == MoveEvent::ACTION_UP)
    {
		if(m_refreshCallBack && m_callBackParm)
		{
			m_refreshCallBack(m_callBackParm, m_iCurrent*1.0/m_iMax, true);
		}
    }
    return m_gestureDetector.OnTouchEvent(moveEvent);
}

void UIBookReaderProgressBar::UpdateLevelByX(int x)
{
	int totalLength = m_iWidth;
	int leftMargin =  m_iLeftMargin + m_iDistance;
	int horizontalWidth = totalLength - (leftMargin << 1);

	if(x < leftMargin)
	{
		SetProgress(0);
		//RedrawWindow();
        UpdateWindow(PAINT_FLAG_DU, true);
	}
	else if(x > (leftMargin + horizontalWidth))
	{
		SetProgress(m_iMax);
		//RedrawWindow();
        UpdateWindow(PAINT_FLAG_DU, true);
	}
	else
	{
		INT fullBarLength = m_iWidth - (m_progress.GetTextWidth() + 10) - 2*(m_iLeftMargin + m_iDistance);
		int length = x - leftMargin;
		int curProgress = length*m_iMax / fullBarLength;
		double percent = m_iCurrent > curProgress ? m_iCurrent*1.0/curProgress : curProgress*1.0/m_iCurrent;
		if(percent > 1.08)
		{
			SetProgress(curProgress);
			//RedrawWindow();
            UpdateWindow(PAINT_FLAG_DU, true);
		}
	}
	
}

