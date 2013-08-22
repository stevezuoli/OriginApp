////////////////////////////////////////////////////////////////////////
// UIComplexButton.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UIComplexButton.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "Public/Base/TPDef.h"

#define UNDER_LINE_HEIGHT 5

#define RightIconMargin 5
#define ELEMENT_MARGIN 3

UIComplexButton::UIComplexButton()
    : UIButton()
    , m_iBackColor(0)
    , m_bBackTransparent(0)
#ifndef KINDLE_FOR_TOUCH
    , m_chHotKey(0)
#endif
    , m_UnderLineWidth(0)
    , m_UnderLineHeight(0)
    , m_Progreess(0)
    , m_SelectLineWidth(0)
    , m_iIconSize(0)
    , m_fIsDrawn(FALSE)
    , m_iBorder(0)
    , m_bIsShowDotted(false)
{
    Init();
}

UIComplexButton::UIComplexButton(UIContainer* pParent, const DWORD dwId)
    : UIButton(pParent, dwId)
    , m_iBackColor(0)
    , m_bBackTransparent(0)
#ifndef KINDLE_FOR_TOUCH
    , m_chHotKey(0)
#endif
    , m_UnderLineWidth(0)
    , m_UnderLineHeight(0)
    , m_Progreess(0)
    , m_SelectLineWidth(0)
    , m_iIconSize(0)
    , m_fIsDrawn(FALSE)
    , m_iBorder(0)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }

    Init();
}

UIComplexButton::~UIComplexButton()
{

}

#ifdef KINDLE_FOR_TOUCH
void UIComplexButton::Initialize(DWORD dwId, LPCSTR szText, INT32 iFontSize, SPtr<ITpImage> Icon)
{
    SetId(dwId);
    SetText(szText);
    SetFontSize(iFontSize);

    // TODO: 应该自己选择缺省Icon
    SetIcon(Icon);
}

void UIComplexButton::ShowDottedLine(bool _bIsShowDotted)
{
    if (m_bIsShowDotted != _bIsShowDotted)
    {
        m_bIsShowDotted = _bIsShowDotted;
        //Repaint();
    }
}

#else
void UIComplexButton::Initialize(DWORD dwId, LPCSTR szText, char chHotKey, INT32 iFontSize, SPtr<ITpImage> Icon)
{
    SetId(dwId);
    SetText(szText);
    SetFontSize(iFontSize);
    SetHotKey(chHotKey);

    // TODO: 应该自己选择缺省Icon
    SetIcon(Icon);
}
#endif

void UIComplexButton::Init()
{
    UIButton::Init();
    
    m_UnderLineWidth = 0;
    m_UnderLineHeight = 1;
    m_SelectLineWidth = 4;
    m_Progreess = 0;
#ifndef KINDLE_FOR_TOUCH
    m_chHotKey = KEY_RESERVED;
#endif
    m_iIconSize = 0;
    m_iBackColor = 0;
    m_bBackTransparent =true;
    m_fIsDrawn = FALSE;
	m_iBackGroundImage = -1;//TODO:-1 means no background image
}

#ifndef KINDLE_FOR_TOUCH
void UIComplexButton::SetHotKey(char key)
{
	if(DeviceInfo::IsK3())
	{
		m_chHotKey = key;
	}
	else
	{
		m_chHotKey = KEY_RESERVED;
	}
}

char UIComplexButton::GetHotKey()
{
    return m_chHotKey;
}
#endif

SPtr<ITpImage> UIComplexButton::GetIcon()
{
    return m_Icon;
}

void UIComplexButton::SetIcon(SPtr<ITpImage> Icon)
{
    if(Icon)
    {
        m_Icon = Icon;
        if(m_Icon.Get()->GetWidth() > 30)
        {
            SetFontSize(26);
            m_iIconSize = 36;
        }
        else
        {
            SetFontSize(18);
            m_iIconSize = 24;
        }

        //Repaint();
    }
}

void UIComplexButton::SetRightIcon(SPtr<ITpImage> spIcon)
{
    m_spRightIcon = spIcon;
}

bool UIComplexButton::SetBorder(int _iBorder)
{
    if (_iBorder < 0)
        return false;
    m_iBorder = _iBorder;
    return true;
}

int UIComplexButton::GetBorder() const
{
    return m_iBorder;
}

int UIComplexButton::GetUnderLineWidth()
{
    return m_UnderLineWidth;
}

