////////////////////////////////////////////////////////////////////////
// UIHomePage.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <tr1/functional>
#include "TouchAppUI/UIHomePage.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/CPageNavigator.h"
#include "TouchAppUI/UIDownloadPage.h"
#include "TouchAppUI/UIWifiDialog.h"
#include "TouchAppUI/UISingleTextInputDialog.h"
#include "TouchAppUI/UIDictionaryDialog.h"
#include "TouchAppUI/BookOpenManager.h"
#include "TouchAppUI/UIButtonDlg.h"
#include "TouchAppUI/UITouchMenu.h"
#include "Framework/CDisplay.h"
#include "Framework/CHourglass.h"
#include "GUI/UIMenu.h"
#include "GUI/UIMessageBox.h"
#include "GUI/GUISystem.h"
#include "Public/Base/TPDef.h"
#include "../Common/FileManager_DK.h"
#include "Wifi/WifiManager.h"
#include "I18n/StringManager.h"
#include "SQM.h"
#include "interface.h"
#include "Utility/Dictionary.h"
#include "Utility/FileSystem.h"
#include "FtpService/FtpService.h"
#include "drivers/DeviceInfo.h"
#include "KernelDef.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Utility/PathManager.h"
#include "Utility/StringUtil.h"
#include "BookStore/LocalCategoryManager.h"
#include "TouchAppUI/UIAddBookToCategoryPage.h"
#include "CommonUI/UIAddCategoryDlg.h"
#include "GUI/UITextBoxOnlyDlg.h"

#define USE_COVERVIEW 1

using namespace WindowsMetrics;
using namespace dk::utility;
using namespace dk::bookstore;

UIHomePage::UIHomePage(HomePageStyle Style)
    : UIPage()
//    , m_btnSearch(this, ID_BTN_TOUCH_SEARCH)
    , m_btnSort(this, ID_BTN_SORT)
    , m_lstDir(BLU_BROWSE)
    , m_btnUpperFolder(this)
    , m_bIsDisposed(FALSE)
    , m_pageStyle(Style)
    , m_FileSorts(UnknowSort)
    , m_topButtonSizer(NULL)
    , m_topUpLevelSizer(NULL)
    , m_titleLeftSizer(NULL)
    , m_titleRightSizer(NULL)
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
#if USE_COVERVIEW
    m_bookListMode = SystemSettingInfo::GetInstance()->GetBookListMode();
#else
    m_bookListMode = BLM_LIST;
#endif
    m_lstDir.SetBookListMode(m_bookListMode);
    Init();
}

UIHomePage::~UIHomePage()
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    Dispose();
}

void UIHomePage::Dispose()
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UIPage::Dispose();
    OnDispose(m_bIsDisposed);
}

void UIHomePage::OnDispose(BOOL bIsDisposed)
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (bIsDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;
    Finalize();
}

CString UIHomePage::GetSortString()
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CString strSortText;

    CDKFileManager* pFileManager = CDKFileManager::GetFileManager();
    switch(pFileManager->GetBookSort())
    {
        case RecentlyAdd:            //最近加入
            strSortText = StringManager::GetStringById(RECENT_ADDED);
            break;
        case RecentlyReadTime:      //最近阅读
            strSortText = StringManager::GetStringById(RECENT_READING);
            break;
        case Name:                   //书名关键字
            strSortText = StringManager::GetStringById(BY_BOOK_TITLE);
            break;
        case DIRECTORY:                     //目录
            strSortText = StringManager::GetStringById(BY_BOOK_DIRECTORY);
            break;
        case UnknowSort:          //获取某一类型文件 比如书（图）、音、视频
        default:
            break;
    }

    return strSortText;
}

