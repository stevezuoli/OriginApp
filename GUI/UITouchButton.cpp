////////////////////////////////////////////////////////////////////////
// UITouchButton.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UITouchButton.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "Public/Base/TPDef.h"
#include "interface.h"

#define UNDER_LINE_HEIGHT 5

#define RightIconMargin 5


UITouchButton::UITouchButton()
    : UIButton()
    , m_iFocusedFontColor(0)
    , m_focusedBackgroundColor(0)
    , m_bBackTransparent(0)
    , m_spBackground()
    , m_spFocusedBackground()
    , m_bUsingBackgroundPicture(false)
    , m_do2gray(true)
{
    Init();
}

UITouchButton::UITouchButton(UIContainer* pParent, const DWORD dwId)
    : UIButton(pParent, dwId)
    , m_iFocusedFontColor(0)
    , m_focusedBackgroundColor(0)
    , m_bBackTransparent(0)
    , m_spBackground()
    , m_spFocusedBackground()
    , m_bUsingBackgroundPicture(false)
    , m_do2gray(true)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }

    Init();
}

void UITouchButton::Init()
{
    UIButton::Init();
    SetBackgroundColor(ColorManager::knWhite);
    SetFocusedBackgroundColor(ColorManager::knBlack);
    SetFontColor(ColorManager::knBlack);
    SetFocusedFontColor(ColorManager::knWhite);

}

UITouchButton::~UITouchButton()
{
}

void UITouchButton::Initialize(DWORD dwId, LPCSTR szText, INT32 iFontSize, int iBackgroundColor, int iFocuedBackgroundColor)
{
    SetId(dwId);
    SetText(szText);
    SetFontSize(iFontSize);

    SetBackgroundColor(iBackgroundColor);
    SetFocusedBackgroundColor(iFocuedBackgroundColor);
}

void UITouchButton::Initialize(DWORD dwId, LPCSTR szText, INT32 iFontSize, SPtr<ITpImage> pBackground, SPtr<ITpImage> pFocusedBackground)
{
    SetId(dwId);
    SetText(szText);
    SetFontSize(iFontSize);

    SetBackground(pBackground);
    SetFocusedBackground(pFocusedBackground);

    m_bUsingBackgroundPicture = true;
}

SPtr<ITpImage> UITouchButton::GetBackground()
{
    return m_spBackground;
}

void UITouchButton::SetBackground(SPtr<ITpImage> pImageBackground)
{
    if(pImageBackground)
    {
        m_spBackground = pImageBackground;
        m_bUsingBackgroundPicture = true;
    }
}

void UITouchButton::SetFocusedFontColor(int _fontColor)
{
    m_iFocusedFontColor = _fontColor;
}

int UITouchButton::GetFocusedFontColor()
{
    return m_iFocusedFontColor;
}


SPtr<ITpImage> UITouchButton::GetFocusedBackground()
{
    return m_spFocusedBackground;
}

void UITouchButton::SetFocusedBackground(SPtr<ITpImage> pImageFocusedBackground)
{
    if(pImageFocusedBackground)
    {
        m_spFocusedBackground = pImageFocusedBackground;
        m_bUsingBackgroundPicture = true;
    }
}

dkSize UITouchButton::GetMinSize() const
{
    int minWidth = 0;
    int minHeight = 0;
    if (m_minWidth == dkDefaultCoord || m_minHeight == dkDefaultCoord)
    {
        if (m_spFocusedBackground)
        {
            minWidth = m_spFocusedBackground->GetWidth() > minWidth ? m_spFocusedBackground->GetWidth() : minWidth;
            minHeight = m_spFocusedBackground->GetHeight() > minHeight ? m_spFocusedBackground->GetHeight() : minHeight;
        }

        if (m_spBackground)
        {
            minWidth = m_spBackground->GetWidth() > minWidth ? m_spFocusedBackground->GetWidth() : minWidth;
            minHeight = m_spBackground->GetHeight() > minHeight ? m_spFocusedBackground->GetHeight() : minHeight;
        }
        ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, m_iFocusedFontColor );
        if(pFont)
        {
            int _iStringWidth = pFont->StringWidth(m_strText);
            int _iStringHeight = pFont->GetHeight();
            minWidth = minWidth > _iStringWidth ? minWidth : _iStringWidth;
            minHeight = minHeight > _iStringHeight ? minHeight : _iStringHeight;
        }
    }

    if (m_minWidth != dkDefaultCoord)
    {
        minWidth = m_minWidth;
    }
    if (m_minHeight != dkDefaultCoord)
    {
        minHeight = m_minHeight;
    }

    return dkSize(minWidth, minHeight);
}

