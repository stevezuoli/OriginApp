#include <tr1/functional>
#include "BookStoreUI/UIBookStoreAddCommentPage.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIMessageBox.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Common/CAccountManager.h"
#include "Utility/EncodeUtil.h"
#include "Utility/StringUtil.h"
#include "drivers/DeviceInfo.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/UISoftKeyboardIME.h"

using namespace dk::bookstore;
using namespace dk::utility;
using namespace dk::account;
using namespace WindowsMetrics;

UIBookStoreAddCommentPage::UIBookStoreAddCommentPage(const std::string& bookId,const std::string& bookName)
    : UIBookStoreNavigation(bookName, false)
    , m_starLevel(this, 0)
    , m_bookId(bookId)
{
    SetLeftMargin(GetWindowMetrics(UIBookStoreTablePageListBoxLeftMarginIndex));
    EventListener::SubscribeEvent(
        UIStars::StarEditChange,
        m_starLevel, 
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreAddCommentPage::UpdateSubmitStatus),
        this,
        std::tr1::placeholders::_1));
    EventListener::SubscribeEvent(
        UIAbstractTextEdit::EventTextEditChange,
        m_commentTitleBox, 
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreAddCommentPage::UpdateSubmitStatus),
        this,
        std::tr1::placeholders::_1));

    EventListener::SubscribeEvent(
        UIAbstractTextEdit::EventTextEditChange,
        m_commentBox, 
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreAddCommentPage::UpdateSubmitStatus),
        this,
        std::tr1::placeholders::_1));

    SubscribeMessageEvent(BookStoreInfoManager::EventBookScoreByCurrentUserUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreAddCommentPage::OnMessageBookScoreByCurrentUserUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    SubscribeMessageEvent(BookStoreInfoManager::EventAddBookCommentUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreAddCommentPage::OnMessageAddBookCommentUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreInfoManager::EventAddBookScoreUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreAddCommentPage::OnMessageAddBookScoreUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    
    Init();
}

UIBookStoreAddCommentPage::~UIBookStoreAddCommentPage()
{
}

void UIBookStoreAddCommentPage::Init()
{
    const int fontsize16 = GetWindowFontSize(FontSize16Index);
    const int fontsize20 = GetWindowFontSize(FontSize20Index);
    const int fontsize26 = GetWindowFontSize(FontSize26Index);

    m_addCommentLabel1.SetFontSize(fontsize26);
    m_addCommentLabel1.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_ADDCOMMENT));
    m_addCommentLabel1.SetForeColor(ColorManager::GetColor(COLOR_DISABLECOLOR));
    AppendChild(&m_addCommentLabel1);

    m_addCommentLabel2.SetFontSize(fontsize20);
    m_addCommentLabel2.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_REQUIRED));
    m_addCommentLabel2.SetForeColor(ColorManager::GetColor(COLOR_DISABLECOLOR));
    AppendChild(&m_addCommentLabel2);

    m_starLevelLabel.SetFontSize(fontsize20);
    m_starLevelLabel.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_STARLEVEL));
    AppendChild(&m_starLevelLabel);

    m_starLevel.SetStarsIcons(ImageManager::GetImage(IMAGE_BOOKSTORE_STARBIG_SOLID),
                              ImageManager::GetImage(IMAGE_BOOKSTORE_STARBIG_HOLLOW));
    m_starLevel.SetStarsMargin(MYSCORE_STARS_MARGIN);
    m_starLevel.SetFocus(TRUE);

    m_commentTitleLabel.SetFontSize(fontsize20);
    m_commentTitleLabel.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_TITLE));
    AppendChild(&m_commentTitleLabel);

    m_commentTitleBox.SetFontSize(fontsize20);
    AppendChild(&m_commentTitleBox);

    m_commentLabel.SetFontSize(fontsize20);
    m_commentLabel.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_CONTENT));
    AppendChild(&m_commentLabel);

    m_commentBox.SetFontSize(fontsize16);
    m_commentBox.SetTipUTF8(StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_DEFAULTCONTENT));
    AppendChild(&m_commentBox);

#ifdef KINDLE_FOR_TOUCH
    m_cancel.Initialize(IDCANCEL, StringManager::GetPCSTRById(ACTION_CANCEL), fontsize20);
    m_ok.Initialize(IDOK, StringManager::GetPCSTRById(ACTION_SUBMIT), fontsize20);
#else
    m_cancel.Initialize(IDCANCEL, StringManager::GetPCSTRById(ACTION_CANCEL), KEY_RESERVED, fontsize20, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));

    m_ok.Initialize(IDOK, StringManager::GetPCSTRById(ACTION_SUBMIT), KEY_RESERVED, fontsize20, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
    m_ok.SetEnable(false);
    AppendChild(&m_cancel);
    AppendChild(&m_ok);
}