void UIHomePage::MoveWindow(INT32 _iLeft, INT32 _iTop, INT32 _iWidth, INT32 _iHeight)
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UIWindow::MoveWindow(_iLeft, _iTop, _iWidth, _iHeight);

    m_lstDir.SetItemWidth(m_iWidth);

    if (!m_windowSizer)
    {
        const int horizonMargin = GetWindowMetrics(UIHorizonMarginIndex);
        SetSize(dkSize(m_iWidth, m_iHeight));
        UIBoxSizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        mainSizer->SetName("UIHomeMain");
        m_topButtonSizer = new UIBoxSizer(dkHORIZONTAL);
        m_topButtonSizer->SetName("UIHomeTopButton");
        UpdateViewModeImage();
        int imgSearchWidth = ImageManager::GetImage(IMAGE_TOUCH_ICON_SEARCH)->GetWidth();
        int searchBtnWidth = m_btnSearch.GetMinWidth();
        int btnLeft = horizonMargin - (searchBtnWidth - imgSearchWidth) / 2 - GetWindowMetrics(UIDefaultInternalBorderIndex);;
        m_topButtonSizer->AddSpacer(btnLeft);
        m_topButtonSizer->Add(&m_topLeftButtonGroup, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        m_topButtonSizer->AddStretchSpacer();
        m_topButtonSizer->Add(&m_btnSort, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        m_topButtonSizer->AddSpacer(horizonMargin);
        m_txtHint.SetText(StringManager::GetPCSTRById(CATEGORY_ADD_FILE_HINT));
        m_txtHint.SetVisible(false);
        m_txtHint.SetMinWidth(m_iWidth - 2 * horizonMargin);
        AppendChild(&m_txtHint);
        m_topUpLevelSizer = new UIBoxSizer(dkHORIZONTAL);
        m_topUpLevelSizer->SetName("UIHomePageTopUpLevel");
        m_topUpLevelSizer->Add(&m_btnUpperFolder, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));

        m_titleSizer = new UIBoxSizer(dkHORIZONTAL);
        m_topUpLevelSizer->Add(m_titleSizer, UISizerFlags(1).Expand());
        m_titleLeftSizer = m_titleSizer->AddSpacer(0);

        m_titleSizer->Add(&m_txtUpperFolder, UISizerFlags(1).Expand().Align(dkALIGN_CENTER));
        m_titleRightSizer = m_titleSizer->AddSpacer(0);
        m_titleSizer->Add(&m_btnNewCategory, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        m_searchBox.SetMinHeight(GetWindowMetrics(UIHomePageSearchDlgHeightIndex));
        m_searchBox.SetId(ID_EDIT_SEARCH_BOOK);
        m_searchBox.SetImage(IMAGE_ICON_SEARCH_DISABLE, IMAGE_ICON_SEARCH);
        if (!m_searchKeyword.empty())
        {
            m_searchBox.SetTextUTF8(m_searchKeyword.c_str());
        }
        m_topUpLevelSizer->Add(&m_searchBox, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        m_topUpLevelSizer->AddSpacer(horizonMargin);
        UIBoxSizer* bottomSizer = new UIBoxSizer(dkHORIZONTAL);
        bottomSizer->SetName("UIHomePageBottomSizer");
        m_txtTotalBook.SetTopMargin(0);
        m_txtPageNo.SetTopMargin(0);
        bottomSizer->Add(&m_txtTotalBook, UISizerFlags(1));
        bottomSizer->Add(&m_txtPageNo, UISizerFlags(1));

        mainSizer->Add(&m_titleBar);
        m_topButtonSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));
        m_topUpLevelSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));
        mainSizer->Add(m_topButtonSizer, UISizerFlags().Expand());
        mainSizer->Add(m_topUpLevelSizer, UISizerFlags().Expand());
        //mainSizer->AddSpacer(GetWindowMetrics(UIHomePageSearchBoxBottomMarginIndex));
        mainSizer->Add(&m_txtHint, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizonMargin).Align(dkALIGN_CENTER_VERTICAL));
        mainSizer->Add(&m_lstDir, UISizerFlags(1).Expand());
        mainSizer->Add(bottomSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizonMargin));
        mainSizer->AddSpacer(GetWindowMetrics(UIHomePageSpaceOverBottomBarIndex));
        mainSizer->Add(m_pBottomBar);
        SetSizer(mainSizer);
        RefreshUI();
    }

    if (SEARCHPAGE == m_pageStyle)
    {
        m_lstDir.SetCurrentPath("");
        m_lstDir.InitListItem();
    }
}

