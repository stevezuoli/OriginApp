////////////////////////////////////////////////////////////////////////
// UIDirectoryListItem.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIDirectoryListItem.h"
#include "DkSPtr.h"
#include "Public/Base/TPDef.h"
#include "I18n/StringManager.h"
#include "../Common/FileManager_DK.h"
#include "Utility/FileSystem.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Common/BookCoverLoader.h"
#include "Utility/PathManager.h"
#include "Utility/StringUtil.h"
#include "Utility/RenderUtil.h"
#include "drivers/DeviceInfo.h"

using namespace WindowsMetrics;
using namespace dk::utility;

UIDirectoryListItem::UIDirectoryListItem(
        UICompoundListBox* pParent,
        BookListUsage usage,
        int iconWidth,
        int iconHeight)
    : UIListItemBase(pParent, IDSTATIC)
    , m_bIsDisposed(FALSE)
    , m_pItemModel(NULL)
    , m_bookListMode(BLM_ICON)
    , m_usage(usage)
    , m_spaceBetweenAuthorAndArrow(NULL)
    , m_cover(this, usage,
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

UIDirectoryListItem::~UIDirectoryListItem()
{
}

void UIDirectoryListItem::SetItemModel(IDirectoryItemModel *pItemModel)
{
    m_pItemModel = pItemModel;
    InitItem();
}

BOOL UIDirectoryListItem::OnKeyPress(INT32 iKeyCode)
{
    return TRUE;
}

void UIDirectoryListItem::SetFocus(bool bIsFocus)
{
    if (m_bIsFocus != bIsFocus)
    {
        m_bIsFocus = bIsFocus;
    }
}

bool  UIDirectoryListItem::SetVisible(bool bVisible)
{
    UIContainer::SetVisible(bVisible);
    return bVisible;
}

void UIDirectoryListItem::SetHighLight(bool isHighLight)
{
    if (BLM_ICON == m_bookListMode)
    {
        m_cover.SetHighlight(isHighLight);
        UpdateWindow();
    }
    else
    {
        UIListItemBase::SetHighLight(isHighLight);
    }
}

static std::string GetItemDisplayName(IDirectoryItemModel* model)
{
    if (model->GetModelType() != IDirectoryItemModel::MT_DIRECTORY)
    {
        return model->GetItemName();
    }
    std::string fullPath = PathManager::ConcatPath(
            model->GetItemPath().c_str(),
            model->GetItemName().c_str());
    return PathManager::GetDirectoryDisplayName(fullPath.c_str());
}

SPtr<ITpImage> UIDirectoryListItem::GetImageByFormat(PCDKFile file)
{
    SPtr<ITpImage> imgBookFormat = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_FOLDER);
    if (file)
    {
        DebugPrintf(DLC_UIDIRECTORYLISTITEM, "%s, %d, %s, %s FileFormat = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, file->GetFileFormat());
        switch(file->GetFileFormat())
        {
        case DFF_Text:
            {
                imgBookFormat = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_TXT);
            }
            break;
        case DFF_ElectronicPublishing:
            {
                if (IsDuoKanBookStoreBook(file->GetFilePath()))
                {
                    imgBookFormat = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_DUOKAN);
                }
                else
                {
                    imgBookFormat = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_EPUB);
                }
            }
            break;
        case DFF_MobiPocket:
            {
                imgBookFormat = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_MOBI);
            }
            break;
        case DFF_PortableDocumentFormat:
            {
                imgBookFormat = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_PDF);
            }
            break;
        case DFF_ZipFile:
        case DFF_RoshalArchive:
            {
                imgBookFormat = ImageManager::GetImage(IMAGE_TOUCH_ICON_TYPE_ZIP);
            }
            break;
        default:
            return SPtr<ITpImage>();
        }
    }
    return imgBookFormat;
}

