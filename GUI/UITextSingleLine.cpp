////////////////////////////////////////////////////////////////////////
// UITextSingleLine.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UITextSingleLine.h"
#include "GUI/FontManager.h"
#include "GUI/GUIHelper.h"
#include "GUI/CTpGraphics.h"
#include "Common/SystemSetting_DK.h"
#include "Utility/ColorManager.h"
#include "interface.h"
#include "KernelEncoding.h"

UITextSingleLine::UITextSingleLine() :
    UIAbstractText(NULL, IDSTATIC)
{
    Init();
}

UITextSingleLine::UITextSingleLine(UIContainer* pParent, const  DWORD dwId) :
    UIAbstractText(pParent, dwId)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }
    Init();
}

UITextSingleLine::~UITextSingleLine()
{
    Dispose();
}

void UITextSingleLine::OnDispose(BOOL bIsDisposed)
{
    if (bIsDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;
}

void UITextSingleLine::Init()
{
    m_bIsTabStop = FALSE;
	m_TextDrawer.SetMode(TextDrawer::TDM_FIXED_LINES);
	m_TextDrawer.SetMaxLines(1);
	m_TextDrawer.SetEndWithEllipsis(true);
	m_TextDrawer.SetMaxWidth(100000);//足够宽使得能够排下所有字符
    SetFontPath();
}

HRESULT UITextSingleLine::Draw(DK_IMAGE drawingImg)
{
	if (!m_bIsVisible)
	{
		DebugPrintf(DLC_UITEXT, "Draw if invisible");
		return S_OK;
	}
	if (drawingImg.pbData == NULL)
    {
        DebugPrintf(DLC_UITEXT,"Draw if (drawingImg.pbData == NULL)");
		return E_FAIL;
	}

    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    CropImage(drawingImg, rcSelf, &imgSelf);
    DrawBackGround(imgSelf);
	hr = m_TextDrawer.Render(imgSelf, 0, 0, m_iWidth, m_iHeight);
    return hr;
}

void UITextSingleLine::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
	DebugPrintf(DLC_ZHY, "UITextSingleLine::MoveWindow(%s): %d, %d, %d, %d", GetText(), iLeft, iTop, iWidth, iHeight);
	if (m_bIsAutoModifyHeight)
	{
		INT32 expectedHeight = GetTextHeight();
		if (iHeight < expectedHeight)
		{
			iHeight = expectedHeight;
			DebugPrintf(DLC_ZHY, "auto height: %d->%d, %s", iHeight, expectedHeight, m_TextDrawer.GetText());
		}
	}

	if (m_bIsAutoModifyWidth)
	{
		INT32 expectedWidth = GetTextWidth();
		if (iWidth < expectedWidth)
		{
			iWidth = expectedWidth;
			DebugPrintf(DLC_ZHY, "auto width: %d->%d, %s", iHeight, expectedWidth, m_TextDrawer.GetText());
		}
	}

	if (iWidth > 0)
		m_TextDrawer.SetMaxWidth(iWidth);
	DebugPrintf(DLC_ZHY, "UITextSingleLine::MoveWindow modified : %d, %d, %d, %d", iLeft, iTop, iWidth, iHeight);
	UIWindow::MoveWindow( iLeft,  iTop,  iWidth,  iHeight);
}

void UITextSingleLine::SetTextMaxWidth(int maxTextWidth)
{
	m_TextDrawer.SetMaxWidth(maxTextWidth);
}

void UITextSingleLine::SetReversedColor(bool reverse)
{
	if (reverse)
	{
		SetForeColor(ColorManager::knWhite);
		SetBackColor(ColorManager::knBlack);
	}
	else
	{
		SetForeColor(ColorManager::knBlack);
		SetBackColor(ColorManager::knWhite);
	}
}

dkSize UITextSingleLine::GetMinSize() const
{
    int width = m_minWidth;
    int height = m_minHeight;
    if ((width != dkDefaultCoord) || GetTextLength() == 0)
    {
        width = WidthDefault(width);
    }
    else
    {
        width = GetTextWidth();
    }


    if ((height != dkDefaultCoord) || GetTextLength() == 0)
    {
        height = WidthDefault(height);
    }
    else
    {
        height = GetTextHeight();
    }
    DebugPrintf(DLC_ZHY, "text: %s, m_minWidth: %d, m_minHeight: %d, minsize: %d,%d", GetText(), m_minWidth, m_minHeight, width, height);

    return dkSize(width, height);
}

void UITextSingleLine::SetFontPath()
{
#ifndef KINDLE_FOR_TOUCH
    DKFont* pChineseFont (NULL);
    wchar_t* wcChineseFontName = EncodingUtil::CharToWChar (SystemSettingInfo::GetInstance()->GetSysDefaultFont());
	wstring pChineseFontName = wcChineseFontName;
    SafeFreePointer(wcChineseFontName);

    pChineseFont = g_pFontManager->GetFontForFaceName (pChineseFontName);
	m_TextDrawer.SetGbFontFile(pChineseFont->GetFontPath());

 //   wstring wstrDefaultEnglishFont = L"Lingoes Unicode";
 //   DKFont* pEnglishFont (NULL);
 //   pEnglishFont = g_pFontManager->GetFontForFaceName (wstrDefaultEnglishFont);
 //   if(NULL == pEnglishFont)
	//{
	//	pEnglishFont = pChineseFont;
	//}
	m_TextDrawer.SetAnsiFontFile(pChineseFont->GetFontPath());
#else
    DKFont* pChineseFont (NULL);
    wchar_t* wcChineseFontName = EncodingUtil::CharToWChar(SystemSettingInfo::GetInstance()->GetDefaultFontByCharset(DK_CHARSET_GB));
	wstring pChineseFontName = wcChineseFontName;
    SafeFreePointer(wcChineseFontName);

    pChineseFont = g_pFontManager->GetFontForFaceName (pChineseFontName);
	m_TextDrawer.SetGbFontFile(pChineseFont->GetFontPath());

    wstring wstrDefaultEnglishFont = L"Lingoes Unicode";
    DKFont* pEnglishFont (NULL);
    pEnglishFont = g_pFontManager->GetFontForFaceName (wstrDefaultEnglishFont);
    if(NULL == pEnglishFont)
	{
		pEnglishFont = pChineseFont;
	}
	m_TextDrawer.SetAnsiFontFile(pChineseFont->GetFontPath());
#endif
}

void UITextSingleLine::SetEnglishGothic()
{
	wstring fontPath = L"Century Gothic";
    DKFont* englishFont (NULL);
	if(g_pFontManager)
	{
		englishFont = g_pFontManager->GetFontForFaceName (fontPath);
		if(englishFont)
		{
			m_TextDrawer.SetAnsiFontFile(englishFont->GetFontPath());
		}
	}
}
