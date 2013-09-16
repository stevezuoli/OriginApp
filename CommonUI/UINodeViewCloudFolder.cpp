#include "CommonUI/UINodeViewCloudFolder.h"
#include "DkSPtr.h"
#include "Public/Base/TPDef.h"
#include "I18n/StringManager.h"
#include "Common/FileManager_DK.h"
#include "Common/WindowsMetrics.h"

#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"

#include "Utility/FileSystem.h"
#include "Utility/PathManager.h"
#include "Utility/StringUtil.h"
#include "Utility/RenderUtil.h"
#include "drivers/DeviceInfo.h"
#include <tr1/functional>
#include "Model/cloud_category_node.h"

using namespace WindowsMetrics;
using namespace dk::utility;
using namespace dk::document_model;

UINodeViewCloudFolder::UINodeViewCloudFolder(UIModelView* pParent,
                                             ModelTree* model_tree, 
                                             BookListUsage usage,
                                             int iconWidth,
                                             int iconHeight)
    : IUINodeView(pParent, model_tree, usage, iconWidth, iconHeight)
{
}

UINodeViewCloudFolder::~UINodeViewCloudFolder()
{
}

void UINodeViewCloudFolder::update()
{
    m_txtItemNameLabel.SetText(m_node->displayName());
    m_imgItem.SetImage(m_node->getInitialImage());
    m_imgItem.SetMinSize(dkSize(GetWindowMetrics(UIHomePageImageMaxWidthIndex), m_imgItem.GetMinHeight()));
    m_cover.SetIsDir(true);
    m_cover.SetDirNameOrBookName(m_node->displayName());
    RetrieveChildrenResult ret = RETRIEVE_FAILED;

    // use parent's status filter (infection)
    ContainerNode* container = dynamic_cast<ContainerNode*>(m_node.get());
    int status_filter = container->statusFilter();
    const ContainerNode* parent = dynamic_cast<const ContainerNode*>(m_node->parent());
    if (parent != 0)
    {
        status_filter = parent->statusFilter();
    }
    const NodePtrs& nodes  = container->children(ret, false, status_filter);
    char temp[32] = {0};
    snprintf(temp, DK_DIM(temp), StringManager::GetPCSTRById(BOOKSTORE_BOOK_COUNT), nodes.size());
    m_cover.SetDirBookNumOrBookAuthor(temp);
    m_cover.SetCoverImage(m_node->coverPath());

    IUINodeView::update();
}

UISizer* UINodeViewCloudFolder::getListViewSizer()
{
    if (NULL == m_listViewSizer)
    {
        m_listViewSizer = new UIBoxSizer(dkVERTICAL);
        if (m_listViewSizer)
        {
            const int horizontalMargin = GetWindowMetrics(UIPixelValue30Index);
            m_listViewSizer->AddStretchSpacer();

            UISizer* horzSizer = new UIBoxSizer(dkHORIZONTAL);
            if (horzSizer)
            {
                m_imgItem.SetAutoSize(true);
                m_imgDirArrow.SetImage(IMAGE_ARROW_STRONG_RIGHT);
                AppendChild(&m_imgItem);
                AppendChild(&m_txtItemNameLabel);
                AppendChild(&m_imgDirArrow);
                AppendChild(&m_imgSelect);

                horzSizer->Add(&m_imgItem, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
                horzSizer->Add(&m_txtItemNameLabel, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Reduce(1));
                horzSizer->AddStretchSpacer();
                horzSizer->Add(&m_imgDirArrow, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
                horzSizer->Add(&m_imgSelect, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, GetWindowMetrics(UIPixelValue10Index)));

                m_listViewSizer->Add(horzSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin));
            }
            AppendChild(&m_readingProgressBar);

            m_listViewSizer->Add(&m_readingProgressBar, UISizerFlags().Border(dkRIGHT, horizontalMargin).Border(dkLEFT, horizontalMargin + GetWindowMetrics(UIHomePageImageMaxWidthIndex)).Expand());
            m_listViewSizer->AddStretchSpacer();
        }
    }
    return m_listViewSizer;
}

void UINodeViewCloudFolder::SetModelDisplayMode(ModelDisplayMode mode)
{
    IUINodeView::SetModelDisplayMode(mode);
    m_imgDirArrow.SetVisible((mode == BLM_LIST) && !isSelectMode());
}

void UINodeViewCloudFolder::execute(unsigned long command_id)
{
}

bool UINodeViewCloudFolder::isSelectMode() const
{
    return m_cover.usage() == BLU_SELECT_EXPAND;
}

void UINodeViewCloudFolder::handleClicked()
{
    if (BLU_SELECT_EXPAND == m_cover.usage()) //&& (rootNodeDisplayMode() != BY_FOLDER))
    {
        SetSelected(!IsSelected());
        UpdateWindow();
        NodeSelectedArgs args;
        args.selected = IsSelected();
        args.node_path = m_node->absolutePath();
        FireEvent(EventNodeSelected, args);
        return;
    }
    SetHighLight(true);

    //CDisplay::GetDisplay()->SetFullRepaint(true);
    //current_page_Stack.push_back(current_page_);
    openFolder();
}

bool UINodeViewCloudFolder::openFolder()
{
    ModelTree* model_tree = ModelTree::getModelTree(MODEL_LOCAL_FILESYSTEM);
    ContainerNode* goto_node = model_tree->cd(m_node);
    return (goto_node != 0);
}

std::vector<int> UINodeViewCloudFolder::getSupportedNodeTypes()
{
    std::vector<int> support_types;
    support_types.push_back(NODE_TYPE_MICLOUD_CATEGORY);
    return support_types;
}
