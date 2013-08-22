#include <tr1/functional>
#include "Framework/CDisplay.h"
#include "CommonUI/UIBookCorrectionDlg.h"
#include "BookReader/IBookReader.h"
#include "GUI/UISizer.h"
#include "GUI/CTpGraphics.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/UIIMEManager.h"
#include "CommonUI/UIUtility.h"
#include "I18n/StringManager.h"
#include "Utility/ColorManager.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/OfflineBookStoreManager.h"
#include "drivers/DeviceInfo.h"
#include "Wifi/WifiManager.h"

using namespace dk::bookstore;
using namespace dk::utility;
using namespace WindowsMetrics;

UIBookCorrectionDlg::UIBookCorrectionDlg(UIContainer* pParent, IBookReader* pBookReader)
    : UIDialog(pParent)
    , m_pBookReader(pBookReader)
    , m_pTips(NULL)
#ifdef KINDLE_FOR_TOUCH
    , m_backButton(this, ID_BTN_TOUCH_BACK)
#endif
{
    if (m_pBookReader)
    {
        SubscribeMessageEvent(BookStoreInfoManager::EventReportBookBugUpdate, 
            *BookStoreInfoManager::GetInstance(),
            std::tr1::bind(
            std::tr1::mem_fn(&UIBookCorrectionDlg::OnMessageReportBookBugUpdate),
            this,
            std::tr1::placeholders::_1)
            );
        Init();
    }
}

UIBookCorrectionDlg::~UIBookCorrectionDlg()
{

}

