////////////////////////////////////////////////////////////////////////
// IUINodeView.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "CommonUI/IUINodeView.h"
#include <tr1/functional>
#include "DkSPtr.h"
#include "Public/Base/TPDef.h"
#include "I18n/StringManager.h"
#include "Common/FileManager_DK.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Common/BookCoverLoader.h"
#include "Utility/FileSystem.h"
#include "Utility/PathManager.h"
#include "Utility/StringUtil.h"
#include "Utility/RenderUtil.h"
#include "drivers/DeviceInfo.h"
#include "Model/container_node.h"
#include "Model/local_doc_node.h"
#include "Model/cloud_file_node.h"
#include "CommonUI/UINodeViewOfLocalDir.h"
#include "CommonUI/UINodeViewOfLocalFile.h"
#include "CommonUI/UINodeViewCloudFile.h"
#include "CommonUI/UINodeViewCloudFolder.h"
#include "TouchAppUI/UIModelView.h"

using namespace WindowsMetrics;
using namespace dk::utility;
using namespace dk::document_model;

const char* IUINodeView::EventNodeSelected = "EventNodeSelected";
const char* IUINodeView::EventNodeOpenBook = "EventNodeOpenBook";

IUINodeView::IUINodeView(
        UIModelView* pParent,
        ModelTree* model_tree,
        BookListUsage usage,
        int iconWidth,
        int iconHeight)
        : UIListItemBase(pParent, IDSTATIC)
        , m_usage(usage)
        , m_modelDisplayMode(BLM_LIST)
        , m_coverViewSizer(NULL)
        , m_listViewSizer(NULL)
        , m_cover(this,
            model_tree,
            usage,
            BookCoverLoader::GetInstance()->GetMinimumCoverWidth(),
            BookCoverLoader::GetInstance()->GetMinimumCoverHeight(),
            iconWidth,
            iconHeight)
{
    SetShowBottomLine(false);
    SetLongClickable(true);
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }

    m_txtItemNameLabel.SetMinWidth(dkDefaultCoord);
    m_txtItemNameLabel.SetFontSize(GetWindowFontSize(UIBookListItemFileNameIndex));

    m_readingProgressBar.SetMaxDot(10);
    m_readingProgressBar.SetPercentageProgess(0);
    m_readingProgressBar.SetBarHeight(GetWindowMetrics(UINodeViewProgressBarHeightIndex));
    m_readingProgressBar.SetMinWidth(GetWindowMetrics(UINodeViewProgressBarWidthIndex));
    m_readingProgressBar.SetMinHeight(GetWindowMetrics(UIPixelValue5Index));

    m_imgSelect.SetAutoSize(true);
    m_imgSelect.SetImage(ImageManager::GetImage(IMAGE_ICON_COVER_UNSELECTED));
    m_imgSelect.SetMinSize(dkSize(GetWindowMetrics(UIHomePageImageMaxWidthIndex), m_imgSelect.GetMinHeight()));

    SubscribeMessageEvent(IUINodeView::EventNodeSelected,
        *this,
        std::tr1::bind(
        std::tr1::mem_fn(&UIModelView::onNodeSelected),
            pParent,
            std::tr1::placeholders::_1));
    SubscribeEvent(IUINodeView::EventNodeOpenBook,
        *this,
        std::tr1::bind(
        std::tr1::mem_fn(&UIModelView::onNodeOpenBook),
            pParent,
            std::tr1::placeholders::_1));
}

IUINodeView::~IUINodeView()
{
}

IUINodeView* IUINodeView::createNodeView(UIModelView* pParent, 
                                         ModelTree* model_tree, 
                                         NodePtr node, 
                                         BookListUsage usage, 
                                         int iconWidth, 
                                         int iconHeight)
{
    IUINodeView* node_view = 0;
    if (node)
    {
        switch(node->type())
        {
        case NODE_TYPE_CATEGORY_LOCAL_BOOK_STORE:
        case NODE_TYPE_CATEGORY_LOCAL_PUSHED:
        case NODE_TYPE_CATEGORY_LOCAL_DOCUMENTS:
        case NODE_TYPE_CATEGORY_LOCAL_FOLDER:
        case NODE_TYPE_CATEGORY_VIRTUAL_BOOK_STORE:
            node_view = new UINodeViewOfLocalDir(pParent, model_tree, usage, iconWidth, iconHeight);
            break;
        case NODE_TYPE_FILE_LOCAL_DOCUMENT:
        case NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK:
            node_view = new UINodeViewOfLocalFile(pParent, model_tree, usage, iconWidth, iconHeight);
            break;
        case NODE_TYPE_MICLOUD_CATEGORY:
            node_view = new UINodeViewCloudFolder(pParent, model_tree, usage, iconWidth, iconHeight);
            break;
        case NODE_TYPE_MICLOUD_BOOK:
            node_view = new UINodeViewCloudFile(pParent, model_tree, usage, iconWidth, iconHeight);
            break;
        default:
            {
                //nodeView = new UINodeView(pParent, model_tree, usage, iconWidth, iconHeight);
            }
            break;
        }
    }
    return node_view;
}

void IUINodeView::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
    UIListItemBase::MoveWindow(iLeft, iTop, iWidth, iHeight);

    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        if (mainSizer)
        {
            getListViewSizer();
            getCoverViewSizer();
            if (m_listViewSizer)
            {
                mainSizer->Add(m_listViewSizer, UISizerFlags(1).Expand());
            }

            if (m_coverViewSizer)
            {
                mainSizer->Add(m_coverViewSizer, UISizerFlags(1).Expand());
            }
            SetSizer(mainSizer);
            SetModelDisplayMode(m_modelDisplayMode);
        }
    }
}

