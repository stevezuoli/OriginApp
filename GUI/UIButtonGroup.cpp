#include "GUI/UIButtonGroup.h"
#include "GUI/UISizer.h"
#include "GUI/UIButton.h"
#include "GUI/CTpGraphics.h"
#include "Common/WindowsMetrics.h"
using namespace WindowsMetrics;

const char*  UIButtonGroup::ButtonClickedChangeEvent = "ButtonClickedChangeEvent";

UIButtonGroup::UIButtonGroup(UIContainer* pParent)
    : UIContainer(pParent)
    , m_exclusive(true)
    , m_displaySpliteLine(true)
    , m_splitLinePixel(1)
    , m_currentFocusedIndex(0)
    , m_elementBorder(-1)
    , m_btnsDirection(BGD_Horizontal)
    , m_pBtnListSizer(NULL)
    , m_pTopLineSizerItem(NULL)
    , m_pBottomLineSizerItem(NULL)
{
    Init();
}

UIButtonGroup::~UIButtonGroup()
{
}

void UIButtonGroup::Init()
{
    m_splitLineHeight = 0;
    m_topLinePixel = GetWindowMetrics(UIButtonGroupTopLinePixelIndex);
    m_bottomLinePixel = GetWindowMetrics(UIButtonGroupBottomLinePixelIndex);

    EnsureSizerExist();
}

void UIButtonGroup::SetFocusedIndex(int index)
{
    if (index >=0 && (unsigned int)index < GetChildrenCount())
    {
        m_currentFocusedIndex = index;
#ifndef KINDLE_FOR_TOUCH
        UIWindow* pWin = GetChildByIndex(index);
        if (pWin)
        {
            pWin->SetFocus(true);
        }
#endif
    }
}

void UIButtonGroup::SetFocusedButton(UIWindow* pWin)
{
    if (pWin)
    {
        SetFocusedIndex(IndexOfWindow(pWin));
    }
}

void UIButtonGroup::AddButton(UIButton* button)
{
    if (!EnsureSizerExist() || NULL == button)
    {
        return;
    }
    if (m_elementBorder >= 0)
    {
#ifdef KINDLE_FOR_TOUCH
        DoInsert(m_pBtnListSizer->GetItemCount(), button, UISizerFlags(1).Expand().Center().Border(dkALL, m_elementBorder));
#else
        DoInsert(m_pBtnListSizer->GetItemCount(), button, UISizerFlags(1).Center().Border(dkALL, m_elementBorder));
#endif
    }
    else
    {
#ifdef KINDLE_FOR_TOUCH
        DoInsert(m_pBtnListSizer->GetItemCount(), button, UISizerFlags(1).Expand().Center().Border());
#else
        DoInsert(m_pBtnListSizer->GetItemCount(), button, UISizerFlags(1).Center().Border());
#endif
    }
}

void UIButtonGroup::AddButton(UIButton* button, const UISizerFlags& flag)
{
    if (!EnsureSizerExist() || NULL == button)
    {
        return;
    }
    DoInsert(m_pBtnListSizer->GetItemCount(), button, flag);
}

void UIButtonGroup::RemoveButton(UIButton* button)
{
    if (!EnsureSizerExist() || NULL == button)
    {
        return;
    }

    RemoveChild(button, false);
    m_pBtnListSizer->Remove(m_pBtnListSizer->IndexOf((UIWindow*)button));
}

UIButton* UIButtonGroup::GetButtonByCommandId(DWORD commandId)
{
    for (unsigned i=0; i<GetChildrenCount(); ++i)
    {
        UIButton* button = (UIButton*)GetChildByIndex(i);
        if (button->GetId() == commandId)
        {
            return button;
        }
    }

    return NULL;
}

void UIButtonGroup::AddSpace(const int pixel)
{
    if (!EnsureSizerExist())
    {
        return;
    }

    m_pBtnListSizer->AddSpacer(pixel);
}

bool UIButtonGroup::IsExclusive() const
{
    return m_exclusive;
}

void UIButtonGroup::SetExclusive(bool exclusive)
{
    m_exclusive = exclusive;
}

bool UIButtonGroup::EnsureSizerExist()
{
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        if (!mainSizer)
        {
            return false;
        }
        SetSizer(mainSizer);
        dkOrientation sizerDir = (m_btnsDirection == BGD_Horizontal) ? dkHORIZONTAL : dkVERTICAL;
        m_pBtnListSizer = new UIBoxSizer(sizerDir);
        m_pTopLineSizerItem = mainSizer->AddSpacer(m_topLinePixel);
        mainSizer->Add(m_pBtnListSizer, UISizerFlags(1).Expand());
        m_pBottomLineSizerItem = mainSizer->AddSpacer(m_bottomLinePixel);
    }

    return true;
}