HRESULT UIHomePage::Init()
{
    HRESULT hr(S_OK);
   
    string strSearch(StringManager::GetPCSTRById(TOUCH_FIRST_TITLE_KEY_WORD));

    m_btnSort.Initialize(ID_BTN_SORT, NULL, GetWindowFontSize(UIHomePageSortTypeButtonIndex));
    m_btnSort.SetIcon(ImageManager::GetImage(IMAGE_ICON_TOUCH_MENU_ARROW), UIButton::ICON_RIGHT);
    m_btnSort.SetAlign(ALIGN_RIGHT);
    m_btnSort.ShowBorder(false);
    m_btnSort.SetUpdateOnPress(false);
    m_btnSort.SetUpdateOnUnPress(false);
    m_btnSort.SetLeftMargin(0);

    m_btnNewCategory.SetFontSize(GetWindowFontSize(UIHomePageSortTypeButtonIndex));
    m_btnNewCategory.ShowBorder(false);
    m_btnNewCategory.SetText(StringManager::GetPCSTRById(CATEGORY_NEW));
    DK_AUTO(imgAdd, ImageManager::GetImage(IMAGE_KPW_ICON_FRONTLIGHT_ADD));
    m_btnNewCategory.SetIcons(imgAdd, imgAdd, UIButton::ICON_LEFT);
    m_btnNewCategory.SetLeftMargin(0);

    m_btnUpperFolder.SetId(ID_BTN_TOUCH_UPPERFOLDER);
    //m_btnUpperFolder.Initialize(ID_BTN_TOUCH_UPPERFOLDER, StringManager::GetPCSTRById(TOUCH_UPPER_FLODER), GetWindowFontSize(FontSize20Index));
    m_txtUpperFolder.SetFontSize(GetWindowFontSize(UIHomePageUpperFolderIndex));
    m_txtUpperFolder.SetTransparent(FALSE);
    m_txtUpperFolder.SetAlign(ALIGN_CENTER);
        
    m_txtTotalBook.SetFontSize(GetWindowFontSize(UIHomePageSortTypeButtonIndex));
	m_txtTotalBook.SetEnglishGothic();
    m_txtTotalBook.SetAlign(ALIGN_LEFT);
    m_txtTotalBook.SetLeftMargin(0);

    m_txtPageNo.SetFontSize(GetWindowFontSize(UIHomePageSortTypeButtonIndex));
	m_txtPageNo.SetEnglishGothic();
    m_txtPageNo.SetAlign(ALIGN_RIGHT);
    m_txtPageNo.SetLeftMargin(0);
    
    if (m_pBottomBar)
    {
        m_pBottomBar->SetFocusedIndex(PAT_BookShelf);
    }

    const int listItemHeight = GetWindowMetrics(UIHomePageListItemHeightIndex);
    const int listItemSpacing = GetWindowMetrics(UIHomePageListItemSpacingIndex);
    const int horizonMargin = GetWindowMetrics(UIHorizonMarginIndex);
    m_lstDir.SetLeftMargin(horizonMargin);
    m_lstDir.SetItemHeight(listItemHeight);
    m_lstDir.SetItemSpacing(listItemSpacing);

    SPtr<ITpImage> searchImage = ImageManager::GetImage(IMAGE_TOUCH_ICON_SEARCH);
    m_btnSearch.SetIcon(searchImage, UIButton::ICON_CENTER);
    m_btnSearch.SetFontSize(GetWindowFontSize(UIHomePageSortTypeButtonIndex));
    m_btnSearch.SetAlign(ALIGN_LEFT);
    m_btnSearch.ShowBorder(false);
    m_btnSearch.SetLeftMargin(0);

    m_btnViewMode.SetFontSize(GetWindowFontSize(UIHomePageSortTypeButtonIndex));
    m_btnViewMode.ShowBorder(false);
    m_btnViewMode.SetAlign(ALIGN_RIGHT);
    m_btnViewMode.SetLeftMargin(0);

    int searchButtonWidth = GetWindowMetrics(UIHomePageSearchButtonWidthIndex);
    int searchButtonHeight = GetWindowMetrics(UIHomePageSearchButtonHeightIndex);


    m_btnSearch.SetMinSize(searchButtonWidth, searchButtonHeight);
    m_btnViewMode.SetMinSize(searchButtonWidth, searchButtonHeight);
    m_topLeftButtonGroup.SetSplitLineHeight(searchButtonHeight/2);
    m_topLeftButtonGroup.SetTopLinePixel(0);
    m_topLeftButtonGroup.SetBottomLinePixel(0);
    m_topLeftButtonGroup.AddButton(&m_btnSearch);
    m_topLeftButtonGroup.AddButton(&m_btnViewMode);
    m_btnSort.SetMinHeight(searchButtonHeight);

    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s AppendChild", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    AppendChild(&m_btnSort);
    AppendChild(&m_btnNewCategory);
    AppendChild(&m_btnUpperFolder);
    AppendChild(&m_txtUpperFolder);
    AppendChild(&m_lstDir);
    AppendChild(&m_txtTotalBook);
    AppendChild(&m_txtPageNo);
    AppendChild(m_pBottomBar);
    AppendChild(&m_titleBar);
    AppendChild(&m_topLeftButtonGroup);
    AppendChild(&m_searchBox);


    CONNECT(m_lstDir, UICompoundListBox::ListItemClick, this, UIHomePage::OnListItemClick)
    CONNECT(m_lstDir, UICompoundListBox::ListTurnPage, this, UIHomePage::OnListContentChanged)
    SubscribeMessageEvent(
            CDKFileManager::EventFileListChanged,
            *CDKFileManager::GetFileManager(),
            std::tr1::bind(
                std::tr1::mem_fn(&UIHomePage::OnFileListChanged),
                this,
                std::tr1::placeholders::_1));

    return hr;
}

