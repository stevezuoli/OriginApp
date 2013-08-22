#include <tr1/functional>
#include "BookStoreUI/UIBookStoreBookComment.h"
#include "BookStoreUI/UIBookStoreBookCommentItem.h"
#include "BookStoreUI/UIBookStoreAddCommentPage.h"
#include "BookStoreUI/UIBookStoreBookCommentDetailedPage.h"
#include "Framework/CDisplay.h"
#include "Framework/INativeMessageQueue.h"
#include "DownloadManager/DownloadManager.h"
#include "BookStore/DataUpdateArgs.h"
#include "DownloadManager/DownloaderImpl.h"
#include "DownloadManager/DownloadTaskFactory.h"
#include "I18n/StringManager.h"
#include "Utility/ImageManager.h"
#include "Utility/PathManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/MessageEventArgs.h"
#include "Common/CAccountManager.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;
using namespace dk::utility;
using namespace dk::account;
/********************************************
* UIBookStoreBookNoComment
********************************************/
UIBookStoreBookNoComment::UIBookStoreBookNoComment()
    : m_imgNoComment(this)
{
    Init();
}

UIBookStoreBookNoComment::~UIBookStoreBookNoComment()
{

}

void UIBookStoreBookNoComment::Init()
{
    std::string imagePath = ImageManager::GetImagePath(IMAGE_ICON_FACE);
    if (!imagePath.empty())
    {
        const int imgSize = GetWindowMetrics(ICON_FACE_SIZEIndex);
        m_imgNoComment.SetImageFile(imagePath);
        m_imgNoComment.SetMinSize(imgSize, imgSize);
    }
    AppendChild(&m_imgNoComment);

    m_noCommentLable.SetFontSize(GetWindowFontSize(FontSize20Index));
    m_noCommentLable.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_NOCOMMENT));
    m_noCommentLable.SetAlign(ALIGN_CENTER);
    AppendChild(&m_noCommentLable);

    m_addCommentLine.SetFontSize(GetWindowFontSize(FontSize28Index));
    m_addCommentLine.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_NOCOMMENT_INFO));
    m_addCommentLine.SetAlign(ALIGN_CENTER);
    AppendChild(&m_addCommentLine);

    m_addComment.Initialize(ID_BIN_BOOKSTORE_ADDCOMMENT ,NULL, 
#ifdef KINDLE_FOR_TOUCH
        GetWindowFontSize(FontSize20Index));
    m_addComment.SetBackground(ImageManager::GetImage(IMAGE_TOUCH_WEIBO_SHARE));
#else
        KEY_RESERVED, 
        GetWindowFontSize(FontSize20Index), 
        ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_addComment.SetFontSize(GetWindowFontSize(FontSize20Index));
#endif
    UpdateAddCommentStatus();
    AppendChild(&m_addComment);

    if (!m_windowSizer)
    {
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
        if (pMainSizer)
        {
            pMainSizer->AddSpacer(GetWindowMetrics(UIPixelValue95Index));
            pMainSizer->Add(&m_imgNoComment, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
            pMainSizer->AddSpacer(GetWindowMetrics(UIPixelValue50Index));
            pMainSizer->Add(&m_noCommentLable, UISizerFlags().Expand().Border(dkBOTTOM, GetWindowMetrics(UIPixelValue16Index)));
            pMainSizer->Add(&m_addCommentLine, UISizerFlags().Expand());
            pMainSizer->Add(&m_addComment, UISizerFlags().Expand().Border(dkTOP, GetWindowMetrics(UIPixelValue80Index)));
            pMainSizer->AddStretchSpacer();
            SetSizer(pMainSizer);
        }
    }
}

void UIBookStoreBookNoComment::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    if (m_pParent)
    {
        m_pParent->OnCommand(dwCmdId, pSender, dwParam);
    }
}

void UIBookStoreBookNoComment::SetFocus(BOOL bIsFocus)
{
    m_addComment.SetFocus(bIsFocus);
}