void IUINodeView::SetIsLoading(bool isLoading)
{
    switch (m_modelDisplayMode)
    {
    case BLM_ICON:
        m_cover.SetIsLoading(isLoading);
        break;
    case BLM_LIST:
        break;
    }
}

void IUINodeView::SetFocus(bool bIsFocus)
{
    if (m_bIsFocus != bIsFocus)
    {
        m_bIsFocus = bIsFocus;
    }
}

void IUINodeView::SetNode(NodePtr node)
{
    m_node = node;
    SubscribeMessageEvent(Node::EventChildrenIsReady,
        *m_node,
        std::tr1::bind(
            std::tr1::mem_fn(&IUINodeView::onChildrenReady),
            this,
            std::tr1::placeholders::_1));
    SubscribeMessageEvent(Node::EventNodeStatusUpdated,
        *m_node,
        std::tr1::bind(
        std::tr1::mem_fn(&IUINodeView::onStatusUpdated),
            this,
            std::tr1::placeholders::_1));
    SubscribeMessageEvent(Node::EventNodeLoadingFinished,
        *m_node,
        std::tr1::bind(
            std::tr1::mem_fn(&IUINodeView::onLoadingFinished),
            this,
            std::tr1::placeholders::_1));
    update();
}

void IUINodeView::SetHighLight(bool isHighLight)
{
    if (BLM_ICON == m_modelDisplayMode)
    {
        m_cover.SetHighlight(isHighLight);
        UpdateWindow();
    }
    else
    {
        UIListItemBase::SetHighLight(isHighLight);
    }
}

UISizer* IUINodeView::getListViewSizer()
{
    return 0;
}

UISizer* IUINodeView::getCoverViewSizer()
{
    if (0 == m_coverViewSizer)
    {
        m_coverViewSizer = new UIBoxSizer(dkVERTICAL);
        if (m_coverViewSizer)
        {
            m_coverViewSizer->SetName("CoverViewSizer");
            m_coverViewSizer->Add(&m_cover, UISizerFlags(1).Expand());
        }
    }
    return m_coverViewSizer;
}

void IUINodeView::update()
{
    // TODO. Implement Me
    updateSelectedStatus();
}

void IUINodeView::SetModelDisplayMode(ModelDisplayMode mode)
{
    m_modelDisplayMode = mode;
    if (m_windowSizer)
    {
        switch (m_modelDisplayMode)
        {
        case BLM_ICON:
            m_windowSizer->Hide(m_listViewSizer);
            m_windowSizer->Show(m_coverViewSizer);
            m_imgSelect.SetVisible(false);
            break;
        case BLM_LIST:
            m_windowSizer->Hide(m_coverViewSizer);
            m_windowSizer->Show(m_listViewSizer);
            m_imgSelect.SetVisible(isSelectMode());
            break;
        }
    }
}

bool IUINodeView::onChildrenReady(const EventArgs& args)
{
    return onChildrenReadyAction(args);
}

bool IUINodeView::onChildrenReadyAction(const EventArgs& args)
{
    // TODO. Implement me in children
    return false;
}

bool IUINodeView::onStatusUpdated(const EventArgs& args)
{
    return onStatusUpdatedAction(args);
}

bool IUINodeView::isSelectMode() const
{
    return m_usage != BLU_BROWSE;
}

bool IUINodeView::onStatusUpdatedAction(const EventArgs& args)
{
    const NodeStatusUpdated& status_update_args = dynamic_cast<const NodeStatusUpdated&>(args);
    if (status_update_args.current_node_path == m_node->absolutePath())
    {
        update();
    }
    return true;
}

bool IUINodeView::onLoadingFinished(const EventArgs& args)
{
    return onLoadingFinishedAction(args);
}

bool IUINodeView::onLoadingFinishedAction(const EventArgs& args)
{
    // TODO. Implement me in children
    return false;
}

bool IUINodeView::IsSelected() const
{
    return isSelectMode() ? m_node->selected() : false;
}

void IUINodeView::SetSelected(bool selected)
{
    if (isSelectMode())
    {
        m_node->setSelected(selected);
        m_cover.SetSelected(selected);
        m_imgSelect.SetImage(ImageManager::GetImage(
            selected 
            ? IMAGE_ICON_COVER_SELECTED 
            : IMAGE_ICON_COVER_UNSELECTED));
    }
}

void IUINodeView::updateSelectedStatus()
{
    if (isSelectMode())
    {
        m_cover.SetSelected(m_node->selected());
        m_imgSelect.SetImage(ImageManager::GetImage(
            m_node->selected() 
            ? IMAGE_ICON_COVER_SELECTED 
            : IMAGE_ICON_COVER_UNSELECTED));
        UpdateWindow();
    }
}

void IUINodeView::execute(unsigned long command_id)
{
    // TODO. Implement Me in children
}

void IUINodeView::handleClicked()
{
    // TODO. Implement Me in children
}

std::vector<int> IUINodeView::getSupportedNodeTypes()
{
    return std::vector<int>();
}

bool IUINodeView::support(NodePtr node)
{
    int type = node->type();
    std::vector<int> supported_types = getSupportedNodeTypes();
    for (int i = 0; i < supported_types.size(); ++i)
    {
        if (supported_types[i] == type)
        {
            return true;
        }
    }
    return false;
}
