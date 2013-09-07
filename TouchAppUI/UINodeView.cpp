////////////////////////////////////////////////////////////////////////
// UINodeView.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UINodeView.h"
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

UINodeView::UINodeView(
        UICompoundListBox* pParent,
        ModelTree* model_tree,
        BookListUsage usage,
        int iconWidth,
        int iconHeight)
    : IUINodeView(pParent, model_tree, usage, iconWidth, iconHeight)
    , m_bIsDisposed(FALSE)
    , m_modelDisplayMode(BLM_ICON)
    , m_usage(usage)
    , m_spaceBetweenAuthorAndArrow(NULL)
    , m_cover(this,
              model_tree,
              usage,
              BookCoverLoader::GetInstance()->GetMinimumCoverWidth(),
              BookCoverLoader::GetInstance()->GetMinimumCoverHeight(),
              iconWidth,
              iconHeight)
{
    SetLongClickable(true);
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }
    Init();
}

UINodeView::~UINodeView()
{
}

void UINodeView::SetNode(NodePtr node)
{
    m_node = node;
    updateByNode();
    SubscribeMessageEvent(Node::EventChildrenIsReady,
        *m_node,
        std::tr1::bind(
            std::tr1::mem_fn(&UINodeView::onChildrenReady),
            this,
            std::tr1::placeholders::_1));
    SubscribeMessageEvent(Node::EventNodeStatusUpdated,
        *m_node,
        std::tr1::bind(
        std::tr1::mem_fn(&UINodeView::onStatusUpdated),
            this,
            std::tr1::placeholders::_1));
    SubscribeMessageEvent(Node::EventNodeLoadingFinished,
        *m_node,
        std::tr1::bind(
            std::tr1::mem_fn(&UINodeView::onLoadingFinished),
            this,
            std::tr1::placeholders::_1));
}

BOOL UINodeView::OnKeyPress(INT32 iKeyCode)
{
    return TRUE;
}

void UINodeView::SetFocus(bool bIsFocus)
{
    if (m_bIsFocus != bIsFocus)
    {
        m_bIsFocus = bIsFocus;
    }
}

bool  UINodeView::SetVisible(bool bVisible)
{
    UIContainer::SetVisible(bVisible);
    return bVisible;
}

void UINodeView::SetHighLight(bool isHighLight)
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

void UINodeView::updateAsDirectory()
{
    ContainerNode* container_node = dynamic_cast<ContainerNode*>(m_node.get());
    if (container_node == 0)
    {
        return;
    }
    m_txtItemNameLabel.SetText(container_node->displayName());

    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    const NodePtrs& children = container_node->children(ret, false, container_node->statusFilter());
    int children_num = children.size();

    char buf[20];
    snprintf(buf, DK_DIM(buf), "%d", children_num);
    m_txtAuthorLabel.SetText(buf);

    m_txtAuthorLabel.SetForeGroundColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    m_txtAuthorLabel.SetFontSize(GetWindowFontSize(UIBookListItemAuthorIndex) - 2);
    m_spaceBetweenAuthorAndArrow->Show(true);
    m_cover.SetDirNameOrBookName(container_node->displayName());

    sprintf(buf, StringManager::GetPCSTRById(BOOKSTORE_BOOK_COUNT), children_num);
    m_cover.SetDirBookNumOrBookAuthor(buf);
    m_cover.SetIsDuoKanBook(false);

    //m_cover.SetTitle(itemShowName + buf);
    m_cover.SetIsDir(true);
    m_imgDirArrow.SetVisible(true);
    m_cover.SetIsTrial(false);
    m_txtTrialLabel.SetVisible(false);
    m_imgNewBook.SetVisible(false);
    m_cover.SetIsNew(false);
}

