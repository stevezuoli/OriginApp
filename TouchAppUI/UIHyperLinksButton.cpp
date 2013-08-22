////////////////////////////////////////////////////////////////////////
// UIHyperLinksButton.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIHyperLinksButton.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "Public/Base/TPDef.h"

#define LINE_SPACE (5)
#define UNDERLINEHEIGHT (3)
#define ICON_TEXT_WIDTH (3)
#define TEXT_WIDTH_MARGIN (5)

UIHyperLinksButton::UIHyperLinksButton()
	: UIButton()
	,m_Text()
	,m_iLinkId(-1)
{

}

UIHyperLinksButton::UIHyperLinksButton(UIContainer* pParent, const DWORD dwId)
	: UIButton(pParent, dwId)
	,m_Text()
	,m_iLinkId(-1)
{
	if (pParent != NULL)
	{
		pParent->AppendChild(this);
	}

}

UIHyperLinksButton::~UIHyperLinksButton()
{

}

void UIHyperLinksButton::Initialize(DWORD dwId, LPCSTR szText, INT32 iFontSize,SPtr<ITpImage> Icon)
{
	SetId(dwId);
	m_strText = CString(szText);
	m_Text.SetText(CString(szText));
	m_Text.SetFontSize(iFontSize);
	m_ifontAttr.m_iSize = iFontSize;
	m_ifontAttr.m_iFace = 0;
	m_ifontAttr.m_iStyle= 0;
	m_Icon = Icon;

}


void UIHyperLinksButton::SetText(const CString& rstrTxt)
{
	m_strText = rstrTxt;
	return (m_Text.SetText(rstrTxt));
}

LPCSTR UIHyperLinksButton::GetText() const
{
	return (m_Text.GetText());
}



void UIHyperLinksButton::SetIcon(SPtr<ITpImage> Icon)
{
	m_Icon = Icon;
}

int UIHyperLinksButton::GetLinkId()
{
	return m_iLinkId;
}

void UIHyperLinksButton::SetLinkId(int nLinkId)
{
	if (m_iLinkId != nLinkId)
	{
		m_iLinkId = nLinkId;
	}
}

BOOL UIHyperLinksButton::LayOutButton(int &iUsedWidth,int &iUsedHeight ,HLBtnLayOutStyle style)
{

	BOOL bIsLayOut = false;
	if(!strlen(m_strText.GetBuffer()) || !m_ifontAttr.m_iSize ||  NULL == m_Icon.Get())
	{
		iUsedWidth = 0;
		iUsedHeight = 0;
		return bIsLayOut;
	}

	m_Text.SetFontSize(m_ifontAttr.m_iSize);
	int nIconWidth =  m_Icon.Get()->GetWidth();

	switch(style)
	{
	case LayOutByConstWidth:
        m_Text.MoveWindow(ICON_TEXT_WIDTH + nIconWidth,0,m_iWidth,m_iHeight);
        // TODO(JUGH):删除GetFullTextHeight接口，本处GetHeightByWidth待测试
		iUsedHeight = m_Text.GetHeightByWidth(m_iWidth);
		iUsedHeight += UNDERLINEHEIGHT;
		iUsedWidth = m_iWidth;
		bIsLayOut = true;
		break;
	case LayOutByConstHeight:
		{
			ITpFont *pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
			if(pFont)
			{
				iUsedWidth = pFont->StringWidth(m_strText) + TEXT_WIDTH_MARGIN + ICON_TEXT_WIDTH + nIconWidth;
                m_Text.MoveWindow(ICON_TEXT_WIDTH + nIconWidth,0,m_iWidth,m_iHeight);
                // TODO(JUGH):删除GetFullTextHeight接口，本处GetHeightByWidth待测试
				iUsedHeight =  m_Text.GetHeightByWidth(m_iWidth) + UNDERLINEHEIGHT;
				bIsLayOut = true;
			}
		}
		break;
	default:
		break;
	}


	return bIsLayOut;
}


HRESULT UIHyperLinksButton::Draw(DK_IMAGE drawingImg)
{
	DebugPrintf(DLC_LIWEI,"UIHyperLinksButton draw");
	if (!m_bIsVisible)
	{
		return S_OK;
	}
	if (drawingImg.pbData == NULL || NULL == m_Icon.Get())
	{
		return E_FAIL;
	}
	HRESULT hr(S_OK);
	DK_IMAGE imgSelf;
    m_Text.MoveWindow(0,0,m_iWidth,m_iHeight);
    // TODO(JUGH):删除GetFullTextHeight接口，本处GetHeightByWidth待测试
	int nUsedHeight = m_Text.GetHeightByWidth(m_iWidth);
	DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
	RTN_HR_IF_FAILED(CropImage(
		drawingImg,
		rcSelf,
		&imgSelf
		));
	CTpGraphics grf(imgSelf);

	RTN_HR_IF_FAILED(grf.FillRect(
		0,
		0,
		m_iWidth,
		m_iHeight,
		ColorManager::knWhite));

	int nIconWidth =  m_Icon.Get()->GetWidth();
	if(m_bIsFocus)
	{
		RTN_HR_IF_FAILED(grf.DrawImageBlend(m_Icon.Get(), 0, 0, 0, 0,nIconWidth, m_Icon.Get()->GetHeight()));
	}

	int nTextDrawLeft = ICON_TEXT_WIDTH + nIconWidth;
	int nTextDrawWidth = m_iWidth - nTextDrawLeft;
	m_Text.MoveWindow(nTextDrawLeft,0,nTextDrawWidth,nUsedHeight);
	m_Text.Draw(imgSelf);

	int iUnderLineY = nUsedHeight;
	grf.DrawLine(nTextDrawLeft,iUnderLineY,nTextDrawWidth,2,DOTTED_STROKE);
	if(m_bIsFocus)
	{
		grf.DrawLine(nTextDrawLeft,iUnderLineY,nTextDrawWidth,4,SOLID_STROKE);
	}
	return hr;
}

HRESULT UIHyperLinksButton::UpdateFocus()
{
	if (!m_bIsVisible)
	{
		return S_OK;
	}

	HRESULT hr(S_OK);
	DK_IMAGE imgParent;
	DK_RECT rectParent;
	RTN_HR_IF_FAILED(GetParentDC(imgParent, rectParent));
	RTN_HR_IF_FAILED(Draw(imgParent/*, TRUE, &rectParent*/));
	UpdateToScreen(rectParent);
	return S_OK;
}


