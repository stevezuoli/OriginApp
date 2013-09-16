////////////////////////////////////////////////////////////////////////
// UINodeViewCloudFileWithCover.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "CommonUI/UINodeViewCloudFile.h"
#include "DkSPtr.h"
#include "Public/Base/TPDef.h"
#include "I18n/StringManager.h"
#include "BookStore/BookDownloadController.h"

#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "GUI/UIMessageBox.h"

#include "Common/FileManager_DK.h"
#include "Common/WindowsMetrics.h"
#include "Common/BookCoverLoader.h"

#include "CommonUI/UIWeiboSharePage.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"

#include "Utility/FileSystem.h"
#include "Utility/PathManager.h"
#include "Utility/StringUtil.h"
#include "Utility/RenderUtil.h"
#include "drivers/DeviceInfo.h"
#include <tr1/functional>
#include "Model/container_node.h"
#include "Model/cloud_file_node.h"

#include "Framework/CDisplay.h"
#include "Framework/CNativeThread.h"

using namespace WindowsMetrics;
using namespace dk::utility;
using namespace dk::document_model;

UINodeViewCloudFile::UINodeViewCloudFile(
        UIModelView* pParent,
        ModelTree* model_tree,
        BookListUsage usage,
        int iconWidth,
        int iconHeight)
    : IUINodeView(pParent, model_tree, usage, iconWidth, iconHeight)
{
    m_btnDownloadOrRead.SetVisible(!isSelectMode());
    m_imgSelect.SetVisible(isSelectMode());
    Init();
}

UINodeViewCloudFile::~UINodeViewCloudFile()
{
}

void UINodeViewCloudFile::updateWndStauts()
{
    CloudFileNode* file_node = dynamic_cast<CloudFileNode*>(m_node.get());
    if (file_node == 0)
    {
        return;
    }
   
    bool newBook = false;
    int pageCount = 0;
    int progress = 0;
    PCDKFile local_file_info = file_node->localFileInfo();
    if (local_file_info == 0)
    {
        m_cover.SetIsDuoKanBook(false);
        m_cover.SetIsTrial(false);

        int node_status = m_node->status();
        if (node_status & NODE_IS_DOWNLOADING)
        {
            int downloading_progress = -1;
            int downloading_state = IDownloadTask::NONE;
            m_node->loadingInfo(downloading_progress, downloading_state);
            if (downloading_progress < 0)
            {
                downloading_progress = 0;
            }
            char buf[100];
            snprintf(buf, DK_DIM(buf), "%d%%", downloading_progress);
            m_btnDownloadOrRead.SetText(buf);
            m_btnDownloadOrRead.SetEnable(FALSE);
        }
        else if (node_status & NODE_CLOUD) // not on local
        {
            m_btnDownloadOrRead.SetText(StringManager::GetStringById(TOUCH_DOWNLOAD));
            m_btnDownloadOrRead.SetEnable(TRUE);
        }
    }
    else
    {
        // is local node
        newBook = (local_file_info->GetFileLastReadTime() == 0);
        pageCount = local_file_info->GetPageCount();
        progress = local_file_info->GetFilePlayProcess();

        m_cover.SetIsDuoKanBook(local_file_info->IsDuoKanBook()); // must be false
        m_cover.SetIsTrial(local_file_info->GetIsTrialBook());

        m_btnDownloadOrRead.SetText(StringManager::GetStringById(isSelectMode() ? BOOKSTORE_DOWNLOADED : BOOK_READING));
        m_btnDownloadOrRead.SetEnable(TRUE);
    }
    m_cover.SetIsNew(newBook);

    m_readingProgressBar.SetMaxDot(PageCountToProgressBarDotCount(pageCount));
    m_readingProgressBar.SetPercentageProgess(progress);
}

void UINodeViewCloudFile::update()
{
    CloudFileNode* file_node = dynamic_cast<CloudFileNode*>(m_node.get());
    if (file_node == 0)
    {
        return;
    }

    file_node->update();

    string item_absolute_path = file_node->absolutePath();

    m_cover.SetBookPath(item_absolute_path);
    m_cover.SetBookFormat(file_node->fileFormat());
    m_cover.SetDirNameOrBookName(file_node->displayName());
    m_cover.SetDirBookNumOrBookAuthor(file_node->artist());
    m_cover.SetCoverImage(file_node->coverPath());

    m_txtItemNameLabel.SetMinWidth(dkDefaultCoord);
    m_txtItemNameLabel.SetText(file_node->displayName());

    m_imgItem.SetImage(file_node->getInitialImage());
    m_imgItem.SetMinSize(dkSize(GetWindowMetrics(UIHomePageImageMaxWidthIndex), m_imgItem.GetMinHeight()));

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
    m_fileSpace.SetText(szTmp);
    m_fileSpace.SetEnglishGothic();
    string strTime = UIUtility::TransferIntToString(file_node->createTime() / 1000);
    snprintf(szTmp, DK_DIM(szTmp), StringManager::GetPCSTRById(CLOUD_UPLOADTIME), strTime.c_str());
    m_uploadTime.SetText(szTmp);
    m_uploadTime.SetEnglishGothic();

    updateWndStauts();
    SetModelDisplayMode(m_modelDisplayMode);
    IUINodeView::update();
    Layout();
    Repaint();
}

