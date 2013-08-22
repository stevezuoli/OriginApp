///////////////////////////////////////////////////////////////////////
// UIDirectoryListBox.cpp
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIDirectoryListBox.h"
#include "TouchAppUI/UIButtonDlg.h"
#include "CommonUI/UIWeiboSharePage.h"
#include "TouchAppUI/UIDeleteCategoryConfirmDlg.h"
#include "CommonUI/CPageNavigator.h"
#include "Model/DirectoryItemModelImpl.h"
#include "Framework/CDisplay.h"
#include "Framework/CNativeThread.h"
#include "SQM.h"
#include "TouchAppUI/BookOpenManager.h"
#include "Common/File_DK.h"
#include "../Common/FileManager_DK.h"
#include "Utility/FileSystem.h"
#include "I18n/StringManager.h"
#include "TouchAppUI/UIBookMenuDlg.h"
#include "GUI/UISizer.h"
#include "CommonUI/UIUtility.h"
#include "GUI/UIMessageBox.h"
#include "Utility/SystemUtil.h"
#include "Utility/PathManager.h"
#include <tr1/functional>
#include "Common/WindowsMetrics.h"
#include "Common/BookCoverLoader.h"
#include "BookStore/LocalCategoryManager.h"
#include <iostream>
#include "Utility/StringUtil.h"
#include "Utility/CharUtil.h"
#include "TouchAppUI/UIAddBookToCategoryPage.h"
#include "drivers/DeviceInfo.h"
#include "CommonUI/UIAddCategoryDlg.h"

using namespace dk::utility;
using namespace dk::bookstore;
using namespace WindowsMetrics;
using namespace std;


static const size_t ICON_ROWS = 2;
static const size_t ICON_COLS = 3;

static int COVER_ICON_WIDTH = 0;
static int COVER_ICON_HEIGHT = 0;
static int COVER_IMAGE_WIDTH = 0;
static int COVER_IMAGE_HEIGHT = 0;
static int COVER_HORIZONTAL_DELTA = 0;
static int COVER_VERTICAL_DELTA = 0;

const char* UIDirectoryListBox::EventChildSelectChanged = "EventChildSelectChanged";

void InitCoverMetrics()
{
    COVER_ICON_WIDTH = GetWindowMetrics(UIHomePageCoverViewItemWidthIndex);
    COVER_ICON_HEIGHT = GetWindowMetrics(UIHomePageCoverViewItemHeightIndex);
    COVER_IMAGE_WIDTH = GetWindowMetrics(UICoverViewItemInnerCoverWidthIndex);
    COVER_IMAGE_HEIGHT = GetWindowMetrics(UICoverViewItemInnerCoverHeightIndex);
    COVER_HORIZONTAL_DELTA = GetWindowMetrics(UIHomePageCoverViewHorzIntervalIndex);
    COVER_VERTICAL_DELTA = GetWindowMetrics(UIHomePageCoverViewVertIntervalIndex);
}

UIDirectoryListBox::UIDirectoryListBox(BookListUsage usage)
    : UICompoundListBox(NULL,IDSTATIC)
    , m_bIsDisposed(false)
    , m_totalPage(0)
    , m_curPage(0)
    , m_itemNum(0)
    , m_itemNumPerPage(0)
    , m_curPath(PathManager::GetRootPath())
    , m_bookListMode(BLM_LIST)
    , m_usage(usage)
{
    InitCoverMetrics();
    BookCoverLoader::GetInstance()->SetMinimumCoverSize(COVER_IMAGE_WIDTH, COVER_IMAGE_HEIGHT);
    m_iconSizer = new UIBoxSizer(dkVERTICAL);
    m_iconSizer->SetName("IconSizer");
    m_listSizer = new UIBoxSizer(dkVERTICAL);
    m_listSizer->SetName("ListSizer");
    m_windowSizer->Add(m_listSizer, UISizerFlags(1).Expand());
    m_windowSizer->SetName("UIDirectoryListBoxMainSizer");
    m_windowSizer->Add(m_iconSizer, UISizerFlags(1).Expand());
    m_windowSizer->Hide(m_iconSizer);
}


void UIDirectoryListBox::ClearListItem()
{
	//ClearAllChild(false);
    //ClearObjectInVector(m_directoryList);
    ClearObjectInVector(m_itemList);
    m_itemNum = 0;
    if (m_windowSizer)
    {
        m_iconSizer->Clear();
        m_listSizer->Clear();
        m_rowSizers.clear();
    }
}

UIDirectoryListBox::~UIDirectoryListBox()
{
    if (m_windowSizer)
    {
        // delete windowSizer before delete obj in m_btns
        // otherwise windowSizer will use wild pointer to obj in m_btns
        delete m_windowSizer;
        m_windowSizer = NULL;
    }
    ClearObjectInVector(m_directoryList);
    ClearListItem();
}