BOOL UIBookStoreBookNoComment::OnKeyPress(INT32 iKeyCode)
{
    if (((KEY_ENTER == iKeyCode) || (KEY_OKAY == iKeyCode)) && m_pParent && m_addComment.IsFocus())
    {
        m_pParent->OnCommand(m_addComment.GetId(), this, 0);
        return FALSE;
    }
    return TRUE;
}

void UIBookStoreBookNoComment::UpdateAddCommentStatus()
{
    CAccountManager* pAccountManager = CAccountManager::GetInstance();
    if (pAccountManager)
    {
        if (!pAccountManager->IsLoggedIn())
        {
            char szTmp[256] = {0};
            snprintf(szTmp, 256, "%s%s", StringManager::GetPCSTRById(AFTER_LOGIN), StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_ADDCOMMENT));
            m_addComment.SetText(szTmp);
        }
        else
        {
            m_addComment.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_ADDCOMMENT));
        }
    }
}

/********************************************
* UIBookStoreBookComment
********************************************/
static const int BOOKCOMMENT_NUMPERPAGE = 3;

UIBookStoreBookComment::UIBookStoreBookComment(const std::string& bookId, const std::string& bookName)
    : m_bookId(bookId)
    , m_bookName(bookName)
    , m_listBoxPanel(BOOKCOMMENT_NUMPERPAGE, GetWindowMetrics(UIBookStoreBookCommentItemHeightIndex))
    , m_fetchMyScoreReturn(NOFETCH)
    , m_fetchTotalScoreReturn(NOFETCH)
    , m_fetchListBoxReturn(NOFETCH)
    , m_isPurchased(false)
    , m_totalStar(this, 0)
    , m_myScore(this, 0)
    , m_prevSortId(-1)
    , m_curSortId(-1)
#ifndef KINDLE_FOR_TOUCH
    , m_mnuSort(this)
#endif
{
    SubscribeMessageEvent(BookStoreInfoManager::EventBookScoreUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreBookComment::OnMessageBookScoreUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreInfoManager::EventCommentListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreBookComment::OnMessageListBoxUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreInfoManager::EventBookScoreByCurrentUserUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreBookComment::OnMessageBookScoreByCurrentUserUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreInfoManager::EventAddBookScoreUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreBookComment::OnMessageAddBookScoreUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    SubscribeMessageEvent(CAccountManager::EventAccount, 
        *CAccountManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreBookComment::LogDataChanged),
        this,
        std::tr1::placeholders::_1));

    SubscribeMessageEvent(BookStoreInfoManager::EventVoteBookCommentUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreBookComment::OnMessageVoteBookComment),
        this,
        std::tr1::placeholders::_1)
        );

    Init();
}

UIBookStoreBookComment::~UIBookStoreBookComment()
{
}

void UIBookStoreBookComment::Init()
{
    const int fontsize16 = GetWindowFontSize(FontSize16Index);
    const int fontsize24 = GetWindowFontSize(FontSize24Index);
    const int myScoreStarsMargin = GetWindowMetrics(UIElementMidSpacingIndex);
    const int btnHorizontalMargin = GetWindowMetrics(UIBtnDefaultHorizontalMarginIndex);
    const int btnVerticalMargin = GetWindowMetrics(UIBtnDefaultVerticalMarginIndex);

    m_myScore.SetStarsIcons(ImageManager::GetImage(IMAGE_BOOKSTORE_STARBIG_SOLID),
                            ImageManager::GetImage(IMAGE_BOOKSTORE_STARBIG_HOLLOW));
    m_myScore.SetStarsMargin(myScoreStarsMargin);

    m_myScoreLable.SetFontSize(fontsize24);
    m_myScoreLable.SetForeColor(ColorManager::GetColor(COLOR_DISABLECOLOR));
    m_myScoreLable.SetText(StringManager::GetPCSTRById(BOOKSTORE_MYBOOKSCORE));
    AppendChild(&m_myScoreLable);

    m_myScoreInfoLable.SetFontSize(fontsize16);
    m_myScoreInfoLable.SetForeColor(ColorManager::GetColor(COLOR_DISABLECOLOR));
    AppendChild(&m_myScoreInfoLable);

#ifdef KINDLE_FOR_TOUCH
    m_addComment.Initialize(ID_BIN_BOOKSTORE_ADDCOMMENT,StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_ADDCOMMENT), fontsize16);