void UIHomePage::Finalize()
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
} 

BOOL UIHomePage::OnKeyPress(INT32 iKeyCode)
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return UIWindow::OnKeyPress(iKeyCode);
}

bool UIHomePage::OnSortClick()
{
    CDKFileManager* pFileManager = CDKFileManager::GetFileManager();
    UITouchMenu menu(this);
    std::vector<std::string> buttonTitles;
    const char* titles[] = 
    {
        StringManager::GetPCSTRById(RECENT_READING),
        StringManager::GetPCSTRById(BY_BOOK_DIRECTORY),
        StringManager::GetPCSTRById(RECENT_ADDED),
        StringManager::GetPCSTRById(BY_BOOK_TITLE)
    };
    int sortTypes[] = {RecentlyReadTime, DIRECTORY, RecentlyAdd, Name};
    int sqmCounters[] = {
        SQM_ACTION_HOMEPAGE_SORT_READ_RECENT, 
        SQM_ACTION_HOMEPAGE_SORT_DIRECTORY, 
        SQM_ACTION_HOMEPAGE_SORT_ADDED_RECENT, 
        SQM_ACTION_HOMEPAGE_SORT_BOOKNAME 
    };
    int fontSize = GetWindowFontSize(FontSize24Index);

    int selected = 0;
    for (size_t i = 0; i < DK_DIM(sortTypes); ++i)
    {
        if (pFileManager->GetBookSort() == sortTypes[i])
        {
            selected = i;
            break;
        }
    }

    for (size_t i = 0; i < DK_DIM(sortTypes); ++i)
    {
        menu.AppendButton(sortTypes[i], titles[i], fontSize);
    }
    menu.SetCheckedButton(selected);
    int top = m_btnSort.GetBottom();

    dkSize menuSize = menu.GetMinSize();
    menu.MoveWindow(m_btnSort.GetX() + m_btnSort.GetWidth() - menuSize.x,
            top,
            menuSize.x,
            menuSize.y);
    menu.Layout();
    if (IDOK == menu.DoModal())
    {
        DK_FileSorts fileSort = (DK_FileSorts)menu.GetCommandId();
        if (fileSort != pFileManager->GetBookSort())
        {
            int newSelected = -1;
            for (size_t j = 0; j < DK_DIM(sqmCounters); ++j)
            {
                if (sortTypes[j] == fileSort)
                {
                    newSelected = j;
                    break;
                }
            }
            if (-1 == newSelected)
            {
                return true;
            }
            SQM::GetInstance()->IncCounter(sqmCounters[newSelected]);
            pFileManager->SetBookSortType(fileSort);
            pFileManager->SortFile(DFC_Book);
            RefreshUI();
        }
        if(m_pageStyle == SEARCHPAGE)
        {
            assert(false);
            CPageNavigator::BackToHome();
        }
    }
    return true;
}