void UITouchButton::SetDo2Gray(bool do2gray)
{
    m_do2gray = do2gray;
}

HRESULT UITouchButton::Draw(DK_IMAGE drawingImg)
{
	DebugPrintf(DLC_CHENM, "UITouchButton::Draw()  Start");

    if (!m_bIsVisible)
    {
	    DebugPrintf(DLC_CHENM, "UITouchButton::Draw()  End: m_bIsVisible = false");
        return S_OK;
    }

    if (drawingImg.pbData == NULL)
    {
	    DebugPrintf(DLC_CHENM, "UITouchButton::Draw()  End: m_bIsVisible = false");
        return E_FAIL;
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


    if (m_bUsingBackgroundPicture)
    {
        // draw background picture if there is
        if (IsFocus() || IsPressed())
        {
            if (m_spFocusedBackground)
            {
                    HRESULT hr(S_OK);
	                DebugPrintf(DLC_CHENM, "UITouchButton::Draw()  Draw background Image  in focus");
                    int startx = (m_iWidth - m_spFocusedBackground.Get()->GetWidth());

                    if (startx >0)
                        startx = startx / 2;
                    else
                        startx =0;

                    int starty = (m_iHeight - m_spFocusedBackground.Get()->GetHeight());

                    if (starty >0)
                        starty = starty / 2;
                    else
                        starty =0;

                    RTN_HR_IF_FAILED(grf.DrawImage(m_spFocusedBackground.Get(),startx,starty,0,0,m_spFocusedBackground.Get()->GetWidth(), m_spFocusedBackground.Get()->GetHeight()));
                    //RTN_HR_IF_FAILED(grf.DrawImage(m_spFocusedBackground.Get(), 0, 0));
            }
        }
        else
        {
            if (m_spBackground)
            {
	                DebugPrintf(DLC_CHENM, "UITouchButton::Draw()  Draw background Image not in focus");

                    int startx = (m_iWidth - m_spBackground.Get()->GetWidth());

                    if (startx >0)
                        startx = startx / 2;
                    else
                        startx =0;

                    int starty = (m_iHeight - m_spBackground.Get()->GetHeight());

                    if (starty >0)
                        starty = starty / 2;
                    else
                        starty =0;

                    HRESULT hr(S_OK);
                    RTN_HR_IF_FAILED(grf.DrawImage(m_spBackground.Get(), startx, starty, 0, 0, m_spBackground.Get()->GetWidth(), m_spBackground.Get()->GetHeight()));
                    //RTN_HR_IF_FAILED(grf.DrawImage(m_spBackground.Get(), 0, 0));
            }
        }
    }
    else
    {
        // draw background in color
        if (!m_bBackTransparent)
        {
            if (IsFocus() || IsPressed())
            {
	            DebugPrintf(DLC_CHENM, "UITouchButton::Draw()  Draw background with color in focus");
                RTN_HR_IF_FAILED(grf.FillRect(
                    0,
                    0,
                    m_iWidth,
                    m_iHeight,
                    m_focusedBackgroundColor));
            }
            else
            {
	            DebugPrintf(DLC_CHENM, "UITouchButton::Draw()  Draw background with color not in focus");
                RTN_HR_IF_FAILED(grf.FillRect(
                    0,
                    0,
                    m_iWidth,
                    m_iHeight,
                    GetBackgroundColor()));
            }
        }
    }
    //draw string
    if (m_strText.Length() > 0)
    {
        m_ifontAttr.m_iFace=FONT_DEFAULT;
        
        ITpFont* pFont = NULL;

        if (IsFocus() || IsPressed())
        {
            pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, m_iFocusedFontColor);
        }
        else
        {
            pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, GetFontColor());
        }

        if(NULL == pFont)
        {
	        DebugPrintf(DLC_CHENM, "UITouchButton::Draw()  End: pFont = NULL");
            return E_FAIL;
        }

        int _iStringWidth = pFont->StringWidth(m_strText);
        int _iStringHeight = pFont->GetHeight();

        if (_iStringWidth > m_iWidth)
        {
	        DebugPrintf(DLC_CHENM, "UITouchButton::Draw()  End:_iStringWidth > m_iWidth");
            return E_FAIL;
        }

        int _H_offset = (m_iWidth - _iStringWidth) / 2;
        int _V_offset = (m_iHeight - _iStringHeight) / 2;

        grf.DrawStringUtf8(m_strText.GetBuffer(), _H_offset, _V_offset, pFont);
    }

    if (m_do2gray)
    {
        grf.Do2Gray(0,0,m_iWidth, m_iHeight, 80, FALSE, (IsEnable() ? 0xff : 0x66));
    }

    DebugPrintf(DLC_CHENM, "UITouchButton::Draw()  End.");
    return hr;
}

