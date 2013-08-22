////////////////////////////////////////////////////////////////////////
// UIBasicListItem.h
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIBasicListItem.h"
#include "DkSPtr.h"
#include "Utility/ColorManager.h"
#include "GUI/CTpGraphics.h"
#include "Public/Base/TPDef.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"
#include "BookReader/TocInfo.h"
#include "Common/WindowsMetrics.h"
using namespace DkFormat;
using namespace WindowsMetrics;
const int PROGRESS_BAR_HEIGHT = 5;

UIBasicListItem::UIBasicListItem()
    : UIListItemBase(NULL, IDSTATIC)
    , m_fIsMarked (FALSE)
    , m_fIndentMode(FALSE)
    , m_bIsEnabled(TRUE)
    , m_hotKeyBtn(this, IDSTATIC)
    , m_itemNameLabel(this, IDSTATIC)
    , m_itemLocation(this, IDSTATIC)
    , m_itemPage(this, IDSTATIC)
    , m_readingProgressBar(this, IDSTATIC)
    , m_familiarToTraditional(false)
    , m_expandButton(this, ID_BTN_TOUCH_TOC_EXPAND)
    , m_displayExpandButton(false)
    , m_expandStatus(DK_TOCINFO::ES_NONE)
    , m_tocEnable(true)
    , m_lPageIndex(-1)
{
    m_expandButton.SetVisible(false);
    m_itemNameLabel.SetMode(UIAbstractText::TextMultiLine);
}

UIBasicListItem::UIBasicListItem(UICompoundListBox* pParent, const DWORD dwId)
    : UIListItemBase(pParent,dwId)
    , m_fIsMarked (FALSE)
    , m_bIsEnabled(TRUE)
#ifndef KINDLE_FOR_TOUCH
    , m_hotKeyBtn(this, IDSTATIC)
#endif
    , m_itemNameLabel(this, IDSTATIC)
    , m_itemLocation(this, IDSTATIC)
    , m_itemPage(this, IDSTATIC)
    , m_readingProgressBar(this, IDSTATIC)
    , m_familiarToTraditional(false)
    , m_expandButton(this, ID_BTN_TOUCH_TOC_EXPAND)
    , m_displayExpandButton(false)
    , m_expandStatus(DK_TOCINFO::ES_NONE)
    , m_tocEnable(true)
    , m_lPageIndex(-1)
{
    if (pParent != NULL)
    {
    	pParent->AppendChild(this);
    }
    m_expandButton.SetVisible(false);
    m_itemNameLabel.SetMode(UIAbstractText::TextMultiLine);
}

UIBasicListItem::~UIBasicListItem()
{
}

void UIBasicListItem::InitItem (PCCH pcchItemName, INT iFontSize, INT iLocation)
{
    if(!pcchItemName || !pcchItemName[0])
    {
    	return;
    }
    SetShowBottomLine(false);

    if (0 <= iLocation)
    {
        SetLocation(iLocation);
    }

    m_itemNameLabel.SetFontSize(iFontSize);
    SetItemName (pcchItemName);
    //if ((2 * iFontSize) >= (m_iHeight - 20))
    //{
    //	m_itemNameLabel.SetJustify (FALSE); // if the item label might take multi-lines to show, we should not enable justify.
    //}
    m_readingProgressBar.MoveWindow(40, m_iHeight - PROGRESS_BAR_HEIGHT, m_iWidth-40*2, PROGRESS_BAR_HEIGHT);
    m_readingProgressBar.SetProgress(0);
}