bool UIHomePage::OnUpperFolderClick()
{
    if (SEARCHPAGE == m_pageStyle)
    {
        CPageNavigator::BackToPrePage();
        return true;
    }
    if (DIRECTORY == m_FileSorts)
    {
        m_lstDir.BackToUpperFolder();
        RefreshUI();
    }
    return true;
}

bool UIHomePage::OnSearchClick()
{
    UITextBoxOnlyDlg keywordDialog(this);
    string strSearch(StringManager::GetPCSTRById(TOUCH_FIRST_TITLE_KEY_WORD));
    keywordDialog.SetTipUTF8(strSearch.c_str());
    keywordDialog.SetTextBoxStyle(ENGLISH_NORMAL);

    int left = m_btnSearch.GetAbsoluteX() + m_btnSearch.GetWidth();
    const int width = m_btnSort.GetAbsoluteX() + m_btnSort.GetWidth() - left;
    const int height = GetWindowMetrics(UIHomePageSearchDlgHeightIndex);
    int top = m_btnSearch.GetAbsoluteY() + (m_btnSearch.GetHeight() - height) / 2;

    keywordDialog.MoveWindow(left, top, width, height);
    keywordDialog.Layout();
    if (IDOK == keywordDialog.DoModal())
    {
        std::string keyword = keywordDialog.GetTextUTF8();
        if (!keyword.empty())
        {
            CDKFileManager* fileManager = CDKFileManager::GetFileManager();
            fileManager->SearchBook(keyword.c_str()); 
            if (SEARCHPAGE != m_pageStyle)
            {
                UIHomePage* page = new UIHomePage(SEARCHPAGE);
                page->SetSearchKeyword(keyword);
                page->MoveWindow(0, 0,
                        DeviceInfo::GetDisplayScreenWidth(),
                        DeviceInfo::GetDisplayScreenHeight());
                CPageNavigator::Goto(page);
            }
        }
    }
    return true;
}

void UIHomePage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    if (dwCmdId == ID_EDIT_SEARCH_BOOK)
    {
        std::string keyword = m_searchBox.GetTextUTF8();
        if (!keyword.empty())
        {
            CDKFileManager* fileManager = CDKFileManager::GetFileManager();
            fileManager->SearchBook(keyword.c_str()); 
            RefreshUI();
            UpdateWindow();
        }
    }
}

void UIHomePage::UpdateNavigationButton()
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CDKFileManager* pFileManager = CDKFileManager::GetFileManager();
    if(!pFileManager)
    {
        DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s GetFileManager is NULL", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return;
    }
    
    CHAR str[50] = {0};
    
    int iCurPage = 0, iTotalPage = 0;
    iCurPage   = m_lstDir.GetCurPageIndex();
    iTotalPage = m_lstDir.GetTotalPageCount();
    int itemNum = m_lstDir.GetItemNum();
    
    if(itemNum)
        sprintf(str, "%d/%d %s", iCurPage + 1, iTotalPage,StringManager::GetPCSTRById(BOOK_PAGE));
    else
        sprintf(str, "%s", StringManager::GetPCSTRById(BOOK_00_PAGE));
    m_txtPageNo.SetText(str);
    
    sprintf(str, "%s %d %s", StringManager::GetPCSTRById(BOOK_TOTAL),itemNum,StringManager::GetPCSTRById(BOOK_BEN));
    m_txtTotalBook.SetText(CString(str));
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

