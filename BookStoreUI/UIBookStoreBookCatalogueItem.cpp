#include "BookStoreUI/UIBookStoreBookCatalogueItem.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "Framework/CDisplay.h"
#include "interface.h"
#include "drivers/DeviceInfo.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;

UIBookStoreBookCatalogueItem::UIBookStoreBookCatalogueItem()
{
#ifdef KINDLE_FOR_TOUCH
	SetClickable(false);
#endif
    Init();
}

UIBookStoreBookCatalogueItem::~UIBookStoreBookCatalogueItem()
{

}

void UIBookStoreBookCatalogueItem::Init()
{
    const int fontsize20 = GetWindowFontSize(FontSize20Index);
    m_catalogue.SetFontSize(fontsize20);
    AppendChild(&m_catalogue);

    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    if (mainSizer)
    {
        mainSizer->AddStretchSpacer();
        mainSizer->Add(&m_catalogue, UISizerFlags().Expand());
        mainSizer->AddStretchSpacer();
        SetSizer(mainSizer);
    }
}

void UIBookStoreBookCatalogueItem::SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr catalogueInfo, int index)
{
    model::CatalogueInfo* catalogue = (model::CatalogueInfo*)(catalogueInfo.get());
    if(NULL != catalogue)
    {
        const int level = catalogue->GetCatalogueLevel() - 1;
        m_catalogue.SetText(catalogue->GetCatalogue().c_str());
        m_catalogue.SetFirstLineIndent(level << 1);
        m_preview = catalogue->GetPreview();
    }
}

bool UIBookStoreBookCatalogueItem::ResponseTouchTap()
{
    return true;
    /*UIContainer* pParent = GetParent();
    CDisplay* pDisplay = CDisplay::GetDisplay();
    if (pDisplay)
    {
        pParent = pDisplay->GetCurrentPage();
    }

    if (pParent)
    {
        string catalogue = m_catalogue.GetText();
        UIBookCataloguePreviewDlg previewDlg(pParent, catalogue, m_preview);
        previewDlg.DoModal();
    }
    return (pParent != NULL);*/
}

UIBookCataloguePreviewDlg::UIBookCataloguePreviewDlg(UIContainer* pParent, const std::string& title, const std::string& preview)
    : UIModalDlg(pParent, title.c_str())
{
    m_preview.SetFontSize(GetWindowMetrics(FontSize20Index));
    if (preview.empty())
    {
        m_preview.SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOKCATALOGUE_NOPREVIEW));
    }
    else
    {
        m_preview.SetText(preview.c_str());
    }
    AppendChild(&m_preview);

    Init();
}

UIBookCataloguePreviewDlg::~UIBookCataloguePreviewDlg()
{

}

void UIBookCataloguePreviewDlg::Init()
{
    if (m_pTopSizer)
    {
        const int parentHeight = m_pParent ? m_pParent->GetHeight() : DeviceInfo::GetDisplayScreenHeight();
        const int horizontalMargin = GetWindowMetrics(UIHorizonMarginIndex);
        const int elementSpacing = GetWindowMetrics(UIElementMidSpacingIndex);
        const int width = DeviceInfo::GetDisplayScreenWidth() - (horizontalMargin << 1);
        int height = m_pTopSizer->GetMinSize().GetHeight() + m_preview.GetHeightByWidth(width) + (elementSpacing << 1);
        if (height > parentHeight)
        {
            height = parentHeight;
        }
        const int left = (DeviceInfo::GetDisplayScreenWidth() - width) >> 1;
        const int top = (DeviceInfo::GetDisplayScreenHeight() - height) >> 1;
        MoveWindow(left, top, width, height);
        if (m_windowSizer)
        {
            m_preview.SetMaxWidth(width);
            m_windowSizer->Add(&m_preview, UISizerFlags(1).Expand().Border(dkALL, elementSpacing));
        }
        Layout();
    }
}
