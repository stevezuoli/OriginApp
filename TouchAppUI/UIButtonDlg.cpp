#include "TouchAppUI/UIButtonDlg.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "drivers/DeviceInfo.h"
using namespace WindowsMetrics;

UIButtonDlg::UIButtonDlg(UIContainer* _pParent)
    : UIDialog(_pParent)
    , m_iSelectedButton(-1)
    , m_iItemMargin(0)
    , m_iNumPerPage(8)
    , m_iCurPage(-1)
    , m_iTotalPage(-1)
    , m_iTextAlign(ALIGN_LEFT)
    , m_iCurPageStartIndex(-1)
    , m_iCurPageEndIndex(-1)
    , m_pBottomSizer(NULL)
    , m_pButtonSizer(NULL)
    , m_btnFontSize(GetWindowFontSize(UIButtonDlgIndex))
{
    m_iButtonHeight = GetWindowMetrics(UIButtonDlgButtonHeightIndex);
    m_vertSpacingFromButtonAttached = GetWindowMetrics(UIButtonDlgVertSpacingFromAttachedButtonIndex);

    m_btnLeft.Initialize(BI_Left, NULL, 0);
    m_btnLeft.SetIcon(ImageManager::GetImage(IMAGE_LEFT_ARROW), UIButton::ICON_LEFT);
    m_btnLeft.SetAlign(ALIGN_LEFT);
    m_btnLeft.ShowBorder(false);

    m_txtPageNum.SetFontSize(GetWindowFontSize(UIPageNumIndex));
    m_txtPageNum.SetAlign(ALIGN_CENTER);

    m_btnRight.Initialize(BI_Right, NULL, 0);
    m_btnRight.SetIcon(ImageManager::GetImage(IMAGE_RIGHT_ARROW), UIButton::ICON_RIGHT);
    m_btnRight.SetAlign(ALIGN_RIGHT);
    m_btnRight.ShowBorder(false);

    AppendChild(&m_txtPageNum);
    AppendChild(&m_btnLeft);
    AppendChild(&m_btnRight);
    HookTouch();
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        if (mainSizer)
        {
            m_pButtonSizer = new UIBoxSizer(dkVERTICAL);
            if (m_pButtonSizer)
            {
                mainSizer->Add(m_pButtonSizer, UISizerFlags(1).Expand());
            }

            m_pBottomSizer = new UIBoxSizer(dkHORIZONTAL);
            if (m_pBottomSizer)
            {
                m_btnLeft.SetMinWidth(m_iButtonHeight);
                m_btnRight.SetMinWidth(m_iButtonHeight);
                m_btnLeft.SetLeftMargin(0);
                m_btnLeft.SetInternalHorzSpacing(0);
                m_btnLeft.SetAlign(ALIGN_LEFT);
                m_btnRight.SetLeftMargin(0);
                m_btnRight.SetInternalHorzSpacing(0);
                m_btnRight.SetAlign(ALIGN_RIGHT);
                m_pBottomSizer->SetMinHeight(m_iButtonHeight);

                m_pBottomSizer->Add(&m_btnLeft, UISizerFlags().Expand());
                m_pBottomSizer->Add(&m_txtPageNum, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
                m_pBottomSizer->Add(&m_btnRight, UISizerFlags().Expand());
                mainSizer->Add(m_pBottomSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIButtonDlgButtonLeftMarginIndex)));
            }
        }

        SetSizer(mainSizer);
    }
}

UIButtonDlg::~UIButtonDlg()
{
}
    

void UIButtonDlg::MoveWindow(int _iLeft, int _iTop, int _iWidth, int _iHeight)
{
    UIWindow::MoveWindow(_iLeft, _iTop, _iWidth, _iHeight);
    Layout();
}

HRESULT UIButtonDlg::UpdateWindow()
{
    UpdateButtons();
    UpdatePageNum();
    Layout();
    return UIWindow::UpdateWindow();
}

void UIButtonDlg::UpdatePageNum()
{
    char strPageNum[128] = {0};
    sprintf(strPageNum, "%d / %d", m_iCurPage + 1, m_iTotalPage);
    m_txtPageNum.SetText(strPageNum);
}

