#include "BookStoreUI/UIBookStoreKeyPage.h"
#include "BookStoreUI/UIBookDetailedInfoPage.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIIMEManager.h"
#include "CommonUI/UIUtility.h"
#include "GUI/CTpGraphics.h"
#include "I18n/StringManager.h"
#include "Common/CAccountManager.h"
#include "Utility/StringUtil.h"
#include "drivers/DeviceInfo.h"
#include "interface.h"
#include "BookStore/BookStoreInfoManager.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;
using namespace dk::account;
using namespace dk::bookstore;
using namespace dk::utility;

static const int KEY_LENGTH = 12;

UIBookStoreKeyPage::UIBookStoreKeyPage()
    : UIBookStoreNavigation(StringManager::GetPCSTRById(BOOKSTORE_BOOK_DUOKAN), false)
{
    SubscribeEvent(
        UIAbstractTextEdit::EventTextEditChange,
        m_duokanKey, 
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreKeyPage::UpdateSendStatus),
        this,
        std::tr1::placeholders::_1));

    SubscribeMessageEvent(BookStoreInfoManager::EventSendBookKey, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreKeyPage::OnMessageSendBookKeyUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    Init();
}

UIBookStoreKeyPage::~UIBookStoreKeyPage()
{
}

UISizer* UIBookStoreKeyPage::CreateMidSizer()
{
    if (NULL == m_pMidSizer)
    {
        m_pMidSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pKeySizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pMidSizer && pKeySizer)
        {
            const int titleInfoMargin = GetWindowMetrics(UIPixelValue20Index);
            const int elemSpacing = GetWindowMetrics(UIElementLargeSpacingIndex);
            const int minWidth = m_iWidth - (m_iLeftMargin << 1);
            m_duokanKeyTips1.SetMaxWidth(minWidth);
            const int keyHelpHeight1 = m_duokanKeyTips1.GetHeightByWidth(minWidth);
            m_duokanKeyTips1.SetMinSize(minWidth, keyHelpHeight1);

            m_duokanKeyTips2.SetMaxWidth(minWidth);
            const int keyHelpHeight2 = m_duokanKeyTips2.GetHeightByWidth(minWidth);
            m_duokanKeyTips2.SetMinSize(minWidth, keyHelpHeight2);

            pKeySizer->Add(&m_duokanKey, UISizerFlags(1).Expand());
            pKeySizer->Add(&m_btnSend, UISizerFlags().Align(dkALIGN_CENTER).Border(dkLEFT, m_iLeftMargin));

            m_pMidSizer->Add(&m_titleInfo, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkTOP | dkBOTTOM, titleInfoMargin));
            m_pMidSizer->Add(pKeySizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkBOTTOM, elemSpacing));
            m_pMidSizer->Add(&m_duokanKeyTips1, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin));
            m_pMidSizer->Add(&m_duokanKeyTips2, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkBOTTOM, elemSpacing));
            m_pMidSizer->AddStretchSpacer();
        }
        else
        {
            SafeDeletePointer(pKeySizer);
            SafeDeletePointer(m_pMidSizer);
            return NULL;
        }
    }
    return m_pMidSizer;
}

void UIBookStoreKeyPage::SetFetchStatus(FETCH_STATUS status)
{
    if (m_fetchStatus != status)
    {
        UIBookStoreNavigation::SetFetchStatus(FETCH_SUCCESS);
    }
}

void UIBookStoreKeyPage::Init()
{
    const int fontsize18 = GetWindowFontSize(FontSize18Index);
    const int fontsize20 = GetWindowFontSize(FontSize20Index);
    const int fontsize22 = GetWindowFontSize(FontSize22Index);
    const int btnHeight = GetWindowMetrics(UINormalBtnHeightIndex);

    m_titleInfo.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOK_DUOKANKEY_INPUT));
    m_titleInfo.SetFontSize(fontsize22);
    AppendChild(&m_titleInfo);

    m_duokanKey.SetFontSize(fontsize20);
    m_duokanKey.SetMinHeight(btnHeight);
    m_duokanKey.SetTipUTF8(StringManager::GetPCSTRById(BOOKSTORE_DUOKANKEY_TIPS));
    m_duokanKey.SetMaxDataLen(KEY_LENGTH);
    m_duokanKey.SetTextBoxStyle(ENGLISH_NORMAL);
    AppendChild(&m_duokanKey);

    m_btnSend.Initialize(
        ID_BTN_BOOK_KEYSEND,
        StringManager::GetPCSTRById(BOOKCORRECTION_SEND),
#ifdef KINDLE_FOR_TOUCH
        fontsize20);
    const int btnWidth = GetWindowMetrics(UINormalSmallBtnWidthIndex);
    m_btnSend.SetMinSize(dkSize(btnWidth, btnHeight));
