////////////////////////////////////////////////////////////////////////
// UIText.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////


#include "GUI/UIText.h"
#include "GUI/GUIHelper.h"
#include "GUI/FontManager.h"
#include "Utility/ColorManager.h"
#include "GUI/CTpGraphics.h"
#include "Utility.h"
#include "Framework/CDisplay.h"
#include "dkColor.h"
#include "FontManager/DKFont.h"
#include "Common/FileManager_DK.h"
#include "BMPProcessor/IBMPProcessor.h"
#include "KernelEncoding.h"
#include "DKRAPI.h"

using namespace  DkFormat;
UIText::UIText() : UIAbstractText(NULL, IDSTATIC)
    , m_iCurPage(0)
    , m_imgBackgroundWidth(0)
    , m_imgBackgroundHeight(0)
{
    Init();
}

UIText::UIText(UIContainer* pParent, const DWORD dwId) : UIAbstractText(pParent, dwId)
    ,m_iCurPage(0)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }

    Init();
}

UIText::~UIText()
{
    Dispose();
}

void UIText::Init()
{
    m_bIsDisposed = FALSE;
    SetEnable(false);
    m_TextDrawer.SetMode(TextDrawer::TDM_UNLIMITED_HEIGHT);
    //SetDrawPageNo(0);
}

void UIText::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
    if (iWidth > 0)
    {
        m_TextDrawer.SetMaxWidth(iWidth);
    }

    if (iHeight > 0)
    {
        if (m_TextDrawer.GetMode() == TextDrawer::TDM_UNLIMITED_HEIGHT)
        {
            m_TextDrawer.SetPageHeight(iHeight);

            if (m_bIsAutoModifyHeight)
            {
                if (m_iCurPage + 1 == (int)GetPageCount())
                {
                    iHeight = GetPageHeightOfPageIndex(m_iCurPage);
                }
            }
        }
        else if (m_TextDrawer.GetMode() == TextDrawer::TDM_FIXED_HEIGHT)
        {
            if (m_bIsAutoModifyHeight)
            {
                m_TextDrawer.SetMaxHeight(iHeight);
            }
        }
    }

    UIWindow::MoveWindow(iLeft, iTop, iWidth, iHeight);
}

void UIText::SetMode(TextMode mode)
{
    switch(mode)
    {
    case TextSingleLine:
        m_TextDrawer.SetMode(TextDrawer::TDM_FIXED_LINES);
        break;
    case TextMultiLine:
        m_TextDrawer.SetMode(TextDrawer::TDM_FIXED_HEIGHT);
        m_TextDrawer.SetEndWithEllipsis(true);
        break;
    case TextMultiPage:
        m_TextDrawer.SetMode(TextDrawer::TDM_UNLIMITED_HEIGHT);
        break;
    }
}

//void UIText::SetFontPath()
//{
//}
//
void UIText::SetLineSpace(DK_DOUBLE lineSpace)
{
    m_TextDrawer.SetLineGap(lineSpace);
}

UINT UIText::GetPageCount()
{
    DebugPrintf(DLC_UITEXT,"GetPageCount start");
    return m_TextDrawer.GetTotalPages();
    DebugPrintf(DLC_UITEXT,"GetPageCount end");
}

void UIText::SetDrawPageNo(UINT _uPageNo)
{
    m_iCurPage = _uPageNo;
    if (m_bIsAutoModifyHeight)
    {
        int height = m_iHeight;
        if (m_iCurPage == (int)GetPageCount()-1)
        {
            height = GetPageHeightOfPageIndex(m_iCurPage);
        }
        else
        {
            height = m_TextDrawer.GetPageHeight();
        }
        UIWindow::MoveWindow(m_iLeft, m_iTop, m_iWidth, height);
    }
}

UINT UIText::GetCurPage()
{
    return m_iCurPage;
}

int  UIText::GetPageHeightOfPageIndex(UINT uPageNo)
{
    return m_TextDrawer.GetTextHeightOfPageIndex(uPageNo);
}

HRESULT UIText::Draw(DK_IMAGE drawingImg)
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
    DebugPrintf(DLC_ZHY, "UIText::Draw: %d,%d,%d,%d\n%s", m_iTop, m_iLeft, m_iWidth, m_iHeight, GetText());
    if (!m_imgBackground.empty())
    {
        CTpGraphics grf(imgSelf);

        grf.DrawImageFile(m_imgBackground.c_str(),
                DK_BOX(0, 0, m_imgBackgroundWidth, m_imgBackgroundHeight),
                DK_BOX(0, 0, m_iWidth, m_iHeight));
    }

    if (m_TextDrawer.GetMode() == TextDrawer::TDM_UNLIMITED_HEIGHT)
    {
        hr = m_TextDrawer.RenderPage(m_iCurPage, imgSelf, 0, 0, m_iWidth, m_iHeight);
    }
    else if (m_TextDrawer.GetMode() == TextDrawer::TDM_FIXED_HEIGHT)
    {
        hr = m_TextDrawer.Render(imgSelf, 0, 0, m_iWidth, m_iHeight);
    }
    return hr;
}

int UIText::GetHeightByWidth(int width) const
{
    DebugPrintf(DLC_UITEXT, "%s:%d:%s  %s start", __FILE__, __LINE__, __FUNCTION__, GetClassName());
    m_TextDrawer.SetMaxWidth(width);
    return UIAbstractText::GetTextHeight();
}

int UIText::GetTextHeight() const
{
    if (m_iWidth > 0)
    {
        return GetHeightByWidth(m_iWidth);
    }
    else
    {
        return -1;
    }
}

int UIText::GetPageNumByWidthHeight(int width, int height)
{
    if (m_TextDrawer.GetMode() != TextDrawer::TDM_UNLIMITED_HEIGHT)
    {
        DebugPrintf(DLC_ERROR, "%s::%s try to get pagenum while mode is not mulitipages");
        return -1;
    }

    m_TextDrawer.SetPageHeight(height);
    m_TextDrawer.SetMaxWidth(width);
    return GetPageCount();
}

int  UIText::SetPageHeight(int height)
{
    m_TextDrawer.SetPageHeight(height);
    return true;
}

dkSize UIText::GetMinSize() const
{
    if (m_minWidth == dkDefaultCoord || GetTextLength() == 0)
    {
        return dkSize(WidthDefault(m_minWidth), WidthDefault(m_minHeight));
    }
    else if (m_minHeight == dkDefaultCoord)
    {
        return dkSize(m_minWidth, GetHeightByWidth(m_minWidth));
    }
    else
    {
        return dkSize(m_minWidth, m_minHeight);
    }

}

void UIText::SetBackgroundImage(const char* imgFile)
{
    if (NULL != imgFile)
    {
        DK_AUTO(imgBackground, EncodeUtil::ToWString(imgFile));
        if (imgBackground != m_imgBackground)
        {
            DKR_AnalyseImageFile(imgBackground.c_str(),
                    &m_imgBackgroundWidth,
                    &m_imgBackgroundHeight);
            m_imgBackground = imgBackground;
        }

    }
}