void UIBookCorrectionDlg::Init()
{
    const int fontsize16 = GetWindowFontSize(FontSize16Index);
    const int fontsize18 = GetWindowFontSize(FontSize18Index);
    const int fontsize20 = GetWindowFontSize(FontSize20Index);
    const int fontsize22 = GetWindowFontSize(FontSize22Index);
    const int fontsize26 = GetWindowFontSize(FontSize26Index);
    const int horizontalMargin = GetWindowMetrics(UIHorizonMarginIndex);
    const int disableColor = ColorManager::GetColor(COLOR_KERNEL_DISABLECOLOR);
    m_titleBar.SetLeftMargin(horizontalMargin);
    AppendChild(&m_titleBar);

    m_barBookName.SetFontSize(fontsize26);
    m_barBookName.SetText(m_pBookReader->GetBookInfo().title.c_str());
    m_barBookName.SetAlign(ALIGN_CENTER);
    AppendChild(&m_barBookName);

    m_barOldContent.SetFontSize(fontsize22);
    m_barOldContent.SetText(StringManager::GetPCSTRById(BOOKCORRECTION_ORIENTCONTENT));
    m_barOldContent.SetForeColor(disableColor);
    AppendChild(&m_barOldContent);

    m_txtOldContent.SetFontSize(fontsize18);
    m_txtOldContent.SetForeColor(disableColor);
    m_txtOldContent.SetMode(UIAbstractText::TextMultiLine);
    AppendChild(&m_txtOldContent);

    m_barNewContent1.SetFontSize(fontsize22);
    m_barNewContent1.SetText(StringManager::GetPCSTRById(BOOKCORRECTION_MODIFY1));
    m_barNewContent1.SetForeColor(disableColor);
    AppendChild(&m_barNewContent1);

    m_barNewContent2.SetFontSize(fontsize16);
    m_barNewContent2.SetText(StringManager::GetPCSTRById(BOOKCORRECTION_MODIFY2));
    m_barNewContent2.SetForeColor(disableColor);
    AppendChild(&m_barNewContent2);

    m_txtNewContent.SetFontSize(fontsize18);
    AppendChild(&m_txtNewContent);

    bool bWifiConnected = WifiManager::GetInstance()->IsConnected();
    if (bWifiConnected)
    {
#ifdef KINDLE_FOR_TOUCH
        m_onlineSend.Initialize(ID_BTN_BOOK_KEYSEND, StringManager::GetPCSTRById(BOOKCORRECTION_SEND), fontsize20);
        m_onlineSend.SetMinSize(dkSize(GetWindowMetrics(UINormalBtnWidthIndex), GetWindowMetrics(UINormalBtnHeightIndex)));
#else
        m_txtNewContent.SetFocus(true);
        m_onlineSend.Initialize(ID_BTN_BOOK_KEYSEND, StringManager::GetPCSTRById(BOOKCORRECTION_SEND), KEY_RESERVED, fontsize20, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
        AppendChild(&m_onlineSend);
    }
    else
    {
#ifdef KINDLE_FOR_TOUCH
        m_onlineSend.Initialize(ID_BTN_BOOK_KEYSEND, StringManager::GetPCSTRById(BOOKCORRECTION_ONLINE_SEND), fontsize20);
        m_onlineSend.SetMinSize(dkSize(GetWindowMetrics(UINormalBtnWidthIndex), GetWindowMetrics(UINormalBtnHeightIndex)));

        m_offlineSend.Initialize(ID_BTN_BOOK_OFFLINE_KEYSEND, StringManager::GetPCSTRById(BOOKCORRECTION_OFFLINE_SEND), fontsize20);
        m_offlineSend.SetMinSize(dkSize(GetWindowMetrics(UINormalBtnWidthIndex), GetWindowMetrics(UINormalBtnHeightIndex)));
#else
        m_txtNewContent.SetFocus(true);
        m_onlineSend.Initialize(ID_BTN_BOOK_KEYSEND, StringManager::GetPCSTRById(BOOKCORRECTION_SEND), KEY_RESERVED, fontsize20, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
        m_offlineSend.Initialize(ID_BTN_BOOK_OFFLINE_KEYSEND, StringManager::GetPCSTRById(BOOKCORRECTION_OFFLINE_SEND), KEY_RESERVED, fontsize20, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
        AppendChild(&m_onlineSend);
        AppendChild(&m_offlineSend);
    }

    UIIME* pIME = UIIMEManager::GetIME(IUIIME_CHINESE_LOWER, &m_txtNewContent);
    if (pIME)
    {
        SetChildWindowFocus(GetChildIndex(&m_txtNewContent), FALSE);
        pIME->SetShowDelay(true);
    }
}

void UIBookCorrectionDlg::SetSelectedText(const char* pText, const CT_RefPos* pStartPos, const CT_RefPos* pEndPos)
{
    if (pText && pStartPos && pEndPos)
    {
        m_txtOldContent.SetText(pText);
        m_startPos = *pStartPos;
        m_endPos = *pEndPos;
    }
}

void UIBookCorrectionDlg::MoveWindow(int left, int top, int width, int height)
{
    UIWindow::MoveWindow(left, top, width, height);

    if (!m_windowSizer)
    {
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pTopSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* pNewContentSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* pBottomSizer = new UIBoxSizer(dkHORIZONTAL);
        if (pMainSizer && pTopSizer && pNewContentSizer && pBottomSizer)
        {
            const int horizontalMargin = GetWindowMetrics(UIHorizonMarginIndex);
            const int elementSpacing = GetWindowMetrics(UIElementMidSpacingIndex);
            const int maxHeight = horizontalMargin * 3;
            const int maxWidth = m_iWidth - (horizontalMargin << 1);
            m_txtNewContent.SetMinHeight(maxHeight);
            int textHeight = m_txtOldContent.GetHeightByWidth(maxWidth);
            if (textHeight > maxHeight)
            {
                textHeight = maxHeight;
            }
            m_txtOldContent.SetMinSize(dkSize(maxWidth, textHeight));
#ifdef KINDLE_FOR_TOUCH
            pTopSizer->Add(&m_backButton, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL));
            pTopSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));
#endif
            pTopSizer->Add(&m_barBookName, UISizerFlags(1).Align(dkALIGN_CENTRE_VERTICAL).Border(dkLEFT | dkRIGHT, elementSpacing));
#ifdef KINDLE_FOR_TOUCH
            pTopSizer->AddSpacer(m_backButton.GetMinWidth());
#endif

            pNewContentSizer->Add(&m_barNewContent1, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL));
            pNewContentSizer->Add(&m_barNewContent2, UISizerFlags().Align(dkALIGN_BOTTOM));

            pBottomSizer->AddStretchSpacer();
            if (!WifiManager::GetInstance()->IsConnected())
            {
                pBottomSizer->Add(&m_offlineSend);
                pBottomSizer->AddSpacer(20);
            }
            pBottomSizer->Add(&m_onlineSend);

            pMainSizer->Add(&m_titleBar, UISizerFlags().Expand());
            pMainSizer->Add(pTopSizer, UISizerFlags().Expand());
            pMainSizer->Add(&m_barOldContent, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin).Border(dkTOP | dkBOTTOM, elementSpacing));
            pMainSizer->Add(&m_txtOldContent, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin));
            pMainSizer->Add(pNewContentSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin).Border(dkTOP, horizontalMargin));
            pMainSizer->Add(&m_txtNewContent, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin).Border(dkTOP | dkBOTTOM, elementSpacing));
            pMainSizer->Add(pBottomSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin).Border(dkBOTTOM, elementSpacing));
            pMainSizer->AddStretchSpacer();

            SetSizer(pMainSizer);
            Layout();
        }
        else
        {
            SafeDeletePointer(pMainSizer);
            SafeDeletePointer(pTopSizer);
            SafeDeletePointer(pNewContentSizer);
            SafeDeletePointer(pBottomSizer);
        }
    }
}

