#include "TouchAppUI/UIBookContentSearchListItem.h"

#include "DkSPtr.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Public/Base/TPDef.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"
#include "Common/SystemSetting_DK.h"
#include "Common/WindowsMetrics.h"
using namespace DkFormat;
using namespace WindowsMetrics;


UIBookContentSearchListItem::UIBookContentSearchListItem()
	: UIBasicListItem()
{
}

UIBookContentSearchListItem::UIBookContentSearchListItem(UICompoundListBox* pParent,const DWORD dwId)
	: UIBasicListItem(pParent, dwId)
{
}

UIBookContentSearchListItem::~UIBookContentSearchListItem()
{
}

void UIBookContentSearchListItem::InitItem (PCCH pcchItemName, INT iFontSize, INT iLocation)
{
	if(!pcchItemName || !pcchItemName[0])
	{
		return;
	}

    if (0 <= iLocation)
    {
        SetLocation(iLocation);
    }

	m_itemNameLabel.SetFontSize(iFontSize);
	this->SetItemName (pcchItemName);
	m_readingProgressBar.SetProgress(0);
    m_itemLocation.SetVAlign(VALIGN_CENTER);
    if (!m_windowSizer)
    {
        //m_itemNameLabel.SetMinSize(dkSize(m_minWidth, 50));
        m_itemNameLabel.SetMinWidth(m_minWidth);
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        mainSizer->Add(&m_itemNameLabel, UISizerFlags(1).Expand().Border(dkTOP, GetWindowMetrics(UIBookContentSearchListItemTopMarginIndex)));
        mainSizer->AddStretchSpacer();
        mainSizer->Add(&m_itemLocation, UISizerFlags().Expand());
        mainSizer->Add(&m_readingProgressBar, UISizerFlags().Expand().Border(dkTOP, GetWindowMetrics(UIPixelValue8Index)));
        SetSizer(mainSizer);
    }
}

void UIBookContentSearchListItem::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
	UIContainer::MoveWindow(iLeft, iTop, iWidth, iHeight);
	//int VerticalMargin = 20;
	//int Top = VerticalMargin;
	//int CommentHeight = 50;
 //   m_itemNameLabel.MoveWindow(UIHorizonMargin, Top, m_iWidth - (UIHorizonMargin<<1), CommentHeight);  //此处m_iHeight - 16，处理查找与目录有压字的问题
	//Top += (CommentHeight + VerticalMargin - 3);
	//m_itemLocation.MoveWindow(UIHorizonMargin, Top - 2, 100, 25);
	//m_readingProgressBar.MoveWindow(UIHorizonMargin, m_iHeight - 1, m_iWidth-(UIHorizonMargin<<1), 1);
}

void UIBookContentSearchListItem::SetItemName (PCCH pcchItemName)
{
	if(pcchItemName)
	{
        m_itemNameLabel.SetText(pcchItemName);
        m_itemNameLabel.SetUseTraditionalChinese((bool)SystemSettingInfo::GetInstance()->GetFontStyle_FamiliarOrTraditional());
	}
}

HRESULT UIBookContentSearchListItem::Draw(DK_IMAGE drawingImg)
{
	return UIContainer::Draw(drawingImg);
}

void UIBookContentSearchListItem::SetLocation(INT iLocation/* = -1*/)
{
    if (-1 != iLocation)
    {
        INT iLoc = (iLocation < 0 || iLocation > 100) ? 100 : iLocation;
        char location[5] = {0};
	    snprintf(location, 5, "%d%%", iLoc);
        m_itemLocation.SetText(CString(location));
        m_itemLocation.SetFontSize(16);
    }
}
