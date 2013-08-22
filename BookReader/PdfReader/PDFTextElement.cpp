//===========================================================================
// Summary:
//      PDFTextElement.cpp
//      供 PDFReader 使用的原版式 PDF 页面文本元素的管理类
// Usage:
//      Null
// Remarks:
//      PDFReader 中实现了分栏、横屏、切边等多种模式，因此文字迭代器需要在应用层实现
// Date:
//      2012-03-22
// Author:
//      Liu Hongjia(liuhj@duokan.com)
//===========================================================================

#include "BookReader/PDFTextElement.h"
#include "BookReader/PDFTextIterator.h"
#include "Common/LaddishChar.h"

PDFTextElementHandler::PDFTextElementHandler ()
{
    // do nothing
}

PDFTextElementHandler::~PDFTextElementHandler ()
{
    ClearAll();
}

void PDFTextElementHandler::EndPage (DK_FLOWPOSITION endPos)
{
    if (IsEmpty())
    {
        return;
    }

    for(unsigned int i(1); i < m_textElements.size(); ++i)
    {
        if (0x0a == m_textElements[i].m_ch || 0x0d == m_textElements[i].m_ch)
        {
            m_textElements[i].m_box.X0 = m_textElements[i - 1].m_box.X1;
            m_textElements[i].m_box.X1 = m_textElements[i].m_box.X0;
            m_textElements[i].m_box.Y0 = m_textElements[i - 1].m_box.Y0;
            m_textElements[i].m_box.Y1 = m_textElements[i - 1].m_box.Y1;
        }
    }

    PDFTextElement endElement(m_textElements.back());
    endElement.m_ch = 0;
    endElement.m_pos = endPos;
    endElement.m_box.X0 = endElement.m_box.X1;
    ++endElement.m_box.X1;

    // 最后加一个不可显的字符用于显示光标.
    AddTextElement(endElement);
}

std::wstring PDFTextElementHandler::GetTextRange(const DK_FLOWPOSITION& startPos, 
                                                const DK_FLOWPOSITION& endPos) const
{
    if (startPos >= endPos || startPos > m_textElements.at(m_textElements.size() - 1).m_pos ||
        endPos <= m_textElements.at(0).m_pos)
    {
        return std::wstring();
    }

    std::wstring resultVec;
    TextElementIter itBegin(m_textElements.begin());
    TextElementIter itEnd(m_textElements.end());

    while(itBegin != itEnd)
    {
        if ((*itBegin).m_pos >= endPos)
        {
            break;
        }
        else if ((*itBegin).m_pos >= startPos)
        {
            resultVec += (*itBegin).m_ch;
        }
        ++itBegin;
    }

    if (0 == resultVec.size())
    {
        return std::wstring();
    }

    return resultVec;
}

DK_ReturnCode PDFTextElementHandler::GetTextRects(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DK_BOX** ppTextRects, unsigned int* pRectCount)
{
    //assert(startPos < endPos);
    if (startPos >= endPos)
    {
        return DKR_INVALIDINPARAM;
    }

    unsigned int startElementIndex = 0;
    while (startElementIndex < m_textElements.size() && m_textElements[startElementIndex].m_pos < startPos)
    {
        ++startElementIndex;
    }
    if (startElementIndex >= m_textElements.size())
    {
        assert(false);
        return DKR_INVALIDINPARAM;
    }

    std::vector<DK_BOX> results;
    bool rotationVertical = m_stModeController.m_iRotation % 180;
    while (startElementIndex < m_textElements.size())
    {
        // 每个循环处理基线对齐的一行
        DK_BOX box = m_textElements[startElementIndex].m_box;
        double currentBaseY = m_textElements[startElementIndex].m_yPos;
        double currentBaseX = m_textElements[startElementIndex].m_xPos;

        ++startElementIndex;

        while (startElementIndex < m_textElements.size()
            && m_textElements[startElementIndex].m_pos < endPos
            && ((!rotationVertical && DkFloatEqual(m_textElements[startElementIndex].m_yPos, currentBaseY))
               || (rotationVertical && DkFloatEqual(m_textElements[startElementIndex].m_xPos, currentBaseX))))
        {
            // 忽略空矩形
            if (!m_textElements[startElementIndex].m_box.IsEmpty())
            {
                box = box.MergeBox(m_textElements[startElementIndex].m_box);
            }
            ++startElementIndex;
        }
        results.push_back(box);

        if (startElementIndex >= m_textElements.size() || m_textElements[startElementIndex].m_pos >= endPos)
        {
            break;
        }
    }

    if (0 == results.size())
    {
        *ppTextRects = NULL;
        *pRectCount = 0;
    }
    else
    {
        *ppTextRects = DK_MALLOC_OBJ_N(DK_BOX, results.size());
        if (NULL == *ppTextRects)
        {
            return DKR_ALLOCTMEMORY_FAIL;
        }
        std::copy(results.begin(), results.end(), *ppTextRects);
        *pRectCount = results.size();
    }

    return DKR_OK;
}

