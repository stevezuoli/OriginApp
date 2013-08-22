#include "TouchAppUI/UIReadingSettingPanel1.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "Common/SystemSetting_DK.h"
#include "TouchAppUI/UINumInput.h"
#include "TouchAppUI/UIButtonDlg.h"
#include "Utility/EncodeUtil.h"
#include "Framework/KernelGear.h"
#include "TouchAppUI/UICommentExportDlg.h"

using namespace WindowsMetrics;
using namespace dk::utility;

UIReadingSettingPanel1::UIReadingSettingPanel1()
{
    Init();
}

UIReadingSettingPanel1::~UIReadingSettingPanel1()
{
}

void UIReadingSettingPanel1::Init()
{
    AddSectionTitle(StringManager::GetPCSTRById(READ_SETTING));

    char buf[20];
    snprintf(buf, DK_DIM(buf), "%d", GetRepaintPage());
    UIBoxSizer* sizer = AddPushButton(
            StringManager::GetPCSTRById(ALL_REFRESH_SPAN),
            SBI_RefreshInterval,
            buf);
    m_refreshIntervalPageButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();

    m_refreshIntervalPage = CreateTextSingleLine(StringManager::GetPCSTRById(BOOK_PAGE), true);
    sizer->AddSpacer(m_itemHorzSpacing);
    sizer->Add(m_refreshIntervalPage, UISizerFlags().Center());


    const char* barText = StringManager::GetPCSTRById(PROGRESSBAR_SELECT_TYPE);
    const char* fullBarText = StringManager::GetPCSTRById(TOUCH_PROGRESSBAR_NORMAL_TYPE);
    const char* miniBarText = StringManager::GetPCSTRById(TOUCH_PROGRESSBAR_MINI_TYPE);
    int barIds[] = {SBI_ProgressBarFull, SBI_ProgressBarMini};
    const char* barTexts[] = {fullBarText, miniBarText};
    UISizer* curRightSizer = AddRadioButtonGroups(barText, 0, 2, barIds, barTexts, GetProgressBarMode());

    m_progressBarModeGroup = (UIRadioButtonGroup*)curRightSizer->GetChildren().front()->GetWindow();

    const char* itemText = StringManager::GetPCSTRById(TEXT_CONVERT);
    int textConvertIds[] = {
        SBI_TextConvertSimpleChinese,
        SBI_TextConvertTraditionalChinese};
    const char* textConvertTexts[] = {
        StringManager::GetPCSTRById(FONT_STYLE_GBK),
        StringManager::GetPCSTRById(FONT_STYLE_BIG5)};
    curRightSizer = AddRadioButtonGroups(itemText, 0, 2, textConvertIds, textConvertTexts, GetTextConvertMode());
    m_textConvertGroup = (UIRadioButtonGroup*)curRightSizer->GetChildren().front()->GetWindow();

    int fontRenderIds[] = {SBI_FontRenderSharp, SBI_FontRenderSmooth};
    const char* fontRenderTexts[] = {
        StringManager::GetPCSTRById(FONT_RENDER_SHARP),
        StringManager::GetPCSTRById(FONT_RENDER_SMOOTH)};
    curRightSizer = AddRadioButtonGroups(
            StringManager::GetPCSTRById(FONT_RENDER),
            0, 2, fontRenderIds, fontRenderTexts, GetFontRender());
    m_fontRenderGroup = (UIRadioButtonGroup*)curRightSizer->GetChildren().front()->GetWindow();

    InitFonts();
    AddArrowButton(StringManager::GetPCSTRById(TOUCH_CUSTOM_CNS_FONT), SBI_ChineseFont, m_curChineseFontName.c_str()); 
    m_chineseFontButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();
    AddArrowButton(StringManager::GetPCSTRById(TOUCH_CUSTOM_ENG_FONT), SBI_EnglishFont, m_curEnglishFontName.c_str()); 
    m_englishFontButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();

    AddPushButton(
            StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_COMMENT_EXPORT_TITLE),
            SBI_ExportReadingData,
            StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_COMMENT_EXPORT));

    int epubMobiIds[] = {SBI_EpubMobiMetaData, SBI_EpubMobiFileName};
    const char* epubMobiTexts[] = {
            StringManager::GetPCSTRById(EPUB_MOBI_METADATA_OPEN),
            StringManager::GetPCSTRById(EPUB_MOBI_METADATA_CLOSE)};

    curRightSizer = AddRadioButtonGroups(
            StringManager::GetPCSTRById(EPUB_MOBI_METADATA),
            0, 2, epubMobiIds, epubMobiTexts, GetEpubMobiMetaDataMode());
    m_epubMobiGroup = (UIRadioButtonGroup*)curRightSizer->GetChildren().front()->GetWindow();
}


