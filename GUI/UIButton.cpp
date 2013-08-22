////////////////////////////////////////////////////////////////////////
// UIButton.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "interface.h"
#include "GUI/UIButton.h"
#include "GUI/FontManager.h"
#include "Utility/ImageManager.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ColorManager.h"
#include <iostream>
#include "I18n/StringManager.h"

#define TEXT_LEFT_PADDING 5
#define TEXT_TOP_PADDING 5
#define m_iTextWidth_PADDING (2 * TEXT_LEFT_PADDING)
#define TEXT_HEIGHT_PADDING (2 * TEXT_TOP_PADDING)
#define BUTTON_BACKBORDER_PADDING 4

UIButton::UIButton() 
    : UIWindow(NULL, IDSTATIC)
    , m_strText("")
    , m_strPresentation("")
    , m_ifontAttr()
    , m_iValign(0)
    , m_fontColor(0)
    , m_bIsShowBorder(true)
    , m_bIsShowIcon(TRUE)
    , m_iIconPos(ICON_TOP)
    , m_bIsDisable(false)
    , m_iDisabledBackgroundColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE))
    , m_iDisabledFontColor(ColorManager::GetColor(COLOR_DISABLECOLOR))
    , m_bIsDisposed(FALSE)
    , m_preGesture(-1)
    , m_flashOnClick(false)
{
    Init();
    //m_TxtContent.SetFontSize(22);

}

UIButton::UIButton(UIContainer* pParent, DWORD dwId, LPCSTR szText) 
    : UIWindow(pParent, dwId)
    , m_strText(szText)
    , m_strPresentation("")
    , m_ifontAttr()
    , m_iValign(0)
    , m_fontColor(0)
    , m_bIsShowBorder(true)
    , m_bIsShowIcon(TRUE)
    , m_iIconPos(ICON_TOP)
    , m_bIsDisable(false)
    , m_iDisabledBackgroundColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE))
    , m_iDisabledFontColor(ColorManager::GetColor(COLOR_DISABLECOLOR))
    , m_bIsDisposed(FALSE)
    , m_preGesture(-1)
    , m_flashOnClick(false)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }

    Init();
}

UIButton::~UIButton()
{
    Dispose();
}

void UIButton::Dispose()
{
    UIWindow::Dispose();
    OnDispose(m_bIsDisposed);
}

void UIButton::Init()
{
    SetClickable(true);
    SetUpdateOnPress(true);
    SetUpdateOnUnPress(true);
    m_fontColor = ColorManager::knBlack;
    m_iAlign = ALIGN_CENTER;
    m_iValign = VALIGN_CENTER;

    m_iWidth = 10;
    m_iHeight = 10;
}

void UIButton::SetAlign(INT32 iAlign)
{
    m_iAlign = iAlign;
    m_IsChanged = true;
    //Repaint();
}

void UIButton::SetIcon(SPtr<ITpImage> Icon, int _iIconPos)
{
    if (Icon)
        m_Icon = Icon;
    m_iIconPos = (_iIconPos < ICON_TOP || ICON_TOP > ICON_BOTTOM) ? ICON_TOP : _iIconPos;
}

void UIButton::SetIconPos(int _iIconPos)
{
    m_iIconPos = (_iIconPos < ICON_TOP || ICON_TOP > ICON_BOTTOM) ? ICON_TOP : _iIconPos;
}

void UIButton::ShowIcon(bool _bShowIcon)
{
    if (m_bIsShowIcon != _bShowIcon)
    {
        m_bIsShowIcon = _bShowIcon;
        m_IsChanged = true;
    }
}
    
INT32 UIButton::GetTextHeight()
{
    //return m_pFont->GetHeight();
    return m_ifontAttr.m_iSize;
}
/*
INT32 UIButton::GetTextWidth()
{
return m_pFont->StringWidth(m_strText);
}
*/
void UIButton::SetVAlign(INT32 iValign)
{
    m_iValign = iValign;
    m_IsChanged = true;
    //Repaint();
}