void UITouchButton::SetFocusedBackgroundColor(dkColor focusedBackgroundColor)
{
    m_focusedBackgroundColor = focusedBackgroundColor;
    m_bUsingBackgroundPicture = false;
}

void UITouchButton::SetBackTransparent(BOOL bIsTransparent)
{
    m_bBackTransparent = bIsTransparent;
}

BOOL UITouchButton::OnKeyPress(INT32 iKeyCode)
{
    DebugPrintf(DLC_CHENM, "UITouchButton::OnKeyPress()  Start");

    //UIContainer* pParent = GetParent();

    //if(IsVisible() && pParent)
    //{
    //    pParent->OnEvent(UIEvent(COMMAND_EVENT, this,GetId()));
    //    DebugPrintf(DLC_CHENM, "UITouchButton::OnKeyPress()  End: keyevent handled");
    //    return FALSE;
    //}

    DebugPrintf(DLC_CHENM, "UITouchButton::OnKeyPress()  End");
    return UIWindow::OnKeyPress(iKeyCode);
}

int UITouchButton::GetTextWidth()
{
    if (m_strText.Length() > 0)
    {
        m_ifontAttr.m_iFace=FONT_DEFAULT;
        
        ITpFont* pFont = NULL;

        if (IsFocus())
        {
            pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, m_iFocusedFontColor);
        }
        else
        {
            pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, GetFontColor());
        }

        if(NULL == pFont)
        {
	        DebugPrintf(DLC_CHENM, "UITouchButton::Draw()  End: pFont = NULL");
            return E_FAIL;
        }

        int _iStringWidth = pFont->StringWidth(m_strText);

        return _iStringWidth;
    }

    return -1;
}

int UITouchButton::GetTextHeight()
{
    if (m_strText.Length() > 0)
    {
        m_ifontAttr.m_iFace=FONT_DEFAULT;
        
        ITpFont* pFont = NULL;

        if (IsFocus())
        {
            pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, m_iFocusedFontColor);
        }
        else
        {
            pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, GetFontColor());
        }

        if(NULL == pFont)
        {
	        DebugPrintf(DLC_CHENM, "UITouchButton::Draw()  End: pFont = NULL");
            return E_FAIL;
        }

        int _iStringHeight = pFont->GetHeight();

        return _iStringHeight;
    }

    return -1;
}
#ifndef KINDLE_FOR_TOUCH
HRESULT UITouchButton::UpdateFocus()
{
    return UpdateWindow();
}
#endif