void UIBasicListItem::SetTocItemEnable(bool bEnable)
{
    m_tocEnable = bEnable;
    // 使用内核颜色设置和目前不一样，颜色应该取反，并加入Alpha
    if (m_tocEnable)
    {
    	m_itemNameLabel.SetForeColor(ColorManager::knBlack);
    	m_readingProgressBar.SetDrawBold(m_bIsFocus);
    }
    else
    {
    	m_readingProgressBar.SetDrawBold(false);
    	m_itemNameLabel.SetForeColor(
                ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    }
    m_itemPage.SetVisible(m_tocEnable && m_lPageIndex >= 0);
}

void UIBasicListItem::SetItemName (PCCH pcchItemName)
{
    if(pcchItemName)
    {
    	const char * pItemName = pcchItemName;
    	int iPrefixLen = 0;
    	if(m_fIndentMode)
    	{
    		char cPrefix = pItemName[0];
    		if(cPrefix <= '9' && cPrefix >= '1')
    		{
                int indent = cPrefix - '0';
                // 只有前两级缩进有展开按钮，故缩进40,其余缩进20即可
                if (indent <= 1)
                {
                    iPrefixLen = indent * 40;
                }
                else
                {
                    iPrefixLen = 40 + (indent - 1) * 20;
                }
    			pItemName = pItemName+1;
    		}
        }
        m_itemNameLabel.SetText(pItemName);
        m_itemNameLabel.SetUseTraditionalChinese(m_familiarToTraditional);
        int nameHeight = m_iHeight - PROGRESS_BAR_HEIGHT;
        m_itemNameLabel.MoveWindow(
                iPrefixLen+40, 0, m_iWidth - iPrefixLen - 40*2 - 40, nameHeight);  
        if (m_itemNameLabel.GetHeight() < nameHeight) // been jusitified
        {
            int nameTop = (nameHeight - m_itemNameLabel.GetHeight()) / 2;
            m_itemNameLabel.MoveWindow(
                    m_itemNameLabel.GetX(),
                    nameTop,
                    m_itemNameLabel.GetWidth(),
                    m_itemNameLabel.GetHeight());

        }
        m_itemNameLabel.SetVAlign(VALIGN_CENTER);
        if (m_displayExpandButton && m_expandStatus != DK_TOCINFO::ES_NONE)
        {
            SPtr<ITpImage> img = ImageManager::GetImage(
                                    m_expandStatus == DK_TOCINFO::ES_PLUS ?
                                    IMAGE_ICON_FOLDER_EXPAND_PLUS:
                                    IMAGE_ICON_FOLDER_EXPAND_MINUS);
            m_expandButton.SetIcons(img, img);
            m_expandButton.MoveWindow(iPrefixLen, 0, 40, GetHeight() - PROGRESS_BAR_HEIGHT); // img->GetWidth(), img->GetHeight());
            m_expandButton.ShowBorder(false);
            m_expandButton.SetFocusedBackgroundColor(ColorManager::knWhite);
            m_expandButton.SetVisible(true);
        }
        else
        {
            m_expandButton.SetVisible(false);
        }
    }
}

void UIBasicListItem::SetMarked (BOOL fIsMarked)
{
    m_fIsMarked = fIsMarked;
}

void UIBasicListItem::SetIndentMode(BOOL fIndentMode)
{
    m_fIndentMode = fIndentMode;
}

BOOL UIBasicListItem::OnKeyPress(INT32 iKeyCode)
{
    return TRUE;
}

void UIBasicListItem::SetFocus(BOOL bIsFocus)
{
    if (m_bIsFocus != bIsFocus)
    {
    	m_bIsFocus = bIsFocus;
    	m_readingProgressBar.UpdateWindow();
    }
}

BOOL  UIBasicListItem::SetVisible(BOOL bVisible)
{
    UIContainer::SetVisible(bVisible);
    m_itemNameLabel.SetVisible(bVisible);
    m_readingProgressBar.SetVisible(bVisible);
    m_itemLocation.SetVisible(bVisible);
    m_itemPage.SetVisible(bVisible && m_tocEnable && m_lPageIndex >= 0);

    return bVisible;
}

HRESULT UIBasicListItem::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
    {
    	return S_OK;
    }

    if (NULL == drawingImg.pbData)
    {
    	return E_INVALIDARG;
    }
	if (m_fIsMarked)
    {
    	CTpGraphics grp(drawingImg);
    	INT iLRArrowWidth = 15;
    	INT iLRArrowHeight = 15;
    	INT iLRArrowTop = (m_iHeight- iLRArrowHeight)/2 - 4;
    	SPtr<ITpImage> spArrow = ImageManager::GetImage(IMAGE_FOUR_ARROW);
    	if(spArrow)
    	{
    		grp.DrawImageBlend(spArrow.Get(), m_iLeft + 12, m_iTop + iLRArrowTop, 0, 45, iLRArrowWidth, iLRArrowHeight);
    	}
    }
    UIContainer::Draw(drawingImg);
    return S_OK;
}

HRESULT UIBasicListItem::DrawBackGround(DK_IMAGE)
{
    return S_OK;
}

void UIBasicListItem::SetKeyword(PCCH pcchKeyword, UINT uHighLightStartPos, UINT uHighLightEndPos)
{
    m_itemNameLabel.SetKeyword(pcchKeyword, uHighLightStartPos, uHighLightEndPos);
}

void UIBasicListItem::SetLocation(INT iLocation)
{
    if (-1 != iLocation)
    {
        INT iLoc = (iLocation < 0 || iLocation > 100) ? 100 : iLocation;
        char location[5] = {0};
        snprintf(location, 5, "%d%%", iLoc);
        m_itemLocation.SetText(CString(location));
        m_itemLocation.SetFontSize(14);
        INT iLeft(MarkIconWidth);
        if (10 <= iLoc && 99 >= iLoc)
        {
            iLeft += 6;
        }
        else if (10 > iLoc)
        {
            iLeft += 10;
        }
        m_itemLocation.MoveWindow(iLeft, m_hotKeyBtn.GetHeight() + 8, m_hotKeyBtn.GetWidth(), 20);
    }
}

void UIBasicListItem::SetPageIndex(long pageIndex)
{
    m_lPageIndex = pageIndex;
    m_itemPage.SetVisible(m_tocEnable && pageIndex >= 0);
    if (pageIndex >= 0)
    {
        char buf[32] = {0};
        snprintf(buf, DK_DIM(buf), "%ld", pageIndex + 1);
        m_itemPage.SetText(buf);
        m_itemPage.SetFontSize(GetWindowFontSize(FontSize14Index));
        int itemWidth = m_itemPage.GetTextWidth();
        int itemHeight = m_itemPage.GetTextHeight();
        m_itemPage.MoveWindow(m_iWidth - 40 - itemWidth, (m_iHeight - itemHeight) >> 1, itemWidth, itemHeight);
    }
}

void UIBasicListItem::SetDisplayExpandButton(bool displayExpandButton)
{
    m_displayExpandButton = displayExpandButton;
}

bool UIBasicListItem::IsDisplayExpandButton() const
{
    return m_displayExpandButton;
}

void UIBasicListItem::SetExpandStatus(int expandStatus)
{
    m_expandStatus = expandStatus;
}

int UIBasicListItem::GetExpandStatus() const
{
    return m_expandStatus;
}

void UIBasicListItem::OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam)
{
    if (pSender == &m_expandButton)
    {
        UIWindow* parent = GetParent();
        if (NULL != parent)
        {
            parent->OnCommand(ID_BTN_TOUCH_TOC_EXPAND, this, 0);
        }
    }
}

void UIBasicListItem::PerformClick(MoveEvent* moveEvent)
{
    if (m_tocEnable)
    {
        UIListItemBase::PerformClick(moveEvent);
    }
}
