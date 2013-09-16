///////////////////////////////////////////////////////////////////////
// UIModelView.cpp
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIModelView.h"
#include "TouchAppUI/UIButtonDlg.h"
#include "TouchAppUI/UIDeleteCategoryConfirmDlg.h"
#include "TouchAppUI/BookOpenManager.h"
#include "TouchAppUI/UIBookMenuDlg.h"
#include "TouchAppUI/UIAddBookToCategoryPage.h"

#include "CommonUI/UIWeiboSharePage.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIAddCategoryDlg.h"
#include "CommonUI/UIUtility.h"

//#include "Model/DirectoryItemModelImpl.h"
#include "Framework/CDisplay.h"
#include "Framework/CNativeThread.h"

#include "Common/WindowsMetrics.h"
#include "Common/BookCoverLoader.h"
#include "Common/File_DK.h"
#include "Common/FileManager_DK.h"
#include "Utility/FileSystem.h"
#include "Utility/SystemUtil.h"
#include "Utility/PathManager.h"
#include "Utility/StringUtil.h"
#include "Utility/CharUtil.h"

#include "I18n/StringManager.h"

#include "GUI/UISizer.h"
#include "GUI/UIMessageBox.h"
#include <tr1/functional>
#include "BookStore/LocalCategoryManager.h"
#include "Model/local_doc_node.h"
#include "Model/local_bookstore_category_node.h"
#include "Model/cloud_file_node.h"
#include <iostream>
#include <algorithm>
#include "drivers/DeviceInfo.h"
#include "SQM.h"

using namespace dk::utility;
using namespace dk::bookstore;
using namespace WindowsMetrics;
using namespace std;
using namespace dk::document_model;

static const size_t ICON_ROWS = 2;
static const size_t ICON_COLS = 3;

static int COVER_ICON_WIDTH = 0;
static int COVER_ICON_HEIGHT = 0;
static int COVER_IMAGE_WIDTH = 0;
static int COVER_IMAGE_HEIGHT = 0;
static int COVER_HORIZONTAL_DELTA = 0;
static int COVER_VERTICAL_DELTA = 0;

const char* UIModelView::EventChildSelectChanged = "EventChildSelectChanged";
const char* UIModelView::EventNodesUpdated = "EventNodesUpdated";

void InitCoverMetrics()
{
    COVER_ICON_WIDTH = GetWindowMetrics(UIHomePageCoverViewItemWidthIndex);
    COVER_ICON_HEIGHT = GetWindowMetrics(UIHomePageCoverViewItemHeightIndex);
    COVER_IMAGE_WIDTH = GetWindowMetrics(UICoverViewItemInnerCoverWidthIndex);
    COVER_IMAGE_HEIGHT = GetWindowMetrics(UICoverViewItemInnerCoverHeightIndex);
    COVER_HORIZONTAL_DELTA = GetWindowMetrics(UIHomePageCoverViewHorzIntervalIndex);
    COVER_VERTICAL_DELTA = GetWindowMetrics(UIHomePageCoverViewVertIntervalIndex);
}

UIModelView::UIModelView(BookListUsage usage, ModelTree * model_tree)
    : UICompoundListBox(NULL, IDSTATIC)
    , m_bIsDisposed(false)
    , items_per_page_(9)
    , current_page_(0)
    , page_count_(0)
    , item_count_(0)
    , model_tree_(model_tree)
    , m_needClearItem(true)
{
    if (model_tree != 0)
    {
        view_ctx_.sort_field_ = model_tree->sortField();
    }
    view_ctx_.book_usage_ = usage;
    InitCoverMetrics();
    BookCoverLoader::GetInstance()->SetMinimumCoverSize(COVER_IMAGE_WIDTH, COVER_IMAGE_HEIGHT);

    m_iconSizer = new UIBoxSizer(dkVERTICAL);
    m_iconSizer->SetName("IconSizer");

    m_listSizer = new UIBoxSizer(dkVERTICAL);
    m_listSizer->SetName("ListSizer");
    
    m_windowSizer->Add(m_listSizer, UISizerFlags(1).Expand());
    m_windowSizer->SetName("UIModelViewMainSizer");
    
    m_windowSizer->Add(m_iconSizer, UISizerFlags(1).Expand());
    m_windowSizer->Hide(m_iconSizer);

    ContainerNode* root_node = model_tree_->root();
    SubscribeMessageEvent(Node::EventChildrenIsReady,
        *root_node,
        std::tr1::bind(
            std::tr1::mem_fn(&UIModelView::onChildrenNodesReady),
            this,
            std::tr1::placeholders::_1));
    SubscribeMessageEvent(ModelTree::EventCurrentNodeChanged,
        *model_tree_,
        std::tr1::bind(
            std::tr1::mem_fn(&UIModelView::onCurrentNodeChanged),
            this,
            std::tr1::placeholders::_1));
}


