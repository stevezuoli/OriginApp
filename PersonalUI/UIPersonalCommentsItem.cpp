#include <tr1/functional>
#include "PersonalUI/UIPersonalCommentsItem.h"
#include "BookStoreUI/UIBookStoreBookCommentDetailedPage.h"
#include "BookStoreUI/UIBookDetailedInfoPage.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "BookStore/BookStoreInfoManager.h"
#include "CommonUI/CPageNavigator.h"
#include "drivers/DeviceInfo.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Utility/StringUtil.h" 

using namespace WindowsMetrics;
using dk::bookstore::model::BookComment;
using namespace dk::bookstore;
using dk::utility::StringUtil;

UIPersonalCommentsItem::UIPersonalCommentsItem()
    : m_star(this, 0)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventVoteBookCommentUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalCommentsItem::OnMessageVoteBookComment),
        this,
        std::tr1::placeholders::_1)
        );

#ifdef KINDLE_FOR_TOUCH
    SubscribeEvent(UIWindow::EventClick,
            m_bookCover,
            std::tr1::bind(
                std::tr1::mem_fn(&UIPersonalCommentsItem::OnMessageBookCoverTouch),
                this,
                std::tr1::placeholders::_1)
            );
#endif
    Init();
}

UIPersonalCommentsItem::~UIPersonalCommentsItem()
{

}

void UIPersonalCommentsItem::Init()
{
    const int titleFont = GetWindowFontSize(UIPersonalCommentsItemTitleIndex);
    const int infoFont = GetWindowFontSize(UIPersonalCommentsItemInfoIndex);
    const int noteFont = GetWindowFontSize(UIPersonalCommentsItemNoteIndex);
    const int contentFont = GetWindowFontSize(UIPersonalCommentsItemContentIndex);

    m_title.SetFontSize(titleFont);
    m_title.SetVAlign(VALIGN_TOP);
    AppendChild(&m_title);

    m_commenterNick.SetFontSize(infoFont);
    AppendChild(&m_commenterNick);

    m_publishTime.SetFontSize(infoFont);
    AppendChild(&m_publishTime);

    m_usefulCount.SetFontSize(noteFont);
    AppendChild(&m_usefulCount);

    m_replyCount.SetFontSize(noteFont);
    m_replyCount.SetAlign(ALIGN_RIGHT);
    AppendChild(&m_replyCount);
    
    m_deviceType.SetFontSize(noteFont);
    AppendChild(&m_deviceType);

    m_content.SetFontSize(contentFont);
    m_content.SetMode(UIAbstractText::TextMultiLine);
    m_content.SetVAlign(VALIGN_TOP);
    m_content.SetMaxWidth(GetWindowMetrics(UIPersonalCommentsItemContentWidthIndex));
    AppendChild(&m_content);
    AppendChild(&m_bookCover);
#ifndef KINDLE_FOR_TOUCH
    m_bookCover.SetEnable(false);
#endif
    m_star.SetEnable(false);
}

void UIPersonalCommentsItem::InitUI()
{
    UISizer* mainSizer = new UIBoxSizer(dkHORIZONTAL);
    if (mainSizer)
    {
        const int topMargin = GetWindowMetrics(UIPersonalCommentsItemTopBorderIndex);
        m_bookCover.SetMinSize(GetWindowMetrics(UIPersonalCommentsItemCoverWidthIndex), GetWindowMetrics(UIPersonalCommentsItemCoverHeightIndex));
        mainSizer->Add(&m_bookCover, UISizerFlags().Border(dkTOP, topMargin));
        mainSizer->AddSpacer(GetWindowMetrics(UIPersonalCommentsItemCoverRightBorderIndex));

        UISizer* rightSizer = new UIBoxSizer(dkVERTICAL);
        if (rightSizer)
        {
            rightSizer->Add(&m_title, UISizerFlags().Border(dkBOTTOM, GetWindowMetrics(UIPixelValue10Index)));
            UISizer* rightMiddleSizer = new UIBoxSizer(dkHORIZONTAL);
            if (rightMiddleSizer)
            {
                rightMiddleSizer->Add(&m_star, UISizerFlags().Border(dkRIGHT, GetWindowMetrics(UIPersonalCommentsItemElementBorderIndex)));
                rightMiddleSizer->Add(&m_commenterNick, UISizerFlags().Border(dkRIGHT, GetWindowMetrics(UIPersonalCommentsItemElementBorderIndex)));
                rightMiddleSizer->Add(&m_publishTime);
                rightSizer->Add(rightMiddleSizer, UISizerFlags().Border(dkBOTTOM, GetWindowMetrics(UIPixelValue13Index)));
            }
            rightSizer->Add(&m_content, UISizerFlags(1).Expand());
            UISizer* bottomSizer = new UIBoxSizer(dkHORIZONTAL);
            if (bottomSizer)
            {
                bottomSizer->Add(&m_deviceType, 1);
                bottomSizer->Add(&m_usefulCount);
                bottomSizer->AddSpacer(GetWindowMetrics(UIPersonalCommentsItemBottomElementBorderIndex));
                bottomSizer->Add(&m_replyCount);
                rightSizer->Add(bottomSizer, UISizerFlags().Expand().Border(dkTOP, GetWindowMetrics(UIPixelValue10Index)));
            }
            mainSizer->Add(rightSizer, UISizerFlags(1).Expand().Border(dkTOP, topMargin).Border(dkBOTTOM, GetWindowMetrics(UIPixelValue10Index)));
        }

        SetSizer(mainSizer);
    }
}