void UIReadingSettingPanel1::OnRefreshIntervalClick()
{
    UINumInput numInputDlg(this);
    int width = numInputDlg.GetTotalWidth();
    int height = numInputDlg.GetTotalHeight();
    numInputDlg.SetInitNum(GetRepaintPage());
    int left = (GetWidth() - width) / 2;
    int top = m_refreshIntervalPageButton->GetY() + m_refreshIntervalPageButton->GetHeight();
    numInputDlg.MoveWindow(left, top, width, height);
    if (numInputDlg.DoModal())
    {
        int result = numInputDlg.GetInputNum();
        (result >= 1) || (result = 1);
        (result <= 99) || (result = 99);
        char buf[20];
        snprintf(buf, DK_DIM(buf), "%02d", result);
        SetRepaintPage(result);
        m_refreshIntervalPageButton->SetText(buf);
    }
}

void UIReadingSettingPanel1::OnProgressBarModeClick(int mode)
{
    m_progressBarModeGroup->SelectChild(mode);
    SetProgressBarMode(mode);
}

void UIReadingSettingPanel1::OnTextConvertModeClick(int mode)
{
    SetTextConvertMode(mode);
    m_textConvertGroup->SelectChild(mode);
}

void UIReadingSettingPanel1::OnFontRenderClick(int mode)
{
    SetFontRender(mode);
    m_fontRenderGroup->SelectChild(mode);
}

void UIReadingSettingPanel1::OnEpubMobiMetaDataMode(int mode)
{
    SetEpubMobiMetaDataMode(mode);
    m_epubMobiGroup->SelectChild(mode);
}

bool UIReadingSettingPanel1::OnChildClick(UIWindow* child)
{
    switch (child->GetId())
    {
        case SBI_RefreshInterval:
            OnRefreshIntervalClick();
            break;
        case SBI_ProgressBarFull:
        case SBI_ProgressBarMini:
            OnProgressBarModeClick(child->GetId() - SBI_ProgressBarFull);
            break;
        case SBI_TextConvertSimpleChinese:
        case SBI_TextConvertTraditionalChinese:
            OnTextConvertModeClick(child->GetId() - SBI_TextConvertSimpleChinese);
            break;
        case SBI_FontRenderSharp:
        case SBI_FontRenderSmooth:
            OnFontRenderClick(child->GetId() - SBI_FontRenderSharp);
            break;
        case SBI_EpubMobiMetaData:
        case SBI_EpubMobiFileName:
            OnEpubMobiMetaDataMode(child->GetId() - SBI_EpubMobiMetaData);
            break;
        case SBI_ChineseFont:
            ShowFontDialog(m_chineseFontButton, m_chineseFontNames, m_selectedChineseFontIndex);
            break;
        case SBI_EnglishFont:
            ShowFontDialog(m_englishFontButton, m_englishFontNames, m_selectedEnglishFontIndex);
            break;
        case SBI_ExportReadingData:
            OnExportReadingData();
            break;
        default:
            break;
    }
    return true;
}

