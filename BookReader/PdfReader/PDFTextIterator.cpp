//===========================================================================
// Summary:
//      PDFTextIterator.cpp
//      PDFReader在原版式页面的文字对象迭代器实现
// Usage:
//      Null
// Remarks:
//      PDFReader中实现了分栏、横屏、切边等多种模式，因此文字迭代器需要在应用层实现
// Date:
//      2012-03-22
// Author:
//      Liu Hongjia (liuhj@duokan.com)
//===========================================================================

#include "BookReader/PDFTextIterator.h"
#include "KernelBaseType.h"

static bool IsValidBox(const DK_BOX* pBox)
{
    return pBox && pBox->X0 != pBox->X1 && pBox->Y0 != pBox->Y1
        && pBox->X0 >= 0 && pBox->X1 >= 0 && pBox->Y0 >= 0 && pBox->Y1 >= 0;
}

PDFTextIterator::PDFTextIterator(const std::vector<PDFTextElement>& pageElements, const PdfModeController& modeController, bool shouldOwn/* = false*/)
        : m_pPageElements(shouldOwn ? new std::vector<PDFTextElement>(pageElements) : NULL)
        , m_pageElements(shouldOwn ? *m_pPageElements : pageElements)
        , m_stModeController(modeController)
{
    m_cursorIndex = -1;
}

bool PDFTextIterator::MoveToNext()
{
    DebugPrintf(DLC_TRACE, "PDFTextIterator::MoveToNext m_cursorIndex = %d", m_cursorIndex);
    if (0 == m_pageElements.size() || m_cursorIndex == (int)m_pageElements.size() - 1)
    {
        return false;
    }
    int cursorIndex = m_cursorIndex + 1;
    while (IsCursorPosOK(cursorIndex) && !IsCursorPosShowOK(cursorIndex))
    {
        ++cursorIndex;
    }
    if (IsCursorPosOK(cursorIndex))
    {
        m_cursorIndex = cursorIndex;
        return true;
    }
    // 刚初始化状态MoveToNext失败,说明一页全空白,停留在页尾
    if (-1 == m_cursorIndex)
    {
        m_cursorIndex = m_pageElements.size() - 1;
        return true;
    }
    return false;
}

bool PDFTextIterator::MoveToPrev()
{
    DebugPrintf(DLC_TRACE, "PDFTextIterator::MoveToPrev m_cursorIndex = %d", m_cursorIndex);
    if (0 == m_pageElements.size() || m_cursorIndex == 0)
    {
        return false;
    }
    int cursorIndex = m_cursorIndex - 1;
    while (IsCursorPosOK(cursorIndex) && !IsCursorPosShowOK(cursorIndex))
    {
        --cursorIndex;
    }
    if (IsCursorPosOK(cursorIndex))
    {
        m_cursorIndex = cursorIndex;
        return true;
    }

    return false;
    // TODO: 这里需要确认一下invisible元素的boundary是多大
}

bool PDFTextIterator::MoveToPrevAdjacentLine()
{
    DebugPrintf(DLC_TRACE, "PDFTextIterator::MoveToPrevAdjacentLine m_cursorIndex = %d", m_cursorIndex);
    return MoveToAdjacentLine(-1);
}

bool PDFTextIterator::MoveToNextAdjacentLine()
{
    DebugPrintf(DLC_TRACE, "PDFTextIterator::MoveToNextAdjacentLine m_cursorIndex = %d", m_cursorIndex);
    return MoveToAdjacentLine(1);
}

DK_ReturnCode PDFTextIterator::GetCurrentCharInfo(DKP_CHAR_INFO* pCharInfo) const
{
    assert(pCharInfo);
    if (!IsCursorPosOK(m_cursorIndex) || NULL == pCharInfo)
    {
        return DKR_FAILED;
    }

    pCharInfo->charCode = m_pageElements[m_cursorIndex].m_ch;
    pCharInfo->boundingBox = m_pageElements[m_cursorIndex].m_box;
//     DebugPrintf (DLC_TRACE, "PDFTextIterator::GetCurrentCharInfo charCode %d, box (%f, %f), (%f, %f)", pCharInfo->charCode, pCharInfo->boundingBox.X0, pCharInfo->boundingBox.Y0, pCharInfo->boundingBox.X1, pCharInfo->boundingBox.Y1);
    pCharInfo->pos = m_pageElements[m_cursorIndex].m_pos;
    return DKR_OK;
}