void UIDirectoryListItem::InitItem()
{
    DebugPrintf(DLC_UIDIRECTORYLISTITEM, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (NULL == m_pItemModel)
    {
        return;
    }
    bool isTrial = false;
    string sItemName = m_pItemModel->GetItemName();
    string itemShowName = GetItemDisplayName(m_pItemModel);

    string itemFullPath(m_pItemModel->GetFullPath());
    
    PCDKFile file = NULL;
    CDKFileManager* fileManager = CDKFileManager::GetFileManager();
    m_txtAuthorLabel.SetMinWidth(dkDefaultCoord);
    m_txtItemNameLabel.SetMinWidth(dkDefaultCoord);
    m_cover.SetIsLoading(false);
    m_cover.SetHighlight(false);
    m_cover.SetBookPath(itemFullPath);
    SetShowBottomLine(false);
    if (IsDir())
    {
        char buf[20];
        snprintf(buf, DK_DIM(buf),
                "%d", fileManager->GetAllBookNumsUnderDir(itemFullPath.c_str()));
        m_txtItemNameLabel.SetText(itemShowName.c_str());
        m_txtAuthorLabel.SetText(buf);
        m_txtAuthorLabel.SetForeGroundColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
        m_txtAuthorLabel.SetFontSize(GetWindowFontSize(UIBookListItemAuthorIndex) - 2);
        m_spaceBetweenAuthorAndArrow->Show(true);
        m_cover.SetDirNameOrBookName(itemShowName);

        //snprintf(buf,
        //        DK_DIM(buf), 
        //        "(%d)",
        //        fileManager->GetAllBookNumsUnderDir(itemFullPath.c_str()));
        sprintf(buf,
            StringManager::GetPCSTRById(BOOKSTORE_BOOK_COUNT),
            fileManager->GetAllBookNumsUnderDir(itemFullPath.c_str()));

        m_cover.SetDirBookNumOrBookAuthor(buf);
        m_cover.SetIsDuoKanBook(false);
        //m_cover.SetTitle(itemShowName + buf);
        m_cover.SetIsDir(true);
    }
    else
    {
        file = fileManager->GetFileByPath(itemFullPath);
        if(!file)
        {
            return;
        }
		file->SyncFile();
        itemShowName = file->GetFileName();
        m_txtAuthorLabel.SetText(file->GetFileArtist());
        m_txtAuthorLabel.SetForeGroundColor(ColorManager::knBlack);
        m_txtAuthorLabel.SetFontSize(GetWindowFontSize(UIBookListItemAuthorIndex));
        m_txtItemNameLabel.SetText(itemShowName.c_str());
        isTrial = file->GetIsTrialBook();

        m_cover.SetBookFormat(file->GetFileFormat());
        m_cover.SetIsDuoKanBook(IsDuoKanBookStoreBook(itemFullPath.c_str()));
        m_cover.SetIsDir(false);
        m_cover.SetDirNameOrBookName(itemShowName);
        m_cover.SetDirBookNumOrBookAuthor(file->GetFileArtist());
        m_spaceBetweenAuthorAndArrow->Show(false);
    }

    
    m_imgItem.SetAutoSize(true);

    switch (m_usage)
    {
        case BLU_SELECT:
            m_imgItem.SetImage(ImageManager::GetImage(
                        IsSelected() ? IMAGE_ICON_COVER_SELECTED : IMAGE_ICON_COVER_UNSELECTED));
            m_cover.SetSelected(IsSelected());
            break;
        case BLU_BROWSE:
        default:
            m_cover.SetSelected(false);
            m_imgItem.SetImage(GetImageByFormat(file));
            break;
    }

    m_imgItem.SetMinSize(dkSize(GetWindowMetrics(UIHomePageImageMaxWidthIndex), m_imgItem.GetMinHeight()));
    
    m_imgDirArrow.SetVisible(IsDir());
    m_cover.SetIsTrial(isTrial);
    m_txtTrialLabel.SetVisible(isTrial);
    bool isNew = file && !file->GetFileLastReadTime();
    m_imgNewBook.SetVisible(isNew);
    m_cover.SetIsNew(isNew);

    dkSize realMin = m_windowSizer->GetMinSize();
    int pageCount = file ? file->GetPageCount() : 0;
    int maxDot = PageCountToProgressBarDotCount(pageCount);
    int progress = file ? file->GetFilePlayProcess() : 0;
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
    
    if (m_windowSizer)
    {
        UISizerItem* fileNameItem = m_windowSizer->GetItem(&m_txtItemNameLabel, true);
        UISizerItem* authorItem = m_windowSizer->GetItem(&m_txtAuthorLabel, true);
        if (fileNameItem && authorItem)
        {
            fileNameItem->SetProportion(0);
            authorItem->SetProportion(0);
            DebugPrintf(DLC_GUI_VERBOSE,
                "UIDirectoryListItem::InitItem(%s), %d~%d, %d",
                itemShowName.c_str(), realMin.x, m_minWidth, m_iWidth);
                    
            if (realMin.x > m_minWidth)
            {
                int minAuthorWidth = GetWindowMetrics(
                        UIHomePageAuthorMinWidthIndex);
                int minNameWidth = GetWindowMetrics(
                        UIHomePageFileNameMinWidthIndex);
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
    switch(m_bookListMode)
    {
        case BLM_ICON:
            mainSizer->Hide(m_listViewSizer);
            if (NULL != file)
            {
                std::string coverFile = file->GetCoverImagePath();
                m_cover.SetCoverImage(coverFile);
            }
            else
            {
                bool isSpecialDir = 
                    PathManager::IsTopSpecialBookFolders(itemFullPath);
                m_cover.SetCoverImage(
                        ImageManager::GetImagePath(isSpecialDir 
                            ? IMAGE_ICON_COVER_DUOKAN_DIR
                            : IMAGE_ICON_COVER_DIR));
            }
            break;
        case BLM_LIST:
            mainSizer->Hide(&m_cover);
            break;
    }
}

bool UIDirectoryListItem::Init()
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

    m_readingProgressBar.SetBarHeight(GetWindowMetrics(UIDirectoryListItemProgressBarHeightIndex));
    m_readingProgressBar.SetMinWidth(GetWindowMetrics(UIDirectoryListItemProgressBarWidthIndex));
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

bool UIDirectoryListItem::IsDir() const
{
    return m_pItemModel->GetModelType() == IDirectoryItemModel::MT_DIRECTORY 
        || m_pItemModel->GetModelType() == IDirectoryItemModel::MT_CATEGORY;
}

void UIDirectoryListItem::SetBookListMode(BookListMode mode)
{
    //mode = BLM_ICON;
    m_bookListMode = mode;
    UISizer* mainSizer = GetSizer();
    if (NULL == mainSizer)
    {
        return;
    }
    switch (m_bookListMode)
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

HRESULT UIDirectoryListItem::Draw(DK_IMAGE drawingImg)
{
    switch (m_bookListMode)
    {
        case BLM_LIST:
            return DrawListView(drawingImg);
        case BLM_ICON:
            return DrawCoverView(drawingImg);
        default:
            return S_OK;
    }

}

HRESULT UIDirectoryListItem::DrawListView(DK_IMAGE drawingImg)
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

HRESULT UIDirectoryListItem::DrawCoverView(DK_IMAGE drawingImg)
{
	HRESULT hr(S_OK);
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    DK_IMAGE imgSelf;
    RTN_HR_IF_FAILED(CropImage(drawingImg, rcSelf, &imgSelf));
    return m_cover.Draw(imgSelf);
}

bool UIDirectoryListItem::IsSelected() const
{
    return m_pItemModel->IsSelected();
}

void UIDirectoryListItem::SetSelected(bool selected)
{
    m_pItemModel->SetSelected(selected);
    m_cover.SetSelected(selected);
    m_imgItem.SetImage(ImageManager::GetImage(
                    selected 
                    ? IMAGE_ICON_COVER_SELECTED 
                    : IMAGE_ICON_COVER_UNSELECTED));
    m_imgItem.SetMinSize(dkSize(GetWindowMetrics(UIHomePageImageMaxWidthIndex), m_imgItem.GetMinHeight()));
}

void UIDirectoryListItem::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
    return UIListItemBase::MoveWindow(iLeft, iTop, iWidth, iHeight);
}

void UIDirectoryListItem::SetIsLoading(bool isLoading)
{
    m_cover.SetIsLoading(isLoading);
}

int UIDirectoryListItem::PageCountToProgressBarDotCount(int pageCount)
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