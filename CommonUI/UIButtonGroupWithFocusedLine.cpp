#include "CommonUI/UIButtonGroupWithFocusedLine.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"

UIButtonGroupWithFocusedLine::UIButtonGroupWithFocusedLine(UIContainer* pParent)
    : UIButtonGroup(pParent)
{
}

UIButtonGroupWithFocusedLine::~UIButtonGroupWithFocusedLine()
{
}

HRESULT UIButtonGroupWithFocusedLine::DrawFocusedSymbol(DK_IMAGE& image)
{
    HRESULT hr(S_OK);
    UISizerItem* focusedItem = m_pBtnListSizer->GetItem(m_currentFocusedIndex);
    if (focusedItem)
    {
        CTpGraphics grfButton(image);
        hr = grfButton.DrawLine(focusedItem->GetPosition().x, 0, focusedItem->GetSize().GetWidth(), GetWindowMetrics(UIBottomBarFocusedLinePixelIndex), SOLID_STROKE);
    }

    return hr;
}