void UIReadingSettingPanel1::ShowFontDialog(UITouchComplexButton* button, const std::vector<std::string>& fontNames, int selectedIndex)
{
    UIButtonDlg dlg(this);
    dlg.SetData(fontNames, selectedIndex);
    int top = dlg.CalculateTop(button);
    dlg.MoveWindow(button->GetX(), top, m_fontMenuWidth, dlg.GetTotalHeight());
    dlg.DoModal();
    int newSelectedIndex = dlg.GetSelectedButtonID() - UIButtonDlg::BI_Base;
    if (newSelectedIndex >= 0 && newSelectedIndex != selectedIndex)
    {
        const std::string& newFontName = fontNames[newSelectedIndex];
        const std::wstring& wNewFontName = EncodeUtil::ToWString(newFontName);
        button->SetText(newFontName.c_str());
        Layout();
        if (button == m_englishFontButton)
        {
            m_selectedEnglishFontIndex = newSelectedIndex;
            g_pFontManager->SetDefaultFontFaceNameForCharSet(DK_CHARSET_ANSI, wNewFontName);
            m_systemSettingInfo->SetFontByCharset(DK_CHARSET_ANSI, newFontName.c_str());
        }
        else
        {
            DKFont* pFont = g_pFontManager->GetFontForFaceName(wNewFontName);
            if (NULL == pFont)
            {
                return;
            }
            if (pFont->IsSupportingCharSet(DK_CHARSET_GB)
                    || pFont->IsSupportingCharSet(DK_CHARSET_BIG5))
            {
                DK_CHARSET_TYPE chineseCharSets[] = {DK_CHARSET_BIG5, DK_CHARSET_SHITJIS, DK_CHARSET_JOHAB, DK_CHARSET_GB};
                for (size_t i = 0; i < DK_DIM(chineseCharSets); ++i)
                {
                    g_pFontManager->SetDefaultFontFaceNameForCharSet(chineseCharSets[i], wNewFontName);
                }
            }
            else
            {
                g_pFontManager->SetDefaultFontFaceNameForCharSet(DK_CHARSET_SHITJIS, wNewFontName);
                g_pFontManager->SetDefaultFontFaceNameForCharSet(DK_CHARSET_JOHAB, wNewFontName);

                std::wstring strSysDefaultFont(L"");
                g_pFontManager->GetSystemDefaultFontFaceName(&strSysDefaultFont);
                g_pFontManager->SetDefaultFontFaceNameForCharSet(DK_CHARSET_GB, strSysDefaultFont);
                g_pFontManager->SetDefaultFontFaceNameForCharSet(DK_CHARSET_BIG5, strSysDefaultFont);
            }

            m_selectedChineseFontIndex = newSelectedIndex;
            m_systemSettingInfo->SetFontByCharset(DK_CHARSET_GB, newFontName.c_str());
        }
        SetKernelDefaultFonts();
        m_systemSettingInfo->SetFontNameChanged(true);
    }
    //Repaint();
}

void UIReadingSettingPanel1::OnExportReadingData()
{
	UICommentExportDlg commentDlg(this, StringManager::GetStringById(TOUCH_COMMENT_EXPORT_TITLE));
	commentDlg.DoModal();
}

void UIReadingSettingPanel1::InitStatus()
{
    //char buf[20];
    //snprintf(buf, DK_DIM(buf), "%d", GetRepaintPage());
    //m_refreshIntervalPageButton->SetText(buf);
    //m_progressBarModeGroup->SelectChild(GetProgressBarMode());
    //m_epubMobiGroup->SelectChild(GetEpubMobiMetaDataMode());
    //m_fontRenderGroup->SelectChild(GetFontRender());
    if (m_textConvertGroup)
    {
        m_textConvertGroup->SelectChild(GetTextConvertMode());
    }
    InitFonts();
    if (m_chineseFontButton)
    {
        m_chineseFontButton->SetText(m_curChineseFontName.c_str());
    }
    if (m_englishFontButton)
    {
        m_englishFontButton->SetText(m_curEnglishFontName.c_str());
    }
}