bool PDFTextIterator::MoveToAdjacentLine(int dir)
{
    if (!IsCursorPosOK(m_cursorIndex))
    {
        return false;
    }

    unsigned int cursorIndex = m_cursorIndex;
    double currentCursorX = m_pageElements[m_cursorIndex].m_box.X0;
    double currentCursorY = m_pageElements[m_cursorIndex].m_box.Y0;
    bool rotationVertical = m_stModeController.m_iRotation % 180;
    if (rotationVertical)
    {
        double currentBaseLineX = m_pageElements[m_cursorIndex].m_xPos;
        while (IsCursorPosOK(cursorIndex) && DkFloatEqual(m_pageElements[cursorIndex].m_xPos, currentBaseLineX))
        {
            cursorIndex += dir;
        }
    }
    else
    {
        double currentBaseLineY = m_pageElements[m_cursorIndex].m_yPos;
        while (IsCursorPosOK(cursorIndex) && DkFloatEqual(m_pageElements[cursorIndex].m_yPos, currentBaseLineY))
        {
            cursorIndex += dir;
        }
    }

    while (IsCursorPosOK(cursorIndex) && !IsCursorPosShowOK(cursorIndex))
    {
        cursorIndex += dir;
    }

    if (IsCursorPosOK(cursorIndex))
    {
        unsigned int bestIndex = cursorIndex;
        if (rotationVertical)
        {
            double adjacentBaseLineX = m_pageElements[cursorIndex].m_xPos;
            double minCursorDistance = fabs(currentCursorY - m_pageElements[cursorIndex].m_box.Y0);
            cursorIndex += dir;

            // 找到最佳匹配光标(以光标x距离最小为判据)
            while (IsCursorPosOK(cursorIndex) && DkFloatEqual(m_pageElements[cursorIndex].m_xPos, adjacentBaseLineX))
            {
                if (IsCursorPosShowOK(cursorIndex))
                {
                    double cursorDistance = fabs(currentCursorY - m_pageElements[cursorIndex].m_box.Y0);
                    if (cursorDistance < minCursorDistance)
                    {
                        minCursorDistance = cursorDistance;
                        bestIndex = cursorIndex;
                    }
                    else
                    {
                        break;
                    }
                }
                cursorIndex += dir;
            }
        }
        else
        {
            double adjacentBaseLineY = m_pageElements[cursorIndex].m_yPos;
            double minCursorDistance = fabs(currentCursorX - m_pageElements[cursorIndex].m_box.X0);
            cursorIndex += dir;

            // 找到最佳匹配光标(以光标x距离最小为判据)
            while (IsCursorPosOK(cursorIndex) && DkFloatEqual(m_pageElements[cursorIndex].m_yPos, adjacentBaseLineY))
            {
                if (IsCursorPosShowOK(cursorIndex))
                {
                    double cursorDistance = fabs(currentCursorX - m_pageElements[cursorIndex].m_box.X0);
                    if (cursorDistance < minCursorDistance)
                    {
                        minCursorDistance = cursorDistance;
                        bestIndex = cursorIndex;
                    }
                    else
                    {
                        break;
                    }
                }
                cursorIndex += dir;
            }
        }
        m_cursorIndex = bestIndex;
        return true;
    }
    return false;
}

bool PDFTextIterator::IsCursorPosOK(int cursorIndex) const
{
    return cursorIndex >= 0 && cursorIndex < (int)m_pageElements.size();
}

bool PDFTextIterator::IsCursorPosShowOK(int cursorIndex) const
{
    return IsValidBox(&(m_pageElements[cursorIndex].m_box))
        || (cursorIndex > 0 && IsValidBox(&(m_pageElements[cursorIndex - 1].m_box)));
}