IDirectoryItemModel *UIDirectoryListBox::GetSelectedItemModel()
{
    return GetSelectedItemModel(m_iSelectedItem);
}

IDirectoryItemModel *UIDirectoryListBox::GetSelectedItemModel(int selectedItem)
{
    if (selectedItem < 0 || selectedItem >= m_iVisibleItemNum)
	{
		return NULL;
	}
    return m_itemList[m_curPage * m_itemNumPerPage+ selectedItem];
}

UIDirectoryListItem* UIDirectoryListBox::GetSelectedUIItem()
{
	if (m_iSelectedItem < 0 || m_iSelectedItem >= m_iVisibleItemNum)
	{
		return NULL;
	}
	return m_directoryList.at(m_iSelectedItem);
}

BOOL UIDirectoryListBox::OnKeyPress(INT32 iKeyCode)
{
    return UICompoundListBox::OnKeyPress(iKeyCode);
}

bool UIDirectoryListBox::SetCurrentPath(const char* path, const char* category)
{
    if (StringUtil::IsNullOrEmpty(path))
    {
        m_curPath.clear();
        m_category.clear();
    }
    else if (StringUtil::IsNullOrEmpty(category))
    {
        m_curPath = path;
        m_category.clear();
    }
    else
    {
        m_curPath = path;
        m_category = category;
    }
    m_curPage = 0;
    return true;
}

