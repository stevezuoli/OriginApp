#include <tr1/functional>
#include "BookStoreUI/UIBookStoreBookCommentReplyPage.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "BookStore/BookStoreInfoManager.h"
#include "I18n/StringManager.h"
#include "drivers/DeviceInfo.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/UISoftKeyboardIME.h"
#include "CommonUI/UIIMEManager.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIBookStoreBookCommentReplyPage::UIBookStoreBookCommentReplyPage(std::string bookName, std::string bookId, std::string commentId)
    : UIBookStoreNavigation(bookName, false)
    , m_bookId(bookId)
    , m_commentId(commentId)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventAddBookCommentReplyUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreBookCommentReplyPage::OnMessageAddBookCommentReplyUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    Init();
}

UIBookStoreBookCommentReplyPage::~UIBookStoreBookCommentReplyPage()
{

}

bool UIBookStoreBookCommentReplyPage::OnMessageAddBookCommentReplyUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    AddBookCommentReplyUpdateArgs updateArgs = dynamic_cast<const AddBookCommentReplyUpdateArgs&>(args);
    if (!updateArgs.succeeded)
    {
        UpdateWindowsStatus(true);
        string errorInfo = updateArgs.errorInfo;
        if (errorInfo.empty())
        {
            errorInfo = StringManager::GetPCSTRById(FAILED_ADD_WORD);
        }
        UIUtility::ShowMessageBox(errorInfo);
        UpdateWindow();
        return false;
    }
    if (updateArgs.bookId == m_bookId && updateArgs.commentId == m_commentId)
    {
        UIUtility::ShowMessageBox(StringManager::GetPCSTRById(SUBMITSUCCESS_PENDINGREVIEW));
        CPageNavigator::BackToPrePage();
    }
    return true;
}

void UIBookStoreBookCommentReplyPage::AddCommentReply()
{
    BookStoreInfoManager *pManager = BookStoreInfoManager::GetInstance();
    if (pManager && UIUtility::CheckAndReLogin())
    {
        pManager->AddBookCommentReply(m_bookId.c_str(), m_commentId.c_str(), m_inputBox.GetTextUTF8().c_str());
        UpdateWindowsStatus(false);
        UpdateWindow();
    }
}

void UIBookStoreBookCommentReplyPage::Init()
{
    const int fontsize18 = GetWindowFontSize(FontSize18Index);
    const int fontsize20 = GetWindowFontSize(FontSize20Index);
    const int replyInfoFontSize = GetWindowFontSize(FontSize24Index);

    char szTmp[64] = {0};
    snprintf(szTmp, 64, "%s%s", StringManager::GetPCSTRById(ADD), StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_REPLY));
    m_addReplyLabel.SetFontSize(replyInfoFontSize);
    m_addReplyLabel.SetText(szTmp);
    m_addReplyLabel.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    AppendChild(&m_addReplyLabel);

    snprintf(szTmp, 64, "%s", StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_REPLY));
    m_replyLabel.SetFontSize(fontsize18);
    m_replyLabel.SetText(szTmp);
    AppendChild(&m_replyLabel);

    m_inputBox.SetFontSize(fontsize18);
    m_inputBox.SetTipUTF8(StringManager::GetPCSTRById(ACCOUNT_EMPTYPARAMETER));
    AppendChild(&m_inputBox);

    snprintf(szTmp, 64, "%s%s", StringManager::GetPCSTRById(BOOKSTORE_VIEW_ALL), StringManager::GetPCSTRById(BOOKSTORE_BOOKCOMMENT_REPLY));

#ifdef KINDLE_FOR_TOUCH
    m_cancelBtn.Initialize(IDCANCEL, StringManager::GetPCSTRById(ACTION_CANCEL), fontsize18);
    m_okBtn.Initialize(IDOK, StringManager::GetPCSTRById(ACTION_SUBMIT), fontsize18);
    m_viewAllReplyBtn.Initialize(ID_BOOKSTORE_VIEW_ALL, szTmp, fontsize18);
