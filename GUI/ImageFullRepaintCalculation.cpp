#include "GUI/ImageFullRepaintCalculation.h"
#include "drivers/DeviceInfo.h"
#include "interface.h"
#include "CommonUI/UIUtility.h"
using namespace std;

SINGLETON_CPP(ImageFullRepaintCalculation)

ImageFullRepaintCalculation::ImageFullRepaintCalculation()
    : m_finalFlag(false)
    , m_threshold(4)
{
}

void ImageFullRepaintCalculation::InitEnvironment(int width, int height, int horinzontalNum, int verticalNum, int threshold)
{
    if (width <= 0 || height <= 0 || horinzontalNum <= 0 || verticalNum <= 0 || threshold <= 0)
    {
        return;
    }

    m_posLst.clear();
    m_posFlag.clear();
    m_finalFlag = false;
    m_threshold = threshold;

    const int hMargin = width / (horinzontalNum << 1);
    const int vMargin = height / (verticalNum << 1);
    const int hDiff = hMargin << 1;
    const int vDiff = vMargin << 1;
    int x = hMargin, y = vMargin;
    for (int i = 0; i < verticalNum; ++i)
    {
        for (int j = 0; j < horinzontalNum; ++j)
        {
            m_posLst.push_back(DK_POS(x, y));
            m_posFlag.push_back(false);
            x += vDiff;
        }
        y += hDiff;
        x = vDiff;
    }
}

ImageFullRepaintCalculation::~ImageFullRepaintCalculation()
{
    m_posLst.clear();
    m_posFlag.clear();
}

bool ImageFullRepaintCalculation::GetFullRepaintFlag()
{
    if (!m_finalFlag)
    {
        int count = 0;
        vector<bool>::iterator flag = m_posFlag.begin();
        while(flag != m_posFlag.end())
        {
            if (*flag)
            {
                count++;
            }
            flag++;
        }
        m_finalFlag = (count >= m_threshold);
    }
    return m_finalFlag;
}

void ImageFullRepaintCalculation::ResetFullRepaintFlag()
{
    m_finalFlag = false;
    vector<bool>::iterator flag = m_posFlag.begin();
    while(flag != m_posFlag.end())
    {
        *flag = false;
        flag++;
    }
}

void ImageFullRepaintCalculation::SetBox(const DK_BOX& box)
{
    for (size_t i = 0; i < m_posLst.size(); ++i)
    {
        const DK_POS& pos = m_posLst[i];
        if (box.PosInBox(pos))
        {
            m_posFlag[i] = true;
        }
    }
}

void ImageFullRepaintCalculation::SetRect(const DK_RECT& rect)
{
    for (int i = 0; i < m_posLst.size(); ++i)
    {
        const DK_POS& pos = m_posLst[i];
        if (UIUtility::PosInRect(rect, pos))
        {
            m_posFlag[i] = true;
        }
    }
}