bool UIDirectoryListBox::OnItemClick(INT32 iSelectedItem)
{
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (iSelectedItem < 0 || iSelectedItem >= m_iVisibleItemNum)
    {
        DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s iSelectedItem out of range", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

#ifdef KINDLE_FOR_TOUCH
    UIDirectoryListItem* pWin = m_directoryList.at(iSelectedItem);
    if (pWin)
    {
        if (BLU_SELECT == m_usage)
        {
            pWin->SetSelected(!pWin->IsSelected());
            pWin->UpdateWindow();
            ChildSelectChangedArgs args(GetSelectedBookCount()); 
            FireEvent(EventChildSelectChanged, args);
            return true;
        }
        pWin->SetHighLight(true);
    }
#endif

    CDisplay::GetDisplay()->SetFullRepaint(true);
    m_curPageStack.push_back(m_curPage);
    int itemNo = m_curPage * m_itemNumPerPage+ iSelectedItem;

    if (m_itemList[itemNo]->GetModelType() != IDirectoryItemModel::MT_BOOK)
    {
        if (m_itemList[itemNo]->GetModelType() == IDirectoryItemModel::MT_DIRECTORY)
        {
            SetCurrentPath(m_itemList[itemNo]->GetFullPath().c_str());
        }
        else if (m_itemList[itemNo]->GetModelType() == IDirectoryItemModel::MT_CATEGORY)
        {

            SetCurrentPath(
                    m_itemList[itemNo]->GetItemPath().c_str(),
                    m_itemList[itemNo]->GetItemName().c_str());
        }
        {
            CDisplay::CacheDisabler disableCache;
            pWin->UpdateWindow();
            
            usleep(200000);
        }
        ClearListItem();
        InitListItem();
        UpdateListItem();
        ListItemClickEvent args(this, ListItemClickEvent::Directory);
        FireEvent(ListItemClick, args);
        Repaint();
    }
    else
    {
        CDKFileManager* pFileManager = CDKFileManager::GetFileManager();
        if(!pFileManager)
        {
            DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s GetFileManager is null", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return false;
        }

        string filePath = m_itemList[itemNo]->GetItemPath() + "/" + m_itemList[itemNo]->GetItemName();

        int iFileID = pFileManager->GetFileIdByPath(filePath.c_str());
        
        BookOpenManager::GetInstance()->Initialize(GetParent(), iFileID, 10, m_iTop + iSelectedItem * GetItemHeight() + 10);

        pWin->SetIsLoading(true);
        pWin->UpdateWindow();
        SNativeMessage msg;
        msg.iType = KMessageOpenBook;
        CNativeThread::Send(msg);
    }

    return true;
}


void UIDirectoryListBox::InitListItemFromSortedFiles()
{
    CDKFileManager* fileManager = CDKFileManager::GetFileManager();
    m_itemNum = fileManager->GetBookCurSortNum();
    m_totalPage = (m_itemNum + m_itemNumPerPage - 1) / m_itemNumPerPage;
    if (!m_totalPage)
    {
        m_totalPage = 1;
    }
    ClearObjectInVector(m_itemList);
    if (m_curPage >= m_totalPage)
    {
        m_curPage = m_totalPage - 1;
    }
    for (size_t i = 0; i < m_itemNum; ++i)
    {
        CDKFile* file = fileManager->GetBookbyListIndex(i);
        std::string fullPath = file->GetFilePath();
        std::string fileName = PathManager::GetFileName(fullPath.c_str());
        std::string pathWithoutName = fullPath.substr(0, fullPath.size() - fileName.size() - 1);
        CDirectoryItemModelImpl* item = new CDirectoryItemModelImpl(pathWithoutName.c_str(), fileName.c_str(), IDirectoryItemModel::MT_BOOK);
        m_itemList.push_back(item);
    }
}

static bool DirectoryItemCompare(IDirectoryItemModel* lhs, IDirectoryItemModel* rhs)
{
    if (lhs->GetModelType() != IDirectoryItemModel::MT_BOOK && rhs->GetModelType() == IDirectoryItemModel::MT_BOOK)
    {
        return true;
    }
    if (lhs->GetModelType() == IDirectoryItemModel::MT_BOOK && rhs->GetModelType() != IDirectoryItemModel::MT_BOOK)
    {
        return false;
    }
    return CharUtil::StringCompareByDisplay(lhs->GetItemGBKDisplayName().c_str(),
                rhs->GetItemGBKDisplayName().c_str()) < 0;
}

void UIDirectoryListBox::InitListItemFromBookStoreDirectory()
{
    if (HasCategory())
    {
        InitListItemFromBookStoreDirectoryWithCategory();
    }
    else
    {
        InitListItemFromBookStoreDirectoryDirect();
    }
}


void UIDirectoryListBox::InitListItemFromBookStoreDirectoryWithCategory()
{
    ClearObjectInVector(m_itemList);
    DK_AUTO(ids,
            LocalCategoryManager::GetBookIdsByCategory(m_category.c_str()));

    std::string fileName;
    std::string fullPath;
    for (DK_AUTO(cur, ids.begin()); cur != ids.end(); ++cur)
    {
        fileName = *cur + ".epub";
        fullPath = PathManager::ConcatPath(GetCurrentPath(), fileName);
        if (!PathManager::IsFileExisting(fullPath))
        {
            continue;
        }
        m_itemList.push_back(new CDirectoryItemModelImpl(GetCurrentPath().c_str(), fileName.c_str(), IDirectoryItemModel::MT_BOOK));
    }
    std::sort(m_itemList.begin(), m_itemList.end(), DirectoryItemCompare);
    m_itemNum = m_itemList.size();
    m_totalPage = (m_itemNum + m_itemNumPerPage - 1) / m_itemNumPerPage;
    if (0 == m_totalPage)
    {
        m_totalPage = 1;
    }
    if (m_curPage >= m_totalPage)
    {
        m_curPage = m_totalPage - 1;
    }
}

void UIDirectoryListBox::InitListItemFromBookStoreDirectoryDirect()
{
    ClearObjectInVector(m_itemList);

    if (BLU_BROWSE == m_usage)
    {
        vector<string> categories = LocalCategoryManager::GetAllCategories();
        for (size_t i = 0; i < categories.size(); ++i)
        {
            m_itemList.push_back(new CDirectoryItemModelImpl(GetCurrentPath().c_str(), categories[i].c_str(), IDirectoryItemModel::MT_CATEGORY));
        }
    }
    std::string fileName;
    vector<string> ids = LocalCategoryManager::GetBookIdsWithNoCategory();
    for (size_t i = 0; i < ids.size(); ++i)
    {
        fileName = ids[i] + ".epub"; 
        m_itemList.push_back(new CDirectoryItemModelImpl(GetCurrentPath().c_str(), fileName.c_str(), IDirectoryItemModel::MT_BOOK));
    }
    std::sort(m_itemList.begin(), m_itemList.end(), DirectoryItemCompare);

    m_itemNum = m_itemList.size();
    m_totalPage = (m_itemNum + m_itemNumPerPage - 1) / m_itemNumPerPage;
    if (0 == m_totalPage)
    {
        m_totalPage = 1;
    }
    if (m_curPage >= m_totalPage)
    {
        m_curPage = m_totalPage - 1;
    }
}

void UIDirectoryListBox::InitListItemFromRootDirectory()
{
    ClearObjectInVector(m_itemList);

    vector<string> vDirItems, vFileItems;
    vector<string> specialFolders = PathManager::GetTopSpecialBookFolders();
    for (size_t i = 0; i < specialFolders.size(); ++i)
    {
        m_itemList.push_back(new CDirectoryItemModelImpl(specialFolders[i].c_str(), IDirectoryItemModel::MT_DIRECTORY));
    }

    vector<string> normalFolders = PathManager::GetTopNormalBookFolders();
    vector<string> dirs;
    for (size_t i = 0; i < normalFolders.size(); ++i)
    {
        vector<string> curDirs = PathManager::GetDirsInPath(normalFolders[i].c_str());
        for (size_t j = 0; j < curDirs.size(); ++j)
        {
            if (PathManager::ShouldBlockPath(normalFolders[i], curDirs[j]))
            {
                continue;
            }
            m_itemList.push_back(new CDirectoryItemModelImpl(normalFolders[i].c_str(), curDirs[j].c_str(), IDirectoryItemModel::MT_DIRECTORY));
        }

        std::string curPath = PathManager::ConcatPath(m_curPath, PathManager::GetFileName(normalFolders[i]));
        std::vector<std::string> fileItems;
        GetBookFilesInDir(curPath, fileItems);
        for (size_t j = 0; j < fileItems.size(); ++j)
        {
            m_itemList.push_back(new CDirectoryItemModelImpl(curPath.c_str(), fileItems[j].c_str(), IDirectoryItemModel::MT_BOOK));
        }
    }
    std::sort(m_itemList.begin() + specialFolders.size(), m_itemList.end(), DirectoryItemCompare);
    m_itemNum = m_itemList.size();
    m_totalPage = (m_itemNum + m_itemNumPerPage - 1) / m_itemNumPerPage;
    if (0 == m_totalPage)
    {
        m_totalPage = 1;
    }
    if (m_curPage >= m_totalPage)
    {
        m_curPage = m_totalPage - 1;
    }
}

void UIDirectoryListBox::InitListItemFromDirectory()
{
    vector<string> vDirItems, vFileItems;
    const std::string& currentPath = GetCurrentPath();
    if(PathManager::IsRootPath(currentPath.c_str()))
    {
        InitListItemFromRootDirectory();
        return;
    }
    else if (PathManager::IsBookStorePath(currentPath.c_str()))
    {
        InitListItemFromBookStoreDirectory();
        return;
    }
    else
    {
        vDirItems = PathManager::GetDirsInPath(currentPath.c_str());
        GetBookFilesInDir(currentPath.c_str(), vFileItems);
    }
    
    ClearObjectInVector(m_itemList);

    CDirectoryItemModelImpl* pItem = NULL;
    for(size_t i = 0; i < vDirItems.size(); i++)
    {
        pItem = new CDirectoryItemModelImpl(currentPath.c_str(), vDirItems[i].c_str(), IDirectoryItemModel::MT_DIRECTORY);
        if(!pItem)
        {
            continue;
        }
        m_itemList.push_back(pItem);
    }


    for(UINT i = 0; i < vFileItems.size(); i++)
    {
        pItem = new CDirectoryItemModelImpl(currentPath.c_str(), vFileItems[i].c_str(), IDirectoryItemModel::MT_BOOK);
        if(!pItem)
        {
            continue;
        }
        m_itemList.push_back(pItem);
    }
    std::sort(m_itemList.begin(), m_itemList.end(), DirectoryItemCompare);
    m_itemNum = m_itemList.size();
    m_totalPage = (m_itemNum + m_itemNumPerPage - 1) / m_itemNumPerPage;
    if(!m_totalPage)
    {
        m_totalPage = 1;
    }
    
    m_curPage = (m_curPage >= (int)m_totalPage) ? ((int)m_totalPage) - 1 : m_curPage;
}

int UIDirectoryListBox::GetItemPerPageByBookListMode(BookListMode mode)
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

void UIDirectoryListBox::InitListItem()
{
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    ClearListItem();
    
    m_itemNumPerPage = GetItemPerPageByBookListMode(m_bookListMode);
    if (GetCurrentPath().empty())
    {
        m_curPageStack.clear();
        InitListItemFromSortedFiles();
    }
    else
    {
        InitListItemFromDirectory();
    }
}

bool UIDirectoryListBox::BackToUpperFolder()
{
    const std::string& currentPath = GetCurrentPath();
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s start %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, currentPath.c_str());
    UINT32 uSize = currentPath.size();
    if(!uSize)
    {
        DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s !uSize", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }
    
    if (HasCategory())
    {
        SetCurrentPath(currentPath.c_str());
    }
    else
    {
        string::size_type pos = currentPath.rfind('/');
        if(string::npos != pos)
        {
            string tmp = currentPath.substr(0, pos);
            std::vector<std::string> topNormalFolders = PathManager::GetTopNormalBookFolders();
            if(tmp.size() < PathManager::GetRootPath().size()
                    || std::find(topNormalFolders.begin(), topNormalFolders.end(), tmp) != topNormalFolders.end())
            {
                SetCurrentPath(PathManager::GetRootPath().c_str());
            }
            else
            {
                SetCurrentPath(tmp.c_str());
            }
        }
    }
	
    if (!m_curPageStack.empty())
    {
        m_curPage = m_curPageStack.back();
        m_curPageStack.pop_back();
    }
    else
    {
        m_curPage = 0;
    }
    InitListItem();
    UpdateListItem();
    //OnEvent(UIEvent(SIZE_EVENT, this, GetId()));
    Layout();
    Repaint();
    
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true; 
}


bool UIDirectoryListBox::UpdateListItem()
{
    switch (m_bookListMode)
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

bool UIDirectoryListBox::UpdateListItemForIcon()
{
    int iconWidth = COVER_ICON_WIDTH; //167;//130; //167; //GetWindowMetrics(UIDirectoryListIconWidthIndex);
    int iconHeight = COVER_ICON_HEIGHT; //226;//178;//226;//GetWindowMetrics(UIDirectoryListIconHeightIndex);
    int horzDelta = COVER_HORIZONTAL_DELTA;
    int vertDelta = COVER_VERTICAL_DELTA;
    int horzSpacing = horzDelta - iconWidth;
    int vertSpacing = vertDelta - iconHeight;
    DebugPrintf(DLC_GUI_VERBOSE, "UIDirectoryListItem::UpdateListItemForIcon, %d, %d",
            horzSpacing, vertSpacing);

    size_t viewItemIndex = 0;
    size_t startIndex = m_curPage * m_itemNumPerPage;
    size_t endIndex = std::min(startIndex + m_itemNumPerPage, m_itemNum);

    UIDirectoryListItem* pItem = NULL;
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
            if (viewItemIndex == m_directoryList.size())
            {
                pItem = new UIDirectoryListItem(this, m_usage, COVER_ICON_WIDTH, COVER_ICON_HEIGHT);
                m_directoryList.push_back(pItem);
            }
            else if (viewItemIndex < m_directoryList.size())
            {
                pItem = m_directoryList[viewItemIndex];
            }
            else
            {
                continue;
            }
            pItem->SetMinSize(iconWidth, iconHeight);
            pItem->SetVisible(true);
            pItem->SetBookListMode(m_bookListMode);
            pItem->SetItemModel(m_itemList[startIndex++]);
            rowSizer->Add(pItem);
            rowSizer->AddSpacer(horzSpacing);
        }
    }

    m_iVisibleItemNum = viewItemIndex; // in CURRENT PAGE, visiable item number;

    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s m_itemNumPerPage = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, m_itemNumPerPage);
    //disable rest of the UIITEM;
    int iUIItemNum = m_directoryList.size();
    int i = m_iVisibleItemNum;
    for (; i < iUIItemNum; i++)
    {
        pItem = m_directoryList.at(i);
        if (pItem)
            pItem->SetVisible(false);
    }
    return true;
}

