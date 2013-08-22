#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "TouchAppUI/UINavigateToDlg.h"
#include "CommonUI/UIIMEManager.h"
using namespace WindowsMetrics;

UINavigateToDlg::UINavigateToDlg(UIContainer* parent, DisplayWay dw) :
UIModalDlg(parent, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_JUMPTITLE)) ,
    m_displayWay(dw) ,
    m_lTotalPage(0) ,
    m_lGotoPage(-1) ,
    m_fGotoPercent(0)
{
    InitUI();
}

UINavigateToDlg::~UINavigateToDlg()
{
}

void UINavigateToDlg::SetCurPage(long page)
{
    if (m_displayWay == DisplayPercent)
        return;

    char str[10] = {0};
    snprintf(str, DK_DIM(str), "%ld", page);
    m_editArgs.ClearText();
    m_editArgs.SetTipUTF8(str, strlen(str));
    Layout();
}

void UINavigateToDlg::SetCurPercent( double percent)
{
    if (m_displayWay==DisplayPage)
        return;

    char str[10] = {0};
    percent *= 100;
    snprintf(str, DK_DIM(str), "%.2lf", percent);
    m_editArgs.ClearText();
    m_editArgs.SetTipUTF8(str, strlen(str));
    Layout();
}

void UINavigateToDlg::InitUI()
{
    m_btnToEntry.Initialize(ID_BTN_TOUCH_NTDIALOG_ENTRY, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_ENTRY), GetWindowFontSize(UINavigateToDialogIndex));
    m_btnToTag.Initialize(ID_BTN_TOUCH_NTDIALOG_TAG, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_TAG), GetWindowFontSize(UINavigateToDialogIndex));
    m_btnNavigateTo.Initialize(ID_BTN_TOUCH_NTDIALOG_NAVIGATETO, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_JUMP), GetWindowFontSize(UINavigateToDialogIndex));
    m_btnCancel.Initialize(ID_BTN_TOUCH_NTDIALOG_CANCEL, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_CLOSE), GetWindowFontSize(UINavigateToDialogIndex));

    if (m_displayWay == DisplayPercent) {
        m_textSuffix.SetText("%");
    } else if (m_displayWay == DisplayPage) {
        char cText[50] = {0};
        snprintf(cText, DK_DIM(cText), "/%ld%s", m_lTotalPage, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_PAGE));
        m_textSuffix.SetText(cText);
    }

    m_textSuffix.SetFontSize(GetWindowFontSize(UINavigateToDialogIndex));
    m_textSuffix.SetAutoModifyWidth(true);
    m_textSuffix.SetEndWithEllipsis(false);

    m_editArgs.SetId(ID_BTN_TOUCH_NTDIALOG_NAVIGATETO);
    m_editArgs.SetFontSize(GetWindowFontSize(UINavigateToDialogIndex));
    if (m_displayWay == DisplayPage)
    {
        m_editArgs.SetTextBoxStyle(NUMBER_ONLY);
    }
    else if (m_displayWay == DisplayPercent)
    {
        m_editArgs.SetTextBoxStyle(PERCENTAGE);
    }

    AppendChild(&m_textSuffix);
    AppendChild(&m_btnToEntry);
    AppendChild(&m_btnToTag);
    AppendChild(&m_editArgs);
    m_btnGroup.AddButton(&m_btnCancel);
    m_btnGroup.AddButton(&m_btnNavigateTo);

    if (m_windowSizer)
    {
        m_btnToEntry.SetMinSize(dkSize(GetWindowMetrics(UINavigateToDlgBtnWidthIndex), GetWindowMetrics(UIModalDialogEditBoxIndex)));
        m_btnToTag.SetMinSize(dkSize(GetWindowMetrics(UINavigateToDlgBtnWidthIndex), GetWindowMetrics(UIModalDialogEditBoxIndex)));
        m_editArgs.SetMinSize(dkSize(GetWindowMetrics(UINavigateToDlgEditBoxWidthIndex), GetWindowMetrics(UIModalDialogEditBoxIndex)));
        //m_btnCancel.SetMinHeight(GetWindowMetrics(UIModalDialogEditBoxIndex));
        //m_btnNavigateTo.SetMinHeight(GetWindowMetrics(UIModalDialogEditBoxIndex));

        //m_windowSizer->AddSpacer(GetWindowMetrics(UINavigateToDlgTopMarginIndex));

        if (m_displayWay == DisplayPage)
        {
            UISizer* pPageSizer = new UIBoxSizer(dkHORIZONTAL);
            pPageSizer->Add(&m_btnToEntry);
            pPageSizer->AddSpacer(GetWindowMetrics(UINavigateToDlgVertMarginIndex));
            pPageSizer->Add(&m_editArgs, UISizerFlags(1).Border(dkRIGHT, GetWindowMetrics(UINavigateToDlgEditBoxRightMarginIndex)));
            pPageSizer->Add(&m_textSuffix, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            pPageSizer->AddSpacer(GetWindowMetrics(UINavigateToDlgVertMarginIndex));
            pPageSizer->Add(&m_btnToTag);

            m_windowSizer->Add(pPageSizer, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)).Expand());
        }

        if (m_displayWay == DisplayPercent)
        {
            UISizer* pPercentSizer = new UIBoxSizer(dkHORIZONTAL);
            pPercentSizer->Add(&m_btnToEntry);
            pPercentSizer->AddSpacer(GetWindowMetrics(UINavigateToDlgVertMarginIndex));
            pPercentSizer->Add(&m_editArgs, UISizerFlags(1).Border(dkRIGHT, GetWindowMetrics(UINavigateToDlgEditBoxRightMarginIndex)));
            pPercentSizer->Add(&m_textSuffix, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            pPercentSizer->AddSpacer(GetWindowMetrics(UINavigateToDlgVertMarginIndex));
            pPercentSizer->Add(&m_btnToTag);

            m_windowSizer->Add(pPercentSizer, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)).Expand());
        }
    
        m_windowSizer->AddSpacer(GetWindowMetrics(UINavigateToDlgBottomMarginIndex));

        //UISizer* bottomSizer = new UIBoxSizer(dkHORIZONTAL);
        //bottomSizer->Add(&m_btnCancel, 1);
        //bottomSizer->AddSpacer(GetWindowMetrics(UIModalDialogBottomBtnMarginIndex));
        //bottomSizer->Add(&m_btnNavigateTo, 1);

        //m_windowSizer->Add(bottomSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)));
        m_windowSizer->Add(&m_btnGroup, UISizerFlags().Expand());
        const int width = GetWindowMetrics(UIModalDialogWidthIndex);
        const int height = m_windowSizer->GetMinSize().GetHeight();
        MakeCenterAboveIME(width, height);

        Layout();
    }
}

 void UINavigateToDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
 {
    switch (dwCmdId) {
    case ID_BTN_TOUCH_NTDIALOG_ENTRY:
        m_lGotoPage = 0;
        m_fGotoPercent = 0;
        EndDialog(Internal);
        break;
    case ID_BTN_TOUCH_NTDIALOG_TAG:
        m_lGotoPage = m_lTotalPage+1;
        m_fGotoPercent = 100.0;
        EndDialog(Internal);
        break;
    case ID_BTN_TOUCH_NTDIALOG_NAVIGATETO:
        {
            std::string sPageNo = m_editArgs.GetTextUTF8();
            if (sPageNo.empty())
            {
                m_lGotoPage = 0;
                m_fGotoPercent = 0;
            }
            if (m_displayWay==DisplayPage)
            {
                m_lGotoPage = atoi(sPageNo.c_str());
                m_lGotoPage--;
            }
            else if (m_displayWay==DisplayPercent)
            {
                m_fGotoPercent = atof(sPageNo.c_str());
            }
            EndDialog(Internal);
        }
        break;
    case ID_BTN_TOUCH_NTDIALOG_CLOSE:
    case ID_BTN_TOUCH_NTDIALOG_CANCEL:
    case IDCANCEL:
        SetVisible(false);
        EndDialog(Invalid);
        break;
    default:
        break;
    }
 }