void UINodeView::updateAsCloudFile()
{
    CloudFileNode* cloud_file_node = dynamic_cast<CloudFileNode*>(m_node.get());
    if (cloud_file_node == 0)
    {
        return;
    }
    cloud_file_node->update();
    m_txtAuthorLabel.SetText(cloud_file_node->artist());
    m_txtAuthorLabel.SetForeGroundColor(ColorManager::knBlack);
    m_txtAuthorLabel.SetFontSize(GetWindowFontSize(UIBookListItemAuthorIndex));

    m_txtItemNameLabel.SetText(cloud_file_node->displayName());
   
    m_cover.SetBookFormat(cloud_file_node->fileFormat());
    m_cover.SetIsDuoKanBook(false);
    m_cover.SetIsDir(false);
    m_cover.SetDirNameOrBookName(cloud_file_node->displayName());
    m_cover.SetDirBookNumOrBookAuthor(cloud_file_node->artist());
    m_spaceBetweenAuthorAndArrow->Show(false);
    m_imgDirArrow.SetVisible(false);

    // TODO.
    m_cover.SetIsTrial(false);
    m_txtTrialLabel.SetVisible(false);

    m_imgNewBook.SetVisible(false);
    m_cover.SetIsNew(false);

    // display reading progress for local file
    PCDKFile file = cloud_file_node->localFileInfo();
    if (file != 0)
    {
        int pageCount = file != 0 ? file->GetPageCount() : 0;
        int maxDot = PageCountToProgressBarDotCount(pageCount);
        int progress = file != 0 ? file->GetFilePlayProcess() : 0;
        m_readingProgressBar.SetMaxDot(maxDot);
        m_readingProgressBar.SetPercentageProgess(progress);

        if (progress <= 0)
        {
            m_cover.SetProgress(StringManager::GetPCSTRById(BOOKSTORE_READING_FEW));
        }
        else
        {
            char progressBuf[128];
            sprintf(progressBuf, StringManager::GetPCSTRById(BOOKSTORE_READING_PROGRESS), progress);
            m_cover.SetProgress(progressBuf);
        }
    }
}

void UINodeView::updateAsLocalFile()
{
	//file->SyncFile(); // TODO. Move to node
    //itemShowName = file->GetFileName();
    //m_txtAuthorLabel.SetText(file->GetFileArtist());
    FileNode* file_node = dynamic_cast<FileNode*>(m_node.get());
    if (file_node == 0)
    {
        return;
    }
    file_node->sync();
    m_txtAuthorLabel.SetText(file_node->artist());
    m_txtAuthorLabel.SetForeGroundColor(ColorManager::knBlack);
    m_txtAuthorLabel.SetFontSize(GetWindowFontSize(UIBookListItemAuthorIndex));

    m_txtItemNameLabel.SetText(m_node->displayName());
   
    m_cover.SetBookFormat(file_node->fileFormat());
    m_cover.SetIsDuoKanBook(file_node->isDuokanBook());
    m_cover.SetIsDir(false);
    m_cover.SetDirNameOrBookName(file_node->displayName());
    m_cover.SetDirBookNumOrBookAuthor(file_node->artist());
    m_spaceBetweenAuthorAndArrow->Show(false);
    m_imgDirArrow.SetVisible(false);


    // TODO.
    m_cover.SetIsTrial(file_node->isTrialBook());
    m_txtTrialLabel.SetVisible(file_node->isTrialBook());

    bool isNew = (file_node->lastRead() == 0);
    m_imgNewBook.SetVisible(isNew);
    m_cover.SetIsNew(isNew);

    PCDKFile file = file_node->fileInfo();
    int pageCount = file != 0 ? file->GetPageCount() : 0;
    int maxDot = PageCountToProgressBarDotCount(pageCount);
    int progress = file != 0 ? file->GetFilePlayProcess() : 0;
    m_readingProgressBar.SetMaxDot(maxDot);
    m_readingProgressBar.SetPercentageProgess(progress);

    if (progress <= 0)
    {
        m_cover.SetProgress(StringManager::GetPCSTRById(BOOKSTORE_READING_FEW));
    }
    else
    {
        char progressBuf[128];
        sprintf(progressBuf, StringManager::GetPCSTRById(BOOKSTORE_READING_PROGRESS), progress);
        m_cover.SetProgress(progressBuf);
    }
}

