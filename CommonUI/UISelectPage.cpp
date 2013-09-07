#include "CommonUI/UISelectPage.h"
#include "Common/WindowsMetrics.h"
#include "GUI/UISizer.h"
#include "I18n/StringManager.h"
#include "CommonUI/CPageNavigator.h"
#include "drivers/DeviceInfo.h"
#include "Utility/PathManager.h"
#include "BookStore/LocalCategoryManager.h"
#include <tr1/functional>

using namespace WindowsMetrics;
using namespace dk::bookstore;
using namespace dk::utility;

UISelectPage::UISelectPage(BookListUsage usage, ModelTree* model_tree)
    : m_pTopSizer(NULL)
    , m_pModelTreeSizer(NULL)
    , m_pBottomSizer(NULL)
    , m_model(model_tree)
    , m_modelView(usage, model_tree)
{
    CONNECT(m_modelView, UIModelView::EventChildSelectChanged, this, UISelectPage::OnChildSelectChanged)
    CONNECT(m_modelView, UIModelView::EventNodesUpdated, this, UISelectPage::OnNodesUpdated)
    CONNECT(m_modelView, UICompoundListBox::ListTurnPage, this, UISelectPage::OnListTurnPage)
}

UISizer* UISelectPage::CreateTopSizer()
{
    return m_pTopSizer;
}

UISizer* UISelectPage::CreateModelTreeSizer()
{
    if (NULL == m_pModelTreeSizer)
    {
        m_pModelTreeSizer = new UIBoxSizer(dkVERTICAL);
        if (m_pModelTreeSizer)
        {
            const int listItemHeight = GetWindowMetrics(UIHomePageListItemHeightIndex);
            const int listItemSpacing = GetWindowMetrics(UIHomePageListItemSpacingIndex);
            m_modelView.SetLeftMargin(GetWindowMetrics(UIPixelValue30Index));
            m_modelView.SetItemHeight(listItemHeight);
            int screenWidth = DeviceInfo::GetDisplayScreenWidth();
            m_modelView.SetItemWidth(screenWidth);
            m_modelView.SetItemSpacing(listItemSpacing);
            m_modelView.SetModelDisplayMode(SystemSettingInfo::GetInstance()->GetModelDisplayMode());
            AppendChild(&m_modelView);

            m_pModelTreeSizer->SetMinHeight(GetWindowMetrics(UIPixelValue61Index));
            m_pModelTreeSizer->Add(&m_modelView, UISizerFlags(1).Expand());
        }
    }
    return m_pModelTreeSizer;
}

UISizer* UISelectPage::CreateBottomSizer()
{
    return m_pBottomSizer;
}

void UISelectPage::MoveWindow(
        int left,
        int top,
        int width,
        int height)
{
    UIWindow::MoveWindow(left, top, width, height);
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        mainSizer->Add(&m_titleBar);

        UISizer* topSizer = CreateTopSizer();
        if (topSizer)
        {
            mainSizer->Add(topSizer, UISizerFlags().Expand());
        }

        UISizer* modelTreeSizer = CreateModelTreeSizer();
        if (modelTreeSizer)
        {
            mainSizer->Add(modelTreeSizer, UISizerFlags(1).Expand());
        }

        UISizer* bottomSizer = CreateBottomSizer();
        if (bottomSizer)
        {
            mainSizer->Add(bottomSizer, UISizerFlags().Expand());
        }

        SetSizer(mainSizer);
        Layout();
    }
    m_modelView.InitListItem();
}

bool UISelectPage::OnChildSelectChanged(const EventArgs& args)
{
    return true;
}

bool UISelectPage::OnListTurnPage(const EventArgs&)
{
    return true;
}

bool UISelectPage::OnNodesUpdated(const EventArgs& args)
{
    return true;
}
