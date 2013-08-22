#include <tr1/functional>
#include "BookStoreUI/UIBookStoreBookCommentDetailedPage.h"
#include "BookStoreUI/UIBookStoreBookCommentReplyItem.h"
#include "BookStoreUI/UIBookStoreBookCommentReplyPage.h"
#include "BookStoreUI/UIBookStoreCommentReplyDetailedPage.h"
#include "BookStoreUI/UIBookDetailedInfoPage.h"
#include "BookStore/BookStoreInfoManager.h"
#include "Framework/CDisplay.h"
#include "I18n/StringManager.h"
#include "drivers/DeviceInfo.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "Utility/StringUtil.h" 

using dk::utility::StringUtil;
using namespace dk::bookstore;
using namespace dk::utility;
using namespace WindowsMetrics;

UIBookStoreBookCommentDetailedPage::UIBookStoreBookCommentDetailedPage(const std::string& bookId,const std::string& bookName, bool showRefresh)
    : UIBookStoreNavigationWithFling(bookName, false)
    , m_star(this, 0)
    , m_bookId(bookId)
    , m_bookName(bookName)
    , m_showRefresh(showRefresh)
    , moveWindowDown(false)
    , setCommentInfoDown(false)
{
    SubscribeEvent(UITouchCommonListBox::EventListBoxSelectedItemChange, 
        m_replyList,
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreBookCommentDetailedPage::OnMessageListBoxSelectedItemClicked),
        this,
        std::tr1::placeholders::_1)
        ); 
    SubscribeMessageEvent(BookStoreInfoManager::EventVoteBookCommentUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreBookCommentDetailedPage::OnMessageVoteBookComment),
        this,
        std::tr1::placeholders::_1)
        );
#ifdef KINDLE_FOR_TOUCH
    SubscribeEvent(UITouchCommonListBox::EventListBoxTurnPage, 
        m_replyList,
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreBookCommentDetailedPage::OnMessageListBoxTurnPage),
        this,
        std::tr1::placeholders::_1)
        );
#endif
    Init();
}

UIBookStoreBookCommentDetailedPage::~UIBookStoreBookCommentDetailedPage()
{

}

bool UIBookStoreBookCommentDetailedPage::OnMessageListBoxTurnPage(const EventArgs& args)
{
#ifdef KINDLE_FOR_TOUCH
    const TouchCommonListBoxTurnPageEventArgs& msgArgs = 
        dynamic_cast<const TouchCommonListBoxTurnPageEventArgs&>(args);
    TurnPage(msgArgs.isDownPage);
#endif
    return true;
}

bool UIBookStoreBookCommentDetailedPage::OnMessageVoteBookComment(const EventArgs& args)
{
    const VoteBookCommentUpdateArgs& voteArgs = dynamic_cast<const VoteBookCommentUpdateArgs&>(args);
    bool successed = voteArgs.succeeded;
    if (voteArgs.bookId == m_bookId && voteArgs.commentId == m_bookComment->GetCommentId())
    {
        if (successed)
        {
            UIUtility::SetScreenTips(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_VOTE_SUCCESS));

            char szTmp[256] = {0};
            snprintf(szTmp, 256, "%s(%d)", StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_USEFULL), voteArgs.useful);
            m_usefulButton.SetText(szTmp);
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            UpdateWindow();
        }
        else
        {
            UIUtility::SetScreenTips(StringManager::GetPCSTRById((BSE_COMMENT_REPEATED_VOTE == voteArgs.errorCode) ? BOOKSTORE_BOOKCOMMENT_VOTE_REPEATED : BOOKSTORE_BOOKCOMMENT_VOTE_FAIL));
        }
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    }
    return successed;
}