void UIComplexButton::SetUnderLineWidth(int width)
{
    if (m_UnderLineWidth != width)
    {
        m_UnderLineWidth = width;
        //Repaint();
    }
}



int UIComplexButton::GetSelectLineWidth()
{
    return m_SelectLineWidth;
}

void UIComplexButton::SetSelectLineWidth(int  width)
{
    if (m_SelectLineWidth != width)
    {
        m_SelectLineWidth = width;
        //Repaint();
    }
}

int UIComplexButton::GetUnderLineHeight()
{
    return m_UnderLineHeight;
}

void UIComplexButton::SetUnderLineHeight(int  height)
{
    if (m_UnderLineHeight != height)
    {
        m_UnderLineHeight = height;
        //Repaint();
    }
}

int UIComplexButton::GetProcess()
{
    return m_Progreess;
}

void UIComplexButton::SetProcess(int porcess)
{
    if (m_Progreess != porcess)
    {
        m_Progreess = porcess;
        //Repaint();
    }
}

void UIComplexButton::SetBackGroundImage(INT32 bg)
{
	m_iBackGroundImage = bg;
}

HRESULT UIComplexButton::DrawBackGround(DK_IMAGE drawingImg)
{
	CTpGraphics prg(drawingImg);
	SPtr<ITpImage> spImg = ImageManager::GetImage(m_iBackGroundImage);
	if (spImg)
	{
		prg.DrawImageStretchBlend(spImg.Get(), 0, 0, m_iWidth, m_iHeight);
	}
    return S_OK;
}

HRESULT UIComplexButton::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
    {
        return S_OK;
    }

    if (drawingImg.pbData == NULL)
    {
        return E_FAIL;
    }
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft + m_iBorder, m_iTop + m_iBorder, m_iLeft + m_iWidth - m_iBorder, m_iTop + m_iHeight - m_iBorder};
    RTN_HR_IF_FAILED(CropImage(
        drawingImg,
        rcSelf,
        &imgSelf
        ));

    CTpGraphics grf(imgSelf);

    
    // draw background
    if (!m_bBackTransparent)
    {
        RTN_HR_IF_FAILED(grf.FillRect(
            0,
            0,
            m_iWidth,
            m_iHeight,
            m_iBackColor));
    }
    RTN_HR_IF_FAILED(UpdateUI(drawingImg, FALSE, NULL));
    m_IsChanged = false;
    m_fIsDrawn = TRUE;
    return hr;
}


void UIComplexButton::SetBackColor(INT32 iColor)
{
    m_iBackColor = iColor;
}

void UIComplexButton::SetBackTransparent(BOOL bIsTransparent)
{
    m_bBackTransparent = bIsTransparent;
}

HRESULT UIComplexButton::UpdateFocus()
{
    if (S_OK == UpdateWindow())
    {
        m_IsChanged = false;
    }

    return S_OK;
}