void UINodeViewCloudFile::Init()
{
    m_imgItem.SetAutoSize(true);

    m_imgNewBook.SetAutoSize(true);
    m_imgNewBook.SetImage(IMAGE_TOUCH_NEW_BOOK);

    const int fileSpaceFontSize = GetWindowFontSize(FontSize14Index);
    m_fileSpace.SetFontSize(fileSpaceFontSize);;

    const int uploadTimeFontSize = GetWindowFontSize(FontSize14Index);
    m_uploadTime.SetFontSize(uploadTimeFontSize);

    const int downloadFontSize = GetWindowFontSize(FontSize18Index);
    m_btnDownloadOrRead.Initialize(IDSTATIC, NULL, downloadFontSize);
    m_btnDownloadOrRead.SetBackgroundImage(IMAGE_IME_KEY_OBLONG);
    m_btnDownloadOrRead.SetMinSize(GetWindowMetrics(UINormalSmallBtnWidthIndex), GetWindowMetrics(UINormalBtnHeightIndex));

    AppendChild(&m_imgItem);
    AppendChild(&m_txtItemNameLabel);
    AppendChild(&m_imgNewBook);
    AppendChild(&m_btnDownloadOrRead);
    AppendChild(&m_imgSelect);
    AppendChild(&m_fileSpace);
    AppendChild(&m_uploadTime);
    AppendChild(&m_readingProgressBar);
}

void UINodeViewCloudFile::updateWndVisible()
{
    const bool isList = (m_modelDisplayMode == BLM_LIST);
    CloudFileNode* file_node = dynamic_cast<CloudFileNode*>(m_node.get());
    if (file_node == 0)
    {
        return;
    }

    int node_status = file_node->status();
    const bool isDownloading = node_status & NODE_IS_DOWNLOADING;
    const bool isNewBook = (file_node->localFileInfo()) && (file_node->lastRead() == 0);

    m_imgSelect.SetVisible(isList && isSelectMode() && !isDownloading);
    m_imgNewBook.SetVisible(isList && isNewBook);
    m_btnDownloadOrRead.SetVisible(isList && (!isSelectMode() || isDownloading));
}

void UINodeViewCloudFile::SetModelDisplayMode(ModelDisplayMode mode)
{
    IUINodeView::SetModelDisplayMode(mode);
    updateWndVisible();
}

UISizer* UINodeViewCloudFile::getListViewSizer()
{
    if (NULL == m_listViewSizer)
    {
        m_listViewSizer = new UIBoxSizer(dkVERTICAL);
        if (m_listViewSizer)
        {
            const int horizontalMargin = GetWindowMetrics(UIPixelValue30Index);
            const int elemSpacing = GetWindowMetrics(UIPixelValue10Index);
            m_listViewSizer->AddStretchSpacer();
            UISizer* horzSizer = new UIBoxSizer(dkHORIZONTAL);
            if (horzSizer)
            {
                const int topMargin = GetWindowMetrics(UIPixelValue3Index);
                horzSizer->Add(&m_imgItem, UISizerFlags().Border(dkTOP, topMargin));
                UISizer* pBookSizer = new UIBoxSizer(dkVERTICAL);
                if (pBookSizer)
                {
                    UISizer* pUpInfoSizer = new UIBoxSizer(dkHORIZONTAL);
                    if (pUpInfoSizer)
                    {
                        pUpInfoSizer->Add(&m_txtItemNameLabel, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
                        pUpInfoSizer->Add(&m_imgNewBook, UISizerFlags().Border(dkTOP, topMargin));
                        pBookSizer->Add(pUpInfoSizer);
                    }
                    pBookSizer->AddSpacer(elemSpacing);
                    UISizer* pDownInfoSizer = new UIBoxSizer(dkHORIZONTAL);
                    if (pDownInfoSizer)
                    {
                        pDownInfoSizer->Add(&m_fileSpace);
                        pDownInfoSizer->Add(&m_uploadTime, UISizerFlags().Expand().Border(dkLEFT, elemSpacing));
                        pBookSizer->Add(pDownInfoSizer);
                    }
                    horzSizer->Add(pBookSizer, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Reduce(1));
                }
                horzSizer->AddStretchSpacer();
                horzSizer->Add(&m_imgSelect, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, elemSpacing));
                horzSizer->Add(&m_btnDownloadOrRead, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, elemSpacing));

                m_listViewSizer->Add(horzSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin));
            }

            m_listViewSizer->Add(&m_readingProgressBar, UISizerFlags().Border(dkRIGHT, horizontalMargin).Border(dkLEFT, horizontalMargin + GetWindowMetrics(UIHomePageImageMaxWidthIndex)).Expand().Border(dkTOP, GetWindowMetrics(UIPixelValue5Index)));
            m_listViewSizer->AddStretchSpacer();
        }
    }
    return m_listViewSizer;
}