bool UIDirectoryListBox::UpdateListItemForList()
{
    size_t iIndex = 0;
    int iNextPageIndex = (m_curPage + 1) == m_totalPage ? m_itemNum : (m_curPage + 1) * m_itemNumPerPage;
    int iLeftMargin = GetLeftMargin();
    int iHeight = GetItemHeight();
    int iWidth  = GetItemWidth() - (iLeftMargin << 1);

    UIDirectoryListItem* pItem = NULL;
    int i;
    for (i = m_curPage * m_itemNumPerPage; i < iNextPageIndex; i++)
    {
        if (iIndex == m_directoryList.size())
        {
            pItem = new UIDirectoryListItem(this, m_usage, COVER_ICON_WIDTH, COVER_ICON_HEIGHT);
            if(pItem)
            {
                //pItem->MoveWindow(iLeftMargin, iIndex * (iHeight + m_iItemSpacing), iWidth, iHeight);
                m_directoryList.push_back(pItem);
            }
            else
            {
                break;
            }
        }
        else if (iIndex < m_directoryList.size())
        {
            pItem = m_directoryList[iIndex];
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
        pItem->SetBookListMode(m_bookListMode);
        pItem->SetItemModel(m_itemList[i]);
        pItem->SetVisible(TRUE);
        iIndex++;
    }

    m_iVisibleItemNum = iIndex; // in CURRENT PAGE, visiable item number;

    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s m_itemNumPerPage = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, m_itemNumPerPage);
    //disable rest of the UIITEM;
    int iUIItemNum = m_directoryList.size();
    i = m_iVisibleItemNum;
    for (; i < iUIItemNum; i++)
    {
        pItem = m_directoryList.at(i);
        if (pItem)
            pItem->SetVisible(false);
    }
    return true;
}