#else
    m_addComment.Initialize(ID_BIN_BOOKSTORE_ADDCOMMENT,StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_ADDCOMMENT), KEY_RESERVED, fontsize16, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
    m_addComment.SetLeftMargin(btnHorizontalMargin);
    m_addComment.SetTopMargin(btnVerticalMargin);
    m_addComment.SetAlign(ALIGN_RIGHT);
    AppendChild(&m_addComment);

#ifdef KINDLE_FOR_TOUCH
    m_btnSort.Initialize(ID_BTN_SORT, NULL, fontsize16);
    m_btnSort.SetIcon(ImageManager::GetImage(IMAGE_ARROW_DOWN), UIButton::ICON_RIGHT);
    m_btnSort.SetLeftMargin(btnHorizontalMargin);
    m_btnSort.SetTopMargin(btnVerticalMargin);
#else
    m_btnSort.Initialize(ID_BTN_SORT, NULL, KEY_RESERVED, fontsize16, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnSort.SetRightIcon(ImageManager::GetImage(IMAGE_ARROW_DOWN));
    m_btnSort.SetLeftMargin(0);
    m_btnSort.SetTopMargin(0);
#endif
    AppendChild(&m_btnSort);

    m_prevSortId = m_curSortId = ID_BTN_BOOKSTORE_SORTCOMMENT_LATEST;
#ifndef KINDLE_FOR_TOUCH
    m_mnuSort.SetMenuType(ID_MNU_SORT);
    m_mnuSort.AppendMenu(ID_BTN_BOOKSTORE_SORTCOMMENT_LATEST, 0, StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_SORT_LATEST), IMAGE_BUTTON_HEAD_24, KEY_RESERVED, TRUE);
    m_mnuSort.AppendMenu(ID_BTN_BOOKSTORE_SORTCOMMENT_USEFULL, 0, StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_SORT_USEFULL), IMAGE_BUTTON_HEAD_24, KEY_RESERVED);
    AppendChild(&m_mnuSort);
#endif

    m_totalStarLabel.SetFontSize(fontsize16);
    AppendChild(&m_totalStarLabel);

    m_totalStar.SetEnable(FALSE);

    m_userCommentLabel.SetFontSize(fontsize24);
    m_userCommentLabel.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT));
    AppendChild(&m_userCommentLabel);

    AppendChild(&m_noCommentWnd);
    AppendChild(&m_listBoxPanel);

    if (!m_windowSizer)
    {
        const int elemspacing = GetWindowMetrics(UIPixelValue10Index);
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* topSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* middleSizer = new UIBoxSizer(dkHORIZONTAL);
        m_commentSizer = new UIBoxSizer(dkVERTICAL);
        if (mainSizer && topSizer && middleSizer && m_commentSizer)
        {
            topSizer->Add(&m_myScoreLable, UISizerFlags().Align(dkALIGN_CENTER).Border(dkRIGHT, elemspacing));
            topSizer->Add(&m_myScore, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, elemspacing));
            topSizer->Add(&m_myScoreInfoLable, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            topSizer->AddStretchSpacer();
            topSizer->Add(&m_addComment, UISizerFlags().Align(dkALIGN_RIGHT));

            middleSizer->Add(&m_userCommentLabel, UISizerFlags().Border(dkRIGHT, 3*elemspacing));
            middleSizer->Add(&m_totalStar, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, elemspacing >> 1));
            middleSizer->Add(&m_totalStarLabel, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            middleSizer->AddStretchSpacer();
            middleSizer->Add(&m_btnSort, UISizerFlags().Align(dkALIGN_RIGHT));

            m_commentSizer->Add(topSizer, UISizerFlags().Expand().Border(dkTOP, elemspacing << 1).Border(dkBOTTOM, elemspacing << 1));
            m_commentSizer->Add(middleSizer, UISizerFlags().Expand().Border(dkTOP, elemspacing << 1));
            m_commentSizer->Add(&m_listBoxPanel, UISizerFlags(1).Expand());

            mainSizer->Add(m_commentSizer, UISizerFlags(1).Expand());
            mainSizer->Add(&m_noCommentWnd, UISizerFlags(1).Expand());
            SetSizer(mainSizer);
        }
        else
        {
            SafeDeletePointer(mainSizer);
            SafeDeletePointer(topSizer);
            SafeDeletePointer(middleSizer);
            SafeDeletePointer(m_commentSizer);
        }
    }
}

