#include "TouchAppUI/UIDeleteCategoryConfirmDlg.h"
#include "GUI/UISizer.h"
#include "I18n/StringManager.h"
#include "Common/WindowsMetrics.h"
#include "drivers/DeviceInfo.h"

using namespace WindowsMetrics;

UIDeleteCategoryConfirmDlg::UIDeleteCategoryConfirmDlg(UIContainer* parent)
    : UIDialog(parent)
{
    InitUI();
}

void UIDeleteCategoryConfirmDlg::InitUI()
{
    const int topPadding = 20;
    const int leftPadding = 20;
    const int vertPadding = topPadding;
    const int horzPadding = 10;

    UIBoxSizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    mainSizer->AddSpacer(topPadding);
    mainSizer->Add(&m_txtHint, 
            UISizerFlags().Border(dkLEFT | dkRIGHT, leftPadding));
    mainSizer->AddSpacer(vertPadding);
    mainSizer->Add(&m_btnDeleteBooks,
            UISizerFlags().Border(dkLEFT | dkRIGHT, leftPadding));
    mainSizer->AddSpacer(vertPadding);
    UIBoxSizer* btnSizer = new UIBoxSizer(dkHORIZONTAL);
    btnSizer->AddStretchSpacer();
    btnSizer->Add(&m_btnCancel);
    btnSizer->AddSpacer(horzPadding);
    btnSizer->Add(&m_btnOK);
    mainSizer->Add(btnSizer,
            UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, leftPadding));
    mainSizer->AddSpacer(topPadding);

    const int fontSize = GetWindowFontSize(FontSize20Index);
    const int btnLeftPadding = GetWindowMetrics(UIButtonLeftPaddingIndex);
    const int btnTopPadding = GetWindowMetrics(UIButtonTopPaddingIndex);

    m_txtHint.SetText(StringManager::GetPCSTRById(CATEGORY_DELETE_CATEGORY_CONFIRM));
    m_txtHint.SetFontSize(fontSize);

    m_btnDeleteBooks.SetText(CATEGORY_DELETE_CATEGORY_FILES_CONFIRM);
    m_btnDeleteBooks.SetFontSize(fontSize);

    m_btnCancel.SetFontSize(fontSize);
    m_btnCancel.SetText(ACTION_CANCEL);
    m_btnCancel.SetLeftMargin(btnLeftPadding);
    m_btnCancel.SetTopMargin(btnTopPadding);

    m_btnOK.SetFontSize(fontSize);
    m_btnOK.SetText(BOOK_DELETE);
    m_btnOK.SetLeftMargin(btnLeftPadding);
    m_btnOK.SetTopMargin(btnTopPadding);

    AppendChild(&m_txtHint);
    AppendChild(&m_btnDeleteBooks);
    AppendChild(&m_btnCancel);
    AppendChild(&m_btnOK);

    SetSizer(mainSizer);
    dkSize minSize = mainSizer->GetMinSize();
    MakeCenter(minSize.x, minSize.y);
    Layout();
}

bool UIDeleteCategoryConfirmDlg::OnChildClick(UIWindow* child)
{
    if (child == &m_btnDeleteBooks)
    {
        m_btnDeleteBooks.SetChecked(!m_btnDeleteBooks.IsChecked());
        m_btnDeleteBooks.UpdateWindow();
    }
    else if (child == &m_btnCancel)
    {
        EndDialog(IDCANCEL);
    }
    else if (child == &m_btnOK)
    {
        EndDialog(IDOK);
    }
    return true;
}

bool UIDeleteCategoryConfirmDlg::ShouldDeleteBooks() const
{
    return m_btnDeleteBooks.IsChecked();
}

HRESULT UIDeleteCategoryConfirmDlg::DrawBackGround(DK_IMAGE drawingImg)
{
    DrawBackgroundWithRoundBorder(drawingImg);
    return S_OK;
}
