////////////////////////////////////////////////////////////////////////
// IUINodeView.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "CommonUI/IUINodeView.h"
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
#include <tr1/functional>
#include "Model/container_node.h"
#include "Model/local_doc_node.h"
#include "Model/cloud_file_node.h"
#include "CommonUI/UINodeViewOfLocalDir.h"
#include "CommonUI/UINodeViewOfLocalFile.h"
#include "CommonUI/UINodeViewOfLocalfileUpload.h"

using namespace WindowsMetrics;
using namespace dk::utility;
using namespace dk::document_model;

IUINodeView* CreateNodeView(
    UICompoundListBox* pParent, 
    ModelTree* model_tree, 
    NodePtr node, 
    BookListUsage usage, 
    int iconWidth, 
    int iconHeight)
{
    IUINodeView* nodeView = NULL;
    if (node)
    {
        printf("-------------------------%d/%d---------------------\n", node->type(), usage);
        switch(node->type())
        {
        case NODE_TYPE_CATEGORY_LOCAL_BOOK_STORE:
        case NODE_TYPE_CATEGORY_LOCAL_PUSHED:
        case NODE_TYPE_CATEGORY_LOCAL_DOCUMENTS:
        case NODE_TYPE_CATEGORY_LOCAL_FOLDER:
        case NODE_TYPE_CATEGORY_VIRTUAL_BOOK_STORE:
            nodeView = new UINodeViewOfLocalDir(pParent, model_tree, usage, iconWidth, iconHeight);
            break;
        case NODE_TYPE_FILE_LOCAL_DOCUMENT:
        case NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK:
            if (usage == BLU_CLOUD_UPLOAD)
            {
                nodeView = new UINodeViewOfLocalfileUpload(pParent, model_tree, usage, iconWidth, iconHeight);
            }
            else if (usage == BLU_BROWSE)
            {
                nodeView = new UINodeViewOfLocalFile(pParent, model_tree, usage, iconWidth, iconHeight);
            }
            break;
        default:
            {
                printf("-------------------------error---------------------\n");
                //nodeView = new UINodeView(pParent, model_tree, usage, iconWidth, iconHeight);
            }
            break;
        }
    }
    return nodeView;
}

IUINodeView::IUINodeView(
        UICompoundListBox* pParent,
        ModelTree* model_tree,
        BookListUsage usage,
        int iconWidth,
        int iconHeight)
        : UIListItemBase(pParent, IDSTATIC)
        , m_selectMode(false)
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
}

IUINodeView::~IUINodeView()
{
}

void IUINodeView::SetSelectMode(bool showSelect)
{
    m_selectMode = showSelect;
}

void IUINodeView::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
    UIListItemBase::MoveWindow(iLeft, iTop, iWidth, iHeight);

    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        if (mainSizer)
        {
            CreateListViewSizer();
            CreateCoverViewSizer();
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
    updateByNode();
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

UISizer* IUINodeView::CreateListViewSizer()
{
    return NULL;
}

UISizer* IUINodeView::CreateCoverViewSizer()
{
    if (NULL == m_coverViewSizer)
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

void IUINodeView::updateByNode()
{
//     m_imgSelect.SetVisible(m_selectMode);
//     Layout();
//     Repaint();
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
            m_imgSelect.SetVisible(m_selectMode);
            break;
        }
    }
}


bool IUINodeView::onStatusUpdated(const EventArgs& args)
{
    const NodeStatusUpdated& status_update_args = dynamic_cast<const NodeStatusUpdated&>(args);
    if (status_update_args.current_node_path == m_node->absolutePath())
    {
        updateByNode();
    }
    return true;
}

bool IUINodeView::IsSelected() const
{
    return m_selectMode ? m_node->selected() : false;
}

void IUINodeView::SetSelected(bool selected)
{
    if (m_selectMode)
    {
        m_node->setSelected(selected);
        m_cover.SetSelected(selected);
        m_imgSelect.SetImage(ImageManager::GetImage(
            selected 
            ? IMAGE_ICON_COVER_SELECTED 
            : IMAGE_ICON_COVER_UNSELECTED));
    }
}

