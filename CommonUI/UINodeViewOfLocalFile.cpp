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

#include "Framework/CDisplay.h"
#include "Framework/CNativeThread.h"

#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "GUI/UIMessageBox.h"

#include "Common/WindowsMetrics.h"
#include "Common/BookCoverLoader.h"

#include "Common/FileManager_DK.h"
#include "CommonUI/UIWeiboSharePage.h"
#include "CommonUI/CPageNavigator.h"

#include "Utility/FileSystem.h"
#include "Utility/PathManager.h"
#include "Utility/StringUtil.h"
#include "Utility/RenderUtil.h"
#include "drivers/DeviceInfo.h"
#include <tr1/functional>
#include "Model/container_node.h"
#include "Model/local_doc_node.h"
#include "Model/local_bookstore_category_node.h"
#include "TouchAppUI/BookOpenManager.h"

using namespace WindowsMetrics;
using namespace dk::utility;
using namespace dk::document_model;

UINodeViewOfLocalFile::UINodeViewOfLocalFile(
        UIModelView* pParent,
        ModelTree* model_tree,
        BookListUsage usage,
        int iconWidth,
        int iconHeight)
    : IUINodeView(pParent, model_tree, usage, iconWidth, iconHeight)
{
    m_imgSelect.SetVisible(usage != BLU_BROWSE);
    Init();
}

UINodeViewOfLocalFile::~UINodeViewOfLocalFile()
{
}

void UINodeViewOfLocalFile::updateWndStauts()
{
    FileNode* file_node = dynamic_cast<FileNode*>(m_node.get());
    if (file_node && isSelectMode())
    {
        int node_status = file_node->status();
        if (node_status & NODE_IS_UPLOADING)
        {
            int uploading_progress = -1;
            int uploading_state = IDownloadTask::NONE;
            file_node->loadingInfo(uploading_progress, uploading_state);
            if (uploading_progress < 0 )
            {
                uploading_progress = 0;
            }
            char buf[100];
            snprintf(buf, DK_DIM(buf), "%d%%", uploading_progress); // TODO. display "waiting" instead of 0%
            m_btnUploadOrRead.SetText(buf);
            m_btnUploadOrRead.SetEnable(FALSE);
        }
        else if (node_status & NODE_CLOUD) // this file has been uploaded
        {
            m_btnUploadOrRead.SetText(StringManager::GetStringById(CLOUD_ALREADY_UPLOADED));
            m_btnUploadOrRead.SetEnable(FALSE);
        }
    }
}

void UINodeViewOfLocalFile::update()
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
    m_txtSizeLabel.SetMinWidth(dkDefaultCoord);

    m_imgItem.SetAutoSize(true);
    m_imgItem.SetImage(m_node->getInitialImage());
    m_imgItem.SetMinSize(dkSize(GetWindowMetrics(UIHomePageImageMaxWidthIndex), m_imgItem.GetMinHeight()));

    m_txtAuthorLabel.SetText(file_node->artist());
    m_txtAuthorLabel.SetForeGroundColor(ColorManager::knBlack);
    m_txtAuthorLabel.SetFontSize(GetWindowFontSize(UIBookListItemAuthorIndex));

    char szTmp[64] = {0};
    size_t size = file_node->fileSize();
    if (size >= 1048576)
    {
        snprintf(szTmp, DK_DIM(szTmp), "%.2f MB", size / 1048576.0);
    }
    else
    {
        snprintf(szTmp, DK_DIM(szTmp), "%.2f KB", size / 1024.0);
    }
    m_txtSizeLabel.SetText(szTmp);
    m_txtItemNameLabel.SetText(file_node->displayName());

    m_cover.SetBookPath(item_absolute_path);
    m_cover.SetBookFormat(file_node->fileFormat());
    m_cover.SetIsDuoKanBook(file_node->isDuokanBook());
    m_cover.SetDirNameOrBookName(file_node->displayName());
    m_cover.SetDirBookNumOrBookAuthor(file_node->artist());
    m_cover.SetCoverImage(file_node->coverPath());
    m_cover.SetIsTrial(file_node->isTrialBook());
    m_cover.SetIsNew(file_node->lastRead() == 0);

    PCDKFile file = file_node->fileInfo();
    int pageCount = file != 0 ? file->GetPageCount() : 0;
    int maxDot = PageCountToProgressBarDotCount(pageCount);
    int progress = file != 0 ? file->GetFilePlayProcess() : 0;
    m_readingProgressBar.SetMaxDot(maxDot);
    m_readingProgressBar.SetPercentageProgess(progress);

    updateWndStauts();
    SetModelDisplayMode(m_modelDisplayMode);
    IUINodeView::update();
    updateItemNameLayout();
    Layout();
    Repaint();
}