void UIDirectoryListBox::UpdateListUI()
{
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    //Layout();
    //OnEvent(UIEvent(SIZE_EVENT, this, GetId()));
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

bool UIDirectoryListBox::PageDown()
{
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    if(0 == m_itemNum )
    {
        //SetBookList(NULL, 0);
        DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s m_itemNum is zero", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    m_curPage++;

    if (m_curPage < 0)
    {
        m_curPage = m_totalPage - 1;
    }
    else if (m_curPage == (int)m_totalPage)
    {
        m_curPage = 0;
    }

    UpdateListItem();
    //OnEvent(UIEvent(SIZE_EVENT, this, GetId()));
    Layout();
    Repaint();

    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

bool UIDirectoryListBox::PageUp()
{
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    if(0 == m_itemNum )
    {
        DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s m_itemNum is zero", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    m_curPage--;

    if (m_curPage < 0)
    {
        m_curPage = m_totalPage - 1;
    }
    else if (m_curPage == (int)m_totalPage)
    {
        m_curPage = 0;
    }

    UpdateListItem();
    //OnEvent(UIEvent(SIZE_EVENT, this, GetId()));
    Layout();
    Repaint();

    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

#ifdef KINDLE_FOR_TOUCH
bool UIDirectoryListBox::DoHandleListTurnPage(bool fKeyUp)
{
    return fKeyUp ? PageUp() : PageDown();
}

bool UIDirectoryListBox::ShouldShowContextMenuForItem(IDirectoryItemModel* item) const
{
     return NULL != item && 
         (item->GetModelType() != IDirectoryItemModel::MT_DIRECTORY
          || !PathManager::IsTopSpecialBookFolders(item->GetFullPath().c_str()));
}

BOOL UIDirectoryListBox::HandleLongTap(INT32 selectedItem)
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

    IDirectoryItemModel* item = GetSelectedItemModel(selectedItem);
    bool showMenu = ShouldShowContextMenuForItem(item);;

    if (showMenu)
    {
    	UIDirectoryListItem* pWin = m_directoryList.at(selectedItem);
		if (pWin)
        {
            pWin->SetHighLight(true);
	        m_iSelectedItem = selectedItem;
	        UICompoundListBox::OnItemClick(selectedItem);
	        LongTap();
			pWin->SetHighLight(false);
		}
    }
    return true;
}
#endif

bool UIDirectoryListBox::LongTap()
{
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    IDirectoryItemModel* pItemModel = GetSelectedItemModel();
    if (pItemModel)
    {
    	CDisplay* pDisplay = CDisplay::GetDisplay();
        UIBookMenuDlg::BookMenuType type = UIBookMenuDlg::BMT_BOOK;
        
		if(pItemModel->GetModelType() == IDirectoryItemModel::MT_DIRECTORY)
		{
            type = UIBookMenuDlg::BMT_FOLDER;
		}
        else if (pItemModel->GetModelType() == IDirectoryItemModel::MT_CATEGORY)
        {
            type = UIBookMenuDlg::BMT_DUOKAN_CATEGORY;
        }
        else if (IsDuoKanBookStoreBook(pItemModel->GetFullPath().c_str()) && !m_category.empty())
        {
            type = UIBookMenuDlg::BMT_DUOKAN_BOOK;
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
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

void UIDirectoryListBox::OnRenameFolder()
{
    if (m_pParent)
    {
        IDirectoryItemModel* pSelectedItem = GetSelectedItemModel();
        if(pSelectedItem)
        {
            UIAddCategoryDlg dlg(this, UIAddCategoryDlg::CAT_RENAME);
            if (IDOK == dlg.DoModal())
            {
                string sPath(pSelectedItem->GetItemPath());
                if (!sPath.empty())
                {
                    sPath.append("/");
                    sPath.append(pSelectedItem->GetItemName());
                    std::string newName = dlg.GetInputText();
                    std::string errorMessage;
                    if (!RenameDirectoryOnUI(sPath, newName, errorMessage))
                    {
                        UIMessageBox msgBox(this,
                            errorMessage.c_str(),
                            MB_OK);
                        msgBox.DoModal();
                    }
                    else
                    {
                        CDKFileManager* fileManager = CDKFileManager::GetFileManager();
                        fileManager->FireFileListChangedEvent();
                    }
                }
            }
        }
    }
}

void UIDirectoryListBox::OnDelete()
{
    DebugPrintf(DLC_UIBOOKLISTBOX, "%s, %d, %s, %s ID_BTN_DELETE", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_pParent)
    {
        IDirectoryItemModel* pSelectedItem = GetSelectedItemModel();
        if(pSelectedItem)
        {
            UIMessageBox messagebox(m_pParent,
                    StringManager::GetStringById(pSelectedItem->GetModelType() == IDirectoryItemModel::MT_DIRECTORY ? SURE_DELETE_DIRECTORY : SURE_DELETE_BOOK),
                    MB_OK | MB_CANCEL);
            if(MB_OK == messagebox.DoModal())
            {
                string sPath(pSelectedItem->GetItemPath());
                sPath.append("/");
                sPath.append(pSelectedItem->GetItemName());

                if(pSelectedItem->GetModelType() != IDirectoryItemModel::MT_DIRECTORY)
                {
                    DeleteFileOnUI(sPath);
                }
                else
                {
                    DeleteDirectoryOnUI(sPath);
                }
            }
        }
        //Layout();
        //m_pParent->UpdateWindow();
    }
}

void UIDirectoryListBox::OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam)
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
                IDirectoryItemModel* pSelectedItem = GetSelectedItemModel();
                if(pSelectedItem)
                {
                    string displayText = StringManager::GetPCSTRById(ELEGANT_READING_TIME);
                    displayText.append(StringManager::GetPCSTRById(SHAREWEIBO_HINT));
                    char buf[1024] = {0};
                    string sPath(pSelectedItem->GetItemPath());
                    sPath.append("/");
                    sPath.append(pSelectedItem->GetItemName());
                    CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
                    if(!pFileManager)
                    {
                        return;
                    }

                    int fileID = pFileManager->GetFileIdByPath(sPath.c_str());
                    if(-1 == fileID)
                    {
                        return;
                    }

                    PCDKFile file = pFileManager->GetFileById(fileID);
                    if(!file)
                    {
                        return;
                    }
                    file->SyncFile();
                    snprintf(buf, sizeof(buf)/sizeof(char), displayText.c_str(), file->GetFileName());
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
            IDirectoryItemModel* pSelectedItem = GetSelectedItemModel();
            if (NULL != pSelectedItem)
            {
                UIAddBookToCategoryPage* addBookPage(new UIAddBookToCategoryPage(pSelectedItem->GetItemName().c_str()));
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
            IDirectoryItemModel* pSelectedItem = GetSelectedItemModel();
            if (NULL != pSelectedItem && !m_category.empty())
            {
                LocalCategoryManager::RemoveBookFromCategory(
                        m_category.c_str(),
                        PathManager::GetFileNameWithoutExt(
                            pSelectedItem->GetItemName().c_str()).c_str());
                InitListItem();
                UpdateListItem();
                Layout();
                UpdateWindow();
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

void UIDirectoryListBox::OnRenameCategory()
{
    IDirectoryItemModel* pSelectedItem = GetSelectedItemModel();
    if (NULL == pSelectedItem 
            || !PathManager::IsBookStorePath(m_curPath.c_str()))
    {
        return;
    }
    string category = pSelectedItem->GetItemName();
    UIAddCategoryDlg dlg(this, UIAddCategoryDlg::CAT_RENAME);
    if (IDOK == dlg.DoModal())
    {
        std::string newName = dlg.GetInputText();
        if (!category.empty())
        {
            std::string errorMessage;
            if (!LocalCategoryManager::RenameCategory(category.c_str(), newName.c_str(), &errorMessage))
            {
                UIMessageBox msgBox(this,
                        errorMessage.c_str(),
                        MB_OK);
                msgBox.DoModal();
            }
            else
            {
                CDKFileManager* fileManager = CDKFileManager::GetFileManager();
                fileManager->FireFileListChangedEvent();
            }
        }
    }
}

void UIDirectoryListBox::OnDeleteCategory()
{
    UIDeleteCategoryConfirmDlg dlg(this);
    if (IDOK == dlg.DoModal())
    {
        IDirectoryItemModel* pSelectedItem = GetSelectedItemModel();
        if (NULL == pSelectedItem 
                || !PathManager::IsBookStorePath(m_curPath.c_str()))
        {
            return;
        }
        string category = pSelectedItem->GetItemName();
        CDKFileManager* fileManager = CDKFileManager::GetFileManager();
        if (dlg.ShouldDeleteBooks())
        {
            DK_AUTO(ids, LocalCategoryManager::GetBookIdsByCategory(category));
            for (DK_AUTO(id, ids.begin()); id != ids.end(); ++id)
            {
                std::string path = PathManager::BookIdToEpubFile(*id);
                fileManager->DelFile(path);
            }
        }
        LocalCategoryManager::RemoveCategory(category.c_str());
        fileManager->FireFileListChangedEvent();
    }
}

void UIDirectoryListBox::DeleteFileOnUI(const std::string& filepath)
{
    if (!m_category.empty())
    {
        LocalCategoryManager::RemoveBookFromCategory(
                m_category.c_str(),
                PathManager::GetFileNameWithoutExt(filepath.c_str()).c_str());

    }
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CDKFileManager* pFileManager = CDKFileManager::GetFileManager();
    if(NULL == pFileManager)
    {
        return;
    }

    if (!pFileManager->DelFile(filepath))
    {
        return;
    }
    IDirectoryItemModel* pItem = NULL;
    for(size_t i = 0; i < m_itemList.size(); i++)
    {
        pItem = m_itemList[i];
        if(!pItem)
        {
            continue;
        }
        std::string itemPath = PathManager::ConcatPath(pItem->GetItemPath().c_str(), pItem->GetItemName().c_str());
        if(itemPath == filepath)
        {
            SafeDeletePointerEx(pItem);
            m_itemList.erase(m_itemList.begin() + i);
            break;
        }
    }
    pFileManager->FireFileListChangedEvent();
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

bool UIDirectoryListBox::RenameDirectoryOnUI(const std::string& dirpath, const std::string& newName, std::string& errMessage)
{
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s start:%s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, dirpath.c_str());

    CDKFileManager* pFileManager = CDKFileManager::GetFileManager();
    if(NULL == pFileManager)
    {
        return false;
    }
    if (newName.empty())
    {
        return false;
    }
    IDirectoryItemModel* pItem = NULL;
    for(size_t i = 0; i < m_itemList.size(); i++)
    {
        pItem = m_itemList[i];
        if(!pItem)
        {
            continue;
        }
        DebugPrintf(DLC_UIDIRECTORYLISTBOX, "dirpath : %s and pItem->GetItemPath().append(pItem->GetItemName()) : %s\n",dirpath.c_str(),(pItem->GetItemPath().append("/").append(pItem->GetItemName())).c_str());
        std::string itemPath = pItem->GetItemPath().append("/").append(pItem->GetItemName());
        if(itemPath == dirpath)
        {
            std::string itemNewPath = pItem->GetItemPath().append("/").append(newName);
            for(size_t j = 0; j < m_itemList.size(); j++)
            {
                if (i == j)
                {
                    continue;
                }
                IDirectoryItemModel* pCmpItem = m_itemList[j];
                std::string compItemPath = pCmpItem->GetItemPath().append("/").append(pCmpItem->GetItemName());
                if (compItemPath == itemNewPath)
                {
                    errMessage = StringManager::GetPCSTRById(CATEGORY_EXISTING);
                    return false;
                }
            }

            char sztmp[1024] = {0};    
            sprintf(sztmp,"mv \"%s\" \"%s\" ", dirpath.c_str(), itemNewPath.c_str());
            SystemUtil::ExecuteCommand(sztmp);
            pItem->SetItemName(newName);
            pFileManager->ReLoadFileToFileManger(SystemSettingInfo::GetInstance()->GetMTDPathLP());
            break;
        }
    }
    pFileManager->FireFileListChangedEvent();
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

void UIDirectoryListBox::DeleteDirectoryOnUI(const std::string& dirpath)
{
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s start:%s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, dirpath.c_str());

    CDKFileManager* pFileManager = CDKFileManager::GetFileManager();
    if(NULL == pFileManager)
    {
        return;
    }
    char sztmp[1024] = {0};
    sprintf(sztmp,"rm \"%s\" -rf",dirpath.c_str());
    SystemUtil::ExecuteCommand(sztmp);
    pFileManager->ReLoadFileToFileManger(SystemSettingInfo::GetInstance()->GetMTDPathLP());
    IDirectoryItemModel* pItem = NULL;
    for(size_t i = 0; i < m_itemList.size(); i++)
    {
        pItem = m_itemList[i];
        if(!pItem)
        {
            continue;
        }
        DebugPrintf(DLC_UIDIRECTORYLISTBOX, "dirpath : %s and pItem->GetItemPath().append(pItem->GetItemName()) : %s\n",dirpath.c_str(),(pItem->GetItemPath().append("/").append(pItem->GetItemName())).c_str());
        if(pItem->GetItemPath().append("/").append(pItem->GetItemName()) == dirpath)
        {
            SafeDeletePointerEx(pItem);
            m_itemList.erase(m_itemList.begin() + i);
            break;
        }
    }
    pFileManager->FireFileListChangedEvent();
    DebugPrintf(DLC_UIDIRECTORYLISTBOX, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UIDirectoryListBox::SetBookListMode(BookListMode bookListMode)
{
    int startItemIndex = m_curPage * m_itemNumPerPage;
    int newItemPerPage = GetItemPerPageByBookListMode(bookListMode);
    m_curPage = startItemIndex  / newItemPerPage;
    m_bookListMode = bookListMode;
    if (BLM_LIST == m_bookListMode)
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

bool UIDirectoryListBox::HasCategory() const
{
    return !m_category.empty();
}

size_t UIDirectoryListBox::GetSelectedBookCount() const
{
    size_t selected = 0;
    for (DK_AUTO(cur, m_itemList.begin());
            cur != m_itemList.end();
            ++cur)
    {
        if ((*cur)->IsSelected())
        {
            ++selected;
        }
    }
    return selected;
}

std::vector<std::string> UIDirectoryListBox::GetSelectedBookIds() const
{
    std::vector<std::string> results;
    for (DK_AUTO(cur, m_itemList.begin());
            cur != m_itemList.end();
            ++cur)
    {
        if ((*cur)->IsSelected())
        {
            results.push_back(
                    PathManager::GetFileNameWithoutExt(
                        (*cur)->GetItemName().c_str()));
        }
    }
    return results;
}
