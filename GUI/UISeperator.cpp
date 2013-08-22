#include "GUI/UISeperator.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ColorManager.h"

UISeperator::UISeperator()
    : m_direction(SD_VERT)
    , m_thickness(1)
{
};


HRESULT UISeperator::Draw(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    if (!m_bIsVisible)
    {
        return hr;
    }
    DK_IMAGE imgSelf;
    DK_RECT rcSelf = {m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(drawingImg, rcSelf, &imgSelf));

    CTpGraphics grf(imgSelf);
    if (SD_VERT == m_direction)
    {
        grf.FillRect(m_iWidth / 2, 0,
                m_iWidth / 2 + m_thickness, m_iHeight,
                ColorManager::knBlack);
    }
    else
    {
        grf.FillRect(0, m_iHeight / 2,
                m_iWidth, m_iHeight /2 + m_thickness,
                ColorManager::knBlack);
    }
    return S_OK;
}

dkSize UISeperator::GetMinSize() const
{
    if (dkDefaultCoord == m_minWidth || dkDefaultCoord == m_minHeight)
    {
        return dkSize(1, 1);
    }
    return dkSize(m_minWidth, m_minHeight);
}