void UINodeViewOfLocalFile::updateItemNameLayout()
{
    if (m_windowSizer && m_txtAuthorLabel.IsVisible())
    {
        dkSize realMin = m_windowSizer->GetMinSize();
        UISizerItem* fileNameItem = m_windowSizer->GetItem(&m_txtItemNameLabel, true);
        UISizerItem* authorItem = m_windowSizer->GetItem(&m_txtAuthorLabel, true);
        if (fileNameItem && authorItem)
        {
            fileNameItem->SetProportion(0);
            authorItem->SetProportion(0);

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
}

void UINodeViewOfLocalFile::Init()
{
    m_imgItem.SetAutoSize(true);
    m_imgNewBook.SetAutoSize(true);
    m_imgNewBook.SetImage(IMAGE_TOUCH_NEW_BOOK);


    const int downloadFontSize = GetWindowFontSize(FontSize18Index);
    m_btnUploadOrRead.Initialize(IDSTATIC, NULL, downloadFontSize);
    m_btnUploadOrRead.ShowBorder(false);
    m_btnUploadOrRead.SetMinSize(GetWindowMetrics(UINormalSmallBtnWidthIndex), GetWindowMetrics(UINormalBtnHeightIndex));

    m_txtTrialLabel.SetFontSize(GetWindowFontSize(UIBookListItemTrialIndex));
    m_txtTrialLabel.SetText(StringManager::GetPCSTRById(BOOKSTORE_TRIALVERSION));
    m_txtAuthorLabel.SetFontSize(GetWindowFontSize(UIBookListItemAuthorIndex));
    m_txtSizeLabel.SetFontSize(GetWindowFontSize(FontSize12Index));
    m_txtSizeLabel.SetEnglishGothic();
    m_txtAuthorLabel.SetAlign(ALIGN_RIGHT);

    AppendChild(&m_imgItem);
    AppendChild(&m_txtItemNameLabel);
    AppendChild(&m_imgNewBook);
    AppendChild(&m_btnUploadOrRead);
    AppendChild(&m_imgSelect);
    AppendChild(&m_txtAuthorLabel);
    AppendChild(&m_txtTrialLabel);
    AppendChild(&m_txtSizeLabel);
    AppendChild(&m_readingProgressBar);
    m_btnUploadOrRead.SetVisible(FALSE); // upload button is disabled by default
}

UISizer* UINodeViewOfLocalFile::getListViewSizer()
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
                const int elemSpacing = GetWindowMetrics(UIPixelValue10Index);
                horzSizer->Add(&m_imgItem, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
                horzSizer->Add(&m_txtItemNameLabel, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Reduce(1));
                UISizer* pFlagSizer = new UIBoxSizer(dkVERTICAL);
                if (pFlagSizer)
                {
                    const int topMargin = GetWindowMetrics(/*UIHomePageListTipsBorder*/UIPixelValue1Index);
                    UISizer* pTrialAndNewSizer = new UIBoxSizer(dkHORIZONTAL);
                    if (pTrialAndNewSizer)
                    {
                        pTrialAndNewSizer->Add(&m_txtTrialLabel, UISizerFlags().Border(dkTOP, topMargin));
                        pTrialAndNewSizer->Add(&m_imgNewBook, UISizerFlags().Border(dkTOP, topMargin));
                        pFlagSizer->Add(pTrialAndNewSizer);
                    }
                    pFlagSizer->AddStretchSpacer();
                    pFlagSizer->Add(&m_txtSizeLabel);
                    horzSizer->Add(pFlagSizer, UISizerFlags().Expand().Border(dkLEFT, elemSpacing).Border(dkBOTTOM, GetWindowMetrics(UIPixelValue8Index)));
                }

                horzSizer->SetMinHeight(GetWindowMetrics(UIHomePageListTextHeightIndex));
                horzSizer->AddStretchSpacer();
                horzSizer->Add(&m_txtAuthorLabel, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, GetWindowMetrics(UIHomePageListAuthorLeftBorderIndex)).Reduce(1));
                horzSizer->Add(&m_imgSelect, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, elemSpacing));
                horzSizer->Add(&m_btnUploadOrRead, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, elemSpacing));

                m_listViewSizer->Add(horzSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin));
            }
            AppendChild(&m_readingProgressBar);

            m_listViewSizer->Add(&m_readingProgressBar, UISizerFlags().Border(dkRIGHT, horizontalMargin).Border(dkLEFT, horizontalMargin + GetWindowMetrics(UIHomePageImageMaxWidthIndex)).Expand());
            m_listViewSizer->AddStretchSpacer();
            updateItemNameLayout();
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