std::string UIHomePage::GetDirectoryViewTitle(const std::string& path) const
{
    if (m_lstDir.HasCategory())
    {
        return m_lstDir.GetCategory();
    }
    return PathManager::GetDirectoryDisplayName(path.c_str());
}

void UIHomePage::UpdateTopBox()
{
    string strPath = m_lstDir.GetCurrentPath(); 
    CDKFileManager* pFileManager = CDKFileManager::GetFileManager();
    DK_FileSorts bookSort = pFileManager->GetBookSort();
    bool showNewCategory = false;
    bool showAddBook = false;
    if (m_pageStyle != SEARCHPAGE && 
            (DIRECTORY == bookSort && 
             PathManager::IsBookStorePath(strPath.c_str())))
    {
        if (m_lstDir.HasCategory())
        {
            showAddBook = true;
        }
        else
        {
            showNewCategory = true;
        }
    }
    bool showUpLevelSizer = false;
    if (((DIRECTORY == bookSort) && (!PathManager::IsRootPath(strPath) && !strPath.empty()))
            || SEARCHPAGE == m_pageStyle)
    {
        m_topButtonSizer->Show(false);
        m_topUpLevelSizer->Show(true);
        if (SEARCHPAGE == m_pageStyle)
        {
            m_btnUpperFolder.ShowSeperator(false);
            m_titleSizer->Show(false);
            m_searchBox.SetVisible(true);
        }
        else
        {
            m_btnUpperFolder.ShowSeperator(true);
            m_titleSizer->Show(true);
            m_searchBox.SetVisible(false);
        }
        showUpLevelSizer = true;

        m_txtUpperFolder.SetText(GetDirectoryViewTitle(strPath).c_str());
    }
    else
    {
        m_topButtonSizer->Show(true);
        m_topUpLevelSizer->Show(false);
    }
    
    m_btnNewCategory.SetVisible(showNewCategory || showAddBook);
    if (showNewCategory)
    {
        m_btnNewCategory.SetText(StringManager::GetPCSTRById(CATEGORY_NEW));
    }
    else if (showAddBook)
    {
        m_btnNewCategory.SetText(StringManager::GetPCSTRById(CATEGORY_ADD_FILE));
    }
    if (showUpLevelSizer)
    {
        int leftWidth = m_btnUpperFolder.GetMinWidth();
        int rightWidth = GetWindowMetrics(UIHorizonMarginIndex);
        if (m_btnNewCategory.IsVisible())
        {
            rightWidth += m_btnNewCategory.GetMinWidth();
        }
        if (leftWidth > rightWidth)
        {
            m_titleLeftSizer->SetMinWidth(0);
            m_titleRightSizer->SetMinWidth(leftWidth - rightWidth);
        }
        else
        {
            m_titleLeftSizer->SetMinWidth(rightWidth - leftWidth);
            m_titleRightSizer->SetMinWidth(0);
        }
    }
}

HRESULT UIHomePage::UpdateBookList()
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CDKFileManager* pFileManager = CDKFileManager::GetFileManager();
    if(!pFileManager)
    {
        DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s GetFileManager is NULL", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return S_FALSE;
    }

    if (SEARCHPAGE == m_pageStyle)
    {
        Layout();
        m_lstDir.SetCurrentPath("");
        m_lstDir.InitListItem();
        m_lstDir.UpdateListItem();
        m_btnSort.SetText(GetSortString());
    }
    else if (m_FileSorts != pFileManager->GetBookSort())
    {
        m_FileSorts = pFileManager->GetBookSort();
        DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s BookSort is changed", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        Layout();
        if (m_FileSorts != DIRECTORY)
        {
            m_lstDir.SetCurrentPath("");
        }
        else
        {
            m_lstDir.SetCurrentPath(PathManager::GetRootPath());
        }
        m_lstDir.SetCurPageIndex(0);
        m_lstDir.InitListItem();
        m_lstDir.UpdateListItem();
        m_btnSort.SetText(GetSortString());
    }
    else
    {
        m_lstDir.InitListItem();
        m_lstDir.UpdateListItem();
    }
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    m_txtHint.SetVisible(false);
    if (m_lstDir.GetItemNum() == 0)
    {
        if (SEARCHPAGE == m_pageStyle)
        {
            m_txtHint.SetText(StringManager::GetPCSTRById(NO_BOOK_SEARCH_RESULT));
            m_txtHint.SetVisible(true);
        }
        else if (m_lstDir.HasCategory())
        {
            m_txtHint.SetText(StringManager::GetPCSTRById(CATEGORY_ADD_FILE_HINT));
            m_txtHint.SetVisible(true);
        }
    }

    return S_OK;
}