void UIButtonDlg::OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam)
{

    switch (_dwCmdId)
    {
    case BI_Left:
        {
            m_iCurPage--;
            if (m_iCurPage < 0)
                m_iCurPage = m_iTotalPage - 1;
            UpdateWindow();
        }
        return ;
    case BI_Right:
        {
            m_iCurPage++;
            if (m_iCurPage >= m_iTotalPage)
                m_iCurPage = 0;
            UpdateWindow();
        }
        return ;
    default:
        break;
    }

    UISizerItem* item = m_pButtonSizer->GetItemById(_dwCmdId);
    if (item)
    {
        int itemIndex = m_pButtonSizer->IndexOf(item);
        if (itemIndex>=0 && (unsigned int)itemIndex<m_pButtonSizer->GetItemCount())
        {
            SetSelectedButtonByIndex(itemIndex);
            EndDialog(0);
        }
    }
}

void UIButtonDlg::UpdateButtons()
{
    if (m_iSelectedButton < 0)
    {
        SetSelectedButtonByIndex(0);
    }
    UpdatePageButtonIndexRange();
    if (m_pButtonSizer)
    {
        m_pButtonSizer->Show(false);
    }
    for ( int i = m_iCurPageStartIndex; i <= m_iCurPageEndIndex; ++i)
    {
        UISizerItem* item = m_pButtonSizer->GetItem(i);
        if (item)
        {
            item->Show(true);
        }
    }
}

HRESULT UIButtonDlg::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
    {
        return S_OK;
    }

    if (drawingImg.pbData == NULL)
    {
        return E_POINTER;
    }

    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        drawingImg,
        rcSelf,
        &imgSelf
        ));

    CTpGraphics grf(imgSelf);
    grf.EraserBackGround(ColorManager::knWhite);

    UINT32 iSize = GetChildrenCount();
    for (UINT32 i = 0; i < iSize; i++)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (pWin)
        {
            /*RTN_HR_IF_FAILED*/(pWin->Draw(imgSelf));
        }
        else
        {
            DebugPrintf(DLC_ERROR, "%s: FAILED to get child %d/%d !!!", GetClassName(), i, iSize);
        }
    }

    grf.DrawLine(0, 0, m_iWidth, GetWindowMetrics(UIButtonDlgBorderLineIndex) ,SOLID_STROKE);
    grf.DrawLine(0, m_iHeight - GetWindowMetrics(UIButtonDlgBorderLineIndex), m_iWidth, GetWindowMetrics(UIButtonDlgBorderLineIndex) ,SOLID_STROKE);
    grf.DrawLine(0, 0, GetWindowMetrics(UIButtonDlgBorderLineIndex),  m_iHeight, SOLID_STROKE);
    grf.DrawLine(m_iWidth - GetWindowMetrics(UIButtonDlgBorderLineIndex), 0, GetWindowMetrics(UIButtonDlgBorderLineIndex), m_iHeight ,SOLID_STROKE);

    const bool bUpSelected   = (m_iSelectedButton == m_iCurPageStartIndex);
    SPtr<ITpImage> pUpLeftImage = ImageManager::GetImage(bUpSelected ? IMAGE_TOUCH_MENU_BG_01_BLACK : IMAGE_TOUCH_MENU_BG_01);
    RTN_HR_IF_FAILED(grf.DrawImageBlend(pUpLeftImage.Get(), 0, 0, 0, 0, pUpLeftImage->GetWidth(),  pUpLeftImage->GetHeight()));

    SPtr<ITpImage> pUpRightImage = ImageManager::GetImage(bUpSelected ? IMAGE_TOUCH_MENU_BG_02_BLACK : IMAGE_TOUCH_MENU_BG_02);
    int iUpRightWidth = pUpRightImage->GetWidth();
    RTN_HR_IF_FAILED(grf.DrawImageBlend(pUpRightImage.Get(), m_iWidth - iUpRightWidth, 0, 0, 0, iUpRightWidth,  pUpRightImage->GetHeight()));

    const bool bDownSelected = (m_iTotalPage <= 1) && (m_iSelectedButton == m_iCurPageEndIndex);
    SPtr<ITpImage> pRightDownImage = ImageManager::GetImage(bDownSelected ? IMAGE_TOUCH_MENU_BG_03_BLACK : IMAGE_TOUCH_MENU_BG_03);
    int iRightDownWidth  = pRightDownImage->GetWidth();
    int iRightDownHeight = pRightDownImage->GetHeight();
    RTN_HR_IF_FAILED(grf.DrawImageBlend(pRightDownImage.Get(), m_iWidth - iRightDownWidth, m_iHeight - iRightDownHeight, 0, 0, iRightDownWidth,  iRightDownHeight));

    SPtr<ITpImage> pLeftDownImage = ImageManager::GetImage(bDownSelected ? IMAGE_TOUCH_MENU_BG_04_BLACK : IMAGE_TOUCH_MENU_BG_04);
    int iLeftDownWidth  = pLeftDownImage->GetWidth();
    int iLeftDownHeight = pLeftDownImage->GetHeight();
    RTN_HR_IF_FAILED(grf.DrawImageBlend(pLeftDownImage.Get(), 0, m_iHeight - iLeftDownHeight, 0, 0, iLeftDownWidth,  iLeftDownHeight));

    return hr;
}