void UIBookStoreBookComment::RequestBookScoreByCurrentUser()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CAccountManager* pAccount = CAccountManager::GetInstance();    
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (pAccount && pAccount->IsLoggedIn() && manager)
    {
        m_fetchMyScoreReturn = FETCHING;
        manager->FetchBookScoreByCurrentUser(m_bookId.c_str());
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

FetchDataResult UIBookStoreBookComment::FetchInfo()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (FETCH_SUCCESS != m_fetchMyScoreReturn)
    {
        RequestBookScoreByCurrentUser();
    }
    if (FETCH_SUCCESS != m_fetchTotalScoreReturn)
    {
        RequestBookScores();
    }
    RequestBookComment();
    return FDR_SUCCESSED;
}

void UIBookStoreBookComment::RequestBookComment()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (manager)
    {
        m_fetchListBoxReturn = FETCHING;
        const int perPageNum = m_listBoxPanel.GetNumberPerPage();
        const int startIndex = perPageNum * m_listBoxPanel.GetCurrentPage();
        const int order = (m_curSortId == ID_BTN_BOOKSTORE_SORTCOMMENT_LATEST) ? 0 : 1;
        manager->FetchCommentList(m_bookId.c_str(), startIndex, perPageNum, ((FetchDataOrder)order));
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UIBookStoreBookComment::RequestBookScores()
{
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (manager)
    {
        m_fetchTotalScoreReturn = FETCHING;
        manager->FetchBookScore(m_bookId.c_str());
    }
}

void UIBookStoreBookComment::CheckFetchReturn()
{
    if (m_commentSizer)
    {
        if ((FETCH_FAIL == m_fetchListBoxReturn)
            || (FETCH_FAIL == m_fetchMyScoreReturn) 
            || (FETCH_FAIL == m_fetchTotalScoreReturn))
        {
            TablePanelArgs panelArgs;
            panelArgs.window = this;
            panelArgs.succeeded = false;
            panelArgs.total = 1;
            FireEvent(EventTablePanelChange, panelArgs);
        }
        else if ((FETCHING != m_fetchListBoxReturn)
            && (FETCHING != m_fetchMyScoreReturn) 
            && (FETCHING != m_fetchTotalScoreReturn))
        {
            TablePanelArgs panelArgs;
            panelArgs.window = this;
            panelArgs.succeeded = true;
            panelArgs.total = 1;
            FireEvent(EventTablePanelChange, panelArgs);

            UpdateMyScore();
            UpdateTotalScore();
            if (m_fetchListBoxReturn == FETCH_NOELEM)
            {
                m_noCommentWnd.SetVisible(true);
#ifndef KINDLE_FOR_TOUCH
                m_noCommentWnd.SetFocus(true);
#endif
                m_commentSizer->Show(false);
            }
            else
            {
                m_noCommentWnd.SetVisible(false);
                m_commentSizer->Show(true);
                m_myScore.SetVisible(m_isPurchased);
            }
            Layout();
            Repaint();
        }
    }
}

bool UIBookStoreBookComment::OnMessageListBoxUpdate(const EventArgs& args)
{
    bool result = m_listBoxPanel.OnMessageListBoxUpdate(args);
    m_fetchListBoxReturn = result ? ((m_listBoxPanel.GetItemCount() > 0) ? FETCH_SUCCESS : FETCH_NOELEM): FETCH_FAIL;
    m_btnSort.SetText(GetSortString().c_str());
    CheckFetchReturn();
    return true;
}

bool UIBookStoreBookComment::OnMessageBookScoreUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const FetchBookScoreUpdateArgs& msgArgs = (const FetchBookScoreUpdateArgs&)args;
    if (m_bookId == msgArgs.bookId)
    {
        m_fetchTotalScoreReturn = FETCH_FAIL;
        if (msgArgs.succeeded)
        {
            m_bookScoreInfo = msgArgs.bookScore;
            if (m_bookScoreInfo)
            {
#ifndef KINDLE_FOR_TOUCH
                m_addComment.SetFocus(true);
#endif
                m_totalStar.SetScore(m_bookScoreInfo->GetScore());
                const int validScoreMinNum = 5;
                const int scoreCount = m_bookScoreInfo->GetTotalStarCount();
                bool starShow = (scoreCount > validScoreMinNum);
                if (starShow)
                {
                    m_fetchTotalScoreReturn = FETCH_SUCCESS;
                    char strScore[32] = {0};
                    sprintf(strScore, "(%d%s%s)", scoreCount,StringManager::GetPCSTRById(TEXT_COUNT),StringManager::GetPCSTRById(BOOKSTORE_BOOKSCORE));
                    m_totalStarLabel.SetText(strScore);
                }
                else
                {
                    m_fetchTotalScoreReturn = FETCH_NOELEM;
                    m_totalStarLabel.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKSCORE_NOSCORE));
                }
            }
            else
            {
                m_fetchTotalScoreReturn = FETCH_SUCCESS;
                m_totalStarLabel.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKSCORE_NOSCORE));
            }
        }
        CheckFetchReturn();
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