PDFTextIterator* PDFTextElementHandler::GetTextIterator(bool iterOwnPageElements/* = false*/) const
{
    return new PDFTextIterator(m_textElements, m_stModeController, iterOwnPageElements);
}

void PDFTextElementHandler::FreeTextIterator(PDFTextIterator* textIterator) const
{
    SafeDeletePointer(textIterator);
}

static bool IsSameKind(const PDFTextElement& lhs, const PDFTextElement& rhs)
{
    if (LaddishCharUtil::IsAlphaNum(lhs.m_ch))
    {
        return LaddishCharUtil::IsAlphaNum(rhs.m_ch);
    }

    //if (LaddishCharUtil::IsHanCharacter(lhs.m_ch))
    //{
        //return LaddishCharUtil::IsHanCharacter(rhs.m_ch);
    //}

    return false;
}

DK_ReturnCode PDFTextElementHandler::HitTestTextRange(const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
{
    assert(NULL != pStartPos);
    assert(NULL != pEndPos);
    if (NULL == pStartPos || NULL == pEndPos)
    {
        return DKR_FAILED;
    }

    unsigned int i(0);
    unsigned int elementsCounts = m_textElements.size();
    int candidateIndex = -1;
    for (; i < elementsCounts; ++i)
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "elementBox: %f,%f,%f,%f", m_textElements[i].m_box.X0, m_textElements[i].m_box.Y0, m_textElements[i].m_box.X1, m_textElements[i].m_box.Y1);
        if (point.Y <= m_textElements[i].m_box.Y1 && point.Y >= m_textElements[i].m_box.Y0)
        {
            if (point.X <= m_textElements[i].m_box.X1 && point.X >= m_textElements[i].m_box.X0)
            {
                break;
            }
            else
            {
                if ((candidateIndex == -1) || (fabs(point.X - m_textElements[i].m_box.X0) < fabs(point.X - m_textElements[candidateIndex].m_box.X0)))
                {
                    candidateIndex = i;
                }
            }
        }
        if (m_textElements[i].m_box.PosInBox(point))
        {
            break;
        }
    }

    //DebugPrintf(DLC_DIAGNOSTIC, "size: %d, i: %d, candidateIndex: %d", m_textElements.size(), i, candidateIndex);
    if ((i >= elementsCounts))
    {
        if (candidateIndex == -1)
        {
            return DKR_FAILED;
        }
        else
        {
            i = candidateIndex;
            wchar_t ch = m_textElements[i].m_ch;
            while (!(LaddishCharUtil::IsAlphaNum(ch) || LaddishCharUtil::IsHanCharacter(ch)))
            {
                i++;
                if (i >= elementsCounts)
                {
                    return DKR_FAILED;
                }
                ch = m_textElements[i].m_ch;
            }
        }
    }

    unsigned int current(i); 
    while (current > 0 && IsSameKind(m_textElements[i], m_textElements[current - 1]))
    {
        --current;
    }

    unsigned int begin(current);

    current = i + 1;
    while (current + 1 < m_textElements.size() && IsSameKind(m_textElements[i], m_textElements[current]))
    {
        ++current;
    }

    unsigned int end = current;
    *pStartPos = m_textElements[begin].m_pos;
    *pEndPos = m_textElements[end].m_pos;
    return DKR_OK;
}