void UIModelView::ClearListItem()
{
    ClearObjectInVector(node_views_);
    if (m_windowSizer)
    {
        m_iconSizer->Clear();
        m_listSizer->Clear();
        m_rowSizers.clear();
    }
}

UIModelView::~UIModelView()
{
    if (m_windowSizer)
    {
        // delete windowSizer before delete obj in m_btns
        // otherwise windowSizer will use wild pointer to obj in m_btns
        delete m_windowSizer;
        m_windowSizer = 0;
    }
    ClearListItem();
    item_count_ = 0;
}

IUINodeView* UIModelView::selectedNodeView()
{
	if (m_iSelectedItem < 0 || m_iSelectedItem >= m_iVisibleItemNum)
	{
		return NULL;
	}
	return node_views_.at(m_iSelectedItem);
}

IUINodeView* UIModelView::nodeView(const string& node_path, int& index)
{
    for (size_t i = 0; i < node_views_.size(); i++)
    {
        IUINodeView* node_view = node_views_.at(i);
        if (node_view != 0 && node_view->data()->absolutePath() == node_path)
        {
            index = i;
            return node_view;
        }
    }
    return 0;
}

BOOL UIModelView::OnKeyPress(INT32 iKeyCode)
{
    return UICompoundListBox::OnKeyPress(iKeyCode);
}

bool UIModelView::onChildrenNodesReady(const EventArgs& args)
{
    const NodeChildenReadyArgs& children_args = dynamic_cast<const NodeChildenReadyArgs&>(args);
    ContainerNode* current_node = model_tree_->currentNode();
    if (current_node == 0 || children_args.current_node_path != current_node->absolutePath())
    {
        // ignore
        return true;
    }

    // get children without rescanning.
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    const int itemsPerPage = GetItemsPerPage();
    NodePtrs children = childrenNodes(false, false, result);
    if (result == RETRIEVE_DONE)
    {
        item_count_ = children.size();
        page_count_ = (item_count_ + itemsPerPage - 1) / itemsPerPage;
        if(page_count_ == 0)
        {
            page_count_ = 1;
        }
        current_page_ = (current_page_ >= (int)page_count_) ? ((int)page_count_) - 1 : current_page_;

        // Notify parent UI
        NodesUpdatedArgs nodes_update_args;
        nodes_update_args.children_num = children.size();
        FireEvent(EventNodesUpdated, nodes_update_args);

        UpdateListItem();
        Layout();
    }
    return true;
}

bool UIModelView::onCurrentNodeChanged(const EventArgs& args)
{
    // TODO. More checking is needed?
    if (!IsDisplay())
    {
        return false;
    }
    //initNodeViews(!view_ctx_.isSelectMode());
    initNodeViews(false);
    return true;
}

bool UIModelView::onNodeSelected(const EventArgs& args)
{
    ChildSelectChangedArgs children_selected_args(GetSelectedBookCount()); 
    FireEvent(EventChildSelectChanged, children_selected_args);

    for (size_t i = 0; i < node_views_.size(); i++)
    {
        IUINodeView* node_view = node_views_.at(i);
        if (node_view != 0 && node_view->IsDisplay())
        {
            node_view->updateSelectedStatus();
        }
    }

    return true;
}

bool UIModelView::onNodeOpenBook(const EventArgs& args)
{
    const NodeOpenBookArgs& open_book_args = dynamic_cast<const NodeOpenBookArgs&>(args);
    int index = 0;
    IUINodeView* node_view = nodeView(open_book_args.node_path, index);
    if (node_view != 0)
    {
        NodePtr node = node_view->data();
        int file_id = -1;
        if (node->type() == NODE_TYPE_FILE_LOCAL_DOCUMENT ||
            node->type() == NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK)
        {
            FileNode* file_node = dynamic_cast<FileNode*>(node.get());
            file_id = file_node->fileInfo()->GetFileID();
        }
        else if (node->type() == NODE_TYPE_MICLOUD_BOOK)
        {
            CloudFileNode* cloud_file_node = dynamic_cast<CloudFileNode*>(node.get());
            file_id = cloud_file_node->localFileInfo()->GetFileID();
        }
        if (file_id >= 0)
        {
            BookOpenManager::GetInstance()->Initialize(GetParent(), file_id, 10, m_iTop + index * GetItemHeight() + 10);
            return true;
        }
    }
    return false;
}

