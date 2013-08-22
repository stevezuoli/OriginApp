//===========================================================================
// Summary:
//      PDFTextElement.h
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

#ifndef __KINDLEAPP_BOOKREADER_PDFREADER_PDFTEXTELEMENT_H__
#define __KINDLEAPP_BOOKREADER_PDFREADER_PDFTEXTELEMENT_H__

#include <vector>
#include "interface.h"
#include "KernelRetCode.h"
#include "KernelType.h"
#include "DKPTypeDef.h"
#include "IBookReader.h"

struct PDFTextElement
{
    // 元素在文档中位置索引
    DK_FLOWPOSITION m_pos;

    // 元素渲染位置坐标
    double m_xPos;
    double m_yPos;
    DK_BOX m_box;

    // 文本属性
    wchar_t m_ch;
    double m_fontSize;

    PDFTextElement (const DK_FLOWPOSITION& pos, const DK_BOX& box, double xPos = 0.0, double yPos = 0.0,
                    wchar_t ch = 0U, double fontSize = 0.0)
        : m_pos (pos),
          m_xPos (xPos),
          m_yPos (yPos),
          m_box (box),
          m_ch (ch),
          m_fontSize (fontSize)
    {
    }

    PDFTextElement (const PDFTextElement& src)
        : m_pos (src.m_pos),
          m_xPos (src.m_xPos),
          m_yPos (src.m_yPos),
          m_box (src.m_box),
          m_ch (src.m_ch),
          m_fontSize (src.m_fontSize)
    {
    }

    ~PDFTextElement() {}
};

//===========================================================================

class PDFTextIterator;

class PDFTextElementHandler
{
public:
    PDFTextElementHandler();
    ~PDFTextElementHandler();

public:
    typedef std::vector<PDFTextElement>::const_iterator TextElementIter;

public:
    bool IsEmpty() const { return m_textElements.empty(); }
    void AddTextElement(const PDFTextElement& element) { m_textElements.push_back (element); }
    void ClearAll() { m_textElements.clear(); }

    std::wstring GetTextRange(const DK_FLOWPOSITION& startPos, 
                                const DK_FLOWPOSITION& endPos) const;

    DK_ReturnCode GetTextRects(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DK_BOX** ppTextRects, unsigned int* pRectCount); 
    void FreeRects(DK_BOX* pRects) { SafeFreePointerEx(pRects); }
    
    DK_ReturnCode GetSelectionRange(const DK_POS& startPos, const DK_POS& endPos, DKP_SELECTION_MODE selectionMode,  DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos);

    PDFTextIterator* GetTextIterator(bool iterOwnPageElements = false) const;
    void FreeTextIterator(PDFTextIterator* textIterator) const;

    DK_ReturnCode HitTestTextRange(const DK_POS& point, DK_FLOWPOSITION* pStartPos, DK_FLOWPOSITION* pEndPos);

    // must call when add all element of a page
    // add a invisible element to show cursor for page end
    void EndPage(DK_FLOWPOSITION endPos);
    
    TextElementIter GetBeginIterator() const { return m_textElements.begin(); }
    TextElementIter GetEndIterator() const { return m_textElements.end(); }
    void SetModeController(const PdfModeController& modeController) { m_stModeController = modeController; }

private:
    bool DetectPointInElement(const int i, const DK_POS& pos, int& startElmentIndex, int& endElementIndex, int& candidateIndex);
    void SelectFromCandidateIndex(int& selectedIndex, int& candidateStartIndex, int& candidateEndIndex);
    std::vector<PDFTextElement> m_textElements;
    PdfModeController m_stModeController;
};

#endif // #ifndef __KINDLEAPP_BOOKREADER_PDFREADER_PDFTEXTELEMENT_H__
