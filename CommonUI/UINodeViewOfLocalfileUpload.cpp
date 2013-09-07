////////////////////////////////////////////////////////////////////////
// UINodeViewOfCloudDownload.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "CommonUI/UINodeViewOfLocalfileUpload.h"
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

using namespace WindowsMetrics;
using namespace dk::utility;
using namespace dk::document_model;

UINodeViewOfLocalfileUpload::UINodeViewOfLocalfileUpload(
        UICompoundListBox* pParent,
        ModelTree* model_tree,
        BookListUsage usage,
        int iconWidth,
        int iconHeight)
    : IUINodeView(pParent, model_tree, usage, iconWidth, iconHeight)
{
    m_selectMode = true;
}

UINodeViewOfLocalfileUpload::~UINodeViewOfLocalfileUpload()
{
}

void UINodeViewOfLocalfileUpload::updateByNode()
{
    DebugPrintf(DLC_UINodeView, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    FileNode* file_node = dynamic_cast<FileNode*>(m_node.get());
    if (file_node == 0)
    {
        return;
    }
    file_node->sync();

    string item_name = file_node->name();
    string item_display_name = file_node->displayName();
    string item_absolute_path = file_node->absolutePath();

    m_cover.SetBookPath(item_absolute_path);
    m_cover.SetBookFormat(file_node->fileFormat());
    m_cover.SetIsDuoKanBook(file_node->isDuokanBook());
    m_cover.SetDirNameOrBookName(file_node->displayName());
    m_cover.SetDirBookNumOrBookAuthor(file_node->artist());
    m_cover.SetIsTrial(file_node->isTrialBook());
    m_cover.SetSelected(IsSelected());
    m_cover.SetCoverImage(file_node->coverPath());
    
    m_txtSizeLabel.SetMinWidth(dkDefaultCoord);
    m_txtItemNameLabel.SetMinWidth(dkDefaultCoord);

    m_imgItem.SetImage(m_node->getInitialImage());
    m_imgItem.SetMinSize(dkSize(GetWindowMetrics(UIHomePageImageMaxWidthIndex), m_imgItem.GetMinHeight()));

    char szTmp[64] = {0};
    size_t size = file_node->fileSize();
    if (size >= 1048576)
    {
        snprintf(szTmp, DK_DIM(szTmp), "%s %.2f MB",StringManager::GetPCSTRById(BOOK_SIZE), size / 1048576.0);
    }
    else
    {
        snprintf(szTmp, DK_DIM(szTmp), "%s %.2f KB", StringManager::GetPCSTRById(BOOK_SIZE), size / 1024.0);
    }
    m_txtSizeLabel.SetText(szTmp);
    m_txtItemNameLabel.SetText(m_node->displayName());
    if (m_windowSizer)
    {
        dkSize realMin = m_windowSizer->GetMinSize();
        UISizerItem* fileNameItem = m_windowSizer->GetItem(&m_txtItemNameLabel, true);
        UISizerItem* authorItem = m_windowSizer->GetItem(&m_txtSizeLabel, true);
        if (fileNameItem && authorItem)
        {
            fileNameItem->SetProportion(0);
            authorItem->SetProportion(0);
            DebugPrintf(DLC_GUI_VERBOSE, "UINodeViewOfLocalfileUpload::InitItem(%s), %d~%d, %d",
                item_display_name.c_str(), realMin.x, m_minWidth, m_iWidth);
                    
            if (realMin.x > m_minWidth)
            {
                int minAuthorWidth = GetWindowMetrics(UIHomePageAuthorMinWidthIndex);
                int minNameWidth = GetWindowMetrics(UIHomePageFileNameMinWidthIndex);
                if (m_txtItemNameLabel.GetTextWidth() < minNameWidth)
                {
                    authorItem->SetProportion(1);
                }
                else if (m_txtSizeLabel.GetTextWidth() < minAuthorWidth)
                {
                    fileNameItem->SetProportion(1);
                }
                else
                {
                    m_txtSizeLabel.SetMinWidth(minAuthorWidth);
                    fileNameItem->SetProportion(1);
                }
            }
            else
            {
                fileNameItem->SetProportion(0);
                authorItem->SetProportion(1);
            }
        }
    }
    IUINodeView::updateByNode();
}

UISizer* UINodeViewOfLocalfileUpload::CreateListViewSizer()
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
                SetShowBottomLine(false);
                m_imgItem.SetAutoSize(true);
                m_txtSizeLabel.SetFontSize(GetWindowFontSize(UIBookListItemAuthorIndex));
                m_txtSizeLabel.SetForeGroundColor(ColorManager::knBlack);
                m_txtSizeLabel.SetFontSize(GetWindowFontSize(UIBookListItemAuthorIndex));

                AppendChild(&m_imgItem);
                AppendChild(&m_txtItemNameLabel);
                AppendChild(&m_txtSizeLabel);
                AppendChild(&m_imgSelect);
                const int margin = GetWindowMetrics(UIPixelValue10Index);
                horzSizer->Add(&m_imgItem, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
                horzSizer->Add(&m_txtItemNameLabel, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Reduce(1));
                horzSizer->AddStretchSpacer();
                horzSizer->Add(&m_txtSizeLabel, UISizerFlags().Border(dkTOP, GetWindowMetrics(UIHomePageListTipsBorderIndex)).Border(dkLEFT, margin));
                horzSizer->Add(&m_imgSelect, UISizerFlags().Border(dkTOP, GetWindowMetrics(UIHomePageListTipsBorderIndex)).Border(dkLEFT, margin).Align(dkALIGN_RIGHT));
                m_listViewSizer->Add(horzSizer, UISizerFlags().Expand());
            }

            AppendChild(&m_readingProgressBar);

            m_listViewSizer->Add(&m_readingProgressBar, UISizerFlags().Border(dkLEFT, GetWindowMetrics(UIHomePageImageMaxWidthIndex)).Expand());
            m_listViewSizer->AddStretchSpacer();
        }
    }

    return m_listViewSizer;
}