UISizer* UIBookStoreAddCommentPage::CreateMidSizer()
{
    if (NULL == m_pMidSizer)
    {
        m_pMidSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pTitleNoteSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* pStarLevelSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* pCommentTitleSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* pBtnLstSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pMidSizer && pTitleNoteSizer && pStarLevelSizer && pCommentTitleSizer && pBtnLstSizer)
        {
            const int normalMargin = GetWindowMetrics(UIHorizonMarginIndex);
            const int specialMargin = m_iLeftMargin - GetWindowMetrics(UIBookStoreAddCommentPageLeftOffsetIndex);
            const int btnHorizontalMargin = GetWindowMetrics(UIBtnDefaultHorizontalMarginIndex);
            const int btnVerticalMargin = GetWindowMetrics(UIBtnDefaultVerticalMarginIndex);
            const int titleNoteVMargin = GetWindowMetrics(UIPixelValue15Index);
            m_ok.SetLeftMargin(btnHorizontalMargin);
            m_ok.SetTopMargin(btnVerticalMargin);
            m_cancel.SetLeftMargin(btnHorizontalMargin);
            m_cancel.SetTopMargin(btnVerticalMargin);

            const int spacerHeight = UISoftKeyboardIME::GetInstance()->GetHeight();
#ifdef KINDLE_FOR_TOUCH
            const int elemspacing = GetWindowMetrics(UIElementSpacingIndex) << 1;
#else
            const int elemspacing = GetWindowMetrics(UIElementMidSpacingIndex);
#endif

            pTitleNoteSizer->Add(&m_addCommentLabel1, UISizerFlags().Expand());
            pTitleNoteSizer->Add(&m_addCommentLabel2, UISizerFlags().Align(dkALIGN_BOTTOM));

            pStarLevelSizer->Add(&m_starLevelLabel, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL).Border(dkRIGHT, elemspacing));
            pStarLevelSizer->Add(&m_starLevel);

            pCommentTitleSizer->Add(&m_commentTitleLabel, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL).Border(dkRIGHT, elemspacing));
            pCommentTitleSizer->Add(&m_commentTitleBox, UISizerFlags(1).Expand());

            pBtnLstSizer->AddStretchSpacer();
            pBtnLstSizer->Add(&m_cancel, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL).Border(dkRIGHT, elemspacing));
            pBtnLstSizer->Add(&m_ok, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL));

            m_pMidSizer->Add(pTitleNoteSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkTOP | dkBOTTOM, titleNoteVMargin));
            m_pMidSizer->Add(pStarLevelSizer, UISizerFlags().Expand().Border(dkLEFT, specialMargin).Border(dkRIGHT, normalMargin).Border(dkTOP | dkBOTTOM, elemspacing));
            m_pMidSizer->Add(pCommentTitleSizer, UISizerFlags().Expand().Border(dkLEFT, specialMargin).Border(dkRIGHT, normalMargin).Border(dkTOP | dkBOTTOM, elemspacing));
            m_pMidSizer->Add(&m_commentLabel, UISizerFlags().Expand().Border(dkLEFT, specialMargin).Border(dkRIGHT, normalMargin).Border(dkTOP | dkBOTTOM, elemspacing));
            m_pMidSizer->Add(&m_commentBox, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkBOTTOM, elemspacing));
            m_pMidSizer->Add(pBtnLstSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkTOP | dkBOTTOM, elemspacing));
            m_pMidSizer->AddSpacer(spacerHeight);
            RequestBookScoreByCurrentUser();
        }
        else
        {
            SafeDeletePointer(m_pMidSizer);
            SafeDeletePointer(pTitleNoteSizer);
            SafeDeletePointer(pStarLevelSizer);
            SafeDeletePointer(pCommentTitleSizer);
            SafeDeletePointer(pBtnLstSizer);
        }
    }
    return m_pMidSizer;
}

void UIBookStoreAddCommentPage::SetFetchStatus(FETCH_STATUS status)
{
    if (m_fetchStatus != status)
    {
        UIBookStoreNavigation::SetFetchStatus(FETCH_SUCCESS);
    }
}

void UIBookStoreAddCommentPage::BackToPrePage()
{
    std::wstring wComment = EncodeUtil::ToWString(StringUtil::Trim(m_commentBox.GetTextUTF8().c_str()));
    if(!wComment.empty())
    {
        UIMessageBox messagebox(this, StringManager::GetStringById(BOOKSTORE_BOOKCOMMENT_EXITCOMMENTINFO), MB_OK | MB_CANCEL);
        if (MB_OK == messagebox.DoModal())
        {
            CPageNavigator::BackToPrePage();
        }
    }
    else
    {
        CPageNavigator::BackToPrePage();
    }
}

void UIBookStoreAddCommentPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
#ifdef KINDLE_FOR_TOUCH
    case ID_BTN_TOUCH_BACK:
#endif
    case IDCANCEL:
        BackToPrePage();
        break;
    case IDOK:
        CheckAddComment();
        break;
    case ID_BOOKSTORE_ADDSCORE:
        AddBookScore();
        break;
    default:
        return UIBookStoreNavigation::OnCommand(dwCmdId, pSender, dwParam);
    }
}

BOOL UIBookStoreAddCommentPage::OnKeyPress(INT32 iKeyCode)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
#ifndef KINDLE_FOR_TOUCH
    if (!SendKeyToChildren(iKeyCode))
    {
        return FALSE;
    }