int UINodeViewCloudFile::PageCountToProgressBarDotCount(int pageCount)
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

void UINodeViewCloudFile::execute(unsigned long command_id)
{
    switch (command_id)
    {
    case ID_BTN_READ_BOOK:
        openFile();
        break;
    case ID_BIN_DOWNLOAD_BOOK:
        download();
        break;
    case ID_BTN_DELETE:
        deleteFile();
        break;
    case ID_BTN_SINAWEIBO_SHARE:
        shareToWeibo();
        break;
    default:
        break;
    }
}

bool UINodeViewCloudFile::deleteFile()
{
    UIMessageBox messagebox(m_pParent,
                            StringManager::GetStringById(SURE_DELETE_BOOK),
                            MB_OK | MB_CANCEL);
    if(MB_OK == messagebox.DoModal())
    {
        /*string sPath(pSelectedItem->GetItemPath());
        sPath.append("/");
        sPath.append(pSelectedItem->GetItemName());*/
        return m_node->remove(false, true);
    }
    return false;
}

bool UINodeViewCloudFile::openFile()
{
    CloudFileNode* cloud_file_node = dynamic_cast<CloudFileNode*>(m_node.get());
    if (cloud_file_node == 0 || cloud_file_node->localFileInfo() == 0)
    {
        return false;
    }

    NodeOpenBookArgs open_book_args;
    open_book_args.node_path = cloud_file_node->absolutePath();
    FireEvent(EventNodeOpenBook, open_book_args);
    SetIsLoading(true);
    UpdateWindow();

    SNativeMessage msg;
    msg.iType = KMessageOpenBook;
    CNativeThread::Send(msg);
    return true;
}

bool UINodeViewCloudFile::shareToWeibo()
{
    string displayText = StringManager::GetPCSTRById(ELEGANT_READING_TIME);
    displayText.append(StringManager::GetPCSTRById(SHAREWEIBO_HINT));
    char buf[1024] = {0};
    snprintf(buf, sizeof(buf)/sizeof(char), displayText.c_str(), m_node->name().c_str());
    UIWeiboSharePage* pPage = new UIWeiboSharePage(buf);
    if (pPage)
    {
        CPageNavigator::Goto(pPage);
        return true;
    }
    return false;
}

bool UINodeViewCloudFile::download()
{
    int node_state = m_node->status();
    if (!(node_state & NODE_IS_DOWNLOADING))
    {
        m_node->download(true);
        return true;
    }
    return true;
}

bool UINodeViewCloudFile::OnChildClick(UIWindow* child)
{
    if (child == &m_btnDownloadOrRead)
    {
        handleClicked();
        return true;
    }
    return IUINodeView::OnChildClick(child);
}

void UINodeViewCloudFile::handleClicked()
{
    if (m_cover.usage() & (BLU_SELECT_EXPAND | BLU_SELECT)) //&& (rootNodeDisplayMode() != BY_FOLDER))
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
    CloudFileNode* cloud_file_node = dynamic_cast<CloudFileNode*>(m_node.get());
    if (cloud_file_node->localFileInfo() != 0)
    {
        openFile();
    }
    else
    {
        download();
    }
}

bool UINodeViewCloudFile::onLoadingFinishedAction(const EventArgs& args)
{
    // TODO. Implement Me
    return false;
}

std::vector<int> UINodeViewCloudFile::getSupportedNodeTypes()
{
    std::vector<int> support_types;
    support_types.push_back(NODE_TYPE_MICLOUD_BOOK);
    return support_types;
}
