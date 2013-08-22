#include <tr1/functional>
#include "BookStoreUI//UIBookStoreCommentReplyDetailedPage.h"
#include "Framework/CDisplay.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Utility/ColorManager.h"
#include "CommonUI/CPageNavigator.h"
#include "Utility/StringUtil.h" 

using dk::utility::StringUtil;
using namespace DkFormat;
using namespace WindowsMetrics;

UIBookStoreCommentReplyDetailedPage::UIBookStoreCommentReplyDetailedPage(std::string bookName, const dk::bookstore::model::CommentReplySPtr replyInfo)
    : UIBookStoreNavigationWithFling(bookName, false)
{
    if (replyInfo)
    {
        string nick = replyInfo->GetReplierNick();
        StringUtil::HideEmailInfo(nick);
        m_userId.SetText(nick.c_str());
        m_replyTime.SetText(replyInfo->GetPublishTime().c_str());
        m_replyContent.SetText(replyInfo->GetContent().c_str());
    }
    Init();
}

UIBookStoreCommentReplyDetailedPage::~UIBookStoreCommentReplyDetailedPage()
{

}

void UIBookStoreCommentReplyDetailedPage::Init()
{
    const int pageNumberFontsize = GetWindowFontSize(FontSize16Index);
    const int fontsize18 = GetWindowFontSize(FontSize18Index);
    const int fontsize20 = GetWindowFontSize(FontSize20Index);

    m_userId.SetFontSize(fontsize18);
    m_userId.SetForeColor(ColorManager::GetColor(COLOR_DISABLECOLOR));
    AppendChild(&m_userId);

    m_replyTime.SetFontSize(fontsize18);
    m_replyTime.SetForeColor(ColorManager::GetColor(COLOR_DISABLECOLOR));
    AppendChild(&m_replyTime);

    m_replyContent.SetFontSize(fontsize20);
    AppendChild(&m_replyContent);

    m_pageNumber.SetFontSize(pageNumberFontsize);
	m_pageNumber.SetEnglishGothic();
    m_pageNumber.SetAlign(ALIGN_RIGHT);
    AppendChild(&m_pageNumber);
}

UISizer* UIBookStoreCommentReplyDetailedPage::CreateMidSizer()
{
    if (NULL == m_pMidSizer)
    {
        m_pMidSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pTitleNoteSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pMidSizer && pTitleNoteSizer)
        {
            const int titleNoteMargin = GetWindowMetrics(UIPixelValue20Index);
            pTitleNoteSizer->Add(&m_userId, UISizerFlags(1).Align(dkALIGN_CENTRE_VERTICAL));
            pTitleNoteSizer->AddStretchSpacer();
            pTitleNoteSizer->Add(&m_replyTime, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL));

            m_replyContent.SetMaxWidth(m_iWidth - (m_iLeftMargin << 1));
            m_pMidSizer->Add(pTitleNoteSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkTOP | dkBOTTOM, titleNoteMargin));
            m_pMidSizer->Add(&m_replyContent, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin));
            m_pMidSizer->Add(&m_pageNumber, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIPixelValue10Index)));
        }
        else
        {
            SafeDeletePointer(pTitleNoteSizer);
            SafeDeletePointer(m_pMidSizer);
        }
    }
    return m_pMidSizer;
}

void UIBookStoreCommentReplyDetailedPage::SetFetchStatus(FETCH_STATUS status)
{
    if (m_fetchStatus != status)
    {
        UIBookStoreNavigationWithFling::SetFetchStatus(FETCH_SUCCESS);
        m_totalPage = m_replyContent.GetPageCount();
        UpdatePageNumber();
    }
}

void UIBookStoreCommentReplyDetailedPage::UpdatePageNumber()
{
    char szTmp[64] = {0};
    sprintf(szTmp, "%d / %d %s", m_curPage + 1, m_totalPage, StringManager::GetPCSTRById(BOOK_PAGE));
    m_pageNumber.SetText(szTmp);
}

bool UIBookStoreCommentReplyDetailedPage::TurnPage(bool pageDown)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (UIBookStoreNavigationWithFling::TurnPage(pageDown))
    {
        m_replyContent.SetDrawPageNo(m_curPage);
        UpdateWindow();
        return true;
    }
    return false;
}