bool UIBookStoreBookCommentDetailedPage::OnMessageListBoxSelectedItemClicked(const EventArgs& args)
{
    const TouchCommonListBoxSelectedChangedEventArgs& msgArgs = 
        dynamic_cast<const TouchCommonListBoxSelectedChangedEventArgs&>(args);
    size_t selectedItem = msgArgs.m_selectedItem;
    if(selectedItem >= 0 && selectedItem < m_replyList.GetChildrenCount())
    {
        VIEW_ATTRIBUTE attribute = m_layoutIndex[m_curPage].GetAppointViewAttribute(&m_replyList);
        if (!attribute.pView)
        {
            return false;
        }
        int curIndex = attribute.listboxStartIndex + selectedItem;
        if (curIndex >= m_bookComment->GetReplyCount() || curIndex < 0)
        {
            return false;
        }
        UIBookStoreCommentReplyDetailedPage* bookInfoPage = new UIBookStoreCommentReplyDetailedPage(m_bookName,m_bookComment->GetReplyList()[curIndex]); 
        bookInfoPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
        CPageNavigator::Goto(bookInfoPage);
         return true;
    }
    return false;
}


void UIBookStoreBookCommentDetailedPage::VoteBookComment()
{
    BookStoreInfoManager *pManager = BookStoreInfoManager::GetInstance();
    if (pManager)
    {
        pManager->VoteBookComment(m_bookId.c_str(), m_bookComment->GetCommentId().c_str(), true);
    }
}

void UIBookStoreBookCommentDetailedPage::ViewBookInfo()
{
    if(UIUtility::CheckAndReLogin())
    {
        UIBookDetailedInfoPage* bookInfoPage = new UIBookDetailedInfoPage(m_bookId, m_bookName);
        if (bookInfoPage)
        {
            bookInfoPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
            CPageNavigator::Goto(bookInfoPage);
        }
    }
}

UISizer* UIBookStoreBookCommentDetailedPage::CreateMidSizer()
{
    if (NULL == m_pMidSizer)
    {
        m_pMidSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pCommentTitleSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* pInfoSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pMidSizer && pCommentTitleSizer && pInfoSizer)
        {
            const int elemspacing = GetWindowMetrics(UIPixelValue15Index);
            const int commentTitleTopMargin = GetWindowMetrics(UIPixelValue30Index);
            const int commentTitleBottomMargin = GetWindowMetrics(UIPixelValue26Index);

            pCommentTitleSizer->Add(&m_commentTitle, UISizerFlags(1).Align(dkALIGN_CENTRE_VERTICAL).Border(dkRIGHT, elemspacing));
            pCommentTitleSizer->Add(&m_usefulButton, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL));

            pInfoSizer->Add(&m_userId, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL).Border(dkRIGHT, elemspacing));
            pInfoSizer->Add(&m_star, UISizerFlags(1).Align(dkALIGN_CENTRE_VERTICAL));
            pInfoSizer->Add(&m_commentTime, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL).Border(dkLEFT, elemspacing));

            m_pMidSizer->Add(pCommentTitleSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkTOP, commentTitleTopMargin).Border(dkBOTTOM, commentTitleBottomMargin));
            m_pMidSizer->Add(pInfoSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkBOTTOM, elemspacing));
            m_pMidSizer->AddStretchSpacer();
            m_pMidSizer->Add(&m_pageNumberLabel, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkTOP | dkBOTTOM, elemspacing));
        }
        else
        {
            SafeDeletePointer(m_pMidSizer);
            SafeDeletePointer(pCommentTitleSizer);
            SafeDeletePointer(pInfoSizer);
        }
    }
    return m_pMidSizer;
}