void UIHomePage::OnLoad()
{
    SetVisible(TRUE);
    UIPage::OnLoad();
    CDKFileManager *pFileManager = CDKFileManager::GetFileManager();  
    if(NULL == pFileManager)
    {
        return;
    }
    if(m_pageStyle == SEARCHPAGE)
    {
        pFileManager->SearchBook(m_searchKeyword.c_str());
    }

    Layout();
    return;
}

void UIHomePage::OnUnLoad()
{
    UIPage::OnUnLoad();
    SetVisible(FALSE);
}


void UIHomePage::SetSearchKeyword(const std::string& keyword)
{
    m_searchKeyword = keyword;

}

void UIHomePage::OnEnter()
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UIPage::OnEnter();
    CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
    if (m_pBottomBar)
    {
        m_pBottomBar->SetFocusedIndex(PAT_BookShelf);
    }
    if(NULL == pFileManager)
    {
        return ;
    }
    if(m_pageStyle == SEARCHPAGE)
    {
        pFileManager->SearchBook(m_searchKeyword.c_str());
    }
    else
    {
        if(pFileManager->GetBookSort() != m_FileSorts || RecentlyReadTime == pFileManager->GetBookSort())
        {
            pFileManager->SortFile(DFC_Book);
            m_lstDir.UpdateListItem();
        }
    }
    RefreshUI();
    return;
}

void UIHomePage::OnLeave()
{
	CDKFileManager *fileManager = CDKFileManager::GetFileManager();
	if(m_pageStyle == SEARCHPAGE)
    {
		fileManager->SetBookSortType(fileManager->GetBookSort());
        fileManager->SortFile(DFC_Book);
	}
    UIPage::OnLeave();
}


void UIHomePage::Show(BOOL bIsShow, BOOL bIsRepaint)
{
    UIWindow::Show(bIsShow, bIsRepaint);
}

void UIHomePage::RefreshUI()
{    
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UpdateTopBox();
    UpdateBookList();
    UpdateNavigationButton();
    Layout();
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

HRESULT UIHomePage::Draw(DK_IMAGE drawingImg)
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!m_bIsVisible)
    {
        DebugPrintf(DLC_UIHOMEPAGE," UIPage::Draw(DK_IMAGE drawingImg) !m_bIsVisible");
        return S_FALSE;
    }

    if (drawingImg.pbData == NULL)// || !m_IsChanged)
    {
        return E_FAIL;
    }
     
    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(DrawBackGround(drawingImg));
    
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        drawingImg,
        rcSelf,
        &imgSelf
        ));
    CTpGraphics grf(imgSelf);
    
    //RefreshUI();
    
    UINT32 iSize = GetChildrenCount();
    for (UINT32 i = 0; i < iSize; i++)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (pWin && pWin->IsVisible())
        {
            DebugPrintf(DLC_GUI_VERBOSE, "%s: Drawing Child %d / %d : %s ..., (%d,%d), (%d,%d), text: %s", GetClassName(), i, iSize, pWin->GetClassName(),
                    pWin->GetX(), pWin->GetY(), pWin->GetWidth(), pWin->GetHeight(), pWin->GetText());
            pWin->Draw(imgSelf);
        }
        else
        {
            DebugPrintf(DLC_GUI_VERBOSE, "%s: FAILED to get child %d/%d !!!", GetClassName(), i, iSize);
        }
    }

    return DrawFocus(imgSelf);
}


