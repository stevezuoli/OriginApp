////////////////////////////////////////////////////////////////////////
// UINodeViewOfLocalDir.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////
#include "CommonUI/UINodeViewOfLocalDir.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIAddCategoryDlg.h"

#include "TouchAppUI/UIAddBookToCategoryPage.h"
#include "TouchAppUI/UIDeleteCategoryConfirmDlg.h"
#include "DkSPtr.h"
#include "Public/Base/TPDef.h"
#include "I18n/StringManager.h"
#include "Common/FileManager_DK.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "GUI/UIMessageBox.h"

#include "Common/WindowsMetrics.h"
#include "Common/BookCoverLoader.h"
#include "Utility/FileSystem.h"
#include "Utility/PathManager.h"
#include "Utility/StringUtil.h"
#include "Utility/RenderUtil.h"
#include "drivers/DeviceInfo.h"
#include <tr1/functional>

#include "Model/container_node.h"
#include "Model/local_folder_node.h"
#include "Model/local_bookstore_category_node.h"

using namespace WindowsMetrics;
using namespace dk::utility;
using namespace dk::document_model;

UINodeViewOfLocalDir::UINodeViewOfLocalDir(
    UIModelView* pParent,
    ModelTree* model_tree, 
    BookListUsage usage,
    int iconWidth,
    int iconHeight)
    : IUINodeView(pParent, model_tree, usage, iconWidth, iconHeight)
{
}

UINodeViewOfLocalDir::~UINodeViewOfLocalDir()
{
}

void UINodeViewOfLocalDir::update()
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