bool UIButtonDlg::AppendButton(DWORD _dwId, LPCSTR _szText, INT32 _iFontSize, SPtr<ITpImage> Icon)
{
    UITouchComplexButton* pButton = new UITouchComplexButton();
    if (!pButton)
    {
        return false;
    }
    pButton->Initialize(_dwId, _szText, _iFontSize);
    pButton->SetLeftMargin(GetWindowMetrics(UIButtonDlgButtonLeftMarginIndex));
    pButton->ShowBorder(false);
    pButton->SetAlign(m_iTextAlign);
    AppendChild(pButton);
    if (m_pButtonSizer)
    {
        pButton->SetMinHeight(m_iButtonHeight);
        m_pButtonSizer->Add(pButton, UISizerFlags().Expand())->SetId(_dwId);
        if (m_pButtonSizer->GetItemCount() > m_iNumPerPage)
        {
            pButton->SetVisible(false);
        }
    }
    return true;
}

void UIButtonDlg::UpdatePageButtonIndexRange()
{
    const int iSize = m_pButtonSizer->GetItemCount();
    m_iCurPageStartIndex = m_iCurPage * m_iNumPerPage;
    m_iCurPageEndIndex = m_iCurPageStartIndex + m_iNumPerPage - 1;
    if (m_iCurPageEndIndex >= iSize)
    {
        m_iCurPageEndIndex = iSize - 1;
    }
	UpdatePageNum();
}

void UIButtonDlg::SetSelectedButtonIndex(int index)
{
    const int iSize = m_pButtonSizer->GetItemCount();
    if (index < 0 || index >= iSize)
        return;

    m_iSelectedButton = index;
    if (iSize > 0)
    {
        m_iCurPage   = m_iSelectedButton >= 0 ? (m_iSelectedButton / m_iNumPerPage) : 0;
        m_iTotalPage = (iSize + m_iNumPerPage - 1)/m_iNumPerPage;
        m_pBottomSizer->Show(m_iTotalPage > 1);
    }
    UpdatePageButtonIndexRange();
}

bool UIButtonDlg::SetSelectedButtonByIndex(int _iSelectedButton)
{
    if (_iSelectedButton < 0 || (unsigned int)_iSelectedButton >= m_pButtonSizer->GetItemCount())
    {
        return false;
    }
    
    if (m_iSelectedButton == _iSelectedButton)
    {
        return true;
    }

    UISizerItem* curItem = m_pButtonSizer->GetItem(_iSelectedButton);
    UISizerItem* preItem = m_pButtonSizer->GetItem(m_iSelectedButton);

    if (preItem)
    {
        UIWindow* pWindow = preItem->GetWindow();
        if (pWindow)
        {
            pWindow->SetFocus(false);
        }
    }

    if (curItem)
    {
        UIWindow* pWindow = curItem->GetWindow();
        if (pWindow)
        {
            pWindow->SetFocus(true);
            SetSelectedButtonIndex(_iSelectedButton);
        }
    }
 
    return true;
}