#else
        KEY_RESERVED,
        fontsize20,
        ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnSend.SetMinSize(m_btnSend.GetMinSize());
    m_btnSend.SetVAlign(ALIGN_CENTER);
#endif
    m_btnSend.SetEnable(false);
    AppendChild(&m_btnSend);

    m_duokanKeyTips1.SetFontSize(fontsize18);
    m_duokanKeyTips1.SetText(StringManager::GetPCSTRById(BOOKSTORE_DUOKANKEY_HELP1));
    AppendChild(&m_duokanKeyTips1);

    m_duokanKeyTips2.SetFontSize(fontsize18);
    m_duokanKeyTips2.SetText(StringManager::GetPCSTRById(BOOKSTORE_DUOKANKEY_HELP2));
    m_duokanKeyTips2.SetFirstLineIndent(2);
    m_duokanKeyTips2.SetLineSpace(1);
    AppendChild(&m_duokanKeyTips2);

    SetChildWindowFocus(GetChildIndex(&m_duokanKey), FALSE);
    UIIME *pIME = UIIMEManager::GetIME(IUIIME_ENGLISH_LOWER, &m_duokanKey);
    if (pIME)
    {
        pIME->SetShowDelay(true);
    }
}

void UIBookStoreKeyPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch(dwCmdId)
    {
    case ID_BTN_BOOK_KEYSEND:
        if (UIUtility::CheckAndReLogin())
        {
            SendBookKey();
        }
        break;
    default:
        UIBookStoreNavigation::OnCommand(dwCmdId, pSender, dwParam);
        break;
    }
}

bool UIBookStoreKeyPage::UpdateSendStatus(const EventArgs& args)
{
    std::wstring wKey = EncodeUtil::ToWString(StringUtil::Trim(m_duokanKey.GetTextUTF8().c_str()));
    bool enable = (wKey.size() == (size_t)KEY_LENGTH);
    if (enable)
    {
        UIIME *pIMESymbol = UIIMEManager::GetCurrentIME();
        if (pIMESymbol)
        {
            pIMESymbol->HideIME();
        }
        m_btnSend.SetEnable(true);
#ifndef KINDLE_FOR_TOUCH
        SetChildWindowFocus(GetChildIndex(&m_btnSend), FALSE);
#endif
    }
    else
    {
        m_btnSend.SetEnable(false);
    }
#ifdef KINDLE_FOR_TOUCH
    m_btnSend.UpdateWindow();
#endif
    return true;
}

void UIBookStoreKeyPage::SendBookKey()
{
    BookStoreInfoManager*pManager = BookStoreInfoManager::GetInstance();
    string key = m_duokanKey.GetTextUTF8();
    if (pManager && !key.empty())
    {
        m_btnSend.SetEnable(false);
        m_btnSend.UpdateWindow();
        pManager->FetchBookKeyInfo(key.c_str());
    }
}

bool UIBookStoreKeyPage::OnMessageSendBookKeyUpdate(const EventArgs& args)
{
    const SendBookKeyUpdateArgs& msgArgs = dynamic_cast<const SendBookKeyUpdateArgs&>(args);
    if ((msgArgs.errorCode > 0) && (!msgArgs.msg.empty()))
    {
        UIUtility::SetScreenTips(msgArgs.msg.c_str());
        SetChildWindowFocus(GetChildIndex(&m_duokanKey), FALSE);
        UIIME *pIME = UIIMEManager::GetIME(IUIIME_ENGLISH_LOWER, &m_duokanKey);
        if (pIME)
        {
            pIME->ShowIME();
        }
    }
    else if (msgArgs.succeeded)
    {
        string duokanKey = m_duokanKey.GetTextUTF8();
        UIPage *pPage = new UIBookDetailedInfoPage(msgArgs.bookInfo->GetBookId(), msgArgs.bookInfo->GetTitle(), duokanKey.c_str());
        if (pPage)
        {
            pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
            CPageNavigator::Goto(pPage);
        }
    }
    return true;
}

