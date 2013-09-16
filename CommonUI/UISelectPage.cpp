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
    , m_btnBack(this)
{
    CONNECT(m_modelView, UIModelView::EventChildSelectChanged, this, UISelectPage::OnChildSelectChanged)
    CONNECT(m_modelView, UIModelView::EventNodesUpdated, this, UISelectPage::OnNodesUpdated)
    CONNECT(m_modelView, UICompoundListBox::ListTurnPage, this, UISelectPage::OnListTurnPage)
    AppendChild(&m_titleBar);
}

void UISelectPage::InitModelView()
{
    const int listItemHeight = GetWindowMetrics(UIHomePageListItemHeightIndex);
    const int listItemSpacing = GetWindowMetrics(UIHomePageListItemSpacingIndex);
    m_modelView.SetItemHeight(listItemHeight);
    m_modelView.SetItemSpacing(listItemSpacing);
    m_modelView.SetItemWidth(DeviceInfo::GetDisplayScreenWidth());
    m_modelView.SetModelDisplayMode(SystemSettingInfo::GetInstance()->GetModelDisplayMode());
}

UISizer* UISelectPage::CreateTopSizer()
{
    if (NULL == m_pTopSizer)
    {
        m_pTopSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pTopSizer)
        {
            m_txtTitle.SetFontSize(GetWindowFontSize(FontSize26Index));
            UpdateModelView(false);

            AppendChild(&m_btnBack);
            AppendChild(&m_txtTitle);

            const int horzSpacing = GetWindowMetrics(UIPixelValue10Index);
            const int Btnmargin = GetWindowMetrics(UIPixelValue5Index);
            int imageId = (BLM_ICON == SystemSettingInfo::GetInstance()->GetModelDisplayMode()) ? IMAGE_ICON_BOOKLIST_LIST : IMAGE_ICON_BOOKLIST_COVER;
            m_imageMode.SetIcon(ImageManager::GetImage(imageId), UIButton::ICON_CENTER);
            m_imageMode.SetInternalHorzSpacing(Btnmargin);
            m_imageMode.SetInternalVertSpacing(Btnmargin);

            imageId = (m_modelView.rootNodeDisplayMode() == EXPAND_ALL) ? IMAGE_ICON_CLOSEFOLDER : IMAGE_ICON_OPENFOLDER;
            m_expandMode.SetIcon(ImageManager::GetImage(imageId), UIButton::ICON_CENTER);
            m_expandMode.SetInternalHorzSpacing(Btnmargin);
            m_expandMode.SetInternalVertSpacing(Btnmargin);

            m_selectAll.SetText(SELECT_ALL);
            m_selectAll.SetFontSize(GetWindowFontSize(FontSize20Index));
            m_selectAll.SetInternalHorzSpacing(Btnmargin);
            m_selectAll.SetInternalVertSpacing(Btnmargin);

            m_btnGroup.AddButton(&m_imageMode, UISizerFlags().Expand().Center().Border(dkLEFT | dkRIGHT, horzSpacing));
            m_btnGroup.AddButton(&m_expandMode, UISizerFlags().Expand().Center().Border(dkLEFT | dkRIGHT, horzSpacing));
            m_btnGroup.AddButton(&m_selectAll, UISizerFlags().Expand().Center().Border(dkLEFT | dkRIGHT, horzSpacing));
            AppendChild(&m_btnGroup);
            m_btnGroup.SetTopLinePixel(0);
            m_btnGroup.SetBottomLinePixel(0);
            m_btnGroup.SetSplitLineHeight(GetWindowMetrics(UIPixelValue20Index));

            const int margin = GetWindowMetrics(UIPixelValue10Index);
            const int rightMargin = GetWindowMetrics(UIPixelValue30Index) - horzSpacing - Btnmargin;
            m_pTopSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));
            m_pTopSizer->Add(&m_btnBack, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pTopSizer->Add(&m_txtTitle, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, margin));
            m_pTopSizer->Add(&m_btnGroup, UISizerFlags().Expand().Align(dkALIGN_RIGHT | dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, rightMargin));
        }
    }
    return m_pTopSizer;
}

UISizer* UISelectPage::CreateModelTreeSizer()
{
    if (NULL == m_pModelTreeSizer)
    {
        m_pModelTreeSizer = new UIBoxSizer(dkVERTICAL);
        if (m_pModelTreeSizer)
        {
            InitModelView();
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

bool UISelectPage::OnChildClick(UIWindow* child)
{
    if(child == &m_btnBack)
    {
        return OnBackClick();
    }
    else if (child == &m_selectAll)
    {
        return OnSelectedAllClick();
    }
    else if (child == &m_imageMode)
    {
        return OnImageModeClick();
    }
    else if (child == &m_expandMode)
    {
        return OnExpandModeClick();
    }
    return false;
}

bool UISelectPage::OnBackClick()
{
    if (m_modelView.rootNodeDisplayMode() == BY_FOLDER && m_modelView.BackToUpperFolder())
    {
        UpdateModelView(true);
    }
    else
    {
        CPageNavigator::BackToPrePage();
    }
    return true;
}

bool UISelectPage::OnSelectedAllClick()
{
    if (m_model)
    {
        ContainerNode* node = m_model->currentNode();
        if (node)
        {
            node->setSelected(!node->selected());
            //Repaint();
            NodeSelectedArgs args;
            args.selected = node->selected();
            args.node_path = node->absolutePath();
            m_modelView.onNodeSelected(args);
        }
    }
    return true;
}

bool UISelectPage::OnExpandModeClick()
{
    DKDisplayMode mode = (m_modelView.rootNodeDisplayMode() == BY_FOLDER) ? EXPAND_ALL : BY_FOLDER;
    BookListUsage usage = (mode == EXPAND_ALL) ? BLU_SELECT_EXPAND : BLU_SELECT;
    if (usage == BLU_SELECT_EXPAND)
    {
        m_modelView.cdRoot();
    }
    const int imageId = (usage == BLU_SELECT_EXPAND) ? IMAGE_ICON_CLOSEFOLDER : IMAGE_ICON_OPENFOLDER;
    m_expandMode.SetIcon(ImageManager::GetImage(imageId), UIButton::ICON_CENTER);
    m_modelView.setRootNodeDisplayMode(mode);
    m_modelView.setBookUsage(usage);
    Repaint();
    return true;
}

bool UISelectPage::OnImageModeClick()
{
    ModelDisplayMode newMode = (BLM_ICON == SystemSettingInfo::GetInstance()->GetModelDisplayMode()) ? BLM_LIST : BLM_ICON;
    SystemSettingInfo::GetInstance()->SetModelDisplayMode(newMode);
    const int imageId = (BLM_ICON == newMode) ? IMAGE_ICON_BOOKLIST_LIST : IMAGE_ICON_BOOKLIST_COVER;
    m_imageMode.SetIcon(ImageManager::GetImage(imageId), UIButton::ICON_CENTER);
    m_modelView.SetModelDisplayMode(newMode);
    m_modelView.InitListItem();
    Repaint();
    return true;
}

void UISelectPage::UpdateModelView(bool layout)
{
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

void UISelectPage::OnEnter()
{
    m_modelView.updateModelByContext(m_model);
    m_modelView.UpdateListItem();
    Repaint();
}