UISizer* UIBookStoreBookCommentDetailedPage::CreateNavigationRightSizer()
{
    if (m_showRefresh)
    {
        return UIBookStoreNavigationWithFling::CreateNavigationRightSizer();
    }
    if (NULL == m_pNavigationRightSizer)
    {
        m_pNavigationRightSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pNavigationRightSizer)
        {
#ifdef KINDLE_FOR_TOUCH
            const int btnHorizontalMargin = GetWindowMetrics(UIBtnDefaultHorizontalMarginIndex);
            const int btnVerticalMargin = GetWindowMetrics(UIBtnDefaultVerticalMarginIndex);
            m_btnRefresh.Initialize(ID_BOOKSTORE_VIEW_BOOKINFO, StringManager::GetPCSTRById(VIEW_BOOKINFO), GetWindowFontSize(FontSize20Index));
            m_pNavigationRightSizer->Add(&m_btnRefresh
                , UISizerFlags().Center().Border(dkRIGHT, GetWindowMetrics(UIBookStoreNavigationRefreshRightMarginIndex)));
#else
            const int btnHorizontalMargin = 0;
            const int btnVerticalMargin = 0;
            m_btnRefresh.Initialize(ID_BOOKSTORE_VIEW_BOOKINFO, StringManager::GetPCSTRById(VIEW_BOOKINFO)
                , 'B', GetWindowFontSize(FontSize20Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
            m_pNavigationRightSizer->Add(&m_btnRefresh, UISizerFlags().Center());
#endif
            m_btnRefresh.SetLeftMargin(btnHorizontalMargin);
            m_btnRefresh.SetTopMargin(btnVerticalMargin);
            AppendChild(&m_btnRefresh);
        }
    }
    return m_pNavigationRightSizer;
}

void UIBookStoreBookCommentDetailedPage::SetFetchStatus(FETCH_STATUS status)
{
    if (m_fetchStatus != status)
    {
        UIBookStoreNavigationWithFling::SetFetchStatus(FETCH_SUCCESS);
    }
}

void UIBookStoreBookCommentDetailedPage::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
    UIBookStoreNavigationWithFling::MoveWindow(iLeft, iTop, iWidth, iHeight);
    if (m_showRefresh)
    {
        m_btnRefresh.SetVisible(false);
    }
    moveWindowDown = true;
    ReLayout();
}

void UIBookStoreBookCommentDetailedPage::UpdatePageView()
{
    const int itemHeight = GetWindowMetrics(UIBookStoreBookCommentReplyItemHeightIndex);
    const int horizontalMargin = GetWindowMetrics(UIHorizonMarginIndex);
    for (vector<VIEW_ATTRIBUTE>::iterator itr = m_layoutIndex[m_curPage].viewList.begin(); itr != m_layoutIndex[m_curPage].viewList.end(); itr++)
    {
        UIWindow *pView = (*itr).pView;
        pView->SetVisible((*itr).isEnable);
        pView->MoveWindow((*itr).left, (*itr).top, (*itr).width, (*itr).height);
        if (pView->IsVisible() && ((*itr).listboxStartIndex || (*itr).listboxEndIndex))
        {
            UIBookStoreBookCommentReplyItem *pItem = NULL;
            int topOffset = 0;
            int i = 0;
            for (i = (*itr).listboxStartIndex; i < (*itr).listboxEndIndex; i++)
            {
                if ((i - (*itr).listboxStartIndex) == (int)m_replyList.GetChildrenCount())
                {
                    pItem = new UIBookStoreBookCommentReplyItem();
                    if(pItem)
                    {
                        m_replyList.AddItem(pItem);
                    }
                }
                else
                {
                    pItem = (UIBookStoreBookCommentReplyItem *)m_replyList.GetChildByIndex(i - (*itr).listboxStartIndex);
                }
                if (pItem)
                {
                    dk::bookstore::model::CommentReplySPtr objectItem = m_bookComment->GetReplyList()[i];
                    pItem->SetVisible(TRUE);
                    pItem->MoveWindow(0, topOffset, m_iWidth - (horizontalMargin << 1), itemHeight);
                    pItem->SetInfoSPtr(objectItem);
                    topOffset += itemHeight;
                }
            }
            for(i = i - (*itr).listboxStartIndex;i < (int)m_replyList.GetChildrenCount();i++)
            {
                pItem = (UIBookStoreBookCommentReplyItem *)m_replyList.GetChildByIndex(i);
                if(pItem)
                {
                    pItem->SetVisible(FALSE);
                }
                else
                {
                    break;
                }
            }

        }
    }
    if (m_commentContent.IsVisible())
    {
        m_commentContent.SetDrawPageNo(m_curPage);
    }
    Layout();
    UpdatePageNumber();
    m_replyList.ResetVisibleItemNum();
#ifndef KINDLE_FOR_TOUCH
    m_usefulButton.SetFocus(true);
#endif
}