bool UIBookStoreBookComment::OnMessageBookScoreByCurrentUserUpdate(const EventArgs& args)
{
    const BookScoreByCurrentUserArgs& msgArgs = (const BookScoreByCurrentUserArgs&)args;
    const int& myScore = msgArgs.myScore;
    if (m_bookId == msgArgs.bookId)
    {
        m_fetchMyScoreReturn = FETCH_FAIL;
        if(msgArgs.succeeded)
        {
            m_fetchMyScoreReturn = (myScore > 0) ? FETCH_SUCCESS : FETCH_NOELEM;
            m_myScore.SetScore((double)myScore);
        }
        CheckFetchReturn();
    }
    return true;
}

bool UIBookStoreBookComment::OnMessageAddBookScoreUpdate(const EventArgs& args)
{
    const AddBookScoreUpdateArgs& msgArgs = (const AddBookScoreUpdateArgs&)args;
    if(msgArgs.succeeded && msgArgs.bookId == m_bookId)
    {
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(msgArgs.succeeded ? BOOKSTORE_BOOKSCORE_SUCCESS : BOOKSTORE_BOOKSCORE_ADDSCOREFAIL));
        m_myScore.SetScore((double)msgArgs.score);
        m_fetchMyScoreReturn = (msgArgs.score > 0) ? FETCH_SUCCESS : FETCH_NOELEM;
        CheckFetchReturn();
    }
    return true;
}

