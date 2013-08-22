////////////////////////////////////////////////////////////////////////
// UIBookmarkListItem.h
// Contact: zhangjd
// Copyright (c) Duokan Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIBookmarkListItem.h"
#include "DkSPtr.h"
#include "../inc/GUI/CTpGraphics.h"
#include "Utility/ColorManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"
#include "BookReader/IBookReader.h"
#include "Common/SystemSetting_DK.h"
#include "Common/WindowsMetrics.h"

using namespace DkFormat;
using namespace WindowsMetrics;

UIBookmarkListItem::UIBookmarkListItem(UICompoundListBox* pParent, IBookReader* pBookReader, int iType)
	: m_pItemModel(NULL)
    , m_percentage(this, IDSTATIC)
    , m_bookmarkTime(this, IDSTATIC)
    , m_comment(this, IDSTATIC)
    , m_fileContent(this, IDSTATIC)
    , m_itemType(iType)
    , m_pBookReader(pBookReader)
{
	if (pParent != NULL)
	{
		pParent->AppendChild(this);
    }
    m_percentage.SetVisible(false);
    m_comment.SetVisible(false);
    if (m_windowSizer == NULL)
    {
        m_windowSizer = new UIBoxSizer(dkVERTICAL);
        SetSizer(m_windowSizer);
    }
	SetLongClickable(true);
}

UIBookmarkListItem::~UIBookmarkListItem()
{
}

void UIBookmarkListItem::SetBookItemModel(IBookmarkItemModel *pItemModel)
{
	m_pItemModel = pItemModel;
	InitItem();
}

BOOL UIBookmarkListItem::OnKeyPress(INT32 iKeyCode)
{
	return UIWindow::OnKeyPress(iKeyCode);
}

void UIBookmarkListItem::InitItem()
{
	if (NULL == m_pItemModel)
	{
		return;
    }
    m_windowSizer->Clear();//element clear
    bool isComment = (ICT_ReadingDataItem::COMMENT == m_pItemModel->GetBookmarkType());
    //const int ELEMSPACING = 10;
    //const int USERCOMMENT = 42;

    //const int TOPMARGIN   = 15;
    //const int BOTTOMMARGIN = 10;
    //const int TIMEHEIGHT  = 20;

    //int fileContentTop = TOPMARGIN;
    //int timeTop = isComment ? (m_iHeight - BOTTOMMARGIN - USERCOMMENT - ELEMSPACING - TIMEHEIGHT) : (m_iHeight - TOPMARGIN - TIMEHEIGHT);
    //int fileContentHeight = (timeTop - ELEMSPACING - fileContentTop);

    m_fileContent.SetFontSize(GetWindowFontSize(UIBookmarkCommentIndex));
    m_fileContent.SetLineSpace(1);
    m_fileContent.SetAutoModifyHeight(false);
    //m_fileContent.MoveWindow(0, fileContentTop, m_iWidth, fileContentHeight);
    m_fileContent.SetText(m_pItemModel->GetBookRefContent().c_str());
    SystemSettingInfo *pSystem = SystemSettingInfo::GetInstance();
    if (pSystem)
    {
        m_fileContent.SetUseTraditionalChinese(pSystem->GetFontStyle_FamiliarOrTraditional());
    }

    //Ê±¼ä
    string _strTime = m_pItemModel->GetCreateTime();
    m_bookmarkTime.SetFontSize(GetWindowFontSize(UIBookmarkCommentIndex));
	m_bookmarkTime.SetEnglishGothic();
    m_bookmarkTime.SetForeColor(ColorManager::GetColor(COLOR_KERNEL_DISABLECOLOR));
    m_bookmarkTime.SetText(_strTime.c_str());
    m_bookmarkTime.SetAlign(ALIGN_RIGHT);

    double percentage = (double)-1;
    const CT_RefPos startPos = m_pItemModel->GetStartPos();
    if (m_pBookReader)
    {
        percentage = m_pBookReader->GetPercent(startPos);
    }
    else// zip
    {
        const int endChapter = m_pItemModel->GetEndPos().GetChapterIndex();
        if (endChapter)
        {
            const double startChapter = (double)(startPos.GetChapterIndex() * 100);
            percentage = (startChapter / endChapter);
        }
    }
    bool showPercent = (percentage >= (double)0.0);
    if (showPercent)
    {
        const double maxPercent = (double)100;
        const double minPercent = (double)0.01;
        percentage = (percentage > maxPercent) ? maxPercent : ((percentage < minPercent) ? minPercent : percentage);
        char cText[32] = {0};
        snprintf(cText, sizeof(cText), "%.2f%%", percentage);
        m_percentage.SetFontSize(GetWindowFontSize(UIBookmarkCommentIndex));
		m_percentage.SetEnglishGothic();
        m_percentage.SetForeColor(ColorManager::GetColor(COLOR_KERNEL_DISABLECOLOR));
        m_percentage.SetText(cText);
    }
    m_percentage.SetVisible(showPercent);

    if (isComment)
    {
        string strComment(StringManager::GetPCSTRById(TOUCH_COMMENTITEM));
        strComment += m_pItemModel->GetUserComment();
        m_comment.SetFontSize(GetWindowFontSize(UIBookmarkCommentIndex));
        m_comment.SetLineSpace(1);
        //m_comment.MoveWindow(0, m_iHeight - BOTTOMMARGIN - USERCOMMENT, m_iWidth, USERCOMMENT);
        m_comment.SetText(strComment.c_str());
    }
    m_comment.SetVisible(isComment);
    if (m_windowSizer)
    {
        m_fileContent.SetMinWidth(m_minWidth);
        m_comment.SetMinWidth(m_minWidth);

        UISizer* timepercentSizer = new UIBoxSizer(dkHORIZONTAL);
        timepercentSizer->Add(&m_percentage, UISizerFlags(1).Align(dkALIGN_CENTER_HORIZONTAL));
        timepercentSizer->Add(&m_bookmarkTime, UISizerFlags(1).Align(dkALIGN_CENTER_HORIZONTAL));

        m_windowSizer->Add(&m_fileContent, UISizerFlags(1).Expand().Border(dkTOP, GetWindowMetrics(UIBookMarkContentLabelBorderIndex)));
        m_windowSizer->Add(timepercentSizer, UISizerFlags().Expand().Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIBookMarkTimeLabelBorderIndex)));
        m_windowSizer->Add(&m_comment, UISizerFlags().Expand().Border(dkBOTTOM, GetWindowMetrics(UIBookMarkBottomBorderIndex)));
    }
}