bool UIButtonDlg::SetSelectedButtonById(DWORD _dwCmdId)
{
    UISizerItem* curItem = m_pButtonSizer->GetItemById(_dwCmdId);
    UISizerItem* preItem = m_pButtonSizer->GetItem(m_iSelectedButton);
    if (preItem)
    {
        UIWindow* pWindow = preItem->GetWindow();
        if (pWindow)
        {
            pWindow->SetFocus(false);
        }
    }
    if (curItem)
    {
        UIWindow* pWindow = curItem->GetWindow();
        if (pWindow)
        {
            pWindow->SetFocus(true);
            SetSelectedButtonIndex(m_pButtonSizer->IndexOf(pWindow));
        }
        return true;
    }
    return false;
}
 
int UIButtonDlg::GetSelectedButtonID() const
{
    if (m_iSelectedButton < 0 || (unsigned int)m_iSelectedButton >= m_pButtonSizer->GetItemCount())
    {
        return -1;
    }
 
    UISizerItem* item = m_pButtonSizer->GetItem(m_iSelectedButton);
    if (item)
    {
        UIWindow* pWindow = item->GetWindow();
        if (pWindow)
        {
            return pWindow->GetId();
        }
    }

    return -1;
}

bool UIButtonDlg::SetItemMargin(int _iItemMargin)
{
    if ((_iItemMargin < 0) || (_iItemMargin > m_iHeight))
    {
        return false;
    }
    
    m_iItemMargin  = _iItemMargin;
    return true;
}

bool UIButtonDlg::SetButtonHeight(int _iHeight)
{
    if ((_iHeight < 0) || (_iHeight > m_iHeight))
    {
        return false;
    }
    
    m_iButtonHeight  = _iHeight;
    return true;
}

int UIButtonDlg::GetTotalHeight() const 
{
    dkSize minSize = GetSizer()->GetMinSize();
    return minSize.GetHeight();
}

bool UIButtonDlg::SetNumPerPage(int _iNumPerPage)
{
    if ((_iNumPerPage < 0) || (((_iNumPerPage * m_iButtonHeight) + ((_iNumPerPage - 1) * m_iItemMargin)) > m_iHeight))
    {
        return false;
    }
    
    
    _iNumPerPage  = _iNumPerPage;
    return true;
}

bool UIButtonDlg::SetTextAlign(int _iAlign)
{
    if ((_iAlign < ALIGN_LEFT) || (_iAlign > ALIGN_RIGHT))
    {
        return false;
    }
    
    m_iTextAlign = _iAlign;
    return true;
}

bool UIButtonDlg::OnHookTouch(MoveEvent* moveEvent)
{
    if (moveEvent->GetActionMasked() != MoveEvent::ACTION_UP)
    {
        return false;
    }
    if (IsVisible() && !PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
    {
        EndDialog(0);
        DebugPrintf(DLC_DIAGNOSTIC, "UIButtonDlg::OnHookTouch() return true");
        return true;
    }
    return false;
}

void UIButtonDlg::SetData(const std::vector<std::string>& ss, int selectedIndex)
{
    for (unsigned int i = 0; i < ss.size(); ++i)
    {
        AppendButton(BI_Base + i, ss[i].c_str(), m_btnFontSize);
    }
    SetSelectedButtonByIndex(selectedIndex);
}

int UIButtonDlg::CalculateTop(const UIButton* button)
{
    if (button)
    {
        const int y = button->GetAbsoluteY();
        int top = y + button->GetHeight() + m_vertSpacingFromButtonAttached;
        int height = GetTotalHeight();
        int parentHeight = DeviceInfo::GetDisplayScreenHeight();//GetParent()->GetHeight();
        if (top + height > parentHeight)
        {
            top = y - height - m_vertSpacingFromButtonAttached;
        }
        return top;
    }
    return 0;
}

bool UIButtonDlg::OnChildClick(UIWindow* child)
{
    return true;
}