#endif

    switch (iKeyCode)
    {
    case KEY_HOME:
        {
            UIMessageBox messagebox(this, StringManager::GetStringById(BOOKSTORE_BOOKCOMMENT_EXITCOMMENTINFO), MB_OK | MB_CANCEL);
            if (MB_OK == messagebox.DoModal())
            {
                CPageNavigator::BackToHome();
            }
        }
        return FALSE;
#ifndef KINDLE_FOR_TOUCH
    case KEY_BACK:
        BackToPrePage();
        return FALSE;
    default:
        break;
#endif
    }
    return UIContainer::OnKeyPress(iKeyCode);
}

bool UIBookStoreAddCommentPage::OnMessageAddBookScoreUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const AddBookScoreUpdateArgs& msgArgs = (const AddBookScoreUpdateArgs&)args;
    if(msgArgs.succeeded && msgArgs.bookId == m_bookId)
    {
        m_starLevel.SetScore((double)msgArgs.score);
        m_starLevel.UpdateWindow();
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

bool UIBookStoreAddCommentPage::OnMessageAddBookCommentUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const AddBookCommentUpdateArgs& msgArgs = (const AddBookCommentUpdateArgs&)args;

    UIUtility::ShowMessageBox(StringManager::GetPCSTRById(msgArgs.succeeded ? SUBMITSUCCESS_PENDINGREVIEW : BOOKSTORE_BOOKCOMMENT_ADDCOMMENTFAILE));

    if (msgArgs.succeeded)
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s msgArgs.succeeded", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        CPageNavigator::BackToPrePage();
    }
    else
    {

        m_ok.SetEnable(true);
        m_ok.SetFocus(TRUE);
        m_ok.UpdateWindow();
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

void UIBookStoreAddCommentPage::RequestBookScoreByCurrentUser()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CAccountManager* pAccount = CAccountManager::GetInstance();    
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (pAccount && pAccount->IsLoggedIn() && manager)
    {
        manager->FetchBookScoreByCurrentUser(m_bookId.c_str());
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

bool UIBookStoreAddCommentPage::OnMessageBookScoreByCurrentUserUpdate(const EventArgs& args)
{
    const BookScoreByCurrentUserArgs& msgArgs = (const BookScoreByCurrentUserArgs&)args;
    if(msgArgs.succeeded  && (m_bookId == msgArgs.bookId) && (msgArgs.myScore > 0))
    {
        m_starLevel.SetScore((double)msgArgs.myScore);
        if (m_starLevel.IsFocus())
        {
            m_commentTitleBox.SetFocus(TRUE);
        }
    }
    m_starLevel.UpdateWindow();
    return true;
}

void UIBookStoreAddCommentPage::CheckAddComment()
{
    CAccountManager* pAccount = CAccountManager::GetInstance();
    if (pAccount && !pAccount->IsLoggedIn() && UIUtility::CheckAndReLogin())
    {
        RequestBookScoreByCurrentUser();
    }
    m_starLevel.UpdateWindow();

    BookStoreInfoManager* pManager = BookStoreInfoManager::GetInstance();
    if (pManager && pAccount->IsLoggedIn())
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s IDOK", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        int score = (int)m_starLevel.GetBlackStars();
        std::string commentTitle = m_commentTitleBox.GetTextUTF8();
        std::string comment = m_commentBox.GetTextUTF8();
        pManager->AddBookComment(m_bookId.c_str(), score, commentTitle.c_str(), comment.c_str());
        m_ok.SetEnable(false);
        m_ok.SetFocus(false);
        m_ok.UpdateWindow();
    }
}

#ifndef KINDLE_FOR_TOUCH
HRESULT UIBookStoreAddCommentPage::MoveFocus(MoveDirection dir, BOOL bLoop)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dir)
    {
    case DIR_DOWN:
        if (m_commentBox.IsFocus() && m_ok.IsEnable())
        {
            m_commentBox.SetFocus(false);
            m_ok.SetFocus(true);
            return false;
        }
        break;
    default:
        break;
    };
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return UIContainer::MoveFocus(dir, bLoop);
}
#endif

bool UIBookStoreAddCommentPage::UpdateSubmitStatus(const EventArgs& args)
{
    if (m_bIsVisible)
    {
        bool isOK = m_starLevel.GetBlackStars() > 0;

        std::string title = StringUtil::Trim(m_commentTitleBox.GetTextUTF8().c_str());
        isOK = isOK && (!title.empty());

        int commentLen = 0;
        std::wstring wComment = EncodeUtil::ToWString(StringUtil::Trim(m_commentBox.GetTextUTF8().c_str()));
        commentLen = wComment.size();
        isOK = isOK && (commentLen > COMMENT_MINLENGTH);

        m_ok.SetEnable(isOK);
        m_ok.UpdateWindow();
    }
    return true;
}

void UIBookStoreAddCommentPage::AddBookScore()
{
    if (UIUtility::CheckAndReLogin())
    {
        BookStoreInfoManager* pManager = BookStoreInfoManager::GetInstance();
        if (pManager)
        {
            int score = (int)m_starLevel.GetBlackStars();
            pManager->AddBookScore(m_bookId.c_str(), score);
        }
    }
}

