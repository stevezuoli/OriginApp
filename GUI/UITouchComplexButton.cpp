////////////////////////////////////////////////////////////////////////
// UITouchComplexButton.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UITouchComplexButton.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "Public/Base/TPDef.h"
#include "interface.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include "drivers/DeviceInfo.h"
#include "CommonUI/UIUtility.h"
#include "GUI/UISizer.h"

using namespace WindowsMetrics;

UITouchComplexButton::UITouchComplexButton()
    : UITouchButton()
    , m_bUpLeftSquare(false)
    , m_bUpRightSquare(false)
    , m_bRightDownSquare(false)
    , m_bLeftDownSquare(false)
    , m_borderLine(1)
    , m_imgBackground(NULL)
{
    Init();
}

UITouchComplexButton::UITouchComplexButton(UIContainer* pParent, const DWORD dwId)
    : UITouchButton(pParent, dwId)
    , m_bUpLeftSquare(false)
    , m_bUpRightSquare(false)
    , m_bRightDownSquare(false)
    , m_bLeftDownSquare(false)
    , m_borderLine(1)
    , m_imgBackground(NULL)
{
    Init();
}

void UITouchComplexButton::Init()
{
    const int leftMargin = GetWindowMetrics(UITouchComplexButtonLeftMarginIndex);
    const int topMargin = GetWindowMetrics(UITouchComplexButtonTopMarginIndex);
    const int internalHorzSpacing = GetWindowMetrics(UITouchComplexButtonInternalHorzSpacingIndex);
    const int internalVertSpacing = GetWindowMetrics(UITouchComplexButtonInternalVertSpacingIndex);
    SetLeftMargin(leftMargin);
    SetTopMargin(topMargin);
    SetInternalHorzSpacing(internalHorzSpacing);
    SetInternalVertSpacing(internalVertSpacing);
}