bool UINodeViewOfLocalFile::deleteFile()
{
    UIMessageBox messagebox(m_pParent,
                            StringManager::GetStringById(SURE_DELETE_BOOK),
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

void UINodeViewOfLocalFile::execute(unsigned long command_id)
{
    switch(command_id)
    {
    case ID_BTN_READ_BOOK:
        handleClicked();
        break;
    case ID_BTN_DELETE:
        deleteFile();
        break;
    case ID_BTN_DELETE_FROM_CATEGORY:
        deleteFromCategory();
        break;
    case ID_BTN_SINAWEIBO_SHARE:
        shareToWeibo();
        break;
    case ID_BIN_UPLOAD_BOOK:
        uploadFile();
        break;
    default:
        break;
    }
}

void UINodeViewOfLocalFile::updateWndVisible()
{
    FileNode* file_node = dynamic_cast<FileNode*>(m_node.get());
    if (file_node == 0)
    {
        return;
    }

    int node_status = file_node->status();
    const bool isDownload = (node_status & NODE_IS_UPLOADING) || (node_status & NODE_CLOUD);
    const bool hasAuthor = !file_node->artist().empty();
    const bool isTrial = file_node->isTrialBook();
    const bool isNewBook = (file_node->lastRead() == 0);
    const bool isList = (m_modelDisplayMode == BLM_LIST);
    const bool selectMode = isSelectMode();
    m_imgSelect.SetVisible(isList && selectMode && !isDownload);
    m_imgNewBook.SetVisible(isList && !selectMode && isNewBook);
    m_btnUploadOrRead.SetVisible(isList && selectMode && isDownload);
    m_txtSizeLabel.SetVisible(isList && selectMode);
    m_txtAuthorLabel.SetVisible(isList && hasAuthor);
    m_txtTrialLabel.SetVisible(isList && isTrial);
    m_readingProgressBar.SetVisible(isList && !selectMode);
}

void UINodeViewOfLocalFile::SetModelDisplayMode(ModelDisplayMode mode)
{
    IUINodeView::SetModelDisplayMode(mode);
    updateWndVisible();
}

void UINodeViewOfLocalFile::handleClicked()
{
    if (m_cover.usage() & (BLU_SELECT_EXPAND | BLU_SELECT))
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
    openFile();
}

bool UINodeViewOfLocalFile::onLoadingFinishedAction(const EventArgs& args)
{
    // TODO. Implement Me
    return false;
}

bool UINodeViewOfLocalFile::openFile()
{
    FileNode* file_node = dynamic_cast<FileNode*>(m_node.get());
    if (file_node == 0)
    {
        return false;
    }

    string file_path = m_node->absolutePath();
    //int file_id = file_node->fileInfo()->GetFileID(); // TODO. Support m_node->id() in the future

    //
    NodeOpenBookArgs open_book_args;
    open_book_args.node_path = file_path;
    FireEvent(EventNodeOpenBook, open_book_args);
    SetIsLoading(true);
    UpdateWindow();

    SNativeMessage msg;
    msg.iType = KMessageOpenBook;
    CNativeThread::Send(msg);
    return true;
}

bool UINodeViewOfLocalFile::uploadFile()
{
    int node_state = m_node->status();
    if (!(node_state & NODE_IS_UPLOADING))
    {
        m_node->upload(true);
        return true;
    }
    return true;
}

bool UINodeViewOfLocalFile::deleteFromCategory()
{
    FileNode* file_node = dynamic_cast<FileNode*>(m_node.get());
    if (0 != file_node)
    {
        return file_node->removeFromCategory();
    }
    return false;
}

bool UINodeViewOfLocalFile::shareToWeibo()
{
    FileNode* file_node = dynamic_cast<FileNode*>(m_node.get());
    if(file_node != 0)
    {
        string displayText = StringManager::GetPCSTRById(ELEGANT_READING_TIME);
        displayText.append(StringManager::GetPCSTRById(SHAREWEIBO_HINT));
        char buf[1024] = {0};
        snprintf(buf, sizeof(buf)/sizeof(char), displayText.c_str(), file_node->displayName().c_str());
        UIWeiboSharePage* pPage = new UIWeiboSharePage(buf);
        if (pPage)
        {
            CPageNavigator::Goto(pPage);
            return true;
        }
    }
    return false;
}

std::vector<int> UINodeViewOfLocalFile::getSupportedNodeTypes()
{
    std::vector<int> support_types;
    support_types.push_back(NODE_TYPE_FILE_LOCAL_DOCUMENT);
    support_types.push_back(NODE_TYPE_FILE_LOCAL_BOOK_STORE_BOOK);
    return support_types;
}