UISizer* UINodeViewOfLocalDir::getListViewSizer()
{
    if (NULL == m_listViewSizer)
    {
        m_listViewSizer = new UIBoxSizer(dkVERTICAL);
        if (m_listViewSizer)
        {
            m_listViewSizer->AddStretchSpacer();

            const int horizontalMargin = GetWindowMetrics(UIPixelValue30Index);
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

void UINodeViewOfLocalDir::SetModelDisplayMode(ModelDisplayMode mode)
{
    IUINodeView::SetModelDisplayMode(mode);
    const bool isList = (m_modelDisplayMode == BLM_LIST);
    const bool selectMode = isSelectMode();
    m_imgDirArrow.SetVisible(isList && !selectMode);
    m_readingProgressBar.SetVisible(isList && !selectMode);
    ModelTree* model_tree = ModelTree::getModelTree(MODEL_LOCAL_FILESYSTEM);
    bool selectExpand = selectMode && model_tree && m_node && (m_node->mutableParent() == model_tree->currentNode()) && m_usage == BLU_SELECT_EXPAND;
    m_imgItem.SetVisible(isList && !selectExpand);
    m_txtItemNameLabel.SetFontSize(GetWindowFontSize(isList && selectExpand ? FontSize28Index : UIBookListItemFileNameIndex));
}

bool UINodeViewOfLocalDir::isSelectMode() const
{
    return m_cover.usage() == BLU_SELECT_EXPAND;
}

void UINodeViewOfLocalDir::execute(unsigned long command_id)
{
    switch(command_id)
    {
    case ID_BTN_OPEN_FOLDER:
        handleClicked();
        break;
    case ID_BTN_RENAME_FOLDER:
        renameFolder();
        break;
    case ID_BTN_DELETE_FOLDER:
    case ID_BTN_DELETE:
        deleteFolder();
        break;
    case ID_BTN_ADD_FILES_TO_CATEGORY:
        addFilesToCategory();
        break;
    case ID_BTN_RENAME_CATEGORY:
        renameCategory();
        break;
    case ID_BTN_DELETE_CATEGORY:
        deleteCategory();
        break;
    default:
        break;
    }
}

void UINodeViewOfLocalDir::handleClicked()
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

bool UINodeViewOfLocalDir::onStatusUpdatedAction(const EventArgs& args)
{
    // TODO. Implement me
    return false;
}

bool UINodeViewOfLocalDir::onLoadingFinishedAction(const EventArgs& args)
{
    // TODO. Implement Me
    return false;
}

bool UINodeViewOfLocalDir::renameFolder()
{
    UIAddCategoryDlg dlg(m_pParent, UIAddCategoryDlg::CAT_RENAME); // parent is model view
    if (IDOK == dlg.DoModal())
    {
        //string sPath(pSelectedItem->GetItemPath());
        string path = m_node->absolutePath();
        if (!path.empty())
        {
            //sPath.append("/");
            //sPath.append(pSelectedItem->GetItemName());
            std::string newName = dlg.GetInputText();
            std::string errorMessage;
            if (!m_node->rename(newName, errorMessage))
            {
                UIMessageBox msgBox(m_pParent,
                    errorMessage.c_str(),
                    MB_OK);
                msgBox.DoModal();
                return false;
            }
            return true;
        }
    }
    return false;
}

bool UINodeViewOfLocalDir::renameCategory()
{
    LocalBookStoreCategoryNode* cat_node = dynamic_cast<LocalBookStoreCategoryNode*>(m_node.get());
    if (cat_node == 0)
    {
        return false;
    }
    
    UIAddCategoryDlg dlg(m_pParent, UIAddCategoryDlg::CAT_RENAME);
    if (IDOK == dlg.DoModal())
    {
        std::string newName = dlg.GetInputText();
        std::string errorMessage;
        if (!cat_node->rename(newName, errorMessage))
        {
            UIMessageBox msgBox(m_pParent,
                    errorMessage.c_str(),
                    MB_OK);
            msgBox.DoModal();
            return false;
        }
        return true;
    }
    return false;
}

bool UINodeViewOfLocalDir::deleteFolder()
{
    UIMessageBox messagebox(m_pParent,
        StringManager::GetStringById(SURE_DELETE_DIRECTORY),
        MB_OK | MB_CANCEL);
    if(MB_OK == messagebox.DoModal())
    {
        /*string sPath(pSelectedItem->GetItemPath());
        sPath.append("/");
        sPath.append(pSelectedItem->GetItemName());*/
        return m_node->remove(true, true);
    }
    return false;
}

bool UINodeViewOfLocalDir::deleteCategory()
{
    UIDeleteCategoryConfirmDlg dlg(m_pParent);
    if (IDOK == dlg.DoModal())
    {
        LocalBookStoreCategoryNode* cat_node = dynamic_cast<LocalBookStoreCategoryNode*>(m_node.get());
        if (0 == cat_node)
        {
            return false;
        }
        return cat_node->remove(dlg.ShouldDeleteBooks());
    }
    return false;
}

bool UINodeViewOfLocalDir::addFilesToCategory()
{
    LocalBookStoreCategoryNode* cat_node = dynamic_cast<LocalBookStoreCategoryNode*>(m_node.get());
    if (0 != cat_node)
    {
        // TODO. get model tree in a more proper way / notify model view
        UIAddBookToCategoryPage* addBookPage(new UIAddBookToCategoryPage(cat_node->name().c_str(),
            ModelTree::getModelTree(MODEL_LOCAL_FILESYSTEM)));
        addBookPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
        addBookPage->Layout();
        CPageNavigator::Goto(addBookPage);
        return true;
    }
    return false;
}

bool UINodeViewOfLocalDir::openFolder()
{
    ModelTree* model_tree = ModelTree::getModelTree(MODEL_LOCAL_FILESYSTEM);
    ContainerNode* goto_node = model_tree->cd(m_node);
    return (goto_node != 0);
}

std::vector<int> UINodeViewOfLocalDir::getSupportedNodeTypes()
{
    std::vector<int> support_types;
    support_types.push_back(NODE_TYPE_CATEGORY_LOCAL_BOOK_STORE);
    support_types.push_back(NODE_TYPE_CATEGORY_LOCAL_PUSHED);
    support_types.push_back(NODE_TYPE_CATEGORY_LOCAL_DOCUMENTS);
    support_types.push_back(NODE_TYPE_CATEGORY_LOCAL_FOLDER);
    support_types.push_back(NODE_TYPE_CATEGORY_VIRTUAL_BOOK_STORE);
    return support_types;
}

HRESULT UINodeViewOfLocalDir::DrawBackGround(DK_IMAGE drawingImg)
{
    IUINodeView::DrawBackGround(drawingImg);
    HRESULT hr(S_OK);
    ModelTree* model_tree = ModelTree::getModelTree(MODEL_LOCAL_FILESYSTEM);
    if (model_tree)
    {
        CTpGraphics grf(drawingImg);
        RTN_HR_IF_FAILED(grf.EraserBackGround());
        if ((m_node->mutableParent() == model_tree->currentNode()) && m_usage == BLU_SELECT_EXPAND)
        {
            int borderLine = GetWindowMetrics(UIProgressBarBorderIndex);
            StrokeStyle style = SOLID_STROKE;
            RTN_HR_IF_FAILED(grf.DrawLine(0, 0, m_iWidth, borderLine,style));
        }
    }
    return hr;
}