UITouchComplexButton::~UITouchComplexButton()
{
	DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
	DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UITouchComplexButton::SetCornerStyle(bool _bUpLeftSquare, bool _bUpRightSquare, bool _bRightDownSquare, bool _bLeftDownSquare)
{
	DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    m_bUpLeftSquare    = _bUpLeftSquare;
    m_bUpRightSquare   = _bUpRightSquare;
    m_bRightDownSquare = _bRightDownSquare;
    m_bLeftDownSquare  = _bLeftDownSquare;

    DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UITouchComplexButton::SetBackgroundImage(int normalImageID)
{
    std::string normalImagePath = ImageManager::GetImagePath(normalImageID);
    if (!normalImagePath.empty())
    {
        m_imgBackground.SetImageFile(normalImagePath.c_str());
    }
}

void UITouchComplexButton::MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height)
{
    UITouchButton::MoveWindow(left, top, width, height);
    m_imgBackground.MoveWindow(0, 0, m_iWidth, m_iHeight);
}

HRESULT UITouchComplexButton::DrawBackGround(DK_IMAGE drawingImg)
{
    DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!IsFocus() && !IsPressed() && m_imgBackground.HasImage())
    {
        DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s m_imgBackground.HasImage", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return m_imgBackground.Draw(drawingImg);
    }
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
    int iBackgroundColor = ColorManager::knWhite;
    if (!IsEnable())
    {
        iBackgroundColor = ColorManager::knWhite;
    }
    else if (IsFocus() || IsPressed())
    {
        iBackgroundColor = GetFocusedBackgroundColor();
    }
    else
    {
        iBackgroundColor = GetBackgroundColor();
    }
    RTN_HR_IF_FAILED(grf.EraserBackGround(iBackgroundColor));
    
    if (m_spBackground || m_spFocusedBackground)
    {
        if (IsFocus() || IsPressed())
        {
            if (m_spFocusedBackground)
            {
                RTN_HR_IF_FAILED(grf.DrawImage(m_spFocusedBackground.Get(), 0, 0, 0, 0, m_spFocusedBackground->GetWidth(), m_spFocusedBackground.Get()->GetHeight()));
            }
        }
        else
        {
            if (m_spBackground)
            {
                RTN_HR_IF_FAILED(grf.DrawImage(m_spBackground.Get(), 0, 0, 0, 0, m_spBackground->GetWidth(), m_spBackground.Get()->GetHeight()));
            }
        }
        if (!IsEnable())
        {
            uint8_t* imgData = drawingImg.pbData;
            for (int i = 0; i < drawingImg.iHeight; ++i)
            {
                uint8_t* data = imgData + i * drawingImg.iStrideWidth;
                for (int j = 0; j < drawingImg.iWidth; ++j)
                {
                    *data = *data > 0x80 ? 0xaa : *imgData;
                    data++;
                }
            }
        }
    }
    else if (IsShowBorder())
    {
    	DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s IsShowBorder", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
		if (!IsEnable()) {
			RTN_HR_IF_FAILED(grf.FillRect(0, 0, m_iWidth, m_iHeight, m_iDisabledFontColor));
		} else {
			RTN_HR_IF_FAILED(grf.FillRect(0, 0, m_iWidth, m_iHeight, ColorManager::knBlack));
		}
		RTN_HR_IF_FAILED(grf.FillRect(m_borderLine, m_borderLine
						 , m_iWidth - m_borderLine, m_iHeight - m_borderLine, iBackgroundColor));
        bool isFocus = IsFocus() || IsPressed();
        if(!m_bUpLeftSquare)
        {
            SPtr<ITpImage> pUpLeftImage = ImageManager::GetImage(!IsEnable() ? IMAGE_ICON_BTN_UPLEFT_DISABLE : (isFocus ? IMAGE_TOUCH_ICON_BTN_UPLEFT_FOCUS : IMAGE_TOUCH_ICON_BTN_UPLEFT));
            RTN_HR_IF_FAILED(grf.DrawImageBlend(pUpLeftImage.Get(), 0, 0, 0, 0, pUpLeftImage->GetWidth(),  pUpLeftImage->GetHeight()));
        }
        if(!m_bUpRightSquare)
        {
            SPtr<ITpImage> pUpRightImage = ImageManager::GetImage(!IsEnable() ? IMAGE_ICON_BTN_UPRIGHT_DISABLE : (isFocus ? IMAGE_TOUCH_ICON_BTN_UPRIGHT_FOCUS : IMAGE_TOUCH_ICON_BTN_UPRIGHT));
            int iUpRightWidth = pUpRightImage->GetWidth();
            RTN_HR_IF_FAILED(grf.DrawImageBlend(pUpRightImage.Get(), m_iWidth - iUpRightWidth, 0, 0, 0, iUpRightWidth,  pUpRightImage->GetHeight()));
        }
        if(!m_bLeftDownSquare)
        {
            SPtr<ITpImage> pLeftDownImage = ImageManager::GetImage(!IsEnable() ? IMAGE_ICON_BTN_LEFTDOWN_DISABLE : (isFocus ? IMAGE_TOUCH_ICON_BTN_LEFTDOWN_FOCUS : IMAGE_TOUCH_ICON_BTN_LEFTDOWN));
            int iLeftDownWidth  = pLeftDownImage->GetWidth();
            int iLeftDownHeight = pLeftDownImage->GetHeight();
            RTN_HR_IF_FAILED(grf.DrawImageBlend(pLeftDownImage.Get(), 0, m_iHeight - iLeftDownHeight, 0, 0, iLeftDownWidth,  iLeftDownHeight));
        }
        if(!m_bRightDownSquare)
        {
            SPtr<ITpImage> pRightDownImage = ImageManager::GetImage(!IsEnable() ? IMAGE_ICON_BTN_RIGHTDOWN_DISABLE : (isFocus ? IMAGE_TOUCH_ICON_BTN_RIGHTDOWN_FOCUS : IMAGE_TOUCH_ICON_BTN_RIGHTDOWN));
            int iRightDownWidth  = pRightDownImage->GetWidth();
            int iRightDownHeight = pRightDownImage->GetHeight();
            RTN_HR_IF_FAILED(grf.DrawImageBlend(pRightDownImage.Get(), m_iWidth - iRightDownWidth, m_iHeight - iRightDownHeight, 0, 0, iRightDownWidth,  iRightDownHeight));
        }
    }
    return hr;
}