HRESULT UIComplexButton::UpdateUI(DK_IMAGE imgParent, BOOL fDrawFocusOnly, DK_RECT *pUpdatedRect)
{
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    
#ifdef KINDLE_FOR_TOUCH
    DK_RECT rcSelf={m_iLeft + m_iBorder, m_iTop + m_iBorder, m_iLeft + m_iWidth - m_iBorder, m_iTop + m_iHeight - m_iBorder};
    RTN_HR_IF_FAILED(CropImage(
        imgParent,
        rcSelf,
        &imgSelf
        ));

    CTpGraphics grf(imgSelf);
    RTN_HR_IF_FAILED(grf.EraserBackGround(m_iBackColor));


	if (m_iBackGroundImage!=-1){
		RTN_HR_IF_FAILED(DrawBackGround(imgSelf));
	}
    
    int iImgLeft = 0, iImgTop = 0, iTxtLeft = 0, iTxtTop = 0;
        
    ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, GetFontColor());
    if(NULL == pFont)
    {
        return E_FAIL;
    }
    int iStringWidth  = pFont->StringWidth(m_strText);
    int iStringHeight = pFont->GetHeight();
    int iImgWidth     = 0;
    int iImgHeight    = 0;
    
    if (m_Icon/* && m_fShowIcon*/)
    {
        iImgWidth  = m_Icon->GetWidth();
        iImgHeight = m_Icon->GetHeight();
    }
    
    switch(m_iIconPos)
    {
    case ICON_LEFT:
        {
            if (iImgHeight < m_iHeight)
            {
                iImgTop = (m_iHeight - iImgHeight) >> 1;
            }
            
            if (ALIGN_CENTER == m_iAlign)
                iImgLeft = (m_iWidth - iImgWidth - iStringWidth) >> 1;
            
            iTxtLeft = iImgLeft + iImgWidth + ELEMENT_MARGIN;
            if (iStringHeight < m_iHeight)
            {
                iTxtTop = (m_iHeight - iStringHeight) >> 1;
            }
        }
        break;
    case ICON_TOP:
        {
            int iAllTop = iImgHeight + iStringHeight;
            if (iAllTop < m_iHeight)
            {
                iImgTop = (m_iHeight - iAllTop) >> 1;
            }
            if (iImgWidth < m_iWidth)
            {
                iImgLeft = (m_iWidth - iImgWidth) >> 1;
            }
            iTxtTop += (iImgTop +iImgHeight + ELEMENT_MARGIN);
            if (iStringWidth < m_iWidth)
            {
                iTxtLeft = (m_iWidth - iStringWidth) >> 1;
            }
        }
        break;
    case ICON_RIGHT:
        {
            if (iStringHeight < m_iHeight)
            {
                iTxtTop = (m_iHeight - iStringHeight) >> 1;
            }
            
            if (iImgHeight < m_iHeight)
            {
                iImgTop = (m_iHeight - iImgHeight) >> 1;
            }
            if (ALIGN_CENTER == m_iAlign)
                iTxtLeft = (m_iWidth - iImgWidth - iStringWidth) >> 1;
            iImgLeft = iTxtLeft + iStringWidth + ELEMENT_MARGIN;//m_iWidth - iImgWidth - ELEMENT_MARGIN;
        }
        break;
    case ICON_BOTTOM:
        {
            int iAllTop = iImgHeight + iStringHeight;
            if (iAllTop < m_iHeight)
            {
                iImgTop = (m_iHeight - iAllTop) >> 1;
            }
            
            if (iStringWidth < m_iWidth)
            {
                iTxtLeft = (m_iWidth - iStringWidth) >> 1;
            }
            
            iImgTop = (iTxtTop + iStringHeight + ELEMENT_MARGIN);
            if (iImgWidth < m_iWidth)
            {
                iImgLeft = (m_iWidth - iImgWidth) >> 1;
            }
        }
        break;
    default:
        break;
    }
    
    if (m_Icon && IsShowIcon())
    {
        RTN_HR_IF_FAILED(grf.DrawImage(m_Icon.Get(), iImgLeft, iImgTop));
    }

    if (iStringWidth > 0)
    {
        grf.DrawStringUtf8(m_strText, iTxtLeft, iTxtTop, pFont);
        
        if (m_bIsFocus)
        {
            grf.DrawLine(iTxtLeft, iTxtTop + iStringHeight, iStringWidth, 2, SOLID_STROKE);
        }
        else if (m_bIsShowDotted)
        {
            grf.DrawLine(iTxtLeft, iTxtTop + iStringHeight, iStringWidth, 2, DOTTED_STROKE);
        }
    }
    