bool UIBookStoreBookComment::CheckAndReLogin()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CAccountManager* pAccountManager = CAccountManager::GetInstance();
    if (NULL == pAccountManager)
    {
        return false;
    }

    if (pAccountManager->IsLoggedIn())
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s IsLoggedIn()", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return true;
    }

    if (UIUtility::CheckAndReLoginSync())
    {
        RequestBookScoreByCurrentUser();
        int tryTimes = 10;
        bool purchasedSuccess = false;
        while (!purchasedSuccess && tryTimes > 0)
        {
            UIUtility::SetScreenTips(StringManager::GetPCSTRById(ACCOUNT_LOGINING));
            purchasedSuccess = (dk::bookstore::BookStoreInfoManager::GetInstance()->GetPurchasedBookSize() > 0);
            tryTimes--;
            sleep(1);
        }
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(ACCOUNT_LOGIN_SUCCESS));
        return true;
    }

    return false;
}

void UIBookStoreBookComment::AddBookScore()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (CheckAndReLogin())
    {
        if (IsPurchasedBook())
        {
            BookStoreInfoManager* pManager = BookStoreInfoManager::GetInstance();
            if (pManager)
            {
                DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s IDOK", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
                int score = (int)m_myScore.GetBlackStars();
                pManager->AddBookScore(m_bookId.c_str(), score);
                return;
            }
        }
        else
        {
            UIUtility::SetScreenTips(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_ADDCOMMENT_UNPURCHASED));
            m_myScore.SetScore(0.0);
        }
    }
    CheckFetchReturn();
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UIBookStoreBookComment::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s PAGEDOWN", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
    case ID_BIN_BOOKSTORE_ADDCOMMENT:
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s ID_BIN_BOOKSTORE_ADDCOMMENT", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            CDisplay* pDisplay = CDisplay::GetDisplay();
            if (pDisplay && CheckAndReLogin())
            {
                if (IsPurchasedBook())
                {
                    UIBookStoreAddCommentPage* pAddCommentPage = new UIBookStoreAddCommentPage(m_bookId, m_bookName);
                    if (pAddCommentPage)
                    {
                        pAddCommentPage->MoveWindow(0, 0, pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
                        CPageNavigator::Goto(pAddCommentPage);
                        return;
                    }
                }
                else
                {
                    UIUtility::SetScreenTips(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_ADDCOMMENT_UNPURCHASED));
                }
            }
            CheckFetchReturn();
        }
        break;
    case ID_BOOKSTORE_ADDSCORE:
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s ID_BOOKSTORE_ADDSCORE", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        AddBookScore();
        break;
    case ID_BTN_SORT:
#ifdef KINDLE_FOR_TOUCH
        {
            UITouchMenu menu(this);
            const DWORD cmdID[] = 
            {
                ID_BTN_BOOKSTORE_SORTCOMMENT_LATEST,
                ID_BTN_BOOKSTORE_SORTCOMMENT_USEFULL
            };
            const SOURCESTRINGID stringID[] = 
            {
                BOOKSTORE_BOOKCOMMENT_SORT_LATEST,
                BOOKSTORE_BOOKCOMMENT_SORT_USEFULL
            };
            const int menuFontsize = GetWindowFontSize(UIBookStoreSortBtnIndex);
            int selectIndex = 0;
            for (size_t i = 0; i < DK_DIM(cmdID); ++i)
            {
                menu.AppendButton(cmdID[i], StringManager::GetPCSTRById(stringID[i]), menuFontsize);
                if (cmdID[i] == m_prevSortId)
                {
                    selectIndex = i;
                }
            }
            menu.SetCheckedButton(selectIndex);

            dkSize menuSize = menu.GetMinSize();
            menu.MoveWindow(m_btnSort.GetX() + m_btnSort.GetWidth() - menuSize.x,
                m_btnSort.GetAbsoluteY() + m_btnSort.GetHeight() + GetWindowMetrics(UIPixelValue3Index),
                menuSize.x,
                menuSize.y);
            menu.Layout();
            if (IDOK == menu.DoModal())
            {
                m_curSortId = menu.GetCommandId();
                ResetSortType(m_curSortId);
            }
        }