dkSize UITouchComplexButton::GetMinSize() const
{
    int imgWidth = 0;
    int imgHeight = 0;
    int minWidth = 0;
    int minHeight = 0;
    ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, !IsEnable() ? m_iDisabledFontColor : (IsFocus() ? m_iFocusedFontColor : GetFontColor()));
    if (pFont && m_strText.Length() > 0)
    {
        minWidth = pFont->StringWidth(m_strText);
        minHeight = pFont->GetHeight();
    }

    if (m_minWidth == dkDefaultCoord || m_minHeight == dkDefaultCoord)
    {
        if (m_Icon)
        {
            imgWidth = m_Icon->GetWidth();
            imgHeight = m_Icon->GetHeight();

            switch (m_iIconPos)
            {
            case ICON_LEFT:
            case ICON_RIGHT:
                minHeight = (imgHeight > minHeight) ? imgHeight : minHeight;
                minWidth = imgWidth + m_internalHorzSpacing + minWidth;
                break;
            case ICON_TOP:
            case ICON_BOTTOM:
                minWidth = (imgWidth > minWidth) ? imgWidth : minWidth;
                minHeight = imgHeight + m_internalVertSpacing + minHeight;
                break;
            case ICON_CUSTOMIZE:
            case ICON_CENTER:
                minWidth = imgWidth + m_internalHorzSpacing + minWidth;
                minHeight = imgHeight + m_internalVertSpacing + minHeight;
                break;
            }
        }
    }

    minWidth += 2 * GetLeftMargin();
    minHeight += 2 * GetTopMargin();
    if (m_spBackground)
    {
        minWidth = minWidth > m_spBackground->GetWidth() ? minWidth : m_spBackground->GetWidth();
        minHeight = minHeight > m_spBackground->GetHeight() ? minHeight : m_spBackground->GetHeight();
    }
    if (m_spFocusedBackground)
    {
        minWidth = minWidth > m_spFocusedBackground->GetWidth() ? minWidth : m_spFocusedBackground->GetWidth();
        minHeight = minHeight > m_spFocusedBackground->GetHeight() ? minHeight : m_spFocusedBackground->GetHeight();
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

HRESULT UITouchComplexButton::Draw(DK_IMAGE drawingImg)
{
	DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!m_bIsVisible)
    {
	    DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s m_bIsVisible = false", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return S_OK;
    }

    if (drawingImg.pbData == NULL)
    {
	    DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s drawingImg.pbData == NULL", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
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
    
    RTN_HR_IF_FAILED(DrawBackGround(imgSelf));
    
    m_ifontAttr.m_iFace=FONT_DEFAULT;
    ITpFont* pFont = NULL;

    bool bIsFocus = IsFocus() || IsPressed();
    pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, !IsEnable() ? m_iDisabledFontColor : (bIsFocus ? m_iFocusedFontColor : GetFontColor()));
    if(NULL == pFont)
    {
    	DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s NULL == pFont", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return E_FAIL;
    }

    int iImgLeft = 0, iImgTop = 0, iTxtLeft = 0, iTxtTop = 0;
    int iImgWidth     = 0;
    int iImgHeight    = 0;
    int iStringWidth  = pFont->StringWidth(m_strText);
    int iStringHeight = pFont->GetHeight();
    if (m_strText.Length() == 0)
    {
        iStringWidth = 0;
        iStringHeight = 0;
    }
    if (m_Icon)
    {
        DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s m_Icon", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        iImgWidth  = m_Icon->GetWidth();
        iImgHeight = m_Icon->GetHeight();
    
        switch(GetIconPos())
        {
        case ICON_LEFT:
            {
                if (iImgHeight < m_iHeight)
                {
                    iImgTop = (m_iHeight - iImgHeight) >> 1;
                }
                    
                if (iStringHeight < m_iHeight)
                {
                    iTxtTop = (m_iHeight - iStringHeight) >> 1;
                }
                
                if (ALIGN_CENTER == m_iAlign)
                {
                    iImgLeft = (m_iWidth - iImgWidth - iStringWidth - m_internalHorzSpacing) >> 1;
                }
                else if (ALIGN_RIGHT == m_iAlign)
                {
                    iImgLeft = (m_iWidth - iImgWidth - iStringWidth - m_internalHorzSpacing);
                }
                iImgLeft = iImgLeft > 0 ? iImgLeft : 0;
                
                iTxtLeft = iImgLeft + iImgWidth + m_internalHorzSpacing;
            }
            break;
        case ICON_TOP:
            {
                int iAllTop = iImgHeight + iStringHeight;
                if (iAllTop < m_iHeight)
                {
                    iImgTop = (m_iHeight - iAllTop) >> 1;
                }
                if ((ALIGN_CENTER == m_iAlign) && (iImgWidth < m_iWidth))
                {
                    iImgLeft = (m_iWidth - iImgWidth) >> 1;
                }
                iTxtTop += (iImgTop +iImgHeight + m_internalVertSpacing);
                if ((ALIGN_CENTER == m_iAlign) && (iStringWidth < m_iWidth))
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

                const int iMargin = GetLeftMargin();
                iImgLeft = m_iWidth - iImgWidth - iMargin;
                
                if (ALIGN_CENTER == m_iAlign)
                {
                    iTxtLeft = iMargin + ((iImgLeft - m_internalHorzSpacing - iStringWidth - iMargin) >> 1);
                }
                else if (ALIGN_RIGHT== m_iAlign)
                {
                    iTxtLeft = iImgLeft - m_internalHorzSpacing - iStringWidth;
                }
                else
                {
                    iTxtLeft = iMargin;
                }
            }
            break;
        case ICON_BOTTOM:
            {
                int iAllTop = iImgHeight + iStringHeight;
                if (iAllTop < m_iHeight)
                {
                    iImgTop = (m_iHeight - iAllTop) >> 1;
                }
                
                if ((ALIGN_CENTER == m_iAlign) && (iStringWidth < m_iWidth))
                {
                    iTxtLeft = (m_iWidth - iStringWidth) >> 1;
                }
                
                iImgTop = (iTxtTop + iStringHeight + m_internalVertSpacing);
                if ((ALIGN_CENTER == m_iAlign) && (iImgWidth < m_iWidth))
                {
                    iImgLeft = (m_iWidth - iImgWidth) >> 1;
                }
            }
            break;
        case ICON_CENTER:
            {
                iImgTop = std::max(0, (m_iHeight - iImgHeight) >> 1);
                iImgLeft = std::max(0, (m_iWidth - iImgWidth) >> 1);
            }
            break;
        case ICON_CUSTOMIZE:
            {
                // Centralize the icon
                iImgTop = std::max(0, (m_iHeight - iImgHeight) >> 1);
                iImgLeft = std::max(0, (m_iWidth - iImgWidth) >> 1);

                // caculate the position of text
                iTxtLeft = iImgLeft + m_rectCustomizedTextArea.left;
                iTxtTop  = iImgTop + m_rectCustomizedTextArea.top;

                iTxtLeft = iTxtLeft + ((m_rectCustomizedTextArea.GetWidth() - iStringWidth) >> 1) + 1;
                iTxtTop  = iTxtTop  + ((m_rectCustomizedTextArea.GetHeight() - iStringHeight) >> 1) + 1;
            }
            break;
        default:
    		break;
    	}
    }
    else
    {
        DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s !m_Icon", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        if (iStringHeight < m_iHeight)
        {
            iTxtTop = (m_iHeight - iStringHeight) >> 1;
        }
        switch(m_iAlign)
        {
        case ALIGN_LEFT:
            iTxtLeft = GetLeftMargin();
            break;
        case ALIGN_CENTER:
            iTxtLeft = (m_iWidth - iStringWidth) >> 1;
            break;
        case ALIGN_RIGHT:
            iTxtLeft = m_iWidth - iStringWidth - GetLeftMargin();
            break;
        default:
            break;
        }
    }

	if (m_Icon && IsShowIcon())
    {
        DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s if (m_Icon && IsShowIcon())", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
		if (!IsEnable() && m_pDisabledIcon) 
        {
            DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s IsDisable", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
			RTN_HR_IF_FAILED(grf.DrawImage(m_pDisabledIcon.Get(), iImgLeft, iImgTop));
		}
		else if (IsFocus() && m_pFocusedIcon) 
        {
            DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s IsFocus", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
			RTN_HR_IF_FAILED(grf.DrawImage(m_pFocusedIcon.Get(), iImgLeft, iImgTop));
		}
		else
        {
            DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s else", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
			RTN_HR_IF_FAILED(grf.DrawImage(m_Icon.Get(), iImgLeft, iImgTop));
            if (IsFocus() || IsPressed())
            {
                grf.ReverseRect(iImgLeft, iImgTop, iImgLeft + m_Icon->GetWidth(), iImgTop + m_Icon->GetHeight());
                grf.BinarizeRect(iImgLeft, iImgTop, iImgLeft + m_Icon->GetWidth(), iImgTop + m_Icon->GetHeight());
            }
	
		}
	}

    //·ÀÖ¹»­µ½½çÍâ
    iTxtLeft = iTxtLeft >= 0 ? iTxtLeft : 0;
    iTxtTop  = iTxtTop >= 0 ? iTxtTop : 0;

    if (iStringWidth > 0)
    {
        DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s iStringWidth", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        grf.DrawStringUtf8(m_strText, iTxtLeft, iTxtTop, pFont);
    }

    DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return hr;
}