#else
    INT32 iRightIconWidth = 0;
    INT32 iRightIconHeight = 0;
    INT32 iRightMargin = 0;
    if (m_spRightIcon)
    {
        iRightIconWidth = m_spRightIcon->GetWidth();
        iRightIconHeight = m_spRightIcon->GetHeight();
        if (iRightIconWidth > 0 && iRightIconHeight > 0)
        {
            iRightMargin = RightIconMargin + 1 + iRightIconWidth;
        }
    }    

    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth - iRightMargin, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        imgParent,
        rcSelf,
        &imgSelf
        ));

    CTpGraphics grf(imgSelf);
    if (!fDrawFocusOnly && !m_bBackTransparent)
    {
        RTN_HR_IF_FAILED(grf.FillRect(
            0,
            0,
            m_iWidth,
            m_iHeight,
            m_iBackColor));
    }

    if (m_spBackGroundImage)
    {
        grf.DrawImage(m_spBackGroundImage.Get(), 0, 0, 0, 0, m_spBackGroundImage->GetWidth(), m_spBackGroundImage->GetHeight());
    }
    // draw shortcut key
    m_ifontAttr.m_iFace=FONT_DEFAULT;
    int iFontColor = GetFontColor();
    ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, iFontColor);

    if(NULL == pFont)
    {
        return E_FAIL;
    }
    INT32 iDrawingWidth = 0;
    if (ShouldShowLeftIcon())
    {
        iDrawingWidth += m_iIconSize + (m_ifontAttr.m_iSize >> 1);
    }

    INT32 iStringWidth = 0;
    if (m_strText.Length() > 0)
    {
        iStringWidth = pFont->StringWidth(m_strText);
        iDrawingWidth += iStringWidth;
    }

	if(m_isSelected || m_isShowSelectedMargin)
	{
		iDrawingWidth += SelectedIconWidth;
		iDrawingWidth += RightIconMargin;
	}

    iDrawingWidth += iRightMargin;

    INT32 iDrawLeft = 0;
    INT32 iClientHeight = m_iHeight;
    INT32 iUnderLineWidth = ((0 != m_UnderLineWidth) ? m_UnderLineWidth : iStringWidth);
    if (iDrawingWidth > 0)
    {
        if (ALIGN_LEFT != m_iAlign)
        {
            if (iDrawingWidth < m_iWidth)
            {
                if (ALIGN_CENTER == m_iAlign)
                {
                    iDrawLeft = (m_iWidth - iDrawingWidth) / 2;
                }
                else if (ALIGN_RIGHT == m_iAlign)
                {
                    iDrawLeft = m_iWidth - iDrawingWidth;
                }
            }
        }

        if(m_isSelected)
        {
        	SPtr<ITpImage> pbgImage = ImageManager::GetImage(IMAGE_SYS_OPTION);
			if(pbgImage)
			{
				RTN_HR_IF_FAILED(grf.DrawImageBlend(pbgImage.Get(), 0, (m_iHeight - pbgImage->bmHeight)/2, 0, 0, pbgImage->bmWidth, pbgImage->bmHeight));
			}
        }
		if(m_isSelected || m_isShowSelectedMargin)
		{
			iDrawLeft += SelectedIconWidth;
			iDrawLeft += RightIconMargin;
		}

        int iFontsize = m_ifontAttr.m_iSize;
        if (ShouldShowLeftIcon())
        {
            if (!fDrawFocusOnly)
            {
                // draw icon
                if (m_iValign == VALIGN_TOP)
                {
                    RTN_HR_IF_FAILED(grf.DrawImageBlend(m_Icon.Get(), iDrawLeft, 0, 0, 0, m_iIconSize, m_iIconSize));
                }
                else if (m_iValign == VALIGN_CENTER)
                {
                    RTN_HR_IF_FAILED(grf.DrawImageBlend(m_Icon.Get(), iDrawLeft, (m_iHeight - m_iIconSize)>>1, 0, 0, m_iIconSize, m_iIconSize));
                }
                char shortkey[2] = {0};
                sprintf(shortkey, "%c", m_chHotKey);

                m_ifontAttr.m_iFace = FONT_DEFAULT;
                ITpFont* pShortcutFont = FontManager::GetInstance()->GetFont(m_ifontAttr, iFontColor);
                INT32 iCharWidth = pShortcutFont->CharWidth(m_chHotKey);
                if (m_iValign == VALIGN_TOP)
                {
                    grf.DrawStringUtf8(shortkey, iDrawLeft  +( (m_iIconSize - iCharWidth) >> 1), 2, pShortcutFont);
                }
                else if (m_iValign == VALIGN_CENTER)
                {
                    grf.DrawStringUtf8(shortkey, iDrawLeft  +( (m_iIconSize - iCharWidth) >> 1), (m_iHeight - pShortcutFont->GetHeight()) >> 1, pShortcutFont);
                }
            }

            iDrawLeft += m_iIconSize + (iFontsize >> 1);
            if (m_iValign == VALIGN_TOP)
            {
                iClientHeight = m_iIconSize;
            }
        }
        if (m_iValign == VALIGN_CENTER)
        {
            m_ifontAttr.m_iFace = FONT_DEFAULT;
            ITpFont* pShortcutFont = FontManager::GetInstance()->GetFont(m_ifontAttr, iFontColor);
            iClientHeight = m_iIconSize + ((m_iHeight - pShortcutFont->GetHeight())>>1);
        }


        if (iDrawingWidth > m_iWidth)
        {
            iUnderLineWidth = m_iWidth - iDrawLeft;
        }

        m_ifontAttr.m_iFace=FONT_DEFAULT;
        pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, iFontColor);
        if(NULL == pFont)
        {
            return E_FAIL;
        }
        if (m_strText.Length() > 0)
        {
            // draw text
            if (!fDrawFocusOnly)
            {
                if (m_iValign == VALIGN_TOP)
                {
                    grf.DrawStringUtf8(m_strText.GetBuffer(), iDrawLeft, 0, pFont);
                }
                else if (m_iValign == VALIGN_CENTER)
                {
                    grf.DrawStringUtf8(m_strText.GetBuffer(), iDrawLeft, (m_iHeight - pFont->GetHeight()) >> 1, pFont);
                }
            }

            // draw dotted line

            /*RTN_HR_IF_FAILED(grf.DrawLine(
            iDrawLeft,
            iClientHeight - 5,
            iUnderLineWidth,
            5, //iClientHeight,
            SOLID_STROKE));*/
            RTN_HR_IF_FAILED(grf.FillRect(
                iDrawLeft,
                iClientHeight - UNDER_LINE_HEIGHT,
                iDrawLeft + iUnderLineWidth,
                iClientHeight,
                m_iBackColor));

            if (IsEnable())
            {
                //todo 5 pixel was need change
                RTN_HR_IF_FAILED(grf.DrawLine(
                    iDrawLeft,
                    iClientHeight - m_UnderLineHeight / 2 -UNDER_LINE_HEIGHT / 2,
                    iUnderLineWidth,
                    m_UnderLineHeight, //iClientHeight,
                    DOTTED_STROKE));

                //draw focus
                if(m_bIsFocus)
                {
                    RTN_HR_IF_FAILED(grf.DrawLine(
                        iDrawLeft,
                        iClientHeight - UNDER_LINE_HEIGHT,
                        iUnderLineWidth,
                        UNDER_LINE_HEIGHT,
                        SOLID_STROKE));
                }
                else if(m_Progreess > 0 && m_Progreess <= 100)
                {
                    // draw progress
                    RTN_HR_IF_FAILED(grf.DrawLine(
                        iDrawLeft,
                        iClientHeight - m_UnderLineHeight,
                        iUnderLineWidth * m_Progreess / 100,
                        m_UnderLineHeight, //iClientHeight,
                        SOLID_STROKE));
                }
            }
        }

        if (!fDrawFocusOnly && m_spRightIcon && iRightIconWidth > 0 && iRightIconHeight > 0)
        {
            INT32 iIconTop = (m_iHeight - iRightIconHeight) / 2;
            if (iIconTop >= 0)
            {
                // draw right icon
                INT32 iIconLeft = m_iLeft + m_iWidth - iRightMargin <  m_iLeft + iDrawLeft + iStringWidth ? m_iLeft + m_iWidth - iRightMargin : m_iLeft + iDrawLeft + iStringWidth;
                DK_RECT rcRightIcon = {iIconLeft, 
                    m_iTop, 
                    iIconLeft + iRightMargin, 
                    m_iTop + m_iHeight};            
                DK_IMAGE imgRightIcon;
                RTN_HR_IF_FAILED(CropImage(
                    imgParent,
                    rcRightIcon,
                    &imgRightIcon
                    ));

                CTpGraphics grfRightIcon(imgRightIcon);
                RTN_HR_IF_FAILED(grfRightIcon.DrawImage(m_spRightIcon.Get(), RightIconMargin, iIconTop));
            }
        }
    }

    if (pUpdatedRect != NULL && fDrawFocusOnly)
    {
        pUpdatedRect->left += m_iLeft + iDrawLeft;
        pUpdatedRect->top += m_iTop + (iClientHeight - UNDER_LINE_HEIGHT/*m_UnderLineHeight < iClientHeight - (m_SelectLineWidth >> 1) ? iClientHeight - m_UnderLineHeight : iClientHeight - (m_SelectLineWidth >> 1)*/ );
        pUpdatedRect->right = iDrawLeft + iUnderLineWidth + pUpdatedRect->left;
        pUpdatedRect->bottom = pUpdatedRect->top + iClientHeight /*+ (m_SelectLineWidth >> 1)*/;
    }
#endif      
    return hr;
}


BOOL UIComplexButton::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    char key = VirtualKeyToChar(iKeyCode);
    UIContainer* pParent = GetParent();
    if((key == m_chHotKey || key == (m_chHotKey + 32)) && IsVisible() && pParent)
    {
        pParent->OnEvent(UIEvent(COMMAND_EVENT, this,GetId()));
        return false;
    }
#endif
    return UIWindow::OnKeyPress(iKeyCode);
}