#else
    m_cancelBtn.Initialize(IDCANCEL, StringManager::GetPCSTRById(ACTION_CANCEL), KEY_RESERVED, fontsize18, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_cancelBtn.SetFontSize(fontsize18);
    m_cancelBtn.SetAlign(ALIGN_RIGHT);

    m_okBtn.Initialize(IDOK, StringManager::GetPCSTRById(ACTION_SUBMIT), KEY_RESERVED, fontsize18, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_okBtn.SetFontSize(fontsize18);
    m_okBtn.SetAlign(ALIGN_RIGHT);

    m_viewAllReplyBtn.Initialize(ID_BOOKSTORE_VIEW_ALL, szTmp, KEY_RESERVED, fontsize18, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_viewAllReplyBtn.SetFontSize(fontsize18);
    m_viewAllReplyBtn.SetAlign(ALIGN_RIGHT);
#endif

    AppendChild(&m_cancelBtn);
    AppendChild(&m_okBtn);
    AppendChild(&m_viewAllReplyBtn);

    m_submitting.SetFontSize(fontsize20);
    m_submitting.SetText(StringManager::GetPCSTRById(BOOKSTORE_SUBMITTING));
    m_submitting.SetVisible(false);
    AppendChild(&m_submitting);

    SetChildWindowFocus(GetChildIndex(&m_inputBox), false);
    UIIME* pIME = UIIMEManager::GetIME(IUIIME_CHINESE_LOWER, &m_inputBox);
    if (pIME)
    {
        pIME->SetShowDelay(true);
    }
}

UISizer* UIBookStoreBookCommentReplyPage::CreateMidSizer()
{
    if (NULL == m_pMidSizer)
    {
        m_pMidSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pCommentSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* pBtnLstSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pMidSizer && pCommentSizer && pBtnLstSizer)
        {
            const int commentTitleMargin = GetWindowMetrics(UIPixelValue30Index);
            const int elemspacing = GetWindowMetrics(UIPixelValue8Index);
            const int btnHorizontalMargin = GetWindowMetrics(UIBtnDefaultHorizontalMarginIndex);
            const int btnVerticalMargin = GetWindowMetrics(UIBtnDefaultVerticalMarginIndex);
            m_okBtn.SetLeftMargin(btnHorizontalMargin);
            m_okBtn.SetTopMargin(btnVerticalMargin);
            m_cancelBtn.SetLeftMargin(btnHorizontalMargin);
            m_cancelBtn.SetTopMargin(btnVerticalMargin);
            m_viewAllReplyBtn.SetLeftMargin(btnHorizontalMargin);
            m_viewAllReplyBtn.SetTopMargin(btnVerticalMargin);

            UISoftKeyboardIME::GetInstance()->SetInputMode(IUIIME_CHINESE_LOWER);
            int spacerHeight = UISoftKeyboardIME::GetInstance()->GetHeight();

            pCommentSizer->Add(&m_addReplyLabel, UISizerFlags(1).Align(dkALIGN_CENTRE_VERTICAL).Border(dkRIGHT, elemspacing));
            pCommentSizer->Add(&m_viewAllReplyBtn, UISizerFlags().Expand());

            pBtnLstSizer->AddStretchSpacer();
            pBtnLstSizer->Add(&m_cancelBtn, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL).Border(dkRIGHT, elemspacing));
            pBtnLstSizer->Add(&m_okBtn, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL));

            m_pMidSizer->Add(pCommentSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkTOP | dkBOTTOM, commentTitleMargin));
            m_pMidSizer->Add(&m_replyLabel, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin));
            m_pMidSizer->Add(&m_inputBox, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkTOP | dkBOTTOM, elemspacing));
            m_pMidSizer->Add(pBtnLstSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkTOP | dkBOTTOM, elemspacing));
            m_pMidSizer->AddSpacer(spacerHeight);
        }
        else
        {
            SafeDeletePointer(m_pMidSizer);
            SafeDeletePointer(pCommentSizer);
            SafeDeletePointer(pBtnLstSizer);
        }
    }
    return m_pMidSizer;
}

void UIBookStoreBookCommentReplyPage::SetFetchStatus(FETCH_STATUS status)
{
    if (m_fetchStatus != status)
    {
        UIBookStoreNavigation::SetFetchStatus(FETCH_SUCCESS);
    }
}

void UIBookStoreBookCommentReplyPage::UpdateWindowsStatus(bool submitted)
{
    m_submitting.SetVisible(!submitted);
    m_inputBox.SetVisible(submitted);
    m_cancelBtn.SetVisible(submitted);
    m_okBtn.SetVisible(submitted);
}

void UIBookStoreBookCommentReplyPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
    case IDCANCEL:
    case ID_BOOKSTORE_VIEW_ALL:
        CPageNavigator::BackToPrePage();
        break;
    case IDOK:
        {
            if (m_inputBox.GetTextUTF8().empty() )
            {
                m_inputBox.SetTipUTF8(StringManager::GetPCSTRById(ACCOUNT_EMPTYPARAMETER));
                m_inputBox.SetFocus(true);
            }
            AddCommentReply();
        }
        break;
    default:
        UIBookStoreNavigation::OnCommand(dwCmdId, pSender, dwParam);
        break;
    }
}
