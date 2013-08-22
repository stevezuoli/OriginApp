#include "BookStoreUI/UIBookStorePayInfoDlg.h"
#include "BookStore/BookStoreInfoManager.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIMessageBox.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;
UIBookStorePayInfoDlg::UIBookStorePayInfoDlg(UIContainer* parent,
                                             const LPCSTR szTitle,
                                             PayMode mode,
                                             const char* mailBox,
                                             const char* orderId)
    : UIModalDlg(parent, szTitle)
    , m_payMode(mode)
{
    if (NULL != mailBox)
    {
        m_mailBox = mailBox;
    }
    if (NULL != orderId)
    {
        m_orderId = orderId;
    }
    InitUI();
    m_sendingStatus = SS_NONE;
    SubscribeMessageEvent(
            BookStoreInfoManager::EventMailOrderUpdate,
            *BookStoreInfoManager::GetInstance(),
            std::tr1::bind(
                std::tr1::mem_fn(&UIBookStorePayInfoDlg::OnMailOrderUpdate),
                this,
                std::tr1::placeholders::_1));
}

UIBookStorePayInfoDlg::~UIBookStorePayInfoDlg()
{
}

void UIBookStorePayInfoDlg::InitUI()
{
    const int dlgWidth = GetWindowMetrics(UIBookStorePayInfoDlgWidthIndex);
	//const int dlgHeight = GetWindowMetrics(UIBookStorePayInfoDlgHeightIndex);
	const int controlSpacing = GetWindowMetrics(UIBookStorePayInfoDlgControlSpacingIndex);
	const int horizontalMargin = GetWindowMetrics(UIBookStorePayInfoDlgHorizontalMarginIndex);
	//const int btnWith = (dlgWidth - 2*horizontalMargin - controlSpacing)/2;
	const int btnHeight = GetWindowMetrics(UIBookStorePayInfoDlgBtnHeightIndex);
	const int textHeight = GetWindowMetrics(UIBookStorePayInfoTextHeightIndex);
	const int textWidth = dlgWidth - 2*horizontalMargin;
    const int footNoteMargin = GetWindowMetrics(UIBookStorePayInfoDlgfootNoteMarginIndex);

	const int btnFontSize = GetWindowFontSize(FontSize20Index);
	const int hintFontSize = GetWindowFontSize(FontSize18Index);
	const int foolNoteFontSize = GetWindowFontSize(FontSize14Index);
	
	m_footnoteLine.SetText(StringManager::GetPCSTRById(MAIL_ORDER_FOOT_NOTE));
	m_footnoteLine.SetFontSize(foolNoteFontSize);
#ifdef KINDLE_FOR_TOUCH
	m_btnLeft.Initialize(ID_PAYINFO_LEFT, StringManager::GetPCSTRById(BOOKSTORE_PAY_FINISHED), btnFontSize);
	m_btnRight.Initialize(ID_PAYINFO_RIGHT, StringManager::GetPCSTRById(BOOKSTORE_I_KNOW), btnFontSize);
    m_btnLeft.SetFlashOnClick(true);
	m_btnRight.SetFlashOnClick(true);
    ShowCloseIcon(true);
#else
    m_btnLeft.Initialize(
            ID_PAYINFO_LEFT, 
            StringManager::GetPCSTRById(BOOKSTORE_PAY_FINISHED),
            KEY_RESERVED,
            btnFontSize,
            ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnRight.Initialize(
            ID_PAYINFO_RIGHT, 
            StringManager::GetPCSTRById(BOOKSTORE_I_KNOW),
            KEY_RESERVED,
            btnFontSize,
            ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
	m_btnLeft.SetText(StringManager::GetPCSTRById(BOOKSTORE_PAY_FINISHED));
    m_btnLeft.SetMinHeight(btnHeight);
	//m_btnLeft.SetMinSize(dkSize(btnWith, btnHeight));
	m_btnLeft.SetAlign(ALIGN_CENTER);

	m_btnRight.SetText(StringManager::GetPCSTRById(BOOKSTORE_I_KNOW));
	m_btnRight.SetMinHeight(btnHeight);
	//m_btnRight.SetMinSize(dkSize(btnWith, btnHeight));
    m_btnRight.SetAlign(ALIGN_CENTER);

	m_hintText.SetFontSize(hintFontSize);
    m_hintText.SetAlign(ALIGN_LEFT);
    m_hintText.SetMinSize(dkSize(textWidth,textHeight));

	AppendChild(&m_footnoteLine);
    AppendChild(&m_hintText);
    m_btnGroup.AddButton(&m_btnLeft);
    m_btnGroup.AddButton(&m_btnRight);
#ifndef KINDLE_FOR_TOUCH
    m_btnGroup.SetFocusedButton(&m_btnRight);
#endif

    switch(m_payMode)
    {
        case PM_EMAIL:
            {
    	        char buf[1024];
                snprintf(buf, DK_DIM(buf),StringManager::GetPCSTRById(BOOKSTORE_HINT_ORDER_MAIL_SENT),m_mailBox.c_str());
                m_hintText.SetText(buf);
		        m_btnLeft.SetVisible(false);
		        m_btnRight.SetText(StringManager::GetPCSTRById(SEND_LINK));
            }
            break;
        case PM_OTHER_DEVICE:
	        {
		        m_footnoteLine.SetVisible(false);
		        m_hintText.SetText(StringManager::GetPCSTRById(BOOKSTORE_HINT_BUY_FROM_OTHER_DEVICE));
	        }
            break;
        case PM_WEBSITE:
            {
                m_footnoteLine.SetVisible(false);
                m_hintText.SetText(StringManager::GetPCSTRById(BOOKSTORE_HINT_BUY_FROM_WEBSITE));
            }
            break;
        default:
            break;
    }
    
	if (m_windowSizer)
	{
		m_windowSizer->Add(&m_hintText, UISizerFlags(1).Expand().Align(dkALIGN_CENTER).Border(dkLEFT | dkRIGHT, horizontalMargin));
		m_windowSizer->AddSpacer(controlSpacing);
        m_windowSizer->Add(&m_btnGroup, UISizerFlags().Expand());
		m_windowSizer->Add(&m_footnoteLine, 
                UISizerFlags().Expand().Align(dkALIGN_BOTTOM).Border(dkLEFT | dkRIGHT, horizontalMargin).Border(dkBOTTOM, footNoteMargin));

        const int width = GetWindowMetrics(UIModalDialogWidthIndex);
        const int height = m_windowSizer->GetMinSize().GetHeight();
        MakeCenter(width, height);
        Layout();
	}
}
#ifndef KINDLE_FOR_TOUCH
bool UIBookStorePayInfoDlg::OnMailKeyPress(int keyCode)
{
    bool bSentClick = false;
	if (IsAlpha(keyCode))
	{
		char cKey = VirtualKeyToChar(keyCode);
        switch (cKey)
        {
            case 'a':
            case 'A':
                bSentClick = true;
                break;
            default:
                break;

        }
    }
    else if (keyCode == KEY_OKAY)
    {
        bSentClick = true;
    }
    if (!bSentClick || m_sendingStatus == SS_SENDING)
    {
        return true;
    }
    if (m_sendingStatus == SS_NONE)
    {
        BookStoreInfoManager* bookStoreInfoManager = BookStoreInfoManager::GetInstance();
        FetchDataResult fdr = bookStoreInfoManager->MailOrder(m_orderId.c_str());
        if (fdr == FDR_FAILED)
        {
            UIMessageBox msgBox(
					this->GetParent(),
                    StringManager::GetStringById(BOOKSTORE_MAILORDER_FAILED),
                    MB_OK);
            msgBox.DoModal();
        }
        else
        {
            m_sendingStatus = SS_SENDING;
            m_btnRight.SetEnable(false);
            m_btnRight.SetText(StringManager::GetStringById(SENDING_LINK));
            m_btnRight.UpdateWindow();
            UpdateWindow();
        }
    }
    else if (SS_SENDED == m_sendingStatus)
    {
        EndDialog(IDOK);
        return false;
    }
    if (SS_SENDING == m_sendingStatus)
    {
        return false;
    }
    return UIDialog::OnKeyPress(keyCode);
}

bool UIBookStorePayInfoDlg::OnOtherDeviceKeyPress(int keyCode)
{
	if (IsAlpha(keyCode))
	{
		char cKey = VirtualKeyToChar(keyCode);
        switch (cKey)
        {
            case 'a':
            case 'A':
            case 'b':
            case 'B':
                EndDialog(IDOK);
                return FALSE;
                break;
            default:
                break;

        }
    }
    else if (keyCode == KEY_OKAY)
    {
        EndDialog(IDOK);
        return false;
    }
    return UIDialog::OnKeyPress(keyCode);
}
#endif

BOOL UIBookStorePayInfoDlg::OnKeyPress(INT32 iKeyCode)
{
#ifdef KINDLE_FOR_TOUCH
    return FALSE;
#else
    switch (m_payMode)
    {
    case PM_EMAIL:
        return OnMailKeyPress(iKeyCode);
    default:
        return OnOtherDeviceKeyPress(iKeyCode);
    }
#endif
}

void UIBookStorePayInfoDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_UISYSTEMSETTING, "%s, %d, %s, %s: %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, dwCmdId);
    switch (dwCmdId)
    {
    case ID_PAYINFO_LEFT:
        EndDialog(IDOK);
        break;
    case ID_PAYINFO_RIGHT:
        if (m_payMode == PM_EMAIL)
        {
            if (m_sendingStatus == SS_NONE)
            {
                BookStoreInfoManager* bookStoreInfoManager = BookStoreInfoManager::GetInstance();
                FetchDataResult fdr = bookStoreInfoManager->MailOrder(m_orderId.c_str());
                if (fdr == FDR_FAILED)
                {
                    UIMessageBox msgBox(
                            this->GetParent(),
                            StringManager::GetStringById(BOOKSTORE_MAILORDER_FAILED),
                            MB_OK);
                    msgBox.DoModal();

                }
                else
                {
                    m_sendingStatus = SS_SENDING;
                    m_btnRight.SetEnable(false);
                    m_btnRight.SetText(StringManager::GetStringById(SENDING_LINK));
                    m_btnRight.UpdateWindow();
                    UpdateWindow();
                }
            }
            else if (SS_SENDED == m_sendingStatus)
            {
                EndDialog(IDOK);
            }

        }
        else
        {
            EndDialog(IDCANCEL);
        }
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

bool UIBookStorePayInfoDlg::OnCloseClick(const EventArgs&)
{
    EndDialog(IDCANCEL);
    return true;
}

bool UIBookStorePayInfoDlg::OnMailOrderUpdate(const EventArgs& eventArgs)
{
    if (m_sendingStatus != SS_SENDING)
    {
        return false;
    }
    const MailOrderUpdateArgs& mailOrderUpdateArgs 
        = (const MailOrderUpdateArgs&)eventArgs;
    if (mailOrderUpdateArgs.orderId != m_orderId)
    {
        return false;
    }
    if (mailOrderUpdateArgs.succeeded)
    {
        m_sendingStatus = SS_SENDED;
        BookStoreInfoManager::GetInstance()->FetchPurchasedList();
        m_btnRight.SetText(StringManager::GetPCSTRById(SENDED_LINK));
        m_btnRight.SetEnable(true);
        return true;
    }
    else
    {
        UIMessageBox msgBox(
                this->GetParent(),
                StringManager::GetStringById(BOOKSTORE_MAILORDER_FAILED),
                MB_OK);
        msgBox.DoModal();
        m_sendingStatus = SS_NONE;
        m_btnRight.SetEnable(true);
        m_btnRight.SetText(StringManager::GetStringById(SEND_LINK));
    }

    return false;
}