void UINodeView::updateByNode()
{
    DebugPrintf(DLC_UINodeView, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (0 == m_node)
    {
        return;
    }

    //string sItemName = m_pItemModel->GetItemName();
    string item_name = m_node->name();

    //string itemShowName = GetItemDisplayName(m_pItemModel);
    string item_display_name = m_node->displayName();

    //string itemFullPath(m_pItemModel->GetFullPath());
    string item_absolute_path = m_node->absolutePath();
    
    //PCDKFile file = NULL;
    //CDKFileManager* fileManager = CDKFileManager::GetFileManager();
    
    m_txtAuthorLabel.SetMinWidth(dkDefaultCoord);
    m_txtItemNameLabel.SetMinWidth(dkDefaultCoord);
    m_cover.SetIsLoading(false);
    m_cover.SetHighlight(false);
    m_cover.SetBookPath(item_absolute_path);

    SetShowBottomLine(false);
    m_imgItem.SetAutoSize(true);
    NodeType type = m_node->type();
    switch (type)
    {
    case NODE_TYPE_CATEGORY_LOCAL_BOOK_STORE:
    case NODE_TYPE_CATEGORY_LOCAL_PUSHED:
    case NODE_TYPE_CATEGORY_LOCAL_FOLDER:
    case NODE_TYPE_CATEGORY_VIRTUAL_BOOK_STORE:
    case NODE_TYPE_BOOKSTORE_CATEGORY:
    case NODE_TYPE_MICLOUD_CATEGORY:
        updateAsDirectory();
        break;
    case NODE_TYPE_MICLOUD_BOOK:
        updateAsCloudFile();
        break;
    case NODE_TYPE_FILE_LOCAL_DOCUMENT:
    case NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK:
        updateAsLocalFile();
        break;
    default:
        break;
    }

    switch (m_usage)
    {
        case BLU_SELECT:
            m_imgItem.SetImage(ImageManager::GetImage(IsSelected() ?
                                   IMAGE_ICON_COVER_SELECTED : IMAGE_ICON_COVER_UNSELECTED));
            m_cover.SetSelected(IsSelected());
            break;
        case BLU_BROWSE:
        default:
            m_cover.SetSelected(false);
            //m_imgItem.SetImage(GetImageByFormat(file));
            m_imgItem.SetImage(m_node->getInitialImage());
            break;
    }

    m_imgItem.SetMinSize(dkSize(GetWindowMetrics(UIHomePageImageMaxWidthIndex), m_imgItem.GetMinHeight()));

    if (m_windowSizer)
    {
        dkSize realMin = m_windowSizer->GetMinSize();
        UISizerItem* fileNameItem = m_windowSizer->GetItem(&m_txtItemNameLabel, true);
        UISizerItem* authorItem = m_windowSizer->GetItem(&m_txtAuthorLabel, true);
        if (fileNameItem && authorItem)
        {
            fileNameItem->SetProportion(0);
            authorItem->SetProportion(0);
            DebugPrintf(DLC_GUI_VERBOSE, "UINodeView::InitItem(%s), %d~%d, %d",
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

    UISizer* mainSizer = GetSizer();
    if (NULL == mainSizer)
    {
        return;
    }

    switch (m_modelDisplayMode)
    {
    case BLM_ICON:
        mainSizer->Hide(m_listViewSizer);
        m_cover.SetCoverImage(m_node->coverPath());
        break;
    case BLM_LIST:
        mainSizer->Hide(&m_cover);
        break;
    default:
        break;
    }
}

bool UINodeView::Init()
{
    if (!m_windowSizer)
    {
        m_listViewSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* horzSizer = new UIBoxSizer(dkHORIZONTAL);

        horzSizer->Add(&m_imgItem, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        horzSizer->Add(&m_txtItemNameLabel, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        horzSizer->Add(&m_txtTrialLabel, UISizerFlags().Border(dkTOP, GetWindowMetrics(UIHomePageListTipsBorderIndex)));
        horzSizer->Add(&m_imgNewBook, UISizerFlags().Border(dkTOP, GetWindowMetrics(UIHomePageListTipsBorderIndex)));

        m_txtAuthorLabel.SetAlign(ALIGN_RIGHT);
        horzSizer->SetMinHeight(GetWindowMetrics(UIHomePageListTextHeightIndex));
        horzSizer->Add(&m_txtAuthorLabel, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, GetWindowMetrics(UIHomePageListAuthorLeftBorderIndex)));
        m_spaceBetweenAuthorAndArrow = horzSizer->AddSpacer(10);
        horzSizer->Add(&m_imgDirArrow, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));

        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        mainSizer->SetName("main");
        m_listViewSizer->AddStretchSpacer();
        m_listViewSizer->Add(horzSizer, UISizerFlags().Expand());
        m_listViewSizer->Add(&m_readingProgressBar, UISizerFlags().Border(dkLEFT, GetWindowMetrics(UIHomePageImageMaxWidthIndex)).Expand());
        m_listViewSizer->AddStretchSpacer();

        m_coverViewSizer = new UIBoxSizer(dkVERTICAL);
        m_coverViewSizer->SetName("CoverViewSizer");
        m_coverViewSizer->Add(&m_cover, UISizerFlags(1).Expand());

        mainSizer->Add(m_listViewSizer, UISizerFlags(1).Expand());
        mainSizer->Add(m_coverViewSizer, UISizerFlags(1).Expand());
        SetSizer(mainSizer);
        //mainSizer->SetSizeHints(this);
    }

    m_readingProgressBar.SetBarHeight(GetWindowMetrics(UINodeViewProgressBarHeightIndex));
    m_readingProgressBar.SetMinWidth(GetWindowMetrics(UINodeViewProgressBarWidthIndex));
    m_txtItemNameLabel.SetFontSize(GetWindowFontSize(UIBookListItemFileNameIndex));
    m_txtTrialLabel.SetFontSize(GetWindowFontSize(UIBookListItemTrialIndex));
    m_txtTrialLabel.SetText(StringManager::GetPCSTRById(BOOKSTORE_TRIALVERSION));
    m_txtAuthorLabel.SetFontSize(GetWindowFontSize(UIBookListItemAuthorIndex));

    m_imgDirArrow.SetImage(IMAGE_ARROW_STRONG_RIGHT);
    m_imgNewBook.SetAutoSize(true);
    m_imgNewBook.SetImage(IMAGE_TOUCH_NEW_BOOK);
    m_readingProgressBar.SetMinHeight(5);

    AppendChild(&m_imgItem);
    AppendChild(&m_imgDirArrow);
    AppendChild(&m_txtItemNameLabel);
    AppendChild(&m_txtAuthorLabel);
    AppendChild(&m_txtTrialLabel);
    AppendChild(&m_imgNewBook);
    AppendChild(&m_readingProgressBar);
    AppendChild(&m_cover);
    return true;
}

bool UINodeView::onChildrenReady(const EventArgs& args)
{
    const NodeChildenReadyArgs& children_args = dynamic_cast<const NodeChildenReadyArgs&>(args);
    if (m_node == 0 || children_args.current_node_path != m_node->absolutePath())
    {
        // ignore
        return true;
    }

    ContainerNode* container_node = dynamic_cast<ContainerNode*>(m_node.get());
    char buf[20];
    RetrieveChildrenResult ret = RETRIEVE_FAILED;
    sprintf(buf, StringManager::GetPCSTRById(BOOKSTORE_BOOK_COUNT),
        container_node->children(ret, false, container_node->statusFilter()).size());
    m_cover.SetDirBookNumOrBookAuthor(buf);
    UpdateWindow();
    return true;
}

bool UINodeView::onStatusUpdated(const EventArgs& args)
{
    const NodeStatusUpdated& status_update_args = dynamic_cast<const NodeStatusUpdated&>(args);
    if (status_update_args.current_node_path == m_node->absolutePath())
    {
        printf("\nUINodeView::onStatusUpdated: %s\n", m_node->absolutePath().c_str());
        printf("\nNode State:%d\n", m_node->status());

        int loading_progress = -1;
        int loading_state    = IDownloadTask::NONE;
        if (m_node->loadingInfo(loading_progress, loading_state))
        {
            printf("\nNode Download Progress:%d\n", loading_progress);
            printf("\nNode Download State:%d\n", loading_state);
        }
        updateByNode();
    }
    return true;
}

bool UINodeView::onLoadingFinished(const EventArgs& args)
{
    const NodeLoadingFinishedArgs& loading_finished_args = dynamic_cast<const NodeLoadingFinishedArgs&>(args);
    if (loading_finished_args.current_node_path == m_node->absolutePath())
    {
        if (!loading_finished_args.succeeded)
        {
            // TODO. Display error message
        }
    }
    return true;
}

void UINodeView::SetModelDisplayMode(ModelDisplayMode mode)
{
    //mode = BLM_ICON;
    m_modelDisplayMode = mode;
    UISizer* mainSizer = GetSizer();
    if (NULL == mainSizer)
    {
        return;
    }
    switch (m_modelDisplayMode)
    {
        case BLM_ICON:
            mainSizer->Hide(m_listViewSizer);
            mainSizer->Show(m_coverViewSizer);
            break;
        case BLM_LIST:
            mainSizer->Hide(m_coverViewSizer);
            mainSizer->Show(m_listViewSizer);
            break;
    }
}

HRESULT UINodeView::Draw(DK_IMAGE drawingImg)
{
    updateSelectedImage();
    switch (m_modelDisplayMode)
    {
        case BLM_LIST:
            return DrawListView(drawingImg);
        case BLM_ICON:
            return DrawCoverView(drawingImg);
        default:
            return S_OK;
    }

}

HRESULT UINodeView::DrawListView(DK_IMAGE drawingImg)
{
	HRESULT hr(S_OK);

    /*DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    DebugPrintf(DLC_GUI_VERBOSE, "highlight: (%d, %d) ~ (%d, %d)",
            m_iLeft, m_iTop, m_iWidth, m_iHeight);
    DK_IMAGE imgSelf;
    RTN_HR_IF_FAILED(CropImage(drawingImg, rcSelf, &imgSelf));*/
    hr = UIListItemBase::Draw(drawingImg);
	return hr;
}

HRESULT UINodeView::DrawCoverView(DK_IMAGE drawingImg)
{
	HRESULT hr(S_OK);
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    DK_IMAGE imgSelf;
    RTN_HR_IF_FAILED(CropImage(drawingImg, rcSelf, &imgSelf));
    return m_cover.Draw(imgSelf);
}

bool UINodeView::IsSelected() const
{
    return m_node->selected();
}

void UINodeView::updateSelectedImage()
{
    if (m_node)
    {
        bool selected = m_node->selected();
        m_cover.SetSelected(selected);
        m_imgItem.SetImage(ImageManager::GetImage(
            selected 
            ? IMAGE_ICON_COVER_SELECTED 
            : IMAGE_ICON_COVER_UNSELECTED));
        m_imgItem.SetMinSize(dkSize(GetWindowMetrics(UIHomePageImageMaxWidthIndex), m_imgItem.GetMinHeight()));
    }
}

void UINodeView::SetSelected(bool selected)
{
    m_node->setSelected(selected);
}

void UINodeView::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
    return UIListItemBase::MoveWindow(iLeft, iTop, iWidth, iHeight);
}

void UINodeView::SetIsLoading(bool isLoading)
{
    m_cover.SetIsLoading(isLoading);
}

int UINodeView::PageCountToProgressBarDotCount(int pageCount)
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