HRESULT UIBookStoreBookCommentDetailedPage::DrawBackGround(DK_IMAGE drawingImg)
{
    UpdatePageView();

    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(UIBookStoreNavigationWithFling::DrawBackGround(drawingImg));
    CTpGraphics grf(drawingImg);

    if (m_commentReplyLabel.IsVisible())
    {
        const int horizontalMargin = GetWindowMetrics(UIHorizonMarginIndex);
        int lineTop = m_commentReplyLabel.GetY() - GetWindowMetrics(UIPixelValue20Index);
        RTN_HR_IF_FAILED(grf.FillRect(horizontalMargin, 
            lineTop,
            m_iWidth - horizontalMargin,
            lineTop + 2, 
            ColorManager::GetColor(COLOR_DISABLECOLOR)));
    }
    return hr;
}

void UIBookStoreBookCommentDetailedPage::SetCommentInfo(const dk::bookstore::model::BookCommentSPtr bookComment)
{
    m_bookComment = dk::bookstore::model::BookCommentSPtr((dk::bookstore::model::BookComment *)bookComment->Clone());
    m_commentTitle.SetText(m_bookComment->GetTitle().c_str());

    char szTmp[256] = {0};
    snprintf(szTmp, 256, "%s(%d)", StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_USEFULL), m_bookComment->GetUsefulCount());
    m_usefulButton.SetText(szTmp);

    string nick = m_bookComment->GetCommenterNick();
    StringUtil::HideEmailInfo(nick);
    m_userId.SetText(nick.c_str());

    m_commentTime.SetText(m_bookComment->GetPublishTime().c_str());
    
    m_commentContent.SetText(m_bookComment->GetContent().c_str());
    //m_commentContent.SetText(StringManager::GetDemoString().c_str());

    snprintf(szTmp, 256, "%s%s(%d)", StringManager::GetPCSTRById(BOOKSTORE_BOOKREVIEW), StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_REPLY), m_bookComment->GetReplyCount());
    m_commentReplyLabel.SetText(szTmp);

    m_star.SetScore((double)(m_bookComment->GetScore()));

    setCommentInfoDown = true;
#ifndef KINDLE_FOR_TOUCH
    m_usefulButton.SetFocus(true);
#endif

    ReLayout();
}

void UIBookStoreBookCommentDetailedPage::Init()
{
    const int fontsize18 = GetWindowFontSize(FontSize18Index);
    const int useFontsize = GetWindowFontSize(FontSize14Index);
    const int replyLabelFontSize = GetWindowFontSize(FontSize20Index);
    const int commentTitleFontSize = GetWindowFontSize(FontSize22Index);
    const int btnVerticalMargin = GetWindowMetrics(UIBtnDefaultVerticalMarginIndex);

    m_commentTitle.SetFontSize(commentTitleFontSize);
    AppendChild(&m_commentTitle);

    char szTmp[64] = {0};
    snprintf(szTmp, 64, "%s%s",StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_REPLY), StringManager::GetPCSTRById(BOOKSTORE_BOOKREVIEW));

#ifdef KINDLE_FOR_TOUCH
    const int btnHorizontalMargin = GetWindowMetrics(UIBtnDefaultHorizontalMarginIndex);
    m_usefulButton.Initialize(ID_BTN_BOOKSTORE_BOOKCOMMENT_USEFUL, StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_USEFULL), commentTitleFontSize);
    m_replyButton.Initialize(ID_BTN_BOOKSTORE_BOOKCOMMENT_REPLY, szTmp, replyLabelFontSize);
