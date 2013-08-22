#include <tr1/functional>
#include "BookStoreUI/UIBookStoreBookCommentItem.h"
#include "BookStoreUI/UIBookStoreBookCommentDetailedPage.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "BookStore/BookStoreInfoManager.h"
#include "CommonUI/CPageNavigator.h"
#include "drivers/DeviceInfo.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Utility/StringUtil.h" 

using dk::utility::StringUtil;
using namespace WindowsMetrics;
using dk::bookstore::model::BookComment;
using namespace dk::bookstore;

UIBookStoreBookCommentItem::UIBookStoreBookCommentItem()
    : m_star(this, 0)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventVoteBookCommentUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreBookCommentItem::OnMessageVoteBookComment),
        this,
        std::tr1::placeholders::_1)
        );
    Init();
}

UIBookStoreBookCommentItem::~UIBookStoreBookCommentItem()
{

}

void UIBookStoreBookCommentItem::Init()
{
    const int fontsize12 = GetWindowFontSize(FontSize12Index);
    const int fontsize16 = GetWindowFontSize(FontSize16Index);
    const int fontsize20 = GetWindowFontSize(FontSize20Index);
    const int pixelValue55 = GetWindowMetrics(UIPixelValue55Index);
    m_title.SetFontSize(fontsize20);
    AppendChild(&m_title);

    m_commenterNick.SetFontSize(fontsize16);
    AppendChild(&m_commenterNick);

    m_publishTime.SetFontSize(fontsize16);
    AppendChild(&m_publishTime);

    m_viewAll.SetFontSize(fontsize12);
    m_viewAll.SetText(StringManager::GetStringById(BOOKSTORE_VIEW_ALL) + " >>");
    AppendChild(&m_viewAll);

    m_usefulCount.SetFontSize(fontsize12);
    AppendChild(&m_usefulCount);

    m_replayCount.SetFontSize(fontsize12);
    m_replayCount.SetAlign(ALIGN_RIGHT);
    AppendChild(&m_replayCount);
    
    m_content.SetFontSize(fontsize16);
    m_content.SetMode(UIAbstractText::TextMultiLine);
    m_content.SetVAlign(ALIGN_LEFT);
	m_content.SetMinHeight(pixelValue55);
    AppendChild(&m_content);

    m_star.SetEnable(false);
}

void UIBookStoreBookCommentItem::InitUI()
{
    const int elemSpacing = GetWindowMetrics(UIElementSpacingIndex);
    const int elemTitleMargin = GetWindowMetrics(UIPixelValue15Index);
    const int elemMidSpacing = GetWindowMetrics(UIElementMidSpacingIndex);
    const int elemLargeSpacing = GetWindowMetrics(UIElementLargeSpacingIndex);

    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);

    UISizer* starSizer = new UIBoxSizer(dkHORIZONTAL);
    starSizer->Add(&m_commenterNick);
    starSizer->AddSpacer(elemLargeSpacing);
    starSizer->Add(&m_star,UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
    starSizer->AddStretchSpacer();
    starSizer->Add(&m_publishTime,UISizerFlags().Align(dkALIGN_RIGHT));

    UISizer* viewAllSizer = new UIBoxSizer(dkHORIZONTAL);
    viewAllSizer->Add(&m_viewAll,UISizerFlags().Align(dkALIGN_LEFT));
    viewAllSizer->AddStretchSpacer();
    viewAllSizer->Add(&m_usefulCount,UISizerFlags().Align(dkALIGN_RIGHT));
    viewAllSizer->AddSpacer(elemLargeSpacing);
    viewAllSizer->Add(&m_replayCount,UISizerFlags().Align(dkALIGN_RIGHT));

    mainSizer->Add(&m_title,UISizerFlags().Border(dkTOP | dkBOTTOM, elemTitleMargin).Expand());
    mainSizer->Add(starSizer,UISizerFlags().Expand());
    mainSizer->Add(&m_content,UISizerFlags(1).Border(dkTOP, elemSpacing).Expand());
    mainSizer->Add(viewAllSizer,UISizerFlags().Border(dkTOP | dkBOTTOM, elemMidSpacing).Expand().Align(dkALIGN_BOTTOM));
    SetSizer(mainSizer);
    Layout();
}

void UIBookStoreBookCommentItem::SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr commentInfo, int index)
{
    if (commentInfo)
    {
        m_bookComment.reset((model::BookComment*)commentInfo.get()->Clone());
        m_title.SetText(m_bookComment->GetTitle().c_str());

        string nick = m_bookComment->GetCommenterNick();
        StringUtil::HideEmailInfo(nick);
        m_commenterNick.SetText(nick.c_str());

        m_publishTime.SetText(m_bookComment->GetPublishTime().c_str());
        m_star.SetScore((double)(m_bookComment->GetScore()));
        char szTmp[64] = {0};

        sprintf(szTmp,"%s(%d)",StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_USEFULL), m_bookComment->GetUsefulCount());
        m_usefulCount.SetText(szTmp);

        sprintf(szTmp,"%s(%d)",StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_REPLY), m_bookComment->GetReplyCount());
        m_replayCount.SetText(szTmp);

        m_content.SetText(m_bookComment->GetContent().c_str());
        InitUI();
    }
}

bool UIBookStoreBookCommentItem::ResponseTouchTap()
{
    if(m_bookComment)
    {
        const string bookId = m_bookComment->GetBookId();
        UIBookStoreBookCommentDetailedPage *pDetailedPage = new UIBookStoreBookCommentDetailedPage(bookId, model::BookInfo::BookIdToTitle(bookId.c_str()), true);
        if(pDetailedPage)
        {
            pDetailedPage->SetCommentInfo(m_bookComment);
            pDetailedPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
            CPageNavigator::Goto(pDetailedPage);
        }
    }
    return true;
}

bool UIBookStoreBookCommentItem::OnMessageVoteBookComment(const EventArgs& args)
{
    const VoteBookCommentUpdateArgs& voteArgs = dynamic_cast<const VoteBookCommentUpdateArgs&>(args);
    bool successed = voteArgs.succeeded;
    if (m_bookComment && (voteArgs.bookId == m_bookComment->GetBookId()) && (voteArgs.commentId == m_bookComment->GetCommentId()))
    {
        if (successed)
        {
            char szTmp[256] = {0};
            snprintf(szTmp, 256, "%s(%d)", StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_USEFULL), voteArgs.useful);
            m_usefulCount.SetText(szTmp);
            Layout();
        }
    }
    return successed;
}