bool UIModelView::OnItemClick(INT32 iSelectedItem)
{
    DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (iSelectedItem < 0 || iSelectedItem >= m_iVisibleItemNum)
    {
        DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s iSelectedItem out of range", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    UICompoundListBox::OnItemClick(iSelectedItem);
    IUINodeView* node_view = selectedNodeView();
    if (node_view != 0)
    {
        node_view->handleClicked();
    }
    return true;
}

BOOL UIModelView::SetItemHeight(INT32 _iItemHeight)
{
    if (UICompoundListBox::SetItemHeight(_iItemHeight))
    {
        m_needClearItem = true;
        return true;
    }
    return false;
}

int UIModelView::GetItemsPerPage() const
{
    switch (view_ctx_.layout_mode_)
    {
        case BLM_LIST:
            return items_per_page_;
        case BLM_ICON:
        default:
            return ICON_COLS * ICON_ROWS;
    }
}

void UIModelView::initNodeViews(bool rescan)
{
    DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    m_needClearItem = true;
    const int itemsPerPage = GetItemsPerPage();

    // Rescan the children from local filesystem or cloud
    RetrieveChildrenResult result = RETRIEVE_FAILED;

    // Update the children but do not rescan. Make sure the children will only be scanned once if they are dirty.
    NodePtrs children = childrenNodes(rescan, false, result);
    if (result == RETRIEVE_DONE)
    {
        item_count_ = children.size();
        page_count_ = (itemsPerPage > 0) ? ((item_count_ + itemsPerPage - 1) / itemsPerPage) : 1;
        if(page_count_ == 0)
        {
            page_count_ = 1;
        }
        current_page_ = (current_page_ >= (int)page_count_) ? ((int)page_count_) - 1 : current_page_;

        // Notify parent UI
        NodesUpdatedArgs nodes_update_args;
        nodes_update_args.children_num = children.size();
        FireEvent(EventNodesUpdated, nodes_update_args);

        UpdateListItem();
        Layout();
    }
}

bool UIModelView::BackToUpperFolder()
{
    return gotoUp();
}

NodeType UIModelView::currentNodeType()
{
    ContainerNode* current_node = model_tree_->currentNode();
    return current_node->type();
}

string UIModelView::currentNodePath()
{
    ContainerNode* current_node = model_tree_->currentNode();
    return current_node->absolutePath();
}

bool UIModelView::UpdateListItem()
{
    //m_needClearItem = (rootNodeDisplayMode() != BY_SORT);
    if (m_needClearItem)
    {
        ClearListItem();
    }

    switch (view_ctx_.layout_mode_)
    {
        case BLM_LIST:
            return UpdateListItemForList();
        case BLM_ICON:
            return UpdateListItemForIcon();
            break;
        default:
            return false;
    }
}

bool UIModelView::UpdateListItemForIcon()
{
    int iconWidth = COVER_ICON_WIDTH; //167;//130; //167; //GetWindowMetrics(UIDirectoryListIconWidthIndex);
    int iconHeight = COVER_ICON_HEIGHT; //226;//178;//226;//GetWindowMetrics(UIDirectoryListIconHeightIndex);
    int horzDelta = COVER_HORIZONTAL_DELTA;
    int vertDelta = COVER_VERTICAL_DELTA;
    int horzSpacing = horzDelta - iconWidth;
    int vertSpacing = vertDelta - iconHeight;
    DebugPrintf(DLC_GUI_VERBOSE, "IUINodeView::UpdateListItemForIcon, %d, %d",
                horzSpacing, vertSpacing);

    int viewItemIndex = 0;
    const int itemsPerPage = GetItemsPerPage();
    int startIndex = current_page_ * itemsPerPage;
    int endIndex = std::min(startIndex + itemsPerPage, item_count_);

    IUINodeView* node_view = 0;
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs children = childrenNodes(false, false, result);
    for (int row = 0; row < (int)ICON_ROWS; ++row)
    {
        UISizer* rowSizer = NULL;
        if (row >= (int)m_rowSizers.size())
        {
            m_rowSizers.push_back(new UIBoxSizer(dkHORIZONTAL));
            rowSizer = m_rowSizers.back();
            char buf[10];
            snprintf(buf, DK_DIM(buf), "row %d", row);
            rowSizer->SetName(buf);
            m_iconSizer->Add(rowSizer);
            m_iconSizer->AddSpacer(vertSpacing);
        }
        else
        {
            rowSizer = m_rowSizers[row];
        }
        rowSizer->Clear();
        rowSizer->AddSpacer(GetWindowMetrics(UIHomePageCoverViewItemLeftIndex));
        
        for (size_t col = 0; col < ICON_COLS && startIndex < endIndex; ++col, ++viewItemIndex)
        {
            if (startIndex < (int)children.size() && children[startIndex] != 0)
            {
                if (viewItemIndex == (int)node_views_.size())
                {
                    node_view = IUINodeView::createNodeView(this,
                        model_tree_,
                        children[startIndex],
                        view_ctx_.book_usage_,
                        COVER_ICON_WIDTH,
                        COVER_ICON_HEIGHT);
                    node_views_.push_back(node_view);
                }
                else if (viewItemIndex < (int)node_views_.size())
                {
                    node_view = node_views_[viewItemIndex];
                }
                else
                {
                    continue;
                }
                node_view->SetMinSize(iconWidth, iconHeight);
                node_view->SetVisible(true);
                node_view->SetModelDisplayMode(view_ctx_.layout_mode_);
                node_view->SetNode(children[startIndex]);

                rowSizer->Add(node_view);
                rowSizer->AddSpacer(horzSpacing);
            }
            startIndex++;
        }
    }

    m_iVisibleItemNum = viewItemIndex; // in CURRENT PAGE, visiable item number;

    DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s items_per_page_ = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, items_per_page_);
    //disable rest of the UIITEM;
    int iUIItemNum = node_views_.size();
    for (int i = m_iVisibleItemNum; i < iUIItemNum; i++)
    {
        node_view = node_views_.at(i);
        if (node_view != 0)
            node_view->SetVisible(false);
    }
    return true;
}