void UIPersonalCommentsItem::SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr commentInfo, int index)
{
    m_bookComment.reset((model::BookComment*)commentInfo.get()->Clone());
    model::BookComment* bookComment = (model::BookComment*)(m_bookComment.get());
    if(NULL != bookComment)
    {
        char szTmp[64] = {0};
        //m_bookName.SetText(bookComment->GetBookName().c_str());
        m_title.SetText(bookComment->GetTitle());
        string nick = bookComment->GetCommenterNick();
        StringUtil::HideEmailInfo(nick);
        m_commenterNick.SetText(nick.c_str());

        if (bookComment->GetDeviceType().compare("Web") == 0)
        {
            m_deviceType.SetText(StringManager::GetPCSTRById(COME_FROM_WEB));
        }
        else
        {
            snprintf(szTmp, DK_DIM(szTmp), StringManager::GetPCSTRById(COME_FROM_WHERE), bookComment->GetDeviceType().c_str());
            m_deviceType.SetText(szTmp);
        }
        m_bookCover.SetCoverUrl(bookComment->GetBookCoverUrl());
        m_star.SetScore((double)(bookComment->GetScore()));

        snprintf(szTmp, DK_DIM(szTmp), "%s(%d)", StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_USEFULL), bookComment->GetUsefulCount());
        m_usefulCount.SetText(szTmp);

        snprintf(szTmp, DK_DIM(szTmp), "%s(%d)", StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_REPLY), bookComment->GetReplyCount());
        m_replyCount.SetText(szTmp);

        time_t currentUTCTime = time(NULL);
        int publishUTCTime = bookComment->GetPublishUTCTime();
        int timeSoFar = (currentUTCTime - publishUTCTime);
        timeSoFar -= 28800;
        if (timeSoFar < 0)
        {
            snprintf(szTmp, DK_DIM(szTmp), " - %s", bookComment->GetPublishTime().c_str());
        }
        else if (timeSoFar < 60)
        {
            snprintf(szTmp, DK_DIM(szTmp), " - %d%s", 1, StringManager::GetPCSTRById(PERSONAL_MINUTE_IN));
        }
        else if (timeSoFar < 60*60)
        {
            snprintf(szTmp, DK_DIM(szTmp), " - %d%s", timeSoFar/60, StringManager::GetPCSTRById(PERSONAL_MINUTE_BEFORE));
        }
        else if (timeSoFar < 60*60*24)
        {
            snprintf(szTmp, DK_DIM(szTmp), " - %d%s", timeSoFar/3600, StringManager::GetPCSTRById(PERSONAL_HOUR_BEFORE));
        }
        else if (timeSoFar >= 60*60*24)
        {
            snprintf(szTmp, DK_DIM(szTmp), " - %s", bookComment->GetPublishTime().c_str());
        }
        m_publishTime.SetText(szTmp);

        m_content.SetText(bookComment->GetContent().c_str());
        InitUI();
    }
}

bool UIPersonalCommentsItem::OnMessageBookCoverTouch(const EventArgs& args)
{
    ShowBookDetailedInfoPage();
    return true;
}

bool UIPersonalCommentsItem::ResponseTouchTap()
{
    if(m_bookComment)
    {
        UIBookStoreBookCommentDetailedPage *pDetailedPage = 
            new UIBookStoreBookCommentDetailedPage(m_bookComment->GetBookId(), m_bookComment->GetBookName());
        if(pDetailedPage)
        {
            pDetailedPage->SetCommentInfo(m_bookComment);
            pDetailedPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
            CPageNavigator::Goto(pDetailedPage);
        }
    }
    return true;
}

void UIPersonalCommentsItem::ShowBookDetailedInfoPage()
{
    if (m_bookComment)
    {
        UIPage* pPage = new UIBookDetailedInfoPage(m_bookComment->GetBookId(), m_bookComment->GetBookName());
        if(pPage)
        {
            pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
            CPageNavigator::Goto(pPage);
        }
    }
}

#ifndef KINDLE_FOR_TOUCH
bool UIPersonalCommentsItem::ResponseOperation()
{
    ShowBookDetailedInfoPage();
    return true;
}
#endif

bool UIPersonalCommentsItem::OnMessageVoteBookComment(const EventArgs& args)
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