DK_ReturnCode PDFTextElementHandler::GetSelectionRange(const DK_POS& startPoint, const DK_POS& endPoint, DKP_SELECTION_MODE selectionMode,  DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "###### PDFTextElementHandler::GetSelectionRange, startPoint: %f,%f, endPoint: %f,%f", startPoint.X, startPoint.Y, endPoint.X, endPoint.Y);
    if (!pStartPos || !pEndPos)
    {
        return DKR_INVALIDINPARAM;
    }

    int startElemIndex = -1;
    int endElemIndex = -1;
    int candidateElemIndexOnStartPoint = -1;
    int candidateElemIndexOnEndPoint = -1;
    int elementsCount = m_textElements.size();

    for (int i = 0; i < elementsCount; ++i)
    {
        if (DetectPointInElement(i, startPoint, startElemIndex, endElemIndex, candidateElemIndexOnStartPoint))
        {
            break;
        }
        if (DetectPointInElement(i, endPoint, startElemIndex, endElemIndex, candidateElemIndexOnEndPoint))
        {
            break;
        }
    }

    if (candidateElemIndexOnStartPoint > candidateElemIndexOnEndPoint)
    {
        dk_swap(candidateElemIndexOnStartPoint, candidateElemIndexOnEndPoint);
    }

    SelectFromCandidateIndex(startElemIndex, candidateElemIndexOnStartPoint, candidateElemIndexOnEndPoint);
    SelectFromCandidateIndex(endElemIndex, candidateElemIndexOnStartPoint, candidateElemIndexOnEndPoint);

    if (startElemIndex > endElemIndex)
    {
        dk_swap(startElemIndex, endElemIndex);
    }

    if (startElemIndex != -1 && endElemIndex != -1)
    {
        wchar_t ch = m_textElements[startElemIndex].m_ch;
        //跳过空格
        while (startElemIndex + 1 < elementsCount && !(LaddishCharUtil::IsAlphaNum(ch) || LaddishCharUtil::IsHanCharacter(ch)))
        {
            ++startElemIndex;
            ch = m_textElements[startElemIndex].m_ch;
        }
        //回溯直到单词开始处
        while (startElemIndex > 0 && LaddishCharUtil::IsAlphaNum(m_textElements[startElemIndex - 1].m_ch))
        {
            --startElemIndex;
        }


        ch = m_textElements[endElemIndex].m_ch;
        //延伸直到单词结尾处
        while (endElemIndex + 1 < elementsCount && LaddishCharUtil::IsAlphaNum(m_textElements[endElemIndex + 1].m_ch))
        {
            ++endElemIndex;
        }

        *pStartPos = m_textElements[startElemIndex].m_pos;
        if (endElemIndex < elementsCount)
        {
            endElemIndex++;
        }
        *pEndPos = m_textElements[endElemIndex].m_pos;
        //DebugPrintf(DLC_DIAGNOSTIC, "PDFTextElementHandler::GetSelectionRange, treturn (%d,%d,%d)->(%d,%d,%d)", 
                //pStartPos->nChapterIndex, pStartPos->nParaIndex, pStartPos->nElemIndex,
                //pEndPos->nChapterIndex, pEndPos->nParaIndex, pEndPos->nElemIndex);
        return DKR_OK;
    }

    return DKR_FAILED;
}

bool PDFTextElementHandler::DetectPointInElement(const int i, const DK_POS& pos, int& startElemIndex, int& endElemIndex, int& candidateIndex)
{
    DK_BOX box = m_textElements[i].m_box;
    switch(m_stModeController.m_iRotation)
    {
    case 90:
    case 270:
        if (pos.X <= box.X1 && pos.X >= box.X0)
        {
            if (pos.Y <= box.Y1 && pos.Y >= box.Y0)
            {
                candidateIndex = -1;
                if (startElemIndex == -1)
                {
                    if (pos.Y < (box.Y1 + box.Y0)/2)
                        startElemIndex = i;
                    else
                        startElemIndex = i + 1;
                }
                else
                {
                    if (pos.Y > (box.Y1 + box.Y0)/2)
                        endElemIndex = i;
                    else
                        endElemIndex = i - 1;
                    return true;
                }
            }
            else
            {
                if (candidateIndex > 0)
                {
                    DK_BOX lastElemBox = m_textElements[candidateIndex].m_box;
                    //同一行之内可能的候选element已经被存储,只保留一个
                    if (fabs(pos.Y - m_textElements[i].m_box.Y1) < fabs(pos.Y - lastElemBox.Y1))
                    {
                        candidateIndex = i;
                    }
                }
                else
                {
                    candidateIndex = i;
                }
            }
        }
        break;
    default:
        if (pos.Y <= box.Y1 && pos.Y >= box.Y0)
        {
            if (pos.X <= box.X1 && pos.X >= box.X0)
            {
                candidateIndex = -1;
                if (startElemIndex == -1)
                {
                    if (pos.X < (box.X1 + box.X0)/2)
                        startElemIndex = i;
                    else
                        startElemIndex = i + 1;
                }
                else
                {
                    if (pos.X > (box.X1 + box.X0)/2)
                        endElemIndex = i;
                    else
                        endElemIndex = i - 1;
                    return true;
                }
            }
            else
            {
                if (candidateIndex > 0)
                {
                    DK_BOX lastElemBox = m_textElements[candidateIndex].m_box;
                    //同一行之内可能的候选element已经被存储,只保留一个
                    if (fabs(pos.X - m_textElements[i].m_box.X0) < fabs(pos.X - lastElemBox.X0))
                    {
                        candidateIndex = i;
                    }
                }
                else
                {
                    candidateIndex = i;
                }
            }
        }
        break;
    }

    return false;
}

void PDFTextElementHandler::SelectFromCandidateIndex(int& selectedIndex, int& candidateStartIndex, int& candidateEndIndex)
{
    if (selectedIndex == -1)
    {
        if (candidateStartIndex > 0)
        {
            selectedIndex = candidateStartIndex;
            candidateStartIndex = -1;
        }
        else if (candidateEndIndex > 0)
        {
            selectedIndex = candidateEndIndex;
            candidateEndIndex = -1;
        }
    }
}