bool UIModelView::UpdateListItemForList()
{
    size_t iIndex = 0;
    const int itemsPerPage = GetItemsPerPage();
    const int iLeftMargin = GetLeftMargin();
    const int iHeight = GetItemHeight();
    const int iWidth  = GetItemWidth() - (iLeftMargin << 1);

    IUINodeView* node_view = 0;
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs children = childrenNodes(false, false, result);
    if (result != RETRIEVE_FAILED)
    {
        item_count_ = children.size();
        page_count_ = (itemsPerPage > 0) ? ((item_count_ + itemsPerPage - 1) / itemsPerPage) : 1;
        if(page_count_ == 0)
        {
            page_count_ = 1;
        }
        current_page_ = (current_page_ >= (int)page_count_) ? ((int)page_count_) - 1 : current_page_;
    }

    int startIndex = current_page_ * itemsPerPage;
    int endIndex = std::min(startIndex + itemsPerPage, item_count_);

    for (int i = startIndex; i < endIndex; i++)
    {
        if (i >= children.size())
        {
            break;
        }
        if (0 == children[i])
        {
            continue;
        }

        NodePtr child = children[i];
        if (iIndex == node_views_.size())
        {
            node_view = IUINodeView::createNodeView(this,
                model_tree_,
                child,
                view_ctx_.book_usage_,
                COVER_ICON_WIDTH,
                COVER_ICON_HEIGHT);
            if(node_view != 0)
            {
                //pItem->MoveWindow(iLeftMargin, iIndex * (iHeight + m_iItemSpacing), iWidth, iHeight);
                node_views_.push_back(node_view);
            }
            else
            {
                break;
            }
        }
        else if (iIndex < node_views_.size())
        {
            node_view = node_views_[iIndex];
        }
        else
        {
            continue;
        }
        if (m_listSizer && m_listSizer->GetItem(node_view) == NULL)
        {
            m_listSizer->Add(node_view, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, iLeftMargin));
        }
        node_view->SetMinSize(dkSize(iWidth, iHeight));
        node_view->SetModelDisplayMode(view_ctx_.layout_mode_);
        node_view->SetNode(child);
        node_view->SetVisible(TRUE);
        iIndex++;
    }

    m_iVisibleItemNum = iIndex; // in CURRENT PAGE, visiable item number;

    DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s items_per_page_ = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, items_per_page_);
    //disable rest of the UIITEM;
    int iUIItemNum = node_views_.size();
    for (int i = m_iVisibleItemNum; i < iUIItemNum; i++)
    {
        node_view = node_views_.at(i);
        if (node_view)
            node_view->SetVisible(false);
    }
    return true;
}

