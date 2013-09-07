////////////////////////////////////////////////////////////////////////
// UINodeViewOfLocalDir.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////
#include "CommonUI/UINodeViewOfLocalDir.h"
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
#include "Model/local_folder_node.h"
#include "Model/local_bookstore_node.h"

using namespace WindowsMetrics;
using namespace dk::utility;
using namespace dk::document_model;

UINodeViewOfLocalDir::UINodeViewOfLocalDir(
    UICompoundListBox* pParent,
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

void UINodeViewOfLocalDir::updateByNode()
{
    ContainerNode* file_node = NULL;
    if (m_node)
    {
        switch (m_node->type())
        {
        case NODE_TYPE_CATEGORY_LOCAL_BOOK_STORE:
            file_node = dynamic_cast<LocalBookStoreNode*>(m_node.get());
            break;
        default:
            file_node = dynamic_cast<LocalFolderNode*>(m_node.get());
            break;
        }
        
    }
    if (file_node)
    {
        m_txtItemNameLabel.SetText(file_node->displayName());
        m_imgItem.SetImage(file_node->getInitialImage());
        m_imgItem.SetMinSize(dkSize(GetWindowMetrics(UIHomePageImageMaxWidthIndex), m_imgItem.GetMinHeight()));
        m_cover.SetIsDir(true);
        m_cover.SetDirNameOrBookName(file_node->displayName());
        RetrieveChildrenResult ret = RETRIEVE_FAILED;
        const NodePtrs& nodes  = file_node->children(ret, false, file_node->statusFilter());
        char temp[32] = {0};
        snprintf(temp, DK_DIM(temp), StringManager::GetPCSTRById(BOOKSTORE_BOOK_COUNT), nodes.size());
        m_cover.SetDirBookNumOrBookAuthor(temp);
        m_cover.SetCoverImage(m_node->coverPath());
    }
    IUINodeView::updateByNode();
}

UISizer* UINodeViewOfLocalDir::CreateListViewSizer()
{
    if (NULL == m_listViewSizer)
    {
        m_listViewSizer = new UIBoxSizer(dkVERTICAL);
        if (m_listViewSizer)
        {
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

                m_listViewSizer->Add(horzSizer, UISizerFlags().Expand());
            }
            AppendChild(&m_readingProgressBar);

            m_listViewSizer->Add(&m_readingProgressBar, UISizerFlags().Border(dkLEFT, GetWindowMetrics(UIHomePageImageMaxWidthIndex)).Expand());
            m_listViewSizer->AddStretchSpacer();
        }
    }
    return m_listViewSizer;
}

void UINodeViewOfLocalDir::SetModelDisplayMode(ModelDisplayMode mode)
{
    IUINodeView::SetModelDisplayMode(mode);
    m_imgDirArrow.SetVisible((mode == BLM_LIST) && !m_selectMode);
}