#else
    const int btnHorizontalMargin = 0;

    m_usefulButton.Initialize(ID_BTN_BOOKSTORE_BOOKCOMMENT_USEFUL, StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_USEFULL), 'G', commentTitleFontSize, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_usefulButton.SetAlign(ALIGN_RIGHT);

    m_replyButton.Initialize(ID_BTN_BOOKSTORE_BOOKCOMMENT_REPLY, szTmp, 'R', replyLabelFontSize, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_replyButton.SetAlign(ALIGN_RIGHT);
#endif
    m_usefulButton.SetLeftMargin(btnHorizontalMargin);
    m_usefulButton.SetTopMargin(btnVerticalMargin);
    m_replyButton.SetLeftMargin(btnHorizontalMargin);
    m_replyButton.SetTopMargin(btnVerticalMargin);
    AppendChild(&m_usefulButton);
    AppendChild(&m_replyButton);

    m_userId.SetFontSize(useFontsize);
    AppendChild(&m_userId);

    m_commentTime.SetFontSize(useFontsize);
    AppendChild(&m_commentTime);
    
    m_commentContent.SetFontSize(fontsize18);
    m_commentContent.SetLineSpace(1);
    m_commentContent.SetEnable(false);
    AppendChild(&m_commentContent);

    m_commentReplyLabel.SetFontSize(replyLabelFontSize);
    AppendChild(&m_commentReplyLabel);

    const int itemHeight = GetWindowMetrics(UIBookStoreBookCommentReplyItemHeightIndex);
    m_replyList.SetItemHeight(itemHeight);
    AppendChild(&m_replyList);

    m_pageNumberLabel.SetFontSize(fontsize18);
    m_pageNumberLabel.SetAlign(ALIGN_RIGHT);
    AppendChild(&m_pageNumberLabel);

    m_star.SetEnable(false);
}