void UINavigateToDlg::Popup()
{
    UINT32 iSize = GetChildrenCount();
    for (UINT32 i = 0; i < iSize; i++)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (pWin) {
            pWin->SetVisible(true);
        }
    }

    UIIME* pIME = UIIMEManager::GetIME(IUIIME_ENGLISH_NUMBER, &m_editArgs);
    if (pIME)
    {
        m_editArgs.SetFocus(true);
        pIME->SetShowDelay(true);
    }

    UIDialog::Popup();
}

void UINavigateToDlg::EndDialog(INT32 iEndCode,BOOL fRepaintIt)
{
    UIDialog::EndDialog(iEndCode, fRepaintIt);
    UINT32 iSize = GetChildrenCount();
    for (UINT32 i = 0; i < iSize; i++)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (pWin) {
            pWin->SetVisible(false);
        }
    }
}


void UINavigateToDlg::SetTotalPage(long total)
{
    m_lTotalPage = total;
    if (m_displayWay == DisplayPercent) {
        return;
    } else if (m_displayWay == DisplayPage) {
        char cText[50] = {0};
        snprintf(cText, DK_DIM(cText), "/%ld%s", m_lTotalPage, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_PAGE));
        m_textSuffix.SetText(cText);
    }
    Layout();
}

long UINavigateToDlg::GetGotoPage() const
{
    return m_lGotoPage; 
}

double UINavigateToDlg::GetGotoPercent() const
{
    return m_fGotoPercent/100.0;
}
