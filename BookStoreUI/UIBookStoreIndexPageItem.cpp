#include <tr1/functional>
#include "BookStoreUI/UIBookStoreIndexPageItem.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Framework/CDisplay.h"
#include "CommonUI/UIUtility.h"

using namespace WindowsMetrics;

UIBookStoreIndexPageItem::UIBookStoreIndexPageItem()
    : UIListItemBase(NULL, IDSTATIC)
    , m_popNum(this)
{
    Init();
}

UIBookStoreIndexPageItem::~UIBookStoreIndexPageItem()
{

}

void UIBookStoreIndexPageItem::Init()
{
    SetTopMargin(GetWindowMetrics(UIDKBookStoreIndexPageItemTopMarginIndex));
    SetBackgroundColor(ColorManager::knWhite);

    m_titleTextLine.SetFontSize(GetWindowFontSize(FontSize24Index));
    AppendChild(&m_titleTextLine);
    
    m_abstructLine.SetFontSize(GetWindowFontSize(FontSize16Index));
    AppendChild(&m_abstructLine);

    SPtr<ITpImage> arrow = ImageManager::GetImage(IMAGE_ARROW_STRONG_RIGHT);
    m_arrow.SetImage(arrow);
    AppendChild(&m_arrow);

    m_popNum.SetVisible(false);
    AppendChild(&m_popNum);
}

HRESULT UIBookStoreIndexPageItem::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
    RTN_HR_IF_FAILED(grf.EraserBackGround());
#ifndef KINDLE_FOR_TOUCH
    if (IsFocus())
    {
        RTN_HR_IF_FAILED(grf.DrawBorder(0, 0, 0, 0));
        return hr;
    }
#endif
    const int lineLeft = GetWindowMetrics(UIDKBookStoreIndexPageLineLeftMarginIndex);
    const int lineBorder = GetWindowMetrics(UIIndexPageItemLineWidthIndex);
    const int topMargin = m_titleTextLine.GetY();
    const int height = m_abstructLine.GetY() + m_abstructLine.GetHeight() - topMargin;
    RTN_HR_IF_FAILED(grf.DrawLine(lineLeft, topMargin, lineBorder, height, SOLID_STROKE));
    return hr;
}

void UIBookStoreIndexPageItem::InitItem(const DWORD dwId, const std::string& title, const std::string& abstruct)
{
    SetId(dwId);
    m_titleTextLine.SetText(title.c_str());
    m_abstructLine.SetText(abstruct.c_str());

    const int leftMargin = GetWindowMetrics(UIDKBookStoreIndexPageTextLeftMarginIndex);
    const int rightMargin = GetWindowMetrics(UIDKBookStoreIndexPageItemRightMarginIndex);

    UISizer* pMainSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* pLeftSizer = new UIBoxSizer(dkVERTICAL);
    UISizer* pTitleSizer = new UIBoxSizer(dkHORIZONTAL);
    if (pMainSizer && pLeftSizer && pTitleSizer)
    {
        const int titleSizerHeight = GetWindowMetrics(UIPixelValue40Index);
        pTitleSizer->SetMinHeight(titleSizerHeight);
        pTitleSizer->Add(&m_titleTextLine, UISizerFlags().Align(dkALIGN_BOTTOM));
        pTitleSizer->Add(&m_popNum, UISizerFlags().Align(dkALIGN_TOP).Border(dkLEFT, GetWindowMetrics(UIPixelValue10Index)).ReserveSpaceEvenIfHidden());

        const int elemSpacing = GetWindowMetrics(UIPixelValue8Index);
        pLeftSizer->Add(pTitleSizer, UISizerFlags().Expand().Border(dkBOTTOM, elemSpacing));
        pLeftSizer->Add(&m_abstructLine, UISizerFlags().Align(dkALIGN_BOTTOM).Border(dkTOP, elemSpacing));

#ifdef KINDLE_FOR_TOUCH
        const int verticalMargin = m_iTopMargin - GetWindowMetrics(UIPixelValue15Index);
        pMainSizer->Add(pLeftSizer, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, leftMargin).Border(dkTOP, verticalMargin).Border(dkBOTTOM, verticalMargin - GetWindowMetrics(UIPixelValue3Index)));
        pMainSizer->Add(&m_arrow, UISizerFlags().Border(dkRIGHT , rightMargin).Border(dkTOP, GetWindowMetrics(UIPixelValue8Index)).Align(dkALIGN_CENTER_VERTICAL));
#else
        const int verticalMargin = m_iTopMargin - GetWindowMetrics(UIPixelValue8Index);
        pMainSizer->Add(pLeftSizer, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, leftMargin).Border(dkTOP, verticalMargin).Border(dkBOTTOM, verticalMargin - GetWindowMetrics(UIPixelValue3Index)));
        pMainSizer->Add(&m_arrow, UISizerFlags().Border(dkRIGHT , rightMargin).Align(dkALIGN_CENTER_VERTICAL));
#endif

        SetSizer(pMainSizer);
    }
    else
    {
        SafeDeletePointer(pMainSizer);
        SafeDeletePointer(pTitleSizer);
        SafeDeletePointer(pLeftSizer);
    }
}

BOOL UIBookStoreIndexPageItem::OnKeyPress(INT32 iKeyCode)
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

void UIBookStoreIndexPageItem::SetUnreadCount(int count)
{
    m_popNum.SetNum(count);
    CDisplay::CacheDisabler cache;
    m_popNum.UpdateWindow();
}

#ifdef KINDLE_FOR_TOUCH
void UIBookStoreIndexPageItem::PerformClick(MoveEvent* moveEvent)
{
    UIContainer* pParent = GetParent();
    if(IsVisible() && pParent)
    {
    	pParent->OnEvent(UIEvent(COMMAND_EVENT, this,GetId()));
    }
    UIWindow::PerformClick(moveEvent);
}
#endif
