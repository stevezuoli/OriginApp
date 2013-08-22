#include "BookStoreUI/UIBookStoreChartsPageItem.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"

UIBookStoreChartsPageItem::UIBookStoreChartsPageItem()
{
    InitUI();
}

UIBookStoreChartsPageItem::~UIBookStoreChartsPageItem()
{
}

bool UIBookStoreChartsPageItem::InitUI()
{
    SPtr<ITpImage> arrow = ImageManager::GetImage(IMAGE_ARROW_STRONG_RIGHT);
    m_arrow.SetImage(arrow);
    AppendChild(&m_seperator);
    AppendChild(&m_titleLabel);
    AppendChild(&m_arrow);

    const int fontSize = GetWindowFontSize(UIBookStoreChartsPageItemIndex);
    const int leftPadding = GetWindowMetrics(UIBookStoreChartsPageItemLeftPaddingIndex);
    const int rightPadding = GetWindowMetrics(UIBookStoreChartsPageItemRightPaddingIndex);
    const int titleLeftPadding = GetWindowMetrics(UIBookStoreChartsPageItemTitleLeftPaddingIndex);
    const int lineBorder = GetWindowMetrics(UIBookStoreChartsPageItemLinePixelIndex);

    m_titleLabel.SetFontSize(fontSize);
    m_seperator.SetMinSize(dkSize(1, fontSize));
    m_seperator.SetThickness(lineBorder);

    UISizer* mainSizer = new UIBoxSizer(dkHORIZONTAL);
    if (mainSizer)
    {
        mainSizer->Add(&m_seperator, UISizerFlags().Border(dkLEFT, leftPadding).Center());
        mainSizer->Add(&m_titleLabel, UISizerFlags(1).Border(dkLEFT, titleLeftPadding).Center());
        mainSizer->Add(&m_arrow, UISizerFlags().Center().Border(dkRIGHT, rightPadding));
        SetSizer(mainSizer);

        return true;
    }

    return false;
}

void UIBookStoreChartsPageItem::InitItem(const DWORD dwId, const std::string& title)
{
    SetId(dwId);
    m_titleLabel.SetText(title);
}

BOOL UIBookStoreChartsPageItem::OnKeyPress(INT32 iKeyCode)
{
#ifdef KINDLE_FOR_TOUCH
    return FALSE;
#else
    switch(iKeyCode)
    {
    case KEY_OKAY:
    case KEY_ENTER:
        {
            UIContainer* pParent = GetParent();
            if(pParent && IsVisible() && IsFocus())
            {
                pParent->OnEvent(UIEvent(COMMAND_EVENT, this, GetId()));
            }
        }
        break;
    default:
        break;
    }
    return TRUE;
#endif
}

#ifdef KINDLE_FOR_TOUCH
void UIBookStoreChartsPageItem::PerformClick(MoveEvent* moveEvent)
{
    UIContainer* pParent = GetParent();
    if(IsVisible() && pParent)
    {
        pParent->OnEvent(UIEvent(COMMAND_EVENT, this,GetId()));
    }
    UIWindow::PerformClick(moveEvent);
}
#endif

HRESULT UIBookStoreChartsPageItem::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
#ifndef KINDLE_FOR_TOUCH
    CTpGraphics grf(drawingImg);
    RTN_HR_IF_FAILED(grf.EraserBackGround());
    if (IsFocus())
    {
        UIUtility::DrawBorder(drawingImg, 0, 0, 0, 0);
    }
    else
    {
        const int lineLeft = GetWindowMetrics(UIDKBookStoreIndexPageLineLeftMarginIndex);
        const int lineBorder = GetWindowMetrics(UIBtnBorderLineIndex);
        RTN_HR_IF_FAILED(grf.DrawLine(lineLeft, m_iTopMargin, lineBorder, m_iHeight - (m_iTopMargin << 1), SOLID_STROKE));
    }
#endif
    return hr;
}