void UIBookCorrectionDlg::OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam)
{
    switch(dwCmdId)
    {
#ifdef KINDLE_FOR_TOUCH
    case ID_BTN_TOUCH_BACK:
        {
            EndDialog(0);
        }
        break;
#endif
    case ID_BTN_BOOK_OFFLINE_KEYSEND:
    case ID_BTN_BOOK_KEYSEND:
        {
            if (m_pBookReader != 0)
            {
                const int chapter = m_startPos.GetChapterIndex();
                const int para = m_startPos.GetParaIndex();
                const int atom = m_startPos.GetAtomIndex();
                char pos[256] = {0};
                snprintf(pos, DK_DIM(pos), "(%d,%d,%d)", chapter, para, atom);
                string bookId = m_pBookReader->GetBookId();
                string bookName = m_pBookReader->GetBookInfo().title;
                string bookRevision = m_pBookReader->GetBookRevision();
                string position(pos);
                string suggestion =  m_txtNewContent.GetTextUTF8();
                DK_FLOWPOSITION posChapter(chapter, para, atom);
                string refChapter = m_pBookReader->GetChapterTitle(posChapter);

                if (dwCmdId == ID_BTN_BOOK_OFFLINE_KEYSEND)
                {
                    OfflineBookStoreManager* pBSOfflineManager = OfflineBookStoreManager::GetInstance();
                    if (pBSOfflineManager->ReportedBookBug(bookId.c_str(),
                        bookName.c_str(),
                        bookRevision.c_str(),
                        position.c_str(),
                        refChapter.c_str(),
                        m_txtOldContent.GetText(),
                        suggestion.c_str()))
                    {
                        ShowTips(StringManager::GetPCSTRById(BOOKCORRECTION_SAVED));
                        usleep(3000000);
                        EndDialog(0);
                    }
                }
                else 
                {
                    if (UIUtility::CheckAndReConnectWifi())
                    {
                        BookStoreInfoManager* pBSInfoManager = BookStoreInfoManager::GetInstance();
                        pBSInfoManager->ReportBookBug(bookId.c_str(),
                            bookName.c_str(),
                            bookRevision.c_str(),
                            position.c_str(),
                            refChapter.c_str(),
                            m_txtOldContent.GetText(),
                            suggestion.c_str());
                        ShowTips(StringManager::GetPCSTRById(BOOKCORRECTION_SENDING));
                    }else
                    {
                        OfflineBookStoreManager* pBSOfflineManager = OfflineBookStoreManager::GetInstance();
                        if (pBSOfflineManager->ReportedBookBug(bookId.c_str(),
                            bookName.c_str(),
                            bookRevision.c_str(),
                            position.c_str(),
                            refChapter.c_str(),
                            m_txtOldContent.GetText(),
                            suggestion.c_str()))
                        {
                            ShowTips(StringManager::GetPCSTRById(BOOKCORRECTION_SEND_CANCELED));
                            usleep(3000000);
                            EndDialog(0);
                        }
                    }
                }
            }
        }
        break;
    default:
        UIDialog::OnCommand(dwCmdId, pSender, dwParam);
        break;
    }
}

bool UIBookCorrectionDlg::OnMessageReportBookBugUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const ReportBookBugUpdateArgs& msgArgs = (const ReportBookBugUpdateArgs&)args;
    string msg = msgArgs.message;
    if (msg.empty() && !msgArgs.succeeded)
    {
        msg = StringManager::GetPCSTRById(BOOKCORRECTION_SEND_FAILED);
    }
    if (msgArgs.succeeded)
    {
        msg = StringManager::GetPCSTRById(BOOKCORRECTION_SEND_SUCCEED);
    }
    ShowTips(msg);
    usleep(3000000);
    EndDialog(0);
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

void UIBookCorrectionDlg::EndDialog(INT32 iEndCode,BOOL fRepaintIt)
{
    HideTips();
    UIDialog::EndDialog(iEndCode, fRepaintIt);
}

HRESULT UIBookCorrectionDlg::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
    RTN_HR_IF_FAILED(grf.EraserBackGround());
#ifndef KINDLE_FOR_TOUCH
    const int lineTop = m_titleBar.GetHeight();
    const int lineHeight = 2;
    grf.FillRect(0, lineTop, m_iWidth, lineTop + lineHeight, ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
#endif
    return hr;
}

void UIBookCorrectionDlg::ShowTips(const std::string& tips)
{
    //HideTips();
    //m_pTips = UIUtility::CreateTip(tips.c_str(), this);
    //AppendChild(m_pTips);

    CDisplay::CacheDisabler forceDraw;
    //UIUtility::ShowTip(m_pTips, true);
    UpdateWindow();
    usleep(500000);
    UIUtility::SetScreenTips(tips.c_str());
}

void UIBookCorrectionDlg::HideTips()
{
    if (m_pTips)
    {
        UIUtility::ShowTip(m_pTips, false);
#ifdef KINDLE_FOR_TOUCH
        RemoveChild(m_pTips, false);
        SafeDeletePointer(m_pTips);
#else
        RemoveChild(m_pTips);
        SafeDeletePointer(m_pTips);
#endif
        UpdateWindow();
    }
}