void UIButtonGroup::DoInsert(int index, UIButton* button, const UISizerFlags& flag)
{
#ifdef KINDLE_FOR_TOUCH
    if (button)
    {
        button->ShowBorder(false);
    }
#endif

    AppendChild(button);//TODO:deal with the case with insertxxx method 
    m_pBtnListSizer->Insert(index, button, flag);
}

HRESULT UIButtonGroup::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
        return S_OK;
    
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    int left = m_iLeft, top = m_iTop, right = m_iLeft + m_iWidth, bottom = m_iTop + m_iHeight;
    DK_RECT rcSelf={left, top, right, bottom};
    RTN_HR_IF_FAILED(CropImage(
        drawingImg,
        rcSelf,
        &imgSelf
        ));
    CTpGraphics grf(imgSelf);
    ValidChildBtnShown();
    int iSize = GetChildrenCount();
    for (int i = 0; i < iSize; i++)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (pWin && pWin->IsVisible())
        {
            DebugPrintf(DLC_GUI_VERBOSE,
                    "%s: Drawing Child %d / %d : %s, (%d, %d)-(%d, %d), %s ...",
                    GetClassName(), i, iSize, pWin->GetClassName(),
                    pWin->GetX(), pWin->GetY(),
                    pWin->GetWidth(), pWin->GetHeight(),
                    pWin->GetText());
            hr = pWin->Draw(imgSelf);
            if (!SUCCEEDED(hr))
            {
                DebugPrintf(DLC_GUI_VERBOSE, "Draw child failed");
            }
        }
        else
        {
            DebugPrintf(DLC_ERROR, "%s: FAILED to get child %d/%d !!!", GetClassName(), i, iSize);
        }
    }

    if (m_btnsDirection == BGD_Horizontal)
    {
        DrawTopLine(grf);
        DrawBottomLine(grf);
    }
    DrawSplitLine(grf);

    if (m_currentFocusedIndex >= 0 && (unsigned int)m_currentFocusedIndex < GetChildrenCount())
    {
        DrawFocusedSymbol(imgSelf);
    }

    return hr;
}

void UIButtonGroup::SetSplitLineHeight(int height)
{
    if (height > 0)
    {
        m_splitLineHeight = height;
    }
}

void UIButtonGroup::SetSplitLineWidth(int width)
{
    if (width > 0)
    {
        m_splitLinePixel = width;
    }
}

HRESULT UIButtonGroup::DrawTopLine(CTpGraphics& grf)
{
    HRESULT hr(S_OK);

    if (m_topLinePixel > 0)
    {
        hr = grf.DrawLine(0, 0, m_iWidth, m_topLinePixel, SOLID_STROKE);
    }

    return hr;
}

HRESULT UIButtonGroup::DrawBottomLine(CTpGraphics& grf)
{
    HRESULT hr(S_OK);

    if (m_bottomLinePixel > 0)
    {
        hr = grf.DrawLine(0, m_iHeight - m_bottomLinePixel, m_iWidth, m_bottomLinePixel, SOLID_STROKE);
    }

    return hr;
}

HRESULT UIButtonGroup::DrawFocusedSymbol(DK_IMAGE& image)
{
    HRESULT hr(S_OK);
    //Do Nothing in BaseClass
    /*
     *UIButton* button = (UIButton*)GetChildByIndex(m_currentFocusedIndex);
     *if (button)
     *{
     *    DK_IMAGE imageButton;
     *    DK_RECT rectButton = {button->GetX(), button->GetY(), button->GetX() + button->GetWidth(), button->GetY() + button->GetHeight()};
     *    RTN_HR_IF_FAILED(CropImage(
     *                image,
     *                rectButton,
     *                &imageButton
     *                ));
     *    CTpGraphics grfButton(imageButton);
     *    grfButton.DrawLine(0, 0, button->GetWidth(), 10, SOLID_STROKE);
     *}
     */
    return hr;
}

void UIButtonGroup::OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam)
{
    m_currentFocusedIndex = IndexOfWindow(pSender);
    //note:only one method should be used in class which uses this. 
    ButtonClickedEventArgs args(dwCmdId, pSender, dwParam);
    FireEvent(ButtonClickedChangeEvent, args);
    if (m_pParent)
    {
        m_pParent->OnCommand(dwCmdId, pSender, dwParam);
    }
    UpdateWindow();
}

int UIButtonGroup::IndexOfWindow(UIWindow* win)
{
    for ( unsigned int i=0; i<GetChildrenCount(); ++i)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (pWin == win)
        {
            return i;
        }
    }

    return -1;
}

void UIButtonGroup::SetTopLinePixel(int pixel)
{
    m_topLinePixel = pixel;
    if (m_pBtnListSizer && m_windowSizer && m_pTopLineSizerItem)
    {
        m_pTopLineSizerItem->AssignSpacer(0, pixel);
    }
}