void UITouchComplexButton::SetIcons(SPtr<ITpImage> UnFocusedIcon, SPtr<ITpImage> focusedIcon, int _iIconPos)
{
	UITouchButton::SetIcon(UnFocusedIcon, _iIconPos);
	m_pFocusedIcon = focusedIcon;
}

void UITouchComplexButton::SetCustomizedIcons(SPtr<ITpImage> UnFocusedIcon,
                                              SPtr<ITpImage> focusedIcon,
                                              const DK_RECT& textArea,
                                              int _iIconPos)
{
    SetIcons(UnFocusedIcon, focusedIcon, _iIconPos);
    m_rectCustomizedTextArea = textArea;
}

void UITouchComplexButton::SetDisabledIcon(SPtr<ITpImage> disabledIcon)
{
    DebugPrintf(DLC_UITOUCHCOMPLEXBUTTON, "%s, %d, %s, %s start %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, disabledIcon ? "not null" : "null");
	m_pDisabledIcon = disabledIcon;
}

void UITouchComplexButton::SetBorderLine(int borderLine)
{
    m_borderLine = borderLine;
}

/////////////////////////////////////////////////////////
// UITouchBackButton
/////////////////////////////////////////////////////////
UITouchBackButton::UITouchBackButton(UIContainer* pParent, const DWORD dwId)
    : UIContainer(pParent, dwId)
    , m_leftSpace(NULL)
{
    Init();
}

UITouchBackButton::~UITouchBackButton()
{
}

void UITouchBackButton::Init()
{
    AppendChild(&m_btn);
    AppendChild(&m_seperator);

    m_btn.ShowBorder(false);
    DK_AUTO(img, ImageManager::GetImage(IMAGE_ICON_GO_BACK));
    m_btn.SetIcons(img, img);
    //ITpImage* pImg = img.Get();
    //int imgWidth = 0, imgHeight = 0;
    //if (pImg)
    //{
        //imgHeight = pImg->GetHeight();
    //}
    int leftMargin = GetWindowMetrics(UIBackButtonLeftMarginIndex);
    m_seperator.SetDirection(UISeperator::SD_VERT);
    m_seperator.SetThickness(1);
    
    UISizer* mainSizer = new UIBoxSizer(dkHORIZONTAL);
    if (mainSizer)
    {
        m_btn.SetMinSize(dkSize(GetWindowMetrics(UIBackBtnWidthIndex), GetWindowMetrics(UIBackBtnHeightIndex)));
        m_btn.SetElemSpacing(0);
        m_btn.SetTopMargin(0);
        m_btn.SetLeftMargin(0);
        m_seperator.SetMinSize(dkSize(1, GetWindowMetrics(UIBackBtnSeperatorHeightIndex)));
        m_leftSpace = mainSizer->AddSpacer(leftMargin);
        mainSizer->Add(&m_btn, UISizerFlags().Center());
        mainSizer->Add(&m_seperator, UISizerFlags().Center().Border(dkLEFT, leftMargin));

        SetSizer(mainSizer);
    }
}

//dkSize UITouchBackButton::GetMinSize() const
//{
    //return dkSize(
            //GetWindowMetrics(UIBackButtonWidthIndex), 
            //GetWindowMetrics(UIBackButtonHeightIndex));
//}

bool UITouchBackButton::OnChildClick(UIWindow* child)
{
    if (child == &m_btn && NULL != GetParent())
    {
        return GetParent()->OnChildClick(this);
    }
    return true;
}

void UITouchBackButton::OnCommand(DWORD dwCmdId,
        UIWindow * pSender,
        DWORD dwParam)
{
    if (GetParent() != NULL)
    {
        GetParent()->OnCommand(GetId(), this, dwParam);
    }
}

void UITouchBackButton::ShowSeperator(bool show)
{
    m_seperator.SetVisible(show);
}

void UITouchBackButton::ShowLeftSpace(bool show)
{
    if (m_leftSpace)
    {
        m_leftSpace->Show(show);
    }
}

void UITouchBackButton::SetBackButtonLongClickable(bool clickable)
{
    m_btn.SetLongClickable(clickable);
}

bool UITouchBackButton::OnChildLongPress(UIWindow* child)
{
    UIContainer* parent = GetParent();
    if (parent != 0 && child == &m_btn)
    {
        return parent->OnChildLongPress(this);
    }
    return false;
}