void UIBookStoreBookCommentDetailedPage::ReLayout()
{
    if (!moveWindowDown || !setCommentInfoDown)
    {
        return;
    }
    const int normalMargin = GetWindowMetrics(UIHorizonMarginIndex);
    const int elemspacing = GetWindowMetrics(UIElementSpacingIndex) << 1;

    int topOffset = 0, height = GetWindowMetrics(UIBookStoreBookCommentDetailedPageContentHeightIndex);
    int unUsedHeightTotal = 0, unUsedTopMargin = 0;
    UISizerItem* pItemCommentTime = m_windowSizer->GetItem(&m_commentTime, true);
    UISizerItem* pItemPageNumber = m_windowSizer->GetItem(&m_pageNumberLabel, true);
    if (pItemCommentTime && pItemPageNumber)
    {
        const dkRect rtCommentTime = pItemCommentTime->GetRect();
        const dkRect rtPageNumber = pItemPageNumber->GetRect();
        topOffset = unUsedTopMargin = rtCommentTime.GetBottom() + elemspacing;
        height = unUsedHeightTotal = rtPageNumber.GetTop() - elemspacing - topOffset;
    }

    m_commentContent.MoveWindow(normalMargin, topOffset, m_iWidth - (normalMargin << 1), height);

    int commentPageNumber = m_commentContent.GetPageCount();
    for (int i = 0; i < commentPageNumber - 1; i++)
    {
        LAYOUT_INDEX_ITEM indexItem;
        indexItem.InsertView(&m_userId, true);

        indexItem.InsertView(&m_star, true);

        indexItem.InsertView(&m_commentTime, true);
        
        indexItem.InsertView(&m_commentContent, true);

        indexItem.InsertView(&m_replyList,false);

        indexItem.InsertView(&m_commentReplyLabel,false);

        indexItem.InsertView(&m_replyButton,false);

        m_layoutIndex.push_back(indexItem);
    }
    
    LAYOUT_INDEX_ITEM indexItem;
    indexItem.InsertView(&m_userId, true);

    indexItem.InsertView(&m_star, true);

    indexItem.InsertView(&m_commentTime, true);
    
    indexItem.InsertView(&m_commentContent, true);

    const int replyBtnHeight = m_replyButton.GetMinHeight();
    int commentReplyLabelLineHeight = m_commentReplyLabel.GetTextHeight() >  replyBtnHeight ? m_commentReplyLabel.GetTextHeight() : replyBtnHeight;
    const int commentHeight = m_commentContent.GetPageHeightOfPageIndex(commentPageNumber - 1) + GetWindowMetrics(UIPixelValue48Index);
    topOffset += commentHeight;
    m_commentReplyLabel.MoveWindow(normalMargin, topOffset, m_commentReplyLabel.GetTextWidth(), m_commentReplyLabel.GetTextHeight());
    int leftOffset = m_iWidth - normalMargin - m_replyButton.GetMinWidth();
    m_replyButton.MoveWindow(leftOffset, topOffset, m_replyButton.GetMinWidth(), replyBtnHeight);
    int listboxSize = m_bookComment->GetReplyCount();
    if (listboxSize <= 0)
    {
        indexItem.InsertView(&m_commentReplyLabel, true);
        indexItem.InsertView(&m_replyButton,true);
        indexItem.InsertView(&m_replyList, false);
        m_layoutIndex.push_back(indexItem);
        m_totalPage = m_layoutIndex.size();
        UpdatePageNumber();
        return;
    }

    const int itemHeight = GetWindowMetrics(UIBookStoreBookCommentReplyItemHeightIndex);
    int unUsedHeight = unUsedHeightTotal - commentHeight;
    bool showListbox = false;

    // 判断只前页还能不能再放下listbox
    if (unUsedHeight > (commentReplyLabelLineHeight + itemHeight))
    {
        showListbox = true;
        topOffset += commentReplyLabelLineHeight;
        unUsedHeight -= commentReplyLabelLineHeight;
        m_replyList.MoveWindow(normalMargin, topOffset, m_iWidth - (normalMargin << 1), unUsedHeight);
    }


    indexItem.InsertView(&m_replyButton,showListbox);
    
    indexItem.InsertView(&m_commentReplyLabel, showListbox);

    int endIndex = unUsedHeight / itemHeight;
    if (endIndex > listboxSize)
    {
        endIndex = listboxSize;
    }

    indexItem.InsertView(&m_replyList, showListbox, 0, endIndex);

    m_layoutIndex.push_back(indexItem);

    topOffset = unUsedTopMargin + normalMargin;
    unUsedHeight = m_iHeight - topOffset - GetWindowMetrics(UIPixelValue60Index);
    m_commentReplyLabel.MoveWindow(normalMargin, topOffset, m_commentReplyLabel.GetTextWidth(), m_commentReplyLabel.GetTextHeight());
    leftOffset = m_iWidth - normalMargin - m_replyButton.GetMinWidth();
    m_replyButton.MoveWindow(leftOffset, topOffset, m_replyButton.GetMinWidth(), m_replyButton.GetMinHeight());
    const int replyLabelUsedHeight = commentReplyLabelLineHeight + GetWindowMetrics(UIPixelValue30Index);
    topOffset += replyLabelUsedHeight;
    unUsedHeight -= replyLabelUsedHeight;
    m_replyList.MoveWindow(normalMargin, topOffset, m_iWidth - normalMargin - normalMargin, unUsedHeight);
    int perPageNumber = unUsedHeight / itemHeight;
    int listboxPageNumber = (listboxSize + perPageNumber - 1 - endIndex) / perPageNumber;

    for (int i = 0; i < listboxPageNumber; i++)
    {
        LAYOUT_INDEX_ITEM indexItem1;

        indexItem1.InsertView(&m_userId, false);

        indexItem1.InsertView(&m_star, false);

        indexItem1.InsertView(&m_commentTime, false);
        
        indexItem1.InsertView(&m_commentContent, false);

        indexItem1.InsertView(&m_commentReplyLabel, true);

        indexItem1.InsertView(&m_replyButton, true);

        int startIndex = endIndex;
        endIndex = startIndex + perPageNumber > listboxSize ? listboxSize : startIndex + perPageNumber;

        indexItem1.InsertView(&m_replyList, true, startIndex, endIndex);
        m_layoutIndex.push_back(indexItem1);
    }
    m_totalPage = m_layoutIndex.size();
    UpdatePageNumber();
}

void UIBookStoreBookCommentDetailedPage::UpdatePageNumber()
{
    char szNum[64] = {0};
    snprintf(szNum, DK_DIM(szNum), "%d / %d", m_curPage + 1, m_totalPage);
    m_pageNumberLabel.SetText(szNum);
}

bool UIBookStoreBookCommentDetailedPage::TurnPage(bool pageDown)
{
    if (UIBookStoreNavigationWithFling::TurnPage(pageDown))
    {
        UpdateWindow();
        return true;
    }
    return false;
}

