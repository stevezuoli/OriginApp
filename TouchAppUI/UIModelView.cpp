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
    , items_per_page_(0)
    , current_page_(0)
    , page_count_(0)
    , item_count_(0)
    , display_mode_(BLM_LIST)
    , status_filter_(NODE_NONE)
    , m_usage(usage)
    , model_tree_(model_tree)
    , m_needClearItem(true)
{
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

IUINodeView* UIModelView::GetSelectedUIItem()
{
	if (m_iSelectedItem < 0 || m_iSelectedItem >= m_iVisibleItemNum)
	{
		return NULL;
	}
	return node_views_.at(m_iSelectedItem);
}

BOOL UIModelView::OnKeyPress(INT32 iKeyCode)
{
    return UICompoundListBox::OnKeyPress(iKeyCode);
}

bool UIModelView::OpenBookByNode(NodePtr node, int select_item)
{
    FileNode* file_node = dynamic_cast<FileNode*>(node.get());
    if (file_node == 0)
    {
        return false;
    }

    IUINodeView* node_view = node_views_.at(select_item);
    string filePath = node->absolutePath();
    int file_id = file_node->fileInfo()->GetFileID();
        
    BookOpenManager::GetInstance()->Initialize(GetParent(), file_id, 10, m_iTop + select_item * GetItemHeight() + 10);
    node_view->SetIsLoading(true);
    node_view->UpdateWindow();

    SNativeMessage msg;
    msg.iType = KMessageOpenBook;
    CNativeThread::Send(msg);
    return true;
}

/// Upload single book
bool UIModelView::uploadBookByNode(NodePtr node, int select_item)
{
    FileNode* file_node = dynamic_cast<FileNode*>(node.get());
    if (file_node == 0)
    {
        return false;
    }

    file_node->setSelected(true);
    file_node->upload();
    IUINodeView* node_view = node_views_.at(select_item);
    node_view->SetIsLoading(true);
    node_view->UpdateWindow();
    return true;
}

void UIModelView::OnNodeClicked(NodePtr node, int select_item)
{
    if (NULL == node)
    {
        return ;
    }

    NodeType type = node->type();
    switch (type)
    {
    case NODE_TYPE_CATEGORY_LOCAL_BOOK_STORE:
    case NODE_TYPE_CATEGORY_LOCAL_PUSHED:
    case NODE_TYPE_CATEGORY_LOCAL_FOLDER:
    case NODE_TYPE_CATEGORY_VIRTUAL_BOOK_STORE:
    case NODE_TYPE_MICLOUD_CATEGORY:
        gotoNode(node);
        break;
    case NODE_TYPE_FILE_LOCAL_DOCUMENT:
    case NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK:
        if (m_usage & BLU_SELECT)
        {
            IUINodeView* node_view = node_views_.at(select_item);
            if (node_view != 0 )
            {
                node_view->SetSelected(!node_view->IsSelected());
                node_view->UpdateWindow();
            }
        }
        else
        {
            OpenBookByNode(node, select_item);
        }
        break;
    case NODE_TYPE_MICLOUD_BOOK:
        // TODO. remove this testing code
        {
            CloudFileNode* cloud_node = dynamic_cast<CloudFileNode*>(node.get());
            if (cloud_node->localFileInfo() != 0)
            {
                CloudFileNode* cloud_file_node = dynamic_cast<CloudFileNode*>(node.get());
                if (cloud_file_node == 0)
                {
                    break;;
                }

                UINodeView* node_view = node_views_.at(select_item);
                string filePath = node->absolutePath();
                int file_id = cloud_file_node->localFileInfo()->GetFileID();
        
                BookOpenManager::GetInstance()->Initialize(GetParent(), file_id, 10, m_iTop + select_item * GetItemHeight() + 10);
                node_view->SetIsLoading(true);
                node_view->UpdateWindow();

                SNativeMessage msg;
                msg.iType = KMessageOpenBook;
                CNativeThread::Send(msg);
            }
            else
            {
                node->download();
            }
            //node->remove(true);
        }
        break;
    default:
        break;
    }
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
    NodePtrs& children = childrenNodes(false, false, result);
    if (result == RETRIEVE_DONE)
    {
        item_count_ = children.size();
        page_count_ = (item_count_ + items_per_page_ - 1) / items_per_page_;
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

bool UIModelView::OnItemClick(INT32 iSelectedItem)
{
    DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (iSelectedItem < 0 || iSelectedItem >= m_iVisibleItemNum)
    {
        DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s iSelectedItem out of range", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    IUINodeView* node_view = node_views_.at(iSelectedItem);
    if (node_view != 0 )
    {
        if (BLU_SELECT == m_usage && (rootNodeDisplayMode() != BY_FOLDER))
        {
            node_view->SetSelected(!node_view->IsSelected());
            node_view->UpdateWindow();
            ChildSelectChangedArgs args(GetSelectedBookCount()); 
            FireEvent(EventChildSelectChanged, args);
            return true;
        }
        node_view->SetHighLight(true);
    }

    //CDisplay::GetDisplay()->SetFullRepaint(true);
    //current_page_Stack.push_back(current_page_);
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs& children = childrenNodes(false, false, result);
    if (result == RETRIEVE_DONE)
    {
        int itemNo = current_page_ * items_per_page_+ iSelectedItem;
        NodePtr node = children[itemNo];
        OnNodeClicked(node, iSelectedItem);
    }
    return true;
}

int UIModelView::GetItemPerPageByModelDisplayMode(ModelDisplayMode mode)
{
    switch (mode)
    {
        case BLM_LIST:
            return 9;
        case BLM_ICON:
        default:
            return ICON_COLS * ICON_ROWS;
    }
}

void UIModelView::InitListItem()
{
    DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    m_needClearItem = true;
    items_per_page_ = GetItemPerPageByModelDisplayMode(display_mode_);

    // Rescan the children from local filesystem or cloud
    RetrieveChildrenResult result = RETRIEVE_FAILED;

    // Update the children but do not rescan. Make sure the children will only be scanned once if they are dirty.
    NodePtrs& children = childrenNodes(false, true, result);
    if (result == RETRIEVE_DONE)
    {
        item_count_ = children.size();
        page_count_ = (item_count_ + items_per_page_ - 1) / items_per_page_;
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
    if (m_needClearItem)
    {
        ClearListItem();
    }

    m_needClearItem = (rootNodeDisplayMode() != BY_SORT);
    switch (display_mode_)
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

    size_t viewItemIndex = 0;
    size_t startIndex = current_page_ * items_per_page_;
    size_t endIndex = std::min(static_cast<int>(startIndex) + items_per_page_, item_count_);

    IUINodeView* pItem = 0;
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs& children = childrenNodes(false, false, result);
    for (size_t row = 0; row < ICON_ROWS; ++row)
    {
        UISizer* rowSizer = NULL;
        if (row >= m_rowSizers.size())
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
            if (children[startIndex])
            {
                if (viewItemIndex == node_views_.size())
                {
                    pItem = CreateNodeView(this, model_tree_, children[startIndex], m_usage, COVER_ICON_WIDTH, COVER_ICON_HEIGHT);
                    node_views_.push_back(pItem);
                }
                else if (viewItemIndex < node_views_.size())
                {
                    pItem = node_views_[viewItemIndex];
                }
                else
                {
                    continue;
                }
                pItem->SetMinSize(iconWidth, iconHeight);
                pItem->SetVisible(true);
                pItem->SetSelectMode(m_usage == BLU_CLOUD_UPLOAD);
                pItem->SetModelDisplayMode(display_mode_);
                pItem->SetNode(children[startIndex]);
                pItem->SetSelected(children[startIndex]->selected());
                rowSizer->Add(pItem);
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
        pItem = node_views_.at(i);
        if (pItem)
            pItem->SetVisible(false);
    }
    return true;
}

bool UIModelView::UpdateListItemForList()
{
    size_t iIndex = 0;
    int iNextPageIndex = (current_page_ + 1) == page_count_ ? item_count_ : (current_page_ + 1) * items_per_page_;
    int iLeftMargin = GetLeftMargin();
    int iHeight = GetItemHeight();
    int iWidth  = GetItemWidth() - (iLeftMargin << 1);

    IUINodeView* pItem = 0;
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs& children = childrenNodes(false, false, result);
    if (children.size() <= 0)
    {
        return false;
    }
    
    for (int i = current_page_ * items_per_page_; i < iNextPageIndex; i++)
    {
        if (NULL == children[i])
        {
            continue;
        }
        if (iIndex == node_views_.size())
        {
            pItem = CreateNodeView(this, model_tree_, children[i], m_usage, COVER_ICON_WIDTH, COVER_ICON_HEIGHT);
            if(pItem)
            {
                //pItem->MoveWindow(iLeftMargin, iIndex * (iHeight + m_iItemSpacing), iWidth, iHeight);
                node_views_.push_back(pItem);
            }
            else
            {
                break;
            }
        }
        else if (iIndex < node_views_.size())
        {
            pItem = node_views_[iIndex];
        }
        else
        {
            continue;
        }
        if (m_listSizer && m_listSizer->GetItem(pItem) == NULL)
        {
            m_listSizer->Add(pItem, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, iLeftMargin));
        }
        pItem->SetMinSize(dkSize(iWidth, iHeight));
        pItem->SetSelectMode(m_usage == BLU_CLOUD_UPLOAD);
        pItem->SetModelDisplayMode(display_mode_);
        pItem->SetNode(children[i]);
        pItem->SetSelected(children[i]->selected());
        pItem->SetVisible(TRUE);
        iIndex++;
    }

    m_iVisibleItemNum = iIndex; // in CURRENT PAGE, visiable item number;

    DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s items_per_page_ = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, items_per_page_);
    //disable rest of the UIITEM;
    int iUIItemNum = node_views_.size();
    for (int i = m_iVisibleItemNum; i < iUIItemNum; i++)
    {
        pItem = node_views_.at(i);
        if (pItem)
            pItem->SetVisible(false);
    }
    return true;
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
    if (m_usage != BLU_BROWSE)
    {
        // in select mode, doesn't support context menu
        return false;
    }
    if (selectedItem < 0 || selectedItem >= m_iVisibleItemNum)
    {
        return false;
    }

    NodePtr node = selectedNode(selectedItem);
    bool showMenu = ShouldShowContextMenuForItem(node);;

    if (showMenu)
    {
    	IUINodeView* node_view = node_views_.at(selectedItem);
		if (node_view)
        {
            node_view->SetHighLight(true);
	        m_iSelectedItem = selectedItem;
	        UICompoundListBox::OnItemClick(selectedItem);
	        LongTap();
			node_view->SetHighLight(false);
		}
    }
    return true;
}

bool UIModelView::LongTap()
{
    DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    NodePtr node = firstSelectedNode();
    if (node)
    {
    	CDisplay* pDisplay = CDisplay::GetDisplay();
        UIBookMenuDlg::BookMenuType type = UIBookMenuDlg::BMT_BOOK;
        if(node->type() == NODE_TYPE_CATEGORY_LOCAL_FOLDER)
		{
            type = UIBookMenuDlg::BMT_FOLDER;
		}
        else if (node->type() == NODE_TYPE_CATEGORY_VIRTUAL_BOOK_STORE)
        {
            type = UIBookMenuDlg::BMT_DUOKAN_CATEGORY;
        }
        else
        {
            FileNode* file_node = dynamic_cast<FileNode*>(node.get());
            if (file_node != 0 && file_node->isDuokanBook())
            {
                type = UIBookMenuDlg::BMT_DUOKAN_BOOK;
            }
        }
		UIBookMenuDlg dlgMenu(this, type);
		int dlgWidth = dlgMenu.GetWidth();
		int dlgHeight = dlgMenu.GetHeight();
		dlgMenu.MoveWindow((pDisplay->GetScreenWidth() - dlgWidth)/2, (pDisplay->GetScreenHeight() - dlgHeight)/2 - m_iTop, dlgWidth, dlgHeight);
        if (dlgMenu.DoModal() == IDOK)
        {
            OnCommand(dlgMenu.GetCommandId(), NULL, 0);
        }
    }
    DebugPrintf(DLC_UIModelView, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

void UIModelView::OnRenameFolder()
{
    if (m_pParent)
    {
        NodePtr node = firstSelectedNode();
        if(node != 0)
        {
            UIAddCategoryDlg dlg(this, UIAddCategoryDlg::CAT_RENAME);
            if (IDOK == dlg.DoModal())
            {
                //string sPath(pSelectedItem->GetItemPath());
                string path = node->absolutePath();
                if (!path.empty())
                {
                    //sPath.append("/");
                    //sPath.append(pSelectedItem->GetItemName());
                    std::string newName = dlg.GetInputText();
                    std::string errorMessage;
                    if (!RenameDirectoryOnUI(node, newName, errorMessage))
                    {
                        UIMessageBox msgBox(this,
                            errorMessage.c_str(),
                            MB_OK);
                        msgBox.DoModal();
                    }
                }
            }
        }
    }
}

void UIModelView::OnDelete()
{
    DebugPrintf(DLC_UIBOOKLISTBOX, "%s, %d, %s, %s ID_BTN_DELETE", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_pParent)
    {
        NodePtr node = firstSelectedNode();
        if(node != 0)
        {
            bool is_folder = (node->type() == NODE_TYPE_CATEGORY_VIRTUAL_BOOK_STORE ||
                              node->type() == NODE_TYPE_CATEGORY_LOCAL_FOLDER);
            UIMessageBox messagebox(m_pParent,
                StringManager::GetStringById(is_folder ? SURE_DELETE_DIRECTORY : SURE_DELETE_BOOK),
                MB_OK | MB_CANCEL);
            if(MB_OK == messagebox.DoModal())
            {
                /*string sPath(pSelectedItem->GetItemPath());
                sPath.append("/");
                sPath.append(pSelectedItem->GetItemName());*/
                if(!is_folder)
                {
                    DeleteFileOnUI(node);
                }
                else
                {
                    DeleteDirectoryOnUI(node);
                }
            }
        }
        //Layout();
        //m_pParent->UpdateWindow();
    }
}

void UIModelView::OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam)
{
    DebugPrintf(DLC_UIBOOKLISTBOX, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch(_dwCmdId)
    {
    case ID_BTN_OPEN_FOLDER:
    case ID_BTN_READ_BOOK:
        {
            DebugPrintf(DLC_UIBOOKLISTBOX, "%s, %d, %s, %s ID_BTN_READ_BOOK", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            OnItemClick(m_iSelectedItem);
        }
        break;
    case ID_BTN_RENAME_FOLDER:
        {
            OnRenameFolder();
        }
        break;
    case ID_BTN_DELETE_FOLDER:
    case ID_BTN_DELETE:
        OnDelete();
        break;
    case ID_BTN_SINAWEIBO_SHARE:
        {
            if (m_pParent)
            {
                NodePtr node = firstSelectedNode();
                FileNode* file_node = dynamic_cast<FileNode*>(node.get());
                if(file_node != 0)
                {
                    string displayText = StringManager::GetPCSTRById(ELEGANT_READING_TIME);
                    displayText.append(StringManager::GetPCSTRById(SHAREWEIBO_HINT));
                    char buf[1024] = {0};
                    snprintf(buf, sizeof(buf)/sizeof(char), displayText.c_str(), file_node->name().c_str());
                    UIWeiboSharePage* pPage = new UIWeiboSharePage(buf);
                    if (pPage)
                    {
                        CPageNavigator::Goto(pPage);
                    }
                }
            }
        }
        break;
    case ID_BTN_ADD_FILES_TO_CATEGORY:
        {
            NodePtr node = firstSelectedNode();
            LocalBookStoreCategoryNode* cat_node = dynamic_cast<LocalBookStoreCategoryNode*>(node.get());
            if (0 != cat_node)
            {
                UIAddBookToCategoryPage* addBookPage(new UIAddBookToCategoryPage(cat_node->name().c_str(), model_tree_));
                addBookPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                addBookPage->Layout();
                CPageNavigator::Goto(addBookPage);
            }
        }

        break;
    case ID_BTN_RENAME_CATEGORY:
        {
            OnRenameCategory();
        }
        break;
    case ID_BTN_DELETE_FROM_CATEGORY:
        {
            NodePtr node = firstSelectedNode();
            FileNode* file_node = dynamic_cast<FileNode*>(node.get());
            if (0 != file_node)
            {
                Node* parent_node = file_node->mutableParent();
                LocalBookStoreCategoryNode* cat_node = dynamic_cast<LocalBookStoreCategoryNode*>(parent_node);
                if (cat_node != 0)
                {
                    // TODO. Move it to local filesystem tree
                    LocalCategoryManager::RemoveBookFromCategory(
                            cat_node->name().c_str(),
                            PathManager::GetFileNameWithoutExt(file_node->name().c_str()).c_str());
                    InitListItem();
                }
            }
        }
        break;
    case ID_BTN_DELETE_CATEGORY:
        OnDeleteCategory();
        break;
    default:
        break;
    }
    DebugPrintf(DLC_UIBOOKLISTBOX, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UIModelView::OnRenameCategory()
{
    NodePtr node = firstSelectedNode();
    LocalBookStoreCategoryNode* cat_node = dynamic_cast<LocalBookStoreCategoryNode*>(node.get());
    if (cat_node == 0)
    {
        return;
    }
    
    UIAddCategoryDlg dlg(this, UIAddCategoryDlg::CAT_RENAME);
    if (IDOK == dlg.DoModal())
    {
        std::string newName = dlg.GetInputText();
        std::string errorMessage;
        if (!cat_node->rename(newName, errorMessage))
        {
            UIMessageBox msgBox(this,
                    errorMessage.c_str(),
                    MB_OK);
            msgBox.DoModal();
        }
    }
}

void UIModelView::OnDeleteCategory()
{
    UIDeleteCategoryConfirmDlg dlg(this);
    if (IDOK == dlg.DoModal())
    {
        NodePtr node = firstSelectedNode();
        LocalBookStoreCategoryNode* cat_node = dynamic_cast<LocalBookStoreCategoryNode*>(node.get());
        if (0 == cat_node)
        {
            return;
        }
        cat_node->remove(dlg.ShouldDeleteBooks());
    }
}

void UIModelView::DeleteFileOnUI(NodePtr node)
{
    node->remove(true);
}

bool UIModelView::RenameDirectoryOnUI(NodePtr node, const std::string& newName, std::string& errMessage)
{
    return node->rename(newName, errMessage);
}

void UIModelView::DeleteDirectoryOnUI(NodePtr node)
{
    node->remove(true);
}

void UIModelView::SetModelDisplayMode(ModelDisplayMode ModelDisplayMode)
{
    int startItemIndex = current_page_ * items_per_page_;
    int newItemPerPage = GetItemPerPageByModelDisplayMode(ModelDisplayMode);
    current_page_ = startItemIndex  / newItemPerPage;
    display_mode_ = ModelDisplayMode;
    if (BLM_LIST == display_mode_)
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
    NodePtrs& children = childrenNodes(false, false, result);
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
    NodePtrs& children = childrenNodes(false, false, result);
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
NodePtr UIModelView::firstVisibleNode()
{
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs & ref = childrenNodes(false, false, result);
    if (result == RETRIEVE_DONE &&
        first_visible_ >= 0 &&
        first_visible_ < static_cast<int>(ref.size()))
    {
        return ref[first_visible_];
    }
    return NodePtr();
}

NodePtr UIModelView::firstSelectedNode()
{
    RetrieveChildrenResult result = RETRIEVE_FAILED;
    NodePtrs & ref = childrenNodes(false, false, result);
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
    NodePtrs & ref = childrenNodes(false, false, result);
    if (result == RETRIEVE_DONE &&
        selected_index >= 0 &&
        selected_index < static_cast<int>(ref.size()))
    {
        return ref[selected_index];
    }
    return NodePtr();
}

NodePtrs& UIModelView::childrenNodes(bool scan, bool update, RetrieveChildrenResult& result)
{
    if (update && !model_tree_->currentNode()->isDirty())
    {
        result = RETRIEVE_DONE;
        return model_tree_->currentNode()->updateChildrenInfo();
    }
    return model_tree_->currentNode()->mutableChildren(result, scan, status_filter_);
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
    return model_tree_->displayMode();
}

void UIModelView::setRootNodeDisplayMode(DKDisplayMode mode)
{
    model_tree_->setDisplayMode(mode);
}

void UIModelView::setStatusFilter(int status_filter)
{
    status_filter_ = status_filter;
}

bool UIModelView::gotoNode(NodePtr node)
{
    ContainerNode* goto_node = model_tree_->cd(node);
    if (goto_node != 0)
    {
        InitListItem();
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
            InitListItem();
            return true;
        }
    }
    return false;
}