bool UIHomePage::OnListItemClick(const EventArgs& args)
{
    ListItemClickEvent listItemClickArgs = (const ListItemClickEvent&)(args);
    if (listItemClickArgs.IsDirectoryItem())
    {
        if (m_bookListMode == BLM_ICON)
        {
            CDisplay::GetDisplay()->SetFullRepaint(true);
        }
        RefreshUI();
        return true;
    }
    return false;
}

bool UIHomePage::OnListContentChanged(const EventArgs& args)
{
    UpdateNavigationButton();
    return true;
}

bool UIHomePage::OnFileListChanged(const EventArgs& args)
{
    RefreshUI();
    Repaint();
    return true;
}

bool UIHomePage::OnNewCategoryClick()
{
    UIAddCategoryDlg dlg(this, UIAddCategoryDlg::CAT_ADD);
    if (IDOK == dlg.DoModal())
    {
        std::string category = dlg.GetInputText();
        if (!category.empty())
        {
            std::string errorMessage;
            if (!LocalCategoryManager::AddCategory(
                        category.c_str(), &errorMessage))
            {
                UIMessageBox msgBox(this,
                        errorMessage.c_str(),
                        MB_OK);
                msgBox.DoModal();
            }
            else
            {
                UIMessageBox dlg(this,
                        StringManager::GetPCSTRById(CATEGORY_HINT_FOR_NEW_CATEGORY),
                        MB_OK | MB_CANCEL);
                if (dlg.DoModal() == MB_OK)
                {
                    UIAddBookToCategoryPage* addBookPage(
                            new UIAddBookToCategoryPage(category.c_str()));
                    addBookPage->MoveWindow(0, 0, 
                            DeviceInfo::GetDisplayScreenWidth(),
                            DeviceInfo::GetDisplayScreenHeight());
                    addBookPage->Layout();
                    CPageNavigator::Goto(addBookPage);
                }
                RefreshUI();
            }
        }
    }

    return true;
}

bool UIHomePage::OnViewModeClick()
{
    BookListMode newMode = BLM_ICON;
    if (BLM_ICON == m_bookListMode)
    {
        newMode = BLM_LIST;
    }
    m_bookListMode = newMode;
    UpdateViewModeImage();
    SystemSettingInfo::GetInstance()->SetBookListMode(m_bookListMode);
    m_lstDir.SetBookListMode(m_bookListMode);
    //m_lstDir.InitListItem();
    //m_lstDir.UpdateListItem();
    RefreshUI();
    UpdateWindow();
    return true;
}

bool UIHomePage::OnAddBookToCategoryClick()
{
    UIAddBookToCategoryPage* addBookPage(new UIAddBookToCategoryPage(m_lstDir.GetCategory().c_str()));
    addBookPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
    addBookPage->Layout();
    CPageNavigator::Goto(addBookPage);
    return true;
}

bool UIHomePage::OnChildClick(UIWindow* child)
{
    if (child == &m_btnSort)
    {
        return OnSortClick();
    }
    else if (child == &m_btnSearch)
    {
        return OnSearchClick();
    }
    else if (child == &m_btnViewMode)
    {
         return OnViewModeClick();
    }
    else if (child == &m_btnNewCategory)
    {
        if (m_lstDir.HasCategory())
        {
            return OnAddBookToCategoryClick();
        }
        else
        {
            return OnNewCategoryClick();
        }
    }
    else if (child == &m_btnUpperFolder)
    {
        return OnUpperFolderClick();
    }
    else
    {
        return UIPage::OnChildClick(child);
    }
}

void UIHomePage::UpdateViewModeImage()
{
    if (BLM_LIST == m_bookListMode)
    {
        m_btnViewMode.SetIcon(ImageManager::GetImage(IMAGE_ICON_BOOKLIST_COVER), UIButton::ICON_CENTER);
    }
    else
    {
        m_btnViewMode.SetIcon(ImageManager::GetImage(IMAGE_ICON_BOOKLIST_LIST), UIButton::ICON_CENTER);
    }
    m_btnViewMode.UpdateWindow();
}