BOOL UIBookStoreBookCommentDetailedPage::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    if (m_mnuMain.IsVisible())
    {
        return UIBookStoreNavigation::OnKeyPress(iKeyCode);
    }

    switch(iKeyCode)
    {
    case KEY_UP:
        if (m_replyList.IsFocus())
        {
            if (m_replyList.OnKeyPress(iKeyCode))
            {
                m_replyList.SetFocus(false);
                if (m_replyButton.IsVisible())
                {
                    m_replyButton.SetFocus(true);
                }
                else
                {
                    m_usefulButton.SetFocus(true);
                }
            }
        }
        else if (m_replyButton.IsFocus())
        {
            m_usefulButton.SetFocus(true);
        }
        else if (m_usefulButton.IsFocus())
        {
            if (m_btnRefresh.IsVisible())
            {
                m_btnRefresh.SetFocus(true);
            }
            else if (m_replyList.GetVisibleItemNum() > 0)
            {
                m_replyList.SetSelectedItem(m_replyList.GetVisibleItemNum() - 1);
                m_replyList.SetFocus(true);
            }
            else
            {
                m_replyButton.SetFocus(true);
            }
        }
        else if (m_btnRefresh.IsFocus())
        {
            if (m_replyList.GetVisibleItemNum() > 0)
            {
                m_replyList.SetSelectedItem(m_replyList.GetVisibleItemNum() - 1);
                m_replyList.SetFocus(true);
            }
            else
            {
                m_replyButton.SetFocus(true);
            }
        }
        return false;
    case KEY_DOWN:
        if (m_replyList.IsFocus())
        {
            if (m_replyList.OnKeyPress(iKeyCode))
            {
                m_replyList.SetFocus(false);
                if (m_btnRefresh.IsVisible())
                {
                    m_btnRefresh.SetFocus(true);
                }
                else 
                {
                    m_usefulButton.SetFocus(true);
                }
            }
        }
        else if (m_usefulButton.IsFocus())
        {
            if (m_replyButton.IsVisible())
            {
                m_replyButton.SetFocus(true);
            }
            else if (m_replyList.GetVisibleItemNum() > 0)
            {
                m_replyList.SetSelectedItem(0);
                m_replyList.SetFocus(true);
            }
        }
        else if (m_replyButton.IsFocus())
        {
            if (m_replyList.GetVisibleItemNum() > 0)
            {
                m_replyList.SetSelectedItem(0);
                m_replyList.SetFocus(true);
            }
            else if (m_btnRefresh.IsVisible())
            {
                m_btnRefresh.SetFocus(true);
            }
            else 
            {
                m_usefulButton.SetFocus(true);
            }
        }
        else if (m_btnRefresh.IsFocus())
        {
            m_usefulButton.SetFocus(true);
        }
        return false;
    case KEY_LEFT:
    case KEY_RIGHT:
        return false;
    default:
        break;
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, UIBookStoreBookCommentDetailedPage, %s end", __FILE__,  __LINE__, __FUNCTION__);
#endif
    return UIBookStoreNavigationWithFling::OnKeyPress(iKeyCode);
}

void UIBookStoreBookCommentDetailedPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
    case ID_BTN_BOOKSTORE_BOOKCOMMENT_USEFUL:
        if(UIUtility::CheckAndReLogin())
        {
            VoteBookComment();
        }
        break;
    case ID_BTN_BOOKSTORE_BOOKCOMMENT_REPLY:
        {
            UIBookStoreBookCommentReplyPage* pReplyPage = new UIBookStoreBookCommentReplyPage(m_bookName, m_bookId, m_bookComment->GetCommentId()); 
            if (pReplyPage)
            {
                pReplyPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                CPageNavigator::Goto(pReplyPage);
            }
        }
        break;
    case ID_BOOKSTORE_VIEW_BOOKINFO:
        {
            ViewBookInfo();
            break;
        }
    default:
        UIBookStoreNavigationWithFling::OnCommand(dwCmdId, pSender, dwParam);
        break;
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}