void UIModelView::updateModelByContext(ModelTree* model_tree)
{
    model_tree->setDisplayMode(view_ctx_.display_mode_);
    model_tree->setSortCriteria(view_ctx_.sort_field_, view_ctx_.sort_order_);
    // TODO. add more restore options
}

void UIModelView::setBookUsage(BookListUsage usage)
{
    if (view_ctx_.book_usage_ != usage)
    {
        view_ctx_.book_usage_ = usage;
        UpdateListItem();
        Layout();
    }
}

bool UIModelView::PageDown()
{
    DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    if(0 == item_count_ )
    {
        //SetBookList(NULL, 0);
        DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s item_count_ is zero", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    current_page_++;
    if (current_page_ < 0)
    {
        current_page_ = page_count_ - 1;
    }
    else if (current_page_ == (int)page_count_)
    {
        current_page_ = 0;
    }

    UpdateListItem();

    Layout();
    Repaint();
    DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

bool UIModelView::PageUp()
{
    DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    if(0 == item_count_ )
    {
        DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s item_count_ is zero", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    current_page_--;
    if (current_page_ < 0)
    {
        current_page_ = page_count_ - 1;
    }
    else if (current_page_ == (int)page_count_)
    {
        current_page_ = 0;
    }

    UpdateListItem();

    Layout();
    Repaint();

    DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

bool UIModelView::DoHandleListTurnPage(bool fKeyUp)
{
    return fKeyUp ? PageUp() : PageDown();
}

bool UIModelView::ShouldShowContextMenuForItem(NodePtr node) const
{
     return (0 != node &&
             node->type() != NODE_TYPE_CATEGORY_LOCAL_BOOK_STORE &&
             node->type() != NODE_TYPE_CATEGORY_LOCAL_PUSHED); // TODO.
}

BOOL UIModelView::HandleLongTap(INT32 selectedItem)
{
    if (view_ctx_.book_usage_ != BLU_BROWSE)
    {
        // in select mode, doesn't support context menu
        return false;
    }
    if (selectedItem < 0 || selectedItem >= m_iVisibleItemNum)
    {
        return false;
    }

    UICompoundListBox::OnItemClick(selectedItem);
    NodePtr node = selectedNode(selectedItem);
    if (ShouldShowContextMenuForItem(node))
    {
    	IUINodeView* node_view = selectedNodeView();
		if (node_view)
        {
            node_view->SetHighLight(true);
	        m_iSelectedItem = selectedItem;
	          
            NodePtr node = node_view->data();
            CDisplay* pDisplay = CDisplay::GetDisplay();
            std::vector<int> command_ids;
            std::vector<int> str_ids;
            if (node->supportedCommands(command_ids, str_ids))
            {
		        UIBookMenuDlg dlgMenu(this, command_ids, str_ids);
		        int dlgWidth = dlgMenu.GetWidth();
		        int dlgHeight = dlgMenu.GetHeight();
		        dlgMenu.MoveWindow((pDisplay->GetScreenWidth() - dlgWidth)/2,
                                   (pDisplay->GetScreenHeight() - dlgHeight)/2 - m_iTop,
                                   dlgWidth,
                                   dlgHeight);
                if (dlgMenu.DoModal() == IDOK)
                {
                    node_view->execute(dlgMenu.GetCommandId());
                }
		    }
            node_view->SetHighLight(false);
        }
    }
    return true;
}

void UIModelView::OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam)
{
    IUINodeView* node_view = selectedNodeView();
    if (node_view != 0)
    {
        node_view->execute(_dwCmdId);
    }
}

void UIModelView::SetModelDisplayMode(ModelDisplayMode layout_mode)
{
    view_ctx_.layout_mode_ = layout_mode;
    if (BLM_LIST == view_ctx_.layout_mode_)
    {
        m_windowSizer->Hide(m_iconSizer);
        m_windowSizer->Show(m_listSizer);
    }
    else
    {
        m_windowSizer->Show(m_iconSizer);
        m_windowSizer->Hide(m_listSizer);
    }
}

size_t UIModelView::GetSelectedBookCount()
{
    size_t selected = 0;
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs children = childrenNodes(false, false, result);
    if (result == RETRIEVE_DONE)
    {
        for (DK_AUTO(cur, children.begin()); cur != children.end(); ++cur)
        {
            if ((*cur)->selected())
            {
                ++selected;
            }
        }
    }
    return selected;
}

std::vector<std::string> UIModelView::GetSelectedBookIds()
{
    std::vector<std::string> results;
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs children = childrenNodes(false, false, result);
    if (result == RETRIEVE_DONE)
    {
        for (DK_AUTO(cur, children.begin()); cur != children.end(); ++cur)
        {
            if ((*cur)->selected())
            {
                results.push_back(
                        PathManager::GetFileNameWithoutExt((*cur)->name().c_str()));
            }
        }
    }
    return results;
}

// added functions
NodePtr UIModelView::firstSelectedNode()
{
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs ref = childrenNodes(false, false, result);
    if (result == RETRIEVE_DONE &&
        m_iSelectedItem >= 0 &&
        m_iSelectedItem < static_cast<int>(ref.size()))
    {
        return ref[m_iSelectedItem];
    }
    return NodePtr();
}

NodePtr UIModelView::selectedNode(const int selected_index)
{
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs ref = childrenNodes(false, false, result);
    if (result == RETRIEVE_DONE &&
        selected_index >= 0 &&
        selected_index < static_cast<int>(ref.size()))
    {
        return ref[selected_index];
    }
    return NodePtr();
}

NodePtrs UIModelView::childrenNodes(bool scan, bool update, RetrieveChildrenResult& result)
{
    if (update && !model_tree_->currentNode()->isDirty())
    {
        result = RETRIEVE_DONE;
        return model_tree_->currentNode()->updateChildrenInfo();
    }
    return model_tree_->currentNode()->children(result, scan, view_ctx_.status_filter_, view_ctx_.keyword_);
}

bool UIModelView::cdPath(const string& path)
{
    return model_tree_->cdPath(path);
    // todo. layout?
}

bool UIModelView::cdRoot()
{
    return model_tree_->cdRootNode() != 0;
}

DKDisplayMode UIModelView::rootNodeDisplayMode()
{
    return view_ctx_.display_mode_;
}

void UIModelView::setRootNodeDisplayMode(DKDisplayMode mode)
{
    view_ctx_.display_mode_ = mode;
    model_tree_->setDisplayMode(mode);
}

void UIModelView::setStatusFilter(int status_filter)
{
    view_ctx_.status_filter_ = status_filter;
}

Field UIModelView::sortField()
{
    return view_ctx_.sort_field_;
}

void UIModelView::sort(Field by, SortOrder order, int status_filter)
{
    setStatusFilter(status_filter);
    view_ctx_.sort_field_ = by;
    view_ctx_.sort_order_ = order;
    model_tree_->setSortCriteria(by, order);
    //model_tree_->sort();

    // get children without rescanning.
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs children = childrenNodes(false, false, result);
    if (result == RETRIEVE_DONE)
    {
        const int itemsPerPage = GetItemsPerPage();
        item_count_ = children.size();
        page_count_ = (item_count_ + itemsPerPage - 1) / itemsPerPage;
        if(page_count_ == 0)
        {
            page_count_ = 1;
        }
        current_page_ = 0;

        // Notify parent UI
        NodesUpdatedArgs nodes_update_args;
        nodes_update_args.children_num = children.size();
        FireEvent(EventNodesUpdated, nodes_update_args);

        UpdateListItem();
        Layout();
    }
}

void UIModelView::setSearchKeyword(const string& keyword)
{
    view_ctx_.keyword_ = keyword;
}

bool UIModelView::gotoNode(NodePtr node)
{
    ContainerNode* goto_node = model_tree_->cd(node);
    if (goto_node != 0)
    {
        //initNodeViews(!view_ctx_.isSelectMode());
        initNodeViews(false);
        return true;
    }
    return false;
}

bool UIModelView::gotoUp()
{
    // Before we goto up, we need to check the position.
    if (model_tree_->canGoUp())
    {
        string name;
        ContainerNode * this_node = model_tree_->currentNode();
        if (this_node != 0)
        {
            name = this_node->name();
        }
        ContainerNode* parent = model_tree_->cdUp();
        if (parent != 0)
        {
            m_iSelectedItem = model_tree_->currentNode()->nodePosition(name);

            // Refresh children
            //initNodeViews(!view_ctx_.isSelectMode());
            initNodeViews(false);
            return true;
        }
    }
    return false;
}

void UIModelView::InitListItem()
{
    initNodeViews(true);
}
