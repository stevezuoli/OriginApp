#include "GUI/UIListItemBase.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;

UIListItemBase::UIListItemBase()
    : UIContainer()
    , m_bIsHighLight(false)
    , m_showBottomLine(true)
{
	Init();
}

UIListItemBase::UIListItemBase(UICompoundListBox* pParent, const DWORD dwId, BOOL bBubbleNotify)
    : UIContainer(pParent, dwId, bBubbleNotify)
    , m_bIsHighLight(false)
    , m_showBottomLine(true)
{
	Init();
}

UIListItemBase::UIListItemBase(UICompoundListBox* pParent, const DWORD dwId)
    : UIContainer(pParent, dwId)
    , m_bIsHighLight(false)
    , m_showBottomLine(true)
{
	Init();
}

UIListItemBase::~UIListItemBase()
{
}

void UIListItemBase::Init()
{
#ifdef KINDLE_FOR_TOUCH
    SetClickable(true);
#endif
}

void UIListItemBase::SetHighLight(bool highLight, bool updateNow)
{
#ifdef KINDLE_FOR_TOUCH
    if (m_bIsHighLight != highLight)
    {
        m_bIsHighLight = highLight;
        if (updateNow)
        {
            CDisplay::CacheDisabler disableCache;
            UpdateWindow();
        }
    }
#endif
}

void UIListItemBase::SetShowBottomLine(bool showBottomLine)
{
    m_showBottomLine = showBottomLine;
}

bool UIListItemBase::ResponseTouchTap()
{ 
    return true; 
}

HRESULT UIListItemBase::Draw(DK_IMAGE drawingImg)
{
    HRESULT hr = UIContainer::Draw(drawingImg);
    DrawHighlight(drawingImg);
    return hr;
}

void UIListItemBase::DrawHighlight(DK_IMAGE drawingImg)
{
    if (IsPressed() && m_bIsHighLight)
    { 
        DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
        DK_IMAGE imgSelf;
        CropImage(drawingImg, rcSelf, &imgSelf);
        ReverseImage(imgSelf);
    }
}

HRESULT UIListItemBase::DrawBackGround(DK_IMAGE drawingImg)
{
    CTpGraphics grf(drawingImg);
    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(grf.EraserBackGround());
    if (m_showBottomLine)
    {
        int borderLine = GetWindowMetrics(UIBtnBorderLineIndex);
        StrokeStyle style = DOTTED_STROKE;
        if (IsFocus())
        {
            borderLine = GetWindowMetrics(UIDialogBorderLineIndex);
            style = SOLID_STROKE;
        }
        RTN_HR_IF_FAILED(grf.DrawLine(m_iLeftMargin, m_iHeight - borderLine, m_iWidth - (m_iLeftMargin << 1), borderLine,style));
    }
    return hr;
}

HRESULT UIListItemBase::UpdateFocus()
{
    return UpdateWindow();
}