#else
        PopupMenu(&m_mnuSort, 
            m_btnSort.GetX() - GetWindowMetrics(UIElementLargeSpacingIndex), 
            (m_btnSort.GetY() + m_btnSort.GetHeight() + GetWindowMetrics(UIElementMidSpacingIndex)));
#endif
        break;
#ifndef KINDLE_FOR_TOUCH
    case ID_BTN_BOOKSTORE_SORTCOMMENT_LATEST:
    case ID_BTN_BOOKSTORE_SORTCOMMENT_USEFULL:
        {
            m_curSortId = m_mnuSort.GetSelectedMenuId();
            ResetSortType(m_curSortId);
        }
        break;
#endif
    case ID_LISTBOX_TURNPAGE:
        if (m_pParent)
        {
            m_pParent->OnCommand(ID_LISTBOX_TURNPAGE, pSender, dwParam);
        }
    default:
        break;
    }
}

void UIBookStoreBookComment::ResetSortType(DWORD newSortType)
{
    if (m_prevSortId != newSortType)
    {
        m_prevSortId = newSortType;
        m_listBoxPanel.ResetListBox();
        if (m_pParent)
        {
            m_pParent->OnCommand(ID_LISTBOX_TURNPAGE, this, 0);
        }
    }
}

std::string UIBookStoreBookComment::GetSortString() 
{
    string strSortText;
    switch(m_curSortId)
    {
    case ID_BTN_BOOKSTORE_SORTCOMMENT_LATEST:
        strSortText = "(";
        strSortText += StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_SORT_LATEST);
        strSortText += ")";
        break;
    case ID_BTN_BOOKSTORE_SORTCOMMENT_USEFULL:
        strSortText = "(";
        strSortText += StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_SORT_USEFULL);
        strSortText += ")";
        break;
    default:
        return strSortText;
    }
    strSortText = string(StringManager::GetPCSTRById(SORT)) + strSortText;
    return strSortText;
}

void UIBookStoreBookComment::UpdateMyScore()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CAccountManager* pAccount = CAccountManager::GetInstance();
    if (pAccount && pAccount->IsLoggedIn())
    {
        if(IsPurchasedBook())
        {
            const int myScore = m_myScore.GetBlackStars();
            m_myScoreInfoLable.SetText(StringManager::GetPCSTRById((myScore == 5) ? BOOKSTORE_BOOKSCORE_ADDSCORE_FULLMARK : BOOKSTORE_BOOKSCORE_ADDSCORE_NOTFULLMARK));
        }
        else
        {
            m_myScoreInfoLable.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKSCORE_CANNOTADDSCORE));
        }
    }
    else
    {
        m_myScoreInfoLable.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKSCORE_NOLOGGIN));
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UIBookStoreBookComment::UpdateTotalScore()
{
    CAccountManager* pAccountManager = CAccountManager::GetInstance();
    if (NULL == pAccountManager)
    {
        return;
    }
    if (!pAccountManager->IsLoggedIn())
    {
        char szTmp[256] = {0};
        snprintf(szTmp, 256, "%s%s", StringManager::GetPCSTRById(AFTER_LOGIN), StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_ADDCOMMENT));
        m_addComment.SetText(szTmp);
    }
    else
    {
        m_addComment.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_ADDCOMMENT));
    }
    m_noCommentWnd.UpdateAddCommentStatus();
}

HRESULT UIBookStoreBookComment::DrawBackGround(DK_IMAGE drawingImg)
{
    CTpGraphics grf(drawingImg);
    grf.EraserBackGround();
    if (!m_noCommentWnd.IsVisible())
    {
        const int lineHeight = GetWindowMetrics(UIDialogBorderLineIndex);
        const int lineTopMargin = m_myScoreLable.GetY() + m_myScoreLable.GetHeight() + 
            (GetWindowMetrics(UIPixelValue10Index) << 1) - lineHeight;
        grf.FillRect(0, lineTopMargin, m_iWidth, lineTopMargin + lineHeight, ColorManager::GetColor(COLOR_DISABLECOLOR));
    }
    return S_OK;
}

