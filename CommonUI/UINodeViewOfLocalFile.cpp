////////////////////////////////////////////////////////////////////////
// UINodeViewOfLocalFileWithCover.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "CommonUI/UINodeViewOfLocalFile.h"
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

UINodeViewOfLocalFile::UINodeViewOfLocalFile(
        UICompoundListBox* pParent,
        ModelTree* model_tree,
        BookListUsage usage,
        int iconWidth,
        int iconHeight)
    : IUINodeView(pParent, model_tree, usage, iconWidth, iconHeight)
    , m_spaceBetweenAuthorAndArrow(NULL)
{
}

UINodeViewOfLocalFile::~UINodeViewOfLocalFile()
{
}

void UINodeViewOfLocalFile::updateByNode()
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
    
    m_txtAuthorLabel.SetMinWidth(dkDefaultCoord);
    m_txtItemNameLabel.SetMinWidth(dkDefaultCoord);

    SetShowBottomLine(false);
    m_imgItem.SetAutoSize(true);
    m_imgItem.SetImage(m_node->getInitialImage());
    m_imgItem.SetMinSize(dkSize(GetWindowMetrics(UIHomePageImageMaxWidthIndex), m_imgItem.GetMinHeight()));

    m_txtAuthorLabel.SetText(file_node->artist());
    m_txtAuthorLabel.SetForeGroundColor(ColorManager::knBlack);
    m_txtAuthorLabel.SetFontSize(GetWindowFontSize(UIBookListItemAuthorIndex));

    m_txtItemNameLabel.SetText(file_node->displayName());

    if (m_spaceBetweenAuthorAndArrow)
    {
        m_spaceBetweenAuthorAndArrow->Show(false);
    }

    // TODO.
    m_txtTrialLabel.SetVisible(file_node->isTrialBook());

    bool isNew = (file_node->lastRead() == 0);
    m_imgNewBook.SetVisible(isNew);

    m_cover.SetBookPath(item_absolute_path);
    m_cover.SetBookFormat(file_node->fileFormat());
    m_cover.SetIsDuoKanBook(file_node->isDuokanBook());
    m_cover.SetDirNameOrBookName(file_node->displayName());
    m_cover.SetDirBookNumOrBookAuthor(file_node->artist());
    m_cover.SetIsTrial(file_node->isTrialBook());
    m_cover.SetIsNew(isNew);
    m_cover.SetCoverImage(file_node->coverPath());

    PCDKFile file = file_node->fileInfo();
    int pageCount = file != 0 ? file->GetPageCount() : 0;
    int maxDot = PageCountToProgressBarDotCount(pageCount);
    int progress = file != 0 ? file->GetFilePlayProcess() : 0;
    m_readingProgressBar.SetMaxDot(maxDot);
    m_readingProgressBar.SetPercentageProgess(progress);

    if (m_windowSizer)
    {
        dkSize realMin = m_windowSizer->GetMinSize();
        UISizerItem* fileNameItem = m_windowSizer->GetItem(&m_txtItemNameLabel, true);
        UISizerItem* authorItem = m_windowSizer->GetItem(&m_txtAuthorLabel, true);
        if (fileNameItem && authorItem)
        {
            fileNameItem->SetProportion(0);
            authorItem->SetProportion(0);
            DebugPrintf(DLC_GUI_VERBOSE, "UINodeViewOfLocalFile::InitItem(%s), %d~%d, %d",
                item_display_name.c_str(), realMin.x, m_minWidth, m_iWidth);
                    
            if (realMin.x > m_minWidth)
            {
                int minAuthorWidth = GetWindowMetrics(UIHomePageAuthorMinWidthIndex);
                int minNameWidth = GetWindowMetrics(UIHomePageFileNameMinWidthIndex);
                if (m_txtItemNameLabel.GetTextWidth() < minNameWidth)
                {
                    authorItem->SetProportion(1);
                }
                else if (m_txtAuthorLabel.GetTextWidth() < minAuthorWidth)
                {
                    fileNameItem->SetProportion(1);
                }
                else
                {
                    m_txtAuthorLabel.SetMinWidth(minAuthorWidth);
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

UISizer* UINodeViewOfLocalFile::CreateListViewSizer()
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
                m_txtTrialLabel.SetFontSize(GetWindowFontSize(UIBookListItemTrialIndex));
                m_txtTrialLabel.SetText(StringManager::GetPCSTRById(BOOKSTORE_TRIALVERSION));
                m_txtAuthorLabel.SetFontSize(GetWindowFontSize(UIBookListItemAuthorIndex));

                m_imgNewBook.SetAutoSize(true);
                m_imgNewBook.SetImage(IMAGE_TOUCH_NEW_BOOK);

                AppendChild(&m_imgItem);
                AppendChild(&m_txtItemNameLabel);
                AppendChild(&m_txtAuthorLabel);
                AppendChild(&m_txtTrialLabel);
                AppendChild(&m_imgNewBook);
                AppendChild(&m_imgSelect);

                horzSizer->Add(&m_imgItem, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
                horzSizer->Add(&m_txtItemNameLabel, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Reduce(1));
                horzSizer->Add(&m_txtTrialLabel, UISizerFlags().Border(dkTOP, GetWindowMetrics(UIHomePageListTipsBorderIndex)));
                horzSizer->Add(&m_imgNewBook, UISizerFlags().Border(dkTOP, GetWindowMetrics(UIHomePageListTipsBorderIndex)));

                m_txtAuthorLabel.SetAlign(ALIGN_RIGHT);
                horzSizer->SetMinHeight(GetWindowMetrics(UIHomePageListTextHeightIndex));
                horzSizer->AddStretchSpacer();
                horzSizer->Add(&m_txtAuthorLabel, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, GetWindowMetrics(UIHomePageListAuthorLeftBorderIndex)));
                horzSizer->Add(&m_imgSelect, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, GetWindowMetrics(UIPixelValue10Index)));
                m_spaceBetweenAuthorAndArrow = horzSizer->AddSpacer(10);
                m_listViewSizer->Add(horzSizer, UISizerFlags().Expand());
            }
            AppendChild(&m_readingProgressBar);

            m_listViewSizer->Add(&m_readingProgressBar, UISizerFlags().Border(dkLEFT, GetWindowMetrics(UIHomePageImageMaxWidthIndex)).Expand());
            m_listViewSizer->AddStretchSpacer();
        }
    }
    return m_listViewSizer;
}

int UINodeViewOfLocalFile::PageCountToProgressBarDotCount(int pageCount)
{
    const int DEFAULT_PAGES_DOTS = 10;
    if (pageCount <= 0)
    {
        return DEFAULT_PAGES_DOTS;
    }
    int pageDotsTable[][2] = {
                {500, 10},
                {1000, 15},
                {1500, 20},
                {2000, 25},
                {5000, 35},
                {10000, 50},
                {INT_MAX, 65},
    };
    int i = 0;
    while (pageCount > pageDotsTable[i][0])
    {
        ++i;
    }
    
    return DeviceInfo::IsKPW() ? pageDotsTable[i][1] * 3 /2 : pageDotsTable[i][1];
}

