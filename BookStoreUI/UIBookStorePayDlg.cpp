#include "BookStoreUI/UIBookStorePayDlg.h"
#include "CommonUI/UISoftKeyboardIME.h"
#include "GUI/CTpGraphics.h"
#include "I18n/StringManager.h"
#include "Common/CAccountManager.h"
#include "Framework/CDisplay.h"
#include "interface.h"
#include "BookStore/BookStoreInfoManager.h"
#include "GUI/UIMessageBox.h"
#include "CommandID.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "drivers/DeviceInfo.h"

using namespace WindowsMetrics;
using namespace dk::account;
using namespace dk::bookstore;

UIBookStorePayDlg::UIBookStorePayDlg(UIContainer* pParent, BOOL enableMail)
    : UIModalDlg(pParent, StringManager::GetStringById(BOOKSTORE_SELECT_PAYMENT_METHOD))
    , m_bEnableMail(enableMail)
    , m_bIsDisposed (FALSE)
{
    Init();
}

UIBookStorePayDlg::~UIBookStorePayDlg()
{
    Dispose ();
}

void UIBookStorePayDlg::Init()
{
    const int btnFontSize = GetWindowFontSize(FontSize22Index);
    const int btnHeight = GetWindowMetrics(UIModalDialogBtnsHeightIndex);
    
#ifdef KINDLE_FOR_TOUCH
    m_btnSendMail.Initialize(
            ID_BIN_ACCOUNT_LOGIN,
            StringManager::GetPCSTRById(BOOKSTORE_PAYMENT_METHOD_SENDMAIL),
            btnFontSize);
    m_btnBuyInOtherDev.Initialize(
            ID_BIN_ACCOUNT_REGISTER,
            StringManager::GetPCSTRById(BOOKSTORE_PAYMENT_METHOD_OTHREDEVICE),
            btnFontSize);
    m_btnBuyFromWebsite.Initialize(
            ID_BIN_ACCOUNT_WEBSITE,
            StringManager::GetPCSTRById(BOOKSTORE_PAYMENT_METHOD_WEBSITE),
            btnFontSize);
    m_btnSendMail.SetFlashOnClick(true);
    m_btnBuyInOtherDev.SetFlashOnClick(true);
    m_btnBuyFromWebsite.SetFlashOnClick(true);
    ShowCloseIcon(true);
    const int width = GetWindowMetrics(UIModalDialogWidthIndex);
    SetClientSize(dkSize(GetWindowMetrics(UIBookStorePayDlgWidthIndex), GetWindowMetrics(UIBookStorePayDlgHeightIndex)));
#else
    m_btnSendMail.Initialize(
            ID_BIN_ACCOUNT_LOGIN,
            StringManager::GetPCSTRById(BOOKSTORE_PAYMENT_METHOD_SENDMAIL),
            KEY_RESERVED,
            btnFontSize,
            ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnBuyInOtherDev.Initialize(
            ID_BIN_ACCOUNT_REGISTER,
            StringManager::GetPCSTRById(BOOKSTORE_PAYMENT_METHOD_OTHREDEVICE),
            KEY_RESERVED,
            btnFontSize,
            ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnBuyFromWebsite.Initialize(
            ID_BIN_ACCOUNT_WEBSITE,
            StringManager::GetPCSTRById(BOOKSTORE_PAYMENT_METHOD_WEBSITE),
            KEY_RESERVED,
            btnFontSize,
            ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));

    m_btnSendMail.SetVAlign(VALIGN_CENTER);
    m_btnBuyInOtherDev.SetVAlign(VALIGN_CENTER);
    m_btnBuyFromWebsite.SetVAlign(VALIGN_CENTER);

    const int width = GetWindowMetrics(UIMessageBoxWidthIndex);
#endif
    m_btnSendMail.SetAlign(ALIGN_CENTER);
    m_btnBuyInOtherDev.SetAlign(ALIGN_CENTER);
    m_btnBuyFromWebsite.SetAlign(ALIGN_CENTER);
    m_btnSendMail.SetMinHeight(btnHeight);
    m_btnBuyInOtherDev.SetMinHeight(btnHeight);
    m_btnBuyFromWebsite.SetMinHeight(btnHeight);
    m_btnSendMail.SetVisible(m_bEnableMail);

    m_btnGroup.AddButton(&m_btnSendMail);
    m_btnGroup.AddButton(&m_btnBuyInOtherDev);
    m_btnGroup.AddButton(&m_btnBuyFromWebsite);
    m_btnGroup.SetBtnsDirection(UIButtonGroup::BGD_Vertical);
    m_btnGroup.SetMinHeight(dkDefaultCoord);
#ifndef KINDLE_FOR_TOUCH
    m_btnGroup.SetFocusedButton(&m_btnBuyFromWebsite);
#endif
    if (m_windowSizer)
    {
        m_windowSizer->Add(&m_btnGroup, UISizerFlags().Expand());
        const int height = m_windowSizer->GetMinSize().GetHeight();
        MakeCenter(width, height);
        Layout();
    }
}

void UIBookStorePayDlg::Dispose ()
{
    DebugPrintf(DLC_UISYSTEMSETTING, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    OnDispose (this->m_bIsDisposed);
}

void UIBookStorePayDlg::OnDispose (BOOL bDisposed)
{
    DebugPrintf(DLC_UISYSTEMSETTING, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (bDisposed)
    {
        return;
    }

    this->m_bIsDisposed = TRUE;
}

bool UIBookStorePayDlg::SendLinkToMyMailBox()
{
    BookStoreInfoManager* bookStoreInfoManager = BookStoreInfoManager::GetInstance();
    FetchDataResult fdr = bookStoreInfoManager->MailOrder(m_orderId.c_str());
    return fdr != FDR_FAILED;
}

bool UIBookStorePayDlg::BuyInOtherDevice()
{
    return false;
}

void UIBookStorePayDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_UISYSTEMSETTING, "%s, %d, %s, %s: %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, dwCmdId);
    switch (dwCmdId)
    {
    case ID_BIN_ACCOUNT_LOGIN:
        m_action = SEND_MAIL;
        EndDialog(IDOK);
        break;
    case ID_BIN_ACCOUNT_REGISTER:
        m_action = BUY_FROM_OTHER_DEVICE;
        EndDialog(IDOK);
        break;
    case ID_BIN_ACCOUNT_WEBSITE:
        m_action = BUY_FROM_WEBSITE;
        EndDialog(IDOK);
        break;
    case IDCANCEL:
        {
            EndDialog(IDCANCEL); 
        }
        break;
    default:
        break;
    }
}

#if 0
bool UIBookStorePayDlg::OnMailOrderUpdate(const EventArgs& args)
{
    const MailOrderUpdateArgs& mailOrderUpdateArgs
        = (const MailOrderUpdateArgs&)args;
    if (mailOrderUpdateArgs.orderId != m_orderId)
    {
        return false;
    }
    SOURCESTRINGID msgId = mailOrderUpdateArgs.succeeded 
        ? BOOKSTORE_MAILORDER_SUCCEEDED 
        : BOOKSTORE_MAILORDER_FAILED;
    UIMessageBox::MessageBoxIcon icon = mailOrderUpdateArgs.succeeded 
        ? UIMessageBox::ICON_INFO
        : UIMessageBox::ICON_WARNING;

    UIMessageBox msgBox(this,
            StringManager::GetStringById(msgId),
            icon,
            MB_OK);
    msgBox.DoModal();
    return true;
}
#endif