void UIButtonGroup::SetBottomLinePixel(int pixel)
{
    m_bottomLinePixel = pixel;
    if (m_pBtnListSizer && m_windowSizer && m_pBottomLineSizerItem)
    {
        m_pBottomLineSizerItem->AssignSpacer(0, pixel);
    }
}

void UIButtonGroup::ValidChildBtnShown()
{
    if (!m_pBtnListSizer)
    {
        return ;
    }
    const size_t btnCount = m_pBtnListSizer->GetItemCount();
    if (m_btnsDirection == BGD_Horizontal)
    {
        for (unsigned int i = 0; i < btnCount; ++i)
        {
            UISizerItem* item = m_pBtnListSizer->GetItem(i);
            if (item && item->IsShown() && (item->GetRect().GetWidth() < item->GetMinSize().GetWidth()))
            {
                item->Show(false);
            }
        }
    }
    else if (m_btnsDirection == BGD_Vertical)
    {
        for (unsigned int i = 0; i < btnCount; ++i)
        {
            UISizerItem* item = m_pBtnListSizer->GetItem(i);
            if (item && item->IsShown() && (item->GetRect().GetHeight() < item->GetMinSize().GetHeight()))
            {
                item->Show(false);
            }
        }
    }
}

HRESULT UIButtonGroup::DrawSplitLine(CTpGraphics& grf)
{
    HRESULT hr(S_OK);
    if (m_btnsDirection == BGD_Horizontal)
    {
        if (m_splitLineHeight > 0 && m_pBtnListSizer && m_displaySpliteLine)
        {
            int splitLineHeight = m_splitLineHeight > m_iHeight ? m_iHeight : m_splitLineHeight;
            int topPadding = (m_iHeight - splitLineHeight) >> 1;
            for (unsigned int i=1; i<m_pBtnListSizer->GetItemCount(); ++i)
            {
                UISizerItem* item = m_pBtnListSizer->GetItem(i);
                if (item && item->IsShown() && (item->GetRect().GetWidth() >= item->GetMinSize().GetWidth()))
                {
                    RTN_HR_IF_FAILED(grf.DrawLine(item->GetPosition().x, topPadding, m_splitLinePixel, splitLineHeight, SOLID_STROKE));
                }
            }
        }
    }
    else if (m_btnsDirection == BGD_Vertical)
    {
        if (m_pBtnListSizer->GetItemCount() > 0)
        {
            int endIndex = m_displaySpliteLine ? m_pBtnListSizer->GetItemCount() : 1;
            for (unsigned int i = 0; i < endIndex; ++i)
            {
                UISizerItem* item = m_pBtnListSizer->GetItem(i);
                if (item && item->IsShown())
                {
                    RTN_HR_IF_FAILED(grf.DrawLine(0, item->GetPosition().y, m_iWidth, m_splitLinePixel, SOLID_STROKE));
                }
            }
        }
    }
    return hr;
}

BOOL UIButtonGroup::OnKeyPress(INT32 iKeyCode)
{
    if (!SendKeyToChildren(iKeyCode))
    {
        return FALSE;
    }

    switch (iKeyCode)
    {
        case KEY_LEFT:
            if (m_btnsDirection == BGD_Horizontal)
            {
                MoveFocus(DIR_LEFT);
            }
            else
            {
                return TRUE;
            }
            break;
        case KEY_RIGHT:
            if (m_btnsDirection == BGD_Horizontal)
            {
                MoveFocus(DIR_RIGHT);
            }
            else
            {
                return TRUE;
            }
            break;
        case KEY_UP:
            if (m_btnsDirection == BGD_Horizontal)
            {
                return TRUE;
            }
            else
            {
                MoveFocus(DIR_TOP);
            }
            break;
        case KEY_DOWN:
            if (m_btnsDirection == BGD_Horizontal)
            {
                return TRUE;
            }
            else
            {
                MoveFocus(DIR_DOWN);
            }
            break;
        default:
            if(SendHotKeyToChildren(iKeyCode))
            {
                return UIWindow::OnKeyPress(iKeyCode);
            }
    }

    return FALSE;
}

void UIButtonGroup::SetBtnsDirection(ButtonGroupDirection dir)
{
    if (m_btnsDirection != dir)
    {
        m_btnsDirection = dir;
        if (m_pBtnListSizer)
        {
            UIBoxSizer* pBtnListSizer = dynamic_cast<UIBoxSizer*>(m_pBtnListSizer);
            if (pBtnListSizer)
            {
                dkOrientation sizerDir = (dir == BGD_Horizontal) ? dkHORIZONTAL : dkVERTICAL;
                pBtnListSizer->SetOrientation(sizerDir);
            }
        }
    }
}

UIButtonGroup::ButtonGroupDirection UIButtonGroup::GetBtnsDirection() const
{
    return m_btnsDirection;
}
