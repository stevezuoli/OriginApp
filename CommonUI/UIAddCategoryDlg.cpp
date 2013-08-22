#include "CommonUI/UIAddCategoryDlg.h"
#include "CommonUI/UIIMEManager.h"
#include "GUI/UISizer.h"
#include "I18n/StringManager.h"
#include "Common/WindowsMetrics.h"
#include "drivers/DeviceInfo.h"
#include <tr1/functional>

using namespace WindowsMetrics;

UIAddCategoryDlg::UIAddCategoryDlg(UIContainer* parent, CategoryOperation opt)
    : UIDialog(parent)
    , m_catOpt(opt)
{
    InitUI();
    SubscribeEvent(
            UIAbstractTextEdit::EventTextEditChange,
            m_input,
            std::tr1::bind(
                std::tr1::mem_fn(&UIAddCategoryDlg::OnInputChange),
                this, std::tr1::placeholders::_1));
    UpdateSaveButtonStatus();
}

void UIAddCategoryDlg::InitUI()
{
    const int topPadding = GetWindowMetrics(UIAddCategoryDlgVerPaddingIndex);
    const int leftPadding = GetWindowMetrics(UIAddCategoryDlgHorPaddingIndex);
    const int vertPadding = topPadding;
    const int btnHorPadding = GetWindowMetrics(UIAddCategoryDlgBtnHorPaddingIndex);
    const int btnVerPadding = GetWindowMetrics(UIAddCategoryDlgBtnVerPaddingIndex);
    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    mainSizer->AddSpacer(topPadding);
    mainSizer->Add(&m_txtHint,
            UISizerFlags().Border(dkLEFT | dkRIGHT, leftPadding));
    mainSizer->AddSpacer(vertPadding);
    mainSizer->Add(&m_input,
            UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, leftPadding));
    mainSizer->AddSpacer(vertPadding);
    mainSizer->Add(&m_btnCancelSaveGroup, 
            UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, btnHorPadding).Border(dkTOP | dkBOTTOM, btnVerPadding));
    m_btnCancelSaveGroup.SetBottomLinePixel(0);//0 means no bottom line

    const int fontSize = GetWindowFontSize(FontSize20Index);

    if (m_catOpt == CAT_ADD)
        m_txtHint.SetText(StringManager::GetPCSTRById(CATEGORY_INPUT_NAME));
    else if (m_catOpt == CAT_RENAME)
        m_txtHint.SetText(StringManager::GetPCSTRById(CATEGORY_RENAME));
    m_txtHint.SetFontSize(GetWindowFontSize(FontSize24Index));
    AppendChild(&m_txtHint);

    m_input.SetMinHeight(GetWindowMetrics(UIAddCategoryDlgEditHeightIndex));
    m_input.SetFontSize(fontSize);
#ifdef KINDLE_FOR_TOUCH
    m_btnCancel.SetMinHeight(GetWindowMetrics(UIAddCategoryDlgBtnHeightIndex));
    m_btnSave.SetMinHeight(GetWindowMetrics(UIAddCategoryDlgBtnHeightIndex));
#endif
    AppendChild(&m_input);

    m_btnCancelSaveGroup.AddButton(&m_btnCancel);
    m_btnCancelSaveGroup.AddButton(&m_btnSave);

    m_btnCancelSaveGroup.SetMinHeight(GetWindowMetrics(UIAddCategoryDlgBtnGroupHeightIndex));
    m_btnCancelSaveGroup.SetSplitLineHeight(GetWindowMetrics(UIAddCategoryDlgBtnHeightIndex) - (GetWindowMetrics(UIButtonGroupSplitLineTopPaddingIndex) << 1));
    m_btnCancel.SetText(ACTION_CANCEL);
    m_btnCancel.SetFontSize(fontSize);

    m_btnSave.SetText(TOUCH_ACTION_SAVE);
    m_btnSave.SetFontSize(fontSize);
    AppendChild(&m_btnCancelSaveGroup);

    SetSizer(mainSizer);
    dkSize minSize = mainSizer->GetMinSize();
    MakeCenterAboveIME(DeviceInfo::GetDisplayScreenWidth() - (GetWindowMetrics(UIHorizonMarginIndex) << 1), minSize.y);
    Layout();
    m_input.SetFocus(true);
    UIIME* pIME = UIIMEManager::GetIME(IUIIME_CHINESE_LOWER, &m_input);
    if (pIME)
    {
        pIME->SetShowDelay(true);
    }
}

HRESULT UIAddCategoryDlg::DrawBackGround(DK_IMAGE drawingImg)
{
    DrawBackgroundWithRoundBorder(drawingImg);
    return S_OK;
}

std::string UIAddCategoryDlg::GetInputText()
{
    return m_input.GetTextUTF8();
}

void UIAddCategoryDlg::UpdateSaveButtonStatus()
{
    m_btnSave.SetEnable(!m_input.GetTextUTF8().empty());
    m_btnSave.UpdateWindow();
}

bool UIAddCategoryDlg::OnInputChange(const EventArgs& args)
{
    UpdateSaveButtonStatus();
    return true;
}
bool UIAddCategoryDlg::OnChildClick(UIWindow* child)
{
    if (child == &m_btnSave)
    {
        EndDialog(IDOK);
        return true;
    }
    else if (child == &m_btnCancel)
    {
        EndDialog(IDCANCEL);
        return true;
    }
    return false;
}
#ifndef KINDLE_FOR_TOUCH
void UIAddCategoryDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    OnChildClick(pSender);
}
#endif
