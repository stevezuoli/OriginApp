#include "TouchAppUI/UIReadingSettingPanel2.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "Common/SystemSetting_DK.h"
#include "TouchAppUI/UIButtonDlg.h"

using namespace WindowsMetrics;

UIReadingSettingPanel2::UIReadingSettingPanel2()
{
    Init();
}

UIReadingSettingPanel2::~UIReadingSettingPanel2()
{
}

void UIReadingSettingPanel2::Init()
{
    AddSectionTitle(StringManager::GetPCSTRById(LAYOUT_SETTING));

    AddArrowButton(StringManager::GetPCSTRById(TOUCH_BOOK_FONT_SIZE),
            SBI_FontSize,
            FontSizeIndexToString(GetFontSizeIndex()).c_str()); 
    m_fontSizeButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();


    InitLayout();
    AddArrowButton(
            StringManager::GetPCSTRById(LAYOUT_OPTIONS),
            SBI_Layout,
            m_layoutNames[GetLayoutMode()].c_str());
    m_layoutButton = (UITouchComplexButton*)m_dynamicCreatedWindows.back();

	int readerLayoutIds[] = {SBI_ReadingLayoutFullScreen, SBI_ReadingLayoutShowReaderMessage};
    const char* readerLayoutTexts[] = {
        StringManager::GetPCSTRById(READERLAYOUT_FULLSCREEN),
        StringManager::GetPCSTRById(READERLAYOUT_SHOWREADERMESSAGE)};
     UISizer* readerLayoutSizer = AddRadioButtonGroups(
            StringManager::GetPCSTRById(READERLAYOUT),
            0, 2, readerLayoutIds, readerLayoutTexts, GetReadingLayout());
    m_readerLayoutGroup = (UIRadioButtonGroup*)readerLayoutSizer->GetChildren().front()->GetWindow();
}

bool UIReadingSettingPanel2::OnChildClick(UIWindow* child)
{
    switch (child->GetId())
    {
        case SBI_FontSize:
            OnFontSizeClick();
            break;
        case SBI_Layout:
            OnLayoutMode();
            break;
		case SBI_ReadingLayoutFullScreen:
        case SBI_ReadingLayoutShowReaderMessage:
			OnReadingLayoutClick(child->GetId() - SBI_ReadingLayoutFullScreen);
			break; 
        default:
            return true;
    }
    return true;
}

void UIReadingSettingPanel2::OnFontSizeClick()
{
    int totalFontSizes = GetFontSizeCount();
    std::vector<std::string> fontSizeStrings;
    for (int i = 0; i < totalFontSizes; ++i)
    {
        fontSizeStrings.push_back(FontSizeIndexToString(i));
    }
    UIButtonDlg dlg(this);
    dlg.SetData(fontSizeStrings, GetFontSizeIndex());
    int top = dlg.CalculateTop(m_fontSizeButton);
    dlg.MoveWindow(m_fontSizeButton->GetX(), top, GetWindowMetrics(FontSizeMenuWidthIndex), dlg.GetTotalHeight());
    dlg.DoModal();
    int index = dlg.GetSelectedButtonIndex();
    if (index != GetFontSizeIndex())
    {
        m_fontSizeButton->SetText(FontSizeIndexToString(index).c_str());
        SetFontSizeIndex(index);
    }
}

std::string UIReadingSettingPanel2::FontSizeIndexToString(int index)
{
    char buf[50];
    snprintf(buf, DK_DIM(buf), "%d%s",
            index + 1, StringManager::GetPCSTRById(BOOK_FONT_SIZE_POSTFIX));
    return buf;
}

void UIReadingSettingPanel2::OnReadingLayoutClick(int mode)
{
	m_readerLayoutGroup->SelectChild(mode);
    SetReadingLayout(mode);
}

void UIReadingSettingPanel2::OnLayoutMode()
{
    int mode = GetLayoutMode();
    UIButtonDlg dlg(this);
    dlg.SetData(m_layoutNames, mode);
    int top = dlg.CalculateTop(m_layoutButton);
    dlg.MoveWindow(m_layoutButton->GetX(), top, GetWindowMetrics(LayoutModeMenuWidthIndex), dlg.GetTotalHeight());
    dlg.DoModal();
    int index = dlg.GetSelectedButtonIndex();
    if (-1 != index && index != mode)
    {
        m_layoutButton->SetText(m_layoutNames[index].c_str());
        SetLayoutMode(index);
    }
}

void UIReadingSettingPanel2::InitStatus()
{
    m_readerLayoutGroup->SelectChild(GetReadingLayout());
    m_layoutButton->SetText(m_layoutNames[GetLayoutMode()].c_str());
    m_fontSizeButton->SetText(FontSizeIndexToString(GetFontSizeIndex()).c_str());
}