void UIButton::SetFontColor(dkColor color)
{
    m_fontColor = color;
    m_IsChanged = true;
    //Repaint();
}

bool UIButton::ShowBorder(bool _bIsShowBorder)
{ 
    if (m_bIsShowBorder != _bIsShowBorder)
    {
        m_bIsShowBorder = _bIsShowBorder; 
        m_IsChanged = true;
    }
    return true;
}

const CString& UIButton::ToString()
{
    return m_strPresentation;
}

void UIButton::SetFontSize(int size)
{
    m_ifontAttr.SetFontAttributes(0, 0, size);
}

LPCSTR UIButton::GetText() const
{
    return m_strText;
}

void UIButton::SetText(const CString& strText)
{
    // TODO do the replacement
    if (m_strText != strText)
    {
        m_strText = strText;
        m_IsChanged = true;
    }
}

void UIButton::SetText(int strId)
{
    return SetText(StringManager::GetPCSTRById((SOURCESTRINGID)strId));
}

void UIButton::SetText(LPCSTR pszString)
{
    CString new_string = CString(pszString);
    if (m_strText != new_string)
    {
        m_strText = new_string;
        m_IsChanged = true;
    }
}

HRESULT UIButton::Draw(DK_IMAGE image)
{
    if (!m_bIsVisible)
    {
        return S_OK;
    }

    if (image.pbData == NULL)// || !m_IsChanged)
    {
        return E_FAIL;
    }

    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        image,
        rcSelf,
        &imgSelf
        ));

    CTpGraphics grf(imgSelf);
    std::string imgPath = GetImage();
    if(!imgPath.empty())
    {
        SPtr<ITpImage> pbgImage = ImageManager::GetImage(imgPath);
        if(pbgImage)
        {
            RTN_HR_IF_FAILED(grf.DrawImageBlend(pbgImage.Get(), 0, 0, 0, 0, m_iWidth, m_iHeight));
        }
    }

    if(m_strText.Length())
    {
        ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, GetFontColor());
        grf.DrawStringUtf8(m_strText, 0, 0, pFont);
    }

    m_IsChanged = false;
    DrawHighlight(imgSelf);
    return hr;
}

void UIButton::OnResize(INT32 iWidth, INT32 iHeight)
{
    UIWindow::OnResize(iWidth, iHeight);
    /*    if (m_pstrText != NULL)
    {
    m_pstrText->SetWidth(iWidth - m_iTextWidth_PADDING);
    }*/
}

void UIButton::OnDispose(BOOL bDisposed)
{

    if (bDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;
    // Note, the font resource will be release by font manager
}

void UIButton::PerformClick(MoveEvent* moveEvent)
{
    UIContainer* pParent = GetParent();
    if(IsVisible() && pParent)
    {
        if (m_flashOnClick)
        {
            SetFocus(true);
            SetFocus(false);
        }
        //this->SetFocus(true);
        pParent->OnEvent(UIEvent(COMMAND_EVENT, this,GetId()));
        //this->SetFocus(false);
    }
    UIWindow::PerformClick(moveEvent);
}

dkColor UIButton::GetFontColor() const
{
    if (IsEnable())
    {
        return m_fontColor;
    }
    else
    {
        return ColorManager::GetColor(COLOR_MENUITEM_INACTIVE);
    }
}

void UIButton::DrawHighlight(DK_IMAGE imgSelf)
{
    if (IsPressed())
    { 

        CTpGraphics grf(imgSelf);
        DK_RECT rc = {0, 0, m_iWidth, m_iHeight};
        if (IsShowBorder())
        {
            DK_RECT rc1 = {1, 1, m_iWidth - 2, m_iHeight - 2};
            rc = rc1;
        }
        grf.ReverseRect(rc);
        grf.BinarizeRect(rc);
    }
}