bool UIBookStoreBookComment::IsPurchasedBook()
{
    if (!m_isPurchased)
    {
        BookStoreInfoManager* pBookStoreInfoManager = BookStoreInfoManager::GetInstance();
        if (pBookStoreInfoManager)
        {
            model::BookInfoSPtr purchasedBookInfo = pBookStoreInfoManager->GetPurchasedBookInfo(m_bookId.c_str());
            m_isPurchased = (NULL != purchasedBookInfo);
        }
    }
    return m_isPurchased;
}

BOOL UIBookStoreBookComment::OnKeyPress(INT32 iKeyCode)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
#ifndef KINDLE_FOR_TOUCH
    if (!SendKeyToChildren(iKeyCode))
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s if (!SendKeyToChildren(iKeyCode))", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return FALSE;
    }
    switch (iKeyCode)
    {
    case KEY_BACK:
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s BACK", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        if (m_mnuSort.IsVisible())
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s if (m_mnuMain.IsVisible())", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            m_mnuSort.Show(FALSE);
            return FALSE;
        }
        return TRUE;
    case KEY_LPAGEDOWN:
    case KEY_RPAGEDOWN:
    case KEY_SPACE:
    case KEY_LPAGEUP:
    case KEY_RPAGEUP:
        return m_listBoxPanel.OnKeyPress(iKeyCode);
    case KEY_UP:
        if (!IsFocus())
        {
            if (m_listBoxPanel.IsVisible())
            {
                return m_listBoxPanel.OnKeyPress(iKeyCode);
            }
            else if (m_noCommentWnd.IsVisible())
            {
                SetFocus(TRUE);
                m_noCommentWnd.SetFocus(TRUE);
            }
            else
            {
                m_btnSort.SetFocus(TRUE);
            }
        }
        else if (m_addComment.IsFocus() || m_myScore.IsFocus() || m_noCommentWnd.IsFocus())
        {
            m_noCommentWnd.SetFocus(FALSE);
            SetFocus(FALSE);
            return TRUE;
        }
        else
        {
            MoveFocus(DIR_TOP);
        }
        break;
    case KEY_DOWN:
        if (!IsFocus())
        {
            if (m_noCommentWnd.IsVisible())
            {
                SetFocus(TRUE);
                m_noCommentWnd.SetFocus(TRUE);
            }
            else
            {
                m_addComment.SetFocus(TRUE);
            }
        }
        else if (m_addComment.IsFocus() || m_myScore.IsFocus())
        {
            MoveFocus(DIR_DOWN);
        }
        else if (m_btnSort.IsFocus())
        {
            return m_listBoxPanel.OnKeyPress(iKeyCode);
        }
        else
        {
            m_noCommentWnd.SetFocus(FALSE);
            SetFocus(FALSE);
            return TRUE;
        }
        break;
    default:
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s default", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        if(SendHotKeyToChildren(iKeyCode))
        {
            return UIContainer::OnKeyPress(iKeyCode);
        }
        break;
    }
#endif
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return FALSE;
}

bool UIBookStoreBookComment::LogDataChanged(const EventArgs& args)
{
    const AccountEventArgs& accountEventArgs = (const AccountEventArgs&)args;
    if (accountEventArgs.IsLoggedIn())
    {
        CheckFetchReturn();
    }
    return true;
}

bool UIBookStoreBookComment::OnMessageVoteBookComment(const EventArgs& args)
{
    const VoteBookCommentUpdateArgs& voteArgs = dynamic_cast<const VoteBookCommentUpdateArgs&>(args);
    if ((ID_BTN_BOOKSTORE_SORTCOMMENT_USEFULL == m_prevSortId) 
        && (voteArgs.bookId == m_bookId) 
        && voteArgs.succeeded 
        && m_pParent)
    {
        m_pParent->OnCommand(ID_LISTBOX_TURNPAGE, this, 0);
    }
    return true;
}
