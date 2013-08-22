#include "CommonUI/UIBookListProgressBar.h"
#include "GUI/CTpGraphics.h"

UIBookListProgressBar::UIBookListProgressBar()
    : m_barHeight(2)
    , m_maxDot(0)
    , m_percentageProgress(0)
    , m_dotInterval(4)
    , m_drawBold(false)
{
}

UIBookListProgressBar::UIBookListProgressBar(UIContainer* parent)
    : UIWindow(parent, IDSTATIC)
    , m_barHeight(2)
    , m_maxDot(0)
    , m_percentageProgress(0)
    , m_dotInterval(4)
    , m_drawBold(false)
{
}

void UIBookListProgressBar::SetBarHeight(int barHeight)
{
    m_barHeight = barHeight;
}

int UIBookListProgressBar::GetBarHeight() const
{
    return m_barHeight;
}

void UIBookListProgressBar::SetDotInterval(int dotInterval)
{
    m_dotInterval = dotInterval;
}

int UIBookListProgressBar::GetDotInterval() const
{
    return m_dotInterval;
}

void UIBookListProgressBar::SetMaxDot(int maxDot)
{
    m_maxDot = maxDot;
}

void UIBookListProgressBar::SetPercentageProgess(int percentageProgress)
{
    m_percentageProgress = percentageProgress;
}

void UIBookListProgressBar::SetDrawBold(bool drawBold)
{
    m_drawBold = drawBold;
}

bool UIBookListProgressBar::IsDrawBold() const
{
    return m_drawBold;
}


HRESULT UIBookListProgressBar::Draw(DK_IMAGE drawingImg)
{
    if (!IsVisible())
    {
        return S_OK;
    }

    if (drawingImg.pbData == NULL)
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

    if(m_drawBold)
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
        int barHeight = dk_min(m_barHeight, m_iHeight);
        int dotWidth = barHeight + m_dotInterval;
        int totalDotWidth = dk_min(dotWidth * m_maxDot, m_iWidth);
        int curDot = m_maxDot * m_percentageProgress / 100;
        int solidWidth = dk_min(dotWidth * curDot, m_iWidth);
        if (solidWidth > m_dotInterval / 2)
        {
            solidWidth -= m_dotInterval / 2;
        }
        int drawTop = (m_iHeight - barHeight) / 2;
        grf.FillRect(
            0,
            0,
            m_iWidth,
            m_iHeight,
            ColorManager::knWhite);

        grf.FillRect(0, drawTop, solidWidth, drawTop + barHeight, 
                ColorManager::knBlack);
        int left = solidWidth + m_dotInterval;
        while (left + barHeight <= totalDotWidth)
        {
            grf.FillRect(left, drawTop, left + barHeight, drawTop + barHeight,
                    ColorManager::knBlack);
            left += dotWidth;
        }
    }

    return S_OK;
}
